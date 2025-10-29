# Restaurant Bot
## Class Relations Explanation:
### 1. Inheritance:
* Base Class: MenuItem Pure virtual functions for getting and setting attributes (name, price, taste balance). Derived classes (e.g., Starter, Salad, MainCourse, Drink, Appetizer, Dessert) inherit from MenuItem. Each derived class overrides virtual functions and introduces additional attributes and functionality specific to its type.
### 2. Association:
* User Class Associated with Menu Class: User class contains a Menu object as an attribute. The User and Menu classes are associated, but not tightly coupled, as a User can exist independently of a Menu.
### 3. Composition:
* Menu Class Composed of MenuItem Objects: Menu class contains a vector of MenuItem pointers. This represents a composition relationship, as a Menu "owns" the MenuItem objects. When a Menu is destroyed, the associated MenuItem objects are also cleaned up.
### 4. Aggregation:
* Menu Class Aggregating Overall Taste Balance and Total Cost: Menu class aggregates attributes for overall taste balance and total cost. This represents an aggregation relationship, as these attributes can exist independently of the Menu. They are part of the Menu, but they can also be considered standalone.
