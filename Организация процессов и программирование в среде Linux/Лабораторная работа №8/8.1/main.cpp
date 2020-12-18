/*Задание: Напишите две программы, которые обмениваются сообщениями. Первая программа создает очередь и 
ожидает сообщение от второй программы определенное время, которое задается при запуске первой программы и выводится на экран. 
Если за это время сообщение от второй программы не поступило, то первая программа завершает свою работу и уничтожает очередь.
 Вторая программа может запускаться несколько раз и только при условии, что первая программа работает, в противном случае она заканчивает свою работу.
 При запуске второй программы указывается новое  время ожидания для первой программы.*/

#include <iostream>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/time.h>
#include <cstring>
#include <iomanip>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>

using namespace std;
const int KEY = 646;
int msgId;
struct myMsgbuf {
    long mtype;   /* тип сообщения, должен быть > 0 */
    int mtext;        /* содержание сообщения */
};

void signal_handler(int signum) {
    if (signum == SIGALRM) {
        msgctl(msgId, IPC_RMID, 0);
        exit(0);
    }
}

int main(int argc, char *argv[]) {

    assert(argc == 2);
    signal(SIGALRM, signal_handler);
	int waitTime = atoi(argv[1]);
	myMsgbuf message;

    //разрешен прием сообщений пользователю, владеющему очередью и
    //разрешена передача сообщений всем остальным пользователям
	msgId = msgget(KEY, 402|IPC_CREAT);
	if(msgId == -1){
		cout<<"Ошибка создания очереди!\n" << endl;
		return 0;
	}
    struct itimerval value, ovalue;
	
	// период перезапуска по умолчанию
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = 0;
	// Первый запуск программы через waitTime сек
	value.it_value.tv_sec = waitTime;
	value.it_value.tv_usec = 0;
	
	// Устанавливаем интервальный таймер
	setitimer(ITIMER_REAL, &value, &ovalue);
    for(;;)
    {
        if( msgrcv(msgId, &message, sizeof(myMsgbuf), 0, 0) != -1){
	    	cout<< "Сообщение: " << message.mtext << endl;
            value.it_value.tv_sec = message.mtext;
            setitimer(ITIMER_REAL, &value, &ovalue);
        }
    }
}