#include "stdafx.h"
#include "GrayBitmap.h"

GrayBitmap* GrayBitmap::s_GrayBitmap = NULL;

GrayBitmap::GrayBitmap() {
	init();
}

void GrayBitmap::init() {
}

void GrayBitmap::GrayBmp(Bitmap* _BmpPlus) {
	int Width = _BmpPlus->GetWidth();
	int Height = _BmpPlus->GetHeight();

	Color color;
	for (int H = 0; H < Height; ++H) {
		for (int W = 0; W < Width; ++W) {
			_BmpPlus->GetPixel(W, H, &color);
			GrayColor(&color);
			_BmpPlus->SetPixel(W, H, color);
		}
	}
}

void GrayBitmap::GrayColor(Color* _color) {
	int grayColor = ((int)_color->GetR() + (int)_color->GetG() + (int)_color->GetB()) / 3;
	ARGB argb = ((ARGB)(grayColor) << 0) |
				((ARGB)(grayColor) << 8) |
				((ARGB)(grayColor) << 16) |
				((ARGB)(255) << 24);
	_color->SetValue(argb);
}

wstring GrayBitmap::GrayPixelConversionChar(UINT _colorRGB) {
	if (_colorRGB >= 230)		return std::wstring(L" ");
	else if (_colorRGB >= 205)	return std::wstring(L"`");
	else if (_colorRGB >= 180)	return std::wstring(L".");
	else if (_colorRGB >= 155)	return std::wstring(L":");
	else if (_colorRGB >= 130)	return std::wstring(L"i");
	else if (_colorRGB >= 105)	return std::wstring(L"!");
	else if (_colorRGB >= 80)	return std::wstring(L"j");
	else if (_colorRGB >= 65)	return std::wstring(L"f");
	else if (_colorRGB >= 40)	return std::wstring(L"k");
	else if (_colorRGB >= 15)	return std::wstring(L"#");
	else return std::wstring(L"%");
}

UINT GrayBitmap::SearchGrayBitmap(Bitmap* _GrayBmpPlus, int _No_X, int _No_Y) {
	int x = _No_X * PIECE_WIDTH;
	int y = _No_Y * PIECE_HEIGHT;
	int maxX = (_No_X + 1) * PIECE_WIDTH;
	int maxY = (_No_Y + 1) * PIECE_HEIGHT;
	COLORREF ColorNow;
	Color color;
	UINT colorRGB = 0;
	for (y = _No_Y * PIECE_HEIGHT; y < maxY; ++y) {
		for (x = _No_X * PIECE_WIDTH; x < maxX; ++x) {
			ColorNow = _GrayBmpPlus->GetPixel(x, y, &color);
			colorRGB += color.GetR();	//图片已经灰度化，r、g、b都相等，任意取一个就行
		}
	}
	colorRGB /= PIECE_WIDTH * PIECE_HEIGHT;
	return colorRGB;
}