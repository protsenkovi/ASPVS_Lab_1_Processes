#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <process.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>

const int BUFFER_LENGTH = 20;

int main(int argc, char *argv[]) {
	pid_t pid_1 = getpid();
	printf("P1>\n");
	int chidP1 = ChannelCreate(0);
	printf("P1> pid: %i, chid: %i\n", pid_1, chidP1);
	char argv0[12];
	sprintf(argv0, "%i", chidP1);
	int pid_2 = spawnl(P_NOWAITO, "/tmp/M2", argv0, NULL);	// Spawned P2
	printf("P1> Spawned new process. res: %i\n", pid_2);

	int coid = ConnectAttach(0, pid_1, chidP1, _NTO_SIDE_CHANNEL, 0);
	if (coid==-1) {
		fprintf(stderr, "P1> Ошибка соединения\n");
		exit(EXIT_FAILURE);
	}
	printf("P1> coid: %x\n", coid);
	char *msg = (char*)malloc(BUFFER_LENGTH*sizeof(char));
	char *rmsg = (char*)malloc(BUFFER_LENGTH*sizeof(char));

	int rcvid = MsgReceive(chidP1, msg, sizeof(msg), NULL);
	printf("P1> Received, rcvid: %i\n", rcvid);
	printf("P1> Message: %s\n", msg);
	int chidP2 = atoi(msg);
	strcpy(msg, "P1 received chidP2");
	MsgReply(rcvid, EOK, msg, sizeof(msg));
	printf("P1> Message: %s Size of message: %i\n", msg, sizeof(msg));
	printf("P1> chidP2: %i\n", chidP2);

	rcvid = MsgReceive(chidP1, msg, 20, NULL);
	int pid_3 = *(int*)((int)msg);
	int chidP3 = *(int*)((int)msg + 4);

	strcpy(rmsg, "P1 received pid_3, chidP3");
	MsgReply(rcvid, EOK, rmsg, sizeof(rmsg));

	printf("P1> pid_3: %i chidP3: %i\n", pid_3, chidP3);

	//12 step Sending to P2 message "P1 send message to P2"
	int coid_2 = ConnectAttach(ND_LOCAL_NODE, pid_2, chidP2, _NTO_SIDE_CHANNEL, 0);
	if (coid_2==-1) {
		fprintf(stderr, "Connection error.\n");
		exit(EXIT_FAILURE);
	}
	sprintf(msg, "P1 send message to P2");
	int msg_res = MsgSend(coid_2, msg, strlen(msg) + 1, rmsg, BUFFER_LENGTH*sizeof(char)); //sizeof(bytes)
	if (msg_res < 0) {
		msg_res = errno;
		fprintf(stderr, "P1> Error MsgSend\n");
		fprintf(stderr, "P1> Error code:%i\n", msg_res);
		exit(EXIT_FAILURE);
	}
	printf("P1> Server replied: %s\n", rmsg);

	//17 step Sending to P3 message "P1 send message to P3"
	int coid_3 = ConnectAttach(ND_LOCAL_NODE, pid_3, chidP3, _NTO_SIDE_CHANNEL, 0);
	if (coid_3==-1) {
		fprintf(stderr, "Connection error.\n");
		exit(EXIT_FAILURE);
	}
	sprintf(msg, "P1 send message to P3");
	 msg_res = MsgSend(coid_3, msg, strlen(msg) + 1, rmsg, BUFFER_LENGTH*sizeof(char)); //sizeof(bytes)
	if (msg_res < 0) {
		msg_res = errno;
		fprintf(stderr, "P1> Error MsgSend\n");
		fprintf(stderr, "P1> Error code:%i\n", msg_res);
		exit(EXIT_FAILURE);
	}
	printf("P1> Server replied: %s\n", rmsg);

	printf("P1> Good by!\n");
	return EXIT_SUCCESS;
}
