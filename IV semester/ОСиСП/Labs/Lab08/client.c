#include <string.h>
#include <sys/socket.h>
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

typedef struct
{
    int socket;
    char current_dir[256];
} Client;

int main(int argc, const char *argv[])
{
    char option[1024];
    int received;
    size_t len;

    int socket_client = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_client == -1)
    {
        fprintf(stderr, "Ошибка создания сокета\n");
        return -1;
    }

    struct sockaddr_in address_serv;    //настройка адреса сервера
    address_serv.sin_family = AF_INET;
    address_serv.sin_addr.s_addr = inet_addr("127.0.0.1");    //адрес сервера - ?
    address_serv.sin_port = htons(1326);

    if(connect(socket_client, (struct sockaddr *)&address_serv, sizeof(address_serv)) == -1)
    {
        printf("Не удалось подключиться к серверу\n");
        return -1;
    }

    while(1)
    {
        memset(option, 0, sizeof(option));

        received = recv(socket_client, option, 1024, 0);
        if(received <= 0)
        {
            printf("Ошибка получения данных от сервера или сервер отключился\n");
            break;
        }

        else
        {
            printf("%s", option);
            if(option[received - 1] == '\n')
            {
                printf("Информация от сервера получена\n");
                break;
            }
        }
    }

    //отправка команд серверу
    while(1)
    {
        printf("> ");
        fflush(stdout);

        memset(option, 0, sizeof(option));
        if(fgets(option, 1024, stdin) == NULL)    //чтение команды
            break;

        len = strlen(option);
        if(len > 0 && option[len-1] == '\n')
            option[len-1] = '\0';
        send(socket_client, option, strlen(option), 0);
        printf("Команда %s отправлена серверу\n", option);

        if(option[0] == '@')    //если команды необходимо читать из файла
        {
            FILE *file = fopen(option+1, "r");
            if(!file)
            {
                fprintf(stderr, "Невозможно открыть файл\n");
                return -1;
            }

            while(fgets(option, 1024, file) == NULL)
            {
                len = strlen(option);
                if(len > 0 && option[len-1] == '\n')
                    option[len-1] = '\0';

                send(socket_client, option, strlen(option), 0);
            }

            fclose(file);
        }

        while(1)    //ожидание ответа от сервера
        {
            memset(option, 0, sizeof(option));

            received = recv(socket_client, option, 1024, 0);
            if(received <= 0)
            {
                printf("Ошибка получения данных от сервера или сервер отключился\n");
                break;
            }

            else
            {
                printf("%s", option);
                if(option[received - 1] == '\n')
                {
                    printf("Информация от сервера получена\n");
                    break;
                }
            }
        }
    }

    close(socket_client);

    return 0;
}
