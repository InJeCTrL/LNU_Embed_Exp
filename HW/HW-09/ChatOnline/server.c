/*
    As a server, I will send ![ACCREG]/![REJREG] to accept or reject a register,
                             message string to client(format is [time ID]\nuserdata),
                        receive ![REG]stringID from the client,
                                ![MSG]string to broadcast userdata,
                                ![SF]filename to save the file from server,
                                ![RF]filename to send the file to target client,
                                ![QUIT] to close the connection between server and client.
*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>
#include<time.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<errno.h>

#define TRUE 1
#define MAX_USERDATA        8192                  //max length of userdata string
#define MAX_ID              1024                  //max length of ID string
#define MAX_TIME            20                    //max length of time string
#define MAX_RECVFROMCLIENT  (MAX_USERDATA + 6)    //max length of ReadytoSend string
#define MAX_READYTOSEND     (MAX_TIME + MAX_ID + MAX_USERDATA + 2)  //max length of RecvfromServ string
#define MAX_REG             (MAX_ID + 6)          //max length of register string
#define MAX_REGRESULT       10                    //max length o fregister sendback string
#define MAX_CONNECT         100                   //max user connected to this host
#define MAX_FILELEN     (1024 * 1024)             //max length of file content

//information of client link
typedef struct
{
    //UDP
    struct sockaddr_in SA_Client;//sock address info of client
    int len_SA_Cient;            //length of SA_Client
    //TCP
    int fd_client;              //file descriptor of socket

    char ID[1024];              //client ID
    int flag_protocol;          //0:TCP 1:UDP
}ClientNode;

ClientNode ClientList[MAX_CONNECT]; //the list record client information
int fd_TCP, fd_UDP;     //file descriptor of TCP and UDP
struct sockaddr_in SA_server;   //server address configuration
int len_SA_server;          //length of SA_server

//deal with the data received from the client
void* th_Proc(void *data)
{
    char RecvfromClient[MAX_RECVFROMCLIENT] = { 0 };       //data received
    char ReadytoSend[MAX_READYTOSEND] = { 0 };      //formatted string ready to send to the client
    int pos_fd = *(int*)data;               //index of clientlist
    char ch;
    int i;
    time_t t = 0;
    struct tm *stu_t;
    char str_time[MAX_TIME] = { 0 };    //string of formatted time
    FILE *fp = NULL;                                 //file pointer of the file ready to be received
    long ReadLen = 0;                                //length of file part written everytime
    unsigned char Buf_File[MAX_RECVFROMCLIENT] = { 0 }; //buffer used to save content of file
    unsigned char tBuf_File[MAX_RECVFROMCLIENT] = { 0 }; //buffer used to save content of file

    //input string and send
    while(TRUE)
    {
        recv(ClientList[pos_fd].fd_client, RecvfromClient, MAX_RECVFROMCLIENT, MSG_NOSIGNAL);
        //receive quit from client
        if (!strncmp(RecvfromClient, "![QUIT]", 7))
        {
            ClientList[pos_fd].flag_protocol = -1;
            send(ClientList[pos_fd].fd_client, "![QUIT]", MAX_READYTOSEND, MSG_NOSIGNAL);
            break;
        }
        //receive register request from client
        else if (!strncmp(RecvfromClient, "![REG]", 6))
        {
            for (i = 0;i<MAX_CONNECT;i++)
            {
                //not setted or different string length
                if (ClientList[i].flag_protocol == -1 || strlen(RecvfromClient) - 6 != strlen(ClientList[i].ID))
                    continue;
                //have same ID, send reject message
                if (!strcmp(RecvfromClient + 6, ClientList[i].ID))
                {
                    strcpy(ReadytoSend, "![REJREG]");
                    break;
                }
            }
            if (i == MAX_CONNECT)
            {
                strcpy(ReadytoSend, "![ACCREG]");
                strcpy(ClientList[pos_fd].ID, RecvfromClient + 6);
            }
            send(ClientList[pos_fd].fd_client, ReadytoSend, MAX_READYTOSEND, MSG_NOSIGNAL);
        }
        //client send file
        else if (!strncmp(RecvfromClient, "![SF]", 5))
        {
            //create file
            fp = fopen(RecvfromClient + 5, "w");
            //fill file content
            memset(Buf_File, 0, MAX_READYTOSEND);
            ReadLen = recv(ClientList[pos_fd].fd_client, Buf_File, MAX_FILELEN, MSG_NOSIGNAL);
            fwrite(Buf_File, 1, ReadLen, fp);
            fclose(fp);
        }
        //client receive file
        else if (!strncmp(RecvfromClient, "![RF]", 5))
        {
            //open existed file
            fp = fopen(RecvfromClient + 5, "r");
            //open file from error
            if (!fp)
                send(ClientList[pos_fd].fd_client, "![REJRF]", MAX_READYTOSEND, MSG_NOSIGNAL);
            else
            {
                while ((ReadLen = fread(tBuf_File, MAX_READYTOSEND - 10, 1, fp)) > 0)
                {
                    memset(Buf_File, 0, MAX_READYTOSEND);
                    strcpy(Buf_File, "![ACCRF]");
                    strcat(Buf_File, tBuf_File);
                    send(ClientList[pos_fd].fd_client, Buf_File, ReadLen + 8, MSG_NOSIGNAL);
                    memset(tBuf_File, 0, MAX_RECVFROMCLIENT);
                }
                fclose(fp);
            }
        }
        //normal message
        else if (!strncmp(RecvfromClient, "![MSG]", 6))
        {
            strcpy(ReadytoSend, "[");
            //connect time string
            time(&t);
            stu_t = localtime(&t);
            strftime(str_time, MAX_TIME, "%Y-%m-%d %H:%M:%S", stu_t);
            strcat(ReadytoSend, str_time);
            strcat(ReadytoSend, " ");
            //connect ID string
            strcat(ReadytoSend, ClientList[pos_fd].ID);
            strcat(ReadytoSend, "]\n");
            //connect userinput string
            strcat(ReadytoSend, RecvfromClient + 6);
            for (i = 0;i < MAX_CONNECT;i++)
            {
                if (ClientList[i].flag_protocol != -1)
                    send(ClientList[i].fd_client, ReadytoSend, MAX_READYTOSEND, MSG_NOSIGNAL);
            }
        }
        //clear every string buffer
        memset(RecvfromClient, 0, MAX_RECVFROMCLIENT);
        memset(ReadytoSend, 0, MAX_READYTOSEND);
        memset(str_time, 0, MAX_TIME);
    }

    return 0;
}

int TCPChatRoom()
{
    int tfd_client;  //temp file descriptor of client
    pthread_t thread_proc[MAX_CONNECT] = { 0 };//thread IDs of process info
    struct sockaddr_in tsa;
    int tl = sizeof(struct sockaddr_in);
    int i;

    if (listen(fd_TCP, MAX_CONNECT) == -1)
    {
        printf("Fail to listen: %s\n", strerror(errno));
        return -1;
    }    
    while (TRUE)
    {
        memset(&tsa, 0, sizeof(tsa));
        tl = sizeof(tsa);
        //new client
        if ((tfd_client = accept(fd_TCP, (struct sockaddr *)&tsa, &tl)) != -1)
        {
            //!!!dangerous!!!
            //if number of conntion is large, fd may be wrong.

            //find NULL connection
            for (i = 0;i < MAX_CONNECT;i++)
            {
                if (ClientList[i].flag_protocol == -1)
                {
                    ClientList[i].fd_client = tfd_client;
                    ClientList[i].flag_protocol = 0;
                    break;
                }
            }
            //connection number is not full
            if (i != MAX_CONNECT)
            {
                //one thread per client
                pthread_create(&thread_proc[i], NULL, th_Proc, (void *)&i);
                pthread_detach(thread_proc[i]);
            }
        }
    }
    
    return 0;
}

int UDPChatRoom()
{
    char RecvfromClient[MAX_RECVFROMCLIENT] = { 0 };    //receivefromclient string
    char ReadytoSend[MAX_READYTOSEND] = { 0 };          //the formatted string ready to send
    pthread_t thread_proc[MAX_CONNECT] = { 0 };//thread IDs of process info
    struct sockaddr_in tsa;
    int tl = sizeof(struct sockaddr_in);
    int i;
    time_t t = 0;
    struct tm *stu_t;
    char str_time[MAX_TIME] = { 0 };    //string of formatted time
    FILE *fp = NULL;                                 //file pointer of the file ready to be received
    long ReadLen = 0;                                //length of file part written everytime
    unsigned char Buf_File[MAX_RECVFROMCLIENT] = { 0 }; //buffer used to save content of file
    unsigned char tBuf_File[MAX_RECVFROMCLIENT] = { 0 }; //buffer used to save content of file

    
    while (TRUE)
    {
        memset(&tsa, 0, sizeof(tsa));
        tl = sizeof(tsa);
        recvfrom(fd_UDP, RecvfromClient, MAX_RECVFROMCLIENT, MSG_NOSIGNAL, (struct sockaddr *)&tsa, &tl);
        //receive quit from client
        if (!strncmp(RecvfromClient, "![QUIT]", 7))
        {
            sendto(fd_UDP, "![QUIT]", MAX_READYTOSEND, MSG_NOSIGNAL, (struct sockaddr *)&tsa, tl);
            //set clientlist
            for (i = 0;i < MAX_CONNECT;i++)
            {
                if (ClientList[i].flag_protocol != -1 && ClientList[i].len_SA_Cient == tl && !memcmp(&ClientList[i].SA_Client, &tsa, tl))
                {
                    ClientList[i].flag_protocol = -1;
                    break;
                }
            }
        }
        else if (!strncmp(RecvfromClient, "![REG]", 6))
        {
            //check same ID
            for (i = 0;i < MAX_CONNECT;i++)
            {
                //not setted or different string length
                if (ClientList[i].flag_protocol == -1 || strlen(RecvfromClient) - 6 != strlen(ClientList[i].ID))
                    continue;
                //have same ID, send reject message
                if (!strcmp(RecvfromClient + 6, ClientList[i].ID))
                {
                    strcpy(ReadytoSend, "![REJREG]");
                    break;
                }
            }
            //no same ID
            if (i == MAX_CONNECT)
            {
                //check NULL client
                for (i = 0;i < MAX_CONNECT;i++)
                {
                    //find a NULL client
                    if (ClientList[i].flag_protocol == -1)
                    {
                        ClientList[i].len_SA_Cient = tl;
                        memcpy(&ClientList[i].SA_Client, &tsa, tl);
                        ClientList[i].flag_protocol = 1;
                        strcpy(ReadytoSend, "![ACCREG]");
                        strcpy(ClientList[i].ID, RecvfromClient + 6);
                        break;
                    }
                }
                //client number is full
                if (i == MAX_CONNECT)
                {
                    strcpy(ReadytoSend, "![REJREG]");
                }
            }
            sendto(fd_UDP, ReadytoSend, MAX_READYTOSEND, MSG_NOSIGNAL, (struct sockaddr *)&tsa, tl);
        }
        //client send file
        else if (!strncmp(RecvfromClient, "![SF]", 5))
        {
            //create file
            fp = fopen(RecvfromClient + 5, "w");
            //fill file content
            memset(Buf_File, 0, MAX_READYTOSEND);
            ReadLen = recvfrom(fd_UDP, Buf_File, MAX_FILELEN, MSG_NOSIGNAL, (struct sockaddr *)&tsa, &tl);
            fwrite(Buf_File, 1, ReadLen, fp);
            fclose(fp);
        }
        //client receive file
        else if (!strncmp(RecvfromClient, "![RF]", 5))
        {
            //open existed file
            fp = fopen(RecvfromClient + 5, "r");
            //open file from error
            if (!fp)
                sendto(fd_UDP, "![REJRF]", MAX_READYTOSEND, MSG_NOSIGNAL, (struct sockaddr *)&tsa, tl);
            else
            {
                while ((ReadLen = fread(tBuf_File, MAX_READYTOSEND - 10, 1, fp)) > 0)
                {
                    memset(Buf_File, 0, MAX_READYTOSEND);
                    strcpy(Buf_File, "![ACCRF]");
                    strcat(Buf_File, tBuf_File);
                    sendto(fd_UDP, Buf_File, ReadLen + 8, MSG_NOSIGNAL, (struct sockaddr *)&tsa, tl);
                    memset(tBuf_File, 0, MAX_RECVFROMCLIENT);
                }
                fclose(fp);
            }
        }
        //normal message
        else if (!strncmp(RecvfromClient, "![MSG]", 6))
        {
            strcpy(ReadytoSend, "[");
            //connect time string
            time(&t);
            stu_t = localtime(&t);
            strftime(str_time, MAX_TIME, "%Y-%m-%d %H:%M:%S", stu_t);
            strcat(ReadytoSend, str_time);
            strcat(ReadytoSend, " ");
            //connect ID string
            for (i = 0;i < MAX_CONNECT;i++)
            {
                if (ClientList[i].flag_protocol != -1 && ClientList[i].len_SA_Cient == tl && !memcmp(&ClientList[i].SA_Client, &tsa, tl))
                {
                    strcat(ReadytoSend, ClientList[i].ID);
                    break;
                }
            }
            strcat(ReadytoSend, "]\n");
            //connect userinput string
            strcat(ReadytoSend, RecvfromClient + 6);
            for (i = 0;i < MAX_CONNECT;i++)
            {
                if (ClientList[i].flag_protocol != -1)
                    sendto(fd_UDP, ReadytoSend, MAX_READYTOSEND, MSG_NOSIGNAL, (struct sockaddr *)&ClientList[i].SA_Client, ClientList[i].len_SA_Cient);
            }
        }
        //clear buffer
        memset(RecvfromClient, 0, MAX_RECVFROMCLIENT);
        memset(ReadytoSend, 0,MAX_READYTOSEND);
    }

    return 0;
}

int InitClientList()
{
    int i;

    for (i = 0;i < MAX_CONNECT;i++)
    {
        ClientList[i].fd_client = 0;
        memset(ClientList[i].ID, 0, MAX_ID);
        ClientList[i].flag_protocol = -1;
        ClientList[i].len_SA_Cient = sizeof(ClientList[i].SA_Client);
        memset(&ClientList[i].SA_Client,0,len_SA_server);
    }
    
    return 0;
}

int main(int argc, char *argv[])
{
    pid_t pid;

    if (argc == 2)
    {
        InitClientList();

        memset(&SA_server,0,sizeof(SA_server));
        SA_server.sin_family = AF_INET;
        SA_server.sin_addr.s_addr = htonl(INADDR_ANY);
        SA_server.sin_port = htons(atoi(argv[1]));
        len_SA_server = sizeof(SA_server);

        fd_TCP = socket(AF_INET, SOCK_STREAM, 0);
        if (bind(fd_TCP, (struct sockaddr *)&SA_server, len_SA_server) == -1)
        {
            printf("Fail to bind: %s\n", strerror(errno));
            return -1;
        }
        fd_UDP = socket(AF_INET, SOCK_DGRAM, 0);
        if (bind(fd_UDP, (struct sockaddr *)&SA_server, len_SA_server) == -1)
        {
            printf("Fail to bind: %s\n", strerror(errno));
            return -1;
        }
        pid = fork();
        //fail to fork
        if (pid < 0)
            exit(-1);
        //child process, manage TCP
        if (pid == 0)
        {
            TCPChatRoom();
        }
        //parent process, manage UDP
        else
        {
            UDPChatRoom();
        }
    }

    return 0;    
}