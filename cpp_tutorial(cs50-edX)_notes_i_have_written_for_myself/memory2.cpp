#include <stdio.h>


//              & Provides the address of something stored in memory.
 //             * Instructs the compiler to go to a location in memory.   (basically * is called a pointer)


int main(void)
{
    int n = 50;
    int *p = &n;                //p is a pointer that stores the adress of integer n
    printf("%p\n", p);
}




//              instead of the above printf("%p\n", p); ...., we could say....
//              printf("%i\n", *p); ... now we would get the integer result of what's stored in the adress pointed by *p


