//
//  main.c
//  LW.6
//
//  Created by Нина Альхимович on 13.03.22.
//  10. В командной строке указано имя текстового файла. В файле записаны строки. Найти строку символов, не содержащую ни одного символа, входящего в другие строки.
//  -29.03.2022

#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
    FILE *f;
    if(!(f = fopen(argv[1], "r+t")))    //проверка доступа к файлу
    {
        printf("\nНевозможно открыть файл");
        fclose(f);
        return -1;
    }
    
    char sym, sym2;
    int n = 0, i = 0, counter = 0;
    fpos_t begin = 0, start = 0, end = 0, current, *array;
    
    if(getc(f) == EOF)    //проверка, не пустой ли файл
    {
        printf("\nФайл пустой\n");
        return -1;
    }
    
    if(!(array = (fpos_t *) calloc(255, sizeof(fpos_t))))
    {
        printf("\nПамять не выделена");
        return -1;
    }
    
    fpos_t skip_spaces(FILE *f, fpos_t begin);    //прототип функции, пропускающей пробелы перед строками
    int boundaries(FILE *f, int n, fpos_t *array);    //прототип функции, определяющей, сколько строк содержит                                                     файл и где они заканчиваются
    void output(FILE *f, int counter, fpos_t start, fpos_t end);    //прототип функции, выводящей результат работы                                                               программы на экран
    
    begin = skip_spaces(f, begin);    //определение начала строк в файле
    
    fsetpos(f, &begin);
    n = boundaries(f, n, array);
    i = 0;
    
    fsetpos(f, &begin);
    start = begin;    //запоминание места начала первой строки
    
    while(!feof(f))
    {
        fscanf(f, "%c", &sym);
        fgetpos(f, &current);    //запоминание позиции текущего символа
        fsetpos(f, &begin);
        
        while(!feof(f))    //поиск идентичного символа в других строках
        {
            fscanf(f, "%c", &sym2);
            if(ftell(f) == start+1 && i != n-1)    //пропуск текущей строки
            {
                fsetpos(f, &array[i]);
                continue;
            }
            else if(ftell(f) == start+1 && i == n-1)    //случай, когда нужная строка последняя
            {
                end = current;
                break;
            }
            if(sym == sym2) counter++;    //если найдено совпадение, счетчик увеличивается
            if(counter) break;    //и происходит выход из цикла
        }
        
        if(counter && i != n-1)    //если в строке есть неуникальный символ и она не последняя
        {
            fsetpos(f, &array[i]);    //УТПФ перемещается на начало следующей строкиы
            fgetpos(f, &start);    //запоминается позиция ее начала
            counter = 0;    //счетчик совпадений обнуляется
            i++;    //индекса элементов массива, хранящего позиции концов строк, увеличивается
        }
        
        else if(current+1 == array[i])    //если строка уникальная
        {
            end = current;    //запоминается расположение ее конца
            break;    //и происходит выход из цикла
        }
        
        else fsetpos(f, &current);    //обеспечение перехода к следующему символу для сравнения
    }
    
    output(f, counter, start, end);
    
    free(array);
    fclose(f);    //закрытие файла
    
    return 0;
}

fpos_t skip_spaces(FILE *f, fpos_t begin)
{
    char sym;
    
    while(!feof(f))
    {
        fgetpos(f, &begin);
        fscanf(f, "%c", &sym);
        if(sym != ' ') break;
    }
    
    return begin;
}

int boundaries(FILE *f, int n, fpos_t *array)
{
    int i = 0;
    char sym;
    
    while(!(feof(f)))
    {
        fscanf(f, "%c", &sym);
        if(sym == '\n')    //если достигнут конец строки
        {
            n++;    //увеличивается счетчик количества строк
            fgetpos(f, array+i);    //запоминается позиция конца строки
            i++;
        }
    }
    n++;
    fgetpos(f, array+i);
    
    return n;
}

void output(FILE *f, int counter, fpos_t start, fpos_t end)
{
    char sym;
    
    if(counter) printf("Строка, удовлетворяющая условию, не найдена");
    else
    {
        fsetpos(f, &start);
        printf("Строка символов, не содержащая ни одного символа, входящего в другие строки: \n");
        while(ftell(f) != end+1)    //посимвольный вывод строки
        {
            fscanf(f, "%c", &sym);
            printf("%c", sym);
        }
    }
    printf("\n");
}
