#include "header.h"

//////////////////// color ////////////////////

// ���� ����
#define MAX_COLOR 18  // �⺻ ������ ��
#define WHITE 17
static char *color_name[MAX_COLOR] = {
	"Black", "Red", "Green", "Blue", "Navy",
	"Orange", "Yellow", "Pink", "Magenta", "Cyan",
	"Brown", "Grey", "LimeGreen", "Turquoise", "Violet",
	"Wheat", "Purple" , "White"
};

// "Black", "Red", "Green", "Blue", "Navy", 
// "Orange", "Yellow", "Pink", "Magenta", "Cyan",
// "Brown", "Grey", "LimeGreen", "Turquoise", "Violet",
// "Wheat", "Purple", "White"
// ex) XSetForeground(dpy, gc, UsrColorPixel("blue"));
static unsigned long UsrColorPixel(char *name) {  // get color
	XColor c0, c1;
	Colormap cmap;
	cmap = DefaultColormap(dpy, 0);
	XAllocNamedColor(dpy, cmap, name, &c0, &c1);
	return (c0.pixel);
}

// GC�� ���� ������ ����
extern void SetForegroundToColorIndex(int i) {
	color = UsrColorPixel(color_name[i]);
	XSetForeground(dpy, gc, color);
}

/*
// GC�� ���� ������ ����
static void SetForegroundToColorName(char* name) {
	color = UsrColorPixel(name);
	XSetForeground(dpy, gc, color);
}
*/

// GC�� ���� ������ ����
static void SetForegroundToColor(unsigned long _color) {
	color = _color;
	XSetForeground(dpy, gc, color);
}

//////////////////// ////////////////////

// GC�� ���� ���⸦ ����
extern void SetLineWidth(int width) {
	XGCValues gv;
	lineWidth = width;
	gv.line_width = width;
	XChangeGC(dpy, gc, GCLineWidth, &gv);
}

#define MAX_NPICK_WIDTH 6

// â�� �⺻ UI �׸��� �Լ�
extern void DrawPallete() {
	int i;
	XGCValues gv;

	// ĵ���� ���� ��ο�
	XSetForeground(dpy, gc, BlackPixel(dpy, 0)); // �ӽ÷� �׸� �Ŵϱ� static void SetForegroundToColor() ������� ����
	gv.line_width = 4;
	XChangeGC(dpy, gc, GCLineWidth, &gv);
	XDrawRectangle(dpy, w, gc, CANVAS_X, CANVAS_Y, CANVAS_WIDTH, CANVAS_HEIGHT);

	// �÷��̾� ���� ��ο�
	XSetForeground(dpy, gc, UsrColorPixel("blue"));
	gv.line_width = 4;
	XChangeGC(dpy, gc, GCLineWidth, &gv);
	XDrawRectangle(dpy, w, gc, PLAYER_X, PLAYER_Y, PLAYER_WIDTH, PLAYER_HEIGHT);

	// ���� ���� ��ο�: ��� ���� �������� ���� �������
	for (i = 0; i< WHITE; i++) {
		XSetForeground(dpy, gc, UsrColorPixel(color_name[i]));
		XFillRectangle(dpy, w, gc, 
			MARGIN + (PALET_AREA_X * i), MARGIN + PALET_AREA_Y,
			PALET_ITEM_WIDTH, PALET_ITEM_HEIGHT);
	}

	// ���� ���� ��ο�: ���
	i = WHITE;
	XSetForeground(dpy, gc, BlackPixel(dpy, 0));
	gv.line_width = 1;
	XChangeGC(dpy, gc, GCLineWidth, &gv);
	XDrawRectangle(dpy, w, gc, 
		MARGIN + (PALET_AREA_X * i), MARGIN + PALET_AREA_Y,
		PALET_ITEM_WIDTH, PALET_ITEM_HEIGHT);

	// ���� ���� ��ο�: CLEAR
	XSetFont(dpy, gc, XLoadFont(dpy, "fixed"));
	XDrawString(dpy, w, gc, 
		MARGIN + (PALET_AREA_X * (i + 1)), MARGIN + PALET_AREA_Y + 15, 
		"CLEAR", 5);

	// ���� ���� ��ο�
	for (i = 0; i< MAX_NPICK_WIDTH; i++) {
		XSetForeground(dpy, gc, BlackPixel(dpy, 0));
		gv.line_width = i * 5 + 5;
		XChangeGC(dpy, gc, GCLineWidth, &gv);
		XDrawLine(dpy, w, gc, 
			MARGIN + (BRUSH_AREA_X * i) + (BRUSH_ITEM_WIDTH / 2) - 1,
			BRUSH_AREA_Y + (BRUSH_ITEM_HEIGHT / 2) - 1,
			MARGIN + (BRUSH_AREA_X * i) + (BRUSH_ITEM_WIDTH / 2) + 1,
			BRUSH_AREA_Y + (BRUSH_ITEM_HEIGHT / 2) + 1);
	}

	// ���� ���ؽ�Ʈ�� �ǵ��ư���
	SetForegroundToColor(color);
	gv.line_width = lineWidth;
	XChangeGC(dpy, gc, GCLineWidth, &gv);
}

// ĵ������ �׸� ���� �����
extern void Clear() {
	XClearArea(dpy, w, CANVAS_X, CANVAS_Y, CANVAS_WIDTH, CANVAS_HEIGHT, 0);
	indexPath = 0;
}


// �Ǻ�

extern bool EventCursorIsWithinCanvas(XEvent xe) {
	return (xe.xmotion.x > CANVAS_X && xe.xmotion.x < CANVAS_WIDTH + CANVAS_X)
		&& (xe.xmotion.y > CANVAS_Y && xe.xmotion.y < CANVAS_HEIGHT + CANVAS_Y);
}

extern bool EventCursorIsWithinColorPick(XEvent xe) {
	return xe.xmotion.y > MARGIN + PALET_AREA_Y && xe.xmotion.y < MARGIN + PALET_AREA_Y + PALET_ITEM_HEIGHT;
}

extern bool EventCursorIsWithinWidthPick(XEvent xe) {
	return xe.xmotion.y > BRUSH_AREA_Y && xe.xmotion.y < BRUSH_AREA_Y + BRUSH_ITEM_HEIGHT;
}

extern bool EventCursorIsBeingClicked(XEvent xe) {
	return xe.xmotion.state & Button1Mask;
}

// Clear ��ư�� ���ȴ��� ���� ��ȯ
extern bool GetClearPick(XEvent xe) {
	int i = MAX_COLOR;
	return xe.xmotion.x > MARGIN + (BRUSH_AREA_X * i) && xe.xmotion.x < MARGIN + (BRUSH_AREA_X * i) + BRUSH_ITEM_WIDTH;
}

// �� ��° ��ư�� ���ȴ��� ��ȯ
extern int GetColorPick(XEvent xe) {
	int i;
	for (i = 0; i < MAX_COLOR + 1; ++i) {
		if (xe.xmotion.x > MARGIN + (BRUSH_AREA_X * i)
			&& xe.xmotion.x < MARGIN + (BRUSH_AREA_X * i) + BRUSH_ITEM_WIDTH) {
			break;
		} // if
	} // for
	if (i >= MAX_COLOR) { return -1; }
	// printf("color %d \n", i);
	return i;
}

// �� ��° ��ư�� ���ȴ��� ��ȯ
extern int GetWidthPick(XEvent xe) {
	int i;
	for (i = 0; i < MAX_COLOR + 1; ++i) {
		if (xe.xmotion.x > MARGIN + (PALET_AREA_X * i)
			&& xe.xmotion.x < MARGIN + (PALET_AREA_X * i) + PALET_ITEM_WIDTH) {
			break;
		} // if
	} // for
	if (i >= MAX_NPICK_WIDTH) { return -1; }
	// printf("width arr n %d \n", i);
	return i;
}




