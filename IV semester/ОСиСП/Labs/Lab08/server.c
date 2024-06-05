//
//  server.c
//  Lab08
//
//  Created by Нина Альхимович on 08.06.23.
//

#define _POSIX_C_SOURCE 200809L

#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#define num_of_clients 10

typedef struct
{
    int socket;
    char current_dir[256];
} Client;

void *process_client(void *arg)
{
    char input[1024], *dir, new_path[256];
    Client *client = (Client *)arg;
    int socket_client = client->socket, received, len;

    dir = (char *) calloc(256, sizeof(char));
    if(!dir)
    {
        fprintf(stderr, "Память не выделена\n");
        return NULL;
    }

    FILE *file = fopen("info.txt", "r");
    if(!file)
    {
        fprintf(stderr, "Невозможно открыть файл\n");
        return NULL;
    }

    else
    {
        while(fgets(input, 1024, file) != NULL)
            send(socket_client, input, strlen(input), 0);
    }

    printf("Ожидание сообщений клиента...\n");

    while(1)
    {
        memset(input, 0, sizeof(input));

        received = recv(socket_client, input, 1024, 0);
        if(received <= 0)
        {
            printf("Ошибка или клиент отключился\n");
            break;
        }
        printf("Команда %s поступила от клиента\n", input);

        if(strcmp(input, "ECHO") == 0)
            send(socket_client, input, strlen(input), 0);

        else if(strcmp(input, "QUIT"))
        {
            printf("Завершение сеанса...\n");
            break;
        }

        else if(strcmp(input, "INFO"))
        {
            while(fgets(input, 1024, file) != NULL)
                send(socket_client, input, strlen(input), 0);
        }

        else if(strncmp(input, "CD ", 3))    //3 - ?
        {
            dir = input + 3;
            len = strlen(dir);

            if(len > 0 && dir[len-1] == '\n')
                dir[len-1] = '\0';

            snprintf(new_path, sizeof(new_path), "%s/%s", client->current_dir, dir);    //формировние нового пути

            if(realpath(new_path, NULL) == NULL)
                send(socket_client, "Неправильный путь\n", 18, 0);
            else
            {
                strcpy(client->current_dir, new_path);
                send(socket_client, "Путь изменен\n", 13, 0);
            }
        }

        else if(strcmp(input, "LIST") == 0)
        {
            FILE *dir_list = popen("ls -l", "r");
            if(dir_list)
                while(fgets(input, 1024, dir_list) != NULL)
                    send(socket_client, input, strlen(input), 0);

            pclose(dir_list);
        }

        else
            send(socket_client, "Неизвестная команда\n", 20, 0);
    }

    close(socket_client);
    free(client);
    //pthread_detach(pthread_self());

    return NULL;
}

int main(int argc, const char *argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "При запуске сервера необходимо указать путь к корневому каталогу\n");
        return -1;
    }

    char *root_dir = (char *) calloc(256, sizeof(char));
    if(!root_dir)
    {
        fprintf(stderr, "Память не выделена\n");
        return -1;
    }
    strcpy(root_dir, argv[1]);

    int socket_serv = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_serv == -1)
    {
        fprintf(stderr, "Ошибка создания сокета\n");
        return -1;
    }

    struct sockaddr_in address_serv;    //настройка адреса сервера
    address_serv.sin_family = AF_INET;
    address_serv.sin_addr.s_addr = INADDR_ANY;
    address_serv.sin_port = htons(1326);

    if(bind(socket_serv, (struct sockaddr *)&address_serv, sizeof(address_serv)) == -1)    //привязка сокета к адресу сервера
    {
        fprintf(stderr, "Ошибка привязки сокета\n");
        return -1;
    }

    if(listen(socket_serv, num_of_clients) == -1)    //прослушивать входящие соединения
    {
        fprintf(stderr, "Невозможно прослушивать входящие соединения\n");
        return -1;
    }

    printf("Сервер запущен. Корневой каталог: %s\n", root_dir);

    //ожидание и обработка входящих соединений
    while(1)
    {
        struct sockaddr_in address_clnt;

        socklen_t len_clnt = sizeof(address_clnt);
        int socket_client = accept(socket_serv, (struct sockaddr *)&address_clnt, &len_clnt);
        if(socket_client == -1)
        {
            fprintf(stderr, "Ошибка создания сокета\n");
            return -1;
        }

        Client *client = (Client *) calloc(1, sizeof(Client));
        if(!client)
        {
            fprintf(stderr, "Память не выделена\n");
            return -1;
        }
        client->socket = socket_client;
        strcpy(client->current_dir, root_dir);

        pthread_t client_thread;
        if(pthread_create(&client_thread, NULL, process_client, (void*)client) != 0)
        {
            fprintf(stderr, "Ошибка открытия потока\n");
            return -1;
        }
        pthread_cancel(client_thread);
    }

    free(root_dir);
    close(socket_serv);

    return 0;
}
