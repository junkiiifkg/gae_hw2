#include "Menu.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include "AI.hpp"
#include <random>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <cctype>
#include <limits>

using namespace std;
using json = nlohmann::json;
using namespace menu;

// normalize JSON category keys to internal names
static string normalizeCategory(const string &cat) {
    string low = cat;
    for (auto &c : low) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    if (low == "starters" || low == "starter") return "Starter";
    if (low == "salads" || low == "salad") return "Salad";
    if (low == "main_courses" || low == "main_course" || low == "maincourse" || low == "maincourses") return "MainCourse";
    if (low == "drinks" || low == "drink") return "Drink";
    if (low == "appetizers" || low == "appetizer") return "Appetizer";
    if (low == "desserts" || low == "dessert") return "Dessert";
    if (!low.empty()) { low[0] = static_cast<char>(toupper(static_cast<unsigned char>(low[0]))); return low; }
    return cat;
}

// parse taste from various JSON forms
static vector<double> parseTasteFromJson(const json &it) {
    vector<double> t(5, 0.5); // order: sweet, salty, sour, bitter, spicy
    if (it.contains("taste")) {
        if (it["taste"].is_array()) {
            size_t idx = 0;
            for (auto &v : it["taste"]) if (idx < 5) t[idx++] = v.get<double>();
            return t;
        }
        if (it["taste"].is_object()) {
            auto &tb = it["taste"];
            auto getv = [&](const string &k)->double {
                if (tb.contains(k) && tb[k].is_number()) return tb[k].get<double>();
                return 0.5;
            };
            t[0] = getv("sweet");
            t[1] = getv("salty");
            t[2] = getv("sour");
            t[3] = getv("bitter");
            t[4] = tb.contains("spicy") ? getv("spicy") : getv("savory");
            return t;
        }
    }

    if (it.contains("taste_balance")) {
        if (it["taste_balance"].is_number()) {
            double tb = it["taste_balance"].get<double>();
            return vector<double>(5, tb);
        }
        if (it["taste_balance"].is_object()) {
            auto &tb = it["taste_balance"];
            auto getv = [&](const string &k)->double {
                if (tb.contains(k) && tb[k].is_number()) return tb[k].get<double>();
                return 0.5;
            };
            t[0] = getv("sweet");
            t[1] = getv("salty");
            t[2] = getv("sour");
            t[3] = getv("bitter");
            t[4] = tb.contains("spicy") ? getv("spicy") : getv("savory");
            return t;
        }
    }

    return t;
}

static map<string, vector<json>> buildCatalog(const json &menuData) {
    map<string, vector<json>> catalog;
    if (menuData.is_null()) return catalog;
    for (auto& [category, items] : menuData.items()) {
        string cat = normalizeCategory(category);
        for (auto& it : items) catalog[cat].push_back(it);
    }

    // ensure each main category has at least 2 items
    vector<string> required = {"Starter","Salad","MainCourse","Drink","Appetizer","Dessert"};
    for (auto &cat : required) {
        auto &vec = catalog[cat]; // creates empty vector if not present
        if (vec.size() == 0) {
            json ph;
            ph["name"] = string("Placeholder ") + cat;
            ph["price"] = 0.0;
            ph["taste_balance"] = json::object({ {"sweet",0.5}, {"salty",0.5}, {"sour",0.5}, {"bitter",0.5}, {"savory",0.5} });
            vec.push_back(ph);
            vec.push_back(ph);
        } else if (vec.size() == 1) {
            // duplicate existing to reach 2
            vec.push_back(vec.front());
        }
    }

    return catalog;
}

static shared_ptr<MenuItem> makeItemFromJson(const string &category, const json &it) {
    string n = it.value("name", string());
    double p = it.value("price", 0.0);
    vector<double> t = parseTasteFromJson(it);

    // vegetarian flag
    bool isVeg = false;
    if (it.contains("vegetarian") && it["vegetarian"].is_boolean()) {
        isVeg = it["vegetarian"].get<bool>();
    } else {
        string low = n;
        for (auto &c : low) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
        if (low.find("veg") != string::npos || low.find("vegetable") != string::npos) isVeg = true;
    }

    // category uses normalized names
    if (category == "Starter") return make_shared<Starter>(n,p,t);
    if (category == "Salad") return make_shared<Salad>(n,p,t);
    if (category == "MainCourse") return make_shared<MainCourse>(n,p,t,isVeg);
    if (category == "Drink") return make_shared<Drink>(n,p,t);
    if (category == "Appetizer") return make_shared<Appetizer>(n,p,t);
    if (category == "Dessert") return make_shared<Dessert>(n,p,t);
    // fallback
    return make_shared<Starter>(n,p,t);
}

static vector<double> tasteVectorFromMenu(const vector<shared_ptr<MenuItem>> &menu) {
    vector<double> avg(5,0.0);
    if (menu.empty()) return vector<double>(5,0.5);
    for (auto &it : menu) {
        auto t = it->getTaste();
        for (size_t i=0;i<5;++i) avg[i] += t[i];
    }
    for (size_t i=0;i<5;++i) avg[i] /= menu.size();
    return avg;
}

static vector<double> tasteVectorFromMenu(const Menu &m) {
    return m.getTasteAvg();
}

static double euclidean(const vector<double> &a, const vector<double> &b) {
    double s=0;
    size_t n = min(a.size(), b.size());
    for (size_t i=0;i<n;++i) {
        double d = a[i]-b[i];
        s += d*d;
    }
    return sqrt(s);
}

// generate many random candidate full-menus and pick the one with highest predicted satisfaction
static vector<shared_ptr<MenuItem>> suggestRandomMenuBest(const map<string, vector<json>> &catalog, ai::LinearRegression &model, bool preferVeg=false, int samples=30) {
    vector<shared_ptr<MenuItem>> bestMenu;
    double bestScore = std::numeric_limits<double>::lowest();
    random_device rd; mt19937 gen(rd());
    for (int s=0;s<samples;++s) {
        vector<shared_ptr<MenuItem>> cand;
        for (auto &kv : catalog) {
            const auto &vec = kv.second;
            if (vec.empty()) continue;

            vector<size_t> candidates;
            for (size_t i=0;i<vec.size();++i) {
                const json &entry = vec[i];
                if (kv.first == "MainCourse" && preferVeg) {
                    bool isVeg = false;
                    if (entry.contains("vegetarian") && entry["vegetarian"].is_boolean()) isVeg = entry["vegetarian"].get<bool>();
                    else {
                        string nm = entry.value("name", string());
                        string low = nm;
                        for (auto &c : low) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
                        if (low.find("veg") != string::npos || low.find("vegetable") != string::npos) isVeg = true;
                    }
                    if (!isVeg) continue;
                }
                candidates.push_back(i);
            }

            if (candidates.empty()) {
                for (size_t i=0;i<vec.size();++i) candidates.push_back(i);
            }

            uniform_int_distribution<size_t> dist(0, candidates.size()-1);
            size_t chosen = candidates[dist(gen)];
            cand.push_back(makeItemFromJson(kv.first, vec[chosen]));
        }
        if (cand.empty()) continue;
        auto taste = tasteVectorFromMenu(cand);
        double score = model.predict(taste);
        if (score > bestScore) { bestScore = score; bestMenu = cand; }
    }
    return bestMenu;
}

static vector<shared_ptr<MenuItem>> suggestByTasteProfile(const map<string, vector<json>> &catalog, const vector<double> &profile, ai::LinearRegression &model, bool preferVeg=false) {
    vector<shared_ptr<MenuItem>> menu;
    for (auto &kv : catalog) {
        const auto &vec = kv.second;
        if (vec.empty()) continue;
        double bestDist = 1e18;
        int bestIdx = -1;
        for (size_t i = 0; i < vec.size(); ++i) {
            const json &candidate = vec[i];
            if (kv.first == "MainCourse" && preferVeg) {
                bool isVeg = false;
                if (candidate.contains("vegetarian") && candidate["vegetarian"].is_boolean()) isVeg = candidate["vegetarian"].get<bool>();
                else {
                    string nm = candidate.value("name", string());
                    string low = nm;
                    for (auto &c : low) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
                    if (low.find("veg") != string::npos || low.find("vegetable") != string::npos) isVeg = true;
                }
                if (!isVeg) continue; // skip non-veg
            }
            vector<double> t = parseTasteFromJson(candidate);
            double d = euclidean(t, profile);
            if (d < bestDist) { bestDist = d; bestIdx = static_cast<int>(i); }
        }
        if (bestIdx >= 0) menu.push_back(makeItemFromJson(kv.first, vec[bestIdx]));
        else {
            auto fallback = min_element(vec.begin(), vec.end(), [&](const json &a, const json &b){
                vector<double> ta = parseTasteFromJson(a), tb = parseTasteFromJson(b);
                return euclidean(ta, profile) < euclidean(tb, profile);
            });
            if (fallback != vec.end()) menu.push_back(makeItemFromJson(kv.first, *fallback));
        }
    }
    return menu;
}

static void showSuggestedMenu(const vector<shared_ptr<MenuItem>> &m) {
    cout << "\n-- Suggested Menu --\n";
    double total = 0;
    for (auto &it : m) { it->printInfo(); total += it->getPrice(); }
    cout << "Total Cost: $" << fixed << setprecision(2) << total << "\n";
}

int main() {
    cout << "==============================\n";
    cout << "  Welcome to Restaurant Bot 🍽️\n";
    cout << "==============================\n\n";

    string fname, lname, gender;
    cout << "Enter your first name: ";
    cin >> fname;
    cout << "Enter your last name: ";
    cin >> lname;
    cout << "Enter your gender (M/F): ";
    cin >> gender;

    User user(fname, lname, gender);
    user.showInfo();

    json menuData;
    ifstream file("menu.json");
    if (file.is_open()) { file >> menuData; cout << "\nMenu loaded from menu.json successfully!\n"; }
    else cout << "\n⚠️ Could not open menu.json. Default items will be used.\n";

    auto catalog = buildCatalog(menuData);

    cout << "\nDo you want a menu suggestion? (1=Random+AI, 2=By taste profile, 0=Skip): ";
    int suggestChoice; cin >> suggestChoice;

    cout << "Prefer vegetarian main course? (1=yes, 0=no): ";
    int pv; cin >> pv;
    bool preferVeg = (pv == 1);

    ai::LinearRegression model(0.01);
    model.loadWeights("weights.json");

    if (suggestChoice == 1) {
        auto sug = suggestRandomMenuBest(catalog, model, preferVeg, 40);
        if (sug.empty()) cout << "No items available for suggestion.\n";
        else {
            showSuggestedMenu(sug);
            cout << "Enter your satisfaction for this suggestion (0-1, or -1 to skip): ";
            double satisfaction; cin >> satisfaction;
            if (satisfaction >= 0.0 && satisfaction <= 1.0) {
                auto taste = tasteVectorFromMenu(sug);
                model.train(taste, satisfaction);
                model.saveWeights("weights.json");
                cout << "Model updated.\n";
            }
        }
    } else if (suggestChoice == 2) {
        cout << "Enter your taste balance (sweet sour bitter salty savory) as 5 numbers: ";
        vector<double> taste(5);
        for (double &v : taste) cin >> v;
        auto sug = suggestByTasteProfile(catalog, taste, model, preferVeg);
        if (sug.empty()) cout << "No items available for suggestion.\n";
        else {
            double score = model.predict(tasteVectorFromMenu(sug));
            cout << "Predicted satisfaction for this suggested menu: " << score << "\n";
            showSuggestedMenu(sug);
            cout << "Your satisfaction score (0–1): ";
            double rating; cin >> rating;
            if (rating >= 0.0 && rating <= 1.0) {
                model.train(taste, rating);
                model.saveWeights("weights.json");
                cout << "Weights updated and saved!\n";
            }
        }
    }

    // pass catalog into interact so user can pick existing items per category
    user.interact(catalog);

    cout << "\nLet's evaluate your menu experience! (0–1 satisfaction)\n";
    vector<double> taste(5);
    cout << "Enter your taste balance (sweet salty sour bitter spicy): ";
    for (double &v : taste) cin >> v;

    double rating;
    cout << "Your satisfaction score (0-1): ";
    cin >> rating;

    double pred = model.predict(taste);
    cout << "\nPredicted satisfaction: " << pred << endl;
    cout << "Actual satisfaction: " << rating << endl;

    model.train(taste, rating);
    model.saveWeights("weights.json");

    cout << "Weights updated and saved!\n";
    model.printWeights();

    cout << "\nThank you for using Restaurant Bot! 🍷\n";
    return 0;
}