#ifndef AI_HPP
#define AI_HPP

#include <iostream>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>

namespace ai {

class LinearRegression {
private:
    std::vector<double> weights; // w0..w5
    double alpha; // learning rate

public:
    LinearRegression(double lr = 0.01);

    double predict(const std::vector<double> &x) const;
    void train(const std::vector<double> &x, double y);
    void saveWeights(const std::string &filename) const;
    void loadWeights(const std::string &filename);
    void printWeights() const;
};

} // namespace ai

#endif
