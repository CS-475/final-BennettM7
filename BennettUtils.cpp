#include "BennettUtils.h"
int div255(int n){
    return (n + 128) * 257 >> 16;
}


int calcA(float a){
    return GRoundToInt(a * 255);
}

int calcRGB(float a, float rgb){
    return GRoundToInt(rgb * a * 255);
}

GPixel erase(GPixel, int a, int r, int g, int b){
    return GPixel_PackARGB(a, r, g, b);
}

GPixel srcOver(GPixel dstPixel, int a, int r, int g, int b){

    unsigned da = GPixel_GetA(dstPixel);
    unsigned dr = GPixel_GetR(dstPixel);
    unsigned dg = GPixel_GetG(dstPixel);
    unsigned db = GPixel_GetB(dstPixel);

    // Blending using pre-multiplied values
    unsigned ra = a + div255((255 - a) * da);
    unsigned rr = r + div255((255 - a) * dr);
    unsigned rg = g + div255((255 - a) * dg);
    unsigned rb = b + div255((255 - a) * db);

    return GPixel_PackARGB(ra, rr, rg, rb);
}

GPixel kClear(GPixel, int, int, int, int){
    return GPixel_PackARGB(0, 0, 0, 0);
}

//void kSrc(GBitmap bitmap, int rect_y, int rect_x, int height, int width, int a, int r, int g, int b){}

GPixel kDst(GPixel dstPixel, int, int, int, int){

    unsigned da = GPixel_GetA(dstPixel);
    unsigned dr = GPixel_GetR(dstPixel);
    unsigned dg = GPixel_GetG(dstPixel);
    unsigned db = GPixel_GetB(dstPixel);

    return GPixel_PackARGB(da, dr, dg, db);
}

GPixel kDstOver(GPixel dstPixel, int a, int r, int g, int b){

    unsigned da = GPixel_GetA(dstPixel);
    unsigned dr = GPixel_GetR(dstPixel);
    unsigned dg = GPixel_GetG(dstPixel);
    unsigned db = GPixel_GetB(dstPixel);

    // Blending using pre-multiplied values
    unsigned ra = da + div255((255 - da) * a);
    unsigned rr = dr + div255((255 - da) * r);
    unsigned rg = dg + div255((255 - da) * g);
    unsigned rb = db + div255((255 - da) * b);

    return GPixel_PackARGB(ra, rr, rg, rb);
}

GPixel kSrcIn(GPixel dstPixel, int a, int r, int g, int b){

    unsigned da = GPixel_GetA(dstPixel);

    // Blending using pre-multiplied values
    unsigned ra = div255(da * a);
    unsigned rr = div255(da * r);
    unsigned rg = div255(da * g);
    unsigned rb = div255(da * b);

    return GPixel_PackARGB(ra, rr, rg, rb);
}

GPixel kDstIn(GPixel dstPixel, int a, int, int, int){

    unsigned da = GPixel_GetA(dstPixel);
    unsigned dr = GPixel_GetR(dstPixel);
    unsigned dg = GPixel_GetG(dstPixel);
    unsigned db = GPixel_GetB(dstPixel);

    // Blending using pre-multiplied values
    unsigned ra = div255(a * da);
    unsigned rr = div255(a * dr);
    unsigned rg = div255(a * dg);
    unsigned rb = div255(a * db);

    return GPixel_PackARGB(ra, rr, rg, rb);
}

GPixel kSrcOut(GPixel dstPixel, int a, int r, int g, int b){

    unsigned da = GPixel_GetA(dstPixel);

    // Blending using pre-multiplied values
    unsigned ra = div255((255 - da)* a);
    unsigned rr = div255((255 - da)* r);
    unsigned rg = div255((255 - da)* g);
    unsigned rb = div255((255 - da)* b);

    return GPixel_PackARGB(ra, rr, rg, rb);
}

GPixel kDstOut(GPixel dstPixel, int a, int , int, int){

    unsigned da = GPixel_GetA(dstPixel);
    unsigned dr = GPixel_GetR(dstPixel);
    unsigned dg = GPixel_GetG(dstPixel);
    unsigned db = GPixel_GetB(dstPixel);

    // Blending using pre-multiplied values
    unsigned ra = div255((255 - a) * da);
    unsigned rr = div255((255 - a) * dr);
    unsigned rg = div255((255 - a) * dg);
    unsigned rb = div255((255 - a) * db);

    return GPixel_PackARGB(ra, rr, rg, rb);
}

GPixel kSrcATop(GPixel dstPixel, int a, int r, int g, int b){

    unsigned da = GPixel_GetA(dstPixel);
    unsigned dr = GPixel_GetR(dstPixel);
    unsigned dg = GPixel_GetG(dstPixel);
    unsigned db = GPixel_GetB(dstPixel);

    // Blending using pre-multiplied values
    unsigned ra = div255(da * a) + div255((255 - a) * da);
    unsigned rr = div255(da * r) + div255((255 - a) * dr);
    unsigned rg = div255(da * g) + div255((255 - a) * dg);
    unsigned rb = div255(da * b) + div255((255 - a) * db);

    return GPixel_PackARGB(ra, rr, rg, rb);
}

GPixel kDstATop(GPixel dstPixel, int a, int r, int g, int b){

    unsigned da = GPixel_GetA(dstPixel);
    unsigned dr = GPixel_GetR(dstPixel);
    unsigned dg = GPixel_GetG(dstPixel);
    unsigned db = GPixel_GetB(dstPixel);

    // Blending using pre-multiplied values
    unsigned ra = div255(a * da) + div255((255 - da) * a);
    unsigned rr = div255(a * dr) + div255((255 - da) * r);
    unsigned rg = div255(a * dg) + div255((255 - da) * g);
    unsigned rb = div255(a * db) + div255((255 - da) * b);

    return GPixel_PackARGB(ra, rr, rg, rb);
}

GPixel kXor(GPixel dstPixel, int a, int r, int g, int b){

    unsigned da = GPixel_GetA(dstPixel);
    unsigned dr = GPixel_GetR(dstPixel);
    unsigned dg = GPixel_GetG(dstPixel);
    unsigned db = GPixel_GetB(dstPixel);

    // Blending using pre-multiplied values
    unsigned ra = div255((255 - a) * da + (255 - da) * a);
    unsigned rr = div255((255 - a) * dr + (255 - da) * r);
    unsigned rg = div255((255 - a) * dg + (255 - da) * g);
    unsigned rb = div255((255 - a) * db + (255 - da) * b);

    return GPixel_PackARGB(ra, rr, rg, rb);
}

std::shared_ptr<GShader> interpolateColor(const GPoint p0, const GPoint p1, const GPoint p2, 
                                                         const GColor& c0, const GColor& c1, const GColor& c2) {
    // First, create linear gradient shaders for each edge of the triangle
    //std::shared_ptr<GShader> gradient01 = GCreateLinearGradient(p0, p1, c0, c1, GTileMode::kClamp);
    //std::shared_ptr<GShader> gradient12 = GCreateLinearGradient(p1, p2, c1, c2, GTileMode::kClamp);
    //std::shared_ptr<GShader> gradient20 = GCreateLinearGradient(p2, p0, c2, c0, GTileMode::kClamp);

    // The shader that interpolates between these three gradients based on the barycentric coordinates
    return std::make_shared<BarycentricColorShader>(p0, p1, p2, c0, c1, c2);
}

GMatrix compute_basis(const GPoint& p0, const GPoint& p1, const GPoint& p2) {
    GPoint U = p1 - p0;
    GPoint V = p2 - p0;

    return GMatrix(U.x, V.x, p0.x, U.y, V.y, p0.y);
}
