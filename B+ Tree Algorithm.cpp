#include <iostream>
#include <fstream>
#include <vector>

const int MAX_CHILDREN = 4;
const int MIN_OCCUPANCY = MAX_CHILDREN / 2;

struct Item {
    std::string name;
    int quantity;
    // Add other item attributes as needed
};

struct BPlusTreeNode {
    std::vector<int> keys;
    std::vector<Item> data;
    std::vector<BPlusTreeNode*> children;
    bool isLeaf;

    BPlusTreeNode() {
        isLeaf = false;
    }
};

class BPlusTree {
private:
    BPlusTreeNode* root;
    std::fstream file;

public:
    BPlusTree(const std::string& fileName) {
        root = nullptr;
        openFile(fileName);
        if (file.peek() == std::fstream::traits_type::eof()) {
            createEmptyTree();
        } else {
            loadTreeFromFile();
        }
    }

    ~BPlusTree() {
        file.close();
        destroyTree(root);
    }

    void insert(int key, const Item& item) {
        if (root == nullptr) {
            createEmptyTree();
        }

        if (root->keys.size() >= MAX_CHILDREN) {
            BPlusTreeNode* newRoot = new BPlusTreeNode();
            newRoot->children.push_back(root);
            splitChild(newRoot, 0);
            root = newRoot;
        }
        insertNonFull(root, key, item);
        saveTreeToFile();
    }

    void search(int key) {
        if (root == nullptr) {
            std::cout << "Tree is empty." << std::endl;
        }
        else {
            BPlusTreeNode* result = searchNode(root, key);
            if (result != nullptr) {
                int index = findKeyIndex(result->keys, key);
                std::cout << "Item found: " << result->data[index].name << ", Quantity: " << result->data[index].quantity << std::endl;
            }
            else {
                std::cout << "Item not found." << std::endl;
            }
        }
    }

    void remove(int key) {
        if (root == nullptr) {
            std::cout << "Tree is empty." << std::endl;
        }
        else {
            removeKey(root, key);
            saveTreeToFile();
        }
    }

private:
    void openFile(const std::string& fileName) {
        file.open(fileName, std::ios::binary | std::ios::in | std::ios::out);
        if (!file) {
            file.open(fileName, std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
        }
    }

    void createEmptyTree() {
        root = new BPlusTreeNode();
        root->isLeaf = true;
        saveTreeToFile();
    }

    void loadTreeFromFile() {
        file.seekg(0);
        int isLeaf;
        file.read(reinterpret_cast<char*>(&isLeaf), sizeof(int));
        root = new BPlusTreeNode();
        root->isLeaf = isLeaf;
        loadNodeFromFile(root);
    }

    void loadNodeFromFile(BPlusTreeNode* node) {
        int numKeys;
        file.read(reinterpret_cast<char*>(&numKeys), sizeof(int));
        node->keys.resize(numKeys);
        node->data.resize(numKeys);
        file.read(reinterpret_cast<char*>(node->keys.data()), numKeys * sizeof(int));
        file.read(reinterpret_cast<char*>(node->data.data()), numKeys * sizeof(Item));

        if (!node->isLeaf) {
            node->children.resize(numKeys + 1);
            for (int i = 0; i < numKeys + 1; ++i) {
                node->children[i] = new BPlusTreeNode();
                int isLeaf;
                file.read(reinterpret_cast<char*>(&isLeaf), sizeof(int));
                node->children[i]->isLeaf = isLeaf;
                loadNodeFromFile(node->children[i]);
            }
        }
    }

    void saveTreeToFile() {
        file.seekp(0);
        int isLeaf = root->isLeaf;
        file.write(reinterpret_cast<const char*>(&isLeaf), sizeof(int));
        saveNodeToFile(root);
    }

    void saveNodeToFile(BPlusTreeNode* node) {
        int numKeys = node->keys.size();
        file.write(reinterpret_cast<const char*>(&numKeys), sizeof(int));
        file.write(reinterpret_cast<const char*>(node->keys.data()), numKeys * sizeof(int));
        file.write(reinterpret_cast<const char*>(node->data.data()), numKeys * sizeof(Item));

        if (!node->isLeaf) {
            for (int i = 0; i < numKeys + 1; ++i) {
                int isLeaf = node->children[i]->isLeaf;
                file.write(reinterpret_cast<const char*>(&isLeaf), sizeof(int));
                saveNodeToFile(node->children[i]);
            }
        }
    }

    void destroyTree(BPlusTreeNode* node) {
        if (node != nullptr) {
            if (!node->isLeaf) {
                for (BPlusTreeNode* child : node->children) {
                    destroyTree(child);
                }
            }
            delete node;
        }
    }

    void insertNonFull(BPlusTreeNode* node, int key, const Item& item) {
        int i = node->keys.size() - 1;
        if (node->isLeaf) {
            while (i >= 0 && key < node->keys[i]) {
                i--;
            }
            node->keys.insert(node->keys.begin() + i + 1, key);
            node->data.insert(node->data.begin() + i + 1, item);
        }
        else {
            while (i >= 0 && key < node->keys[i]) {
                i--;
            }
            i++;
            if (node->children[i]->keys.size() >= MAX_CHILDREN) {
                splitChild(node, i);
                if (key > node->keys[i]) {
                    i++;
                }
            }
            insertNonFull(node->children[i], key, item);
        }
    }

    void splitChild(BPlusTreeNode* parentNode, int childIndex) {
        BPlusTreeNode* childNode = parentNode->children[childIndex];
        BPlusTreeNode* newNode = new BPlusTreeNode();
        newNode->isLeaf = childNode->isLeaf;

        parentNode->keys.insert(parentNode->keys.begin() + childIndex, childNode->keys[MIN_OCCUPANCY]);
        parentNode->data.insert(parentNode->data.begin() + childIndex, childNode->data[MIN_OCCUPANCY]);

        newNode->keys.assign(childNode->keys.begin() + MIN_OCCUPANCY + 1, childNode->keys.end());
        newNode->data.assign(childNode->data.begin() + MIN_OCCUPANCY + 1, childNode->data.end());
        childNode->keys.resize(MIN_OCCUPANCY);
        childNode->data.resize(MIN_OCCUPANCY);

        if (!childNode->isLeaf) {
            newNode->children.assign(childNode->children.begin() + MIN_OCCUPANCY + 1, childNode->children.end());
            childNode->children.resize(MIN_OCCUPANCY + 1);
        }

        parentNode->children.insert(parentNode->children.begin() + childIndex + 1, newNode);
    }

    BPlusTreeNode* searchNode(BPlusTreeNode* node, int key) {
        int i = 0;
        while (i < node->keys.size() && key > node->keys[i]) {
            i++;
        }
        if (i < node->keys.size() && key == node->keys[i]) {
            return node;
        }
        else if (node->isLeaf) {
            return nullptr;
        }
        else {
            return searchNode(node->children[i], key);
        }
    }

    int findKeyIndex(const std::vector<int>& keys, int key) {
        int i = 0;
        while (i < keys.size() && key > keys[i]) {
            i++;
        }
        return i;
    }

    void removeKey(BPlusTreeNode* node, int key) {
        int index = findKeyIndex(node->keys, key);
        if (index < node->keys.size() && key == node->keys[index]) {
            if (node->isLeaf) {
                node->keys.erase(node->keys.begin() + index);
                node->data.erase(node->data.begin() + index);
            }
            else {
                // Handle deletion from internal node
            }
        }
        else {
            if (node->isLeaf) {
                std::cout << "Item not found." << std::endl;
            }
            else {
                // Handle deletion from internal node
            }
        }
    }
};

int main() {
    std::string fileName = "inventory.dat";
    BPlusTree inventory(fileName);

    // Insert items
    inventory.insert(1001, { "Item 1", 10 });
    inventory.insert(1002, { "Item 2", 5 });
    inventory.insert(1003, { "Item 3", 8 });

    // Search for items
    inventory.search(1002);
    inventory.search(1004);

    // Remove an item
    inventory.remove(1002);

    // Search again after deletion
    inventory.search(1002);

    return 0;
}