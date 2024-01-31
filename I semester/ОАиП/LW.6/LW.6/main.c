//
//  main.c
//  LW.6
//
//  Created by Нина Альхимович on 22.09.21.
//  1. В квадратной матрице порядка n найти наибольший по модулю элемент. Получить квадратную матрицу порядка n-1 путем выбрасывания из исходной матрицы каких-нибудь строки и столбца, на пересечении которых расположен элемент с найденным значением.

#include <stdio.h>
#include <locale.h>
#include <stdlib.h>

int main()
{
    setlocale(LC_ALL, "Russian");    //руссификация консоли
    int **ptr;
    int n, i, j, max = 0, im = 0, jm = 0;
    
    printf("\nВведите порядок матрицы: ");
    scanf("%d", &n);
    
    ptr = (int**)calloc(n,sizeof(int*));    //выделение памяти
    for(i=1; i<n+1; i++) ptr[i] = (int*)calloc(n,sizeof(int));
    
    for(i=1; i<n+1; i++)    //заполнение массива элементами, вводимыми с клавиатуры
        for(j=1; j<n+1; j++)
        {
            printf("\nВведите элемент массива [%d][%d] = ", i, j);
            scanf("%d", ptr+i*n+j);
        }
    
    printf("\nИсходный массив:");    //вывод исходного массива на экран
    for(i=1; i<n+1; i++)
    {
        printf("\n");
        for(j=1; j<n+1; j++)
        printf("%3d ", *(ptr+i*n+j));
    }
    
    for(i=1; i<n+1; i++)    //нахождение наибольшего по модулю элемента
        for(j=1; j<n+1; j++)
            if (*(ptr+i*n+j) > max)
            {
                im = i;    //запоминание его значения
                jm = j;
                max = *(ptr+i*n+j);
            }
    
    printf("\n\nНаибольший по модулю элемент: [%d][%d] = %d\n", im, jm, max);

    printf("\nНовый массив:");    //вывод нового массива на экран, исключая нужные строку и                                                                             столбец
    for(i=1; i<n+1; i++)
    {
        if (i == im) continue;
        printf("\n");
        for(j=1; j<n+1; j++)
        {
            if (j == jm) continue;
            printf("%3d ", *(ptr+i*n+j));
            
        }
    }
    
    free(ptr);    //очистка памяти
    printf ("\n\n");
    
    return 0;
}
