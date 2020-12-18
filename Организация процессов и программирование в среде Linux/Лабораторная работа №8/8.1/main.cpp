/*Задание: Напишите две программы, которые обмениваются сообщениями. Первая программа создает очередь и 
ожидает сообщение от второй программы определенное время, которое задается при запуске первой программы и выводится на экран. 
Если за это время сообщение от второй программы не поступило, то первая программа завершает свою работу и уничтожает очередь.
 Вторая программа может запускаться несколько раз и только при условии, что первая программа работает, в противном случае она заканчивает свою работу.
 При запуске второй программы указывается новое  время ожидания для первой программы.*/

#include <iostream>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <cstring>
#include <iomanip>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

using namespace std;
const int KEY = 646;

struct myMsgbuf {
    long mtype;   /* тип сообщения, должен быть > 0 */
    int mtext;        /* содержание сообщения */
};

int main(int argc, char *argv[]) {

    assert(argc == 2);

	int waitTime = atoi(argv[1]);
	myMsgbuf message;

    //разрешен прием сообщений пользователю, владеющему очередью и
    //разрешена передача сообщений всем остальным пользователям
	int msgId = msgget(KEY, 402|IPC_CREAT);
	if(msgId == -1){
		cout<<"Ошибка создания очереди!\n" << endl;
		return 0;
	}
    for(int timer = waitTime; timer > 0; --timer)
    {
        if( msgrcv(msgId, &message, sizeof(myMsgbuf), 1, IPC_NOWAIT) != -1){
	    	cout<< "Сообщение: " << message.mtext << endl;
            timer = message.mtext;
        }
        sleep(1);
    }

    msgctl(msgId, IPC_RMID, 0);

	return 0;
}