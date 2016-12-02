// ipc1 : �׸��� ����
// ipc.c : ���μ����� ���
#include "header.h"

#define KEY				(key_t)600201
#define PERMISSION		IPC_CREAT | 0660
#define MSGTYPE_PATH		1
#define MSGTYPE_ANSWER		2
#define MSGTYPE_GAMEOVER	3
static int qid;

// �ְ� ���� ������: �׸� ��
struct path {
	long	data_type;
	int		index;
	int		x;
	int		y;
	int		color;
	int		width;
};

// ����
static void fatal(char *err) {
	perror(err);
	exit(0);
}

// includes msgget()
extern void IpcInit() {
	if (-1 == (qid = msgget(KEY, PERMISSION))) {
		fatal("failed to init queue");
	}
}

// includes msgctl() msgget()
extern void IpcInitClear() {
	msgctl(qid, IPC_RMID, 0);
	if (-1 == (qid = msgget(KEY, PERMISSION))) {
		fatal("failed to init queue");
	}
}

// �׸� ���� �� ������ �޽��� ����
extern void SndPath(int _index, int _x, int _y, int _color, int _width) {
	struct path buf;
	// copy path
	printf("sndpath\n");
	buf.data_type = MSGTYPE_PATH;
	buf.index = _index;
	buf.x = _x;
	buf.y = _y;
	buf.color = _color;
	buf.width = _width;
	if (-1 == msgsnd(qid, &buf, sizeof(struct path) - sizeof(long), 0)) {
		fatal("SndPath msgsnd() failed");
	}
	printf("%d \n", _index);
}

// ���� �׸� ������(����ü �޽���; �� �ϳ�)�� Ǯ� ����
static void RcvPath(struct path rcvpath) {
	if (indexPath >= MAX_INDEX_PATH) { return; } // ������ ������ ������ �߰��� �� ����
	// ������ġ ����
	path[rcvpath.index].x = rcvpath.x;
	path[rcvpath.index].y = rcvpath.y;
	// ������ġ�� �Բ� �ٸ� (�߰�) ���� ����
	pathColor[rcvpath.index] = rcvpath.color;
	pathWidth[rcvpath.index] = rcvpath.width;
	// ��������
	if (indexPath <= rcvpath.index) {
		indexPath = rcvpath.index + 1;
	}
}

// ������ ������ ��(reader)�� �޽����� ��� ��
// ("Loop"�� ����(�޽����� �߻�)�� �ƴ� ���(�а�) �ϱ� ���� ��... Ÿ�̸� �̺�Ʈ�� �ƴ� �̻�)
// (�޽����� ������ �޽����� �߻��� �̺�Ʈ���� �����ϴ� ���� �Ǵ�)
static void IpcLoopReader() {
	struct path buf;
	
	// �׸� �����͸� �д´�
	for (;;) {
		if (-1 == msgrcv(qid, &buf, sizeof(struct path) - sizeof(long), MSGTYPE_PATH, 0)) {
			fatal("failed to msgrcv()");
		}
		RcvPath(buf);
		RepaintPath();
	}
} // func

// �� ��ƾ�� writer(�׸��� ��)���� ȣ���Ѵ�
extern void *Thread2Writer() {
//	IpcLoopWriter();
	pthread_exit(NULL);
}

// �� ��ƾ�� reader(�д� ��)���� ȣ���Ѵ�
extern void *Thread2Reader() {
	IpcLoopReader();
	pthread_exit(NULL);
}


// ���α׷��� writer(�׸��� ��)�� ���� ������ �� reader(�д� ��)�� �����ؾ� �Ѵ�

