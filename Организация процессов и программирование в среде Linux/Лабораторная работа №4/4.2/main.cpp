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
#include <iostream>

using namespace std;

//структура для передачи аргументов потоку
struct argStruct{

    string str;       //строка
    int fd;           //дескриптор файла
    //конструктор с аргументами
    argStruct(string str, int fd): fd(fd), str(str) {};
    //конструктор по умолчанию
    argStruct() {};

};
//функция, которую будет выполнять поток по нечетным строкам
static void* threadFuncOdd(void* args){
    //приведение аргументов к необходимому типу argStruct
    argStruct arg = *((argStruct*) args);
    //записываем в файл с дескриптором fd строку str с длиной str.length()
    write(arg.fd,(arg.str).c_str(),(arg.str).length());
    //завершаем работу нити
    pthread_exit(0);

}

//функция, которую будет выполнять поток по четным строкам
static void* threadFuncEven(void* args){
    //приведение аргументов к необходимому типо argStruct
    argStruct arg = *((argStruct*) args);
    //Записываем в файл с дескриптором fd строку str с длиной str.length()
    write(arg.fd, (arg.str).c_str(), (arg.str).length());
    //завершаем работу нити
    pthread_exit(0);

}
 
int main(){
    pthread_t threadOdd,      //1 поток
              threadEven;     //2 поток
    pthread_attr_t attr;      //атрибуты потоков
    pthread_attr_init(&attr); //инициализируем атрибуты
    //открываем файл mainFile.txt, из которого будем считывать строчки
    ifstream mainFile("mainFile.txt");
    //создаем файлы для потоков, возвращается дескрипторы открытых файлов
    int fdOdd = open("oddFile.txt", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH );
    int fdEven = open("evenFile.txt", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
    //объявляем статические переменные аргументов функций потоков
    static argStruct arg1, arg2;
    //флаги завершения чтения файла, если файл содержит четное количество строк или нечетное
    bool isNotEndOdd = true;
    bool isNotEndEven = true;
    //пока не достигнут конец файла
    while((isNotEndEven && isNotEndOdd)){
        //переменные, куда будут записываться строки из файла
        string str1, str2;
        //считываем строку по нечетному порядку
        if(getline(mainFile,str1)){
            //присваиваем аргументы функции - строка и дескриптор файла
            arg1 = argStruct(str1+"\n",fdOdd);
            //создаем нить, которая выполнит функцию threadFuncOdd с аргументами arg1
            pthread_create(&threadOdd, &attr, &threadFuncOdd, &arg1);  
         }
         else{ //если чтение не удалось - значит достигли конца файла
             isNotEndOdd = false;
         } //считываем четную по порядку строку
         if(getline(mainFile,str2)){
             //присваиваем аргументы функции - строка и дескриптор файла
            arg2 = argStruct(str2+"\n",fdEven);
            //создаем нить, которая выполнит функцию threadFuncEven с аргументами arg2
            pthread_create(&threadEven, &attr, &threadFuncEven, &arg2);
         }
         else{ //если чтение не удалось - достигл конца файла
             isNotEndEven = false;
         }
         //приостанавливаем основный процесс, пока потоки не завершатся
        pthread_join(threadOdd, NULL);
        pthread_join(threadEven, NULL);
    }
    //удаляем аттрибуты потоков
    pthread_attr_destroy(&attr);
    //закрываем все файла
    mainFile.close();
    close(fdOdd);
    close(fdEven);
}