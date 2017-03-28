// ipc.c : 프로세스간 통신
#include "header.h"

#define MSGTYPE_PATH		1
#define MSGTYPE_ANSWER		2
#define MSGTYPE_GAMEOVER	3

#define PORT				9000
static pthread_t t_id; // accept() 수행 뒤에 생성되는 각각의 연결에 대한 스레드

// extern
// sock :
int sock; // 클라이언트 프로세스가 사용
int serv_sock; // 서버 프로세스가 사용

// sock (server) :
#define MAX_CLNT 256
static int clnt_cnt = 0;
static int clnt_socks[MAX_CLNT];
static pthread_mutex_t mutx;

// 주고 받을 데이터: 그린 점
struct path {
	int		index;
	int		x;
	int		y;
	int		color;
	int		width;
};

// 오류
static void fatal(char *err) {
	perror(err);
	exit(0);
}

// 클라이언트 프로세스
extern void ClientInitSock(char* ip) {
	struct sockaddr_in serv_addr;

	sock = socket(PF_INET, SOCK_STREAM, 0); // 소켓 생성

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ip);
	serv_addr.sin_port = htons(PORT);

	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) // 소켓 연결
		fatal("connect() error");
}

// 클라이언트 프로세스에서 수행
// 그린 지점 한 단위를 메시지 전송
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

////////////////////////////////// 이하 서버 프로세스 ////////////////////////////////// 
// 코드는 거꾸로 읽을 것

// 서버 프로세스에서 수행
// 받은 그린 데이터(구조체 메시지; 점 하나)를 풀어서 저장
static void RcvPath(struct path rcvpath) {
	if (indexPath >= MAX_INDEX_PATH) { return; } // 여분의 공간이 없으면 추가할 수 없음
	// 현재위치 저장
	path[rcvpath.index].x = rcvpath.x;
	path[rcvpath.index].y = rcvpath.y;
	// 현재위치와 함께 다른 (추가) 정보 저장
	pathColor[rcvpath.index] = rcvpath.color;
	pathWidth[rcvpath.index] = rcvpath.width;
	// CLEAR 입력 받은 경우
	if (rcvpath.index == -1) {
		Clear(); // 지운다
		indexPath = 0;
	}
	// 다음으로
	if (indexPath <= rcvpath.index) {
		indexPath = rcvpath.index + 1;
	}
}

// 서버 프로세스에서 실행한다.
// 하나의 스레드 루틴이다.
// accept된 해당 연결에 대해서 처리하는 코드루틴(함수)이다.
// 인자는 accept()가 반환한 해당 연결의 소켓 디스크립터다.
static void *handle_clnt(void *arg) {
	int i, str_len = 0;
	struct path buf;
	int clnt_sock = 0;

	// 읽어: 연결이 유지되는 한 계속 통신할 것을 지시
	for (;(str_len = read(*((int*)arg), &buf, sizeof(buf))) != 0;) {
		RcvPath(buf);
		RepaintPath();
	}

	// 이 연결을 해제하고 메모리-관리배열에서 축출한다
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

// 정답을 맞히는 측(reader)이 메시지를 듣게 함
// ("Loop"은 쓰기(메시지의 발생)가 아닌 듣게(읽게) 하기 위한 것... 타이머 이벤트가 아닌 이상)
// (메시지의 전송은 메시지가 발생한 이벤트에서 수행하는 것이 옳다)
static void ServerLoopAccept() {
	// 소켓용 임시변수
	unsigned int clnt_adr_sz;
	struct sockaddr_in clnt_adr;
	int clnt_sock = 0;

	// 그린 데이터를 읽는다
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

// 서버 프로세스
static void ServerInitSock() {
	struct sockaddr_in serv_adr;

	pthread_mutex_init(&mutx, NULL);
	serv_sock = socket(PF_INET, SOCK_STREAM, 0); // 소켓 생성

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(PORT);

	if (bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) // 바인드: 포트 엮어
		fatal("bind() error");
	if (listen(serv_sock, 5) == -1) // accept() 가능한 상태로 한다
		fatal("listen() error");
}

// 서버 프로세스에서 실행한다.
// 즉 이 루틴을 reader(읽는 측)만이 호출한다.
extern void *Thread2Reader() {
	ServerInitSock();
	ServerLoopAccept();
	pthread_exit(NULL);
}

// 거꾸로 읽을 것!
