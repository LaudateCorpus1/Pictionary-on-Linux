// main.c : ���Լ��� ������ ����
#include "header.h"

#define N_THREADS 2
static pthread_t thid[N_THREADS];

// externs
char strAnswerCorrect[100] = "nn45nn3c8n";
bool isDrawer = false;
bool bGameOver = false;

extern int main(int argc, char* argv[]) {

	// �ɼ� �������� �ʾ��� �� ���� ����ϰ� ����
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

	// writer�� reader ���� ����
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

	// ���� ����
	printf("creating threads... \n");
	if (isDrawer) { // Ŭ���̾�Ʈ ���μ���
		ClientInitSock(argv[2]); // �׸� �׸��� ���� ��Ŷ�� ������ �Լ��� ȣ��Ǳ� ������ ��Ŷ�� ������ �ռ� ����Ǿ�� �ϴ� ���� ������ ��� �ִ� �� �Լ��� �׸� �׷��� ��Ŷ ���� �� �ְ� �ϴ� �� �������� ���� ��ġ�ؾ� �Ѵ�
		InitDisplay(); // x window �ʱ�ȭ
		pthread_create(&thid[0], NULL, Thread1, NULL);  // ������0: x window �Է� ����
		printf("hello there \n");
	} else /* if (!isDrawer) */ { // ���� ���μ���
		InitDisplay();
		pthread_create(&thid[0], NULL, Thread1, NULL);  // x window
		pthread_create(&thid[1], NULL, Thread2Reader, NULL);  // ipc (��´�: ���� �Է� ����)
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

	// ����
	printf("end of process (0) \n");
	return 0;
}



