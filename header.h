// header.h : ������ ���
#include <stdio.h>
#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

typedef enum bool{ false, true } bool;

// â�� ��Ұ� ��ġ�� �� â�� �����̳� ���� ����
#define MARGIN				50

// ĵ���� ����: �׸� �׸��� ��
#define CANVAS_X			MARGIN
#define CANVAS_Y			MARGIN
#define CANVAS_WIDTH		600
#define CANVAS_HEIGHT		300

// �÷��̾� ����: �����ʿ� �׷����� �簢�� �� ��
#define PLAYER_X			MARGIN + CANVAS_WIDTH + MARGIN
#define PLAYER_Y			MARGIN
#define PLAYER_WIDTH		300
#define PLAYER_HEIGHT		CANVAS_HEIGHT

// ���� (���� ����) ����
#define PALET_AREA_X		MARGIN
#define PALET_AREA_Y		370
#define PALET_ITEM_WIDTH	30
#define PALET_ITEM_HEIGHT	30

// ���� ���� ���� ����
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

// drawn line
#define	MAX_INDEX_PATH	100000
extern int				indexPath;
extern XPoint			path[MAX_INDEX_PATH];
extern int				pathColor[MAX_INDEX_PATH];
extern int				pathWidth[MAX_INDEX_PATH];

// drawing
extern void ContinuePath();
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

// user_input // ipc
extern void *Thread1();
extern void *Thread2();


