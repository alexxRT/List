#include "../lib/List.h"
#include "../lib/ListDebug.h"
#include <stdio.h>



int main ()
{
    InitDumpFile ();
    InitGraphDumpFile ();
    InitLogFile ();

    my_list list = {};

    ListInit (&list, 10);

    list_data_t a = 5;
    list_data_t b = 6;

    for (int i = 0; i < 10; i++)
    {
        ListInsertRight (&list/*NULL*/, 0, i); // u can, for instance, insearting in head of NULL list, check then log file
        //ListTextDump (&list);
    }

    //ListTextDump (&list);

    ListDelete (&list, 3);
    ListDelete (&list, 7);

    ListInsertLeft (&list, 3, 777);
    ListDelete (&list, 7);

    MakeListGreatAgain (&list);

    ListTextDump  (&list);
    ListGraphDump (&list); 

    ListDestroy (&list);

    DestroyLogFile ();
    DestroyDumpFile ();
    DestroyGraphDumpFile ();

    return 0;
}