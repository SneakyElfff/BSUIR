//
//  main.c
//  LW.1
//
//  Created by Нина Альхимович on 13.02.22.
//  1. Имеются сведения о веществах: указывается название вещества, его удельный вес и проводимость (проводник, полупроводник, изолятор). а) найти удельные веса и название всех полупроводников; б) выбрать данные о проводниках и упорядочить их по убыванию удельных весов.

#include <stdio.h>
#include <stdlib.h>

struct substance
{
    char *name;
    float weight;
    int conductivity;
};

int main()
{
    struct substance *ptr;
    int n, i, j, k;
    struct substance temp;

    printf("Введите количество веществ: ");
    scanf("%d", &n);

    if((ptr = (struct substance *) calloc(n, sizeof(struct substance))) == NULL)
    {
        printf("\nПамять не выделена");
        return 0;
    }

    for(i=0; i<n; i++)
    {
        printf("\nВводите информацию о %d-ом веществе", i+1);

        printf("\nВведите название: ");
        if((ptr[i].name = (char *) calloc(30, sizeof(char))) == NULL)
        {
            printf("\nПамять не выделена");
            return 0;
        }
        fflush(stdin);
        fgets(ptr[i].name, 31, stdin);

        printf("Введите удельный вес: ");
        scanf("%f", &ptr[i].weight);

        printf("Введите данные о проводимости (1 - проводник, 2 - полупроводник, 3 - изолятор): ");
        scanf("%d", &ptr[i].conductivity);
    }

    printf("\nВведенная информация о веществах: ");
    printf("\nНазвание   Удельный вес   Проводимость\n");
    for(i=0; i<n; i++)
        printf("%s%6.3f%4d\n", ptr[i].name, ptr[i].weight, ptr[i].conductivity);

//    printf("\nНазвание   Удельный вес");
//    for(i=0; i<n; i++)
//        if(ptr[i].conductivity == 2)
//        {
//            printf("\n");
//            printf("%16s", ptr[i].name);
//            printf("%3.4f", ptr[i].weight);
//        }
//
//    for(j=0; j<n-1; j++)
//        for(k=n-1; k>j; k--)
//            if(ptr[k-1].weight < ptr[k].weight && ptr[k-1].conductivity == 1 && ptr[k].conductivity == 1)
//            {
//                temp = ptr[k-1];
//                ptr[k-1] = ptr[k];
//                ptr[k] = temp;
//            }
//
//    printf("\n\nУпорядоченный по условию перечень веществ: ");
//    printf("\nНазвание   Удельный вес   Проводимость");
//    for(i=0; i<n; i++)
//    {
//        printf("\n");
//        printf("%s", ptr[i].name);
//        printf("%3.4f", ptr[i].weight);
//        printf("%3d", ptr[i].conductivity);
//    }
//
    printf("\n\n");

    free(ptr);

    return 0;
}


//  9. Имеются сведения о студентах: указывается ФИО, домашний адрес, дата рождения. Сформировать массив, в который записать в алфавитном порядке студентов, которым исполнилось n лет. Ввод, поиск, вывод выполнять в разных функциях.

//#include <stdio.h>
//#include <stdlib.h>
//
//struct student
//{
//    char *name;
//    char *surname:
//    char *patronymic;
//    char *street;
//    int house;
//    int day;
//    int month;
//    int year;
//};
//
//int main()
//{
//    struct student *ptr;
//    int n;
//    void input(int n, struct student *ptr);
//    
//    printf("Введите количество студентов: ");
//    scanf("%d", &n);
//    
//    if((ptr = (struct student *) calloc(n, sizeof(struct student))) == NULL)
//    {
//        printf("\nПамять не выделена");
//        return 0;
//    }
//    
//    input(n, ptr);
//    
//}
//
//void input(int n, struct student *ptr)
//{
//    int i;
//    
//    for(i=0; i<n; i++)
//    {
//        printf("Введите данные о %d-ом студенте: ", i+1);
//        
//        printf("\nВведите имя: ");
//        ptr[i].name = (char *) calloc(10, sizeof(char));
//        fflush(stdin);
//        fgets(ptr[i].name, 11, stdin);
//        
//        printf("\nВведите фамилию: ");
//        ptr[i].surname = (char *) calloc(20, sizeof(char));
//        fflush(stdin);
//        fgets(ptr[i].surname, 21, stdin);
//        
//        printf("\nВведите отчество: ");
//        ptr[i].patronymic = (char *) calloc(20, sizeof(char));
//        fflush(stdin);
//        fgets(ptr[i].patronymic, 21, stdin);
//        
//        
//        
//    }
//}
//
