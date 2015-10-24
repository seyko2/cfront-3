/*ident	"@(#)ipc:demos/testipc.c	3.1" */
/******************************************************************************
*
* C++ Standard Components, Release 3.0.
*
* Copyright (c) 1991, 1992 AT&T and Unix System Laboratories, Inc.
* Copyright (c) 1988, 1989, 1990 AT&T.  All Rights Reserved.
*
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T and Unix System
* Laboratories, Inc.  The copyright notice above does not evidence
* any actual or intended publication of such source code.
*
******************************************************************************/

#include <fstream.h>
#include "ipcstream.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <new.h>
#include <stdlib.h>
#include "assert.h"

const int	CLIENTS=5;	// no more than 5 or too many files
int	client(int);
char*	testfile = "testfile";
const char*	testipc = "testatt";

void
burp()
{
	assert(0);
}

main()
{
	set_new_handler(burp);

	int	xfd = open(testfile, O_WRONLY|O_CREAT|O_EXCL, 0666);
	assert(xfd >= 0);
	key_t	key = ftok(testfile, 'a');
	assert(key != key_t(-1));
	int	sem_id = semget(key, 1, IPC_CREAT|IPC_EXCL|0666);
	assert(sem_id >= 0);
	semun	arg;  arg.val = 1;
	if (semctl(sem_id, 0, SETVAL, arg) != 0) {
		assert(0);
	}
	// semaphore created.  Fork off clients
	for (int i = CLIENTS; i--; ) {
		int	child = fork();
		assert(child != -1);
		if (child == 0)
			exit(client(i));
	}
	// create ipc_attachment
	ipc_attachment	att(testipc);
	assert(att);
	// set semaphore so clients can proceed
	sembuf	sbuf;
	sbuf.sem_num = 0;
	sbuf.sem_op = -1;
	sbuf.sem_flg = 0;
	i = semop(sem_id, &sbuf, 1);
	assert(i != -1);
	ipcstream**	ipc_p = new ipcstream*[CLIENTS];
	int	client_arr[CLIENTS];
	// listen for clients connections
	for (i = CLIENTS; i--; ) {
		att.listen();
		ipc_p[i] = new ipcstream(att);	// accept client's connection
		assert(ipc_p[i] && *ipc_p[i]);

		// cerr << "Caller: " << att.username() << " uid: " << att.uid() << " gid: " << att.gid() << "\n";

		*ipc_p[i] << "Hello\n";
		client_arr[i] = 0;
	}
	// exchange a message with each client
	for (i = 0; i < CLIENTS; i++) {
		int	client_no;
		*ipc_p[i] >> client_no;
		assert(client_no >= 0 && client_no < CLIENTS);
		// cerr << "Server: receive client " << client_no << endl;
		client_arr[client_no] = 1;
	}
	// be sure all clients have participated and clean up
	for (i = CLIENTS; i--; ) {
		assert(client_arr[i]);
		delete ipc_p[i];
		int	status;
#if defined(BSD) && !defined(__alpha)
                int     proc_no = wait((union wait*)&status);
#else
                int     proc_no = wait(&status);
#endif
		assert(proc_no != -1);
		assert(status == 0);
	}
	semctl(sem_id, 0, IPC_RMID, arg);	// remove semaphore
	close(xfd);
	unlink(testfile);
	return 0;
}

int
client(int n)
{
	// wait for semaphore
	key_t	key = ftok(testfile, 'a');
	assert(key != key_t(-1));
	int	sem_id = semget(key, 1, 0);
	assert(sem_id >= 0);
	sembuf	sbuf;
	sbuf.sem_num = 0;
	sbuf.sem_op = 0;
	sbuf.sem_flg = 0;
	int	sem_result = semop(sem_id, &sbuf, 1);
	assert(sem_result != -1);
	// semaphore okay
	ipcstream	ipc(testipc);	// connect to server
	String	hello;
	assert(ipc);
	// exchange messages with server
	if (n % 2) {
		ipc >> hello;
		ipc << n << "\n";
	} else {
		ipc << n << "\n";
		ipc >> hello;
	}
	// cerr << "Client" << n << ": receive " << hello << endl;
	assert(ipc);
	return (hello != String("Hello"));
}
