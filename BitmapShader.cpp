#include "./include/GMatrix.h"
#include "./include/GBitmap.h"
#include "./include/GShader.h"
#include "./include/GMath.h"
class BitmapShader : public GShader {
    private:
        GBitmap bitmap;
        GMatrix localMatrix;
        GMatrix inverseCTM;
        GTileMode tileMode;
    public:
        BitmapShader(const GBitmap& bmp, const GMatrix& localM, GTileMode mode) : bitmap(bmp), localMatrix(localM), tileMode(mode){}

        bool isOpaque() override {
            return bitmap.isOpaque();
        }

        bool setContext(const GMatrix& ctm) override {
            auto inv = GMatrix::Concat(ctm, localMatrix).invert();
            if (inv.has_value()){
                inverseCTM = inv.value();
                return true;
            }else{
                return false;
            }
        }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        for (int i = 0; i < count; ++i) {
            GPoint local_point;
            GPoint device_point = {x + i + 0.5f, y + 0.5f};
            inverseCTM.mapPoints(&local_point, &device_point, 1);
            
            int ix = GFloorToInt(local_point.x);
            int iy = GFloorToInt(local_point.y);
            
            int clampedX = 0, clampedY = 0;

            // Handle tile modes
            switch (tileMode) {
                case GTileMode::kClamp:
                    clampedX = std::clamp(ix, 0, bitmap.width() - 1);
                    clampedY = std::clamp(iy, 0, bitmap.height() - 1);
                    break;

                case GTileMode::kRepeat:
                    clampedX = (ix % bitmap.width() + bitmap.width()) % bitmap.width();
                    clampedY = (iy % bitmap.height() + bitmap.height()) % bitmap.height();
                    break;

                case GTileMode::kMirror:
                    clampedX = (ix % (2 * bitmap.width()) + 2 * bitmap.width()) % (2 * bitmap.width());
                    if (clampedX >= bitmap.width()) {
                        clampedX = bitmap.width() - 1 - (clampedX - bitmap.width());
                    }
                    clampedY = (iy % (2 * bitmap.height()) + 2 * bitmap.height()) % (2 * bitmap.height());
                    if (clampedY >= bitmap.height()) {
                        clampedY = bitmap.height() - 1 - (clampedY - bitmap.height());
                    }
                    break;

                default:
                    // Handle unexpected tile modes if necessary
                    break;
            }

            row[i] = *bitmap.getAddr(clampedX, clampedY);
        }
    }

    GBitmap getBitmap(){
        return bitmap;
    }
};

std::shared_ptr<GShader> GCreateBitmapShader(const GBitmap& bmp, const GMatrix& localM, GTileMode tileMode) {
    return std::make_shared<BitmapShader>(bmp, localM, tileMode);
}
