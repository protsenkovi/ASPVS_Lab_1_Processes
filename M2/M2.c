#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <process.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <errno.h>

const int BUFFER_LENGTH = 20;

int main(int argc, char *argv[]) {
	pid_t pid_2 = getpid();
	pid_t pid_1 = getppid();
	printf("P2>\n");
	printf("P2> Parent pid: %i\n", pid_1);
	printf("P2> %s\n", argv[0]);
	int chidP1 = atoi(argv[0]);
	int chidP2 = ChannelCreate(0);
	printf("P2> pid: %i, P1 chid: %i, P2 chid: %i\n", pid_2, chidP1, chidP2);
	char argv0[12];
	sprintf(argv0, "%i", chidP2);
	int exit_val2 = spawnl(P_NOWAITO, "/tmp/M3", argv0, NULL);

	int coid = ConnectAttach(ND_LOCAL_NODE, pid_1, chidP1, _NTO_SIDE_CHANNEL, 0);
		if (coid==-1) {
			fprintf(stderr, "Connection error.\n");
			exit(EXIT_FAILURE);
		}
	printf("P2> P2 coid: %i\n", coid);

	char *msg = (char*)malloc(BUFFER_LENGTH*sizeof(char));
	char *rmsg = (char*)malloc(BUFFER_LENGTH*sizeof(char));
	sprintf(msg, "%i", chidP2);
	int msg_res = MsgSend(coid, msg, strlen(msg) + 1, rmsg, BUFFER_LENGTH*sizeof(char)); //sizeof(bytes)
	if (msg_res < 0) {
		msg_res = errno;
		fprintf(stderr, "P2> Error MsgSend\n");
		fprintf(stderr, "P2> Error code:%i\n", msg_res);
		exit(EXIT_FAILURE);
	}
	printf("P2> Server replied \n%s\n", rmsg);

	// Step 9
	int rcvid = MsgReceive(chidP2, msg, sizeof(msg), NULL);
	printf("P2> Received, rcvid: %i\n", rcvid);
	printf("P2> Message: %s\n", msg);
	*(int*)((int)msg) = pid_1;
	*(int*)((int)msg + 4) = chidP1;
	MsgReply(rcvid, EOK, msg, 8);

	// Step 14
	rcvid = MsgReceive(chidP2, msg, sizeof(msg), NULL);
	printf("P2> Received, rcvid: %i\n", rcvid);
	printf("P2> Message: %s\n", msg);
	strcpy(msg, "P2 OK");
	MsgReply(rcvid, EOK, msg, strlen(msg) + 1);

	printf("P2> Good luck!\n");
	return EXIT_SUCCESS;
}
