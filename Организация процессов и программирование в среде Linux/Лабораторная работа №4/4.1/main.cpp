/* Задание: Напишитие программу, которая открывает текстовый файл, порождает поток, а затем ожидает его завершения.
Поток выводит на экран класс планирования, текущий, минимальный и максимальные приоритеты, содержимое файла и закрывает файл
После завершения работы потока программа должна вывести текущий приоритет и проверить - закрыт ли файл,
и если он не закрыт, то принудительно закрыть*/

/*линковка:
    g++ -Wall -pthread main.cpp -o <anyNameFile> 
*/
#include <fcntl.h>	/* open() and O_XXX flags */
#include <sys/types.h>	
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>	/* close() */
#include <iostream>
#include <pthread.h>
#include <errno.h>
#include "sched.h"
//структура для передачи аргументов потоку
struct argStruct{
    int fd;          //дескриптор файла
};
//функция, которую будет выполнять поток
void* threadFunc(void* args){
    //приведение аргументов к необходимому типу argStruct
    argStruct* arg = (argStruct*) args;
    //вспомогательная переменная для чтения файла, по которой определяем закончилось ли чтение файла
    ssize_t ret = 0;
    //вспомогательная переменная, в которую будет записан текущий прочитанный символ
    char ch;
    //вспомогательная переменная для чтения текущего приоритета процесса
    //получаем текущий pid процесса
    pid_t pid = getpid();
    //получаем номер класса планирования
    int policy = sched_getscheduler(pid);
    //и выводим его значение на экран
    printf("Класс планирования процесса: ");
    switch(policy){
        case SCHED_OTHER:
            printf("SCHED_OTHER\n");
            break;
        case SCHED_BATCH:
            printf("SCHED_BATCH\n");
            break;
        case SCHED_IDLE:
            printf("SCHED_OTHER\n");
            break;
        case SCHED_FIFO:
            printf("SCHED_FIFO\n");
            break;
        case SCHED_RR:
            printf("SCHED_RR\n");
    }
    //получаем значение приоритета по pid процессу
    printf("Текущий статический приоритет процесса-потока: %d\n",getpriority(PRIO_PROCESS,pid ));
    printf("Максимальный разрешенный статический приоритет: %d\n",sched_get_priority_max(policy));
    printf("Минимальный разрешенный статический приоритет: %d\n", sched_get_priority_min(policy));
    printf("\nСодержимое файла: \n");
    //Выводим содержимое файла
    while ((ret = read (arg->fd, &ch, 1)) > 0)
	{
		putchar (ch);
	}
    //закрываем файл
  //  close(arg->fd);
    return 0;
}
//главная функция
int main(){
    //переменная потока
    pthread_t thread;
    //переменная с передаваемыми параметрами потоку
    argStruct arg;
    int status;     //вспомогательная переменная для определения ошибок
    //вспомогательная переменная для чтения текущего приоритета процесса
    //имя файла
    char nameFile[] = "file.txt";
    //открываем файл с именем nameFile только для чтения и возвращаем его дескриптор в переменную fd
    int fd = open(nameFile, O_RDONLY);
    //заполняем структуру для передачи данных в поток
    arg.fd = fd;
    //создаем поток с выполнением функции threadFunc с аргументами arg
    status = pthread_create(&thread, NULL, threadFunc, (void*) &arg);
    //Если поток создать не удалось выводим ошибку
    if(status != 0){
        printf("Невозможно создать поток, код ошибки: %d\n", status); 
        exit(-11);
    }
    //приостанавливаем основной поток, пока не завершится поток thread
    status = pthread_join(thread, NULL);
    if(status != 0){
       printf("Невозможно приостановить основной поток, код ошибки: %d\n", status);
        exit(-12);
    }
    printf("\nТекущий статический приоритет процесса: %d\n",getpriority(PRIO_PROCESS,getpid()));
    //с помощью fcntl пытаемся получить флаги дескриптора fd, если произошла ошибка функция вернет -1 и 
    //сгенерируется ошибка, что fd не является открытым дескриптором
    if(fcntl(fd, F_GETFD) != -1 || errno != EBADF){
        //если поток не закрыл файл - главный процесс должен его закрыть
        if(close(fd) != -1){
            printf("Файл не был закрыт потоком, но теперь он закрыт родительским процессом\n");
        }
    }
    else {
        printf("Файл был успешно закрыт потоком\n");
    }

}