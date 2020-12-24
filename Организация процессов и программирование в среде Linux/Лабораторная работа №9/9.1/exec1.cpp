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
#include <assert.h>

const int KEY = 128,
        NUM_PROCESSES = 3,
        NUM = 0;

struct shared {
    int choosing[NUM_PROCESSES];
    int number[NUM_PROCESSES];
    int stop;
};

std::ofstream file;

//Алгоритм Булочной
void lock();

shared* shared_var;
/* argv[1] - количество строк, argv[2] - период записи */
int main(int argc, char* argv[]) {

    int id_shm;
    void* shmaddr = NULL; /* Указатель на виртуальный адрес */
    int count_str, period, counter;
    assert(argc == 3);

    count_str = atoi(argv[1]);
    period = atoi(argv[2]);
    //запрос на разделяемый сегмент памяти
    if ((id_shm = shmget((key_t)KEY, sizeof(shared), (0666 | IPC_CREAT))) == -1) {
        perror("shmget");
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

    if (shared_var->stop == NUM_PROCESSES) {
        //отсоединение сегмента
        if (shmdt(shmaddr) == -1) 
        {
            perror("shmdt");
        }
        //удаление сегмента
        if (shmctl(id_shm, IPC_RMID, NULL) == -1) 
        {
            perror("shmctl+IPC_RMID");
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
//Алгоритм булочной
void lock() {
    shared_var->choosing[NUM] = 1;
    shared_var->number[NUM] = 1 + max(shared_var->number[0], 
                                        max(shared_var->number[1], shared_var->number[2]));
    shared_var->choosing[NUM] = 0;
    for (int j = 0; j < NUM_PROCESSES; ++j) {
            //ждем пока поток j получит свой номер
            while (shared_var->choosing[j]);
            //ждем пока все потоки с меньшим номером или с таким же номером, но с более высоким приоритетом, закончат свою работу
            while (shared_var->number[j] != 0 && less(shared_var->number[j], j, shared_var->number[NUM],NUM));
    }
    /* Критическая секция */
}