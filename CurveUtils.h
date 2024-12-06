#include "./include/GPoint.h"
#include <array>
#include <vector>

void computeQuadBounds(const GPoint pts[3], GPoint bounds[2]);

GPoint evaluateQuadAt(const GPoint pts[3], float t);

GPoint bezierCubic(float t, const GPoint& P0, const GPoint& P1, const GPoint& P2, const GPoint& P3);

// Compute coefficients of the derivative polynomial
std::array<float, 3> computeCoefficients(const GPoint& P0, const GPoint& P1, const GPoint& P2, const GPoint& P3);
// Find roots of the derivative polynomial
std::vector<float> findRoots(float a, float b, float c);

// Find the maximum x and y values of the cubic Bézier curve extrema
void computeCubicBounds(const GPoint pts[4], GPoint extrema[2]);