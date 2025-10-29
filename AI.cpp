#include "AI.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;
using namespace std;

namespace ai {

LinearRegression::LinearRegression(double lr) : alpha(lr) {
    weights.resize(6, 0.1); // bias + 5 taste weights
}

double LinearRegression::predict(const vector<double> &x) const {
    double y_hat = 0.0;
    if (weights.empty()) return y_hat;
    y_hat = weights[0]; // bias
    size_t n = min<size_t>(5, x.size());
    for (size_t i = 0; i < n; ++i) {
        y_hat += weights[i + 1] * x[i];
    }
    return y_hat;
}

void LinearRegression::train(const vector<double> &x, double y) {
    double y_hat = predict(x);
    double err = (y - y_hat);
    // update weights w1..w5
    size_t n = min<size_t>(5, x.size());
    for (size_t i = 0; i < n; ++i) {
        weights[i + 1] += alpha * err * x[i];
    }
    // update bias
    weights[0] += alpha * err;
}

void LinearRegression::saveWeights(const string &filename) const {
    json j;
    j["weights"] = weights;
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Warning: could not open " << filename << " to save weights\n";
        return;
    }
    file << j.dump(4);
}

void LinearRegression::loadWeights(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        // no saved weights -> keep defaults
        return;
    }
    try {
        json j;
        file >> j;
        if (j.contains("weights") && j["weights"].is_array()) {
            auto arr = j["weights"];
            vector<double> w;
            for (auto &v : arr) w.push_back(v.get<double>());
            if (w.size() >= 1) weights = w;
            // ensure length 6
            if (weights.size() < 6) weights.resize(6, 0.0);
        }
    } catch (const std::exception &e) {
        cerr << "Error loading weights: " << e.what() << "\n";
    }
}

void LinearRegression::printWeights() const {
    cout << "LinearRegression weights: [";
    for (size_t i = 0; i < weights.size(); ++i) {
        cout << weights[i];
        if (i + 1 < weights.size()) cout << ", ";
    }
    cout << "]\n";
}

} // namespace ai
