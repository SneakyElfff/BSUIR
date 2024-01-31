//
//  main.c
//  practice
//
//  Created by Нина Альхимович.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define set 256
#define prime_num 101

int main()
{
    clock_t time;
    time = clock();    //функция нахождения времени работы программы
    
    FILE *f;
    if(!(f = fopen("strings.txt", "rt")))    //проверка доступа к файлу
    {
        printf("\nНевозможно открыть файл");
        fclose(f);
        return -1;
    }
    
    fpos_t begin = 0, ind_str = 0, ind_substr = 0;
    char sym;
//    fpos_t skip_spaces(FILE *f, fpos_t begin);    //прототип функции, пропускающей пробелы перед строками
    int RabinKarpSearch(FILE *f, fpos_t ind_str, fpos_t ind);    //прототип функции, реализующей поиск                                                                   подстроки в строке
    
    if(getc(f) == EOF)    //проверка, не пустой ли файл
    {
        printf("Файл пустой\n");
        return -1;
    }
    
//    begin = skip_spaces(f, begin);    //определение начала подстроки в файле
    fsetpos(f, &begin);
    ind_substr = begin;    //запоминание места начала подстроки
    
    while(!(feof(f)))
    {
        fscanf(f, "%c", &sym);
        if(sym == '\n')    //если достигнут конец подстроки
        {
            fgetpos(f, &ind_str);    //запоминается позиция начала строки
            break;
        }
    }
    fsetpos(f, &begin);
    
    if((RabinKarpSearch(f, ind_str, ind_substr)) == -1) printf("\nТребуемая подстрока не найдена");
    
    fclose(f);
    
    time = clock() - time;
    printf("\n\nВремя работы программы, реализующей алгоритм Рабина-Карпа: %f секунд\n\n", ((double)time)/CLOCKS_PER_SEC);
    
    return 0;
}

//fpos_t skip_spaces(FILE *f, fpos_t begin)
//{
//    char sym;
//
//    while(!feof(f))
//    {
//        fgetpos(f, &begin);
//        fscanf(f, "%c", &sym);
//        if(sym != ' ') break;
//    }
//
//    return begin;
//}

int RabinKarpSearch(FILE *f, fpos_t ind_str, fpos_t ind_substr)
{
    unsigned long int len_str = 0, len_substr = 0;
    int constant = 1, hash_substr = 0, hash_str = 0, i = -1, j = 0, flag = 0;
    
    unsigned long int StringLength(FILE *f, unsigned long int length, fpos_t ind);    //прототип функции, определяющей длину строк
    int RollingHash(FILE *f, fpos_t ind, unsigned long int length_substring, int constant, int hash, int i);    //прототип функции, возвращающей значение хеша окна
    int StringsMatch(FILE *f, fpos_t ind_str, fpos_t ind_substr, unsigned long int length_substring, int i);    //прототип функции, осуществляющей посимвольное сравнение строк
    
    len_str = StringLength(f, len_str, ind_str);
    len_substr = StringLength(f, len_substr, ind_substr);
    
    if(len_str < len_substr)
    {
        printf("\nПроизошла ошибка. Длина требуемой подстроки превышает длину строки");
        return 0;
    }
    
    for(j=0; j<len_substr-1; j++)
        constant = (constant * set) % prime_num;
    
    hash_substr = RollingHash(f, ind_substr, len_substr, constant, hash_substr, i);    //нахождение хеша подстроки и всей строки
    hash_str = RollingHash(f, ind_str, len_substr, constant, hash_str, i);
    
    for(i=0; i<=len_str-len_substr; i++)    //"пооконное" движение по строке
    {
        if(hash_str == hash_substr)    //если найдено совпадение хешей
            if(!StringsMatch(f, ind_str, ind_substr, len_substr, i))    //посимвольное сравнение окна и                                                                 шаблона
            {
                printf("\nИндекс вхождения подстроки: %d", i);
                flag = 1;    //"поднятие" флага, если найдено хотя бы одно вхождение
            }
        
        if(i<len_str-len_substr)
        {
            hash_str = RollingHash(f, ind_str, len_substr, constant, hash_str, i);    //вычисление                                                                                    очередного хеша окна
            if(hash_str < 0)
                hash_str += prime_num;
        }
    }
    
    if(flag) return 0;    //если искомая подстрока была найдена в строке
    else return -1;
}

unsigned long int StringLength(FILE *f, unsigned long int length, fpos_t ind)
{
    fsetpos(f, &ind);
    char sym;
    
    while(!feof(f))    //пока не достигнут конец файла
    {
        fscanf(f, "%c", &sym);
        if(sym == '\n')    //если достигнут конец подстроки
            break;
        length++;
    }
    return length;
}

int RollingHash(FILE *f, fpos_t ind, unsigned long int length_substring, int constant, int hash, int i)
{
    fsetpos(f, &ind);
    char sym, sym2;
    
    if(i == -1)
        for(i=0; i<length_substring; i++)
        {
            fscanf(f, "%c", &sym);
            hash = (set * hash + sym) % prime_num;
        }
    else
    {
        fseek(f, i, 1);
        fscanf(f, "%c", &sym);
        fseek(f, length_substring-1, 1);
        fscanf(f, "%c", &sym2);
        hash = (set * (hash - sym*constant) + sym2) % prime_num;
    }
    
    return hash;
}

int StringsMatch(FILE *f, fpos_t ind_str, fpos_t ind_substr, unsigned long int length_substring, int i)
{
    int j, flag = 0;
    char sym, *substring;
    fsetpos(f, &ind_substr);
    
    if((substring = (char *) malloc(length_substring*sizeof(char))) == NULL)
    {
        printf("\nПамять не выделена");
        return -1;
    }
    
    fgets(substring, length_substring+1, f);
    fsetpos(f, &ind_str);
    fseek(f, i, 1);
    
    for(j=0; j<length_substring; j++)
    {
        fscanf(f, "%c", &sym);
        if(sym != substring[j])
        {
            flag = 1;
            break;
        }
    }
    
    free(substring);
    
    return flag;
}
