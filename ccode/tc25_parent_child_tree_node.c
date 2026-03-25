#include <stdio.h>
#include <stdlib.h>

// Description: A C tree where a child node has a struct Node* parent pointer, used to traverse back up the tree.

typedef struct Node {
    int value;
    struct Node* parent; // Parent pointer to traverse back up
    struct Node* left;
    struct Node* right;
} Node;

Node* create_node(int value, Node* parent) {
    Node* node = malloc(sizeof(Node));
    node->value = value;
    node->parent = parent;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void traverse_up(Node* node) {
    Node* current = node;
    printf("Traversing up from %d: ", current->value);
    while (current != NULL) {
        printf("%d -> ", current->value);
        current = current->parent;
    }
    printf("NULL\n");
}

int main() {
    Node* root = create_node(10, NULL);
    root->left = create_node(5, root);
    root->right = create_node(15, root);
    
    Node* left_child = root->left;
    left_child->left = create_node(2, left_child);
    left_child->right = create_node(7, left_child);

    Node* target_node = left_child->right;

    traverse_up(target_node);

    // Cleanup omitted for brevity
    return 0;
}
