#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <pthread.h>

#define PORT 8080
#define MAX_EVENTS 100
#define THREAD_POOL_SIZE 4

const char *RESPONSE = "HTTP/1.1 200 OK\r\nContent-Length: 6\r\nConnection: close\r\n\r\nhello!";

// Глобальная переменная для epoll-дескриптора
int epoll_fd;

// Функция обработки клиентского соединения
void handle_client(int client_fd) {
    char buffer[1024];
    read(client_fd, buffer, sizeof(buffer)); // Читаем HTTP-запрос (но не обрабатываем его полностью)
    write(client_fd, RESPONSE, strlen(RESPONSE)); // Отправляем ответ
    close(client_fd); // Закрываем соединение
}

// Функция потока, который обрабатывает события epoll
void *worker_thread(void *arg) {
    struct epoll_event events[MAX_EVENTS];

    while (1) {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < num_events; i++) {
            int client_fd = events[i].data.fd;
            handle_client(client_fd); // Обрабатываем клиента
        }
    }
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in server_addr;

    // Создание TCP-сокета
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Адрес сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Привязываем сокет
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Переводим сокет в режим прослушивания
    if (listen(server_fd, SOMAXCONN) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Создаем epoll-дескриптор
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_fd;

    // Добавляем серверный сокет в epoll
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        perror("epoll_ctl");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Запускаем пул потоков
    pthread_t threads[THREAD_POOL_SIZE];
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_create(&threads[i], NULL, worker_thread, NULL);
    }

    printf("Server is running on port %d...\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        struct epoll_event client_event;
        client_event.events = EPOLLIN;
        client_event.data.fd = client_fd;

        // Добавляем клиента в epoll
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) == -1) {
            perror("epoll_ctl (client)");
            close(client_fd);
        }
    }

    close(server_fd);
    return 0;
}
