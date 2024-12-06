#include "./include/GMath.h"
#include "./include/GBitmap.h"
#include "./include/GPoint.h"
#include "./include/GColor.h"
#include "./include/GShader.h"
#include "./include/GMatrix.h"
#include "BarycentricColorShader.cpp"
#pragma once

int div255(int n);


int calcA(float a);

int calcRGB(float a, float rgb);

GPixel erase(GPixel, int a, int r, int g, int b);

GPixel srcOver(GPixel dstPixel, int a, int r, int g, int b);

GPixel kClear(GPixel, int, int, int, int);

//void kSrc(GBitmap bitmap, int rect_y, int rect_x, int height, int width, int a, int r, int g, int b){}

GPixel kDst(GPixel dstPixel, int, int, int, int);

GPixel kDstOver(GPixel dstPixel, int a, int r, int g, int b);

GPixel kSrcIn(GPixel dstPixel, int a, int r, int g, int b);

GPixel kDstIn(GPixel dstPixel, int a, int, int, int);

GPixel kSrcOut(GPixel dstPixel, int a, int r, int g, int b);

GPixel kDstOut(GPixel dstPixel, int a, int , int, int);

GPixel kSrcATop(GPixel dstPixel, int a, int r, int g, int b);

GPixel kDstATop(GPixel dstPixel, int a, int r, int g, int b);

GPixel kXor(GPixel dstPixel, int a, int r, int g, int b);

GMatrix compute_basis(const GPoint& p0, const GPoint& p1, const GPoint& p2);

std::shared_ptr<GShader> interpolateColor(GPoint p0, GPoint p1, GPoint p2, const GColor& c0, const GColor& c1, const GColor& c2);
