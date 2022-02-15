#include <stdio.h>
#include <stdlib.h>

void mystrcpy(char *t, const char *s)
{
    // Programmer can explicitly cast-away constness
    // char *s2 = (char *)s;
    // s2[1] = 'b';

    while ((*t++ = *s++) != 0)
        ;
}

int myCompareInt(const void *a, const void *b)
{
    int x = *(const int *)a;
    int y = *(const int *)b;
    /*
    if (x < y)
        return -1;
    else if (x > y)
        return 1;
    else
        return 0;
        */
    return x - y;
}

int main()
{
    // constness

    const char *aa = "ABC";
    char b[10];

    mystrcpy(b, aa);
    printf("%s\n", b);

    // ptr to function

    int a[5] = { -1, 36, 5, 2, -4 };

    qsort(a, sizeof(a)/sizeof(*a), sizeof(int), &myCompareInt);
                        // int (*)(const void *, const void *)
    
    for (int i = 0; i < 5; i++) {
        printf("%d\n", a[i]);
    }

    int (*f1)(const void *, const void *);
    f1 = &myCompareInt;
    f1 =  myCompareInt;

    int sum = 0;
    int x = 100;
    int y = 200;
    sum += (*f1)(&x, &y);
    sum +=   f1 (&x, &y);
    printf("%d\n", sum);

    // function prototype

    int *f2(const void *, const void *);

    // complex declaration

    int (*f3[5])(const void *, const void *);
    f3[2] = &myCompareInt;
    f3[2] =  myCompareInt;
    sum += (*f3[2])(&x, &y);
    sum +=   f3[2] (&x, &y);
    printf("%d\n", sum);
}
