#ifndef PANTRYC_SIGNAL_H_
#define PANTRYC_SIGNAL_H_

#include <signal.h>

#define	PANTRYC_SIGNAL__SIGHUP		1<<SIGHUP		/* Hangup (POSIX).  */
#define	PANTRYC_SIGNAL__SIGINT		1<<SIGINT		/* Interrupt (ANSI).  */
#define	PANTRYC_SIGNAL__SIGQUIT		1<<SIGQUIT		/* Quit (POSIX).  */
#define	PANTRYC_SIGNAL__SIGILL		1<<SIGILL		/* Illegal instruction (ANSI).  */
#define	PANTRYC_SIGNAL__SIGTRAP		1<<SIGTRAP		/* Trace trap (POSIX).  */
#define	PANTRYC_SIGNAL__SIGABRT		1<<SIGABRT		/* Abort (ANSI).  */
#define	PANTRYC_SIGNAL__SIGIOT		1<<SIGIOT		/* IOT trap (4.2 BSD).  */
#define	PANTRYC_SIGNAL__SIGBUS		1<<SIGBUS		/* BUS error (4.2 BSD).  */
#define	PANTRYC_SIGNAL__SIGFPE		1<<SIGFPE		/* Floating-point exception (ANSI).  */
#define	PANTRYC_SIGNAL__SIGKILL		1<<SIGKILL		/* Kill, unblockable (POSIX).  */
#define	PANTRYC_SIGNAL__SIGUSR1		1<<SIGUSR1		/* User-defined signal 1 (POSIX).  */
#define	PANTRYC_SIGNAL__SIGSEGV		1<<SIGSEGV		/* Segmentation violation (ANSI).  */
#define	PANTRYC_SIGNAL__SIGUSR2		1<<SIGUSR2		/* User-defined signal 2 (POSIX).  */
#define	PANTRYC_SIGNAL__SIGPIPE		1<<SIGPIPE		/* Broken pipe (POSIX).  */
#define	PANTRYC_SIGNAL__SIGALRM		1<<SIGALRM		/* Alarm clock (POSIX).  */
#define	PANTRYC_SIGNAL__SIGTERM		1<<SIGTERM		/* Termination (ANSI).  */
#define	PANTRYC_SIGNAL__SIGSTKFLT	1<<SIGSTKFLT	/* Stack fault.  */
#define	PANTRYC_SIGNAL__SIGCLD		1<<SIGCLD		/* Same as SIGCHLD (System V).  */
#define	PANTRYC_SIGNAL__SIGCHLD		1<<SIGCHLD		/* Child status has changed (POSIX).  */
#define	PANTRYC_SIGNAL__SIGCONT		1<<SIGCONT		/* Continue (POSIX).  */
#define	PANTRYC_SIGNAL__SIGSTOP		1<<SIGSTOP		/* Stop, unblockable (POSIX).  */
#define	PANTRYC_SIGNAL__SIGTSTP		1<<SIGTSTP		/* Keyboard stop (POSIX).  */
#define	PANTRYC_SIGNAL__SIGTTIN		1<<SIGTTIN		/* Background read from tty (POSIX).  */
#define	PANTRYC_SIGNAL__SIGTTOU		1<<SIGTTOU		/* Background write to tty (POSIX).  */
#define	PANTRYC_SIGNAL__SIGURG		1<<SIGURG		/* Urgent condition on socket (4.2 BSD).  */
#define	PANTRYC_SIGNAL__SIGXCPU		1<<SIGXCPU		/* CPU limit exceeded (4.2 BSD).  */
#define	PANTRYC_SIGNAL__SIGXFSZ		1<<SIGXFSZ		/* File size limit exceeded (4.2 BSD).  */
#define	PANTRYC_SIGNAL__SIGVTALRM	1<<SIGVTALRM	/* Virtual alarm clock (4.2 BSD).  */
#define	PANTRYC_SIGNAL__SIGPROF		1<<SIGPROF		/* Profiling alarm clock (4.2 BSD).  */
#define	PANTRYC_SIGNAL__SIGWINCH	1<<SIGWINCH	/* Window size change (4.3 BSD, Sun).  */
#define	PANTRYC_SIGNAL__SIGPOLL		1<<SIGPOLL		/* Pollable event occurred (System V).  */
#define	PANTRYC_SIGNAL__SIGIO		1<<SIGIO		/* I/O now possible (4.2 BSD).  */
#define	PANTRYC_SIGNAL__SIGPWR		1<<SIGPWR		/* Power failure restart (System V).  */
#define	PANTRYC_SIGNAL__SIGSYS		1<<SIGSYS		/* Bad system call.  */
#define	PANTRYC_SIGNAL__SIGUNUSED	1<<SIGUNUSED

/**
 * Register signal handlers
 * @signals			signals need to register (ex: PANTRYC_SIGNAL__SIGTERM | PANTRYC_SIGNAL__SIGSEGV...)
 */
// TODO change return type to bool
int pantryc_signal__register(int signals);

#endif /* PANTRYC_SIGNAL_H_ */
