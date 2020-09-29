#include <string>
#include <fstream>
#include "unistd.h"

using namespace std;
//функция записи в файл информации о процессе
void body(char* nameFile, char* process){
    ofstream file(string(nameFile), ios_base::app); //открываем файл для записи
    if(file.is_open()){ //если файл успешно открыт
        file << process << ":\n" << //записываем тип процесса: Родитель/Потомок
        "Идентификатор процесса: " << getpid() << "\n" << 
        "Идентификатор предка: "   << getppid() << "\n" <<
        "Идентификатор сессии процесса: " << getsid(getpid()) << "\n" <<
        "Идентификатор группы процессов: " << getpgid(getpid()) << "\n" <<
        "Реальный идентификатор пользователя: " << getuid() << "\n" <<
        "Эффективный идентификатор пользователя: " << getgid() << "\n" <<
        "Эффективный групповой идентификатор: "   << getegid() << "\n";
        file.close();
    }
}