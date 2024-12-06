#include "./include/GShader.h"
#include "./include/GMatrix.h"
#include "./include/GPixel.h"
#include "BennettUtils.h"
#include <vector>
#include <memory>

class LinearGradientShader : public GShader {
private:
    GPoint p0, p1;
    std::vector<GColor> colors;
    GMatrix inverseCTM;
    GTileMode tileMode;
    bool isOpaqueGradient;

    float length;
    GVector direction;


public:
    LinearGradientShader(GPoint p0, GPoint p1, const GColor* c, int count, GTileMode mode)
        : p0(p0), p1(p1), colors(c, c + count), tileMode(mode) {

        direction = p1 - p0;
        length = direction.length();
        direction = direction * (1.0f / length);  // Normalize

        isOpaqueGradient = std::all_of(colors.begin(), colors.end(), [](const GColor& color) {
            return GPinToUnit(color.a) == 1.0f;
        });
    }

    bool isOpaque() override {
        return isOpaqueGradient;
    }

    bool setContext(const GMatrix& ctm) override {
        auto inv = ctm.invert();
        if (inv.has_value()) {
            inverseCTM = inv.value();
            return true;
        }
        return false;
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        // Transform the coordinates to the local space of the shader
        for (int i = 0; i < count; i++) {
            GPoint local_point;
            GPoint device_point = {x + i + 0.5f, y + 0.5f};
            inverseCTM.mapPoints(&local_point, &device_point, 1);

            // Calculate the direction and length
            float dx = p1.x - p0.x;
            float dy = p1.y - p0.y;
            float D = std::sqrt(dx * dx + dy * dy);

            // Normalize the direction
            float invD = 1.0f / D;
            float cosTheta = dx * invD;
            float sinTheta = dy * invD;

            // Calculate the local position on the gradient
            float t = (local_point.x - p0.x) * cosTheta + (local_point.y - p0.y) * sinTheta;
            
            // Handle tile modes
            if (tileMode == GTileMode::kClamp) {
                t = GPinToUnit(t / D);  // Normalize t to [0, 1]
            } else if (tileMode == GTileMode::kRepeat) {
                t = fmod(t / D, 1.0f);  // Repeat the gradient
            } else if (tileMode == GTileMode::kMirror) {
                t = fmod(t / D, 2.0f);  // Mirror the gradient
                if (t > 1.0f) t = 2.0f - t;  // Adjust for mirroring
            } else {
                // Other tile modes could be added here
            }

            // Interpolate the color
            int colorCount = static_cast<int>(colors.size());
            float scaledT = t * (colorCount - 1);
            int idx = GFloorToInt(scaledT);
            float localT = scaledT - idx;

            // Get the two colors to interpolate
            GColor c0 = colors[idx];
            GColor c1 = colors[std::min(idx + 1, colorCount - 1)];

            // Interpolate between the two colors
            GColor resultColor = {
                c0.r + localT * (c1.r - c0.r),
                c0.g + localT * (c1.g - c0.g),
                c0.b + localT * (c1.b - c0.b),
                c0.a + localT * (c1.a - c0.a),
            };

            // Pack the result color into the pixel
            row[i] = GPixel_PackARGB(
                calcA(resultColor.a),
                calcRGB(resultColor.a, resultColor.r),
                calcRGB(resultColor.a, resultColor.g),
                calcRGB(resultColor.a, resultColor.b)
            );
        }
    }
};

std::shared_ptr<GShader> GCreateLinearGradient(GPoint p0, GPoint p1, const GColor colors[], int count, GTileMode tileMode) {
    if (count < 1) {
        return nullptr;
    }
    return std::make_shared<LinearGradientShader>(p0, p1, colors, count, tileMode);
}