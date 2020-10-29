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
	m_WndClassEx.lpfnWndProc = static_cast<WNDPROC>(&Work::WndProc);	//���ڹ��̺���
	m_WndClassEx.cbClsExtra = 0;
	m_WndClassEx.cbWndExtra = 0;
	m_WndClassEx.hInstance = _hInstance;	//����ʵ��
	m_WndClassEx.hIcon = LoadIcon(_hInstance, NULL);	//����ͼ��
	m_WndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);		//IDC_CROSSΪʮ�ֹ��
	m_WndClassEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	m_WndClassEx.lpszMenuName = NULL;		//�˵�
	m_WndClassEx.lpszClassName = L"Work";
	m_WndClassEx.hIconSm = LoadIcon(_hInstance, NULL);


	//ע�ᴰ��
	RegisterWIN();
	//��������
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

void Work::InitWIN() {		//���ڳ�ʼ��
	//��ʾ����
	ShowWIN();
	//���´���
	UpdateWIN();

	GetClientRect(m_Hwnd, &m_WindowRc);	//��ȡ���ڳߴ�
}

void Work::InitTimer() {	//��ʱ����ʼ��
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
	UnregisterWIN();			//���ٴ���
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


bool Work::RegisterWIN() {		//ע�ᴰ��
	return RegisterClassExW(&m_WndClassEx);
}

void Work::CreateWIN() {		//��������
	m_Hwnd = CreateWindowEx(WS_EX_ACCEPTFILES, L"Work", L"Character Painting", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
		100, 70, 900, 600, NULL, NULL, *m_hInstance, NULL);
}

void Work::ShowWIN() {			//��ʾ����
	ShowWindow(m_Hwnd, CmdShow);
}

void Work::UpdateWIN() {		//���´���
	UpdateWindow(m_Hwnd);
}

void Work::UnregisterWIN() {	//���ٴ���
	UnregisterClass(L"Work", *m_hInstance);
}

void Work::Run() {
	Init();					//���г�ʼ��
	while (!m_isExit) {
		dwRet = MsgWaitForMultipleObjects(1, &phWait, FALSE, INFINITE, QS_ALLINPUT);
		switch (dwRet) {
			case WAIT_OBJECT_0: {
				//��ͼ
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

	//*****************����*******************
	if (m_BmpPlus && m_LastFile) {
		//DrawBitmap(hdc);			//����ͼƬ
		if (m_LastFile->m_FileType < File::Gif)
			DrawCharPaint(hdc);			//�����ַ���
		else if (m_LastFile->m_FileType == File::Gif)
			DrawCharPaintGIF(hdc);
	}

	//*****************����**********************
	if (m_isSave) {
		m_isSave = false;
		DWORD nThreadId = 0;
		CreateThread(NULL, 0, threadSave, (LPVOID)this, 0, &nThreadId);	//�����߳�
	}

	ReleaseDC(m_Hwnd, hdc);
}

//����ȡ����ԭͼ
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

	//���ڴ���DC��
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
	UINT Frame = _Gif->GetFrameCount(&pDimensionIDs[0]);		//��ȡ֡��
	//�ȵ��� Image::GetPropertyItemSize����ж��ٸ�ʱ��������item.
	UINT FrameDelayNums = _Gif->GetPropertyItemSize(PropertyTagFrameDelay);

	PropertyItem* lpPropertyItem = new PropertyItem[FrameDelayNums];
	//Ȼ�����Image::GetPropertyItem����õ�items.
	_Gif->GetPropertyItem(PropertyTagFrameDelay, FrameDelayNums, lpPropertyItem);

	long PlayFrameTime = ((long*)lpPropertyItem->value)[m_PlayFrame] * 10;		//��ȡ��һ֡�ļ��ʱ��

	wstring SaveFileName = _File->m_Prefix + _File->m_FileNameSuffix;

	CGifEncoder gifEncoder;
	gifEncoder.StartEncoder(SaveFileName);
	for (int i = 0; i < Frame; ++i) {
		//��֡����	Ȼ��ÿ֡�ϳ�GIF�ļ�
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
	int Count = DragQueryFile(hDrop, 0xffffffff, NULL, 0);	//��ȡ�ļ���
	for (int i = 0; i < Count; ++i) {
		//�����Ͻ����ڵ��ļ�
		DragQueryFile(hDrop, i, PathFileName, _MAX_PATH);
		File* file = new File(PathFileName);
		if (file->m_FileType == File::Other) {
			MessageBox(NULL, L"�ļ���ʽ����", L"��ʾ", MB_OK);
			delete file;
			return;
		}
		m_VectorFile.push_back(file);
	}

	m_LastFile = m_VectorFile.back();
	//���֮ǰ�Ѿ������ļ�	��ˢ����Ļ�ͷ���Դ
	if (m_BmpPlus) {
		//ˢ����Ļ
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
	//��ʼ����
	m_DrawTime = 0;
}

void Work::UpDataCharPaint(Bitmap* _grayBmp, vector<wstring>& _vectorCharPaint) {
	Bitmap_to_CharPaint(_grayBmp, _vectorCharPaint);
}

void Work::UpDataCharPaintGIF(Bitmap* _grayBmp, vector<wstring>& _vectorCharPaint) {
	UINT count = _grayBmp->GetFrameDimensionsCount();
	GUID *pDimensionIDs = (GUID*)new GUID[count];
	_grayBmp->GetFrameDimensionsList(pDimensionIDs, count);
	m_Frame = _grayBmp->GetFrameCount(&pDimensionIDs[0]);		//��ȡ֡��
	//�ȵ��� Image::GetPropertyItemSize����ж��ٸ�ʱ��������item.
	UINT FrameDelayNums = _grayBmp->GetPropertyItemSize(PropertyTagFrameDelay);

	PropertyItem* lpPropertyItem = new PropertyItem[FrameDelayNums];
	//Ȼ�����Image::GetPropertyItem����õ�items.
	_grayBmp->GetPropertyItem(PropertyTagFrameDelay, FrameDelayNums, lpPropertyItem);

	if (m_PlayFrame + 1 >= m_Frame)
		m_PlayFrame = 0;
	else
		++m_PlayFrame;

	//��ȡ��һ֡�ļ��ʱ��
	if (m_PlayFrame == 0)
		m_PlayFrameTime = ((long*)lpPropertyItem->value)[m_Frame - 1] * 10;			//��һ֡���ӳ�ʱ�����������һ��Ԫ��
	else 
		m_PlayFrameTime = ((long*)lpPropertyItem->value)[m_PlayFrame - 1] * 10;		//���һ֡�ӳ�ʱ���������һ��Ԫ��
	
	_grayBmp->SelectActiveFrame(pDimensionIDs, m_PlayFrame);
	UpDataCharPaint(_grayBmp, _vectorCharPaint);


	delete[] lpPropertyItem;
	delete[] pDimensionIDs;
}


void Bitmap_to_CharPaint(Bitmap* _Bmp, vector<wstring>& _vectorWstr) {
	_vectorWstr.clear();
	GrayBitmap* grayBitbmp = GrayBitmap::getInstance();
	grayBitbmp->GrayBmp(_Bmp);		//���ػҶȻ����ͼƬ

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
	//�����ļ�DC��
	for (int len = 0; len < PieceNumberY; ++len) {
		wstring& wstr = _vectorWstr.at(len);
		TextOut(_HDC, 0, len * FONT_HEIGHT, wstr.c_str(), wstr.length());
	}
	DeleteObject(hBmp);
}
