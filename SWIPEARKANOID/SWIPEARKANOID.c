#include<Windows.h>
#include<tchar.h>
#include<time.h>
#include<math.h>
#include<process.h>
#include"resource.h"

#define BSIZE 8
#define BALL_MAX_SIZE 10000
#define GROUND 490
#define SPACE 2
#define BALL_SPACE (BSIZE * 2) + 2
#define ROW 10
#define COL 6
#define SPEED 10

HINSTANCE hInst;
HWND hWnd;
BOOL MOUSECHECK;
BOOL GAMEMAINCHECK = TRUE;
BOOL GAMEENDCHECK = FALSE;
int STAGE = 0;
int rotX;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow) {
	srand((unsigned)time(NULL));
	HWND hwnd;
	MSG msg;
	WNDCLASS WndClass;
	hInst = hInstance;

	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR));
	WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = _T("Window Class Name");

	RegisterClass(&WndClass);

	hwnd = CreateWindow(
		_T("Window Class Name"),
		_T("SWIPE ARKANOID"),
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		615,
		615,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

typedef struct {
	int r, g, b;
}RGBCOLOR;

typedef struct {
	double r, g, b;
}RGBCOLOR_DOUBLE;
RGBCOLOR_DOUBLE dRgb;

typedef struct {
	double x, y;
	double degree;
	double xDist, yDist;
	BOOL VISIBLE;
	BOOL MOVABLE;
	BOOL START;
}BALL;
BALL ball[BALL_MAX_SIZE];

typedef struct mBlock {
	int count;
	RECT rect;
	RGBCOLOR RGB;
}mBLOCK;
mBLOCK block[ROW][COL];

double length(double x1, double y1, double x2, double y2) {
	return sqrt(((x2 - x1)*(x2 - x1)) + ((y2 - y1)*(y2 - y1)));
}

void RndBlockCreate() {
	int blockNum = rand() % COL;
	if (STAGE <= 3) {
		blockNum = rand() % COL;
		block[1][blockNum].count = STAGE;
		block[1][blockNum].RGB.r = 255;
		block[1][blockNum].RGB.g = 65;
		block[1][blockNum].RGB.b = 82;
	}
	else if (STAGE <= 10) {
		for (int i = 0; i < 2; i++) {
			blockNum = rand() % COL;
			block[1][blockNum].count = STAGE;
			block[1][blockNum].RGB.r = 255;
			block[1][blockNum].RGB.g = 65;
			block[1][blockNum].RGB.b = 82;
		}
	}
	else if (STAGE <= 25) {
		for (int i = 0; i < 3; i++) {
			blockNum = rand() % COL;
			block[1][blockNum].count = STAGE;
			block[1][blockNum].RGB.r = 255;
			block[1][blockNum].RGB.g = 65;
			block[1][blockNum].RGB.b = 82;
		}
	}
	else if (STAGE <= 50) {
		for (int i = 0; i < 4; i++) {
			blockNum = rand() % COL;
			block[1][blockNum].count = STAGE;
			block[1][blockNum].RGB.r = 255;
			block[1][blockNum].RGB.g = 65;
			block[1][blockNum].RGB.b = 82;
		}
	}
	else {
		for (int i = 0; i < COL; i++) {
			blockNum = rand() % COL;
			block[1][blockNum].count = STAGE;
			block[1][blockNum].RGB.r = 255;
			block[1][blockNum].RGB.g = 65;
			block[1][blockNum].RGB.b = 82;
		}
	}
}

void nextStage() {	
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			block[ROW - 1 - i][j].count = block[ROW - 1 - i - 1][j].count;
			block[ROW - 1 - i][j].RGB.r = block[ROW - 1 - i - 1][j].RGB.r;
			block[ROW - 1 - i][j].RGB.g = block[ROW - 1 - i - 1][j].RGB.g;
			block[ROW - 1 - i][j].RGB.b = block[ROW - 1 - i - 1][j].RGB.b;
		}
	}
	for (int i = 0; i < COL; i++)
		block[1][i].count = 0;
	RndBlockCreate();
}

void AddBallCount() {
	if (STAGE != 0) {
		ball[STAGE].x = ball[STAGE - 1].x;
	}
	ball[STAGE].y = GROUND - BSIZE;
	ball[STAGE].VISIBLE = TRUE;
	ball[STAGE].MOVABLE = TRUE;
	ball[STAGE].START = TRUE;

	if (STAGE >= BALL_MAX_SIZE - 1)
		return;
	else
		STAGE++;
}

BOOL isGameEnd() {
	for (int i = 0; i < COL; i++) {
		if (block[ROW - 1][i].count > 0)
			return TRUE;
	}
	return FALSE;
}

void ThreadProc1() {//Thread
	BOOL TRIGGER=TRUE;
	rotX = 0;
	for (;;) {
		if (!GAMEMAINCHECK && !GAMEENDCHECK && !MOUSECHECK) {
			if (TRIGGER) {
				rotX += 1;
			}
			else {
				rotX -= 1;
			}
			if (rotX == 35)
				TRIGGER = FALSE;
			if (rotX == 0)
				TRIGGER = TRUE;
			InvalidateRgn(hWnd, NULL, FALSE);
		}
		Sleep(10);
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	HDC hdc, memdc;
	HBITMAP hBitmap;
	HBITMAP hMainBitmap;
	HBITMAP selectArrowBitmap;
	PAINTSTRUCT ps;
	HPEN hPen, oldPen;
	HBRUSH hBrush, oldBrush;
	HFONT hFont, oldFont;
	TCHAR blockNumStr[100];
	TCHAR ballStr[100];
	TCHAR stageStr[100];
	TCHAR highScoreStr[100];
	TCHAR scoreStr[100];
	TCHAR endScore[100];
	TCHAR endStage[100];
	static HANDLE hThread1;
	static RECT rectView;
	static int score = 0;
	static int groundBallNum;
	static int selectMain = 1;
	static int selectEnd = 1;
	static int HIGHSCORE;
	static double oldX, oldY;
	static double dx, dy;
	static double mx, my;
	static BOOL collisionCheck;
	static BOOL ballReadyChk = FALSE;
	static BOOL groundBallEmptyChk = TRUE;

	switch (iMsg) {
	case WM_CREATE:
		hWnd = hwnd;
		hThread1 = (HANDLE)_beginthreadex(
			NULL, 
			0, 
			(unsigned int(__stdcall*)(void*))ThreadProc1,
			NULL,
			0,
			NULL);
		GetClientRect(hwnd, &rectView);
		for (int i = 0; i < ROW; i++) {
			for (int j = 0; j < COL; j++) {
				/*
				block[i][j].rect.left = rectView.right / COL * j + SPACE;
				block[i][j].rect.top = 50 * i + SPACE;
				block[i][j].rect.right = (rectView.right / COL)*(j + 1) - SPACE;
				block[i][j].rect.bottom = 50 * i + 50 - SPACE;
				*/
				block[i][j].rect.left = 100 * j + SPACE;
				block[i][j].rect.top = 50 * i + SPACE;
				block[i][j].rect.right = (100)*(j + 1) - SPACE;
				block[i][j].rect.bottom = 50 * i + 50 - SPACE;
				block[i][j].count = STAGE;
			}
		}
		for (int i = 0; i < BALL_MAX_SIZE; i++) {
			ball[i].x = rand() % 570 + 20;
			ball[i].y = GROUND - BSIZE;
			ball[i].VISIBLE = FALSE;
			ball[i].MOVABLE = FALSE;
			ball[i].START = FALSE;
		}
		MOUSECHECK = FALSE;
		AddBallCount();
		RndBlockCreate();
		dRgb.r = 255 - 246;
		dRgb.g = 65 - 160;
		dRgb.b = 82 - 130;
		HIGHSCORE = score;
		break;
	case WM_PAINT:
		GetClientRect(hwnd, &rectView);
		if (GAMEMAINCHECK) {
			//MAIN SCREEN DRAW START
			hMainBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MAIN_BACKGROUND));
			selectArrowBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SELECT_ARROW));
			hdc = GetDC(hwnd);
			memdc = CreateCompatibleDC(hdc);
			SelectObject(memdc, hMainBitmap);
			BitBlt(hdc, 0, 0, rectView.right, rectView.bottom, memdc, 0, 0, SRCCOPY);
			SelectObject(memdc, selectArrowBitmap);
			switch (selectMain) {
			case 1:
				BitBlt(hdc, 50, 270, 125, 92, memdc, 0, 0, SRCCOPY);
				break;
			case 2:
				BitBlt(hdc, 50, 390, 125, 92, memdc, 0, 0, SRCCOPY);
				break;
			}
			DeleteObject(selectArrowBitmap);
			DeleteObject(hMainBitmap);
			DeleteDC(memdc);
			ReleaseDC(hwnd, hdc);
			//MAIN SCREEN DRAW END
		}
		else if (GAMEENDCHECK) {
			//END SCREEN DRAW START
			hMainBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_END_BACKGROUND));
			selectArrowBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SELECT_ARROW));
			hdc = GetDC(hwnd);
			memdc = CreateCompatibleDC(hdc);
			SelectObject(memdc, hMainBitmap);
			//STRING DRAW START
			SetBkMode(memdc, TRANSPARENT);
			hFont = CreateFont(28, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "consolas");
			SelectObject(memdc, hFont);
			wsprintf(highScoreStr, _T("HIGH SCORE: %d"), HIGHSCORE);
			TextOut(memdc, 220, 160, highScoreStr, _tcslen(highScoreStr));
			wsprintf(endScore, _T("SCORE: %d"), score);
			TextOut(memdc, 220, 195, endScore, _tcslen(endScore));
			wsprintf(endStage, _T("STAGE: %d"), STAGE);
			TextOut(memdc, 220, 230, endStage, _tcslen(endStage));
			DeleteObject(hFont);
			//STRING DRAW END
			BitBlt(hdc, 0, 0, rectView.right, rectView.bottom, memdc, 0, 0, SRCCOPY);
			SelectObject(memdc, selectArrowBitmap);
			switch (selectEnd) {
			case 1:
				BitBlt(hdc, 50, 270, 125, 92, memdc, 0, 0, SRCCOPY);
				break;
			case 2:
				BitBlt(hdc, 50, 390, 125, 92, memdc, 0, 0, SRCCOPY);
				break;
			}
			DeleteObject(selectArrowBitmap);
			DeleteObject(hMainBitmap);
			DeleteDC(memdc);
			ReleaseDC(hwnd, hdc);
			//END SCREEN DRAW END
		}
		else {
			//DOUBLE BUFFERING DRAW START
			hdc = BeginPaint(hwnd, &ps);
			memdc = CreateCompatibleDC(hdc);
			hBitmap = CreateCompatibleBitmap(hdc, rectView.right, rectView.bottom);
			SelectObject(memdc, hBitmap);
			hBrush = CreateSolidBrush(RGB(230, 230, 230));
			FillRect(memdc, &rectView, hBrush);
			DeleteObject(hBrush);
			SetBkMode(memdc, TRANSPARENT);
			//BALL SHADOW DRAW START
			if (ballReadyChk) {
				hBrush = CreateSolidBrush(RGB(194, 206, 219));
				hPen = CreatePen(PS_SOLID, 1, RGB(194, 206, 219));
				oldBrush = (HBRUSH)SelectObject(memdc, hBrush);
				oldPen = (HPEN)SelectObject(memdc, hPen);
				Ellipse(memdc, (int)ball[0].x - BSIZE + 5, (int)ball[0].y - BSIZE + 5, (int)ball[0].x + BSIZE + 5, (int)ball[0].y + BSIZE + 5);
				SelectObject(memdc, oldBrush);
				SelectObject(memdc, oldPen);
				DeleteObject(hBrush);
				DeleteObject(hPen);
			}
			//BALL SHADOW DRAW END
			//BLOCK SHADOW DRAW START
			for (int i = 0; i < ROW; i++) {
				for (int j = 0; j < COL; j++) {
					if (block[i][j].count > 0) {
						hPen = CreatePen(PS_SOLID, 1, RGB(222, 196, 198));
						hBrush = CreateSolidBrush(RGB(222, 196, 198));
						oldBrush = (HBRUSH)SelectObject(memdc, hBrush);
						oldPen = (HPEN)SelectObject(memdc, hPen);
						Rectangle(memdc, block[i][j].rect.left + 5, block[i][j].rect.top + 5, block[i][j].rect.right + 5, block[i][j].rect.bottom + 5);
						SelectObject(memdc, oldBrush);
						SelectObject(memdc, oldPen);
						DeleteObject(hBrush);
						DeleteObject(hPen);
					}
				}
			}
			//BLOCK SHADOW DRAW END
			//STRING RECTANGLE DRAW START	
			hBrush = CreateSolidBrush(RGB(230, 230, 230));
			hPen = CreatePen(PS_SOLID, 5, RGB(100, 100, 100));
			oldBrush = (HBRUSH)SelectObject(memdc, hBrush);
			oldPen = (HPEN)SelectObject(memdc, hPen);
			Rectangle(memdc, rectView.left + 5, GROUND + 34, rectView.right - 10, GROUND + 65);
			SelectObject(memdc, oldBrush);
			SelectObject(memdc, oldPen);
			DeleteObject(hBrush);
			DeleteObject(hPen);
			//STRING RECTANGLE DRAW END
			//BALL EFFECT DRAW START
			if (ballReadyChk) {
				SelectObject(memdc, (HBRUSH)GetStockObject(NULL_BRUSH));
				hPen = CreatePen(PS_DOT, 1, RGB(70, 130, 210));
				oldPen = (HPEN)SelectObject(memdc, hPen);
				Ellipse(memdc, (int)ball[0].x - BSIZE + rotX, (int)ball[0].y - BSIZE+rotX, (int)ball[0].x + BSIZE - rotX, (int)ball[0].y + BSIZE-rotX);
				SelectObject(memdc, oldPen);
				DeleteObject(hPen);
			}
			//BALL EFFECT DRAW END
			//SCROE,STAGE,HIGH SCORE STRING DRAW START
			hFont = CreateFont(26, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "consolas");
			oldFont = (HFONT)SelectObject(memdc, hFont);
			wsprintf(stageStr, _T("STAGE:%d"), STAGE);
			TextOut(memdc, 20, GROUND + 35, stageStr, _tcslen(stageStr));
			wsprintf(scoreStr, _T("SCORE:%d"), score);
			TextOut(memdc, 160, GROUND + 35, scoreStr, _tcslen(scoreStr));
			wsprintf(highScoreStr, _T("HIGH SCORE:%d"), HIGHSCORE);
			TextOut(memdc, 340, GROUND + 35, highScoreStr, _tcslen(highScoreStr));
			SelectObject(memdc, oldFont);
			DeleteObject(hFont);
			//SCROE,STAGE,HIGH SCORE STRING DRAW END
			//BLOCK DRAW START
			for (int i = 0; i < ROW; i++) {
				for (int j = 0; j < COL; j++) {
					if (block[i][j].count > 0) {
						hBrush = CreateSolidBrush(
							RGB(
								block[i][j].RGB.r,
								block[i][j].RGB.g,
								block[i][j].RGB.b
							));
						hPen = CreatePen(
							PS_SOLID,
							1,
							RGB(
								block[i][j].RGB.r,
								block[i][j].RGB.g,
								block[i][j].RGB.b
							));
						hFont = CreateFont(20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "consolas");
						oldFont = (HFONT)SelectObject(memdc, hFont);
						SelectObject(memdc, hPen);
						SelectObject(memdc, hBrush);
						SetTextColor(memdc, RGB(255, 255, 255));
						Rectangle(memdc, block[i][j].rect.left, block[i][j].rect.top, block[i][j].rect.right, block[i][j].rect.bottom);
						wsprintf(blockNumStr, _T("%d"), block[i][j].count);
						if (block[i][j].count < 10)
							TextOut(memdc, (100 * j) + 45, (50 * i) + 15, blockNumStr, _tcslen(blockNumStr));
						else if (block[i][j].count < 100)
							TextOut(memdc, (100 * j) + 41, (50 * i) + 15, blockNumStr, _tcslen(blockNumStr));
						else if (block[i][j].count < 1000)
							TextOut(memdc, (100 * j) + 37, (50 * i) + 15, blockNumStr, _tcslen(blockNumStr));
						else if (block[i][j].count < 10000)
							TextOut(memdc, (100 * j) + 33, (50 * i) + 15, blockNumStr, _tcslen(blockNumStr));
						SetTextColor(memdc, RGB(0, 0, 0));
						SelectObject(memdc, oldFont);
						DeleteObject(hFont);
						DeleteObject(hBrush);
						DeleteObject(hPen);
					}
				}
			}
			//BLOCK DRAW END
			//BALL DRAW START
			for (int i = 0; i < STAGE; i++) {
				if (ball[i].VISIBLE) {
					hBrush = CreateSolidBrush(RGB(91, 167, 244));
					hPen = CreatePen(PS_SOLID, 1, RGB(91, 167, 244));
					SelectObject(memdc, hBrush);
					SelectObject(memdc, hPen);
					Ellipse(memdc, (int)ball[i].x - BSIZE, (int)ball[i].y - BSIZE, (int)ball[i].x + BSIZE, (int)ball[i].y + BSIZE);
					SelectObject(memdc, (HPEN)GetStockObject(BLACK_PEN));
					DeleteObject(hPen);
					DeleteObject(hBrush);
				}
			}
			//BALL DRAW END
			//BLOCK COUNT(BOTTOM) STRING DRAW START
			if (ballReadyChk) {
				hFont = CreateFont(25, 0, 0, 0, 1000, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "consolas");
				SelectObject(memdc, hFont);
				SetTextColor(memdc, RGB(91, 167, 244));
				wsprintf(ballStr, _T("x%d"), STAGE);
				TextOut(memdc, ball[0].x - BSIZE - 3, GROUND + 8, ballStr, _tcslen(ballStr));
				DeleteObject(hFont);
			}
			//BLOCK COUNT(BOTTOM) STRING DRAW END
			//LINE DRAW START
			for (int i = 0; i < 10; i++) {
				SelectObject(memdc, (HPEN)GetStockObject(BLACK_PEN));
				MoveToEx(memdc, 0, GROUND + i, NULL);
				LineTo(memdc, rectView.right, GROUND + i);
			}
			//LINE DRAW END
			BitBlt(hdc, 0, 0, rectView.right, rectView.bottom, memdc, 0, 0, SRCCOPY);
			DeleteObject(hBitmap);
			DeleteDC(memdc);
			EndPaint(hwnd, &ps);
			//DOUBLE BUFFERING DRAW END
		}
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_RIGHT://HAVE TO DELETE THIS CODE
			//for (int i = 0; i < STAGE; i++) {
			//	ball[i].x += 10;
			//}
			//InvalidateRgn(hwnd, NULL, FALSE);
			break;
		case VK_LEFT://HAVE TO DELETE THIS CODE
			//for (int i = 0; i < STAGE; i++) {
			//	ball[i].x -= 10;
			//}
			//InvalidateRgn(hwnd, NULL, FALSE);
			break;
		case VK_RETURN:
			if (GAMEMAINCHECK) {
				if (selectMain == 1) {
					GAMEMAINCHECK = FALSE;
					ballReadyChk = TRUE;
					InvalidateRgn(hwnd, NULL, FALSE);
				}
				else if (selectMain == 2) {
					PostQuitMessage(0);
				}
			}
			if (GAMEENDCHECK) {
				if (selectEnd == 1) {
					GAMEENDCHECK = FALSE;
					STAGE = 0;
					score = 0;
					ballReadyChk = TRUE;
					for (int i = 0; i < ROW; i++) {
						for (int j = 0; j < COL; j++) {
							block[i][j].count = 0;
						}
					}
					for (int i = 0; i < BALL_MAX_SIZE; i++) {
						ball[i].x = rand() % 570 + 20;
						ball[i].y = GROUND - BSIZE;
						ball[i].VISIBLE = FALSE;
						ball[i].MOVABLE = FALSE;
						ball[i].START = FALSE;
					}
					MOUSECHECK = FALSE;
					AddBallCount();
					RndBlockCreate();
					InvalidateRgn(hwnd, NULL, FALSE);
				}
				else if (selectEnd == 2) {
					PostQuitMessage(0);
				}
			}
			break;
		case VK_UP:
			if (GAMEMAINCHECK) {
				if (selectMain == 1)
					selectMain++;
				else
					selectMain--;
				InvalidateRgn(hwnd, NULL, FALSE);
			}
			if (GAMEENDCHECK) {
				if (selectEnd == 1)
					selectEnd++;
				else
					selectEnd--;
				InvalidateRgn(hwnd, NULL, FALSE);
			}
			break;
		case VK_DOWN:
			if (GAMEMAINCHECK) {
				if (selectMain == 1)
					selectMain++;
				else
					selectMain--;
				InvalidateRgn(hwnd, NULL, FALSE);
			}
			if (GAMEENDCHECK) {
				if (selectEnd == 1)
					selectEnd++;
				else
					selectEnd--;
				InvalidateRgn(hwnd, NULL, FALSE);
			}
			break;
		case VK_ESCAPE:
			if (!GAMEMAINCHECK && !GAMEENDCHECK) {
				int msg;
				KillTimer(hwnd, 1);
				msg = MessageBox(hwnd, _T("모든 진행상황이 초기화됩니다.\n메인 화면으로 돌아가시겠습니까?"), _T("돌아가기"), MB_YESNO | MB_ICONQUESTION);
				if (msg == IDYES) {
					GAMEMAINCHECK = TRUE;
					STAGE = 0;
					score = 0;
					ballReadyChk = TRUE;
					for (int i = 0; i < ROW; i++) {
						for (int j = 0; j < COL; j++) {
							block[i][j].count = 0;
						}
					}
					for (int i = 0; i < BALL_MAX_SIZE; i++) {
						ball[i].x = rand() % 570 + 20;
						ball[i].y = GROUND - BSIZE;
						ball[i].VISIBLE = FALSE;
						ball[i].MOVABLE = FALSE;
						ball[i].START = FALSE;
					}					
					MOUSECHECK = FALSE;
					AddBallCount();
					RndBlockCreate();
					InvalidateRgn(hwnd, NULL, FALSE);
				}
				else {
					SetTimer(hwnd, 1, 10, NULL);
				}
			}
			break;
		case VK_SPACE://HAVE TO DELETE THIS CODE
			/*nextStage();
			AddBallCount();
			for (int i = 0; i < COL; i++) {
				block[9][i].count = 0;
			}*/
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		if (!GAMEMAINCHECK && !GAMEENDCHECK && ballReadyChk) {
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
			MOUSECHECK = TRUE;
			if (my > GROUND - 60) {
				my = GROUND - 60 - SPACE;
			}			
			if (ballReadyChk) {				
				hdc = GetDC(hwnd);
				hBitmap = CreateCompatibleBitmap(hdc, rectView.right, rectView.bottom);
				memdc = CreateCompatibleDC(hdc);
				SelectObject(memdc, hBitmap);
				FillRect(memdc, &rectView, (HBRUSH)GetSysColorBrush(COLOR_WINDOW));
				BitBlt(memdc, 0, 0, rectView.right, rectView.bottom, hdc, 0, 0, SRCCOPY);
				//LINE DRAW START
				MoveToEx(memdc, ball[0].x, ball[0].y, NULL);
				LineTo(memdc, mx, my);				
				//LINE DRAW END
				//BALL[0] DRAW START
				hBrush = CreateSolidBrush(RGB(91, 167, 244));
				hPen = CreatePen(PS_SOLID, 1, RGB(91, 167, 244));
				SelectObject(memdc, hBrush);
				SelectObject(memdc, hPen);
				Ellipse(memdc, ball[0].x - BSIZE, ball[0].y - BSIZE, ball[0].x + BSIZE, ball[0].y + BSIZE);
				//BALL[0] DRAW END
				BitBlt(hdc, 0, 0, rectView.right, rectView.bottom, memdc, 0, 0, SRCCOPY);
				DeleteObject(hBrush);
				DeleteObject(hPen);
				DeleteDC(memdc);
				DeleteObject(hBitmap);
				ReleaseDC(hwnd, hdc);
			}
			oldX = mx;
			oldY = my;
		}
		break;
	case WM_MOUSEMOVE:
		if (!GAMEMAINCHECK && !GAMEENDCHECK) {
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
			if (my > GROUND - 60) {
				my = GROUND - 60 - SPACE;
			}
			if (MOUSECHECK) {
				if (ballReadyChk) {
					hdc = GetDC(hwnd);
					hBitmap = CreateCompatibleBitmap(hdc, rectView.right, rectView.bottom);
					memdc = CreateCompatibleDC(hdc);
					SetBkMode(memdc, TRANSPARENT);
					SelectObject(memdc, hBitmap);
					BitBlt(memdc, 0, 0, rectView.right, rectView.bottom, hdc, 0, 0, SRCCOPY);
					//BLOCK SHADOW DRAW START
					for (int i = 0; i < ROW; i++) {
						for (int j = 0; j < COL; j++) {
							if (block[i][j].count > 0) {
								hPen = CreatePen(PS_SOLID, 1, RGB(222, 196, 198));
								hBrush = CreateSolidBrush(RGB(222, 196, 198));
								oldBrush = (HBRUSH)SelectObject(memdc, hBrush);
								oldPen = (HPEN)SelectObject(memdc, hPen);
								Rectangle(memdc, block[i][j].rect.left + 5, block[i][j].rect.top + 5, block[i][j].rect.right + 5, block[i][j].rect.bottom + 5);
								SelectObject(memdc, oldBrush);
								SelectObject(memdc, oldPen);
								DeleteObject(hBrush);
								DeleteObject(hPen);
							}
						}
					}
					//BLOCK SHADOW DRAW END
					//BLOCK DRAW START
					for (int i = 0; i < ROW; i++) {
						for (int j = 0; j < COL; j++) {
							if (block[i][j].count > 0) {
								hBrush = CreateSolidBrush(
									RGB(
										block[i][j].RGB.r,
										block[i][j].RGB.g,
										block[i][j].RGB.b
									));
								hPen = CreatePen(
									PS_SOLID,
									1,
									RGB(
										block[i][j].RGB.r,
										block[i][j].RGB.g,
										block[i][j].RGB.b
									));
								hFont = CreateFont(20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "consolas");
								oldFont = (HFONT)SelectObject(memdc, hFont);
								SelectObject(memdc, hPen);
								SelectObject(memdc, hBrush);
								SetTextColor(memdc, RGB(255, 255, 255));
								Rectangle(memdc, block[i][j].rect.left, block[i][j].rect.top, block[i][j].rect.right, block[i][j].rect.bottom);
								wsprintf(blockNumStr, _T("%d"), block[i][j].count);
								if (block[i][j].count < 10)
									TextOut(memdc, (100 * j) + 45, (50 * i) + 15, blockNumStr, _tcslen(blockNumStr));
								else if (block[i][j].count < 100)
									TextOut(memdc, (100 * j) + 41, (50 * i) + 15, blockNumStr, _tcslen(blockNumStr));
								else if (block[i][j].count < 1000)
									TextOut(memdc, (100 * j) + 37, (50 * i) + 15, blockNumStr, _tcslen(blockNumStr));
								else if (block[i][j].count < 10000)
									TextOut(memdc, (100 * j) + 33, (50 * i) + 15, blockNumStr, _tcslen(blockNumStr));
								SetTextColor(memdc, RGB(0, 0, 0));
								SelectObject(memdc, oldFont);
								DeleteObject(hFont);
								DeleteObject(hBrush);
								DeleteObject(hPen);
							}
						}
					}
					//BLOCK DRAW END
					BitBlt(hdc, 0, 0, rectView.right, rectView.bottom, memdc, 0, 0, SRCCOPY);					
					DeleteDC(memdc);
					DeleteObject(hBitmap);
					ReleaseDC(hwnd, hdc);
				}
				//LINE DRAW START
				if (ballReadyChk) {					
					hdc = GetDC(hwnd);
					hBitmap = CreateCompatibleBitmap(hdc, rectView.right, rectView.bottom);
					memdc = CreateCompatibleDC(hdc);
					SelectObject(memdc, hBitmap);
					FillRect(memdc, &rectView, (HBRUSH)GetSysColorBrush(COLOR_WINDOW));
					BitBlt(memdc, 0, 0, rectView.right, rectView.bottom, hdc, 0, 0, SRCCOPY);
					hPen = CreatePen(PS_SOLID, 1, RGB(230, 230, 230));
					//LINE DRAW START
					SetROP2(memdc, R2_XORPEN);
					SelectObject(memdc, hPen);
					MoveToEx(memdc, ball[0].x, ball[0].y, NULL);
					LineTo(memdc, mx, my);
					MoveToEx(memdc, ball[0].x, ball[0].y, NULL);
					LineTo(memdc, oldX, oldY);
					//LINE DRAW END
					oldX = mx; oldY = my;
					BitBlt(hdc, 0, 0, rectView.right, rectView.bottom, memdc, 0, 0, SRCCOPY);
					DeleteObject(hPen);
					DeleteDC(memdc);
					DeleteObject(hBitmap);
					ReleaseDC(hwnd, hdc);					
				}				
				//LINE DRAW END
				//BLOCK[0] DRAW START
				if (ballReadyChk) {					
					hdc = GetDC(hwnd);
					hBitmap = CreateCompatibleBitmap(hdc, rectView.right, rectView.bottom);
					memdc = CreateCompatibleDC(hdc);
					SelectObject(memdc, hBitmap);
					FillRect(memdc, &rectView, (HBRUSH)GetSysColorBrush(COLOR_WINDOW));
					BitBlt(memdc, 0, 0, rectView.right, rectView.bottom, hdc, 0, 0, SRCCOPY);
					hBrush = CreateSolidBrush(RGB(91, 167, 244));
					hPen = CreatePen(PS_SOLID, 1, RGB(91, 167, 244));
					SelectObject(memdc, hBrush);
					SelectObject(memdc, hPen);
					//BALL[0] DRAW STRT
					Ellipse(memdc, ball[0].x - BSIZE, ball[0].y - BSIZE, ball[0].x + BSIZE, ball[0].y + BSIZE);
					//BALL[0] DRAW END
					BitBlt(hdc, 0, 0, rectView.right, rectView.bottom, memdc, 0, 0, SRCCOPY);
					DeleteObject(hBrush);
					DeleteObject(hPen);
					DeleteDC(memdc);
					DeleteObject(hBitmap);
					ReleaseDC(hwnd, hdc);
				}
				//BLOCK[0] DRAW END
			}
		}
		break;
	case WM_LBUTTONUP:
		if (!GAMEMAINCHECK && !GAMEENDCHECK && MOUSECHECK) {
			for (int i = 0; i < STAGE; i++) {
				if (ball[i].VISIBLE && ball[i].MOVABLE) {
					dx = mx - ball[i].x;
					dy = my - ball[i].y;
					ball[i].degree = atan2(dy, dx);
					ball[i].xDist = cos(ball[i].degree)*SPEED;
					ball[i].yDist = sin(ball[i].degree)*SPEED;
					ball[i].MOVABLE = FALSE;
				}
			}
			SetTimer(hwnd, 1, 10, NULL);
			MOUSECHECK = FALSE;
		}
		break;
	case WM_TIMER:
		//WALL COLLISION CALIBRATION START
		for (int i = 0; i < STAGE; i++) {
			if (ball[i].x - BSIZE <= rectView.left)
				ball[i].x = rectView.left + BSIZE + 1;
			else if (ball[i].x + BSIZE >= rectView.right)
				ball[i].x = rectView.right - BSIZE - 1;
			if (ball[i].y - BSIZE <= rectView.top)
				ball[i].y = rectView.top + BSIZE + 1;
			else if (ball[i].y + BSIZE >= rectView.bottom)
				ball[i].y = rectView.bottom - BSIZE - 1;
		}
		//WALL COLLISION CALIBRATION END
		//START CHECK START
		for (int i = 0; i < COL; i++) {
			if (block[8][i].count != 0) {				
				for (int j = 0; j < STAGE; j++) {
					if (ball[j].x > block[8][i].rect.left && ball[j].x<block[8][i].rect.right && ball[j].y>block[8][i].rect.bottom) {
						if (j == 0 && ball[j].START) {
							ball[j].START = FALSE;
						}
						else {
							if (length(ball[j].x, ball[j].y, ball[j - 1].x, ball[j - 1].y) >= 5 && ball[j].START) {
								ball[j].START = FALSE;
							}
						}
					}
				}
			}
		}
		for (int i = 0; i < STAGE; i++) {
			if (i == 0 && ball[i].START) {
				ball[i].START = FALSE;
			}
			else {
				if (length(ball[i].x, ball[i].y, ball[i - 1].x, ball[i - 1].y) >= BALL_SPACE && ball[i].START) {
					ball[i].START = FALSE;
				}
			}
		}
		//START CHECK END
		for (int i = 0; i <= STAGE; i++) {
			if (ball[i].VISIBLE && ball[i].START == FALSE) {
				ball[i].x += ball[i].xDist;
				ball[i].y += ball[i].yDist;
			}
			//WALL COLIISON CHECK START
			if (ball[i].x - BSIZE <= rectView.left)
				ball[i].xDist *= -1;
			else if (ball[i].x + BSIZE >= rectView.right)
				ball[i].xDist *= -1;
			if (ball[i].y - BSIZE <= rectView.top)
				ball[i].yDist *= -1;
			else if (ball[i].y + BSIZE >= GROUND + 1) {
				if (groundBallEmptyChk) {
					groundBallNum = i;
					groundBallEmptyChk = FALSE;
				}
				ball[i].VISIBLE = FALSE;
			}
			//WALL COLIISON CHECK END
		}
		//BLOCK COLIISON CHECK START
		for (int i = 0; i < STAGE; i++) {
			for (int j = 0; j < ROW; j++) {
				for (int k = 0; k < COL; k++) {
					collisionCheck = TRUE;
					if (block[j][k].count <= 0)
						continue;
					else {
						dRgb.r = block[j][k].RGB.r - 246;
						dRgb.g = block[j][k].RGB.g - 160;
						dRgb.b = block[j][k].RGB.b - 130;
						//CIRCLE AND RECTANGLE COLIISION CHECK START
						if (collisionCheck) {							
							if ((block[j][k].rect.left - BSIZE <= ball[i].x + ball[i].xDist) && (block[j][k].rect.right + BSIZE >= ball[i].x + ball[i].xDist) &&
								(block[j][k].rect.top - BSIZE <= ball[i].y + ball[i].yDist) && (block[j][k].rect.bottom + BSIZE >= ball[i].y + ball[i].yDist)) {
								if (ball[i].x + ball[i].xDist <= block[j][k].rect.left && ball[i].y + ball[i].yDist >= block[j][k].rect.top && ball[i].y + ball[i].yDist <= block[j][k].rect.bottom) {
									ball[i].xDist *= -1;
									score++;
									block[j][k].count--;
									//BLOCK COLOR CHANGE START
									block[j][k].RGB.r -= (dRgb.r / (double)block[j][k].count);
									block[j][k].RGB.g -= (dRgb.g / (double)block[j][k].count);
									block[j][k].RGB.b -= (dRgb.b / (double)block[j][k].count);
									//BLOCK COLOR CHANGE END
									collisionCheck = FALSE;
								}
								else if (ball[i].x + ball[i].xDist >= block[j][k].rect.right && ball[i].y + ball[i].yDist >= block[j][k].rect.top && ball[i].y + ball[i].yDist <= block[j][k].rect.bottom) {
									ball[i].xDist *= -1;
									score++;
									block[j][k].count--;
									//BLOCK COLOR CHANGE START
									block[j][k].RGB.r -= (dRgb.r / (double)block[j][k].count);
									block[j][k].RGB.g -= (dRgb.g / (double)block[j][k].count);
									block[j][k].RGB.b -= (dRgb.b / (double)block[j][k].count);
									//BLOCK COLOR CHANGE END
									collisionCheck = FALSE;
								}
								if (ball[i].y + ball[i].yDist <= block[j][k].rect.top && ball[i].x + ball[i].xDist >= block[j][k].rect.left && ball[i].x + ball[i].xDist <= block[j][k].rect.right) {
									ball[i].yDist *= -1;
									score++;
									block[j][k].count--;
									//BLOCK COLOR CHANGE START
									block[j][k].RGB.r -= (dRgb.r / (double)block[j][k].count);
									block[j][k].RGB.g -= (dRgb.g / (double)block[j][k].count);
									block[j][k].RGB.b -= (dRgb.b / (double)block[j][k].count);
									//BLOCK COLOR CHANGE END
									collisionCheck = FALSE;
								}
								else if (ball[i].y + ball[i].yDist >= block[j][k].rect.bottom && ball[i].x + ball[i].xDist >= block[j][k].rect.left && ball[i].x + ball[i].xDist <= block[j][k].rect.right) {
									ball[i].yDist *= -1;
									score++;
									block[j][k].count--;
									//BLOCK COLOR CHANGE START
									block[j][k].RGB.r -= (dRgb.r / (double)block[j][k].count);
									block[j][k].RGB.g -= (dRgb.g / (double)block[j][k].count);
									block[j][k].RGB.b -= (dRgb.b / (double)block[j][k].count);
									//BLOCK COLOR CHANGE END
									collisionCheck = FALSE;
								}
							}
						}
						//CIRCLE AND RECTANGLE COLIISION CHECK END
						//ANGLE COLLISION CHECK START
						if (collisionCheck) {
							if (length(ball[i].x + ball[i].xDist, ball[i].y + ball[i].yDist, block[j][k].rect.left, block[j][k].rect.top) <= BSIZE) {
								if (ball[i].xDist > 0 && ball[i].yDist > 0) {// ↘
									ball[i].xDist *= -1;
									ball[i].yDist *= -1;
								}
								else if (ball[i].xDist > 0 && ball[i].yDist < 0) {// ↗
									ball[i].xDist *= -1;
								}
								else if (ball[i].xDist < 0 && ball[i].yDist>0) {// ↙
									ball[i].yDist *= -1;
								}
								score++;
								block[j][k].count--;
								//BLOCK COLOR CHANGE START
								block[j][k].RGB.r -= (dRgb.r / (double)block[j][k].count);
								block[j][k].RGB.g -= (dRgb.g / (double)block[j][k].count);
								block[j][k].RGB.b -= (dRgb.b / (double)block[j][k].count);
								//BLOCK COLOR CHANGE END
								collisionCheck = FALSE;
								//LEFT TOP ANGLE COLLISION CHECK
							}
							else if (length(ball[i].x + ball[i].xDist, ball[i].y + ball[i].yDist, block[j][k].rect.right, block[j][k].rect.top) <= BSIZE) {
								if (ball[i].xDist > 0 && ball[i].yDist > 0) { // ↘
									ball[i].yDist *= -1;
								}
								else if (ball[i].xDist < 0 && ball[i].yDist > 0) { // ↙
									ball[i].xDist *= -1;
									ball[i].yDist *= -1;
								}
								else if (ball[i].xDist < 0 && ball[i].yDist < 0) { // ↖
									ball[i].xDist *= -1;
								}
								score++;
								block[j][k].count--;
								//BLOCK COLOR CHANGE START
								block[j][k].RGB.r -= (dRgb.r / (double)block[j][k].count);
								block[j][k].RGB.g -= (dRgb.g / (double)block[j][k].count);
								block[j][k].RGB.b -= (dRgb.b / (double)block[j][k].count);
								//BLOCK COLOR CHANGE END
								collisionCheck = FALSE;
								//RIGHT TOP ANGLE COLLISION CHECK
							}
							else if (length(ball[i].x + ball[i].xDist, ball[i].y + ball[i].yDist, block[j][k].rect.left, block[j][k].rect.bottom) <= BSIZE) {
								if (ball[i].xDist > 0 && ball[i].yDist > 0) { // ↘
									ball[i].xDist *= -1;
								}
								else if (ball[i].xDist > 0 && ball[i].yDist < 0) { // ↗
									ball[i].xDist *= -1;
									ball[i].yDist *= -1;
								}
								else if (ball[i].xDist < 0 && ball[i].yDist < 0) { // ↖
									ball[i].yDist *= -1;
								}
								score++;
								block[j][k].count--;
								//BLOCK COLOR CHANGE START
								block[j][k].RGB.r -= (dRgb.r / (double)block[j][k].count);
								block[j][k].RGB.g -= (dRgb.g / (double)block[j][k].count);
								block[j][k].RGB.b -= (dRgb.b / (double)block[j][k].count);
								//BLOCK COLOR CHANGE END
								collisionCheck = FALSE;
								//LEFT BOTTOM ANGLE COLLISION CHECK
							}
							else if (length(ball[i].x + ball[i].xDist, ball[i].y + ball[i].yDist, block[j][k].rect.right, block[j][k].rect.top) <= BSIZE) {
								if (ball[i].xDist > 0 && ball[i].yDist < 0) { // ↗
									ball[i].yDist *= -1;
								}
								else if (ball[i].xDist < 0 && ball[i].yDist < 0) { // ↖
									ball[i].xDist *= -1;
									ball[i].yDist *= -1;
								}
								else if (ball[i].xDist < 0 && ball[i].yDist>0) { // ↙
									ball[i].xDist *= -1;
								}
								score++;
								block[j][k].count--;
								//BLOCK COLOR CHANGE START
								block[j][k].RGB.r -= (dRgb.r / (double)block[j][k].count);
								block[j][k].RGB.g -= (dRgb.g / (double)block[j][k].count);
								block[j][k].RGB.b -= (dRgb.b / (double)block[j][k].count);
								//BLOCK COLOR CHANGE END
								collisionCheck = FALSE;
								//RIGHT BOTTOM ANGLE COLLISION CHECK
							}
						}
						//ANGLE COLLISION CHECK END
						//BLOCK COLLISION CALIBRATION START
						if (ball[i].x + ball[i].xDist >= block[j][k].rect.left && ball[i].x + ball[i].xDist <= block[j][k].rect.right &&
							ball[i].y + ball[i].yDist >= block[j][k].rect.top && ball[i].y + ball[i].yDist <= block[j][k].rect.bottom) {
							if (ball[i].y >= block[j][k].rect.bottom)
								ball[i].yDist *= -1;
							if (ball[i].y <= block[j][k].rect.top)
								ball[i].yDist *= -1;
							if (ball[i].x <= block[j][k].rect.left)
								ball[i].xDist *= -1;
							if (ball[i].x >= block[j][k].rect.right)
								ball[i].xDist *= -1;
							score++;
							block[j][k].count--;
							//BLOCK COLOR CHANGE START
							block[j][k].RGB.r -= (dRgb.r / (double)block[j][k].count);
							block[j][k].RGB.g -= (dRgb.g / (double)block[j][k].count);
							block[j][k].RGB.b -= (dRgb.b / (double)block[j][k].count);
							//BLOCK COLOR CHANGE END
						}
						//BLOCK COLLISION CALIBRATION END
					}
				}
			}
		}
		//BLOCK COLIISON CHECK END
		//NEXT STAGE CHECK START
		ballReadyChk = TRUE;
		for (int i = 0; i < STAGE; i++) {
			if (ball[i].VISIBLE)
				ballReadyChk = FALSE;
		}
		if (ballReadyChk) {
			if (score > HIGHSCORE)//HIGHSCORE CHECK
				HIGHSCORE = score;
			KillTimer(hwnd, 1);
			for (int i = 0; i < STAGE; i++) {
				if (ball[groundBallNum].x + BSIZE >= rectView.right)
					ball[groundBallNum].x = rectView.right - BSIZE - 1;
				else if (ball[groundBallNum].x - BSIZE <= rectView.left)
					ball[groundBallNum].x = rectView.left + BSIZE + 1;
				ball[i].x = ball[groundBallNum].x;
				ball[i].y = GROUND - BSIZE;
				ball[i].VISIBLE = TRUE;
				ball[i].MOVABLE = TRUE;
				ball[i].START = TRUE;
			}
			groundBallEmptyChk = TRUE;
			AddBallCount();
			nextStage();
		}
		//NEST STAGE CHECK END
		//GAME END CHECK START
		if (ballReadyChk && isGameEnd()) {
			GAMEENDCHECK = TRUE;
			InvalidateRgn(hwnd, NULL, FALSE);
		}
		//GAME END CHECK END
		InvalidateRgn(hwnd, NULL, FALSE);
		break;
	case WM_DESTROY:
		CloseHandle(hThread1);
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}