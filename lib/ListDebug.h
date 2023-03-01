#ifndef LIST_DEBUG
#define LIST_DEBUG


void InitLogFile ();
void DestroyLogFile ();
void InitDumpFile ();
void DestroyDumpFile ();
void InitGraphDumpFile ();
void DestroyGraphDumpFile ();

LIST_ERR_CODE ListTextDump (my_list* list);
LIST_ERR_CODE ListGraphDump (my_list* list);

LIST_ERR_CODE PrintList (my_list* list, int list_type);  //definition below
void          PrintListElem (list_elem* elem);


LIST_ERR_CODE ListValid (my_list* list);
void          PrintErr (my_list* list, LIST_ERR_CODE ErrCode, const int line, const char* func);


#ifdef DEBUG_VERSION //this custom assert will print error that happened and 'll exit function
#define LIST_VALIDATE( lst_ptr )                                 \
do{                                                              \
    LIST_ERR_CODE ErrCode = ListValid (lst_ptr);                 \
    if (ErrCode != SUCCESS)                                      \
        {                                                        \
            PrintErr (lst_ptr, ErrCode, __LINE__, __func__);     \
            return ErrCode;                                      \
        }                                                        \
}                                                                \
while (0)                                                        \

#else
#define LIST_VALIDATE( lst_ptr ) (void*)0
#endif




#endif