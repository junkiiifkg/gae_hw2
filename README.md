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
