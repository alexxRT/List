#include "memory.h"
#include "list.h"
#include "list_debug.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


#define CHECK_UP( lst_ptr ) lst_ptr->size == lst_ptr->capacity
#define CHECK_DOWN( lst_ptr ) lst_ptr->size < (lst_ptr->capacity) / 4 && \
lst_ptr->capacity / 2 >= lst_ptr->min_capacity

#define ELEM( lst, pos ) (lst->buffer  + pos)
#define NEXT( lst, pos ) (lst->buffer + (lst->buffer + pos)->next)
#define PREV( lst, pos ) (lst->buffer + (lst->buffer + pos)->prev) 

static const int HEAD_ID = 0;


int        InsertFreeList (my_list* list, size_t elem);
size_t     TakeFreeList   (my_list* list);

//------------------------------------------------------INIT/DESTROY---------------------------------------------------//

LIST_ERR_CODE ListInit (my_list* list, size_t elem_num)
{
    assert (list != NULL);

    list->buffer = CALLOC (elem_num + 1, list_elem);
    list->free_head_id = HEAD_ID;
    list->capacity  = elem_num;
    list->min_capacity = elem_num;
    list->size     = 0;

    list->buffer[0].status = NODE_STATUS::MASTER;
    list->buffer[0].next = HEAD_ID;
    list->buffer[0].prev = HEAD_ID;

    for (size_t i = 1; i <= elem_num; i++)
    {
        list->buffer[i].index = i;
        int err_code = InsertFreeList (list, i);
    }

    LIST_VALIDATE (list);

    return SUCCESS;
}

LIST_ERR_CODE ListDestroy (my_list* list)
{
    LIST_VALIDATE (list);

    list->capacity  = 0;
    list->size      = 0;
    list->free_head_id = 0;

    FREE (list->buffer);

    return SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------//

size_t GetElem (my_list* list, size_t pos) //Gives elem by his order number
{
    size_t to_get = 0;
    size_t i = 0;


    for (; i < pos; i ++)    
        to_get = NEXT(list, to_get)->index;

    //if element position is out of list size 
    //we do not want to go round the list and looking for elem
    if (i >= list->size)
        return -1;
    
    return to_get;
}

//------------------------------------------INSERT/DELETE TO/FROM LOGIC ELEM'S POSITION--------------------------------//

int InsertRight (my_list* list, size_t elem, size_t pos);
int DeleteRight (my_list* list, size_t pos);
int InsertLeft  (my_list* list, size_t elem, size_t pos);
int DeleteLeft  (my_list* list, size_t pos);

// head ---> node_1 ---> node_2 ---> ....
//  0          1           2         ....
//inserting with position 0 everytime will couse inserting in tail

LIST_ERR_CODE ListInsertHead(my_list* list, list_data_t data) {
    LIST_VALIDATE (list);

    ListResize (list);

    size_t new_to_add = TakeFreeList (list);
    ELEM(list, new_to_add)->status = NODE_STATUS::ENGAGED;
    list->size ++;

    ELEM(list, new_to_add)->data = data;

    InsertRight(list, HEAD_ID, new_to_add);

    LIST_VALIDATE(list);

    return LIST_ERR_CODE::SUCCESS;
}

LIST_ERR_CODE ListInsert (my_list* list, size_t pos, list_data_t data) {
    LIST_VALIDATE (list);

    ListResize (list);

    size_t new_to_add = TakeFreeList (list);
    ELEM(list, new_to_add)->status = NODE_STATUS::ENGAGED;
    ELEM(list, new_to_add)->data = data;

    list->size ++;

    size_t dest = GetElem (list, pos);
    size_t rel_dest = ELEM(list, dest)->prev;

    if (dest != LIST_ERR_CODE::WRONG_POS) {
        InsertRight (list, rel_dest, new_to_add);
        LIST_VALIDATE (list);

        return LIST_ERR_CODE::SUCCESS;
    }

    return LIST_ERR_CODE::WRONG_POS;
}


LIST_ERR_CODE ListDelete (my_list* list, size_t pos) {
    LIST_VALIDATE (list);

    if (list->capacity == 0)
        return LIST_UNDERFLOW;
    
    size_t del_elem = GetElem (list, pos);

    if (del_elem == -1)
        return WRONG_POS;

    if (del_elem == HEAD_ID)
        return HEAD_DELEATE;

    DeleteRight (list, pos);
    list->size --;

    InsertFreeList (list, del_elem);
    ListResize(list);

    LIST_VALIDATE (list);

    return SUCCESS;
}


int InsertRight (my_list* list, size_t elem, size_t pos) {
    size_t old_elem            = ELEM(list, pos)->next;
    ELEM(list, pos )->next     = elem;
    ELEM(list, elem)->prev     = ELEM(list, pos)->index;
    ELEM(list, elem)->next     = old_elem;
    ELEM(list, old_elem)->prev = elem;

    return SUCCESS;
}

int DeleteRight (my_list* list, size_t elem) {
    size_t del_elem = ELEM(list, elem)->next;
    ELEM(list, elem)->next = ELEM(list, del_elem)->next;
    NEXT(list, del_elem)->prev = elem;

    ELEM(list, del_elem)->prev = 0;
    ELEM(list, del_elem)->next = 0;

    return SUCCESS;
}

int InsertLeft (my_list* list, size_t elem, size_t pos) {
    size_t old_elem            = ELEM(list, pos)->prev;
    ELEM(list, pos )->prev     = elem;
    ELEM(list, elem)->next     = ELEM(list, pos)->index;
    ELEM(list, elem)->prev     = old_elem;
    ELEM(list, old_elem)->next = elem;

    return SUCCESS;
}

int DeleteLeft (my_list* list, size_t elem) {
    size_t del_elem = ELEM(list, elem)->prev;
    ELEM(list, elem)->prev = ELEM(list, del_elem)->prev;
    PREV(list, del_elem)->next = elem;

    ELEM(list, del_elem)->prev = 0;
    ELEM(list, del_elem)->next = 0;

    return SUCCESS;
}


size_t TakeFreeList (my_list* list) {
    assert (list != NULL);

    list_elem* head = ELEM(list, list->free_head_id);

    if (head->next == HEAD_ID) //only head left
    {
        list->free_head_id = HEAD_ID;
        head->next = 0;
        head->prev = 0;

        return head->index;
    }

    size_t pop_elem = head->next;
    DeleteRight (list, head->index);

    return pop_elem;
}


int InsertFreeList (my_list* list, size_t elem)
{
    assert (list != NULL);
    assert (elem != NULL);

    ELEM(list, elem)->status = NODE_STATUS::FREE;
    ELEM(list, elem)->data = 0;

    if (list->free_head_id == HEAD_ID) // no free elems
    {
        list->free_head_id     = ELEM(list, elem)->index;        //initing free head
        ELEM(list, elem)->next = ELEM(list, elem)->index;        //cycling the list
        ELEM(list, elem)->prev = ELEM(list, elem)->index;
    }
    else
        InsertRight (list, list->free_head_id, elem);

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
    new_to_add->status = NODE_STATUS::ENGAGED;
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
        list->buffer[index].status == NODE_STATUS::FREE)
        return WRONG_INDX;

    if (list->capacity == 0)
        return LIST_UNDERFLOW;

    if (index == HEAD_ID)
        return HEAD_DELEATE;
    
    size_t rel_pos  = ELEM(list, index)->prev;
    size_t del_elem = ELEM(list, index)->index; 

    DeleteRight (list, rel_pos);
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
