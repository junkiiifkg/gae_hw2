#ifndef MENU_HPP
#define MENU_HPP

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <nlohmann/json.hpp>

namespace menu {

using json = nlohmann::json;

// ========== BASE CLASS ==========
class MenuItem {
protected:
    std::string name;
    double price;
    std::vector<double> taste; // [sweet, salty, sour, bitter, spicy]

public:
    MenuItem(const std::string &n = "", double p = 0.0, const std::vector<double> &t = std::vector<double>(5, 0.5));
    virtual ~MenuItem() = default;

    virtual void printInfo() const = 0;
    virtual void customize() = 0;

    std::string getName() const;
    double getPrice() const;
    std::vector<double> getTaste() const;
    double getTasteAvg() const;

    void setName(const std::string &n);
    void setPrice(double p);
    void setTaste(const std::vector<double> &t);
};

// ========== CHILD CLASSES ==========
class Starter : public MenuItem {
    bool isHot;
public:
    Starter(const std::string &n = "", double p = 0.0, const std::vector<double> &t = std::vector<double>(5,0.5), bool hot=false);
    void printInfo() const override;
    void customize() override;
};

class Salad : public MenuItem {
    bool hasTopping;
public:
    Salad(const std::string &n = "", double p = 0.0, const std::vector<double> &t = std::vector<double>(5,0.5), bool topping=false);
    void printInfo() const override;
    void customize() override;
};

class MainCourse : public MenuItem {
    bool isVegetarian;
public:
    MainCourse(const std::string &n = "", double p = 0.0, const std::vector<double> &t = std::vector<double>(5,0.5), bool veg=false);
    void printInfo() const override;
    void customize() override;
};

class Drink : public MenuItem {
    bool carbonated;
    bool extraShot;
public:
    Drink(const std::string &n = "", double p = 0.0, const std::vector<double> &t = std::vector<double>(5,0.5), bool carb=false, bool shot=false);
    void printInfo() const override;
    void customize() override;
};

class Appetizer : public MenuItem {
    std::string serveTime;
public:
    Appetizer(const std::string &n = "", double p = 0.0, const std::vector<double> &t = std::vector<double>(5,0.5), const std::string &serve="before");
    void printInfo() const override;
    void customize() override;
};

class Dessert : public MenuItem {
    bool extraChocolate;
public:
    Dessert(const std::string &n = "", double p = 0.0, const std::vector<double> &t = std::vector<double>(5,0.5), bool choc=false);
    void printInfo() const override;
    void customize() override;
};

// ========== MENU & USER ==========
class Menu {
    std::vector<std::shared_ptr<MenuItem>> items;
    double totalCost;
    std::vector<double> tasteAvg; // average taste vector
public:
    Menu();
    void addItem(std::shared_ptr<MenuItem> item);
    void removeItem(const std::string &name);
    void updateItem(const std::string &name);
    void showMenu() const;
    double getTotalCost() const;
    std::vector<double> getTasteAvg() const;
};

class User {
    std::string firstName;
    std::string lastName;
    std::string gender;
    Menu userMenu;
public:
    User(const std::string &f="", const std::string &l="", const std::string &g="");
    void showInfo() const;
    Menu &getMenu();

    // updated: accept catalog so interact can list existing items per category
    void interact(const std::map<std::string, std::vector<json>> &catalog);
};

} // namespace menu

#endif