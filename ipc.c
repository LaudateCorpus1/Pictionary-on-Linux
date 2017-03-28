// ipc.c : ���μ����� ���
#include "header.h"

#define MSGTYPE_PATH		1
#define MSGTYPE_ANSWER		2
#define MSGTYPE_GAMEOVER	3

#define PORT				9000
static pthread_t t_id; // accept() ���� �ڿ� �����Ǵ� ������ ���ῡ ���� ������

// extern
// sock :
int sock; // Ŭ���̾�Ʈ ���μ����� ���
int serv_sock; // ���� ���μ����� ���

// sock (server) :
#define MAX_CLNT 256
static int clnt_cnt = 0;
static int clnt_socks[MAX_CLNT];
static pthread_mutex_t mutx;

// �ְ� ���� ������: �׸� ��
struct path {
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

// Ŭ���̾�Ʈ ���μ���
extern void ClientInitSock(char* ip) {
	struct sockaddr_in serv_addr;

	sock = socket(PF_INET, SOCK_STREAM, 0); // ���� ����

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ip);
	serv_addr.sin_port = htons(PORT);

	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) // ���� ����
		fatal("connect() error");
}

// Ŭ���̾�Ʈ ���μ������� ����
// �׸� ���� �� ������ �޽��� ����
extern void SndPath(int _index, int _x, int _y, int _color, int _width) {
	struct path buf;
	// copy path
	buf.index = _index;
	buf.x = _x;
	buf.y = _y;
	buf.color = _color;
	buf.width = _width;
	// send it to socket
	write(sock, &buf, sizeof(buf));
}

////////////////////////////////// ���� ���� ���μ��� ////////////////////////////////// 
// �ڵ�� �Ųٷ� ���� ��

// ���� ���μ������� ����
// ���� �׸� ������(����ü �޽���; �� �ϳ�)�� Ǯ� ����
static void RcvPath(struct path rcvpath) {
	if (indexPath >= MAX_INDEX_PATH) { return; } // ������ ������ ������ �߰��� �� ����
	// ������ġ ����
	path[rcvpath.index].x = rcvpath.x;
	path[rcvpath.index].y = rcvpath.y;
	// ������ġ�� �Բ� �ٸ� (�߰�) ���� ����
	pathColor[rcvpath.index] = rcvpath.color;
	pathWidth[rcvpath.index] = rcvpath.width;
	// CLEAR �Է� ���� ���
	if (rcvpath.index == -1) {
		Clear(); // �����
		indexPath = 0;
	}
	// ��������
	if (indexPath <= rcvpath.index) {
		indexPath = rcvpath.index + 1;
	}
}

// ���� ���μ������� �����Ѵ�.
// �ϳ��� ������ ��ƾ�̴�.
// accept�� �ش� ���ῡ ���ؼ� ó���ϴ� �ڵ��ƾ(�Լ�)�̴�.
// ���ڴ� accept()�� ��ȯ�� �ش� ������ ���� ��ũ���ʹ�.
static void *handle_clnt(void *arg) {
	int i, str_len = 0;
	struct path buf;
	int clnt_sock = 0;

	// �о�: ������ �����Ǵ� �� ��� ����� ���� ����
	for (;(str_len = read(*((int*)arg), &buf, sizeof(buf))) != 0;) {
		RcvPath(buf);
		RepaintPath();
	}

	// �� ������ �����ϰ� �޸�-�����迭���� �����Ѵ�
	pthread_mutex_lock(&mutx);
	for (i = 0; i<clnt_cnt; i++) {
		if (clnt_sock == clnt_socks[i])
		{
			while (i++<clnt_cnt - 1)
				clnt_socks[i] = clnt_socks[i + 1];
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);

	return NULL;
}

// ������ ������ ��(reader)�� �޽����� ��� ��
// ("Loop"�� ����(�޽����� �߻�)�� �ƴ� ���(�а�) �ϱ� ���� ��... Ÿ�̸� �̺�Ʈ�� �ƴ� �̻�)
// (�޽����� ������ �޽����� �߻��� �̺�Ʈ���� �����ϴ� ���� �Ǵ�)
static void ServerLoopAccept() {
	// ���Ͽ� �ӽú���
	unsigned int clnt_adr_sz;
	struct sockaddr_in clnt_adr;
	int clnt_sock = 0;

	// �׸� �����͸� �д´�
	for (;;) {
		clnt_adr_sz = sizeof(clnt_adr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++] = clnt_sock;
		pthread_mutex_unlock(&mutx);

		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_detach(t_id);
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
	}
} // func

// ���� ���μ���
static void ServerInitSock() {
	struct sockaddr_in serv_adr;

	pthread_mutex_init(&mutx, NULL);
	serv_sock = socket(PF_INET, SOCK_STREAM, 0); // ���� ����

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(PORT);

	if (bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) // ���ε�: ��Ʈ ����
		fatal("bind() error");
	if (listen(serv_sock, 5) == -1) // accept() ������ ���·� �Ѵ�
		fatal("listen() error");
}

// ���� ���μ������� �����Ѵ�.
// �� �� ��ƾ�� reader(�д� ��)���� ȣ���Ѵ�.
extern void *Thread2Reader() {
	ServerInitSock();
	ServerLoopAccept();
	pthread_exit(NULL);
}

// �Ųٷ� ���� ��!
