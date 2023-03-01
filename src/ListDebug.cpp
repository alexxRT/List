#include "../lib/Memory.h"
#include "../lib/List.h"
#include "../lib/ListDebug.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>



///----------------------------------------------------FILES FOR DEBUG PRINT-------------------------------------------//

static FILE* dump_file  = NULL; 
static FILE* graph_file = NULL;
static FILE* log_file   = NULL; 


void InitLogFile ()
{
    log_file = fopen ("../sys/LOG.txt", "a");
    assert (log_file != NULL);
}

void DestroyLogFile ()
{
    fflush (log_file);
    fclose (log_file);
    log_file = NULL;
}

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

//-----------------------------------------------------------------------------------------------------------------------------//



//------------------------------------------------------LOGING ERRORS----------------------------------------------------------//

void PrintErr (my_list* list, LIST_ERR_CODE ErrCode, const int line, const char* func)
{
    assert (log_file != NULL && "Please init log file");

    switch (ErrCode)
    {
        case LIST_NULL:
            fprintf (log_file, "LIST is a NULL ptr ");
            break;
        case DATA_NULL:
            fprintf (log_file, "LIST [%p] has NULL buffer ptr ", list);
            break;
        case LIST_OVERFLOW:
            fprintf (log_file, "LIST [%p] overflowed, size - [%lu], capacity - [%lu] ", list, list->size, list->capacity);
            break;
        case LIST_UNDERFLOW:
            fprintf (log_file, "LIST [%p] underflowed, size - [%lu], capacity - [%lu] ",list, list->size, list->capacity);
            break;
        case CAPACITY_INVALID:
            fprintf (log_file, "LIST [%p] capacity equal zero - [%lu] ", list, list->capacity);
            break;
        case WRONG_SIZE:
            fprintf (log_file, "LIST [%p] has incorrect size - [%lu] ", list, list->size);
            break;
        case BROAKEN_LOOP:
            fprintf (log_file, "LIST [%p] the loop is broaken ", list);
            break;
        case NULL_LINK:
            fprintf (log_file, "LIST [%p] one of elems has invalid NULL link ", list);
            break;
        case WRONG_ID:
            fprintf (log_file, "LIST [%p] Elem's ID you use is invalid, please check your list ", list);
            break;
        case WRONG_INDX:
            fprintf (log_file, "LIST [%p] Elem's INDEX is invalid, please check your list ", list);
            break;
        case HEAD_DELEATE:
            fprintf (log_file, "LIST [%p] Trying to delete HEAD! ", list);
        default:
        {
            fprintf (log_file, "Unexpected error code is %d\n", ErrCode);
            assert (0 && "!!Unexpected error code in ErrPrint function!!\n");
        }
    }

    fprintf (log_file, "error happend in function [%s()] and on the line [%d]\n", func, line);
}

//------------------------------------------------------------------------------------------------------------------------//



//---------------------------------------------------VALIDATION FUNCTIONS-------------------------------------------------//

LIST_ERR_CODE ListFieldsValid (my_list* list);
LIST_ERR_CODE EngagedListValid (my_list* list);
LIST_ERR_CODE FreeListValid (my_list* list);


LIST_ERR_CODE ListValid (my_list* list)
{
    LIST_ERR_CODE ErrCode = SUCCESS;

    ErrCode = ListFieldsValid  (list);
    if (ErrCode)
        return ErrCode;

    ErrCode = EngagedListValid (list);
    if (ErrCode) 
      return ErrCode;

    ErrCode = FreeListValid    (list);
    if (ErrCode) 
        return ErrCode;


    return ErrCode;
}

LIST_ERR_CODE ListFieldsValid (my_list* list)
{
    if (!list)                       return LIST_NULL;
    if (!list->buffer)               return DATA_NULL;
    if (list->size > list->capacity) return LIST_OVERFLOW;
    if (list->capacity <= 0)         return CAPACITY_INVALID; // can not create list for 0 elems, its not resizable otherwise

    return SUCCESS;
}

LIST_ERR_CODE EngagedListValid (my_list* list) //check if loop is safe and sound, no empty links
{
    int num_elems = list->size;
    int counter = 0;

    for (int i = 0; i <= list->capacity; i ++)
    {
        if (list->buffer[i].status == ENGAGED)
            counter ++;
    }

    if (num_elems != counter)
        return WRONG_SIZE;
    
    list_elem* head = list->buffer;
    list_elem* elem = head;

    // it goes cloсkwise
    for (int i = 0; i <= num_elems; i ++)
    {
        if (!elem->next || !elem->prev)
            return NULL_LINK;
        elem = elem->next;
    }

    if (elem != head)
        return BROAKEN_LOOP;

    //it goes counter-clockwise
    for (int i = 0; i <= num_elems; i ++)
    {
        if (!elem->next || !elem->prev)
            return NULL_LINK;
        elem = elem->prev;
    }

    if (elem != head)
        return BROAKEN_LOOP;


    //double check, forward and backward
    //i store ptr, so they all should be valid   !!!
    //before it, check if size has correct value !!!
    //mark node where i started and compare where i ended
    //if they are not the same, the loop is broken, taa daaaa

    return SUCCESS;
}

LIST_ERR_CODE FreeListValid (my_list* list)
{
    //free list is empty, nothing to check
    int free_size = list->capacity - list->size;
    if (!free_size)
        return SUCCESS;

    list_elem* head = list->free_head;
    list_elem* elem = head;

    // it goes cloсkwise
    for (int i = 0; i < free_size; i ++)
    {
        if (!elem->next || !elem->prev)
            return NULL_LINK;
        elem = elem->next;
    }

    if (elem != head)
        return BROAKEN_LOOP;

    //it goes counter-clockwise
    for (int i = 0; i < free_size; i ++)
    {
        if (!elem->next || !elem->prev)
            return NULL_LINK;
        elem = elem->prev;
    }

    if (elem != head)
        return BROAKEN_LOOP;

    return SUCCESS;
}

//-------------------------------------------------------------------------------------------------------------------------------//


//-----------------------------------------------------DUMP FUNCTIONS------------------------------------------------------------//

LIST_ERR_CODE PrintList (my_list* list, int list_type);  //definition below
void PrintListElem (list_elem* elem);                    //   ||
                                                         //   ||
                                                         //   \/


LIST_ERR_CODE ListTextDump (my_list* list)
{

    LIST_VALIDATE (list);

    assert (dump_file != NULL && "Please init Dump file");

    fprintf (dump_file, "\n\n-------------------------LIST DUMP OCCURED-----------------------\n\n");
    fprintf (dump_file, "List address [%p]\n", list);
    fprintf (dump_file, "List head address [%p]\n", list->buffer);

    fprintf   (dump_file, "Now in the list [%lu/%lu] elems are engaged\n" , list->size, list->capacity);

    fprintf   (dump_file, "listing of engaged elems:\n");
    PrintList (list, ENGAGED);

    fprintf   (dump_file, "Now in the list [%lu] elems are free\n", list->capacity - list->size);

    fprintf   (dump_file, "Listing of free elems:\n");
    PrintList (list, FREE);

    fprintf   (dump_file, "\n\n-------------------------LIST DUMP FINISHED-----------------------\n\n");

    LIST_VALIDATE (list);

    return SUCCESS;    
}

//!!!GRAPHIC DUMP, SO BEAUTIFUL!!! 

int GetElemId (my_list* list, int index, int stat);

LIST_ERR_CODE ListGraphDump (my_list* list)
{
    LIST_VALIDATE (list);

    assert (dump_file != NULL && "Please init GraphDump file");

    //initilizing starting attributes
    fprintf (graph_file,
    "digraph {\n\
    rankdir=LR;\n\
    pack=true;\n\
    splines=ortho;\n\
    node [ shape=record ];\n\
    ");


    for (int i = 0; i <= list->capacity; i ++) //initing each node, gives it color, data, order num
    {
        if (list->buffer[i].status == ENGAGED)
        {
            int id = GetElemId (list, i, ENGAGED);

            if (id == WRONG_ID)
                return WRONG_ID;

            fprintf (graph_file, "\n\tNode%d", i);
            fprintf (graph_file, "[label = \"INDX: %d|NUM: %d|DATA: %d\";];\n", i, id, list->buffer[i].data);
            fprintf (graph_file, "\tNode%d", i);
            fprintf (graph_file, "[color = \"green\";];\n");
        }
        else if (list->buffer[i].status == FREE)
        {
            int id = GetElemId (list, i, FREE);

            if (id == WRONG_ID)
                return WRONG_ID;

            fprintf (graph_file, "\n\tNode%d", i);
            fprintf (graph_file, "[label = \"INDX: %d|NUM: %d|DATA: %d\";];\n", i, id, list->buffer[i].data);
            fprintf (graph_file, "\tNode%d", i);
            fprintf (graph_file, "[color = \"red\";];\n");
        }
        else if (list->buffer[i].status == MASTER)
        {
            int id = GetElemId (list, i, ENGAGED);

            if (id == WRONG_ID)
                return WRONG_ID;

            fprintf (graph_file, "\n\tNode%d", i);
            fprintf (graph_file, "[label = \"INDX: %d|NUM: %d|DATA: %d\";];\n", i, id, list->buffer[i].data);
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

    fprintf (graph_file, "}\n");

    LIST_VALIDATE (list);

    return SUCCESS;
}

//-----------------------------------------------------------------------------------------------------------------------//


//------------------------------------------------------PRINT LIST FUNCTIONS---------------------------------------------//


LIST_ERR_CODE PrintList (my_list* list, int list_type)
{
    LIST_VALIDATE (list);

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

        if (head == list->buffer)
            return SUCCESS;
        
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

    LIST_VALIDATE (list);

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

//----------------------------------------------------------------------------------------------------------------------//

// function return elem's logical order number, by giving array index
int GetElemId (my_list* list, int index, int stat)
{
    LIST_VALIDATE (list);

    list_elem* head = NULL;

    if (stat == ENGAGED)
        head = list->buffer;
    else 
        head = list->free_head;

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

    return WRONG_ID; // its only returned if no elem was not found
}