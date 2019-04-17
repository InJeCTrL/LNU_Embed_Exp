#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define BufferSize 8192
#define ID_SHMem 666

int main(void)
{
    int IDSHMem;
    void *ptr_Mem = NULL;
    FILE *pF = NULL;
    unsigned char *ptr_SharedData = NULL;
	unsigned char FileData[BufferSize] = { 0 };

    //get file content
    pF = fopen("AwesomeFile.bin", "r");
    while (fread(FileData, 1, BufferSize, pF));
    fclose(pF);
    printf("Get file content succeeded!\n");
    //try to create SHMem
    IDSHMem = shmget(ID_SHMem, BufferSize, 0666 | IPC_CREAT);
    if (IDSHMem == -1)
    {
        printf("Create SHMem segment failed!\n");
        return -1;
    }
    printf("Create SHMem segment succeeded!\n");
    //try to map memory segment
    ptr_Mem = shmat(IDSHMem, 0, 0);
    if (ptr_Mem == (void *)-1)
    {
        printf("Map memory failed!\n");
        return -1;
    }
    printf("Map shared memory succeeded!\n");
    //point shared data segment to SHMem
    ptr_SharedData = (unsigned char*)ptr_Mem;
    printf("Point SharedData segment to SHMem succeeded!\n");
    //write filedata to SharedData segment
    strcpy(ptr_SharedData, FileData);
    printf("Write file content to shared memory succeeded!\n");
    //test SHMem
    printf("Check the shared memory segment:\n[%s]\n", ptr_SharedData);
    //unmap shared memory from this process
    shmdt(ptr_Mem);
    printf("Unmap shared memory segment succeeded!\n");

    return 0;
}