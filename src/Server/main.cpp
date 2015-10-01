/* Sea Server */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <map>
#include <iostream>
//#define WIN32
#ifndef WIN32

//заголовки для Windows
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <dlfcn.h>

#else
//заголовки для Linux
#define HAVE_STRUCT_TIMESPEC 1
#include <winsock2.h>

#endif

#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <string>
/*
#include <pthread_compat.h>
#include <pthread_signal.h>
#include <pthread_time.h>
#include <pthread_unistd.h>
*/
#include <pthread.h>

typedef unsigned int my_uint32_t;
#define uint32_t my_uint32_t

uint32_t game_port = 12501;  //порт, на котором запускать сервер
const size_t NS = 1024; //максимальное количество байтов в сообщении
const size_t CNT = 128; //размер блока для чтения

using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
map < int, string > clients; //карта клиентов, в качестве ключа используется идентификатор клиента

long int getNext()
{
    pthread_mutex_lock( &mutex );

    FILE * fp = fopen("game.id","r+");
    long int prev;
    fscanf(fp, "%ld", &prev);

    prev++;
    rewind(fp);
    fprintf(fp, "%ld", prev);

    fclose(fp);

    pthread_mutex_unlock( &mutex );
    return prev;
}

void PANIC (const char *msg)
{
    perror(msg);
    exit(-1);
}

bool writeToClient(int client, const char * mes, const int mc)
{

    int len = sizeof(uint32_t) + mc;
    uint32_t mescnt = htonl(mc);

    char * message = (char *) malloc(len);
    char *p = message;
    char *n = (char *) & mescnt;

    for(int i =0; i< sizeof(uint32_t); ++i)
        *p++ = *n++;

    for(int i=0; i< mc; ++i)
        *p++ = *mes++;

    ssize_t  rcount = send(client, message, len,0);
    write(1, message, len);
    free(message);

    printf(" LEN %d\n", len);

    return rcount == len;
}

//анализируем сообщение пользователя и передаём результат
void analyze(char *mes, int size, int client)
{
    mes += sizeof(uint32_t);
    size -= sizeof(uint32_t);

    map<int, string>::iterator it; //для доступа к данным карты

    //первое слово команда, затем зависмые от команды данные

    char * cmd = mes;
    char * p = mes;
    char * e = mes + size;

    while(p < e && *p != ' ') p++;
    if(p == e) return;
    *p='\0';

    if(!strcmp(cmd, "LOGIN"))
    {
        char *n = ++p;
        while(p < e && *p != ' ') p++;
        *p ='\0';

        for ( it=clients.begin() ; it != clients.end(); it++ )
        {
            string st  = string(n) + " is OnLine!";
            writeToClient((*it).first, st.c_str(), st.size());
        }

        pthread_mutex_lock( &mutex );
        clients[client] = n;
        pthread_mutex_unlock( &mutex );

        //отправляем пользователю приветствие
        const char * welcome = "Welcome to the Sea Batle server!";
        writeToClient(client, welcome, strlen(welcome));

    }
    else if(!strcmp(cmd, "LIST"))
    {
    size_t mlen = 0;
    string users = "LIST\n";
    for ( it=clients.begin() ; it != clients.end(); it++ )
        if((*it).first != client)
            users += (*it).second + "\n";

    writeToClient(client, users.c_str(), users.size());
    }
    else if(!strcmp(cmd, "SEND"))
    {
        char *n = ++p;
        while(p < e && *p != ' ') p++;
        *p ='\0';

        bool ok = false;
        for ( it=clients.begin() ; it != clients.end(); it++ )
        {
            if((*it).second == n)
            {
                p++;
                writeToClient((*it).first, p, e-p);
                ok = true;
            }
        }
    if(!ok)
        {
            const char * out = "Пользователь не подключен к серверу.";
            writeToClient(client, out, strlen(out));
        }
    }
    else if(!strcmp(cmd, "WELCOME") || !strcmp(cmd, "ACCEPT") || !strcmp(cmd, "REJECT"))
    {
        char *n = ++p;
        while(p < e && *p != ' ') p++;
        *p ='\0';

        bool ok = false;
        int ic=-1;

        for ( it=clients.begin() ; it != clients.end(); it++ )
        {
            if((*it).second == n)
            {
                p++;
        string st  = string(cmd) + " " + clients[client];
                writeToClient((*it).first, st.c_str(), st.size());
                ic = (*it).first;
                ok = true;
            }
        }
    if(!ok)
        {
            const char * out = "Пользователь не подключен к серверу.";
            writeToClient(client, out, strlen(out));
        }
        else
        {
            if(!strcmp(cmd, "ACCEPT"))
            {
                long int next = getNext();
                char s[255];
                sprintf(s,"GAMEID %ld", next);
                writeToClient(client,s, strlen(s));
                writeToClient(ic, s, strlen(s));
            }
        }
    }
}

//потоковая функция коммуникации с индивидуальным клиентом, в качестве параметра передаётся
//его идентификатор

void * Child (void *arg)
{
    int client = *(int *) arg;

    printf("New client!\n");
    map<int, string>::iterator it;

    char buffer[CNT]; //буфер для чтения вызовом read
    char data[NS]; //общий буфер сообщения

    char * i = data; //начало
    char * e = data + NS;  //конец буфера для сообщения

    ssize_t rcount;  //количество реально обработанных байтов
    uint32_t mescnt = 0; //размер блока сообщения

    while((rcount = recv(client, buffer, CNT, 0)) > 0) //Читаем <= CNT байт из сокета и записываем в buffer.
        //Количество реально прочитанных байт в rcount.
    {

        char * p = buffer;
        for(int q=0; q< rcount && i< e; q++) //копируем вновь полученные байты во временный буфер, проверяем границы массива!
            *i++ = *p++;

        if(mescnt == 0) // размер блока равен нулю
        {
            if(rcount >= sizeof(uint32_t)) //заголовок сообщения передан
            {
                uint32_t mc = * (uint32_t *) data; //сетевой порядок
                mescnt = ntohl(mc); //преобразуем в прямой
            }
        }
        if(mescnt > 0) //принимаем и разбираем остаток сообщения
        {
            //поскольку неизвестно заранее, сколько пришло сообщений,
            //запускаем бесконечный цикл
            char *s = data;
            while(true)
            {
                char * real_data = s + sizeof(uint32_t);

                //сколько уже пришло байт?
                int already_read = i - real_data;
                if(already_read == mescnt) //пришло всё сообщение целиком
                {
                    analyze(real_data, mescnt, client);
                    //разбор серии сообщений закончен
                    mescnt = 0;
                    i = data;
                    break;
                }
                else //пришла либо часть сообщения, либо сразу несколько сообщений
                {
                    if(already_read < mescnt)  // пришло меньше, чем надо, читаем дальше
                        break;

                    //пришло больше, чем надо
                    analyze(real_data, mescnt, client);
                    s = real_data + mescnt;

                    if(i - s < sizeof(uint32_t)) //размер буфера не поместился
                    {
                        int q =0;
                        for( ; s < i; ++q)
                            data[q] = *s++;

                        i = data + q;
                        mescnt = 0;
                        break;
                    }

                    //размер заголовка поместился

                    uint32_t mc = * (uint32_t *) s; //сетевой порядок
                    mescnt = ntohl(mc); //преобразуем в прямой

                    //замыкаем цикл
                }
            }
        }

    }

    //На канале ошибка, закрываем наше клиентское соединение,
    //убираем коннект из карты клиентов. Поскольку это операция с общими
    //данными, то обрамляем эту конструкции в критическую секцию.

    close (client);

    string user = clients[client];

    pthread_mutex_lock( &mutex );
    clients.erase(client);
    pthread_mutex_unlock( &mutex );

    for ( it=clients.begin() ; it != clients.end(); it++ )
    {
        string st  = user + " closed the session.";
        writeToClient((*it).first, st.c_str(), st.size());

    }

    printf("Client died!\n");
}

int main (int , char ** )
{

    int t_port;
    printf("Input port number: ");
    scanf("%d", &t_port);

    if(t_port > 0 && t_port < 65536)
      game_port = t_port;
    else
      printf("Illegal port. Started on 12501.\n");

    //game_port = atoi(argv[1]);


#ifdef WIN32
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(1,1), &wsadata) == SOCKET_ERROR) {
        printf("Error creating socket.");
        return -1;
    }

#endif

    int sd;
    struct sockaddr_in addr;

    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
        PANIC ("Socket");

    addr.sin_family = AF_INET;
    addr.sin_port = htons (game_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind (sd, (struct sockaddr *) &addr, sizeof (addr)) != 0)
        PANIC ("Bind");
    if (listen (sd, 20) != 0)
        PANIC ("Listen");

    while (1) //бесконечный цикл по обслуживанию клиентов, с появлением нового клиента
        //запускается новый поток
    {
        int client, addr_size = sizeof (addr);
        client =
#ifdef WIN32
            accept (sd, (struct sockaddr *) &addr,  & addr_size);
#else
            accept (sd, (struct sockaddr *) &addr,  (socklen_t *)& addr_size);
#endif
        if(accept >=0)
        {
            pthread_t thread;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

            pthread_mutex_lock( &mutex );
            pthread_create(&thread, &attr, Child, &client);
            pthread_mutex_unlock( &mutex );
        }
    }

#ifdef WIN32
    WSACleanup();
#endif

    return 0;
}

