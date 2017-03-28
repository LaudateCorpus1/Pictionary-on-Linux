// event.c : 창의 초기화와 이벤트 처리 거시적 관점
#include "header.h"

// main window
Display		*dpy;
Window		w;
XEvent		xe;
// other ui stuff (context)
GC			gc;
int			lineWidth;
int			color;
// drawn line
int			indexPath;
XPoint		path[MAX_INDEX_PATH];
int			pathColor[MAX_INDEX_PATH];
int         pathWidth[MAX_INDEX_PATH];

extern void InitDisplay() {
	// 1. 창 띄우고 이벤트(입력) 연결
	dpy = XOpenDisplay(NULL);
	w = XCreateSimpleWindow(dpy, RootWindow(dpy, 0), 50, 50, 1050, 500,  // Display, Window, x, y, width, height
							5, BlackPixel(dpy, 0), WhitePixel(dpy, 0));  // border width, border color, bg color
	XSelectInput(dpy, w, ExposureMask | 
		ButtonPressMask | ButtonReleaseMask |  // 마우스 버튼 눌림, 뗌
	//	EnterWindowMask | LeaveWindowMask |  // 창에 들어옴, 나감
		PointerMotionMask  // 커서 이동 (MotionNotify 발생)
	);
	XMapWindow(dpy, w);
	XFlush(dpy);
	// 2. gc, font, line, width, color 설정
	gc = XCreateGC(dpy, w, 0L, (XGCValues *)NULL);
	color = BlackPixel(dpy, 0);
	lineWidth = 5;
	XSetFont(dpy, gc, XLoadFont(dpy, "fixed"));
	XSetLineAttributes(dpy, gc, lineWidth, LineSolid, CapRound, JoinRound);
	XFlush(dpy);
} // func

// (XEvent xe)
static void onExpose() {
	DrawPallete();
	RepaintPath();
} // func

static void onMouseMoved(XEvent xe) {
	if (!isDrawer) { return; } // 이 프로세스가 그림 그리는 측인 경우에만 그린다
	// 캔버스 안에서 커서가 눌린 채 움직였고 그림 데이터 저장공간이 여유 있다면
	if (EventCursorIsWithinCanvas(xe) && EventCursorIsBeingClicked(xe) && indexPath < MAX_INDEX_PATH) {
		// 현재위치 저장
		path[indexPath].x = xe.xmotion.x;
		path[indexPath].y = xe.xmotion.y;
		// 직전위치부터 현재위치까지 잇는 선을 그린다 (드로우)
		if (indexPath == 0) { return; }
		XDrawLine(dpy, w, gc,
			path[indexPath - 1].x, path[indexPath - 1].y,
			path[indexPath].x, path[indexPath].y);
		// 다음으로 (현재위치는 직전위치가 된다)
		++indexPath;
		// 기본 UI 그려주기
		// DrawPallete();
		// 통신 추가
		if (isDrawer) {
			SndPath(indexPath - 1, xe.xmotion.x, xe.xmotion.y, color, lineWidth);
		}
	} // if
} // func

static void onButtonPress(XEvent xe) {
	if (!isDrawer) { return; } // 이 프로세스가 그림 그리는 측인 경우에만 그린다
	if (EventCursorIsWithinCanvas(xe) && indexPath < MAX_INDEX_PATH) {
		ContinuePath(xe.xmotion.x, xe.xmotion.y);
	}
} // func

static void onButtonRelease(XEvent xe) {
	int result;
	if (!isDrawer) { return; } // 이 프로세스가 그림 그리는 측인 경우에만 그린다
	XEvent local_xe = xe;
	if (EventCursorIsWithinColorPick(local_xe)) { // 색상 선택
		// printf("EventCursorIsWithinColorPick\n");
		if ((result = GetColorPick(local_xe)) != -1) { // 몇 번째 요소인지 확인하고
			SetForegroundToColorIndex(result); // 그 요소의 색상을 적용한다
		} else if (GetClearPick(local_xe)) { // "CLEAR" 선택시
			Clear();
		}
	} else if (EventCursorIsWithinWidthPick(local_xe)) { // 붓 굵기 선택
		// printf("EventCursorIsWithinWidthPick\n");
		if ((result = GetWidthPick(local_xe)) != -1) { // 몇 번째 요소인지 확인하고
			SetLineWidth(result * 5 + 5); // 그 요소의 굵기를 적용한다
		}
	} else if (EventCursorIsWithinCanvas(local_xe)) {
		ContinuePath(-1, -1);
	} // elif
} // func

// input processing routine for main thread
static void UserInputMessageLoop() {
	for (;;) {
		XNextEvent(dpy, &xe);
		switch (xe.type) {
		case Expose:  // 창 크기 변경 (resize) 
			onExpose(xe);
			break;
		case MotionNotify:  // 커서 이동
			onMouseMoved(xe);
			break;
		case ButtonPress:  // 마우스 클릭: 누름
			onButtonPress(xe);
			break;
		case ButtonRelease:  // 마우스 클릭: 뗌
			onButtonRelease(xe);
			break;
		case EnterNotify:  // 커서가 창 안으로 들어옴
			break;
		case LeaveNotify:  // 커서가 창 밖으로 나감
			break;
		default:  // Neither of above
			printf("xe.type unknown: check either XSelectInput() or switch case (warning) \n");
			break;
		}
	} // for(;;)
} // func

extern void *Thread1() {
	UserInputMessageLoop();
	pthread_exit(NULL);
} // func

