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
	printf("P3> P3\n");
	printf("P3> Parent pid: %i\n", pid_2);
	int chidP2 = atoi(argv[0]);
	int chidP3 = ChannelCreate(0);
	printf("P3> pid: %i, P2 chid: %i, P3 chid: %i\n", pid_3, chidP2, chidP3);

	int coid_2 = ConnectAttach(ND_LOCAL_NODE, pid_2, chidP2, _NTO_SIDE_CHANNEL, 0);
	if (coid_2==-1) {
		fprintf(stderr, "Connection error.\n");
		exit(EXIT_FAILURE);
	}
	printf("P3> P3 coid: %i\n", coid_2);

	char *msg = (char*)malloc(BUFFER_LENGTH*sizeof(char));
	char *rmsg = (char*)malloc(BUFFER_LENGTH*sizeof(char));
	sprintf(msg, "pid_1, chidP1?");
	int msg_res = MsgSend(coid_2, msg, strlen(msg) + 1, rmsg, BUFFER_LENGTH*sizeof(char));
	if (msg_res < 0) {
		msg_res = errno;
		fprintf(stderr, "P2> Error MsgSend\n");
		fprintf(stderr, "P2> Error code:%i\n", msg_res);
		exit(EXIT_FAILURE);
	}
	printf("P3> Server replied \n");

	int pid_1 = *(int*)((int)rmsg);
	int chidP1 = *(int*)((int)rmsg + 4);
	printf("P3> pid: %i chidP1: %i\n", pid_1, chidP1);

	int coid_1 = ConnectAttach(ND_LOCAL_NODE, pid_1, chidP1, _NTO_SIDE_CHANNEL, 0);
		if (coid_1==-1) {
			fprintf(stderr, "Connection error.\n");
			exit(EXIT_FAILURE);
		}
	printf("P3> P3 coid: %i\n", coid_1);
	*(int*)((int)msg) = pid_3;
	*(int*)((int)msg + 4) = chidP3;
	msg_res = MsgSend(coid_1, msg, 20, rmsg, BUFFER_LENGTH*sizeof(char));
		if (msg_res < 0) {
			msg_res = errno;
			fprintf(stderr, "P2> Error MsgSend\n");
			fprintf(stderr, "P2> Error code:%i\n", msg_res);
			exit(EXIT_FAILURE);
		}
	printf("P3> Server replied \n");

	// Step 19
	int rcvid = MsgReceive(chidP3, msg, sizeof(msg), NULL);
	printf("P3> Received, rcvid: %i\n", rcvid);
	printf("P3> Message: %s\n", msg);
	strcpy(msg, "P3 OK");
	MsgReply(rcvid, EOK, msg, strlen(msg) + 1);

	printf("P3> Have a nice day!\n");
	return EXIT_SUCCESS;
}
