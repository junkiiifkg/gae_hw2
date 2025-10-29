# Restaurant Bot 🍽️

This C++ console application acts as a restaurant bot. It helps users build a menu, get food recommendations based on their taste preferences, and uses a simple linear regression model to predict user satisfaction.

## Features

* **Interactive Menu:** Add, remove, update, and view items on your personal menu.

* **AI-Powered Suggestions:**

  * Get a random menu suggestion optimized by an AI model for the best-predicted satisfaction.

  * Request a menu tailored to your specific taste profile (sweet, salty, sour, bitter, umami/savory).

* **Satisfaction-Based Learning:** Train a linear regression model by rating your suggested or final menu to improve future recommendations.

* **Dynamic Menu:** Loads menu items from an external `menu.json` file.

* **Persistent AI Model:** The AI model's weights are saved to and loaded from `weights.json`.

* **Object-Oriented Design:** Uses inheritance, composition, and association to model menu items, menus, and users.

## Project Design: OOP Relationships

This project is structured using fundamental Object-Oriented Programming (OOP) relationships to manage complexity and create a flexible system.

### Concepts of Relationships in OOP

* **Inheritance:** A mechanism that allows a new class (derived or child class) to inherit attributes and behaviors from an existing class (base or parent class).
 
* **Association:** A relationship where two or more classes are connected, but they can exist independently of each other.

* **Composition:** A strong "has-a" relationship where a class (the whole) "owns" or is composed of another class (the part), and the part cannot exist independently of the whole. If the whole is destroyed, the part is also destroyed.

* **Aggregation:** A weaker "has-a" relationship where one class contains another, but the contained class can exist independently of the container.

### Class Relations in This Project

### Inheritance:

* **Base Class:** MenuItem (An abstract base class).

* **Derived Classes:** Starter, Salad, MainCourse, Drink, Appetizer, and Dessert all inherit from MenuItem. Each derived class provides concrete implementations for virtual functions like printInfo and customize, and adds its own specific attributes (e.g., isHot for Starter).

### Composition:

* **Relation:** The User class is composed of a Menu object.

* **Details:** The User class has a member variable Menu userMenu; (as seen in Menu.hpp). This Menu object is a part of the User, and its lifecycle is bound to the User object. A User owns a Menu.

* **Relation:** The User class is composed of a MenuItem object.

* **Details:** The Menu class contains a std::vector<std::shared_ptr<MenuItem>> items;. This represents a composition relationship as the Menu "owns" the MenuItem objects it contains.

### Association:

* **Relations:** The User class is associated with the menu catalog.

* **Details:** The User class uses the menu catalog (std::map<std::string, std::vector<json>>), passed as a parameter to its interact method, to display available items. However, the User does not own this catalog or control its lifecycle. They are independent entities that interact.

### Aggregation:

* **Relations:** The Menu class aggregates the total cost and average taste balance.

* **Details:** The Menu class holds totalCost and tasteAvg attributes. These values are derived from the MenuItem objects it contains. They represent an aggregation of data from the parts that it is composed of.

## JSON File Integration

This project uses the nlohmann/json library to handle external data.

* menu.json: Contains all available restaurant items, their prices, and detailed taste profiles, organized by category. This file is loaded at runtime.

* weights.json: Stores the learned weights of the AI's linear regression model. The application reads this file on startup and saves to it after the model is trained with new user feedback.

## AI and Taste Balance

The bot's core feature is its ability to suggest menus and learn from user feedback.

* Random Menu (AI Optimized): When the user requests a random menu, the bot generates multiple (e.g., 30-40) random menus and uses the ai::LinearRegression model to predict user satisfaction for each. It then suggests the menu with the highest predicted score.

* Taste Profile Menu: If the user provides a target taste balance (e.g., high sweet, low sour), the bot iterates through the catalog and picks the item from each category that is closest (using Euclidean distance) to the user's desired profile.

* Training: After a menu is suggested or built, the user is asked for a satisfaction score (0.0 to 1.0). This score, along with the menu's average taste vector, is used to train the model, updating its weights to make better predictions in the future.
