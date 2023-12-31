#pragma once
#include <iostream>
#include <queue>
#include "registry.h"
using namespace std;

// Citation: https://algorithmtutor.com/Data-Structures/Tree/Splay-Trees/
// Citation: https://www.tutorialspoint.com/cplusplus-program-to-implement-splay-tree

struct SplayTreeNode {
    Registry::Vehicle* vehicle;
    SplayTreeNode *left, *right, *parent;

    SplayTreeNode(Registry::Vehicle* v) : vehicle(v), left(nullptr), right(nullptr), parent(nullptr) {}
};

class SplayTree {
private:
    SplayTreeNode* root;

    // Rotations
    void rotateLeft(SplayTreeNode* node);
    void rotateRight(SplayTreeNode* node);
    void splay(SplayTreeNode* &node);

public:
    SplayTree() : root(nullptr) {}
    ~SplayTree();

    // Helper function for the destructor
    void deleteTree(SplayTreeNode* node);

    // Member functions
    void insert(Registry::Vehicle* vehicle);
    Registry::Vehicle* search(const string& model);
    void BFS() const;
};

void SplayTree::rotateLeft(SplayTreeNode* node) {
    if (node == nullptr || node->right == nullptr) return;  // Can't rotate left if node or node's right child is null

    SplayTreeNode* rightChild = node->right;   // rightChild is the right child of node
    node->right = rightChild->left;            // rightChild's left subtree becomes node's right subtree
    if (rightChild->left != nullptr) {
        rightChild->left->parent = node;       // Set parent of rightChild's left child (if exists)
    }

    rightChild->parent = node->parent;         // Link node's parent to rightChild
    if (node->parent == nullptr) {             // node is the root
        root = rightChild;
    }
    else if (node == node->parent->left) {   // node is a left child
        node->parent->left = rightChild;
    }
    else {                                   // node is a right child
        node->parent->right = rightChild;
    }

    rightChild->left = node;                   // Place node on rightChild's left
    node->parent = rightChild;                 // Update node's parent
}

// rotateRight is mirror of RotateLeft
void SplayTree::rotateRight(SplayTreeNode* node) {
    if (node == nullptr || node->left == nullptr) return;  // Can't rotate right if node or node's left child is null

    SplayTreeNode* leftChild = node->left;  // leftChild is the left child of node
    node->left = leftChild->right;          // leftChild's right subtree becomes node's left subtree

    if (leftChild->right != nullptr) {
        leftChild->right->parent = node;    // Set parent of leftChild's right child (if exists)
    }

    leftChild->parent = node->parent;       // Link node's parent to leftChild

    if (node->parent == nullptr) {          // node is the root
        root = leftChild;
    } else if (node == node->parent->right) { // node is a left child
        node->parent->right = leftChild;
    } else {                                 // node is a right child
        node->parent->left = leftChild;
    }

    leftChild->right = node;                // Place node on leftChild's right
    node->parent = leftChild;               // Update node's parent
}

void SplayTree::splay(SplayTreeNode* &node) {
    if (node == nullptr) return;

    while (node->parent != nullptr) {  // Continue until node is the root
        if (node->parent->parent == nullptr) {
            // Zig step: node has a parent but no grandparent
            if (node->parent->left == node) {
                rotateRight(node->parent);  // Zig rotation
            } else if (node->parent->right == node){
                rotateLeft(node->parent);   // Zag rotation
            }
        } else if (node->parent->left == node && node->parent->parent->left == node->parent) {
            // Zig-Zig step
            rotateRight(node->parent->parent);
            rotateRight(node->parent);
        } else if (node->parent->right == node && node->parent->parent->right == node->parent) {
            // Zig-Zig step (symmetric)
            rotateLeft(node->parent->parent);
            rotateLeft(node->parent);
        } else if (node->parent->right == node && node->parent->parent->left == node->parent) {
            // Zig-Zag step
            rotateLeft(node->parent);
            rotateRight(node->parent->parent);
        } else if (node->parent->left == node && node->parent->parent->right == node->parent){
            // Zig-Zag step (symmetric)
            rotateRight(node->parent);
            rotateLeft(node->parent->parent);
        }
    }
    root = node;  // Update the root after splaying
}

void SplayTree::insert(Registry::Vehicle* vehicle) {
    // Step 1: Perform standard BST insert
    SplayTreeNode* parentNode = nullptr;
    SplayTreeNode* current = root;
    bool insert = true;

    // Find the correct parent for the new node or find a duplicate
    while (current != nullptr) {
        parentNode = current;

        if (vehicle->model == current->vehicle->model) {
            current->vehicle->AddDuplicate(vehicle);
            insert = false; // No need to insert a new node, delete the created one
            break;
        } else if (vehicle->model < current->vehicle->model) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    if(insert){
        // Create a new node only if no duplicate was found
        SplayTreeNode* newNode = new SplayTreeNode(vehicle); // Deep copy

        // parentNode is the parent of the new node
        newNode->parent = parentNode;

        // Determine which child of the parent new node will be
        if (parentNode == nullptr) {
            root = newNode; // The tree was empty, new node is root
        } else if (vehicle->model < parentNode->vehicle->model) {
            parentNode->left = newNode;
        } else {
            parentNode->right = newNode;
        }

        // Step 2: Splay the tree at the newly inserted node
        splay(newNode);
    }
}

Registry::Vehicle* SplayTree::search(const string& model) {
    SplayTreeNode* current = root;
    SplayTreeNode* lastVisited = nullptr;

    while (current != nullptr) {
        lastVisited = current;

        if (model == current->vehicle->model) {
            splay(current);  // Splay at the found node
            return current->vehicle;  // Return the Vehicle object
        } else {
            // Decide whether to go left or right based on model
            current = (model < current->vehicle->model) ? current->left : current->right;
        }
    }

    // Splay at the last visited node if the exact vehicle isn't found
    if (lastVisited != nullptr) {
        splay(lastVisited);
    }

    return nullptr;  // No matching node found
}

void SplayTree::deleteTree(SplayTreeNode* node) {
    if (node != nullptr) {
        deleteTree(node->left);   // Delete left subtree
        deleteTree(node->right);  // Delete right subtree
        delete node->vehicle;     // Assuming ownership of vehicle, delete it
        delete node;              // Delete the node itself
    }
}

SplayTree::~SplayTree() {
    deleteTree(root);  // Start the recursive deletion from the root
}
