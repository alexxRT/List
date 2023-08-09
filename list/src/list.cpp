#include "memory.h"
#include "list.h"
#include "list_debug.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


#define CHECK_UP( lst_ptr ) lst_ptr->size == lst_ptr->capacity

#define CHECK_DOWN( lst_ptr ) lst_ptr->size < (lst_ptr->capacity) / 4 && \
lst_ptr->capacity / 2 >= lst_ptr->min_capacity

//TO_DO:
// PREV(el) NEXT(el) add macroses to make code more readable

int        InsertFreeList (my_list* list, list_elem* elem);
list_elem* TakeFreeList   (my_list* list);

//------------------------------------------------------INIT/DESTROY---------------------------------------------------//

LIST_ERR_CODE ListInit (my_list* list, size_t elem_num)
{
    assert (list != NULL);

    list->buffer    = CALLOC (elem_num + 1, list_elem);
    list->free_head = list->buffer;
    list->capacity  = elem_num;
    list->min_capacity = elem_num;
    list->size     = 0;

    list->buffer[0].status = MASTER;
    list->buffer[0].next = list->buffer;
    list->buffer[0].prev = list->buffer;

    for (size_t i = 1; i <= elem_num; i++)
    {
        list->buffer[i].index = i;
        int err_code = InsertFreeList (list, list->buffer + i);
    }

    LIST_VALIDATE (list);

    return SUCCESS;
}

LIST_ERR_CODE ListDestroy (my_list* list)
{
    LIST_VALIDATE (list);

    list->capacity  = 0;
    list->size      = 0;
    list->free_head = NULL;

    FREE (list->buffer);

    return SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------//

list_elem* GetElem (my_list* list, size_t id) //Gives elem by his order number
{
    list_elem* to_get = list->buffer;

    for (size_t i = 0; i < id; i ++)
    {
        to_get = to_get->next;
    }

    return to_get;
}

//------------------------------------------INSERT/DELETE TO/FROM LOGIC ELEM'S POSITION--------------------------------//

int InsertRight (list_elem* dest, list_elem* elem);
int DeleteRight (list_elem* dest);
int InsertLeft (list_elem* dest, list_elem* elem);
int DeleteLeft (list_elem* dest);


LIST_ERR_CODE ListInsertRight (my_list* list, size_t id, list_data_t data)
{
    LIST_VALIDATE (list);

    ListResize (list);

    list_elem* new_to_add = TakeFreeList (list);
    new_to_add->status = ENGAGED;
    list->size ++;

    new_to_add->data = data;

    list_elem* dest = GetElem (list, id);
    InsertRight (dest, new_to_add);

    LIST_VALIDATE (list);

    return SUCCESS;
}


LIST_ERR_CODE ListInsertLeft (my_list* list, size_t id, list_data_t data)
{
    LIST_VALIDATE (list);

    ListResize (list);

    list_elem* new_to_add = TakeFreeList (list);
    new_to_add->status = ENGAGED;
    list->size ++;

    new_to_add->data = data;

    list_elem* dest = GetElem (list, id);
    InsertLeft (dest, new_to_add);

    LIST_VALIDATE (list);

    return SUCCESS;
}

LIST_ERR_CODE ListDelete (my_list* list, size_t id)
{
    LIST_VALIDATE (list);

    if (list->capacity == 0)
        return LIST_UNDERFLOW;
    
    list_elem* del_elem = GetElem (list, id); 

    if (del_elem == list->buffer)
        return HEAD_DELEATE;

    DeleteRight (del_elem->prev);
    list->size --;

    InsertFreeList (list, del_elem);
    ListResize(list);

    LIST_VALIDATE (list);

    return SUCCESS;
}



int InsertRight (list_elem* dest, list_elem* elem)
{
    list_elem* old_elem = dest->next;

    dest->next     = elem;
    elem->prev     = dest;
    elem->next     = old_elem;
    old_elem->prev = elem;

    return SUCCESS;
}

int DeleteRight (list_elem* dest)
{
    list_elem* del_elem = dest->next;
    dest->next = del_elem->next;
    del_elem->next->prev = dest;

    del_elem->prev = NULL;
    del_elem->next = NULL;

    return SUCCESS;
}

int InsertLeft (list_elem* dest, list_elem* elem)
{
    list_elem* old_elem = dest->prev;

    dest->prev = elem;
    elem->next = dest;
    elem->prev = old_elem;
    old_elem->next = elem;

    return SUCCESS;
}

int DeleteLeft (list_elem* dest)
{
    list_elem* del_elem = dest->prev;
    dest->prev = del_elem->prev;
    del_elem->prev->next = dest;

    del_elem->next = NULL;
    del_elem->prev = NULL;

    return SUCCESS;
}


list_elem* TakeFreeList (my_list* list)
{
    assert (list            != NULL);
    assert (list->free_head != NULL);

    list_elem* head = list->free_head;

    if (head->next == head) //only head left
    {
        list->free_head = list->buffer;
        head->next = NULL;
        head->prev = NULL;

        return head;
    }
    else 
    {
        list_elem* pop_elem = head->next;
        DeleteRight (head);

        return pop_elem;
    }

    return NULL;
}


int InsertFreeList (my_list* list, list_elem* elem)
{
    assert (list != NULL);
    assert (elem != NULL);

    elem->status = FREE;
    elem->data = 0;

    if (list->free_head == list->buffer) // no free elems
    {
        list->free_head = elem;   //initing free head
        elem->next = elem;        //cycling the list
        elem->prev = elem;
    }
    else
        InsertRight (list->free_head, elem);

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------------//




//----------------------------------------INSERT/DELETE BY ELEM'S INDEX-----------------------------------//

int DeleteFreeHead (my_list* list);
int DeleteIndex (my_list* list, int indx);
list_elem* TakeFreeList_indx (my_list* list, int indx);


LIST_ERR_CODE ListInsertIndex (my_list* list, size_t index, list_data_t data)
{
    LIST_VALIDATE (list);

    if (index > list->capacity)
        return WRONG_INDX;

    ListResize (list);

    list_elem* new_to_add = TakeFreeList_indx (list, index);
    new_to_add->status = ENGAGED;
    list->size ++;

    new_to_add->data = data;

    InsertLeft (list->buffer, new_to_add);

    LIST_VALIDATE (list);

    return SUCCESS;
}

LIST_ERR_CODE ListDeleteIndex (my_list* list, size_t index)
{
    LIST_VALIDATE (list);
    
    if (index > list->capacity ||
        list->buffer[index].status == FREE)
        return WRONG_INDX;

    if (list->capacity == 0)
        return LIST_UNDERFLOW;

    if (index == 0)
        return HEAD_DELEATE;
    
    list_elem* del_elem = list->buffer + index; 

    DeleteRight (del_elem->prev);
    list->size --;

    InsertFreeList (list, del_elem);
    ListResize(list);

    LIST_VALIDATE (list);

    return SUCCESS;
}

int DeleteFreeHead (my_list* list)
{
    list_elem* old_head = list->free_head;
    list_elem* new_head = list->free_head->next;

    new_head->prev = old_head->prev;
    old_head->prev->next = new_head;
    list->free_head = new_head;

    old_head->next = NULL;
    old_head->prev = NULL;

    return SUCCESS;
}

int DeleteIndex (my_list* list, int indx)
{
    list_elem* elem = list->buffer + indx;

    elem->prev->next = elem->next;
    elem->next->prev = elem->prev;

    elem->next = NULL;
    elem->prev = NULL;

    return SUCCESS;
}

list_elem* TakeFreeList_indx (my_list* list, int indx)
{
    list_elem* elem = list->buffer + indx;
    assert (elem->status == FREE);

    if (elem->next == elem) //only head left
    {
        list->free_head = NULL;
        elem->next = NULL;
        elem->prev = NULL;

        return elem;
    }

    else if (elem == list->free_head)
        DeleteFreeHead (list);

    else 
        DeleteIndex (list, indx);

    return elem;
}

//------------------------------------------------------------------------------------------------//


//---------------------------------------------LINIRIAZATION---------------------------------------//

LIST_ERR_CODE MakeListGreatAgain (my_list* list)
{
    LIST_VALIDATE (list);

    my_list new_list = {};
    ListInit (&new_list, list->capacity);

    int indx = 1;
    list_elem* head = list->buffer;
    list_elem* elem = head->next;

    while (elem != head)
    {
        list_data_t data = elem->data;
        ListInsertIndex (&new_list, indx, data);

        elem = elem->next;
        indx ++;
    }

    list_elem* old_buffer = list->buffer;
    list->buffer = new_list.buffer;
    list->free_head = new_list.free_head;

    FREE (old_buffer);

    LIST_VALIDATE (list);

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------//



//---------------------------------------------------------------------------------------------------//



//------------------------------------------RESIZE FUNCTIONS-----------------------------------------//

LIST_ERR_CODE ResizeUp (my_list* list)
{
    LIST_VALIDATE (list);

    my_list new_list = {};
    ListInit (&new_list, 2*list->capacity);

    for (size_t i = 1; i <= list->capacity; i++)
    {
        list_elem* elem = list->buffer + i;
        list_data_t data = elem->data;

        ListInsertIndex (&new_list, i, data);
    }

    list->capacity = new_list.capacity;
    list_elem* old_buffer = list->buffer;
    list->buffer = new_list.buffer;
    list->free_head = new_list.free_head;

    FREE (old_buffer);

    LIST_VALIDATE (list);

    return SUCCESS;
}

LIST_ERR_CODE ResizeDown (my_list* list)
{
    LIST_VALIDATE (list);

    my_list new_list = {};
    ListInit (&new_list, list->capacity / 2);

    for (size_t i = 1; i <= list->size; i ++)
    {
        list_elem* elem = list->buffer + i;
        list_data_t data = elem->data;

        ListInsertIndex (&new_list, i, data);
    }

    list->capacity = new_list.capacity;
    list_elem* old_buffer = list->buffer;
    list->buffer = new_list.buffer;
    list->free_head = new_list.free_head;

    FREE (old_buffer);

    LIST_VALIDATE (list);

    return SUCCESS;
}

LIST_ERR_CODE ListResize (my_list* list)
{
    LIST_VALIDATE (list);

    if (CHECK_UP(list))
    {
        MakeListGreatAgain (list);
        ResizeUp (list);
    }
    else if (CHECK_DOWN(list))
    {
        MakeListGreatAgain (list);
        ResizeDown (list);
    }

    LIST_VALIDATE (list);

    return SUCCESS;
}

//----------------------------------------------------------------------------------------------------------------//
