#pragma once
//文件的一些保存、处理头文件
#ifndef HandleFile_H
#define HandleFile_H

#include "Character painting.h"

struct File {
	wchar_t m_PathFileName[_MAX_PATH];	//文件全路径			如：L"C:\\image.bmp"
	wstring m_FileNameSuffix;			//文件名				如：L"image.bmp"
	wstring m_FileName;					//文件名（不包含后缀）如：L"image"
	wstring m_Suffix;					//文件名后缀			如：L".bmp"
	short m_FileType;					//文件类型
	enum { Bmp, Jpg, Png, Gif, Other };	//文件类型枚举
	wstring m_Prefix;					//文件名前缀			如：L"Gray"	保存文件后名为：L"Grayimage.bmp"

	File(const wchar_t* _FileName, wstring _Prefix = L"CPaint");
	void init(const wchar_t* _FileName, wstring _Prefix);
};

//文件名称读取
void GetFileName(const wchar_t* _FileNmae, wstring& _wstr);
wstring GetFileName(const wchar_t* _FileName);		//获取不包含文件路径的文件名称		如："image.jpg"
void GetFileSuffix(const wchar_t* _FileName, wstring& _wstr);
wstring GetFileSuffix(const wchar_t* _FileName);	//获取文件后缀名					如：".bmp"、".jpg"、".gif"


//Bmp文件保存
BOOL WriteBmp(const wstring &strFile, const std::vector<BYTE> &vtData, const SIZE &sizeImg);
BOOL WriteBmp(const wstring &strFile, HDC hdc);
BOOL WriteBmp(const wstring &strFile, HDC hdc, const RECT &rcDC);


//Gif文件保存
void HDC_to_Bitmap(HDC& hdc, const RECT& rc, Bitmap** _pBmp);
bool WriteGif(const wstring &strFile, HDC hdc, const RECT &rc);


class CGifEncoder

{

public:

	CGifEncoder();

	~CGifEncoder();



public:

	//=================================StartEncoder()=================================

	/// @brief 开始gif编码

	///

	/// @param [in] saveFilePath gif图像保存的全路径

	///

	/// @return 成功返回true

	//================================================================================

	bool StartEncoder(wstring &saveFilePath);

	//===================================AddFrame()===================================

	/// @brief 添加图像

	///

	/// @param [in] im  Image对象

	///

	/// @return 成功返回true

	//================================================================================

	bool AddFrame(Image *pImage);

	//===================================AddFrame()===================================

	/// @brief 添加图像

	///

	/// @param [in] framePath 图像的全路径

	///

	/// @return  成功返回true

	//================================================================================

	bool AddFrame(wstring &framePath);

	//================================FinishEncoder()===============================

	/// @brief 结束gif的编码

	///

	/// @return  成功返回true

	//================================================================================

	bool FinishEncoder();

	//=================================SetDelayTime()=================================

	/// @brief 设置两幅图像切换的时间间隔

	///

	/// @param [in] ms 时间,以毫秒为单位

	///

	/// @return 无

	//================================================================================

	void SetDelayTime(int ms);

	//=================================SetRepeatNum()=================================

	/// @brief 设置gif动画播放的次数

	///

	/// @param [in] num 次数，0表示无限次

	///

	/// @return 无

	//================================================================================

	void SetRepeatNum(int num);

	//=================================SetFrameRate()=================================

	/// @brief 设置图像的帧率

	///

	/// @param [in] fps 帧率，每秒播放图像的数目

	///

	/// @return 无

	//================================================================================

	void SetFrameRate(float fps);

	//=================================SetFrameSize()=================================

	/// @brief 设置图像的尺寸

	///

	/// @param [in] width  图像的宽度

	/// @param [in] height 图像的高度

	///

	/// @return 无

	//================================================================================

	void SetFrameSize(int width, int height);

private:
	void SetImagePropertyItem();
	bool GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
private:
	int					m_width;
	int					m_height;
	int				m_repeatNum;
	vector<int>			m_delayTime;
	bool				m_started;
	bool				m_haveFrame;
	long* m_plValue;
	wstring				*m_pStrSavePath;
	Bitmap		*m_pImage;
	vector<Bitmap *> m_pBitMapVec;
};


#endif