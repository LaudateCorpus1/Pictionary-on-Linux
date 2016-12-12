// header.h : 유일한 헤더
#include <stdio.h>
#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>
//
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
//
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef enum bool{ false, true } bool;

// 창에 요소가 배치될 때 창의 왼쪽이나 위쪽 여백
#define MARGIN				50

// 캔버스 영역: 그림 그리는 곳
#define CANVAS_X			MARGIN
#define CANVAS_Y			MARGIN
#define CANVAS_WIDTH		600
#define CANVAS_HEIGHT		300

// 플레이어 영역: 오른쪽에 그려지는 사각형 두 개
#define PLAYER_X			MARGIN + CANVAS_WIDTH + MARGIN
#define PLAYER_Y			MARGIN
#define PLAYER_WIDTH		300
#define PLAYER_HEIGHT		CANVAS_HEIGHT

// 물감 (색상 선택) 영역
#define PALET_AREA_X		MARGIN
#define PALET_AREA_Y		370
#define PALET_ITEM_WIDTH	30
#define PALET_ITEM_HEIGHT	30

// 붓의 굵기 선택 영역
#define BRUSH_ITEM_HEIGHT	30
#define BRUSH_ITEM_WIDTH	30
#define BRUSH_AREA_X		MARGIN
#define BRUSH_AREA_Y		MARGIN + CANVAS_HEIGHT + BRUSH_ITEM_HEIGHT

// main window
extern Display			*dpy;
extern Window			w;
extern XEvent			xe;

// other ui stuff (context)
extern GC				gc;
extern int				lineWidth;
extern int				color;

// socket
extern int				sock;
extern int				serv_sock, clnt_sock;

// ipc
extern char				strAnswerCorrect[100];
extern bool				isDrawer;
extern bool				bGameOver;

// drawn line
#define	MAX_INDEX_PATH	100000
extern int				indexPath;
extern XPoint			path[MAX_INDEX_PATH];
extern int				pathColor[MAX_INDEX_PATH];
extern int				pathWidth[MAX_INDEX_PATH];

// drawing
extern void ContinuePath(int x, int y);
extern void DrawPallete();
extern void RepaintPath();
extern void Clear();
extern bool EventCursorIsWithinCanvas(XEvent);
extern bool EventCursorIsWithinColorPick(XEvent);
extern bool EventCursorIsWithinWidthPick(XEvent);
extern bool EventCursorIsBeingClicked(XEvent);
extern bool GetClearPick(XEvent);
extern int GetColorPick(XEvent);
extern int GetWidthPick(XEvent);
extern void SetForegroundToColorIndex(int i);
extern void SetLineWidth(int width);

// ipc
extern void SndPath(int _index, int _x, int _y, int _color, int _width);
extern void ClientInitSock(char* ip);


// init
extern void InitDisplay();

// threads
extern void *Thread1();
extern void *Thread2Reader();


