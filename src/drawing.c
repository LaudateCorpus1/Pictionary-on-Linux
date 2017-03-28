// drawing.c 모든 자잘한 (미시적인) 그리는 처리와 그에 연관된 것
#include "header.h"

//////////////////// color ////////////////////

// color related
#define MAX_COLOR 18  // number of basic colors available to pick
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

// GC의 선의 색상을 지정
extern void SetForegroundToColorIndex(int i) {
	color = UsrColorPixel(color_name[i]);
	XSetForeground(dpy, gc, color);
}

/*
// GC의 선의 색상을 지정
static void SetForegroundToColorName(char* name) {
	color = UsrColorPixel(name);
	XSetForeground(dpy, gc, color);
}
*/

// GC의 선의 색상을 지정
static void SetForegroundToColor(unsigned long _color) {
	color = _color;
	XSetForeground(dpy, gc, color);
}

//////////////////// ////////////////////

// GC의 선의 굵기를 지정
extern void SetLineWidth(int width) {
	XGCValues gv;
	lineWidth = width;
	gv.line_width = width;
	XChangeGC(dpy, gc, GCLineWidth, &gv);
}

// path에 점 하나를 추가함
extern void ContinuePath(int x, int y) {
	if (indexPath >= MAX_INDEX_PATH) { return; } // 여분의 공간이 없으면 추가할 수 없음
	// 현재위치 저장
	path[indexPath].x = x;
	path[indexPath].y = y;
	// 현재위치와 함께 다른 (추가) 정보 저장
	pathColor[indexPath] = color;
	pathWidth[indexPath] = lineWidth;
	// 다음으로 (현재위치는 직전위치가 된다)
	++indexPath;
	// 통신 추가
	if (isDrawer) {
		SndPath(indexPath - 1, x, y, color, lineWidth);
	}
}

#define MAX_NPICK_WIDTH 6

// 창에 기본 UI 그리는 함수
extern void DrawPallete() {
	int i;
	XGCValues gv;

	// 캔버스 영역 드로우
	XSetForeground(dpy, gc, BlackPixel(dpy, 0)); // 임시로 그릴 거니까 static void SetForegroundToColor() 사용하지 않음
	gv.line_width = 4;
	XChangeGC(dpy, gc, GCLineWidth, &gv);
	XDrawRectangle(dpy, w, gc, CANVAS_X, CANVAS_Y, CANVAS_WIDTH, CANVAS_HEIGHT);

	// 정답 영역 드로우
	XSetForeground(dpy, gc, UsrColorPixel("blue"));
	gv.line_width = 4;
	XChangeGC(dpy, gc, GCLineWidth, &gv);
	XDrawRectangle(dpy, w, gc, PLAYER_X, PLAYER_Y, PLAYER_WIDTH, PLAYER_HEIGHT);

	// 정답 영역 문자
	XSetForeground(dpy, gc, BlackPixel(dpy, 0)); // 임시로 그릴 거니까 static void SetForegroundToColor() 사용하지 않음
	if (isDrawer) {
		XDrawString(dpy, w, gc, 
			PLAYER_X + (PLAYER_WIDTH / 5), 
			PLAYER_Y + (PLAYER_HEIGHT / 2), 
			strAnswerCorrect, strlen(strAnswerCorrect));
	} else {
		XDrawString(dpy, w, gc, 
			PLAYER_X + (PLAYER_WIDTH / 5),
			PLAYER_Y + (PLAYER_HEIGHT / 2),
			"????????????? : guess what!", strlen("????????????? : guess what!"));
	}

	// 물감 영역 드로우: 흰색 차례 직전까지 색깔 순서대로
	for (i = 0; i< WHITE; i++) {
		XSetForeground(dpy, gc, UsrColorPixel(color_name[i]));
		XFillRectangle(dpy, w, gc, 
			MARGIN + (PALET_AREA_X * i), MARGIN + PALET_AREA_Y,
			PALET_ITEM_WIDTH, PALET_ITEM_HEIGHT);
	}

	// 물감 영역 드로우: 흰색
	i = WHITE;
	XSetForeground(dpy, gc, BlackPixel(dpy, 0));
	gv.line_width = 1;
	XChangeGC(dpy, gc, GCLineWidth, &gv);
	XDrawRectangle(dpy, w, gc, 
		MARGIN + (PALET_AREA_X * i), MARGIN + PALET_AREA_Y,
		PALET_ITEM_WIDTH, PALET_ITEM_HEIGHT);

	// 물감 영역 드로우: CLEAR
	XSetFont(dpy, gc, XLoadFont(dpy, "fixed"));
	XDrawString(dpy, w, gc, 
		MARGIN + (PALET_AREA_X * (i + 1)), MARGIN + PALET_AREA_Y + 15, 
		"CLEAR", 5);

	// 붓의 굵기 드로우
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

	// 이전 콘텍스트로 되돌아간다
	SetForegroundToColor(color);
	gv.line_width = lineWidth;
	XChangeGC(dpy, gc, GCLineWidth, &gv);
}

// 다시 따라 그린다
extern void RepaintPath() {
	int i; // 반복문 제어 변수
	int contextColor = color; // 뒷정리 준비: 이 함수가 끝나면 돌아갈 곳 1 (스택 프레임처럼 시작할 때 한다)
	int contextWidth = lineWidth;  // 뒷정리 준비: 이 함수가 끝나면 돌아갈 곳 2

	SetForegroundToColor(pathColor[0]);
	SetLineWidth(pathWidth[0]);
	for (i = 1; i<indexPath; i++) {
		if (path[i].x != -1) { // 지금 지점이 끊어진 점(-1)이 아닌 유효한 점이니까, 그리기를 시작하는 중이던지 그리는 한 중간이던지 둘 중 하나다.
			if (path[i - 1].x != -1) { // 이전 지점도 지금 지점도 끊어진 점(-1)이 아니니까, 붓칠을 하는 중간이다.
				XDrawLine(dpy, w, gc, path[i - 1].x, path[i - 1].y, path[i].x, path[i].y); // 두 점을 잇는 선을 따라 붓칠한다.
			} else if (path[i - 1].x == -1) { // 이전 지점은 끊어졌고 지금 지점은 있으니까, 두점을 따라 잇는 선을 그리기 시작하는 때다.
				SetForegroundToColor(pathColor[i]); // 붓을 든다.
				SetLineWidth(pathWidth[i]);
			}
		} else { /* 지금 지점이 끊어진 점(-1)이 아닌 유효한 점이니까, 그리기를 중단한 상태다. 붓을 내린다. */ }
	}

	SetForegroundToColor(contextColor); // 뒷정리
	SetLineWidth(contextWidth);
} // func

// 캔버스에 그린 것을 지운다
extern void Clear() {
	XClearArea(dpy, w, CANVAS_X, CANVAS_Y, CANVAS_WIDTH, CANVAS_HEIGHT, 0);
	indexPath = 0;
	// 통신 추가
	if (isDrawer) {
		SndPath(-1, -1, -1, color, lineWidth);
	}
}


// 판별

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

// 
버튼이 눌렸는지 여부 반환
extern bool GetClearPick(XEvent xe) {
	int i = MAX_COLOR;
	return xe.xmotion.x > MARGIN + (BRUSH_AREA_X * i) && xe.xmotion.x < MARGIN + (BRUSH_AREA_X * i) + BRUSH_ITEM_WIDTH;
}

// 몇 번째 버튼이 눌렸는지 반환
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

// 몇 번째 버튼이 눌렸는지 반환
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




