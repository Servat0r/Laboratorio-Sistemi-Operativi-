/**
 * @brief Feature test macros (needed for correct use of signals with the option
 * '-std=c99')
 */
#define _POSIX_C_SOURCE 200809L

#include <defines.h>
#include <sys/types.h>
#include <sys/un.h>
#include <signal.h>
#include <pthread.h>

#define BSIZE 24

typedef volatile sig_atomic_t sigsafe_t;

/* How many Ctrl-C has been done */
sigsafe_t int_count = 0;

/* How many Ctrl-Z has been done */
sigsafe_t tstp_count = 0;

/* user_cont == 1 <=> user wants to continue after >=3 'Ctrl-Z' */
sigsafe_t user_cont = 0;


int convert(char* buf, size_t bufsize, sigsafe_t n){
	char tmpbuf[bufsize];
	if (n < 0) {
		tmpbuf[0] = '-';
		return 1 + convert(buf+1, bufsize-1, -n);
	} else if (n == 0) {
		buf[0] = '0';
		buf[1] = '\0';
		return 2; /* "0\0" */
	} else {
		int ok = 0;
		int i = 0;
		while (i < bufsize){
			if (n <= 0){ tmpbuf[i++] = '\0'; ok = 1; break; } /* #chars written to buf */
			long int m = n % 10; /* 0 <= m <= 9 */
			n = (n - m) / 10; 
			tmpbuf[i++] = m + '0';
		}
		if (!ok) return -1; /* buffer overflow */
		for (int j = 0; j < i-1; j++){
			buf[j] = tmpbuf[i-2-j];
		}
		buf[i-1] = tmpbuf[i-1];
		return i;
	}
}

void handle_alarm(int sig){
	if (sig == SIGALRM){
		write(1, "\nAlarm caught\n", 15);
		if (user_cont == 1) user_cont = 0;
		else _exit(EXIT_SUCCESS);
	}
}


void handle_sigs(int sig){
	if (sig == SIGINT){
		int_count++;
		write(1, "\n", 2);
	} else if (sig == SIGTSTP) {
		char buf[BSIZE];
		tstp_count++;
		int i = convert(buf, BSIZE, int_count);
		if (i == -1) _exit(EXIT_FAILURE); /* buffer overflow */
		write(1, buf, i);//, "write1");
		write(1, "\n", 2);//, "write2");
		if (tstp_count >= 3){
			user_cont = 0;
			for (int j = 0; j < BSIZE; j++) buf[j] = 0;
			SYSCALL_EXIT(write(1, "Do you want to exit (y\\n)? ", 28), "write3");		
			alarm(6);
			i = read(0, buf, BSIZE);
			if (i == -1) perror("read");
			/* FIXME Could be some race conditions on user_cont ? */
			else if (buf[0] == 'n') user_cont = 1;
			else if (buf[0] == 'y') { user_cont = 0; _exit(EXIT_SUCCESS); }
		}
	}
}

void init_handler(struct sigaction* sa, void (*handler)(int)){
	memset(sa, 0, sizeof(*sa));
	sa->sa_handler = handler;
}



int main(void){
	sigset_t mset; 
	sigset_t oldmset; 
	struct sigaction sa; 
	struct sigaction sa2; 
	struct sigaction sa_ign; 
	SYSCALL_EXIT(sigfillset(&mset), "sigfillset"); /* Masks all signals */ 
	SYSCALL_EXIT(pthread_sigmask(SIG_SETMASK, &mset, &oldmset), "sigmask (1)");
	init_handler(&sa, &handle_sigs); 
	init_handler(&sa2, &handle_alarm);
	sigaddset(&sa.sa_mask, SIGINT);
	sigaddset(&sa.sa_mask, SIGTSTP); 
	memset(&sa_ign, 0, sizeof(sa_ign)); 
	sa_ign.sa_handler = SIG_IGN; 
	sa.sa_flags = sa.sa_flags & SA_RESTART; 
	SYSCALL_EXIT(sigaction(SIGINT, &sa, NULL), "sigaction[SIGINT]");
	SYSCALL_EXIT(sigaction(SIGTSTP, &sa, NULL), "sigaction[SIGTSTP]");
	SYSCALL_EXIT(sigaction(SIGALRM, &sa2, NULL), "sigaction[SIGALRM]");
	SYSCALL_EXIT(sigaction(SIGPIPE, &sa_ign, NULL), "sigaction[SIGPIPE]");
	SYSCALL_EXIT(sigaction(SIGTERM, &sa_ign, NULL), "sigaction[SIGTERM]");
	SYSCALL_EXIT(pthread_sigmask(SIG_SETMASK, &oldmset, NULL), "sigmask (2)");

#if 1
	while (1) ;
#endif

#if 0
	while (1){ pause(); }
#endif

#if 0
	/* This piece of cose uses sigwait */
	int sig;
	sigset_t sset;
	sigemptyset(&sset);
	sigaddset(&sset, SIGINT);
	sigaddset(&sset, SIGTSTP);
	pthread_sigmask(SIG_SETMASK, &sset, NULL);
	while (1){
		sigwait(&sset, &sig);
		if ((sig == SIGINT) || (sig == SIGTSTP)) handle_sigs(sig);
		else if (sig == SIGALRM) handle_alarm(sig);
	}
#endif
	return 0;
}
