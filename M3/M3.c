#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <process.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <errno.h>

const int BUFFER_LENGTH = 20;

int main(int argc, char *argv[]) {
	pid_t pid_3 = getpid();
	pid_t pid_2 = getppid();
	char *msg = (char*)malloc(BUFFER_LENGTH*sizeof(char));
	char *rmsg = (char*)malloc(BUFFER_LENGTH*sizeof(char));
	printf("  P3> P3\n");
	printf("  P3> Parent pid: %i\n", pid_2);
	int chidP2 = atoi(argv[0]);

	// Step 5 Creating channel - chidP3.
	int chidP3 = ChannelCreate(0);
	printf("  P3> pid: %i, P2 chid: %i, P3 chid: %i\n", pid_3, chidP2, chidP3);

	// Step 8 Sending "query" to P2.
	int coid_2 = ConnectAttach(ND_LOCAL_NODE, pid_2, chidP2, _NTO_SIDE_CHANNEL, 0);
	if (coid_2==-1) {
		fprintf(stderr, "Connection error.\n");
		exit(EXIT_FAILURE);
	}
	printf("  P3> P3 coid: %i\n", coid_2);

	sprintf(msg, "pid_1, chidP1?");
	int msg_res = MsgSend(coid_2, msg, strlen(msg) + 1, rmsg, BUFFER_LENGTH*sizeof(char));
	if (msg_res < 0) {
		msg_res = errno;
		fprintf(stderr, "  P3> Error MsgSend\n");
		fprintf(stderr, "  P3> Error code:%i\n", msg_res);
		exit(EXIT_FAILURE);
	}
	printf("  P3> Server replied \n");

	// Step 9 Receiving from P2 pid_1 and chidP1.
	int pid_1 = *(int*)((int)rmsg);
	int chidP1 = *(int*)((int)rmsg + 4);
	printf("  P3> pid: %i chidP1: %i\n", pid_1, chidP1);

	// Step 10 Sending pid_3 and chidP3 to P1. Packing int 8 byte buffer. |0__pid_3__|4__chidP3__|8
	int coid_1 = ConnectAttach(ND_LOCAL_NODE, pid_1, chidP1, _NTO_SIDE_CHANNEL, 0);
	if (coid_1==-1) {
		fprintf(stderr, "Connection error.\n");
		exit(EXIT_FAILURE);
	}
	printf("  P3> P3 coid: %i\n", coid_1);
	*(int*)((int)msg) = pid_3;
	*(int*)((int)msg + 4) = chidP3;
	msg_res = MsgSend(coid_1, msg, 20, rmsg, BUFFER_LENGTH*sizeof(char));
	if (msg_res < 0) {
		msg_res = errno;
		fprintf(stderr, "  P3> Error MsgSend\n");
		fprintf(stderr, "  P3> Error code:%i\n", msg_res);
		exit(EXIT_FAILURE);
	}
	printf("P3> Server replied \n");

	// Step 17 Receiving message from P1. Reply.
	int rcvid = MsgReceive(chidP3, msg, sizeof(msg), NULL);
	printf("  P3> Received, rcvid: %i\n", rcvid);
	printf("  P3> Message: %s\n", msg);
	strcpy(msg, "P3 OK");
	MsgReply(rcvid, EOK, msg, strlen(msg) + 1);

	printf("  P3> Have a nice day!\n");
	return EXIT_SUCCESS;
}
