/*Задание: Напишите 3 программы, которые запускаются в произвольном порядке и построчно записывают свои
 индивидуальные данные в один файл. Пока не закончит писать строку одна программа, другие две не должны обращаться к файлу. 
 Интервалы записи в файл и количество записываемых строк определяются входными параметрами, 
 задаваемыми при запуске каждой программы. При завершении работы одной из программ, другие должны продолжить свою работу. 
 Синхронизация работы программ должна осуществляться с помощью общих переменных, размещенных в разделяемой памяти.*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fstream>

const int KEY = 128,
        NUM_PROCESS = 3,
        NUM = 0;

struct shared {
    int choosing[NUM_PROCESS];
    int number[NUM_PROCESS];
    int stop;
};

int open_file();
int close_file();
/* Полное уничтожение разделяемого сегмента */
int dest_shm(int);
int und_shm(void* shmaddr);

std::ofstream file;

int id_shm = -1;
//Алгоритм Петерсона
void lock();

shared* shared_var;
/* argv[1] - количество строк, argv[2] - период записи */
int main(int argc, char* argv[]) {
    void* shmaddr = NULL; /* Указатель на виртуальный адрес */
    int count_str, period, counter;

    if (argc != 3) {
        exit(EXIT_FAILURE);
    } else {
        count_str = atoi(argv[1]);
        period = atoi(argv[2]);
    }
    //запрос на разделяемый сегмент памяти
    if ((id_shm = shmget((key_t)KEY, sizeof(shared), (0666 | IPC_CREAT))) == -1) {
        perror("req_shm -> shmget");
        return -1;
    }
    //включение разделяемой памяти в пространство процесса
    //0 - чтение и запись
    if ((int*)(shmaddr = shmat(id_shm, NULL, 0)) == (int*)-1) {
        perror("shmat");
    }

    shared_var = (shared*)shmaddr;
    time_t t;
    tm* current_time;
    counter = 0;

    while (1) {
        /* Период записи */
        sleep(period);
        t = time(NULL);
        current_time = localtime(&t);
        printf("Программа №%d. Процесс готовится ко входу в критическую секцию. Время: %s\n", NUM + 1, asctime(current_time));
        lock();

        /* Критическая секция */
        if (counter < count_str) 
        {
            file.open("file.txt", std::ios::out | std::ios::app);
            if(!file)
            {
                perror("open file");
                exit(EXIT_FAILURE);
            }
            t = time(NULL);
            current_time = localtime(&t);
            file << counter + 1 << ". Программа №" << NUM + 1 << ". Время: " << asctime(current_time) << std::endl;
            file.close();
            counter++;         
        
        } 
        else 
        {
            (shared_var->stop)++;
            shared_var->number[NUM] = 0;
            break;
        }

        shared_var->number[NUM] = 0;
        t = time(NULL);
        current_time = localtime(&t);
        printf("Программа №%d. Процесс вышел из критической секции. Время: %s\n", NUM+1, asctime(current_time));
    }
    if (shared_var->stop == NUM_PROCESS) {
        file.close();
        if (shmdt(shmaddr) == -1) 
        {
            perror("und_shm -> shmdt");
        }
        if (shmctl(id_shm, IPC_RMID, NULL) == -1) 
        {
            perror("dest_shm -> shmctl+IPC_RMID");
        }
    }

    return EXIT_SUCCESS;
}
int max(int a, int b)
{
    int res = (a > b) ? a : b;
    return res;
}

bool less(int a, int b, int c, int d)
{
    if(a < c)
        return true;
    if (a == c && b < d)
        return true;
    return false;
}
//Алгоритм Пекарни
void lock() {
    shared_var->choosing[NUM] = 1;
    shared_var->number[NUM] = 1 + max(shared_var->number[0], 
                                        max(shared_var->number[1], shared_var->number[2]));
    shared_var->choosing[NUM] = 0;
    for (int j = 0; j < NUM_PROCESS; ++j) {
       // if (j != NUM) {
            while (shared_var->choosing[j]);
            while (shared_var->number[j] != 0 && less(shared_var->number[j], j, shared_var->number[NUM],NUM));
       // }
    }
    /* Критическая секция */
}