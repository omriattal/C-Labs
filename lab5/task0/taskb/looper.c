#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void signal_handler(int sigNum) {
	printf("%s signal was received \n",strsignal(sigNum));
	signal(sigNum,SIG_DFL);
	if(sigNum == SIGTSTP) {
		signal(SIGCONT,signal_handler);
	} else if(sigNum == SIGCONT) {
		signal(SIGTSTP, signal_handler);
	} 
	raise(sigNum);
}

int main(int argc, char **argv){ 
	printf("Starting the program\n");
	signal(SIGCONT,signal_handler);
	signal(SIGTSTP,signal_handler);
	signal(SIGINT,signal_handler);

	while(1) {
		sleep(2);
	}

	return 0;
}