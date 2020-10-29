#include "stdafx.h"
#include "Work.h"
#include "GrayBitmap.h"
using namespace std;

Work::Work(HINSTANCE _hInstance, int _CmdShow) : m_isExit(false), m_isSave(false),
m_Hwnd(NULL), m_WindowRc({ 0 }), m_hInstance(&_hInstance), CmdShow(_CmdShow), 
m_HDC(NULL), m_ImageHDC(NULL), m_CharPaintHDC(NULL), m_BmpPlus(NULL), m_hBmp(NULL), m_hFont(NULL),
m_DrawTime(0), m_LastFile(NULL), m_Frame(0), m_PlayFrame(0), m_PlayFrameTime(0) {
	m_WndClassEx = { 0 };
	m_WndClassEx.cbSize = sizeof(WNDCLASSEX);
	m_WndClassEx.style = CS_GLOBALCLASS;
	m_WndClassEx.lpfnWndProc = static_cast<WNDPROC>(&Work::WndProc);	//窗口过程函数
	m_WndClassEx.cbClsExtra = 0;
	m_WndClassEx.cbWndExtra = 0;
	m_WndClassEx.hInstance = _hInstance;	//窗口实例
	m_WndClassEx.hIcon = LoadIcon(_hInstance, NULL);	//程序图标
	m_WndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);		//IDC_CROSS为十字光标
	m_WndClassEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	m_WndClassEx.lpszMenuName = NULL;		//菜单
	m_WndClassEx.lpszClassName = L"Work";
	m_WndClassEx.hIconSm = LoadIcon(_hInstance, NULL);


	//注册窗口
	RegisterWIN();
	//创建窗口
	CreateWIN();
}

Work::~Work() {
	Release();
}

void Work::Init() {
	InitWIN();
	InitTimer();
	InitFont();
}

void Work::InitWIN() {		//窗口初始化
	//显示窗口
	ShowWIN();
	//更新窗口
	UpdateWIN();

	GetClientRect(m_Hwnd, &m_WindowRc);	//获取窗口尺寸
}

void Work::InitTimer() {	//计时器初始化
	phWait = CreateWaitableTimer(NULL, FALSE, NULL);
	liDueTime.QuadPart = -1i64;
	SetWaitableTimer(phWait, &liDueTime, 10, NULL, NULL, 0);
	dwRet = 0;
}

void Work::InitFont() {
	m_hFont = CreateFont(10, 0, 0, 0, 3, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, L"Consolas");
}

void Work::Release() {
	ReleaseWIN();
	ReleaseHDC();
	ReleaseFont();
	ReleaseHBITMAP();
}

void Work::ReleaseWIN() {
	UnregisterWIN();			//销毁窗口
}

void Work::ReleaseHDC() {
	if (!m_ImageHDC)
		DeleteDC(m_ImageHDC);
}

void Work::ReleaseFont() {
	DeleteObject(m_hFont);
}

void Work::ReleaseHBITMAP() {
	if (m_hBmp) {
		DeleteObject(m_hBmp);
	}
}


bool Work::RegisterWIN() {		//注册窗口
	return RegisterClassExW(&m_WndClassEx);
}

void Work::CreateWIN() {		//创建窗口
	m_Hwnd = CreateWindowEx(WS_EX_ACCEPTFILES, L"Work", L"Character Painting", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
		100, 70, 900, 600, NULL, NULL, *m_hInstance, NULL);
}

void Work::ShowWIN() {			//显示窗口
	ShowWindow(m_Hwnd, CmdShow);
}

void Work::UpdateWIN() {		//更新窗口
	UpdateWindow(m_Hwnd);
}

void Work::UnregisterWIN() {	//销毁窗口
	UnregisterClass(L"Work", *m_hInstance);
}

void Work::Run() {
	Init();					//进行初始化
	while (!m_isExit) {
		dwRet = MsgWaitForMultipleObjects(1, &phWait, FALSE, INFINITE, QS_ALLINPUT);
		switch (dwRet) {
			case WAIT_OBJECT_0: {
				//绘图
				Draw();
			}
			break;
			case WAIT_OBJECT_0 + 1: {
				MSG msg;
				while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			break;
		}
	}
}

DWORD WINAPI threadSave(LPVOID _pParam) {
	Work* work = (Work*)_pParam;
	int Count = work->m_VectorFile.size();
	vector<wstring> CharPaint;
	File* file = NULL;
	for (int i = 0; i < Count; ++i) {
		file = work->m_VectorFile.at(i);
		wstring FileName = file->m_PathFileName;
		Bitmap* Thread_GrayBmp = new Bitmap(FileName.c_str());
		work->Save(file, work->m_CharPaintHDC, CharPaint, Thread_GrayBmp);
		delete Thread_GrayBmp;
	}
	return 100;
}

void Work::Draw() {
	HDC hdc = GetDC(m_Hwnd);

	//*****************绘制*******************
	if (m_BmpPlus && m_LastFile) {
		//DrawBitmap(hdc);			//绘制图片
		if (m_LastFile->m_FileType < File::Gif)
			DrawCharPaint(hdc);			//绘制字符画
		else if (m_LastFile->m_FileType == File::Gif)
			DrawCharPaintGIF(hdc);
	}

	//*****************保存**********************
	if (m_isSave) {
		m_isSave = false;
		DWORD nThreadId = 0;
		CreateThread(NULL, 0, threadSave, (LPVOID)this, 0, &nThreadId);	//开启线程
	}

	ReleaseDC(m_Hwnd, hdc);
}

//不采取绘制原图
void Work::DrawBitmap(HDC _HDC) {
	if(!m_ImageHDC)
		m_ImageHDC = CreateCompatibleDC(_HDC);
	if(!m_hBmp)
		m_BmpPlus->GetHBITMAP(Color(1, 0, 0, 0), &m_hBmp);
	SelectObject(m_ImageHDC, m_hBmp);
	BitBlt(_HDC, 0, 0, m_BmpPlus->GetWidth(), m_BmpPlus->GetHeight(), m_ImageHDC, 0, 0, SRCCOPY);
}

void Work::DrawCharPaint(HDC _HDC) {
	int Width = m_GrayBmpPlus->GetWidth();
	int Height = m_GrayBmpPlus->GetHeight();
	int PieceNumberX = Width / PIECE_WIDTH;
	int PieceNumberY = Height / PIECE_HEIGHT;

	//贴在窗口DC上
	SelectObject(_HDC, m_hFont);
	if (m_VectorCharPaint.size() == 0)	return;
	for (int len = 0; len < PieceNumberY; ++len) {
		wstring& wstr = m_VectorCharPaint.at(len);
		TextOut(_HDC, 0, len * FONT_HEIGHT, wstr.c_str(), wstr.length());
	}

}

void Work::DrawCharPaintGIF(HDC _HDC) {
	if (m_DrawTime == 0)
		m_DrawTime = GetTickCount();
	DWORD NowTime = GetTickCount();
	DWORD Sleep = NowTime - m_DrawTime;
	if (Sleep < m_PlayFrameTime)	return;
	m_DrawTime = NowTime;
	UpDataCharPaintGIF(m_GrayBmpPlus, m_VectorCharPaint);
	DrawCharPaint(_HDC);
}

//Save
void Work::Save(File* _File, HDC& _HDC, vector<wstring>& _vectorWstr, Bitmap* _Image) {
	if (_File->m_FileType < File::Gif) {
		wstring SaveFileName = _File->m_Prefix + _File->m_FileNameSuffix;
		SaveBMP(_HDC, _vectorWstr, _Image, SaveFileName);
	}
	else if (_File->m_FileType == File::Gif) {
		SaveGIF(_File, _HDC, _vectorWstr, _Image);
	}	
}

void Work::SaveBMP(HDC& _HDC, vector<wstring>& _vectorWstr, Bitmap* _Bmp, const wstring& _SaveFileName) {
	UpDataCharPaint(_Bmp, _vectorWstr);
	CharPaint_BitBlt_HDC(_vectorWstr, _HDC, m_hFont);
	int FontWidth = _vectorWstr.at(0).length() * FONT_WIDTH;
	int FontHeight = _vectorWstr.size() * FONT_HEIGHT;

	RECT rc = { 0, 0, FontWidth, FontHeight };
	WriteBmp(_SaveFileName.c_str(), _HDC, rc);

	DeleteDC(_HDC);
	_HDC = NULL;
}

void Work::SaveGIF(File* _File, HDC& _HDC, vector<wstring>& _vectorWstr, Bitmap* _Gif) {
	UINT count = _Gif->GetFrameDimensionsCount();
	GUID *pDimensionIDs = (GUID*)new GUID[count];
	_Gif->GetFrameDimensionsList(pDimensionIDs, count);
	UINT Frame = _Gif->GetFrameCount(&pDimensionIDs[0]);		//获取帧数
	//先调用 Image::GetPropertyItemSize获得有多少个时间间隔属性item.
	UINT FrameDelayNums = _Gif->GetPropertyItemSize(PropertyTagFrameDelay);

	PropertyItem* lpPropertyItem = new PropertyItem[FrameDelayNums];
	//然后调用Image::GetPropertyItem填充获得的items.
	_Gif->GetPropertyItem(PropertyTagFrameDelay, FrameDelayNums, lpPropertyItem);

	long PlayFrameTime = ((long*)lpPropertyItem->value)[m_PlayFrame] * 10;		//获取下一帧的间隔时间

	wstring SaveFileName = _File->m_Prefix + _File->m_FileNameSuffix;

	CGifEncoder gifEncoder;
	gifEncoder.StartEncoder(SaveFileName);
	for (int i = 0; i < Frame; ++i) {
		//逐帧保存	然后将每帧合成GIF文件
		_Gif->SelectActiveFrame(pDimensionIDs, i);
		vector<wstring> vectorWstr;
		UpDataCharPaint(_Gif, vectorWstr);
		CharPaint_BitBlt_HDC(vectorWstr, m_CharPaintHDC, m_hFont);
		int FontWidth = vectorWstr.at(0).length() * FONT_WIDTH;
		int FontHeight = vectorWstr.size() * FONT_HEIGHT;
		RECT rc = { 0,0, FontWidth, FontHeight };
		gifEncoder.SetFrameSize(rc.right, rc.bottom);


		Bitmap* pBmp = NULL;
		HDC_to_Bitmap(m_CharPaintHDC, rc, &pBmp);
		gifEncoder.SetDelayTime(((long*)lpPropertyItem->value)[i] * 10);
		gifEncoder.AddFrame(pBmp);
		delete pBmp;
	}
	gifEncoder.FinishEncoder();

	delete[] lpPropertyItem;
	delete[] pDimensionIDs;
}

void Work::AcceptFile(WPARAM _wParam) {
	while (m_VectorFile.size()) {
		File* file = m_VectorFile.at(0);
		delete file;
		m_VectorFile.erase(m_VectorFile.begin());
	}
	wchar_t PathFileName[_MAX_PATH];
	wsprintf(PathFileName, L"\0", _MAX_PATH);
	HDROP hDrop = (HDROP)_wParam;
	int Count = DragQueryFile(hDrop, 0xffffffff, NULL, 0);	//获取文件数
	for (int i = 0; i < Count; ++i) {
		//检索拖进窗口的文件
		DragQueryFile(hDrop, i, PathFileName, _MAX_PATH);
		File* file = new File(PathFileName);
		if (file->m_FileType == File::Other) {
			MessageBox(NULL, L"文件格式不对", L"提示", MB_OK);
			delete file;
			return;
		}
		m_VectorFile.push_back(file);
	}

	m_LastFile = m_VectorFile.back();
	//如果之前已经拖入文件	先刷新屏幕释放资源
	if (m_BmpPlus) {
		//刷新屏幕
		RECT rc = { 0, 0, 2 * m_BmpPlus->GetWidth(), m_BmpPlus->GetHeight() };
		InvalidateRect(m_Hwnd, &rc, true);
		delete m_BmpPlus;
		delete m_GrayBmpPlus;
		DeleteObject(m_hBmp);
		m_BmpPlus = NULL;
		m_GrayBmpPlus = NULL;
		m_hBmp = NULL;
	}
	if (!m_BmpPlus) {
		m_BmpPlus = new Bitmap(m_LastFile->m_PathFileName);
		m_GrayBmpPlus = new Bitmap(m_LastFile->m_PathFileName);
	}
	DragFinish(hDrop);

	if (m_LastFile->m_FileType < File::Gif)
		UpDataCharPaint(m_GrayBmpPlus, m_VectorCharPaint);
	else if(m_LastFile->m_FileType == File::Gif)
		UpDataCharPaintGIF(m_GrayBmpPlus, m_VectorCharPaint);

	m_isSave = true;
	//开始绘制
	m_DrawTime = 0;
}

void Work::UpDataCharPaint(Bitmap* _grayBmp, vector<wstring>& _vectorCharPaint) {
	Bitmap_to_CharPaint(_grayBmp, _vectorCharPaint);
}

void Work::UpDataCharPaintGIF(Bitmap* _grayBmp, vector<wstring>& _vectorCharPaint) {
	UINT count = _grayBmp->GetFrameDimensionsCount();
	GUID *pDimensionIDs = (GUID*)new GUID[count];
	_grayBmp->GetFrameDimensionsList(pDimensionIDs, count);
	m_Frame = _grayBmp->GetFrameCount(&pDimensionIDs[0]);		//获取帧数
	//先调用 Image::GetPropertyItemSize获得有多少个时间间隔属性item.
	UINT FrameDelayNums = _grayBmp->GetPropertyItemSize(PropertyTagFrameDelay);

	PropertyItem* lpPropertyItem = new PropertyItem[FrameDelayNums];
	//然后调用Image::GetPropertyItem填充获得的items.
	_grayBmp->GetPropertyItem(PropertyTagFrameDelay, FrameDelayNums, lpPropertyItem);

	if (m_PlayFrame + 1 >= m_Frame)
		m_PlayFrame = 0;
	else
		++m_PlayFrame;

	//获取下一帧的间隔时间
	if (m_PlayFrame == 0)
		m_PlayFrameTime = ((long*)lpPropertyItem->value)[m_Frame - 1] * 10;			//第一帧的延迟时间在数组最后一个元素
	else 
		m_PlayFrameTime = ((long*)lpPropertyItem->value)[m_PlayFrame - 1] * 10;		//最后一帧延迟时间在数组第一个元素
	
	_grayBmp->SelectActiveFrame(pDimensionIDs, m_PlayFrame);
	UpDataCharPaint(_grayBmp, _vectorCharPaint);


	delete[] lpPropertyItem;
	delete[] pDimensionIDs;
}


void Bitmap_to_CharPaint(Bitmap* _Bmp, vector<wstring>& _vectorWstr) {
	_vectorWstr.clear();
	GrayBitmap* grayBitbmp = GrayBitmap::getInstance();
	grayBitbmp->GrayBmp(_Bmp);		//返回灰度化后的图片

	int Width = _Bmp->GetWidth();
	int Height = _Bmp->GetHeight();
	int PieceNumberX = Width / PIECE_WIDTH;
	int PieceNumberY = Height / PIECE_HEIGHT;

	for (int No_Y = 0; No_Y < PieceNumberY; ++No_Y) {
		wstring wstr;
		for (int No_X = 0; No_X < PieceNumberX; ++No_X) {
			UINT colorRGB = grayBitbmp->SearchGrayBitmap(_Bmp, No_X, No_Y);
			wstr += grayBitbmp->GrayPixelConversionChar(colorRGB);
		}
		_vectorWstr.push_back(wstr);
	}
}

void CharPaint_BitBlt_HDC(vector<wstring>& _vectorWstr, HDC& _HDC, HFONT& _hFont) {
	if (_HDC) {
		DeleteDC(_HDC);
		_HDC = NULL;
	}
	int FontWidth = _vectorWstr.at(0).length() * FONT_WIDTH;
	int FontHeight = _vectorWstr.size() * FONT_HEIGHT;

	int PieceNumberY = FontHeight / PIECE_HEIGHT;
	_HDC = CreateCompatibleDC(_HDC);
	HBITMAP hBmp = CreateCompatibleBitmap(_HDC, FontWidth, FontHeight);
	SelectObject(_HDC, _hFont);
	SelectObject(_HDC, hBmp);
	//贴在文件DC上
	for (int len = 0; len < PieceNumberY; ++len) {
		wstring& wstr = _vectorWstr.at(len);
		TextOut(_HDC, 0, len * FONT_HEIGHT, wstr.c_str(), wstr.length());
	}
	DeleteObject(hBmp);
}
