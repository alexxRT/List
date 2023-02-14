#include "../lib/Memory.h"
#include "../lib/List.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

const int FREE    = 1;
const int ENGAGED = 2; 
const int MASTER  = 3; // main list head

static FILE* dump_file  = NULL; 
static FILE* graph_file = NULL;

#define CHECK_UP( lst_ptr ) lst_ptr->size == lst_ptr->capacity

#define CHECK_DOWN( lst_ptr ) lst_ptr->size < (lst_ptr->capacity) / 4 && \
lst_ptr->capacity / 2 >= lst_ptr->min_capacity


// #define SET_NODE( elem_ptr, id )                                                                          \
// fprintf (graph_file, "[label = \"Num: %d|indx: %d|data: %d\";];\n", id, elem_ptr->index, elem_ptr->data)  \

// #define SET_COLOR( elem_ptr, id, color)                    \
// fprintf (graph_file, "[color = \"%s\";];\n", color)        \

void PrintListElem (list_elem* elem);

void InitDumpFile ()
{
    dump_file = stderr;//fopen ("../sys/DUMP.txt", "a");
    assert (dump_file != NULL);
}

void DestroyDumpFile ()
{
    fflush (dump_file);
    fclose (dump_file);
    dump_file = NULL;
}

void InitGraphDumpFile ()
{
    graph_file = fopen ("../graphviz/test.gv", "w");
    assert (graph_file != NULL);
}

void DestroyGraphDumpFile ()
{
    fflush (graph_file);
    fclose (graph_file);
    graph_file = NULL;
}

int ListCheck(my_list* list, int stat)
{
    int counter = 0;

    for (int i = 0; i <= list->capacity; i++)
    {
        if (list->buffer[i].status == stat)
            counter ++;
    }

    return counter;
}
enum ERROR_CODES
{
    SUCCESS        = 0,
    LIST_NULL      = 1,
    DATA_NULL      = 2,
    LIST_FULL      = 3,
    LIST_EMPTY     = 4,
    REALLOC_FAILED = 5,
    INVALID_ID     = 6
};

// LEFT(el) add macroses


list_elem* GetElem (my_list* list, size_t id)
{
    list_elem* to_get = list->buffer;

    for (int i = 0; i < id; i ++)
    {
        to_get = to_get->next;
    }

    return to_get;
}

int GetElemId (list_elem* head, int index)
{
    int counter = 0;

    if (index == head->index)
        return counter;

    counter++;

    list_elem* current_elem = head->next;

    while (current_elem != head)
    {
        if (index == current_elem->index)
            return counter;
        
        current_elem = current_elem->next;
        counter++;
    }

    return -1; //but it is not success :(( add error constant
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
        list->free_head = NULL;
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

    if (list->free_head == NULL) // no free elems
    {
        list->free_head = elem;   //initing free head
        elem->next = elem;        //cycling the list
        elem->prev = elem;
    }
    else
        InsertRight (list->free_head, elem);

    return SUCCESS;
}

int ListInsertRight (my_list* list, size_t id, list_data_t data)
{
    ListResize (list);

    list_elem* new_to_add = TakeFreeList (list);
    new_to_add->status = ENGAGED;
    list->size ++;

    new_to_add->data = data;

    list_elem* dest = GetElem (list, id);
    InsertRight (dest, new_to_add);

    return SUCCESS;
}


int ListInsertLeft (my_list* list, size_t id, list_data_t data)
{
    ListResize (list);

    list_elem* new_to_add = TakeFreeList (list);
    new_to_add->status = ENGAGED;
    list->size ++;

    new_to_add->data = data;

    list_elem* dest = GetElem (list, id);
    InsertLeft (dest, new_to_add);

    return SUCCESS;
}

int ListDelete (my_list* list, size_t id)
{
    assert (list != NULL);

    if (list->capacity == 0)
        return LIST_EMPTY;
    
    list_elem* del_elem = GetElem (list, id); 

    DeleteRight (del_elem->prev);
    list->size --;

    InsertFreeList (list, del_elem);
    ListResize(list);

    return SUCCESS;
}

void PrintListElem (list_elem* elem)
{
    fprintf (dump_file,  "Elem address [%p]\n",   elem);
    fprintf (dump_file,  "Elem index   [%d]\n",   elem->index);
    fprintf (dump_file,  "Elem data    [%d]\n",   elem->data);
    fprintf (dump_file,  "Elem status  [%d]\n\n", elem->status);

    return;
}


void PrintList (my_list* list, int list_type)
{
    if (list_type == ENGAGED)
    {
        list_elem* head =       list->buffer;
        list_elem* debug_elem = head->next;

        int order_num = 1;

        while (debug_elem != head)
        {
            fprintf (dump_file,"\nOrder number [%d]\n", order_num);
            PrintListElem (debug_elem);

            order_num ++;
            debug_elem = debug_elem->next;
        }
    }
    else if (list_type == FREE)
    {
        list_elem* head = list->free_head;

        if (head == NULL)
            return;
        
        int order_num = 1;

        fprintf (dump_file, "\nOrder number [%d]\n", order_num);
        PrintListElem (head);
        order_num ++;

        list_elem* debug_elem = head->next;

        while (debug_elem != head)
        {
            fprintf (dump_file, "\nOrder number [%d]\n", order_num);
            PrintListElem (debug_elem);

            order_num ++;
            debug_elem = debug_elem->next;
        }
    }
}

int ListTextDump (my_list* list)
{
    fprintf (stderr, "%p\n", dump_file);

    fprintf (dump_file, "\n\n-------------------------LIST DUMP OCCURED-----------------------\n\n");
    fprintf (dump_file, "List address [%p]\n", list);
    fprintf (dump_file, "List head address [%p]\n", list->buffer);

    fprintf   (dump_file, "Now in the list [%d/%d] elems are engaged\n" , list->size, list->capacity);
    fprintf   (dump_file, "CHECKING...\n[%d] - after direct counting\n", ListCheck(list, ENGAGED));

    fprintf   (dump_file, "listing of engaged elems:\n");
    PrintList (list, ENGAGED);

    fprintf   (dump_file, "Now in the list [%d] elems are free\n", list->capacity - list->size);
    fprintf   (dump_file, "CHECKING...\n[%d] - after direct counting\n", ListCheck(list, FREE));

    fprintf   (dump_file, "Listing of free elems:\n");
    PrintList (list, FREE);

    fprintf   (dump_file, "\n\n-------------------------LIST DUMP FINISHED-----------------------\n\n");

    return SUCCESS;    
}


int ListGraphDump (my_list* list)
{
    //dump of engaged elems

    //initilizing starting attributes
    fprintf (graph_file,
    "digraph {\n\
    rankdir=LR;\n\
    pack=true;\n\
    splines=ortho;\n\
    node [ shape=record ];\n\
    ");


    for (int i = 0; i <= list->capacity; i ++)
    {
        if (list->buffer[i].status == ENGAGED)
        {
            fprintf (graph_file, "\n\tNode%d", i);
            fprintf (graph_file, "[label = \"INDX: %d|NUM: %d|DATA: %d\";];\n", i, GetElemId (list->buffer, i), list->buffer[i].data);
            fprintf (graph_file, "\tNode%d", i);
            fprintf (graph_file, "[color = \"green\";];\n");
        }
        else if (list->buffer[i].status == FREE)
        {
            fprintf (graph_file, "\n\tNode%d", i);
            fprintf (graph_file, "[label = \"INDX: %d|NUM: %d|DATA: %d\";];\n", i, GetElemId (list->free_head, i), list->buffer[i].data);
            fprintf (graph_file, "\tNode%d", i);
            fprintf (graph_file, "[color = \"red\";];\n");
        }
        else if (list->buffer[i].status == MASTER)
        {
            fprintf (graph_file, "\n\tNode%d", i);
            fprintf (graph_file, "[label = \"INDX: %d|NUM: %d|DATA: %d\";];\n", i, GetElemId (list->buffer, i), list->buffer[i].data);
            fprintf (graph_file, "\tNode%d", i);
            fprintf (graph_file, "[color = \"purple\";];\n");
        }
    }

    fprintf (graph_file, "\n");

    for (int i = 0; i < list->capacity; i ++)
        fprintf (graph_file, "\tNode%d -> Node%d[color = \"white\";];\n", i, i+1); //ordering elems as they are in ram

    fprintf (graph_file, "\n");

    for (int i = 0; i <= list->capacity; i++)
    {
        if (list->buffer[i].status == ENGAGED || list->buffer[i].status == MASTER)
        {
            int next_indx = list->buffer[i].next->index;
            fprintf (graph_file, "\tNode%d -> Node%d [constraint = false;];\n", i, next_indx);
        }
        else 
        {
            int next_indx = list->buffer[i].next->index;
            fprintf (graph_file, "\tNode%d -> Node%d [constraint = false;];\n", i, next_indx);
        }
    }

    // while (counter < elem_num)
    // {
    //     int next = 0;

    //     if (counter == elem_num - 1)
    //         next = 0;
    //     else 
    //         next = counter + 1;

    //     fprintf (graph_file, "\tNode%d -> Node%d\n", counter, next);
    //     counter ++;
    // }

    

    fprintf (graph_file, "}\n");

    //while ();


    return SUCCESS;
}



int ListInit (my_list* list, size_t elem_num)
{
    assert (list != NULL);

    list->buffer   = CALLOC (elem_num + 1, list_elem);
    list->capacity = elem_num;
    list->min_capacity = elem_num;
    list->size     = 0;
    list->free_head = NULL;

    list->buffer[0].status = MASTER;
    list->buffer[0].next = list->buffer;
    list->buffer[0].prev = list->buffer;

    for (int i = 1; i <= elem_num; i++)
    {
        list->buffer[i].index = i;
        InsertFreeList (list, list->buffer + i);
    }

    return SUCCESS;
}

int ListDestroy (my_list* list)
{
    assert (list != NULL);

    list->capacity = 0;
    list->size = 0;
    list->free_head = NULL;

    FREE (list->buffer);

    return SUCCESS;
}



//----------------------------------------New functions for linearization------------------------------//

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

int ListInsertIndex (my_list* list, int index, list_data_t data)
{
    ListResize (list);

    list_elem* new_to_add = TakeFreeList_indx (list, index);
    new_to_add->status = ENGAGED;
    list->size ++;

    new_to_add->data = data;

    InsertLeft (list->buffer, new_to_add);

    return SUCCESS;
}

int ListDeleteIndex (my_list* list, int index)
{
    assert (list != NULL);
    assert (list->buffer[index].status == ENGAGED);

    if (list->capacity == 0)
        return LIST_EMPTY;
    
    list_elem* del_elem = list->buffer + index; 

    DeleteRight (del_elem->prev);
    list->size --;

    InsertFreeList (list, del_elem);
    ListResize(list);

    return SUCCESS;
}

//---------------------------------------------------------------------------------------------------//


int MakeListGreatAgain (my_list* list)
{
    assert (list != NULL);

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

    return SUCCESS;
}

int ResizeUp (my_list* list)
{
    assert (list != NULL);

    my_list new_list = {};
    ListInit (&new_list, 2*list->capacity);

    for (int i = 1; i <= list->capacity; i++)
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


    return SUCCESS;
}

int ResizeDown (my_list* list)
{
    assert (list != NULL);

    my_list new_list = {};
    ListInit (&new_list, list->capacity / 2);

    for (int i = 1; i <= list->size; i ++)
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

    return SUCCESS;
}

int ListResize (my_list* list)
{
    assert (list != NULL);

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

    return SUCCESS;
}