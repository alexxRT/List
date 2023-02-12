#ifndef LIST_H
#define LIST_H

#include <stdlib.h>


typedef int list_data_t;

typedef struct _list_elem
{
    struct _list_elem* next;
    struct _list_elem* prev;
    list_data_t data;
    int index;
    int status;
} list_elem;

typedef struct _list
{
    list_elem* buffer;
    list_elem* free_head;
    int size;
    int capacity;
} my_list;

void InitDumpFile ();
void DestroyDumpFile ();

int ListInit     (my_list* list, size_t elem_num);
int ListDestroy  (my_list* list);

int ListAdd (my_list* list, list_data_t data);
int ListPop (my_list* list, list_data_t* data);

int ListTextDump (my_list* list);
void PrintList   (my_list* list, int list_type);


#endif