#pragma once
#ifndef GrayBitmap_h
#define GrayBitmap_h
#include "Character painting.h"


class GrayBitmap {

	static GrayBitmap* s_GrayBitmap;

	void init();


	//灰度化像素颜色
	void GrayColor(Color* _color);
public:
	GrayBitmap();
	~GrayBitmap() {}

	static GrayBitmap* getInstance() {
		if (!s_GrayBitmap)
			return s_GrayBitmap = new GrayBitmap();
		return s_GrayBitmap;
	}

	//灰度化图片
	void GrayBmp(Bitmap* _BmpPlus);

	//将灰度化的像素转为字符
	wstring GrayPixelConversionChar(UINT _colorRGB);

	UINT SearchGrayBitmap(Bitmap* _GrayBmpPlus, int _No_X, int _No_Y);	//搜索已灰度化的图片并返回一个区域中的平均像素的RGB值
};

#endif