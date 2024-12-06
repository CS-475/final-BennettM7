#include "CurveUtils.h"

void computeQuadBounds(const GPoint pts[3], GPoint bounds[2]) {
    float minX = std::min(pts[0].x, pts[2].x);
    float minY = std::min(pts[0].y, pts[2].y);
    float maxX = std::max(pts[0].x, pts[2].x);
    float maxY = std::max(pts[0].y, pts[2].y);

    // Solve for extrema in x and y
    float tX = (pts[0].x - pts[1].x) / (pts[0].x - 2 * pts[1].x + pts[2].x);
    float tY = (pts[0].y - pts[1].y) / (pts[0].y - 2 * pts[1].y + pts[2].y);

    if (tX > 0 && tX < 1) {
        GPoint extremumX = evaluateQuadAt(pts, tX);
        minX = std::min(minX, extremumX.x);
        maxX = std::max(maxX, extremumX.x);
    }
    if (tY > 0 && tY < 1) {
        GPoint extremumY = evaluateQuadAt(pts, tY);
        minY = std::min(minY, extremumY.y);
        maxY = std::max(maxY, extremumY.y);
    }

    bounds[0] = {minX, minY};
    bounds[1] = {maxX, maxY};
}

GPoint evaluateQuadAt(const GPoint pts[3], float t){
    float omt = 1 - t; // o.m.t = one minus t

    float x = omt * omt * pts[0].x + 2 * omt * t * pts[1].x + t * t * pts[2].x;
    float y = omt * omt * pts[0].y + 2 * omt * t * pts[1].y + t * t * pts[2].y;

    return {x, y};
}

GPoint bezierCubic(float t, const GPoint& P0, const GPoint& P1, const GPoint& P2, const GPoint& P3) {
    GPoint point;
    float omt = 1 - t;
    point.x = omt * omt * omt * P0.x +
              3 * omt * omt * t * P1.x +
              3 * omt * t * t * P2.x +
              t * t * t * P3.x;

    point.y = omt * omt * omt * P0.y +
              3 * omt * omt * t * P1.y +
              3 * omt * t * t * P2.y +
              t * t * t * P3.y;

    return point;
}

// Compute coefficients of the derivative polynomial
std::array<float, 3> computeCoefficientsX(const GPoint& P0, const GPoint& P1, const GPoint& P2, const GPoint& P3) {
    float a = -3 * P0.x + 9 * P1.x - 9 * P2.x + 3 * P3.x;
    float b = 6 * P0.x - 12 * P1.x + 6 * P2.x;
    float c = -3 * P0.x + 3 * P1.x;
    return { a, b, c };
}

std::array<float, 3> computeCoefficientsY(const GPoint& P0, const GPoint& P1, const GPoint& P2, const GPoint& P3) {
    float a = -3 * P0.y + 9 * P1.y - 9 * P2.y + 3 * P3.y;
    float b = 6 * P0.y - 12 * P1.y + 6 * P2.y;
    float c = -3 * P0.y + 3 * P1.y;
    return { a, b, c };
}

// Find roots of the derivative polynomial
void findRoots(float a, float b, float c, std::vector<float>& roots) {
    if (a == 0) {
        // Linear case: bx + c = 0
        if (b != 0) {
            float root = -c / b;
            if (root >= 0 && root <= 1) {
                roots.push_back(root);
            }
        }
    } else {
        // Quadratic case: ax^2 + bx + c = 0
        float discriminant = b * b - 4 * a * c;
        if (discriminant >= 0) {
            float root1 = (-b + sqrt(discriminant)) / (2 * a);
            float root2 = (-b - sqrt(discriminant)) / (2 * a);
            if (root1 >= 0 && root1 <= 1) roots.push_back(root1);
            if (root2 >= 0 && root2 <= 1) roots.push_back(root2);
        }
    }
}

void computeCubicBounds(const GPoint pts[4], GPoint extrema[2]) {
    auto coeffs = computeCoefficientsX(pts[0], pts[1], pts[2], pts[3]);
    float aX = coeffs[0];
    float bX = coeffs[1];
    float cX = coeffs[2];

    coeffs = computeCoefficientsY(pts[0], pts[1], pts[2], pts[3]);
    float aY = coeffs[0];
    float bY = coeffs[1];
    float cY = coeffs[2];

    std::vector<float> roots; 
    findRoots(aX, bX, cX, roots); 
    findRoots(aY, bY, cY, roots);

    

    float minX = pts[0].x;
    float minY = pts[0].y;
    float maxX = pts[0].x;
    float maxY = pts[0].y;

    // Evaluate the cubic Bezier at each root found
    for (float t : roots) {
        GPoint extremumPoint = bezierCubic(t, pts[0], pts[1], pts[2], pts[3]);
        minX = std::min(minX, extremumPoint.x);
        minY = std::min(minY, extremumPoint.y);
        maxX = std::max(maxX, extremumPoint.x);
        maxY = std::max(maxY, extremumPoint.y);
    }
    

    // Store the maximum x and y extrema points
    extrema[0] = {minX, minY}; // Store minimum (minX and minY)
    extrema[1] = {maxX, maxY}; // Store maximum (maxX and maxY)
}