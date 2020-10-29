#include "stdafx.h"
#include "HandleFile.h"


File::File(const wchar_t* _FileName, wstring _Prefix) {
	init(_FileName, _Prefix);
}

void File::init(const wchar_t* _FileName, wstring _Prefix) {
	wsprintf(m_PathFileName, _FileName, _MAX_PATH);
	GetFileName(m_PathFileName, m_FileNameSuffix);
	m_FileName = m_FileNameSuffix;
	GetFileSuffix(m_PathFileName, m_Suffix);
	if (!m_Suffix.find(L".bmp")) {
		m_FileType = File::Bmp;
		m_FileName.erase(m_FileName.end() - 4, m_FileName.end());		//去掉后缀	L".bmp"
	}
	else if (!m_Suffix.find(L".jpg")) {
		m_FileType = File::Jpg;
		m_FileName.erase(m_FileName.end() - 4, m_FileName.end());
	}
	else if (!m_Suffix.find(L".png")) {
		m_FileType = File::Png;
		m_FileName.erase(m_FileName.end() - 4, m_FileName.end());
	}
	else if (!m_Suffix.find(L".gif")) {
		m_FileType = File::Gif;
		m_FileName.erase(m_FileName.end() - 4, m_FileName.end());
	}
	else {
		m_FileType = File::Other;
	}
	m_Prefix = _Prefix;
}


//文件名称读取
void GetFileName(const wchar_t* _FileName, wstring& _wstr) {
	wstring FileName(_FileName);	//包含文件路径的文件名称
	int FileNameNumber = FileName.rfind('\\', FileName.length()) + 1;	//+1是去掉'\\'
	_wstr = wstring(FileName, FileNameNumber, FileName.length());
}
wstring GetFileName(const wchar_t* _FileName) {
	wstring Name;
	GetFileName(_FileName, Name);
	return Name;
}

void GetFileSuffix(const wchar_t* _FileName, wstring& _wstr) {
	wstring FileName(_FileName);
	int FileSuffixNumber = FileName.rfind('.', FileName.length());
	_wstr = wstring(FileName, FileSuffixNumber, FileName.length());
}
wstring GetFileSuffix(const wchar_t* _FileName) {
	wstring Suffix;
	GetFileSuffix(_FileName, Suffix);
	return Suffix;
}




//Bmp文件保存
BOOL WriteBmp(const wstring &strFile, const std::vector<BYTE> &vtData, const SIZE &sizeImg){

	BITMAPINFOHEADER bmInfoHeader = { 0 };
	bmInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfoHeader.biWidth = sizeImg.cx;
	bmInfoHeader.biHeight = sizeImg.cy;
	bmInfoHeader.biPlanes = 1;
	bmInfoHeader.biBitCount = 24;

	//Bimap file header in order to write bmp file  
	BITMAPFILEHEADER bmFileHeader = { 0 };
	bmFileHeader.bfType = 0x4d42;  //bmp    
	bmFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmFileHeader.bfSize = bmFileHeader.bfOffBits + ((bmInfoHeader.biWidth * bmInfoHeader.biHeight) * 3); ///3=(24 / 8)  

	HANDLE hFile = CreateFile(strFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		return FALSE;
	}

	DWORD dwWrite = 0;
	WriteFile(hFile, &bmFileHeader, sizeof(BITMAPFILEHEADER), &dwWrite, NULL);
	WriteFile(hFile, &bmInfoHeader, sizeof(BITMAPINFOHEADER), &dwWrite, NULL);
	WriteFile(hFile, &vtData[0], vtData.size(), &dwWrite, NULL);


	CloseHandle(hFile);

	return TRUE;
}

BOOL WriteBmp(const wstring &strFile, HDC hdc){
	int iWidth = GetDeviceCaps(hdc, HORZRES);
	int iHeight = GetDeviceCaps(hdc, VERTRES);
	RECT rcDC = { 0,0,iWidth,iHeight };

	return WriteBmp(strFile, hdc, rcDC);
}

BOOL WriteBmp(const wstring &strFile, HDC hdc, const RECT &rcDC){
	BOOL bRes = FALSE;
	BITMAPINFO bmpInfo = { 0 };
	BYTE *pData = NULL;
	SIZE sizeImg = { 0 };
	HBITMAP hBmp = NULL;
	std::vector<BYTE> vtData;
	HGDIOBJ hOldObj = NULL;
	HDC hdcMem = NULL;

	//Initilaize the bitmap information   
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = rcDC.right - rcDC.left;
	bmpInfo.bmiHeader.biHeight = rcDC.bottom - rcDC.top;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 24;

	//Create the compatible DC to get the data  
	hdcMem = CreateCompatibleDC(hdc);
	if (hdcMem == NULL){
		goto EXIT;
	}

	//Get the data from the memory DC     
	hBmp = CreateDIBSection(hdcMem, &bmpInfo, DIB_RGB_COLORS, reinterpret_cast<VOID **>(&pData), NULL, 0);
	if (hBmp == NULL){
		goto EXIT;
	}
	hOldObj = SelectObject(hdcMem, hBmp);

	//Draw to the memory DC  
	sizeImg.cx = bmpInfo.bmiHeader.biWidth;
	sizeImg.cy = bmpInfo.bmiHeader.biHeight;
	StretchBlt(hdcMem,
		0,
		0,
		sizeImg.cx,
		sizeImg.cy,
		hdc,
		rcDC.left,
		rcDC.top,
		rcDC.right - rcDC.left + 1,
		rcDC.bottom - rcDC.top + 1,
		SRCCOPY);


	vtData.resize(sizeImg.cx * sizeImg.cy * 3);
	memcpy(&vtData[0], pData, vtData.size());
	bRes = WriteBmp(strFile, vtData, sizeImg);

	SelectObject(hdcMem, hOldObj);

EXIT:
	if (hBmp != NULL){
		DeleteObject(hBmp);
	}

	if (hdcMem != NULL){
		DeleteDC(hdcMem);
	}

	return bRes;
}




void HDC_to_Bitmap(HDC& _HDC, const RECT& _rc, Bitmap** _pBmp) {
	HDC          hMemDC;
	HBITMAP      hBmp;
	BITMAP       bmp;
	HANDLE       hOld;
	RECT         rcWnd;

	// 计算子窗口绘图区域的宽度和高度
	int nWidth = _rc.right - _rc.left;
	int nHeight = _rc.bottom - _rc.top;

	// 将窗口内容复制到内存 DC 中
	hMemDC = CreateCompatibleDC(_HDC);
	hBmp = CreateCompatibleBitmap(_HDC, nWidth, nHeight);
	hOld = SelectObject(hMemDC, hBmp);
	BOOL rt = BitBlt(hMemDC, 0, 0, nWidth, nHeight, _HDC, 0, 0, SRCCOPY);
	if (!rt) {
		// 释放资源
		::DeleteObject(hBmp);
		::DeleteDC(hMemDC);
	}

	// 从内存 DC 中获取 BITMAP
	hBmp = (HBITMAP)SelectObject(hMemDC, hOld);
	*_pBmp = new Bitmap(hBmp, NULL);

	// 释放资源
	::DeleteObject(hBmp);
	::DeleteDC(hMemDC);
}

bool WriteGif(const wstring &strFile, HDC hdc, const RECT &rcDC) {
	Bitmap* BmpPlus = NULL;
	return true;
}


CGifEncoder::CGifEncoder() {
	m_started = false;
	m_width = 320;
	m_height = 240;
	m_repeatNum = 0;
	m_haveFrame = false;
	m_pStrSavePath = NULL;
	m_pImage = NULL;
	m_plValue = NULL;
}



CGifEncoder::~CGifEncoder() {
	if (NULL != m_pStrSavePath)
		delete m_pStrSavePath;

	if (NULL != m_pImage)
		delete m_pImage;

	if (NULL != m_plValue)
		delete[] m_plValue;

	size_t size = m_pBitMapVec.size();

	for (size_t ix = 0; ix < size; ix++) {
		delete m_pBitMapVec[ix];
		m_pBitMapVec[ix] = NULL;
	}

}

bool CGifEncoder::StartEncoder(wstring &saveFilePath) {
	bool flag = true;
	if (NULL != m_pStrSavePath) {
		delete m_pStrSavePath;
		m_pStrSavePath = NULL;
	}

	m_pStrSavePath = new wstring(saveFilePath);
	m_started = true;
	return(flag);
}

bool CGifEncoder::AddFrame(Image *pImage) {
	if (!m_started || NULL == pImage)
		return(false);

	bool flag = true;

	if (!m_haveFrame) {
		m_pImage = new Bitmap(m_width, m_height);
		Graphics g(m_pImage);
		g.DrawImage(pImage, 0, 0, m_width, m_height);

		m_haveFrame = true;

		return(true);
	}

	Bitmap * pBitMap = new Bitmap(m_width, m_height);
	Graphics g(pBitMap);
	g.DrawImage(pImage, 0, 0, m_width, m_height);
	m_pBitMapVec.push_back(pBitMap);
	return(flag);
}

bool CGifEncoder::AddFrame(wstring &framePath) {
	if (!m_started)
		return(false);

	bool flag = true;
	Gdiplus::Status statue;
	if (!m_haveFrame) {
		m_pImage = new Bitmap(m_width, m_height);
		Graphics g(m_pImage);
		Bitmap bitmap(framePath.c_str());
		g.DrawImage(&bitmap, 0, 0, m_width, m_height);
		m_haveFrame = true;
		return(true);
	}
	Bitmap		*pBitMap = new Bitmap(m_width, m_height);
	Graphics	g(pBitMap);
	Bitmap bitmap(framePath.c_str());
	statue = g.DrawImage(&bitmap, 0, 0, m_width, m_height);
	m_pBitMapVec.push_back(pBitMap);
	return(flag);
}

bool CGifEncoder::FinishEncoder() {
	if (!m_started || !m_haveFrame)
		return(false);

	bool flag = true;
	Gdiplus::Status statue;

	//1.0 设置图像的属性
	SetImagePropertyItem();

	//2.0 保存第一副图像
	GUID						gifGUID;
	EncoderParameters	encoderParams;
	GetEncoderClsid(L"image/gif", &gifGUID);
	encoderParams.Count = 1;
	encoderParams.Parameter[0].Guid = Gdiplus::EncoderSaveFlag;
	encoderParams.Parameter[0].NumberOfValues = 1;
	encoderParams.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;//第一帧需要设置为MultiFrame

	long firstValue = Gdiplus::EncoderValueMultiFrame;

	encoderParams.Parameter[0].Value = &firstValue;

	m_pImage->Save(m_pStrSavePath->c_str(), &gifGUID, &encoderParams);

	//3.0 保存剩余的图像
	size_t size = m_pBitMapVec.size();

	firstValue = Gdiplus::EncoderValueFrameDimensionTime;

	encoderParams.Parameter[0].Value = &firstValue;

	for (size_t ix = 0; ix < size; ix++)
		statue = m_pImage->SaveAdd(m_pBitMapVec[ix], &encoderParams);

	m_started = false;
	m_haveFrame = false;
	return(flag);
}

void CGifEncoder::SetDelayTime(int ms) {
	if (m_plValue != NULL)
		delete[] m_plValue;
	if (ms > 0)
		ms /= 10.0f;
	m_delayTime.push_back(ms);
	m_plValue = new long[m_delayTime.size()];
	for (int i = 0; i < m_delayTime.size(); ++i) {
		m_plValue[i] = m_delayTime[i];
	}
}

void CGifEncoder::SetRepeatNum(int num) {
	if (num >= 0)
		m_repeatNum = num;
}

void CGifEncoder::SetFrameSize(int width, int height) {
	if (!m_haveFrame)
	{
		m_width = width;
		m_height = height;
		if (m_width < 1)
			m_width = 320;

		if (m_height < 1)
			height = 240;
	}
}

void CGifEncoder::SetImagePropertyItem() {
	if (!m_started || NULL == m_pImage)
		return;

	PropertyItem propertyItem;

	//1.0 设置动画循环的次数	
	short sValue = m_repeatNum; //0 -- 不限次数
	propertyItem.id = PropertyTagLoopCount;
	propertyItem.length = 2; //字节数
	propertyItem.type = PropertyTagTypeShort;
	propertyItem.value = &sValue;
	m_pImage->SetPropertyItem(&propertyItem);

	//2.0 设置每副图像延迟的时间，从第一副开始
	long lImageNum = 1 + m_pBitMapVec.size();

	propertyItem.id = PropertyTagFrameDelay;
	propertyItem.length = 4 * lImageNum;//字节数
	propertyItem.type = PropertyTagTypeLong;
	propertyItem.value = m_plValue; //不限次数
	m_pImage->SetPropertyItem(&propertyItem);

}

bool CGifEncoder::GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
	UINT num = 0, size = 0;

	GetImageEncodersSize(&num, &size);

	if (size == 0)
		return false;  // Failure

	ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size));

	GetImageEncoders(num, size, pImageCodecInfo);
	bool found = false;
	for (UINT ix = 0; !found && ix < num; ++ix) {
		if (_wcsicmp(pImageCodecInfo[ix].MimeType, format) == 0) {
			*pClsid = pImageCodecInfo[ix].Clsid;
			found = true;
			break;
		}
	}
	free(pImageCodecInfo);
	return found;
}