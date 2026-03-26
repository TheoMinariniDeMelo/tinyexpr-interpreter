#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

struct Node{
    struct Node* next_node;
    void* obj;
    int magic;
};

typedef struct {
    struct Node* inode;
    size_t size;
}List;

List* init_list(void);

void push_value(List* list, void* value);

void pop_value(List* list);

void* get_value(List* list, int index);

void set_value(List *list, int index, void* value);

void free_list(List* list);
