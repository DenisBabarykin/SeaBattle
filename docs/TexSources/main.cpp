int main (void)
{
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
    addr.sin_port = htons (MY_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind (sd, (struct sockaddr *) &addr, sizeof (addr)) != 0)
        PANIC ("Bind");
    if (listen (sd, 20) != 0)
        PANIC ("Listen");
    while (1)
    {
        int client, addr_size = sizeof (addr);
        client =
#ifdef WIN32
            accept (sd, (struct sockaddr *) &addr,  & addr_size);
#else
            accept (sd, (struct sockaddr *) &addr,  (socklen_t *)& addr_size);
#endif
        if (client > 0)
        {
            pthread_t thread;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            pthread_mutex_lock( &mutex );
            pthread_create(&thread, &attr, Child, &client);
        }
    }
#ifdef WIN32
    WSACleanup();
#endif
    return 0;
}
