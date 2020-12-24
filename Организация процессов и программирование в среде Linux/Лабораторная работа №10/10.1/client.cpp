#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <stdio.h>

const int SEM_WRITE = 0,
          SEM_READ = 1,
          SEM_END = 2;

using namespace std;


int main(int argc, char* argv[])
{
    key_t key = 150;
    int shmid, 
        semid,
        buffSize,
        *buff;

    buffSize = 5;
     //запрос на разделяемый сегмент памяти
    if ((shmid = shmget(key, 128, 0666)) == -1)
        perror("shmget");

     //включение разделяемой памяти в пространство процесса
    buff = (int*)shmat(shmid, NULL, 0);

    //открываем множественный семафор(с количеством простых семафоров равным 3)
    if ((semid = semget(key, 3, 0666)) == -1)
        perror("semget");

    //создаем списки команд для работы с множественным семафором
    struct sembuf operations[3];
    //инициализируем список команд для записи
    operations[SEM_WRITE].sem_num = SEM_WRITE;
    operations[SEM_WRITE].sem_op = 1;     //увеличиваем на 1 - открываем семафор на запись
   //инициадизируем список команд для чтения
    operations[SEM_READ].sem_num = SEM_READ;
    operations[SEM_READ].sem_op = -1;     //уменьшаем на 1 - включаем в очередь к семафору
    //инициализируем список команд для определения завершения работы
    operations[SEM_END].sem_num = SEM_END;
    operations[SEM_END].sem_op = 1;       //увеличиваем на 1 - открываем семафор на завершение работы

    int readIndex = 0;
    while(buff[readIndex] != EOF)
    {
        //выполняем операцию для семафора чтения
        semop(semid, operations + 1, 1);
        printf("%c", buff[readIndex]);
        readIndex++;
        //выполняем операцию для семафора записи
        semop(semid, operations, 1);
    }
    //выполняем операцию для семафора завершения работы и тем самым
    //позволяем писателю завершить работу
    semop(semid, operations + 2, 1);
    semctl(semid, IPC_RMID, 0); // Удаляем множество семафоров
    shmdt((void*)buff); // Исключаем сегмент разделяемой памяти из адресного пространства текущего процесса
    shmctl(shmid, IPC_RMID, 0);
    return 0;
} 