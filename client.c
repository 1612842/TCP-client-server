#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

int *arr;
int sizeOfArr = 0;
int isOver = 0;
int isFull=0;
FILE *f;

void chat(int sockfd)
{
    char buff[MAX];
    int n, id;
    int tmp = 0;

    while (1)
    {

        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));


        if (tmp == 0)
            id = atoi(buff);
        else
        {
                printf("Server reponse: %s\n", buff);
                
                char filename[MAX];
                sprintf(filename, "result-%d.txt", id);    
                FILE* fres = fopen(filename, "w");
                fprintf(fres,"%s",buff);
                fclose(fres);

                if (strcmp(buff, "over") != 0)
                {
                    writeToFile(id, buff);
                }
                else
                {
                    isOver = 1;
                    char filename[MAX];
                    sprintf(filename, "client-%d.txt", id);
                    if (sizeOfArr == 0)
                        f = fopen(filename, "w");
                    else
                        f = fopen(filename, "r");

                    fseek(f, 0L, SEEK_END);
                    // calculating the size of the file
                    int count = ftell(f);
                    fclose(f);

                    bzero(buff, sizeof(buff));
                    strcpy(buff, "post\n");
                    write(sockfd, buff, sizeof(buff));

                    printf("send file: %d bytes\n", count);
                }
        }
        tmp = 1;

        if (isOver)
        {
            char content[MAX]="";
            getContent(id,content);
            printf("%s\n", content);

            write(sockfd, content, sizeof(content));

            isOver=0;
            isFull=1;

        }
        else
        {
            if (isFull) continue;

            bzero(buff, sizeof(buff));
            printf("Client %d: ", id);
            n = 0;
            while ((buff[n++] = getchar()) != '\n')
                ;

            write(sockfd, buff, sizeof(buff));

            if ((strncmp(buff, "exit", 4)) == 0)
            {
                printf("Client Exit...\n");
                break;
            }
        }
    }
}

int main()
{

    createSocketAndConnectServer();
}

void createSocketAndConnectServer()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    chat(sockfd);
    close(sockfd);
}

void writeToFile(int id, char *buff)
{
    char filename[MAX];
    sprintf(filename, "client-%d.txt", id);
    f = fopen(filename, "w");

    if (isFull==0){
        int num = atoi(buff);
        sizeOfArr++;
        arr = (int *)realloc(arr, sizeOfArr);
        arr[sizeOfArr - 1] = num;

        sort(arr, sizeOfArr);
        printArr(arr, sizeOfArr, f);
    }

    fclose(f);
}

void printArr(int *arr, int size, FILE *f)
{
    for (int i = 0; i < size; i++)
    {
        fprintf(f, "%d ", arr[i]);
    }
}

void sort(int *arr, int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (arr[i] > arr[j])
            {
                int temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

void getContent(int id, char*res)
{
    char filename[MAX];
    sprintf(filename, "client-%d.txt", id);
    f = fopen(filename, "r");

    char line[MAX]="";
    while (fgets(line, sizeof line, f) != NULL)
    {
        strcat(res, line);
    }
    fclose(f);
    
}