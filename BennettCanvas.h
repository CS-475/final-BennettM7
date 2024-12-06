#ifndef BennettCanvas_DEFINED
#define BennettCanvas_DEFINED

#include "./include/GCanvas.h"
#include "./include/GRect.h"
#include "./include/GPixel.h"
#include "./include/GColor.h"
#include "./include/GBlendMode.h"
#include "./include/GMatrix.h"
#include "./include/GShader.h"
#include "./include/GPathBuilder.h"
#include "./include/GPath.h"
#include "ProxyShader.cpp"
#include "ComposShader.cpp"
#include "BennettUtils.h"
#include "CurveUtils.h"
#include "BEdge.cpp"
#include <stack>
#include <vector>

class BennettCanvas : public GCanvas {
    private:
        GBitmap bitmap;

        typedef GPixel (*BlendFunc)(GPixel dstPixel, int a, int r, int g, int b);

        BlendFunc blendFunc;

        GMatrix ctm;
        std::stack<GMatrix> ctmStack;

    public:
        BennettCanvas(const GBitmap& bmp);

        void clear(const GColor&) override;
        void drawRect(const GRect& rect, const GPaint& paint) override;
        void drawConvexPolygon(const GPoint* points, int count, const GPaint& paint) override;
        void save() override;
        void restore() override;
        void concat(const GMatrix& matrix) override;
        void drawPath(const GPath&, const GPaint&) override;
        void blitSpan(int L, int y, int spanWidth, const GPaint& paint, GShader* shader);
        void drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[],
                             int count, const int indices[], const GPaint& paint) override;
        
        void drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4],
                          int level, const GPaint&) override;

};

/**
 *  Implemnt this, returning an instance of your subclass of GCanvas.
 */
std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap&);

/**
 *  Implement this, drawing into the provided canvas, and returning the title of your artwork.
 */
std::string GDrawSomething(GCanvas* canvas, GISize dim);

#endif
