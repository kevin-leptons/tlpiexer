/*
SYNOPSIS

    Solution for exercise 30-02.

REQUIREMENT

    Implement a set of thread-safe functions that update and search and 
    umbalanced binary tree. This binary should include functions (with the 
    obvious purposes) of the following form.

        initialize(tree);
        add(tree, char *key, void *value);
        delete(tree, char *key);
        boolean bookup(char *key, void **value);

    In the above prototypes, tree is a structure taht points to the root of the
    tree (you will need to define a suitable structure for this purpose).
    Each element of the tree holds a key-value pair. You will also need to 
    define the structure for each element to include a mutex that protects that
    element so that only one thread at a time can access it. The initalize(),
    add(), and lookup() fucntions are reaalatively simple to implement.
    The delete() operation requires a little more effort.

    Removing the need to maintain a balanced tree greatly simplifies the
    looking requirements of the implementation, but carries the risk cartain 
    patterns of input would result in a tree that performs poorly. Maintaining
    a balanced tree neccessitataes moving nodes between subtrees during the 
    add() and delete() operations, which requires much more compelex locking
    strategies.

SOLUTION

    Modify interface.

        struct tree_elem;
        struct tree;
        int tree_init(struct tree *root);
        int tree_add(struct tree *root, char *key, void *value);
        int tree_del(struct tree *root, char *key);
        int tree_look(struct tree *root, char *key, void **value);

    Implement same as hints of requirements.

USAGE

    $ ./dest/bin/30-02 -k name -v kevin
    key=a; value=first-value
    key=b; value=second-value
    key=c; value=third-value
    key=name; value=kevin

    $ ./dest/bin/30-02 -d a
    key=b; value=second-value
    key=c; value=third-value

    $ ./dest/bin/30-2 -l b
    tree_lookup: key=b; value=second-value
    key=a; value=first-value
    key=b; value=second-value
    key=c; value=third-value

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1
#define TREE_KEY_MAX 64

// spec     : element of tree, contains data of element
struct tree_elem
{
    char *key;
    void *value;
    struct tree_elem *prev, *next;
};

// spec     : contains maintain data for root of tree
struct tree
{
    struct tree_elem *first, *last;
    pthread_mutex_t mutex;
};

// spec     : initialize root of tree
// arg      : 
//  - root: point to root of tree
// ret      : 
//  - 0 on success
//  - -1 on error
int tree_init(struct tree *root);

// spec     : add an element with key and value to tree
// arg      :
//  - root: point to root of tree
//  - key: key of element
//  - value: value of element
// ret      :
//  - 0 on success
//  - -1 on error
int tree_add(struct tree *root, char *key, void *value);

// spec     : remove element from tree
// arg      :
//  - root: point to root of tree
//  - key: key of element
// ret      :
//  - 0 on success
//  - -1 on error
int tree_del(struct tree *root, char *key);

// spec     : find element coresponding with key
// arg      :
//  - root: point to root of tree
//  - key: key to find
//  - value: list of value
// ret      :
//  - 0 on success
//  - -1 on error
//  - 1 on not found
int tree_look(struct tree *root, char *key, void **value);

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char **argv)
{
    // variables
    int opt;
    struct tree root;
    char *key;
    char *value;
    void *value_found;
    char op;
    struct tree_elem *elem;

    // init
    op = 0;
    value_found = NULL;

    // init tree
    if (tree_init(&root) == -1) {
        fprintf(stderr, "error: tree_init\n");
        return EXIT_FAILURE;
    }

    // add first element
    // skip for error for simple code
    key = malloc(TREE_KEY_MAX);
    value = malloc(32);
    strcpy(key, "a");
    strcpy((char*)value, "first-value");
    if (tree_add(&root, key, value) == -1) {
        fprintf(stderr, "error: tree_add\n");
    }

    // add second element
    // skip for error for simple code
    key = malloc(TREE_KEY_MAX);
    value = malloc(32);
    strcpy(key, "b");
    strcpy((char*)value, "second-value");
    if (tree_add(&root, key, value) == -1) {
        fprintf(stderr, "error: tree_add\n");
    }

    // add third element
    // skip for error for simple code
    key = malloc(TREE_KEY_MAX);
    value = malloc(32);
    strcpy(key, "c");
    strcpy((char*)value, "third-value");
    if (tree_add(&root, key, value) == -1) {
        fprintf(stderr, "error: tree_add\n");
    }

    // parse options
    while ((opt = getopt(argc, argv, "hk:v:d:l:")) != -1) {
        switch (opt) {
            case 'k':
                op = 'a';
                key = optarg;
                break;
            case 'v':
                value = optarg;
                break;
            case 'd':
                op = 'd';
                key = optarg;
                break;
            case 'l':
                op = 'l';
                key = optarg;
                break;
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default:
                showhelp();
                return EXIT_FAILURE;
        }
    }

    // verify no arguments has provided
    if (optind > argc || !(op == 'a' || op == 'd' || op == 'l')) {
        showhelp();
        return EXIT_FAILURE;
    }

    // do actions
    switch (op) {
        case 'a':
            if (tree_add(&root, key, (void*)value) == -1) {
                fprintf(stderr, "error: tree_add\n");
                return EXIT_FAILURE;
            }
            break;
        case 'd':
            if (tree_del(&root, key) == -1) {
                fprintf(stderr, "error: tree_del\n");
                return EXIT_FAILURE;
            }
            break;
        case 'l':
            if (tree_look(&root, key, &value_found) == -1) {
                fprintf(stderr, "error: tree_look\n");
                return EXIT_FAILURE;
            }

            // show lookup values
            if (value_found == NULL)
                printf("tree_lookup: not found\n");
            else
                printf("tree_lookup: key=%s; value=%s\n", key, 
                        (char*)value_found);
    }

    // show tree elements
    pthread_mutex_lock(&root.mutex);
    for (elem = root.first; elem != NULL; elem = elem->next) {
        printf("key=%s; value=%s\n", elem->key, (char*)elem->value);
    }
    pthread_mutex_unlock(&root.mutex);

    // no function allow free tree so no need to free memory by hand here 
    // heap will free by kernel when process exit

    // success
    return EXIT_SUCCESS;
}

int tree_init(struct tree *root)
{
    if (pthread_mutex_init(&root->mutex, NULL) == -1)
        return RET_FAILURE;
    root->first = NULL;
    root->last = NULL;

    // success
    return RET_SUCCESS;
}

int tree_add(struct tree *root, char *key, void *value)
{
    // variable
    struct tree_elem *elem;

    // get control to tree from other thread
    pthread_mutex_lock(&root->mutex);

    // find exist key
    // do not add exist key
    for (elem = root->first; elem != NULL; elem = elem->next) {
        if (strcmp(elem->key, key) == 0)
            return RET_FAILURE;
    }

    // prepare new element
    elem = malloc(sizeof(struct tree_elem));
    if (elem == NULL)
        return RET_FAILURE;
    elem->key = key;
    elem->value = value;
    elem->prev = NULL;
    elem->next = NULL;

    // add first element
    if (root->first == NULL) {
        root->first = elem;
        root->last = elem;
    } else {
        root->last->next = elem;
        elem->prev = root->last;
        root->last = elem;
    }

    // back control to tree to other thread
    pthread_mutex_unlock(&root->mutex);

    // success
    return RET_SUCCESS;
}

int tree_del(struct tree *root, char *key)
{
    // variables
    struct tree_elem *elem;
    int del = 0;

    // init
    del = 0;

    // get control to tree from other thread
    pthread_mutex_lock(&root->mutex);

    // walk on tree and find key
    for (elem = root->first; elem != NULL; elem = elem->next) {

        // skip for element not match key
        if (strcmp(elem->key, key) != 0)
            continue;

        // case element is firt
        if (elem->prev == NULL) {
            root->first = elem->next;
            if (root->first != NULL)
                root->first->prev = NULL;
        } 

        // case element is last
        if (elem->next == NULL) {
            root->last = elem->prev;
            if (root->last != NULL)
                root->last->next = NULL;
        }

        // case element is middle
        if (elem->prev != NULL && elem->next != NULL) {
            elem->prev->next = elem->next;
            elem->next->prev = elem->prev;
        }

        // free memory of element
        free(elem->key);
        free(elem->value);

        // mark delete
        del = 1;

        // modify element pointer
        // so it will correct point to prev element
        elem = elem->prev;
        if (elem == NULL)
            break;
    }

    // back control to tree to other thread
    pthread_mutex_unlock(&root->mutex);

    // not found key to delete
    if (!del)
        return RET_FAILURE;

    // success
    return RET_SUCCESS;
}

int tree_look(struct tree *root, char *key, void **value)
{
    // variables
    struct tree_elem *elem;

    // verify value is null, this function will allocate memory for that
    if (*value != NULL)
        return RET_FAILURE;

    // get control to tree from other thread
    pthread_mutex_lock(&root->mutex);

    // walk though tree and find key
    for (elem = root->first; elem != NULL; elem = elem->next) {

        // skip for element not match key
        if (strcmp(elem->key, key) != 0)
            continue;

        // point pointer to data
        *value = elem->value;

        // stop find
        break;
    }

    // back control to tree to other thread
    pthread_mutex_unlock(&root->mutex);

    // not found
    if (*value == NULL)
        return 1;

    // success
    return RET_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "30-2";

    printf("\nUSAGE:\n\n");
    printf("%s -k key -v value\n", exename);
    printf("%s -d key\n", exename);
    printf("%s -l key\n", exename);
    printf("%s -h\n\n", exename);

    printf("OPTIONS:\n\n");
    printf("-k: set element of tree by key\n");
    printf("-v: value of element to set by key\n");
    printf("-d: delete element match with key\n");
    printf("-l: find element match with key\n\n");
}
