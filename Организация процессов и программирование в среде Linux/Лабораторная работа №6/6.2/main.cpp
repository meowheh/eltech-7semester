/*Задание: Написать периодическую программу, в которой период запуска и
количество запусков должны задаваться в качестве ее параметров. При
каждом очередном запуске программа должна порождать новый процесс,
который выводит на экран свой идентификатор, дату и время старта.
Программа и ее дочерний процесс должны быть заблокированы от
завершения при нажатии клавиши Ctrl/z. После завершения дочернего
процесса программа должна вывести на экран информацию о времени своей
работы и дочернего процесса. */

#include <sys/time.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

typedef struct init_param {
    int run_counts;         /* Кол-во запусков */
    long tv_sec;            /* Период повторения в сек */
} inparam;

void init_param(inparam* param) {
    printf("Параметры программы\nКоличество запусков: ");
    scanf("%d", &param->run_counts);
    printf("Период повторения (с): ");
    scanf("%ld", &param->tv_sec);
}

int g_current_counts = 0;

void signal_handler(int signum) {
    if (signum == SIGALRM) {
        if (fork() == 0) {
            time_t t = time(NULL);
            tm* start_time = localtime(&t);
            printf("\nПроцесс %d\nPID: %d\nДата и время старта: %s\n", g_current_counts, getpid(), asctime(start_time));
            exit(EXIT_SUCCESS);
        }
        ++g_current_counts;
    }
}


int main(int argc, char* argv[]) {
    
    struct sigaction act;
	act.sa_handler = signal_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &act, NULL);
    sigaction(SIGTSTP, &act, NULL);


    time_t start, end;
    inparam param;
    if(argc < 3)
        init_param(&param);
    else
    {
        param.run_counts = atoi(argv[1]);
        param.tv_sec = atoi(argv[2]);
    }
    time(&start);
    struct itimerval value, ovalue;
    value.it_value.tv_sec = param.tv_sec;
    value.it_value.tv_usec = 0;
    value.it_interval.tv_sec = param.tv_sec;
    value.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &value, &ovalue);
    for (;;){
		pause();
        wait(NULL);
        if (g_current_counts == param.run_counts) {
            break;
        }
	}
    
    time(&end);
    printf("Время работы программы: %.2f сек\n", difftime(end, start));

    return EXIT_SUCCESS;
}
