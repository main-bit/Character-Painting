#pragma once
//�ļ���һЩ���桢����ͷ�ļ�
#ifndef HandleFile_H
#define HandleFile_H

#include "Character painting.h"

struct File {
	wchar_t m_PathFileName[_MAX_PATH];	//�ļ�ȫ·��			�磺L"C:\\image.bmp"
	wstring m_FileNameSuffix;			//�ļ���				�磺L"image.bmp"
	wstring m_FileName;					//�ļ�������������׺���磺L"image"
	wstring m_Suffix;					//�ļ�����׺			�磺L".bmp"
	short m_FileType;					//�ļ�����
	enum { Bmp, Jpg, Png, Gif, Other };	//�ļ�����ö��
	wstring m_Prefix;					//�ļ���ǰ׺			�磺L"Gray"	�����ļ�����Ϊ��L"Grayimage.bmp"

	File(const wchar_t* _FileName, wstring _Prefix = L"CPaint");
	void init(const wchar_t* _FileName, wstring _Prefix);
};

//�ļ����ƶ�ȡ
void GetFileName(const wchar_t* _FileNmae, wstring& _wstr);
wstring GetFileName(const wchar_t* _FileName);		//��ȡ�������ļ�·�����ļ�����		�磺"image.jpg"
void GetFileSuffix(const wchar_t* _FileName, wstring& _wstr);
wstring GetFileSuffix(const wchar_t* _FileName);	//��ȡ�ļ���׺��					�磺".bmp"��".jpg"��".gif"


//Bmp�ļ�����
BOOL WriteBmp(const wstring &strFile, const std::vector<BYTE> &vtData, const SIZE &sizeImg);
BOOL WriteBmp(const wstring &strFile, HDC hdc);
BOOL WriteBmp(const wstring &strFile, HDC hdc, const RECT &rcDC);


//Gif�ļ�����
void HDC_to_Bitmap(HDC& hdc, const RECT& rc, Bitmap** _pBmp);
bool WriteGif(const wstring &strFile, HDC hdc, const RECT &rc);


class CGifEncoder

{

public:

	CGifEncoder();

	~CGifEncoder();



public:

	//=================================StartEncoder()=================================

	/// @brief ��ʼgif����

	///

	/// @param [in] saveFilePath gifͼ�񱣴��ȫ·��

	///

	/// @return �ɹ�����true

	//================================================================================

	bool StartEncoder(wstring &saveFilePath);

	//===================================AddFrame()===================================

	/// @brief ���ͼ��

	///

	/// @param [in] im  Image����

	///

	/// @return �ɹ�����true

	//================================================================================

	bool AddFrame(Image *pImage);

	//===================================AddFrame()===================================

	/// @brief ���ͼ��

	///

	/// @param [in] framePath ͼ���ȫ·��

	///

	/// @return  �ɹ�����true

	//================================================================================

	bool AddFrame(wstring &framePath);

	//================================FinishEncoder()===============================

	/// @brief ����gif�ı���

	///

	/// @return  �ɹ�����true

	//================================================================================

	bool FinishEncoder();

	//=================================SetDelayTime()=================================

	/// @brief ��������ͼ���л���ʱ����

	///

	/// @param [in] ms ʱ��,�Ժ���Ϊ��λ

	///

	/// @return ��

	//================================================================================

	void SetDelayTime(int ms);

	//=================================SetRepeatNum()=================================

	/// @brief ����gif�������ŵĴ���

	///

	/// @param [in] num ������0��ʾ���޴�

	///

	/// @return ��

	//================================================================================

	void SetRepeatNum(int num);

	//=================================SetFrameRate()=================================

	/// @brief ����ͼ���֡��

	///

	/// @param [in] fps ֡�ʣ�ÿ�벥��ͼ�����Ŀ

	///

	/// @return ��

	//================================================================================

	void SetFrameRate(float fps);

	//=================================SetFrameSize()=================================

	/// @brief ����ͼ��ĳߴ�

	///

	/// @param [in] width  ͼ��Ŀ��

	/// @param [in] height ͼ��ĸ߶�

	///

	/// @return ��

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