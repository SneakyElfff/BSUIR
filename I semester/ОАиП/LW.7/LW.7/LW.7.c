//
//  main.c
//  LW.7
//
//  Created by Нина Альхимович on 6.10.21.
//  10. Дана действительная квадратная матрица размером n. Расположить элементы матрицы по спирали.

#include <stdio.h>
#include <locale.h>
#include <stdlib.h>

int main()
{
    setlocale(LC_ALL, "Russian");    //руссификация консоли
    int **ptr;
    int n, N, i, j, k = 1, h, s1 = 1, s2;

    printf("\nВведите порядок матрицы: ");
    scanf("%d", &n);
    N = n;

    ptr = (int**)calloc(n,sizeof(int*));    //выделение памяти
    for(i=1; i<n+1; i++) ptr[i] = (int*)calloc(n,sizeof(int));

    for(h=1; h<(n/2+n%2+1); h++)
    {
        for(j=s1; j<n+1-s2; j++) *(ptr+s1*n+j) = k++;    //заполнение первой строки
        for(i=s1+1; i<n+1-s2; i++) *(ptr+i*n+(n-s2)) = k++;    //заполнение                                                               последнего                                                                  столбца
        k += (n-2*s2)-2;
        for(j=s1; j<n-s2; j++) *(ptr+(n-s2)*n+j) = k--;    //заполнение последней                                                                   строки
        k += 2*(n-2*s2)-3;
        for(i=s1+1; i<(n-s2); i++) *(ptr+i*n+s1) = k--;    //заполнение первого                                                                   столбца
        k += (n-2*s2)-1;
        s1++;
        s2++;
    }

        printf("\nПолученный массив:");
        for(i=1; i<N+1; i++)
        {
            printf("\n");
            for(j=1; j<N+1; j++)
                printf("%3d", *(ptr+i*N+j));
        }
    
    free(ptr);    //очистка памяти
    printf("\n\n");

    return 0;
}

