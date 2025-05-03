#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum Color { RED, BLACK } Color;
typedef enum Dir { LEFT, RIGHT } Dir;

typedef struct Node {
    int val;
    struct Node *left;
    struct Node *right;
    struct Node *parent;
    Color color;
} Node;

Node *newNode(int val, Node *parent);
Node *insert(Node *n, int val, Node *parent, Dir d);
Node *fix(Node *n);
void printNode(Node *n, FILE *fp);
void printTree(Node *n);
int treeLength(Node *n);

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: ./rbt input.csv\n");
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        printf("File doesn't exist\n");
        return 1;
    }

    int cap = 10, size = 0;
    int *arr = malloc(cap * sizeof(int));

    char l[1024];
    fgets(l, sizeof(l), fp);
    fclose(fp);

    char *c = strtok(l, ",");

    while (c != NULL) {
        if (size >= cap) {
            cap *= 2;
            arr = realloc(arr, cap * sizeof(int));
        }
        arr[size++] = atoi(c);
        c = strtok(NULL, ",");
    }

    Node *root = NULL;
    for (int i = 0; i < size; i++) {
        root = insert(root, arr[i], root, RIGHT);
    }

    printTree(root);

    free(arr);
    return 0;
}

Node *leftRotate(Node *n) {
    Node *parent = n->parent;
    parent->right = n->left;
    if (parent->right)
        parent->right->parent = parent;
    n->left = parent;
    n->parent = parent->parent;
    parent->parent = n;
    if (!n->parent) {
        return n;
    }
    if (parent == n->parent->left) {
        n->parent->left = n;
    } else {
        n->parent->right = n;
    }
    return parent;
}

Node *rightRotate(Node *n) {
    Node *parent = n->parent;
    parent->left = n->right;
    if (parent->left)
        parent->left->parent = parent;
    n->right = parent;
    n->parent = parent->parent;
    parent->parent = n;
    if (!n->parent) {
        return n;
    }
    if (parent == n->parent->right) {
        n->parent->right = n;
    } else {
        n->parent->left = n;
    }
    return parent;
}

Node *fix(Node *n) {
    if (!n->parent) {
        n->color = BLACK;
        return n;
    }
    Node *parent = n->parent;
    if (n->color == RED && parent && parent->color == RED) {
        Node *grand = parent->parent;
        Dir parentDir = (grand->right && grand->right == parent) ? RIGHT : LEFT;
        Node *uncle = (parentDir == RIGHT) ? grand->left : grand->right;
        Color uncleColor = uncle ? uncle->color : BLACK;

        if (uncleColor == BLACK) {
            Dir dir = (n == parent->right) ? RIGHT : LEFT;
            if (parentDir == dir) {
                grand->color = RED;
                parent->color = BLACK;
                parent = (dir == RIGHT) ? leftRotate(parent) : rightRotate(parent);
                return fix(parent);
            } else {
                n = (dir == RIGHT) ? leftRotate(n) : rightRotate(n);
                return fix(n);
            }
        } else {
            parent->color = BLACK;
            uncle->color = BLACK;
            grand->color = RED;
            return fix(grand);
        }
    }
    return fix(n->parent);
}

Node *insert(Node *n, int val, Node *parent, Dir d) {
    if(n) {
        if (val == n->val) {
            return n;
        } else {
            parent = n;
            if (val < n->val) {
                return insert(n->left, val, n, LEFT);
            } else {
                return insert(n->right, val, n, RIGHT);
            }
        }
    }

    Node *new = newNode(val, parent);

    if (!parent)
        return new;
    if (d == LEFT) {
        parent->left = new;
    } else {
        parent->right = new;
    }

    return fix(new);
}

Node *newNode(int val, Node *parent) {
    Node *n = malloc(sizeof(Node));
    n->val = val;
    n->left = NULL;
    n->right = NULL;
    n->parent = parent;
    n->color = (n->parent) ? RED : BLACK;
    return n;
}

void printTree(Node *n) {
    FILE *fp = fopen("dot.gv", "w");

    fprintf(fp, "digraph BST {\n");

    printNode(n, fp);

    fprintf(fp, "}");
    fclose(fp);

    system("dot -Tpng dot.gv -o dot.png && feh dot.png");
}

void printNode(Node *n, FILE *fp) {
    if (!n) {
        return;
    }

    char *color = (n->color == BLACK) ? "black" : "red";
    fprintf(fp,
            "%d [shape=circle, style=filled, fillcolor=%s, "
            "fontcolor=white];\n",
            n->val, color);

    if (n->left) {
        fprintf(fp, "%d -> %d;\n", n->val, n->left->val);
        printNode(n->left, fp);
    } else {
        fprintf(fp,
                "NULL_LEFT_%d [shape=point];\n"
                "%d -> NULL_LEFT_%d;\n",
                n->val, n->val, n->val);
    }

    if (n->right) {
        fprintf(fp, "%d -> %d;\n", n->val, n->right->val);
        printNode(n->right, fp);
    } else {
        fprintf(fp,
                "NULL_RIGHT_%d [shape=point];\n"
                "%d -> NULL_RIGHT_%d;\n",
                n->val, n->val, n->val);
    }

    free(n);
}
