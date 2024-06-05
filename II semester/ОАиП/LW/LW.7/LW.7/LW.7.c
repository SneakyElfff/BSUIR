//
//  main.c
//  LW.7
//
//  Created by Нина Альхимович on 2.04.22.
//  2. Дан бинарный файл f, содержащий сведения об экспортируемых товарах: наименование товара; страна, импортирующая товар; объем поставляемой партии в штуках. Найти страны, в которые экспортируется данный товар, и общий объем его экспорта.
//  -12.04.2022

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

struct goods
{
    char name[30];
    char country[30];
    int num;
};

int main()
{
    FILE *f;
    int n;
    void fillin(FILE *f, int n);    //прототип функции, в которой в файл записывается информация о структурах
    void show(FILE *f, int n);    //прототип функции, которая выводит информацию из файла
    void inquiry(FILE *f, int n);    //прототип функции, выводящей информацию по запросу
    
    printf("Введите количество товаров: ");
    while((scanf("%d", &n)) != 1)
    {
        printf("\nНекорректный ввод, повторите попытку: ");
        scanf("%d", &n);
        fflush(stdin);
    }
    
    fillin(f, n);
    
    show(f, n);
    
    inquiry(f, n);
    
    printf("\n");
    
    return 0;
}

void fillin(FILE *f, int n)
{
    if(!(f = fopen("Export.txt", "ab")))
    {
        printf("\nНевозможно открыть файл");
        fclose(f);
        return;
    }
    
    static int i = 0;;
    struct goods *ptr;
    if((ptr = (struct goods *) calloc(1, sizeof(struct goods))) == NULL)    //выделение памяти под указатель                                                                        на структуру
    {
        printf("\nПамять не выделена");
        return;
    }

    fflush(stdin);
    printf("\nВведите наименование товара: ");
    fflush(stdin);
    fgets(ptr->name, 31, stdin);
    while(ptr->name[0] == '\n' || ptr->name[0] == ' ')
    {
        printf("\nОшибка ввода. Повторите попытку: ");
        fgets(ptr->name, 31, stdin);
    }
        
    printf("Введите страну, импортирующую товар: ");
    fflush(stdin);
    fgets(ptr->country, 31, stdin);
    while(ptr->country[0] == '\n' || ptr->country[0] == ' ')
    {
        printf("\nОшибка ввода. Повторите попытку: ");
        fgets(ptr->country, 31, stdin);
    }
        
    printf("Введите объем поставляемой партии в штуках: ");
    while((scanf("%d", &(ptr->num))) != 1)
    {
        printf("\nОшибка ввода. Повторите попытку: ");
        scanf("%d", &(ptr->num));
        fflush(stdin);
    }
        
    fwrite(ptr, sizeof(struct goods), 1, f);    //запись информации об очередном товаре в файл
    
    i++;
    if(i<n) fillin(f, n);    //если не все товары внесены в перечень, происходит очередной вызов функции
    
    free(ptr);
    
    fclose(f);
}

void show(FILE *f, int n)
{
    if(!(f = fopen("Export.txt", "rb")))
    {
        printf("\nНевозможно открыть файл");
        fclose(f);
        return;
    }
    
    if(getc(f) == EOF)    //проверка, не пустой ли файл
    {
        printf("\nФайл пустой\n");
        return;
    }
    
    int i = 0;
    struct goods st;
    
    rewind(f);
    puts("\nПеречень товаров:");
    for(i=0; i<n; i++)
    {
        fread(&st, sizeof(struct goods), 1, f);    //считывание информации об очередном товаре из файла
        if(feof(f)) break;
        printf("\n%s%s%d", st.name, st.country, st.num);
        printf("\n");
    }
    
    fclose(f);
}

void inquiry(FILE *f, int n)
{
    if(!(f = fopen("Export.txt", "r+b")))
    {
        printf("\nНевозможно открыть файл");
        fclose(f);
        return;
    }

    char inq[30];
    int i, counter = 0;
    struct goods st;

    printf("\nВведите наименование товара, о котором хотите получить информацию: ");
    fflush(stdin);
    fgets(inq, 31, stdin);
    while(inq[0] == '\n' || inq[0] == ' ')
    {
        printf("\nОшибка ввода. Повторите попытку: ");
        fgets(inq, 31, stdin);
    }
    
    for(i=0; i<n; i++)
    {
        if(feof(f)) break;
        fread(&st, sizeof(struct goods), 1, f);    //считывание информации об очередном товаре из файла
        if(strcmp(st.name, inq) == 0)    //если найден запрашиваемый товар
        {
            printf("\n%s%d", st.country, st.num);
            printf("\n");
            counter++;
        }
    }
    
    if(!counter) printf("\nУказанный товар не найден");
    
    struct goods s1, s2;
    fpos_t pos;
    
    rewind(f);
    do
    {
        fread(&s1, sizeof(struct goods), 1, f);
        if(feof(f)) break;
        if(strcmp(s1.name, inq) == 0)    //если найдена удаляемая строка
        {
            do
            {
                fread(&s2, sizeof(struct goods), 1, f);    //считывание следующей структуры
                if(feof(f)) break;
                pos = ftell(f);
                fseek(f, pos - (fpos_t)(2 * sizeof(struct goods)), 0);    //сдвиг назад к началу удаляемой                                                            структуры
                fwrite(&s2, sizeof(struct goods), 1, f);    //запись вновь считанной структуры
                fseek(f, pos, 0);    // возврат к позиции pos
            } while(!feof(f));
        }
    } while(1);

    fclose(f);
    
    n--;
    show(f, n);
}
