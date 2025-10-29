#include "Menu.hpp"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

namespace menu {

// ========== BASE ==========

MenuItem::MenuItem(const std::string &n, double p, const std::vector<double> &t)
    : name(n), price(p), taste(t) {
    if (taste.size() != 5) taste = std::vector<double>(5, 0.5);
}

string MenuItem::getName() const { return name; }
double MenuItem::getPrice() const { return price; }
std::vector<double> MenuItem::getTaste() const { return taste; }
double MenuItem::getTasteAvg() const {
    if (taste.empty()) return 0.5;
    double s = accumulate(taste.begin(), taste.end(), 0.0);
    return s / taste.size();
}

void MenuItem::setName(const string &n) { name = n; }
void MenuItem::setPrice(double p) { price = p; }
void MenuItem::setTaste(const std::vector<double> &t) { taste = (t.size()==5? t : std::vector<double>(5,0.5)); }

// ========== Starter ==========
Starter::Starter(const std::string &n, double p, const std::vector<double> &t, bool hot)
    : MenuItem(n,p,t), isHot(hot) {}
void Starter::printInfo() const {
    cout << "[Starter] " << name << " - $" << price << " - taste(avg:" << getTasteAvg() << ") - " << (isHot ? "Hot" : "Cold") << "\n";
}
void Starter::customize() {
    cout << "Starter - hot? (1=yes,0=no): ";
    int v; if (!(cin >> v)) { cin.clear(); cin.ignore(10000,'\n'); return; }
    isHot = (v==1);
}

// ========== Salad ==========
Salad::Salad(const std::string &n, double p, const std::vector<double> &t, bool topping)
    : MenuItem(n,p,t), hasTopping(topping) {}
void Salad::printInfo() const {
    cout << "[Salad] " << name << " - $" << price << (hasTopping ? " +topping" : "") << " - taste(avg:" << getTasteAvg() << ")\n";
}
void Salad::customize() {
    cout << "Add topping +$2.25? (1=yes,0=no): ";
    int v; if (!(cin >> v)) { cin.clear(); cin.ignore(10000,'\n'); return; }
    if (v==1 && !hasTopping) { hasTopping=true; price += 2.25; }
}

// ========== MainCourse ==========
MainCourse::MainCourse(const std::string &n, double p, const std::vector<double> &t, bool veg)
    : MenuItem(n,p,t), isVegetarian(veg) {}
void MainCourse::printInfo() const {
    cout << "[Main] " << name << " - $" << price << " - " << (isVegetarian ? "Vegetarian" : "Non-veg") << " - taste(avg:" << getTasteAvg() << ")\n";
}
void MainCourse::customize() {
    cout << "Vegetarian? (1=yes,0=no): ";
    int v; if (!(cin >> v)) { cin.clear(); cin.ignore(10000,'\n'); return; }
    isVegetarian = (v==1);
}

// ========== Drink ==========
Drink::Drink(const std::string &n, double p, const std::vector<double> &t, bool carb, bool shot)
    : MenuItem(n,p,t), carbonated(carb), extraShot(shot) {}
void Drink::printInfo() const {
    cout << "[Drink] " << name << " - $" << price << (carbonated ? " +carbonation" : "") << (extraShot ? " +shot" : "") << " - taste(avg:" << getTasteAvg() << ")\n";
}
void Drink::customize() {
    cout << "Carbonated +$0.5? (1=yes,0=no): ";
    int v; if (!(cin >> v)) { cin.clear(); cin.ignore(10000,'\n'); return; }
    if (v==1 && !carbonated) { carbonated=true; price += 0.5; }
    cout << "Extra alcohol shot +$2.5? (1=yes,0=no): ";
    if (!(cin >> v)) { cin.clear(); cin.ignore(10000,'\n'); return; }
    if (v==1 && !extraShot) { extraShot=true; price += 2.5; }
}

// ========== Appetizer ==========
Appetizer::Appetizer(const std::string &n, double p, const std::vector<double> &t, const std::string &serve)
    : MenuItem(n,p,t), serveTime(serve) {}
void Appetizer::printInfo() const {
    cout << "[Appetizer] " << name << " - $" << price << " - serve: " << serveTime << " - taste(avg:" << getTasteAvg() << ")\n";
}
void Appetizer::customize() {
    cout << "Serve before main? (1=before,0=after): ";
    int v; if (!(cin >> v)) { cin.clear(); cin.ignore(10000,'\n'); return; }
    serveTime = (v==1 ? "before" : "after");
}

// ========== Dessert ==========
Dessert::Dessert(const std::string &n, double p, const std::vector<double> &t, bool choc)
    : MenuItem(n,p,t), extraChocolate(choc) {}
void Dessert::printInfo() const {
    cout << "[Dessert] " << name << " - $" << price << (extraChocolate ? " +choc" : "") << " - taste(avg:" << getTasteAvg() << ")\n";
}
void Dessert::customize() {
    cout << "Add extra chocolate +$1.5? (1=yes,0=no): ";
    int v; if (!(cin >> v)) { cin.clear(); cin.ignore(10000,'\n'); return; }
    if (v==1 && !extraChocolate) { extraChocolate=true; price += 1.5; }
}

// ========== MENU ==========
Menu::Menu() : totalCost(0.0), tasteAvg(5,0.5) {}

void Menu::addItem(shared_ptr<MenuItem> item) {
    if (!item) return;
    items.push_back(item);
    totalCost += item->getPrice();
    // recompute tasteAvg
    vector<double> sum(5,0.0);
    for (auto &it : items) {
        auto t = it->getTaste();
        for (size_t i=0;i<5;++i) sum[i] += t[i];
    }
    for (size_t i=0;i<5;++i) tasteAvg[i] = sum[i] / items.size();
}

void Menu::removeItem(const string &name) {
    auto it = find_if(items.begin(), items.end(), [&](auto &i){ return i->getName() == name; });
    if (it != items.end()) {
        totalCost -= (*it)->getPrice();
        items.erase(it);
        // recompute tasteAvg
        if (items.empty()) tasteAvg = vector<double>(5,0.5);
        else {
            vector<double> sum(5,0.0);
            for (auto &it2 : items) { auto t = it2->getTaste(); for (size_t k=0;k<5;++k) sum[k]+=t[k]; }
            for (size_t k=0;k<5;++k) tasteAvg[k] = sum[k] / items.size();
        }
    } else cout << "Item to remove not found: " << name << "\n";
}

void Menu::updateItem(const string &name) {
    auto it = find_if(items.begin(), items.end(), [&](auto &i){ return i->getName() == name; });
    if (it != items.end()) {
        (*it)->customize();
        // recompute cost/taste
        totalCost = 0;
        vector<double> sum(5,0.0);
        for (auto &it2 : items) { totalCost += it2->getPrice(); auto t = it2->getTaste(); for (size_t k=0;k<5;++k) sum[k]+=t[k]; }
        for (size_t k=0;k<5;++k) tasteAvg[k] = items.empty()?0.5: sum[k] / items.size();
    } else cout << "Item to update not found: " << name << "\n";
}

void Menu::showMenu() const {
    if (items.empty()) {
        cout << "-- Your menu is empty --\n";
        return;
    }
    cout << "-- Your Menu --\n";
    for (size_t i = 0; i < items.size(); ++i) {
        cout << i + 1 << ". ";
        items[i]->printInfo();
    }
    cout << "Total cost: $" << totalCost << " | Taste avg: [";
    for (size_t i = 0; i < tasteAvg.size(); ++i) {
        cout << tasteAvg[i];
        if (i + 1 < tasteAvg.size()) cout << ", ";
    }
    cout << "]\n";
}

double Menu::getTotalCost() const { return totalCost; }
std::vector<double> Menu::getTasteAvg() const { return tasteAvg; }

// ========== USER ==========
User::User(const std::string &f, const std::string &l, const std::string &g)
    : firstName(f), lastName(l), gender(g), userMenu() {}

void User::showInfo() const {
    string title = "Mx.";
    if (!gender.empty()) {
        if (gender=="M"||gender=="m") title = "Mr.";
        else if (gender=="F"||gender=="f") title = "Mrs.";
    }
    cout << "Welcome " << title << " " << firstName << " " << lastName << "!\n";
}

Menu &User::getMenu() { return userMenu; }

// helper: normalizeCategory (keeps Menu.cpp independent)
static std::string normalizeCategory(const std::string &cat) {
    std::string low = cat;
    for (auto &c : low) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    if (low == "starters" || low == "starter") return "Starter";
    if (low == "salads" || low == "salad") return "Salad";
    if (low == "main_courses" || low == "main_course" || low == "maincourse" || low == "maincourses") return "MainCourse";
    if (low == "drinks" || low == "drink") return "Drink";
    if (low == "appetizers" || low == "appetizer") return "Appetizer";
    if (low == "desserts" || low == "dessert") return "Dessert";
    if (!low.empty()) { low[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(low[0]))); return low; }
    return cat;
}

// helper: parse taste helper (handles object with named keys, arrays, or taste_balance)
static std::vector<double> parseTasteFromJson(const json &it) {
    std::vector<double> t(5, 0.5); // order: sweet, salty, sour, bitter, spicy/savory
    if (it.contains("taste")) {
        if (it["taste"].is_array()) {
            size_t idx = 0;
            for (auto &v : it["taste"]) if (idx < 5) t[idx++] = v.get<double>();
            return t;
        }
        if (it["taste"].is_object()) {
            auto &tb = it["taste"];
            auto getv = [&](const std::string &k)->double {
                // use value(key, default) to avoid template parsing issues and simplify
                if (tb.contains(k) && tb[k].is_number()) return tb.value(k, 0.5);
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
            return std::vector<double>(5, tb);
        }
        if (it["taste_balance"].is_object()) {
            auto &tb = it["taste_balance"];
            auto getv = [&](const std::string &k)->double {
                if (tb.contains(k) && tb[k].is_number()) return tb.value(k, 0.5);
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


// updated interact: accept catalog and list available items for chosen category
void User::interact(const std::map<std::string, std::vector<json>> &catalog) {
    while(true) {
        cout << "\nOptions: 1=show 2=add 3=remove 4=update 0=exit\nChoice: ";
        int c; if (!(cin>>c)) { cin.clear(); cin.ignore(10000,'\n'); continue; }
        if (c==0) break;
        if (c==1) userMenu.showMenu();
        else if (c==2) {
            cout << "Category (Starter/Salad/MainCourse/Drink/Appetizer/Dessert): ";
            string cat; cin >> cat;
            cat = normalizeCategory(cat); // normalize user input so comparisons work
            cin.ignore();

            string name;
            double price = 0.0;
            vector<double> t(5,0.5);

            // If catalog has entries for this category, list them with prices
            auto it = catalog.find(cat);
            if (it != catalog.end() && !it->second.empty()) {
                cout << "\nAvailable items in " << cat << ":\n";
                for (size_t i = 0; i < it->second.size(); ++i) {
                    string nm = it->second[i].value("name", string());
                    double pr = it->second[i].value("price", 0.0);
                    cout << " " << (i+1) << ") " << nm << " - $" << pr << "\n";
                }
                cout << "Enter number to prefill that item, or 0 to enter new: ";
                int sel; if (!(cin >> sel)) { cin.clear(); cin.ignore(10000,'\n'); sel = 0; }
                cin.ignore();
                if (sel > 0 && static_cast<size_t>(sel) <= it->second.size()) {
                    const json &selj = it->second[sel-1];
                    name = selj.value("name", string());
                    price = selj.value("price", 0.0);
                    // parse taste in all supported shapes (object with keys, array, taste_balance)
                    t = parseTasteFromJson(selj);
                } else {
                    // manual entry
                    cout << "Name: "; getline(cin, name);
                    cout << "Price: "; cin >> price;
                    cout << "Enter 5 taste numbers (sweet sour bitter salty savory): ";
                    for (auto &v: t) cin >> v;
                    cin.ignore();
                }
            } else {
                // no catalog entry -> manual entry
                cout << "Name: "; getline(cin, name);
                cout << "Price: "; cin >> price;
                cout << "Enter 5 taste numbers (sweet sour bitter salty savory): ";
                for (auto &v: t) cin >> v;
                cin.ignore();
            }

            shared_ptr<MenuItem> ititem;
            if (cat=="Starter") ititem = make_shared<Starter>(name, price, t);
            else if (cat=="Salad") ititem = make_shared<Salad>(name, price, t);
            else if (cat=="MainCourse") {
                // if catalog provided a matching item, try to read vegetarian flag; otherwise ask user
                bool isVeg = false;
                auto cit = catalog.find(cat);
                if (cit != catalog.end()) {
                    auto found = find_if(cit->second.begin(), cit->second.end(), [&](const json &j){ return j.value("name",string())==name; });
                    if (found != cit->second.end() && found->contains("vegetarian") && (*found)["vegetarian"].is_boolean())
                        isVeg = (*found)["vegetarian"].get<bool>();
                    else {
                        cout << "Vegetarian? (1=yes,0=no): ";
                        int vv; if (!(cin>>vv)) { cin.clear(); cin.ignore(10000,'\n'); vv=0; }
                        isVeg = (vv==1);
                        cin.ignore();
                    }
                } else {
                    cout << "Vegetarian? (1=yes,0=no): ";
                    int vv; if (!(cin>>vv)) { cin.clear(); cin.ignore(10000,'\n'); vv=0; }
                    isVeg = (vv==1);
                    cin.ignore();
                }
                ititem = make_shared<MainCourse>(name, price, t, isVeg);
            }
            else if (cat=="Drink") ititem = make_shared<Drink>(name, price, t);
            else if (cat=="Appetizer") ititem = make_shared<Appetizer>(name, price, t);
            else if (cat=="Dessert") ititem = make_shared<Dessert>(name, price, t);
            else { cout << "Unknown category\n"; continue; }

            if (ititem) {
                ititem->customize();
                userMenu.addItem(ititem);
                cout << "Added.\n";
            } else {
                cout << "Failed to create item.\n";
            }
        }
        else if (c==3) {
            cout << "Name to remove: "; cin.ignore(); string n; getline(cin,n);
            userMenu.removeItem(n);
        }
        else if (c==4) {
            cout << "Name to update: "; cin.ignore(); string n; getline(cin,n);
            userMenu.updateItem(n);
        }
    }
}

} // namespace menu