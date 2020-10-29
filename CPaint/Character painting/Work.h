#pragma once
#ifndef Work_h
#define Work_h
#include <Windows.h>
#include "GrayBitmap.h"
#include "HandleFile.h"

class Work {
private:
	//��������
	HWND m_Hwnd;
	RECT m_WindowRc;
	WNDCLASSEX m_WndClassEx;
	HINSTANCE* m_hInstance;
	int CmdShow;
private:
	HANDLE phWait;			//����һ����ʱ��
	LARGE_INTEGER liDueTime;
	DWORD dwRet;
	bool m_isExit;				//�Ƿ��˳�
	bool m_isSave;				//�Ƿ���Ҫ����
private:
	//ͼƬ���
	wchar_t m_FileName[_MAX_PATH];	//�ļ���
	HDC m_HDC;					//����DC
	HDC m_ImageHDC;				//ԭͼƬDC
	HDC m_CharPaintHDC;			//�ַ���DC
	Bitmap* m_BmpPlus;
	Bitmap* m_GrayBmpPlus;	//����ҶȻ����ͼƬ
	HBITMAP m_hBmp;
	HFONT m_hFont;
	vector<wstring> m_VectorCharPaint;
	DWORD m_DrawTime;
private:
	//�ļ��������
	wstring Name;			//�������ļ�·�����ļ�����		��"People.bmp"
	wstring GrayName;		//������ļ���	��"GrayPeople.bmp"
	wstring Suffix;			//�ļ���׺�磺".bmp"��".jpg"
	vector<wstring> m_VectorFileName;	//�������ļ�����ʱ���������ÿ���ļ���·��

	File* m_LastFile;
	UINT m_Frame;			//֡��
	UINT m_PlayFrame;		//���ŵڼ�֡
	long m_PlayFrameTime;	//ÿ֡ʱ����
	vector<File*> m_VectorFile;
private:
	//����Ϊ��ʼ��������Դ
	void Init();
	void InitWIN();						//���ڳ�ʼ��
	void InitTimer();					//��ʼ����ʱ��
	void InitFont();
	//����Ϊ�ͷŸ�����Դ
	void Release();
	void ReleaseWIN();					//�ͷŴ���
	void ReleaseHDC();
	void ReleaseFont();
	void ReleaseHBITMAP();

	void SaveBMP(HDC& _HDC, vector<wstring>& _vectorWstr, Bitmap* _Bmp, const wstring& _SaveFileName);	//���澲̬ͼ
	void SaveGIF(File* _File, HDC& _HDC, vector<wstring>& _vectorWstr, Bitmap* _Gif);	//���涯̬ͼ
protected:
	bool RegisterWIN();		//ע�ᴰ��
	void CreateWIN();		//��������
	void UpdateWIN();		//���´���
	void ShowWIN();			//��ʾ����
	void UnregisterWIN();	//���ٴ���
public:
	Work(HINSTANCE _hInstance, int _CmdShow);
	~Work();
public:
	void SetExit(bool _Exit) { m_isExit = _Exit; }

	RECT GetWindowRC()const { return m_WindowRc; }
public:
	static LRESULT CALLBACK WndProc(HWND _hwnd, UINT _message, WPARAM _wParam, LPARAM _lParam);	//���ڹ��̺���
	void Run();
	void Draw();					//����
	void DrawBitmap(HDC _HDC);		//����ͼƬ
	void DrawCharPaint(HDC _HDC);	//�����ַ���
	void DrawCharPaintGIF(HDC _HDC);
	void Save(File* _File, HDC& _HDC, vector<wstring>& _vectorWstr, Bitmap* _Image);			//�����ַ���
	
	//���ڽ��յ��ļ�ʱ����
	void AcceptFile(WPARAM _wParam);
	void UpDataCharPaint(Bitmap* _grayBmp, vector<wstring>& _vectorCharPaint);
	void UpDataCharPaintGIF(Bitmap* _grayBmp, vector<wstring>& _vectorCharPaint);

	friend DWORD WINAPI threadSave(LPVOID pParam);	//����һ���߳� ���ڱ���ͼƬ�ļ�

	friend void Bitmap_to_CharPaint(Bitmap* _Bmp, vector<wstring>& _vectorWstr);
	friend void CharPaint_BitBlt_HDC(vector<wstring>& _vectorWstr, HDC& _HDC, HFONT& _hFont);
};

void Bitmap_to_CharPaint(Bitmap* _Bmp, vector<wstring>& _vectorWstr);
void CharPaint_BitBlt_HDC(vector<wstring>& _vectorWstr, HDC& _HDC, HFONT& _hFont);

#endif