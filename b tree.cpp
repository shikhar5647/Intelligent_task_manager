#include <iostream>
#include <vector>

struct BTreeNode {
    std::vector<int> keys;
    std::vector<BTreeNode*> children;
    bool leaf;
    int t; // Minimum degree
};

BTreeNode* createNode(int t, bool leaf) {
    BTreeNode* newNode = new BTreeNode();
    newNode->leaf = leaf;
    newNode->t = t;
    return newNode;
}

void splitChild(BTreeNode* parent, int index, BTreeNode* child) {
    BTreeNode* newNode = createNode(child->t, child->leaf);
    newNode->keys.resize(child->t - 1);

    for (int i = 0; i < child->t - 1; i++)
        newNode->keys[i] = child->keys[i + child->t];

    if (!child->leaf) {
        newNode->children.resize(child->t);
        for (int i = 0; i < child->t; i++)
            newNode->children[i] = child->children[i + child->t];
    }

    child->keys.resize(child->t - 1);

    parent->keys.insert(parent->keys.begin() + index, child->keys[child->t - 1]);
    parent->children.insert(parent->children.begin() + index + 1, newNode);
}

void insertNonFull(BTreeNode* node, int key) {
    int i = node->keys.size() - 1;

    if (node->leaf) {
        node->keys.push_back(0);
        while (i >= 0 && node->keys[i] > key) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
    } else {
        while (i >= 0 && node->keys[i] > key)
            i--;

        i++;
        if (node->children[i]->keys.size() == 2 * node->t - 1) {
            splitChild(node, i, node->children[i]);
            if (node->keys[i] < key)
                i++;
        }
        insertNonFull(node->children[i], key);
    }
}

void insert(BTreeNode*& root, int key) {
    if (!root) {
        root = createNode(3, true);
        root->keys.push_back(key);
    } else {
        if (root->keys.size() == 2 * root->t - 1) {
            BTreeNode* newNode = createNode(root->t, false);
            newNode->children.push_back(root);
            splitChild(newNode, 0, root);
            int i = 0;
            if (newNode->keys[0] < key)
                i++;
            insertNonFull(newNode->children[i], key);
            root = newNode;
        } else
            insertNonFull(root, key);
    }
}

void traverse(BTreeNode* root) {
    int i;
    for (i = 0; i < root->keys.size(); i++) {
        if (!root->leaf)
            traverse(root->children[i]);
        std::cout << " " << root->keys[i];
    }
    if (!root->leaf)
        traverse(root->children[i]);
}

BTreeNode* search(BTreeNode* root, int key) {
    int i = 0;
    while (i < root->keys.size() && key > root->keys[i])
        i++;
    if (root->keys[i] == key)
        return root;
    if (root->leaf)
        return nullptr;
    return search(root->children[i], key);
}

int main() {
    BTreeNode* root = nullptr;
    int keys[] = { 10, 20, 5, 6, 12, 30, 7, 17 };

    for (int key : keys)
        insert(root, key);

    std::cout << "Traversal of the constructed B-tree is: ";
    traverse(root);
    std::cout << std::endl;

    int searchKey = 6;
    BTreeNode* foundNode = search(root, searchKey);
    if (foundNode)
        std::cout << "Key " << searchKey << " found!" << std::endl;
    else
        std::cout << "Key " << searchKey << " not found!" << std::endl;

    return 0;
}
