/*Задание: написать программу, которая открывает входной файл и 2
выходных файла. Затем она должна в цикле построчно читать входной файл
и порождать 2 потока. Одному потоку передавать нечетную строку, 
а другому - четную. Оба потока должны работать параллельно. Каждый
поток записывает в свой выходной файл полученную строку и завершает работу.
Программа должна ожидать завершения работы каждого потока и повторять
цикл порождения потоков и чтения строк входного файла, пока не прочтет
последнюю строку, после чего закрыть все файлы */

/*Линковка: g++ -Wall -pthread main.cpp -o <Имя бинарника> */

#include <fcntl.h>
#include <string> // подключаем строки
#include <fstream> // подключаем файлы
#include <pthread.h>
#include <unistd.h>	/* close() */
#include <sys/types.h>	
#include <sys/stat.h>	

using namespace std;

//структура для передачи аргументов потоку
struct argStruct{

    int fd;           //дескриптор файла
    string str;       //строка
};
//функция, которую будет выполнять поток по нечетным строкам
void* threadFuncOdd(void* args){
    //приведение аргументов к необходимому типу argStruct
    argStruct* arg = (argStruct*) args;
    //записываем в файл с дескриптором fd строку str с длиной str.length()
    write(arg->fd,(arg->str).c_str(),arg->str.length());
    return 0;

}

//функция, которую будет выполнять поток по четным строкам
void* threadFuncEven(void* args){
    //приведение аргументов к необходимому типо argStruct
    argStruct* arg = (argStruct*) args;
    //Записываем в файл с дескриптором fd строку str с длиной str.length()
    write(arg->fd, (arg->str).c_str(), arg->str.length());
    return 0;

}

int main(){
    string str;              //считываемая строка с файла
    pthread_t threads[2];    //массив потоков, содержит два потока
    argStruct arg;           //передаваемый аргумент функциям потоков
    //открываем файл mainFile.txt, из которого будем считывать строчки
    ifstream mainFile("mainFile.txt");
    //создаем файлы для потоков, возвращается дескрипторы открытых файлов
    int fdOdd = open("oddFile.txt", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH );
    int fdEven = open("evenFile.txt", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
    //пока файл не пуст считываем каждую строку 
    for(int i = 1; getline(mainFile, str); i++){
        //добавляем к строке символ переноса строки для удобств
            arg.str = str+'\n';
            //если считывается нечетная по порядку строка
            if((i % 2) == 1){
                //записываем дескриптор файла для потока
                arg.fd= fdOdd;
                //создаем поток, который запустит функцию threadFuncOdd с аргументами arg
                pthread_create(&threads[0], NULL, threadFuncOdd, (void*) &arg);
            }
            //если считывается четная по порядку строка
            else if((i %2) == 0){
                //создаем поток, который запустит функцию threadFuncEven с аргументами arg
                arg.fd = fdEven;
                pthread_create(&threads[1],NULL, threadFuncEven, (void*) &arg);
            }
            //приостанавливаем основной процесс, пока потоки не завершатся
             pthread_join(threads[0], NULL);
             pthread_join(threads[1], NULL);
    }
    //закрываем все файлы
    mainFile.close();
    close(fdOdd);
    close(fdEven);
}