#include "./include/GShader.h"
#include "./include/GMatrix.h"
#include "./include/GPixel.h"
#include "BennettUtils.h"
#include <vector>
#include <memory>
#include <cmath>


class VoronoiShader : public GShader{
    private:
        std::vector<GPoint> pts;
        std::vector<GColor> colors;
        int color_count;
        GMatrix inverseCTM;

        GColor findClosestPointColor(GPoint point){
            int closest_color = 0;
            float curr_distance = sqrt(((pts[0].x - point.x) * (pts[0].x - point.x)) + ((pts[0].y - point.y) * (pts[0].y - point.y)));
            for(int i = 0; i < color_count; i++){
                float dist = sqrt(((pts[i].x - point.x) * (pts[i].x - point.x)) + ((pts[i].y - point.y) * (pts[i].y - point.y)));
                if (curr_distance > dist){
                    curr_distance = dist;
                    closest_color = i;
                }
            }
            return colors[closest_color];
        }
    public:
        VoronoiShader(const GPoint* pts, const GColor* colors, int count)
            : pts(pts, pts + count), colors(colors, colors + count), color_count(count){}
        
        bool isOpaque() override{
            int o_count = 0;
            for (int i = 0; i < color_count; i++){
                if(colors[i].a == 0){
                    o_count++;
                }
            }
            return o_count == color_count;
        }

        bool setContext(const GMatrix& ctm) override{
            auto inv = ctm.invert();
            if (inv.has_value()) {
                inverseCTM = inv.value();
                return true;
            }
            return false;
        }

        void shadeRow(int x, int y, int count, GPixel row[]) override{
            for (int i = 0; i < count; i++){
                GPoint local_point;
                GPoint device_point = {x + i + 0.5f, y + 0.5f};
                inverseCTM.mapPoints(&local_point, &device_point, 1);
                GColor closest_color = findClosestPointColor(local_point);
                row[i] = GPixel_PackARGB(
                    calcA(closest_color.a),
                    calcRGB(closest_color.a, closest_color.r),
                    calcRGB(closest_color.a, closest_color.g),
                    calcRGB(closest_color.a, closest_color.b)
                );
            }
        }
};