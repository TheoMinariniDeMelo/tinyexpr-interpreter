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

List* init_list(void){
    struct Node* node = malloc(sizeof(struct Node));
    node->magic = 1;
    node->obj = NULL;
    node->next_node = NULL;
    List* list = malloc(sizeof(List));
    list->inode = node;
    list->size = 0;
    return list;
}

void push_value(List* list, void* value){
    struct Node* node = malloc(sizeof(struct Node));
    node->obj = value;
    node->magic = 0;
    node->next_node = NULL;
    size_t size = list->size;
    struct Node* current_node = list->inode;
    for(int i = 0; i < size; i++){
        current_node = current_node->next_node;
    }
    current_node->next_node = node;
    list->size += 1;
}
void pop_value(List* list){
   if(list->size == 0) return;

    struct Node* current_node = list->inode;
    size_t size = list->size;

    for(int i = 0; i < size - 1; i++){
        current_node = current_node->next_node;
    }

    struct Node* to_remove = current_node->next_node;
    current_node->next_node = to_remove->next_node; // Será NULL se for o último

    free(to_remove);
    list->size -= 1;
}

void* get_value(List* list, int index){
    assert(index < list->size);
    struct Node* node = list->inode->next_node;
    for(int i = 0; i < index; i++){
        node = node->next_node;
    }
    return node->obj;
}
void set_value(List *list, int index, void* value){
    assert(index < list->size);
    struct Node* node = list->inode->next_node;
    for(int i = 0; i < index; i++){
        node = node->next_node;
    }
    node->obj = value;
}
void free_list(List* list){
    struct Node* next_node = list->inode;
    for(int i = 0; i <= list->size; i++){
        struct Node* node = next_node;
        next_node = node->next_node;
        free(node);
    }
    free(next_node);
    free(list);
}
