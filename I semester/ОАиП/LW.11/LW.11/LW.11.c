//
//  main.c
//  LW.11
//
//  Created by Нина Альхимович on 31.10.21.
//  10. Ввести арифметическое выражение и, используя рекурсивную функцию, проверить в выражении правильность расстановки скобок любого вида.

#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#define n 10

int main()
{
    int check(char *str, int i);    //прототип функции
    char *str;
    int i = 0;
    
    setlocale(LC_ALL, "Russian");    //руссификация консоли
    
    printf("Введите арифметическое выражение: ");
    if ((str=(char*)calloc(n, sizeof(char))) == NULL)    //выделение памяти под символьную                                                                            строку с проверкой
    {
        puts("Нет свободной памяти");
        return 0;
    }
    
    fflush(stdin);    //очистка потока ввода
    fgets(str, n+1, stdin);    //функция контроля количества вводимых символов в строке
    
    printf("\nИсходная строка: %s", str);
    
    check(str, i);    //вызов функции check на выполнение
    if(check(str, i) == 0) printf("\nСкобки расставлены правильно");
    else printf("\nСкобки расставлены неверно");
    
    printf("\n\n");
    
    free(str);    //очитска памяти
    
    return 0;
}

int check(char *str, int i)    //определение функции check
{
        switch(*(str+i))    //поэлементный перебор строки
        {
            case '\0': return 0;
            case '(': return check(str, i+1)+1;    //открывающая скобка увеличивает возвращаемое значение на                                            единицу, закрывающая - уменьшает
            case '[': return check(str, i+1)+1;
            case '{': return check(str, i+1)+1;
            case ')': return check(str, i+1)-1;
            case ']': return check(str, i+1)-1;
            case '}': return check(str, i+1)-1;
            default: return check(str, i+1)+0;
        }
}
