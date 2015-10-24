//
//  main.cpp
//  myAppStore
//
//  Created by Alex McIntosh on 9/29/15.
//  Copyright © 2015 Alex McIntosh. All rights reserved.
//

#include <algorithm>
#include <float.h>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <cstring>
#include <sstream>

using namespace std;

#define   CAT_NAME_LEN   50
#define   APP_NAME_LEN   80
#define   VERSION_LEN   25
#define   UNIT_SIZE   3


struct app_info{
    char category[ CAT_NAME_LEN ]; // Name of category
    char app_name[ APP_NAME_LEN ]; // Name of the application
    char version[ VERSION_LEN ]; // Version number
    float size; // Size of the application
    char units[ UNIT_SIZE ]; // GB or MB
    float price; // Price in $ of the application
};

struct tree{ // A 2-3 tree
    struct app_info small; // Information about the application
    struct app_info medium;
    struct app_info large;
    struct tree *left;  // Pointer to the left subtree
    struct tree *mid; // Pointer to the middle subtree
    struct tree *right;  // Pointer to the right subtree
    struct tree *parent;
};

struct categories{
    char category[ CAT_NAME_LEN ]; // Name of category
    struct tree *root; // Pointer to root of search tree for this category
};

struct hash_table_entry{
    char app_name[ APP_NAME_LEN ]; // Name of the application
    struct tree *app_node; // Pointer to node in tree containing the application information
};

//********************** Forward Declarations **********************
void newCats(categories Category[]);
void Insertion(tree *&node, app_info app, hash_table_entry table[]);
void createTable(hash_table_entry table[], int position, tree app);
void Operation(string command, hash_table_entry table[], categories Category[]);
int findPrime(int num);
void findApp(string command, hash_table_entry table[]);
void findCat(string, categories Category[]);
void findFree(float price, tree *node);
void priceRange(float low, float high, tree *node);
void appRange(char low, char high, tree *node);
void deleteHash(string command, hash_table_entry table[]);
void printName(tree *&node);
void Print(tree *node);

//********************** Global Vars **********************
int hashSize;   //size of hashtable for reference
int catSize;    //number of categories to add for reference
int queries;    //number of operations allowed before terminating
bool found = false;

int main() {
    int i = 0;  //iterator
    int numApps;   //number of apps to add
    string command; //string for queries
    
    cin >> catSize; //read in number categories to add
    cin.ignore();   //flush stream
    categories* Category = new categories[catSize];   //initialize array of categories
    newCats(Category); //call to get input of categories
    
    //Check categories entered
    /*
    for (int j = 0; j < catSize; j++){
        cout << Category[j].category << endl;
    }
     */
    
    cin >> numApps; //read in number of apps to add
    cin.ignore();
    hashSize = findPrime(2*numApps);    //initialize hashsize
    hash_table_entry* table = new hash_table_entry[hashSize];   //initialize hashtable
    while (i < numApps){    //stops adding in apps after quota met
        app_info newApp;
        cin.getline(newApp.category, CAT_NAME_LEN);  //read in information of app to be inserted into BST
        cin.getline(newApp.app_name, APP_NAME_LEN);
        cin.getline(newApp.version, VERSION_LEN);
        cin >> newApp.size;
        cin.ignore();
        cin.getline(newApp.units, UNIT_SIZE);
        cin >> newApp.price;
        cin.ignore();
        for (int j = 0; j < catSize; j++){  //searches if category exists
            if (strcmp(Category[j].category, newApp.category) == 0) {
                Insertion(Category[j].root, newApp, table);   //if found insert
            }
        }
        i++;
    }
    
    cin >> queries;
    cin.ignore();
    while (queries > 0) {   //stopping condition for queries
        Operation(command, table, Category);    //call for queries
    }
    
    delete [] Category; //delete dynamically allocated structs arrays
    delete [] table;
    system("pause");
    return 0;
}

//fills array of categories from user input
void newCats(categories Category[]){
    int i = 0;
    while (i < catSize){    //stopping condition to insert new categories
        tree *newTree = new tree;
        newTree = NULL;
        cin.getline(Category[i].category, CAT_NAME_LEN);
        Category[i].root = newTree;
        delete newTree;
        i++;
    }
}

//inserts app into BST
void Insertion(tree *&node, app_info app, hash_table_entry table[]) {
    int value = 0;
    if (node == NULL) {
        node = new tree;
        node->small = app;
        node->left = new tree;
        node->left = NULL;
        node->right = new tree;
        node->right = NULL;
        node->mid = new tree;
        node->mid = NULL;
        delete node->left;
        delete node->mid;
        delete node->right;
        delete node;
    }
    else if (app.app_name[0] > node->small.app_name[0] && ) { //search for open position in tree
        for (int i = 0; i < strlen(node->small.app_name); i++){  //get ACII value for position to insert into hash
            value += app.app_name[i];
        }
        delete node;
        int position = value % hashSize-1;
        createTable(table, position, *node);
    }
    else if ((int) node->small.app_name[0] > (int) app.app_name[0]) {   //if name is greater than root insert right
        Insertion(node->left, app, table);
    }
    else {  //else insert left
        Insertion(node->right, app, table);
    }
}

//insert keys into table
void createTable(hash_table_entry table[], int position, tree app){
    if (table[position].app_node == NULL && position <= hashSize) { //search for open position in hashtable
        table[position].app_node = &app;
        strcpy(table[position].app_name, app.small.app_name);
    }
    else if (position == hashSize && table[position].app_node != NULL){ //if hashtbale reaches end, jump to beginning
        position = 0;
        createTable(table, position, app);
    }
    else {  //if postion taken, jump to next position
        position++;
        createTable(table, position, app);
    }
}


void Operation(string command, hash_table_entry table[], categories Category[]){
    stringstream range;
    getline(cin, command);
    if (command.find("find app") != string::npos) { //if string contains key word, do operation
        command.erase(0 , 10);  //trim string to pass
        command.erase(command.begin()+command.length()-1);
        findApp(command, table);
        found = false;
    }
    else if (command.find("find price free") != string::npos) {
        command.erase(0 , 16);
        for (int i = 0; i < catSize; i++) {
            findFree(0.00, Category[i].root);
        }
        if (!found) {
            cout << "No free applications found."  << endl;
        }
        found = false;
    }
    else if (command.find("find category") != string::npos) {
        command.erase(0 , 15);
        command.erase(command.begin()+command.length()-1);
        findCat(command, Category);
        found = false;
    }
    else if (command.find("range") != string::npos) {
        command.erase(0 , 6);
        command.erase(remove(command.begin(), command.end(),'\"'), command.end());
        for (int i = 0; i < catSize; i++) {
            if (command.find(Category[i].category) != string::npos) {
                string toDelete = string (Category[i].category);    //trim string
                size_t length = toDelete.length();
                command.erase(0, length+1);
                string empty;
                float low, high;
                char lo, hi;
                stringstream range(command);
                if (command.find("app") != string::npos) {
                    range >> empty >> lo >> hi;
                    appRange(lo, hi, Category[i].root);
                    if (!found){
                        cout << "No applications found for given range." << endl;
                    }
                }
                else {
                    range >> empty >> low >> high;
                    priceRange(low, high, Category[i].root);
                    if (!found){
                        cout << "No applications found for given range." << endl;
                    }
                }
            }
        }
        found = false;
    }
    else if (command.find("delete") != string::npos) {
        command.erase(0 , 6);
        command.erase(remove(command.begin(), command.end(),'\"'), command.end());
        for (int i = 0; i < catSize; i++) {
            if (command.find(Category[i].category) != string::npos) {
                string toDelete = string (Category[i].category);    //trim string
                size_t length = toDelete.length();
                command.erase(0, length+1);
                cout << "Not ready yet" << endl;
                deleteHash(command, table);
            }
        }
        found = false;
    }
    else {
        cout << "Invalid command" << endl;
    }
    queries--;
}

//creates hashtable size
int findPrime(int num){
    int hashSize = num;
    bool prime = true;  //assumed prime
    while (hashSize >= 1) { //stopping condition
        for (int j = 2; j < sqrt(hashSize)+1; j++) {    //only have to search up to sqrt of number to find prime
            if (hashSize % j == 0) {
                prime = false;
            }
        }
        if (prime) {
            return hashSize;
            break;
        }
        prime = true;
        hashSize++;
    }
    return hashSize;
}

void findApp(string command, hash_table_entry table[]){
    int value = 0;
    int pos = 0;
    int i = 0;
    for (int j = 0; j < command.size(); j++) { //get position
        value += command[j];
    }
    pos = value % hashSize-1;

    while (i < hashSize-1) {
        string app = string(table[pos].app_name);
        if (command.find(app) != string::npos) {
            Print(table[pos].app_node);
            found = true;
            break;
        }
        else if (pos == hashSize-1 && command.find(app) == string::npos){
            pos = 0;
            i++;
        }
        else {
            pos++;
            i++;
        }
    }
    
    if (!found){
        cout << "Application not found." << endl;
    }
}

void findCat(string command, categories Category[]){
    for (int i = 0; i < catSize; i++) {
        string cat = string(Category[i].category);
        if (command.find(cat) != string::npos) {
            if (Category[i].root) { //check to see if apps have been inserted into the category
                 printName(Category[i].root);
            }
            else {
                cout << "No existing apps for this category" << endl;
            }
            found = true;
            break;
        }
    }
    
    if (!found){
        cout << "Category not found." << endl;
    }
}

void findFree(float price, tree *node){
    if (node) {
        findFree(price, node->left);
        if (node->a1.price == 0.00) {
            found = true;
            cout << node->a1.app_name << endl;
        }
        findFree(price, node->right);
    }
}

void priceRange(float low, float high, tree *node){
    if (node) {
        priceRange(low, high, node->left);
        if ((node->a1.price >= low) && (node->a1.price <= high)) {
            found = true;
            cout << node->a1.app_name << endl;
        }
        priceRange(low, high, node->right);
    }
    
}

void appRange(char low, char high, tree *node){
    int value = 0;
    char lowlow = tolower(low);
    char hihi = tolower(high);
    if (node) {
        appRange(low, high, node->left);
        value = (int) tolower(node->a1.app_name[0]);
        if (((int) lowlow <= value) && (value < (int) hihi)) {
            found = true;
            cout << node->a1.app_name << endl;
        }
        appRange(low, high, node->right);
    }
}

void deleteHash(string command, hash_table_entry table[]){
    int value = 0;
    int pos = 0;
    int i = 0;
    for (int i = 0; i < command.size(); i++) { //get position
        value += command[i];
    }
    pos = value % hashSize-1;
    
    tree temp;
    while (i < hashSize-1) {
        string app = string(table[pos].app_name);
        if (command.find(app) != string::npos) {
            table[pos].app_node = &temp;
            strcpy(table[pos].app_name, "DELETED");
            found = true;
            cout << "Deleted." << endl;
            break;
        }
        else if (pos == hashSize && command.find(app) == string::npos){
            pos = 0;
            i++;
        }
        else {
            pos++;
            i++;
        }
    }
}


//prints out app a1
void Print(tree *node){
    if (node) {
        cout << "CATEGORY: " << node->a1.category << endl;
        cout << "APP NAME: " << node->a1.app_name << endl;
        cout << "VERSION: " << node->a1.version << endl;
        cout << "SIZE: " << node->a1.size << " " << node->a1.units << endl;
        cout << "PRICE: " << node->a1.price << endl;
    }
}

//indorder traversal of names to be printed
void printName(tree *&node){
    if (node) {
        printName(node->left);
        cout << node->a1.app_name << endl;
        printName(node->right);
    }
}

bool isTwoNode(){
    
    return true;
}
