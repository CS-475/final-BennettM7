#include "./include/GShader.h"
#include "./include/GMatrix.h"
class BarycentricColorShader : public GShader {
private:
    GPoint p0, p1, p2;
    GColor c0, c1, c2;
    GMatrix inverseCTM;

public:
    BarycentricColorShader(GPoint p0, GPoint p1, GPoint p2, GColor c0, GColor c1, GColor c2)
        : p0(p0), p1(p1), p2(p2), c0(c0), c1(c1), c2(c2) {}

    bool setContext(const GMatrix& ctm) override {
        auto inv = ctm.invert();
        if (inv.has_value()) {
            inverseCTM = inv.value();
            return true;
        }
        return false;
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        for (int i = 0; i < count; ++i) {
            GPoint local_point;
            GPoint device_point = {x + i + 0.5f, y + 0.5f};
            inverseCTM.mapPoints(&local_point, &device_point, 1);

            float area = (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y);
            float alpha = ((p1.x - local_point.x) * (p2.y - local_point.y) - (p2.x - local_point.x) * (p1.y - local_point.y)) / area;
            float beta = ((p2.x - local_point.x) * (p0.y - local_point.y) - (p0.x - local_point.x) * (p2.y - local_point.y)) / area;
            float gamma = 1.0f - alpha - beta;

            GColor resultColor = {
                alpha * c0.r + beta * c1.r + gamma * c2.r,
                alpha * c0.g + beta * c1.g + gamma * c2.g,
                alpha * c0.b + beta * c1.b + gamma * c2.b,
                alpha * c0.a + beta * c1.a + gamma * c2.a,
            };
            
            int a = GRoundToInt(255 * resultColor.a);
            int r = GRoundToInt(255 * resultColor.a * resultColor.r);
            int g = GRoundToInt(255 * resultColor.a * resultColor.g);
            int b = GRoundToInt(255 * resultColor.a * resultColor.b);
            row[i] = GPixel_PackARGB(a, r, g, b);
        }
    }

    bool isOpaque() override {
        return (c0.a == 1.0f && c1.a == 1.0f && c2.a == 1.0f);
    }
};
