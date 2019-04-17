#include<stdio.h>
#include<stdlib.h>
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

    //try to get SHMem
    IDSHMem = shmget(ID_SHMem, BufferSize, 0666 | IPC_CREAT);
    if (IDSHMem == -1)
    {
        printf("Get SHMem segment failed!\n");
        return -1;
    }
    printf("Get SHMem segment succeeded!\n");
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
    //read filedata from SharedData segment
    strcpy(FileData, ptr_SharedData);
    printf("Read data from shared memory succeeded!\n");
    //test FileData
    printf("Check the file data:\n[%s]\n", FileData);
    //unmap shared memory from this process
    shmdt(ptr_Mem);
    printf("Unmap shared memory segment succeeded!\n");
    //delete shared memory
    shmctl(IDSHMem, IPC_RMID, 0);
    printf("Delete shared memory segment succeeded!\n");
    //write file content
    pF = fopen("AwesomeFile_cpy.bin", "w");
    fwrite(FileData, 1, strlen(FileData) + 1, pF);
    fclose(pF);
    printf("Write file content succeeded!\n");
    //test file content
    printf("Check the file content:\n");
    system("cat AwesomeFile_cpy.bin");

    return 0;
}