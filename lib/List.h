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
    size_t size;
    size_t capacity;
    size_t min_capacity;
} my_list;


enum NODE_SATUS
{
    FREE    = 1,
    ENGAGED = 2, 
    MASTER  = 3 
};

typedef enum ERROR_CODES
{
    SUCCESS          = 0,
    LIST_NULL        = 1,
    DATA_NULL        = 2,
    LIST_OVERFLOW    = 3,
    LIST_UNDERFLOW   = 4,
    CAPACITY_INVALID = 5,
    WRONG_SIZE       = 6,
    BROAKEN_LOOP     = 7,
    NULL_LINK        = 8,
    HEAD_DELEATE     = 9,
    WRONG_INDX       =-2, //these error codes can be normaly returned by functions, so they have "imposble" value 
    WRONG_ID         =-1 //
} LIST_ERR_CODE;


LIST_ERR_CODE ListInit     (my_list* list, size_t elem_num);
LIST_ERR_CODE ListDestroy  (my_list* list);

LIST_ERR_CODE ListInsertRight (my_list* list, size_t id, list_data_t data);
LIST_ERR_CODE ListInsertLeft  (my_list* list, size_t id, list_data_t data);
LIST_ERR_CODE ListDelete (my_list* list, size_t id);

LIST_ERR_CODE ListInsertIndex (my_list* list, int index, list_data_t data);
LIST_ERR_CODE ListDeleteIndex (my_list* list, int index);

LIST_ERR_CODE ListResize (my_list* list);

list_elem* GetElem (my_list* list, size_t id);

LIST_ERR_CODE MakeListGreatAgain (my_list* list);

#endif