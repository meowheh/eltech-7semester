#include <iostream>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

using namespace std;

const int KEY = 646;

struct myMsgbuf {
    long  mtype;   /* тип сообщения, должен быть > 0 */
    int  mtext;        /* содержание сообщения */

};
int main(int argc, char *argv[]) {

    assert(argc == 2);

	int waitTime = atoi(argv[1]);

	myMsgbuf message;

    int msgId;
	if((msgId = msgget(KEY, IPC_CREAT|IPC_EXCL))!= -1){
		cout<< "Первая программа не запущена!\n"<<endl;
		msgctl(msgId, IPC_RMID, 0);
		return 0;
	}

	msgId = msgget(KEY, IPC_CREAT);

    cout << "Время ожидания: " << waitTime << endl;

    message.mtype = 1;
    message.mtext = waitTime;

    if( (msgsnd(msgId, &message, sizeof(myMsgbuf), 0)) != -1) {
    	cout<< "Сообщение '"<<  message.mtext<< "' отправлено" << endl;
    } else{
    	cout<< "Сообщение не отправлено" << endl;
    	perror("msgsnd");
    }

    return 0;
}