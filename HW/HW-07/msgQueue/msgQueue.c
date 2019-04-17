#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/msg.h>

#define MAX_MSGLEN 1024
#define ID_MSGQUEUE 666
#define TRUE 1

typedef struct
{
    long msg_type;
    unsigned char msg[MAX_MSGLEN];
}MSG_ATOM;

int main(void)
{
    MSG_ATOM msgpkg;
    int msgQID;
    unsigned char msgbuf[MAX_MSGLEN] = { 0 };
	pid_t pid;

    //try to create message queue
    msgQID = msgget(ID_MSGQUEUE, 0666 | IPC_CREAT);
    if (msgQID == -1)
    {
        printf("Create Message Queue Error!\n");
		return -1;
    }
	printf("TIP: Send [QUIT] to quit.\n");
	pid = fork();
	//fork failed
	if (pid < 0)
	{
		printf("Fork Process Error!\n");
		return -1;
	}
	//parent process
	else if (pid)
	{
		while (TRUE)
		{
			printf("This is parent process, say something to child:\n");
			scanf("%s", msgbuf);
            strcpy(msgpkg.msg, msgbuf);
            msgpkg.msg_type = 1;
			//send type1 msg
			if (msgsnd(msgQID, &msgpkg, MAX_MSGLEN, 0) == -1)
            {
                printf("Send message failed!\n");
                return -1;
            }
			//recv type2 msg
            if (msgrcv(msgQID, &msgpkg, MAX_MSGLEN, 2, 0) == -1)
            {
                printf("Recv message failed!\n");
                return -1;
            }
			//parent recv QUIT msg, delete msgQueue and exit.
			if (!strcmp(msgpkg.msg, "QUIT"))
			{
				msgctl(msgQID, IPC_RMID, 0);
				printf("Parent process exit!\n");
				return 0;
			}
			//show
			printf("This is parent process, heard from child:\n%s\n\n", msgpkg.msg);
		}
	}
	//child process
	else
	{
		while (TRUE)
		{
			//recv type1 msg
			if (msgrcv(msgQID, &msgpkg, MAX_MSGLEN, 1, 0) == -1)
            {
                printf("Recv message failed!\n");
                return -1;
            }
			//child recv QUIT msg, send QUIT to parent and exit
			if (!strcmp(msgpkg.msg, "QUIT"))
			{
				msgpkg.msg_type = 2;
				strcpy(msgpkg.msg, "QUIT");
				if (msgsnd(msgQID, &msgpkg, MAX_MSGLEN, 0) == -1)
				{
					printf("Send message failed!\n");
					return -1;
				}
				printf("Child process exit!\n");
				return 0;
			}
			//show
			printf("This is child process, heard from parent:\n%s\n\n", msgpkg.msg);
			printf("This is child process, say something to parent:\n");
			scanf("%s", msgbuf);
            strcpy(msgpkg.msg, msgbuf);
            msgpkg.msg_type = 2;
			//send type2 msg
			if (msgsnd(msgQID, &msgpkg, MAX_MSGLEN, 0) == -1)
            {
                printf("Send message failed!\n");
                return -1;
            }
			//child send QUIT msg, exit
			if (!strcmp(msgbuf, "QUIT"))
			{
				printf("Child process exit!\n");
				return 0;
			}
		}
	}
	
	return 0;
}