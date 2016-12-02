// main.c : ���Լ��� ������ ���� + �͹̳� �Է� �ޱ�
#include "header.h"

#define N_THREADS 2
static pthread_t thid[N_THREADS];

// externs
char *strAnswerCorrect = "nnnnn";
bool isDrawer = false;
bool bGameOver = false;

extern int main(int argc, char* argv[]) {
	int i;
	char tmp_strAnswer[100];

	// �ɼ� �������� �ʾ��� �� ���� ����ϰ� ����
	if (argc == 1) {
		printf(
			"not enough argument. \n"
			"usage: \n"
			"[%s -w] for writer \n"
			"[%s -r] for reader \n", 
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
	/*
	// �׸���(writer) ������ ���þ� �Է� ����
	if (isDrawer) {
		printf("tell me what your answer is: ");
		tcflush(0, TCIFLUSH);
		scanf("%s", tmp_strAnswer);
		getchar();
		strAnswerCorrect = tmp_strAnswer;
		printf("the answer is %s\n", strAnswerCorrect);
		tcflush(0, TCIFLUSH);		
	//	SndAnswerCorrect(strAnswerCorrect);
	}
	*/
	// ���� ����
	printf("creating threads... \n");
	if (isDrawer) {
		IpcInitClear();
		InitDisplay();
		pthread_create(&thid[0], NULL, Thread1, NULL);  // user_input
		pthread_create(&thid[1], NULL, Thread2Writer, NULL);  // ipc
	} else /* if (!isDrawer) */ {
		IpcInit();
		InitDisplay();
		pthread_create(&thid[0], NULL, Thread1, NULL);  // user_input
		pthread_create(&thid[1], NULL, Thread2Reader, NULL);  // ipc
	}	
	printf("hello there \n");

	for (;;);
	/*
	// ������(reader) ������ ���þ� �Է� ����
	if (!isDrawer) {
		for (;;) {
			printf("\ntell me what you think the answer is: \n");
			tcflush(0, TCIFLUSH);
			scanf("%s", tmp_strAnswer); // �Է� �޾Ƽ�
			if (strcmp(strAnswerCorrect, tmp_strAnswer) == 0) { // �����ٸ�
				for (i = 0;i < 5;++i) {
					printf("%s, you got that right\n", tmp_strAnswer);
				}
				SndGameOver();
				break;
			}
			else { // Ʋ�ȴٸ�
				printf("your answer is %s, which is incorrect. lol\n", tmp_strAnswer);
			}
			tcflush(0, TCIFLUSH);
		}
	} else { // �׸���(writer) ���� ��� ������ ���� ������ ����
		for (;!bGameOver;sleep(3));
		for (i = 0;i < 5;++i) {
			printf("%s, your mate got that one right\n", strAnswerCorrect);
		}
	}
	*/
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



