#pragma once
#ifndef GrayBitmap_h
#define GrayBitmap_h
#include "Character painting.h"


class GrayBitmap {

	static GrayBitmap* s_GrayBitmap;

	void init();


	//�ҶȻ�������ɫ
	void GrayColor(Color* _color);
public:
	GrayBitmap();
	~GrayBitmap() {}

	static GrayBitmap* getInstance() {
		if (!s_GrayBitmap)
			return s_GrayBitmap = new GrayBitmap();
		return s_GrayBitmap;
	}

	//�ҶȻ�ͼƬ
	void GrayBmp(Bitmap* _BmpPlus);

	//���ҶȻ�������תΪ�ַ�
	wstring GrayPixelConversionChar(UINT _colorRGB);

	UINT SearchGrayBitmap(Bitmap* _GrayBmpPlus, int _No_X, int _No_Y);	//�����ѻҶȻ���ͼƬ������һ�������е�ƽ�����ص�RGBֵ
};

#endif