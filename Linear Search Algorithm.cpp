#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

struct Item {
    std::string name;
    int quantity;
};

std::vector<Item> inventory_search(const std::vector<Item>& inventory, const std::string& searchQuery) {
    std::vector<Item> searchResults;

    for (const Item& item : inventory) {
        // Convert both the search query and item name to lowercase for case-insensitive search
        std::string lowerSearchQuery = searchQuery;
        std::transform(lowerSearchQuery.begin(), lowerSearchQuery.end(), lowerSearchQuery.begin(), ::tolower);
        std::string lowerItemName = item.name;
        std::transform(lowerItemName.begin(), lowerItemName.end(), lowerItemName.begin(), ::tolower);

        if (lowerItemName.find(lowerSearchQuery) != std::string::npos) {
            searchResults.push_back(item);
        }
    }

    return searchResults;
}

void add_item(std::vector<Item>& inventory, const std::string& name, int quantity) {
    inventory.push_back({name, quantity});
}

bool delete_item(std::vector<Item>& inventory, const std::string& name) {
    auto it = std::find_if(inventory.begin(), inventory.end(), [&](const Item& item) {
        return item.name == name;
    });

    if (it != inventory.end()) {
        inventory.erase(it);
        return true;
    }

    return false;
}

bool update_item(std::vector<Item>& inventory, const std::string& name, int newQuantity) {
    auto it = std::find_if(inventory.begin(), inventory.end(), [&](const Item& item) {
        return item.name == name;
    });

    if (it != inventory.end()) {
        it->quantity = newQuantity;
        return true;
    }

    return false;
}

int main() {
    std::vector<Item> inventory = {
        {"Apple", 10},
        {"Banana", 5},
        {"Orange", 7},
        {"Pear", 3}
    };

    std::string searchQuery = "apple";
    std::vector<Item> results = inventory_search(inventory, searchQuery);

    std::cout << "Search Results:" << std::endl;
    for (const Item& item : results) {
        std::cout << "Name: " << item.name << ", Quantity: " << item.quantity << std::endl;
    }

    // Add a new item
    add_item(inventory, "Grapes", 8);

    // Delete an item
    bool deleted = delete_item(inventory, "Banana");
    if (deleted) {
        std::cout << "Banana deleted from inventory." << std::endl;
    } else {
        std::cout << "Banana not found in inventory." << std::endl;
    }

    // Update an item
    bool updated = update_item(inventory, "Orange", 15);
    if (updated) {
        std::cout << "Quantity of Orange updated." << std::endl;
    } else {
        std::cout << "Orange not found in inventory." << std::endl;
    }

    // Print the updated inventory
    std::cout << "Updated Inventory:" << std::endl;
    for (const Item& item : inventory) {
        std::cout << "Name: " << item.name << ", Quantity: " << item.quantity << std::endl;
    }

    return 0;
}
