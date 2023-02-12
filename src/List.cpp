#include "../lib/Memory.h"
#include "../lib/List.h"
#include <stdio.h>
#include <assert.h>


const int FREE    = 1;
const int ENGAGED = 2; 
const int MASTER  = 3; // main list head

static FILE* dump_file = NULL; 

void InitDumpFile ()
{
    dump_file = fopen ("../sys/DUMP.txt", "a");
    assert (dump_file != NULL);
}

void DestroyDumpFile ()
{
    fflush (dump_file);
    fclose (dump_file);
    dump_file = NULL;
}

int LIST_CHECK(my_list* list, int stat)
{
    int counter = 0;

    for (int i = 1; i < list->capacity+1; i++)
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
};

// LEFT(el) add macroses

list_elem* PopFromFreeList (my_list* list)
{
    assert (list            != NULL);
    assert (list->free_head != NULL);

    list_elem* head = list->free_head;

    if (head->next == head) //only head left
    {
        list->free_head = NULL;
        return head;
    }
    else 
    {
        list_elem* pop_elem = list->free_head->next;
        list->free_head->next = pop_elem->next;

        return pop_elem;
    }

    return NULL;
}

int AddToFreeList (my_list* list, list_elem* elem)
{
    assert (list != NULL);
    assert (elem != NULL);

    elem->status = FREE;

    if (list->free_head == NULL) // no free elems
    {
        list->free_head = elem;
        elem->next = elem;       //cycling the list
    }
    else
    {
        list_elem* old_elem = list->free_head->next;
        list->free_head->next = elem;
        elem->next = old_elem;
    }

    return SUCCESS;
}

int ListAdd (my_list* list, list_data_t data)
{
    //ListResize (list);

    list_elem* new_to_add = PopFromFreeList (list);
    new_to_add->status = ENGAGED;
    list->size ++;

    new_to_add->data = data;
    list_elem* head = list->buffer;

    list_elem* old_next = head->next;
    head->next = new_to_add;
    new_to_add->next = old_next;

    return SUCCESS;
}

int ListPop (my_list* list, list_data_t* data)
{
    assert (list != NULL);

    if (list->capacity == 0)
        return LIST_EMPTY;

    list_elem* head = list->buffer;
    list_elem* pop_elem = head->next;

    pop_elem->status = FREE;
    list->size --;

    *data = pop_elem->data;

    head->next = pop_elem->next;
    pop_elem->next = NULL;

    AddToFreeList (list, pop_elem);
    //ListResize(list);

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
        list_elem* head = list->buffer;
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
    fprintf (dump_file, "\n\n-------------------------LIST DUMP OCCURED-----------------------\n\n");
    fprintf (dump_file, "List address [%p]\n", list);
    fprintf (dump_file, "List head address [%p]\n", list->buffer);

    fprintf   (dump_file, "Now in the list [%d/%d] elems are engaged\n" , list->size, list->capacity);
    fprintf   (dump_file, "CHECKING...\n [%d] - after direct counting\n", LIST_CHECK(list, ENGAGED));

    fprintf   (dump_file, "listing of engaged elems:\n");
    PrintList (list, ENGAGED);

    fprintf   (dump_file, "Now in the list [%d] elems are free\n", list->capacity - list->size);
    fprintf   (dump_file, "CHECKING...\n [%d] - after direct counting\n", LIST_CHECK(list, FREE));

    fprintf   (dump_file, "Listing of free elems:\n");
    PrintList (list, FREE);

    fprintf   (dump_file, "\n\n-------------------------LIST DUMP FINISHED-----------------------\n\n");

    return SUCCESS;    
}



int ListInit (my_list* list, size_t elem_num)
{
    assert (list != NULL);

    list->buffer   = CALLOC (elem_num + 1, list_elem);
    list->capacity = elem_num;
    list->size     = 0;
    list->free_head = NULL;

    list->buffer[0].status = MASTER;
    list->buffer[0].next = list->buffer;
    //list->buffer[0].prev = NULL;

    for (int i = 1; i < elem_num + 1; i++)
    {
        list->buffer[i].index = i;
        AddToFreeList (list, list->buffer + i);
    }

    return SUCCESS;
}

int ListDestroy (my_list* list)
{
    assert (list != NULL);

    list->capacity = 0;
    FREE (list->buffer);

    return SUCCESS;
}