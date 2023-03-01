#include "../lib/List.h"
#include <stdio.h>



int main ()
{
    
    my_list list = {};

    ListInit (&list, 10);
    printf ("\n1) LIST INITED\n");

    for (int i = 0; i < 11; i ++)
        ListInsertRight (&list, 0, i);

    printf ("2) INSERTATION COMPLETE 11 ELEMS\n");

    ListDeleteIndex (&list, 7);
    ListDeleteIndex (&list, 4);
    ListDeleteIndex (&list, 10);

    printf ("3) 3 ELEMS DELETED BY INDEX\n");

    MakeListGreatAgain (&list);
    
    printf ("4) LIST LINIARIZED\n");


    for (int i = 0; i < 5; i ++)
    {
        int err = ListDelete (&list, 1);
    }

    printf ("5) 5 ELEMS DELETED\n");

    ListDestroy (&list);

    printf ("6) LIST DESTROYED\n\n");

    return 0;
}