#include "./include/GMath.h"
#include "./include/GPoint.h"
#include "./include/GMatrix.h"
#include "BennettUtils.h"

class Edge {
public:
    int yTop, yBottom;
    float slope;
    int winding;
    float b;

    Edge(const GPoint& p0, const GPoint& p1) {
        GPoint point0 = p0;
        GPoint point1 = p1;
        winding = (p0.y < p1.y) ? 1 : -1;
        if (point0.y > point1.y) {
            std::swap(point0, point1);
        }
        yTop = GRoundToInt(point0.y);
        yBottom = GRoundToInt(point1.y);
        slope = (point1.x - point0.x) / (point1.y - point0.y);
        b = p0.x - p0.y * slope;
    }

    float computeX(int y) const {
        return b + slope * (y  + 0.5f);
    }

    bool isValid(int y) const {
        return y >= yTop && y < yBottom;
    }
    
    static bool compareByYThenX(const Edge& a, const Edge& b) {
        if (a.yTop == b.yTop) {
            return a.computeX(a.yTop) < b.computeX(b.yTop);
        }
        return a.yTop < b.yTop;
    }

    int topY() const {
        return yTop;
    }

    int bottomY() const {
        return yBottom;
    }

    static bool compareByXForNextY(int nextY, const Edge& a, const Edge& b) {
        float xA = a.computeX(nextY);
        float xB = b.computeX(nextY);
        return xA < xB;
    }
};