#include "../lib/List.h"



int main ()
{
    InitDumpFile ();
    my_list list = {};

    ListInit (&list, 10);

    list_data_t a = 5;
    list_data_t b = 6;

    ListAdd (&list, a);
    ListAdd (&list, b);

    // ListPop (&list, &b);
    // ListPop (&list, &a);

    ListTextDump (&list);

    ListDestroy (&list);
    DestroyDumpFile ();

    return 0;
}