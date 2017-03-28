// main.c : 주함수와 스레드 관리
#include "header.h"

#define N_THREADS 2
static pthread_t thid[N_THREADS];

// externs
char strAnswerCorrect[100] = "nn45nn3c8n";
bool isDrawer = false;
bool bGameOver = false;

extern int main(int argc, char* argv[]) {

	// 옵션 지정하지 않았을 때 에러 출력하고 종료
	if (argc < 3) {
		printf(
			"not enough argument. \n"
			"usage: \n"
			"[%s -w <IP>] for writer \n"
			"[%s -r <whatever>] for reader \n", 
			argv[0], argv[0]
		);
		return 1;
	}

	// writer와 reader 인자 구분
	if (strcmp(argv[1],"-w") == 0) {
		isDrawer = true;
		printf("starting as a writer. \n");
	} else if (strcmp(argv[1], "-r") == 0) {
		isDrawer = false;
		printf("starting as a reader. \n");
	} else {
		printf("invalid argument \n");
		return 1;
	}

	// 본편 실행
	printf("creating threads... \n");
	if (isDrawer) { // 클라이언트 프로세스
		ClientInitSock(argv[2]); // 그림 그리는 순간 패킷을 보내는 함수가 호출되기 때문에 패킷을 보내기 앞서 선행되어야 하는 연결 설정을 담고 있는 이 함수는 그림 그려서 패킷 보낼 수 있게 하는 그 지점보다 전에 위치해야 한다
		InitDisplay(); // x window 초기화
		pthread_create(&thid[0], NULL, Thread1, NULL);  // 스레드0: x window 입력 받음
		printf("hello there \n");
	} else /* if (!isDrawer) */ { // 서버 프로세스
		InitDisplay();
		pthread_create(&thid[0], NULL, Thread1, NULL);  // x window
		pthread_create(&thid[1], NULL, Thread2Reader, NULL);  // ipc (듣는다: 소켓 입력 받음)
		printf("hello there \n");
	}

	for (;;);

	/*
	for (i = 0; i < N_THREADS; ++i) {
		if (pthread_join(thid[i], NULL) != 0) {
			printf("end of process (-1) \n");
			return -1;
		}
	}
	*/

	// 종료
	printf("end of process (0) \n");
	return 0;
}



