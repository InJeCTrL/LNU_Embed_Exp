/*
    As a client, I will send ![REG]string to register a new ID,
                             ![MSG]string to send userdata,
                             ![SF]filepath to send a file,
                             ![RF]filepath to receive a file,
                             ![QUIT] to quit the chat room,
                        receive ![ACCREG] / ![REJREG] from the server to ensure the register.
    I will receive string from server, the format is [time ID]\nuserdata.
    While sending a file, I will send ![SF]filename first, then send the file.
    While receiving a file, I will send ![RF]filename first, then receive the file until the buffer received is NULL.
*/

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/stat.h>

#define TRUE            1
#define MAX_USERDATA    8192                  //max length of userdata string
#define MAX_ID          1024                  //max length of ID string
#define MAX_TIME        20                    //max length of time string
#define MAX_READYTOSEND (MAX_USERDATA + 6)    //max length of ReadytoSend string
#define MAX_RECVFROMSERV    (MAX_TIME + MAX_ID + MAX_USERDATA + 2)  //max length of RecvfromServ string
#define MAX_REG         (MAX_ID + 6)          //max length of register string
#define MAX_REGRESULT   10                    //max length of register sendback string
#define MAX_FILELEN     (1024 * 1024)         //max length of file content

void* th_Recv(void *);

struct sockaddr_in SA; //save target address
int len_SA;            //length of SA
int fd_server;         //file descriptor of target server
pthread_t thread_RS, thread_Snd;//thread IDs of recv and send
int mode_protocol = -1;  //0:TCP 1:UDP
void *ret1 = NULL, *ret2 = NULL;    //return value of two threads

void* th_Send(void *data)
{
    char Userdata[MAX_USERDATA] = { 0 };             //userdata to be filled
    char ReadytoSend[MAX_READYTOSEND] = { 0 };       //![REG]userdata
    long len;
    FILE *fp = NULL;                                 //file pointer of the file ready to be sent
    struct stat FileInfo;                            //record file information
    long ReadLen = 0;                                //length of file part read everytime
    unsigned char Buf_File[MAX_READYTOSEND] = { 0 }; //buffer used to save content of file

    //input string and send
    while(TRUE)
    {
        scanf("%s", Userdata);
        //client send quit, send quit msg and exit thread
        if (!strncmp(Userdata, "![QUIT]", 7))
        {
            strcpy(ReadytoSend, Userdata);
            //TCP
            if (!mode_protocol)
            {
                send(fd_server, ReadytoSend, MAX_READYTOSEND, MSG_NOSIGNAL);
            }
            //UDP
            else
            {
                sendto(fd_server, ReadytoSend, MAX_READYTOSEND, MSG_NOSIGNAL, (struct sockaddr *)&SA, len_SA);
            }
            break;
        }
        //client send file
        else if (!strncmp(Userdata, "![SF]", 5))
        {
            stat(Userdata + 5, &FileInfo);
            if (FileInfo.st_size > 1024 * 1024)
            {
                printf("No larger than 1 MiB!\n");
            }
            else
            {
                //send ![SF]filename
                strcpy(ReadytoSend, "![SF]");
                strcat(ReadytoSend, strchr(Userdata + 5, '/') ? strchr(Userdata + 5, '/') + 1 : Userdata + 5);
                //TCP
                if (!mode_protocol)
                {
                    send(fd_server, ReadytoSend, MAX_READYTOSEND, MSG_NOSIGNAL);
                }
                //UDP
                else
                {
                    sendto(fd_server, ReadytoSend, MAX_READYTOSEND, MSG_NOSIGNAL, (struct sockaddr *)&SA, len_SA);
                }
                printf("Sending...\n");
                //send file content
                fp = fopen(Userdata + 5, "r");
                memset(Buf_File, 0, MAX_READYTOSEND);
                ReadLen = fread(Buf_File, 1, MAX_FILELEN, fp);
                //TCP
                if (!mode_protocol)
                {
                    send(fd_server, Buf_File, ReadLen, MSG_NOSIGNAL);
                    send(fd_server, Buf_File, ReadLen, MSG_NOSIGNAL);
                }
                //UDP
                else
                {
                    sendto(fd_server, Buf_File, ReadLen, MSG_NOSIGNAL, (struct sockaddr *)&SA, len_SA);
                    sendto(fd_server, Buf_File, ReadLen, MSG_NOSIGNAL, (struct sockaddr *)&SA, len_SA);
                }
                fclose(fp);
                printf("OK\n");
            }
        }
        //client send file request
        else if (!strncmp(Userdata, "![RF]", 5))
        {
            strcpy(ReadytoSend, Userdata);
            //TCP
            if (!mode_protocol)
            {
                send(fd_server, ReadytoSend, MAX_READYTOSEND, MSG_NOSIGNAL);
            }
            //UDP
            else
            {
                sendto(fd_server, ReadytoSend, MAX_READYTOSEND, MSG_NOSIGNAL, (struct sockaddr *)&SA, len_SA);
            }
        }
        //client send normal messages
        else
        {
            strcpy(ReadytoSend, "![MSG]");      //Add ![REG] to ReadytoSend
            strcat(ReadytoSend, Userdata);      //connect userdata to ReadytoSend
        }
        //TCP
        if (!mode_protocol)
        {
            send(fd_server, ReadytoSend, MAX_READYTOSEND, MSG_NOSIGNAL);
        }
        //UDP
        else
        {
            sendto(fd_server, ReadytoSend, MAX_READYTOSEND, MSG_NOSIGNAL, (struct sockaddr *)&SA, len_SA);
        }
        //clear buffers
        memset(ReadytoSend, 0, MAX_READYTOSEND);
        memset(Userdata, 0, MAX_USERDATA);
    }

    return 0;
}

void* th_Recv(void *data)
{
    char RecvfromServ[MAX_RECVFROMSERV] = { 0 };    //formatted string received from the server
    FILE *fp = NULL;                                 //file pointer of the file ready to be received
    long ReadLen = 0;                                //length of file part written everytime
    unsigned char Buf_File[MAX_RECVFROMSERV] = { 0 }; //buffer used to save content of file

    while (TRUE)
    {
        //TCP
        if (!mode_protocol)
        {
            recv(fd_server, RecvfromServ, MAX_RECVFROMSERV, MSG_NOSIGNAL);
        }
        //UDP
        else
        {
            recvfrom(fd_server, RecvfromServ, MAX_RECVFROMSERV, MSG_NOSIGNAL, (struct sockaddr *)&SA, &len_SA);
        }
        if (!strncmp(RecvfromServ, "![QUIT]", 7))
        {
            break;
        }
        //server sendback success flag of receiving a file
        else if (!strncmp(RecvfromServ, "![ACCRF]", 8))
        {
            //create file
            fp = fopen(RecvfromServ + 8, "w");
            //fill file content
            //TCP
            if (!mode_protocol)
            {
                ReadLen = recv(fd_server, Buf_File, MAX_RECVFROMSERV, MSG_NOSIGNAL);
            }
            //UDP
            else
            {
                ReadLen = recvfrom(fd_server, Buf_File, MAX_RECVFROMSERV, MSG_NOSIGNAL, (struct sockaddr *)&SA, &len_SA);
            }
            //only ![ACCRF] header to save
            if (!strncmp(Buf_File, "![ACCRF]", 8))
                fwrite(Buf_File + 8, 1, ReadLen - 8, fp);
            fclose(fp);
            printf("File has been downloaded!\n");
        }
        //server sendback error flag of receiving a file
        else if (!strncmp(RecvfromServ, "![REJRF]", 8))
        {
            printf("File received error!\n");
        }
        //server broadcast normal messages
        else if (!strncmp(RecvfromServ, "[", 1))
        {
            printf("%s\n\n", RecvfromServ);
        }
        memset(RecvfromServ, 0, MAX_RECVFROMSERV);
    }

    return 0;
}

int Register()
{
    char str_inputID[MAX_REG] = { 0 };
    char str_REG[MAX_REG] = { 0 };     //register string
    char ReadytoSend[MAX_READYTOSEND] = { 0 };  //readytosend string
    char str_REGresult[MAX_REGRESULT] = { 0 };  //register sendback string

    while (TRUE)
    {
        printf("Please register first\nformat:ID(no space, Max length=1023 Bytes):");
        strcpy(str_REG, "![REG]");
        scanf("%s", str_inputID);
        strcat(str_REG, str_inputID);
        strcpy(ReadytoSend, str_REG);
        //TCP
        if (!mode_protocol)
        {
            send(fd_server, str_REG, MAX_REG, MSG_NOSIGNAL);
            recv(fd_server, str_REGresult, MAX_REGRESULT, MSG_NOSIGNAL);
        }
        //UDP
        else
        {
            sendto(fd_server, str_REG, MAX_REG, MSG_NOSIGNAL, (struct sockaddr *)&SA, len_SA);
            recvfrom(fd_server, str_REGresult, MAX_REGRESULT, MSG_NOSIGNAL, (struct sockaddr *)&SA, &len_SA);
        }
        //get accept from server
        if (!strncmp(str_REGresult, "![ACCREG]", MAX_REGRESULT - 1))
        {
            printf("Register successfully!\n");
            break;
        }
        printf("\nRetry another ID to register!\n");
        //clear buffers
        memset(str_REG, 0, MAX_REG);
        memset(str_inputID, 0, MAX_REG);
        memset(ReadytoSend, 0, MAX_READYTOSEND);
        memset(str_REGresult, 0, MAX_REGRESULT);
    }

    return 0;
}

int TCPChatRoom()
{
    fd_server = socket(AF_INET, SOCK_STREAM, 0);    //set file descriptor of TCP
    connect(fd_server, (struct sockaddr *)&SA, len_SA);
    Register();
    pthread_create(&thread_RS, NULL, th_Recv, 0);
    pthread_create(&thread_Snd, NULL, th_Send, 0);
    pthread_join(thread_Snd, &ret1);        //wait for send thread to quit
    pthread_join(thread_RS, &ret2);        //wait for recv thread to quit
    close(fd_server);
    
    return 0;
}

int UDPChatRoom()
{
    pthread_t thread_RS, thread_Snd;//thread IDs of recv and send
    void *ret1 = NULL, *ret2 = NULL;

    fd_server = socket(AF_INET, SOCK_DGRAM, 0);    //set file descriptor of UDP
    Register();
    pthread_create(&thread_RS, NULL, th_Recv, 0);
    pthread_create(&thread_Snd, NULL, th_Send, 0);
    pthread_join(thread_Snd, &ret1);        //wait for send thread to quit
    pthread_join(thread_RS, &ret2);        //wait for recv thread to quit
    
    return 0;
}

int main(int argc, char *argv[])
{
    char sel;

    if (argc == 3)
    {
        memset(&SA,0,sizeof(SA));
        SA.sin_family = AF_INET;
        SA.sin_port = htons(atoi(argv[2]));
        inet_pton(AF_INET,argv[1],&SA.sin_addr);
        len_SA = sizeof(SA);
        printf("In the ChatRoom:\n\"![SF][FilePath]\" to send a file.\n\"![RF][FileName]\" to recv a file and save to the current folder.\n\"![QUIT]\" to exit the chatroom\n1.TCP ChatRoom\n2.UDP ChatRoom\n");
        do
        {
            sel = getchar();
            getchar();
            if (sel == '1')
            {
                mode_protocol = 0;
                TCPChatRoom();
                continue;
            }
            else if (sel == '2')
            {
                mode_protocol = 1;
                UDPChatRoom();
                continue;
            }
            system("clear");
        }while (sel != '1' && sel != '2');
    }

    return 0;    
}