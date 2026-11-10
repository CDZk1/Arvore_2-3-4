#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Node {
    int num_keys;
    int keys[3];
    struct Node* children[4];
    bool is_leaf;
} Node;

typedef struct QueueNode {
    Node* treeNode;
    int level;
    struct QueueNode* next;
} QueueNode;

typedef struct {
    QueueNode* front;
    QueueNode* rear;
} Queue;

Node* createNode(bool is_leaf);
void splitChild(Node* parent, int i, Node* child);
void insertNonFull(Node* node, int key);
void insert(Node** root, int key);
bool search(Node* node, int key);
void inOrder(Node* node);
void levelOrder(Node* root);
void freeTree(Node* node);
int findKey(Node* node, int key);
void removeFromLeaf(Node* node, int idx);
void removeFromNonLeaf(Node* node, int idx);
int getPred(Node* node, int idx);
int getSucc(Node* node, int idx);
void fill(Node* node, int idx);
void borrowFromPrev(Node* node, int idx);
void borrowFromNext(Node* node, int idx);
void merge(Node* node, int idx);
void deleteKey(Node* node, int key);
void deleteTreeKey(Node** root, int key);

Node* createNode(bool is_leaf) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        printf("Erro de alocacao de memoria!\n");
        exit(1);
    }
    newNode->is_leaf = is_leaf;
    newNode->num_keys = 0;
    for (int i = 0; i < 4; i++) {
        newNode->children[i] = NULL;
    }
    return newNode;
}

void splitChild(Node* parent, int i, Node* child) {
    Node* newNode = createNode(child->is_leaf);
    newNode->num_keys = 1;
    
    newNode->keys[0] = child->keys[2];

    if (!child->is_leaf) {
        newNode->children[0] = child->children[2];
        newNode->children[1] = child->children[3];
    }
    
    child->num_keys = 1;

    for (int j = parent->num_keys; j >= i + 1; j--) {
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[i + 1] = newNode;

    for (int j = parent->num_keys - 1; j >= i; j--) {
        parent->keys[j + 1] = parent->keys[j];
    }
    parent->keys[i] = child->keys[1];
    parent->num_keys++;
}

void insertNonFull(Node* node, int key) {
    int i = node->num_keys - 1;

    if (node->is_leaf) {
        while (i >= 0 && node->keys[i] > key) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->num_keys++;
    } else {
        while (i >= 0 && node->keys[i] > key) {
            i--;
        }
        i++;
        
        if (node->children[i]->num_keys == 3) {
            splitChild(node, i, node->children[i]);
            if (node->keys[i] < key) {
                i++;
            }
        }
        insertNonFull(node->children[i], key);
    }
}

void insert(Node** root, int key) {
    if (search(*root, key)) {
        printf("Aviso: O valor %d ja esta presente na arvore e sera ignorado.\n", key);
        return; 
    }

    if (*root == NULL) {
        *root = createNode(true);
        (*root)->keys[0] = key;
        (*root)->num_keys = 1;
        return;
    }

    if ((*root)->num_keys == 3) {
        Node* newRoot = createNode(false);
        newRoot->children[0] = *root;
        splitChild(newRoot, 0, *root);
        
        int i = 0;
        if (newRoot->keys[0] < key) {
            i++;
        }
        insertNonFull(newRoot->children[i], key);
        *root = newRoot;
    } else {
        insertNonFull(*root, key);
    }
}

bool search(Node* node, int key) {
    if (node == NULL) return false;
    
    int i = 0;
    while (i < node->num_keys && key > node->keys[i]) {
        i++;
    }
    
    if (i < node->num_keys && key == node->keys[i]) {
        return true;
    }
    
    if (node->is_leaf) {
        return false;
    }
    
    return search(node->children[i], key);
}

void inOrder(Node* node) {
    if (node != NULL) {
        int i;
        for (i = 0; i < node->num_keys; i++) {
            if (!node->is_leaf) {
                inOrder(node->children[i]);
            }
            printf("%d ", node->keys[i]);
        }
        if (!node->is_leaf) {
            inOrder(node->children[i]);
        }
    }
}

void enqueue(Queue* q, Node* node, int level) {
    QueueNode* temp = (QueueNode*)malloc(sizeof(QueueNode));
    temp->treeNode = node;
    temp->level = level;
    temp->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = temp;
        return;
    }
    q->rear->next = temp;
    q->rear = temp;
}

QueueNode* dequeue(Queue* q) {
    if (q->front == NULL) return NULL;
    QueueNode* temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    return temp;
}

void levelOrder(Node* root) {
    if (root == NULL) {
        printf("A arvore esta vazia.\n");
        return;
    }
    Queue q = {NULL, NULL};
    enqueue(&q, root, 0);

    int current_level = 0;
    printf("Nivel 0: ");
    while (q.front != NULL) {
        QueueNode* qn = dequeue(&q);
        Node* curr = qn->treeNode;
        int level = qn->level;

        if (level > current_level) {
            printf("\nNivel %d: ", level);
            current_level = level;
        }

        printf("[");
        for (int i = 0; i < curr->num_keys; i++) {
            printf("%d", curr->keys[i]);
            if (i < curr->num_keys - 1) printf(" ");
        }
        printf("] ");

        if (!curr->is_leaf) {
            for (int i = 0; i <= curr->num_keys; i++) {
                enqueue(&q, curr->children[i], level + 1);
            }
        }
        free(qn);
    }
    printf("\n");
}

int findKey(Node* node, int key) {
    int idx = 0;
    while (idx < node->num_keys && node->keys[idx] < key)
        ++idx;
    return idx;
}

void removeFromLeaf(Node* node, int idx) {
    for (int i = idx + 1; i < node->num_keys; ++i)
        node->keys[i - 1] = node->keys[i];
    node->num_keys--;
}

int getPred(Node* node, int idx) {
    Node* curr = node->children[idx];
    while (!curr->is_leaf)
        curr = curr->children[curr->num_keys];
    return curr->keys[curr->num_keys - 1];
}

int getSucc(Node* node, int idx) {
    Node* curr = node->children[idx + 1];
    while (!curr->is_leaf)
        curr = curr->children[0];
    return curr->keys[0];
}

void borrowFromPrev(Node* node, int idx) {
    Node* child = node->children[idx];
    Node* sibling = node->children[idx - 1];

    for (int i = child->num_keys - 1; i >= 0; --i)
        child->keys[i + 1] = child->keys[i];

    if (!child->is_leaf) {
        for (int i = child->num_keys; i >= 0; --i)
            child->children[i + 1] = child->children[i];
    }

    child->keys[0] = node->keys[idx - 1];
    if (!child->is_leaf)
        child->children[0] = sibling->children[sibling->num_keys];

    node->keys[idx - 1] = sibling->keys[sibling->num_keys - 1];
    child->num_keys += 1;
    sibling->num_keys -= 1;
}

void borrowFromNext(Node* node, int idx) {
    Node* child = node->children[idx];
    Node* sibling = node->children[idx + 1];

    child->keys[(child->num_keys)] = node->keys[idx];

    if (!child->is_leaf)
        child->children[(child->num_keys) + 1] = sibling->children[0];

    node->keys[idx] = sibling->keys[0];

    for (int i = 1; i < sibling->num_keys; ++i)
        sibling->keys[i - 1] = sibling->keys[i];

    if (!sibling->is_leaf) {
        for (int i = 1; i <= sibling->num_keys; ++i)
            sibling->children[i - 1] = sibling->children[i];
    }

    child->num_keys += 1;
    sibling->num_keys -= 1;
}

void merge(Node* node, int idx) {
    Node* child = node->children[idx];
    Node* sibling = node->children[idx + 1];

    child->keys[1] = node->keys[idx];

    for (int i = 0; i < sibling->num_keys; ++i)
        child->keys[i + 2] = sibling->keys[i];

    if (!child->is_leaf) {
        for (int i = 0; i <= sibling->num_keys; ++i)
            child->children[i + 2] = sibling->children[i];
    }

    for (int i = idx + 1; i < node->num_keys; ++i)
        node->keys[i - 1] = node->keys[i];

    for (int i = idx + 2; i <= node->num_keys; ++i)
        node->children[i - 1] = node->children[i];

    child->num_keys += sibling->num_keys + 1;
    node->num_keys--;
    free(sibling);
}

void fill(Node* node, int idx) {
    if (idx != 0 && node->children[idx - 1]->num_keys >= 2)
        borrowFromPrev(node, idx);
    else if (idx != node->num_keys && node->children[idx + 1]->num_keys >= 2)
        borrowFromNext(node, idx);
    else {
        if (idx != node->num_keys)
            merge(node, idx);
        else
            merge(node, idx - 1);
    }
}

void removeFromNonLeaf(Node* node, int idx) {
    int key = node->keys[idx];
    if (node->children[idx]->num_keys >= 2) {
        int pred = getPred(node, idx);
        node->keys[idx] = pred;
        deleteKey(node->children[idx], pred);
    }
    else if (node->children[idx + 1]->num_keys >= 2) {
        int succ = getSucc(node, idx);
        node->keys[idx] = succ;
        deleteKey(node->children[idx + 1], succ);
    }
    else {
        merge(node, idx);
        deleteKey(node->children[idx], key);
    }
}

void deleteKey(Node* node, int key) {
    int idx = findKey(node, key);

    if (idx < node->num_keys && node->keys[idx] == key) {
        if (node->is_leaf)
            removeFromLeaf(node, idx);
        else
            removeFromNonLeaf(node, idx);
    } else {
        if (node->is_leaf) {
            printf("A chave %d nao esta na arvore.\n", key);
            return;
        }

        bool flag = ((idx == node->num_keys) ? true : false);

        if (node->children[idx]->num_keys < 2)
            fill(node, idx);

        if (flag && idx > node->num_keys)
            deleteKey(node->children[idx - 1], key);
        else
            deleteKey(node->children[idx], key);
    }
}

void deleteTreeKey(Node** root, int key) {
    if (!*root) {
        printf("A arvore esta vazia.\n");
        return;
    }
    
    deleteKey(*root, key);
    
    if ((*root)->num_keys == 0) {
        Node* tmp = *root;
        if ((*root)->is_leaf)
            *root = NULL;
        else
            *root = (*root)->children[0];
        free(tmp);
    }
}

void freeTree(Node* node) {
    if (node != NULL) {
        if (!node->is_leaf) {
            for (int i = 0; i <= node->num_keys; i++) {
                freeTree(node->children[i]);
            }
        }
        free(node);
    }
}

int main() {
    Node* root = NULL;
    int opcao, valor;

    do {
        printf("\n--- Arvore 2-3-4 ---\n");
        printf("1 - Inserir valor\n");
        printf("2 - Buscar valor\n");
        printf("3 - Remover valor\n");
        printf("4 - Percorrer em ordem\n");
        printf("5 - Exibir arvore por niveis\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");
        
        if(scanf("%d", &opcao) != 1) { 
            while(getchar() != '\n'); 
            opcao = -1;
        }

        switch (opcao) {
            case 1:
                printf("Digite o valor para inserir: ");
                scanf("%d", &valor);
                insert(&root, valor);
                break;
            case 2:
                printf("Digite o valor para buscar: ");
                scanf("%d", &valor);
                if (search(root, valor)) {
                    printf("Valor %d encontrado na arvore.\n", valor);
                } else {
                    printf("Valor %d nao encontrado na arvore.\n", valor);
                }
                break;
            case 3:
                printf("Digite o valor para remover: ");
                scanf("%d", &valor);
                deleteTreeKey(&root, valor);
                break;
            case 4:
                printf("Percurso em ordem: ");
                if (root == NULL) printf("Arvore vazia.");
                else inOrder(root);
                printf("\n");
                break;
            case 5:
                printf("Exibicao por niveis:\n");
                levelOrder(root);
                break;
            case 0:
                printf("Encerrando programa e liberando memoria...\n");
                freeTree(root);
                root = NULL;
                break;
            default:
                printf("Opcao invalida! Tente novamente.\n");
        }
    } while (opcao != 0);

    return 0;
}