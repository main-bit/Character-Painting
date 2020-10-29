#pragma once

#ifndef Character_Painting_H
#define Character_Painting_H

#include "resource.h"
#include <iostream>
#include <string>
#include <vector>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace std;
using namespace Gdiplus;



//字体尺寸
#define FONT_SIZE		(10)
#define FONT_WIDTH		(FONT_SIZE / 2)
#define FONT_HEIGHT		(FONT_SIZE)

//将图片分成若干个矩形	每个矩形的尺寸
#define PIECE_WIDTH		FONT_WIDTH
#define PIECE_HEIGHT	FONT_HEIGHT

void initGDIPlus();
void ReleaseGDIPlus();


#endif