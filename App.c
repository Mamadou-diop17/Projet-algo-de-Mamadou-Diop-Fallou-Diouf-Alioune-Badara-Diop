#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

/*
==========================================================
    STRUCTURE OF A COURSE MATERIAL (SUPPORT)
==========================================================
*/

typedef struct Support {
    int id;
    char title[100];
    char module[50];
    char type[20];
    char teacher[50];
    char path[200];
    char date[20];
} Support;

/*
==========================================================
        LINKED LIST STRUCTURE
==========================================================
*/

typedef struct Node {
    Support data;
    struct Node* next;
} Node;

Node* head = NULL;

/*
==========================================================
        STACK (PILE) FOR HISTORY
==========================================================
*/

typedef struct StackNode {
    char action[200];
    struct StackNode* next;
} StackNode;

StackNode* stackTop = NULL;

/*
==========================================================
        QUEUE (FILE) FOR NOTIFICATIONS
==========================================================
*/

typedef struct QueueNode {
    char message[200];
    struct QueueNode* next;
} QueueNode;

QueueNode *front = NULL, *rear = NULL;

/*
==========================================================
        MYSQL CONNECTION
==========================================================
*/
MYSQL *conn;

/*
==========================================================
        STACK FUNCTIONS
==========================================================
*/
void push(char *msg) {
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    strcpy(newNode->action, msg);
    newNode->next = stackTop;
    stackTop = newNode;
}

void printStack() {
    StackNode* temp = stackTop;
    printf("\n--- ACTION HISTORY (STACK) ---\n");
    while (temp != NULL) {
        printf("%s\n", temp->action);
        temp = temp->next;
    }
}

/*
==========================================================
        QUEUE FUNCTIONS (NOTIFICATIONS)
==========================================================
*/
void enqueue(char *msg) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    strcpy(newNode->message, msg);
    newNode->next = NULL;
    if (rear == NULL) {
        front = rear = newNode;
        return;
    }
    rear->next = newNode;
    rear = newNode;
}

void printQueue() {
    QueueNode* temp = front;
    printf("\n--- NOTIFICATIONS (QUEUE) ---\n");
    while (temp != NULL) {
        printf("• %s\n", temp->message);
        temp = temp->next;
    }
}

/*
==========================================================
        LINKED LIST FUNCTIONS
==========================================================
*/

void insertEnd(Support s) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = s;
    newNode->next = NULL;

    if (head == NULL) {
        head = newNode;
        return;
    }
    Node* temp = head;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = newNode;
}

void printList() {
    Node* temp = head;
    printf("\n========== SUPPORT LIST ==========\n");
    while (temp != NULL) {
        printf("ID: %d | %s | %s | %s | %s\n",
               temp->data.id,
               temp->data.title,
               temp->data.module,
               temp->data.type,
               temp->data.teacher);
        temp = temp->next;
    }
}

/*
==========================================================
        MYSQL FUNCTIONS
==========================================================
*/

void connectDB() {
    conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, "localhost", "root", "", "supports", 0, NULL, 0)) {
        printf("MySQL connection failed: %s\n", mysql_error(conn));
        exit(1);
    }
}

void loadFromDB() {
    mysql_query(conn, "SELECT * FROM supports");
    MYSQL_RES *res = mysql_store_result(conn);
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res))) {
        Support s;
        s.id = atoi(row[0]);
        strcpy(s.title, row[1]);
        strcpy(s.module, row[2]);
        strcpy(s.type, row[3]);
        strcpy(s.teacher, row[4]);
        strcpy(s.path, row[5]);
        strcpy(s.date, row[6]);

        insertEnd(s);
    }
}

/*
==========================================================
        ADD A NEW SUPPORT
==========================================================
*/

void addSupport() {
    Support s;

    printf("\nEnter title: ");
    scanf(" %[^\n]", s.title);

    printf("Enter module: ");
    scanf(" %[^\n]", s.module);

    printf("Enter type: ");
    scanf(" %[^\n]", s.type);

    printf("Enter teacher: ");
    scanf(" %[^\n]", s.teacher);

    printf("Enter file path: ");
    scanf(" %[^\n]", s.path);

    printf("Enter date: ");
    scanf(" %[^\n]", s.date);

    char query[500];
    sprintf(query,
            "INSERT INTO supports (titre, module, type, enseignant, chemin, date_ajout) "
            "VALUES ('%s','%s','%s','%s','%s','%s')",
            s.title, s.module, s.type, s.teacher, s.path, s.date);

    if (mysql_query(conn, query) == 0) {
        enqueue("New support added.");
        push("You added a support.");

        printf("\nSupport added successfully.\n");
    } else {
        printf("Error: %s\n", mysql_error(conn));
    }
}

/*
==========================================================
        MAIN MENU
==========================================================
*/

void menu() {
    int choice;

    do {
        printf("\n========= MAIN MENU =========\n");
        printf("1. Show all supports\n");
        printf("2. Add new support\n");
        printf("3. Show history (stack)\n");
        printf("4. Show notifications (queue)\n");
        printf("0. Exit\n");
        printf("Your choice: ");

        scanf("%d", &choice);

        switch (choice) {
            case 1: printList(); break;
            case 2: addSupport(); break;
            case 3: printStack(); break;
            case 4: printQueue(); break;
            case 0: printf("Exiting...\n"); break;
        }

    } while (choice != 0);
}

/*
==========================================================
        MAIN FUNCTION
==========================================================
*/

int main() {
    connectDB();
    loadFromDB();
    menu();
    mysql_close(conn);
    return 0;
}