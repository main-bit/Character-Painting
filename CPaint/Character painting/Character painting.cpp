// Character painting.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Character painting.h"
#include "GrayBitmap.h"
#include "Work.h"

#define MAX_LOADSTRING 100
ULONG_PTR g_gdiplusToken;

static Work* s_work = NULL;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	initGDIPlus();

	s_work = new Work(hInstance, nCmdShow);
	s_work->Run();
	delete s_work;

	ReleaseGDIPlus();
	return 0;
}

void initGDIPlus() {
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);
}
void ReleaseGDIPlus() {
	GdiplusShutdown(g_gdiplusToken);
}



LRESULT CALLBACK Work::WndProc(HWND _hwnd, UINT _message, WPARAM _wParam, LPARAM _lParam) {
	switch (_message) {
		case WM_DROPFILES: 		//窗口接收到文件
			s_work->AcceptFile(_wParam);
			break;

		case WM_CLOSE:			//当点击了“X"按钮
			s_work->SetExit(true);
			break;

		case WM_DESTROY:		//销毁窗口
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(_hwnd, _message, _wParam, _lParam);
		}
	return 0;
}