#include "./include/GShader.h"
#include "./include/GPixel.h"
#include "./include/GMatrix.h"
class ComposShader : public GShader {
private:
    std::shared_ptr<GShader> shader1;
    std::shared_ptr<GShader> shader2;

public:
    ComposShader(std::shared_ptr<GShader> s1, std::shared_ptr<GShader> s2)
        : shader1(s1), shader2(s2) {}

    bool isOpaque() override {
        return shader1->isOpaque() && shader2->isOpaque();
    }

    bool setContext(const GMatrix& ctm) override {
        return shader1->setContext(ctm) && shader2->setContext(ctm);
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        std::vector<GPixel> row1(count);
        std::vector<GPixel> row2(count);

        shader1->shadeRow(x, y, count, row1.data());
        shader2->shadeRow(x, y, count, row2.data());

        for (int i = 0; i < count; ++i) {
            int a1 = GPixel_GetA(row1[i]);
            int r1 = GPixel_GetR(row1[i]);
            int g1 = GPixel_GetG(row1[i]);
            int b1 = GPixel_GetB(row1[i]);

            int a2 = GPixel_GetA(row2[i]);
            int r2 = GPixel_GetR(row2[i]);
            int g2 = GPixel_GetG(row2[i]);
            int b2 = GPixel_GetB(row2[i]);

            int a = a1 * a2 / 255;
            int r = r1 * r2 / 255;
            int g = g1 * g2 / 255;
            int b = b1 * b2 / 255;

            row[i] = GPixel_PackARGB(a, r, g, b);
        }
    }
};