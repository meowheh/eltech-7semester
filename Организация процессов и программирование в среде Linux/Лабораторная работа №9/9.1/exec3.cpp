
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
        NUM = 2;

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
   
    counter = 0;
    
   while (counter < count_str) {
        lock();
        printf("%d. Программа №%d. Процесс готовится ко входу в критическую секцию.\n", counter + 1, NUM + 1);
      
            file.open("file.txt", std::ios::out | std::ios::app);
            if(!file)
            {
                perror("open file");
                exit(EXIT_FAILURE);
            }
            file << counter + 1 << ". Программа №" << NUM + 1 << std::endl;
            file.close();

            shared_var->number[NUM] = 0;

            printf("%d. Программа №%d. Процесс вышел из критической секции.\n",counter + 1, NUM+1 );
            counter++;  
            sleep(period);       
    }
    (shared_var->stop)++;
    shared_var->number[NUM] = 0;
    if (shared_var->stop == NUM_PROCESS) {
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

void lock() {
    shared_var->choosing[NUM] = 1;
    shared_var->number[NUM] = 1 + max(shared_var->number[0], 
                                        max(shared_var->number[1], shared_var->number[2]));
    shared_var->choosing[NUM] = 0;
    for (int j = 0; j < NUM_PROCESS; ++j) 
    {
            while (shared_var->choosing[j]);
            while (shared_var->number[j] != 0 && less(shared_var->number[j], j, shared_var->number[NUM],NUM));
    }
    /* Критическая секция */
}