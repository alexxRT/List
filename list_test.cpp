#include "list.h"
#include "list_debug.h"
#include <stdio.h>
#include <iostream>


int main ()
{
    InitLogFile("Log.txt");

    my_list list = {};

    ListInit (&list, 10);
    printf ("\n1) LIST INITED\n");

    for (int i = 0; i < 11; i ++)
        ListInsertHead (&list, i);

    printf ("2) INSERTATION COMPLETE 11 ELEMS\n");

    ListDeleteIndex (&list, 7);
    ListDeleteIndex (&list, 4);
    ListDeleteIndex (&list, 10);

    InitGraphDumpFile("befor_linerize.gv");
    ListGraphDump(&list);
    DestroyGraphDumpFile();

    printf ("3) 3 ELEMS DELETED BY INDEX\n");

    MakeListGreatAgain (&list);
    
    printf ("4) LIST LINIARIZED\n");

    InitGraphDumpFile("after_linerize.gv");
    ListGraphDump(&list);
    DestroyGraphDumpFile();

    for (int i = 0; i < 5; i ++)
    {
        LIST_ERR_CODE err = ListDelete (&list, 1);
        PrintErr(&list, err, __LINE__, __func__);
    }

    printf ("5) 5 ELEMS DELETED\n");

    ListDestroy (&list);

    printf ("6) LIST DESTROYED\n\n");

    DestroyLogFile();

    return 0;
}