
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX 100
#define SIZE 30

// Binary Tree Node
struct Node {
    char data[SIZE];
    struct Node *left;
    struct Node *right;
};

// Stacks
char operatorStack[MAX];
int opTop = -1;

struct Node *treeStack[MAX];
int treeTop = -1;

// Check operators
int isOperator(char ch) {
    return ch == '+' || ch == '-' ||
           ch == '*' || ch == '/' || ch == '^';
}

// Operator precedence
int precedence(char ch) {
    if (ch == '^') return 3;
    if (ch == '*' || ch == '/') return 2;
    if (ch == '+' || ch == '-') return 1;
    return 0;
}

// Create new node
struct Node* createNode(char value[]) {
    struct Node *newNode =
        (struct Node*)malloc(sizeof(struct Node));

    if (newNode == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    strcpy(newNode->data, value);
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

// Add token to postfix
void addPostfix(char postfix[][SIZE],
                int *count, char token[]) {
    if (*count >= MAX) {
        printf("Expression is too long!\n");
        exit(1);
    }

    strcpy(postfix[*count], token);
    (*count)++;
}

// Convert Infix to Postfix
int infixToPostfix(char infix[],
                   char postfix[][SIZE], int *count) {

    int i = 0;
    char token[SIZE];

    *count = 0;
    opTop = -1;

    while (infix[i] != '\0') {

        if (isspace((unsigned char)infix[i])) {
            i++;
        }

        // Read numbers
        else if (isdigit((unsigned char)infix[i]) ||
                 infix[i] == '.') {

            int j = 0;
            int dots = 0;

            while (isdigit((unsigned char)infix[i]) ||
                   infix[i] == '.') {

                if (infix[i] == '.') {
                    dots++;
                    if (dots > 1)
                        return 0;
                }

                if (j >= SIZE - 1)
                    return 0;

                token[j++] = infix[i++];
            }

            if (j == 0 || (j == 1 && token[0] == '.'))
                return 0;

            token[j] = '\0';
            addPostfix(postfix, count, token);
        }

        // Opening bracket
        else if (infix[i] == '(') {
            if (opTop >= MAX - 1)
                return 0;

            operatorStack[++opTop] = infix[i++];
        }

        // Closing bracket
        else if (infix[i] == ')') {

            while (opTop != -1 &&
                   operatorStack[opTop] != '(') {

                token[0] = operatorStack[opTop--];
                token[1] = '\0';

                addPostfix(postfix, count, token);
            }

            if (opTop == -1)
                return 0;

            opTop--;
            i++;
        }

        // Operators
        else if (isOperator(infix[i])) {

            char current = infix[i];

            while (opTop != -1 &&
                   operatorStack[opTop] != '(' &&
                   (precedence(operatorStack[opTop]) >
                    precedence(current) ||
                   (precedence(operatorStack[opTop]) ==
                    precedence(current) && current != '^'))) {

                token[0] = operatorStack[opTop--];
                token[1] = '\0';

                addPostfix(postfix, count, token);
            }

            if (opTop >= MAX - 1)
                return 0;

            operatorStack[++opTop] = current;
            i++;
        }

        else {
            return 0;
        }
    }

    // Pop remaining operators
    while (opTop != -1) {

        if (operatorStack[opTop] == '(')
            return 0;

        token[0] = operatorStack[opTop--];
        token[1] = '\0';

        addPostfix(postfix, count, token);
    }

    return (*count > 0);
}

// Build Expression Tree
struct Node* buildExpressionTree(
    char postfix[][SIZE], int count) {

    treeTop = -1;

    for (int i = 0; i < count; i++) {

        struct Node *newNode = createNode(postfix[i]);

        // Operator node
        if (strlen(postfix[i]) == 1 &&
            isOperator(postfix[i][0])) {

            if (treeTop < 1) {
                free(newNode);
                return NULL;
            }

            newNode->right = treeStack[treeTop--];
            newNode->left = treeStack[treeTop--];
        }

        // Push node
        if (treeTop >= MAX - 1) {
            free(newNode);
            return NULL;
        }

        treeStack[++treeTop] = newNode;
    }

    if (treeTop != 0)
        return NULL;

    return treeStack[treeTop];
}

// Inorder Traversal
void inorder(struct Node *root) {

    if (root != NULL) {

        if (isOperator(root->data[0]))
            printf("(");

        inorder(root->left);

        printf("%s", root->data);

        inorder(root->right);

        if (isOperator(root->data[0]))
            printf(")");
    }
}

// Preorder Traversal
void preorder(struct Node *root) {

    if (root != NULL) {

        printf("%s ", root->data);

        preorder(root->left);

        preorder(root->right);
    }
}

// Postorder Traversal
void postorder(struct Node *root) {

    if (root != NULL) {

        postorder(root->left);

        postorder(root->right);

        printf("%s ", root->data);
    }
}

// Evaluate Expression Tree
double evaluate(struct Node *root, int *valid) {

    double left, right;

    if (root == NULL) {
        *valid = 0;
        return 0;
    }

    // Leaf node
    if (!isOperator(root->data[0])) {
        return atof(root->data);
    }

    left = evaluate(root->left, valid);
    right = evaluate(root->right, valid);

    if (!(*valid))
        return 0;

    switch (root->data[0]) {

        case '+':
            return left + right;

        case '-':
            return left - right;

        case '*':
            return left * right;

        case '/':
            if (right == 0) {
                *valid = 0;
                return 0;
            }
            return left / right;

        case '^':
            return pow(left, right);

        default:
            *valid = 0;
            return 0;
    }
}

// Free Tree Memory
void freeTree(struct Node *root) {

    if (root != NULL) {

        freeTree(root->left);

        freeTree(root->right);

        free(root);
    }
}

// Main Function
int main() {

    char infix[MAX];
    char postfix[MAX][SIZE];

    int count = 0;
    int valid = 1;

    struct Node *root;

    printf("\n====================================\n");
    printf("       EXPRESSION TREE CALCULATOR\n");
    printf("====================================\n");

    printf("\nEnter mathematical expression: ");

    if (fgets(infix, MAX, stdin) == NULL) {
        printf("Input error!\n");
        return 1;
    }

    // Infix to Postfix Conversion
    if (!infixToPostfix(infix, postfix, &count)) {
        printf("\nInvalid Expression!\n");
        return 1;
    }

    printf("\nPostfix Expression: ");

    for (int i = 0; i < count; i++) {
        printf("%s ", postfix[i]);
    }

    // Build Expression Tree
    root = buildExpressionTree(postfix, count);

    if (root == NULL) {
        printf("\nInvalid Expression!\n");
        return 1;
    }

    // Display Traversals
    printf("\n\n========== TREE TRAVERSALS ==========\n");

    printf("\nInorder Traversal: ");
    inorder(root);

    printf("\nPreorder Traversal: ");
    preorder(root);

    printf("\nPostorder Traversal: ");
    postorder(root);

    // Calculate Result
    printf("\n\n========== CALCULATION ==========\n");

    double result = evaluate(root, &valid);

    if (valid) {
        printf("\nFinal Result: %.2lf\n", result);
    }
    else {
        printf("\nError: Division by zero or invalid calculation!\n");
    }

    printf("\n====================================\n");

    // Free allocated memory
    freeTree(root);

    return 0;
}
