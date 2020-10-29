#pragma once
#ifndef Work_h
#define Work_h
#include <Windows.h>
#include "GrayBitmap.h"
#include "HandleFile.h"

class Work {
private:
	//窗口属性
	HWND m_Hwnd;
	RECT m_WindowRc;
	WNDCLASSEX m_WndClassEx;
	HINSTANCE* m_hInstance;
	int CmdShow;
private:
	HANDLE phWait;			//创建一个定时器
	LARGE_INTEGER liDueTime;
	DWORD dwRet;
	bool m_isExit;				//是否退出
	bool m_isSave;				//是否需要保存
private:
	//图片输出
	wchar_t m_FileName[_MAX_PATH];	//文件名
	HDC m_HDC;					//窗口DC
	HDC m_ImageHDC;				//原图片DC
	HDC m_CharPaintHDC;			//字符画DC
	Bitmap* m_BmpPlus;
	Bitmap* m_GrayBmpPlus;	//储存灰度化后的图片
	HBITMAP m_hBmp;
	HFONT m_hFont;
	vector<wstring> m_VectorCharPaint;
	DWORD m_DrawTime;
private:
	//文件属性相关
	wstring Name;			//不包含文件路径的文件名称		如"People.bmp"
	wstring GrayName;		//保存的文件名	如"GrayPeople.bmp"
	wstring Suffix;			//文件后缀如：".bmp"、".jpg"
	vector<wstring> m_VectorFileName;	//当批量文件拖入时，逐个储存每个文件的路径

	File* m_LastFile;
	UINT m_Frame;			//帧数
	UINT m_PlayFrame;		//播放第几帧
	long m_PlayFrameTime;	//每帧时间间隔
	vector<File*> m_VectorFile;
private:
	//以下为初始化各种资源
	void Init();
	void InitWIN();						//窗口初始化
	void InitTimer();					//初始化定时器
	void InitFont();
	//以下为释放各种资源
	void Release();
	void ReleaseWIN();					//释放窗口
	void ReleaseHDC();
	void ReleaseFont();
	void ReleaseHBITMAP();

	void SaveBMP(HDC& _HDC, vector<wstring>& _vectorWstr, Bitmap* _Bmp, const wstring& _SaveFileName);	//保存静态图
	void SaveGIF(File* _File, HDC& _HDC, vector<wstring>& _vectorWstr, Bitmap* _Gif);	//保存动态图
protected:
	bool RegisterWIN();		//注册窗口
	void CreateWIN();		//创建窗口
	void UpdateWIN();		//更新窗口
	void ShowWIN();			//显示窗口
	void UnregisterWIN();	//销毁窗口
public:
	Work(HINSTANCE _hInstance, int _CmdShow);
	~Work();
public:
	void SetExit(bool _Exit) { m_isExit = _Exit; }

	RECT GetWindowRC()const { return m_WindowRc; }
public:
	static LRESULT CALLBACK WndProc(HWND _hwnd, UINT _message, WPARAM _wParam, LPARAM _lParam);	//窗口过程函数
	void Run();
	void Draw();					//绘制
	void DrawBitmap(HDC _HDC);		//绘制图片
	void DrawCharPaint(HDC _HDC);	//绘制字符画
	void DrawCharPaintGIF(HDC _HDC);
	void Save(File* _File, HDC& _HDC, vector<wstring>& _vectorWstr, Bitmap* _Image);			//保存字符画
	
	//窗口接收到文件时调用
	void AcceptFile(WPARAM _wParam);
	void UpDataCharPaint(Bitmap* _grayBmp, vector<wstring>& _vectorCharPaint);
	void UpDataCharPaintGIF(Bitmap* _grayBmp, vector<wstring>& _vectorCharPaint);

	friend DWORD WINAPI threadSave(LPVOID pParam);	//开启一个线程 用于保存图片文件

	friend void Bitmap_to_CharPaint(Bitmap* _Bmp, vector<wstring>& _vectorWstr);
	friend void CharPaint_BitBlt_HDC(vector<wstring>& _vectorWstr, HDC& _HDC, HFONT& _hFont);
};

void Bitmap_to_CharPaint(Bitmap* _Bmp, vector<wstring>& _vectorWstr);
void CharPaint_BitBlt_HDC(vector<wstring>& _vectorWstr, HDC& _HDC, HFONT& _hFont);

#endif