
#include <stdlib.h>
#include "body.h"
using namespace std;

int main(int argc, char* argv[]){
    sleep(atoi(argv[1]));  //задержка 
    //запускаем функцию body, котора запишет информацию о процессе в файл
    body(argv[0], argv[2]);
    exit(EXIT_SUCCESS);
}