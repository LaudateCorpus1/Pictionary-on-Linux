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

static void InitDisplay() {
	// 1. â ���� �̺�Ʈ(�Է�) ����
	dpy = XOpenDisplay(NULL);
	w = XCreateSimpleWindow(dpy, RootWindow(dpy, 0), 50, 50, 1050, 500,  // Display, Window, x, y, width, height
							5, BlackPixel(dpy, 0), WhitePixel(dpy, 0));  // border width, border color, bg color
	XSelectInput(dpy, w, ExposureMask | 
		ButtonPressMask | ButtonReleaseMask |  // ���콺 ��ư ����, ��
	//	EnterWindowMask | LeaveWindowMask |  // â�� ����, ����
		PointerMotionMask  // Ŀ�� �̵� (MotionNotify �߻�)
	);
	XMapWindow(dpy, w);
	XFlush(dpy);
	// 2. gc, font, line, width, color ����
	gc = XCreateGC(dpy, w, 0L, (XGCValues *)NULL);
	color = BlackPixel(dpy, 0);
	lineWidth = 5;
	XSetFont(dpy, gc, XLoadFont(dpy, "fixed"));
	XSetLineAttributes(dpy, gc, lineWidth, LineSolid, CapRound, JoinRound);
	XFlush(dpy);
} // func

// (XEvent xe)
static void onDraw() {
	DrawPallete();
	// repaint(xe);
} // func

static void onMouseMoved(XEvent xe) {
	unsigned static char n = 0;
	// ĵ���� �ȿ��� Ŀ���� ���� ä �������� �׸� ������ ��������� ���� �ִٸ�
	if (EventCursorIsWithinCanvas(xe) && EventCursorIsBeingClicked(xe) && indexPath < MAX_INDEX_PATH) {
		// ������ġ ����
		path[indexPath].x = xe.xmotion.x;
		path[indexPath].y = xe.xmotion.y;
		// ������ġ���� ������ġ���� �մ� ���� �׸��� (��ο�)
		if (indexPath == 0) { return; }
		XDrawLine(dpy, w, gc,
			path[indexPath - 1].x, path[indexPath - 1].y,
			path[indexPath].x, path[indexPath].y);
		// �������� (������ġ�� ������ġ�� �ȴ�)
		++indexPath;
		// �⺻ UI �׷��ֱ�
		++n;
		if (n == 0) {
			DrawPallete();
		}
	} // if
} // func

static void onButtonPress(XEvent xe) {
	if (EventCursorIsWithinCanvas(xe) && indexPath < MAX_INDEX_PATH) {
		// ������ġ ����
		path[indexPath].x = xe.xmotion.x;
		path[indexPath].y = xe.xmotion.y;
		// �������� (������ġ�� ������ġ�� �ȴ�)
		++indexPath;
	}
} // func

static void onButtonRelease(XEvent xe) {
	int result;
	XEvent local_xe = xe;
	if (EventCursorIsWithinColorPick(local_xe)) { // ���� ����
		// printf("EventCursorIsWithinColorPick\n");
		if ((result = GetColorPick(local_xe)) != -1) { // �� ��° ������� Ȯ���ϰ�
			SetForegroundToColorIndex(result); // �� ����� ������ �����Ѵ�
		} else if (GetClearPick(local_xe)) { // "CLEAR" ���ý�
			Clear();
		}
	} // if
	else if (EventCursorIsWithinWidthPick(local_xe)) { // �� ���� ����
		// printf("EventCursorIsWithinWidthPick\n");
		if ((result = GetWidthPick(local_xe)) != -1) { // �� ��° ������� Ȯ���ϰ�
			SetLineWidth(result * 5 + 5); // �� ����� ���⸦ �����Ѵ�
		}
	} // elif
} // func

// input processing routine for main thread
static void UserInputMessageLoop() {
	for (;;) {
		XNextEvent(dpy, &xe);
		switch (xe.type) {
		case Expose:  // â ũ�� ���� (resize) 
			onDraw(xe);
			break;
		case MotionNotify:  // Ŀ�� �̵�
			onMouseMoved(xe);
			break;
		case ButtonPress:  // ���콺 Ŭ��: ����
			onButtonPress(xe);
			break;
		case ButtonRelease:  // ���콺 Ŭ��: ��
			onButtonRelease(xe);
			break;
		case EnterNotify:  // Ŀ���� â ������ ����
			break;
		case LeaveNotify:  // Ŀ���� â ������ ����
			break;
		default:  // Neither of above
			printf("xe.type unknown: check either XSelectInput() or switch case (warning) \n");
			break;
		}
	} // for(;;)
} // func

extern void *Thread1() {
	InitDisplay();
	UserInputMessageLoop();
	pthread_exit(NULL);
} // func

