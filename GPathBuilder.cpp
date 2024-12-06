#include "./include/GPathBuilder.h"
#include "./include/GPath.h"
#include "CurveUtils.h"

void GPathBuilder::addRect(const GRect& rect, GPathDirection dir){
    GPoint topLeft = {rect.left, rect.top};
    GPoint topRight = {rect.right, rect.top};
    GPoint bottomRight = {rect.right, rect.bottom};
    GPoint bottomLeft = {rect.left, rect.bottom};

    this->moveTo(topLeft);

    if (dir == GPathDirection::kCW) {
        this->lineTo(topRight);
        this->lineTo(bottomRight);
        this->lineTo(bottomLeft);
    } else {
        this->lineTo(bottomLeft);
        this->lineTo(bottomRight);
        this->lineTo(topRight);
    }
    
    this->lineTo(topLeft);
}

void GPathBuilder::addPolygon(const GPoint pts[], int count) {
    if (count < 2) {
        return;
    }

    this->moveTo(pts[0]);

    for (int i = 1; i < count; ++i) {
        this->lineTo(pts[i]);
    }
}

GRect GPath::bounds() const {
    if (fPts.empty()) {
        return GRect::LTRB(0, 0, 0, 0);
    }

    float minX = fPts[0].x;
    float minY = fPts[0].y;
    float maxX = fPts[0].x;
    float maxY = fPts[0].y;

    GPoint pts[GPath::kMaxNextPoints];
    Iter iter(*this);
    while (auto verb = iter.next(pts)) {
        if (*verb == GPathVerb::kLine) {
            // Update bounds based on line endpoints
            minX = std::min({minX, pts[0].x, pts[1].x});
            minY = std::min({minY, pts[0].y, pts[1].y});
            maxX = std::max({maxX, pts[0].x, pts[1].x});
            maxY = std::max({maxY, pts[0].y, pts[1].y});
        } else if (*verb == GPathVerb::kQuad) {
            // Get bounds for quadratic Bezier curve
            GPoint quadBounds[2];
            computeQuadBounds(pts, quadBounds);
            minX = std::min({minX, quadBounds[0].x, quadBounds[1].x});
            minY = std::min({minY, quadBounds[0].y, quadBounds[1].y});
            maxX = std::max({maxX, quadBounds[0].x, quadBounds[1].x});
            maxY = std::max({maxY, quadBounds[0].y, quadBounds[1].y});
        } else if (*verb == GPathVerb::kCubic) {
            // Get bounds for cubic Bezier curve
            GPoint cubicBounds[2];
            computeCubicBounds(pts, cubicBounds);
            minX = std::min({minX, cubicBounds[0].x, cubicBounds[1].x});
            minY = std::min({minY, cubicBounds[0].y, cubicBounds[1].y});
            maxX = std::max({maxX, cubicBounds[0].x, cubicBounds[1].x});
            maxY = std::max({maxY, cubicBounds[0].y, cubicBounds[1].y});
        }
    }

    return GRect::LTRB(minX, minY, maxX, maxY);
}



void GPathBuilder::addCircle(GPoint center, float radius, GPathDirection dir) {
    float c = 0.55228475f * radius;
    
    GPoint points[13] = {
        {center.x, center.y - radius}, {center.x + c, center.y - radius}, {center.x + radius, center.y - c},
        {center.x + radius, center.y}, {center.x + radius, center.y + c}, {center.x + c, center.y + radius},
        {center.x, center.y + radius}, {center.x - c, center.y + radius}, {center.x - radius, center.y + c},
        {center.x - radius, center.y}, {center.x - radius, center.y - c}, {center.x - c, center.y - radius},
        {center.x, center.y - radius}
    };

    this->moveTo(points[0]);

    if (dir == GPathDirection::kCW) {
        for (int i = 1; i < 12; i += 3) {
            this->quadTo(points[i], points[i + 1]);
            this->lineTo(points[i + 2]);
        }
    } else {
        for (int i = 11; i >= 0; i -= 3) {
            this->quadTo(points[i], points[i - 1]);
            this->lineTo(points[i - 2]);
        }
    }
}

void GPath::ChopQuadAt(const GPoint src[3], GPoint dst[5], float t) {
    GPoint p01 = src[0] + (src[1] - src[0]) * t;
    GPoint p12 = src[1] + (src[2] - src[1]) * t;
    GPoint p012 = p01 + (p12 - p01) * t;

    dst[0] = src[0];
    dst[1] = p01;
    dst[2] = p012;
    dst[3] = p12;
    dst[4] = src[2];
}

void GPath::ChopCubicAt(const GPoint src[4], GPoint dst[7], float t) {
    GPoint p01 = src[0] + (src[1] - src[0]) * t;
    GPoint p12 = src[1] + (src[2] - src[1]) * t;
    GPoint p23 = src[2] + (src[3] - src[2]) * t;
    GPoint p012 = p01 + (p12 - p01) * t;
    GPoint p123 = p12 + (p23 - p12) * t;
    GPoint p0123 = p012 + (p123 - p012) * t;

    dst[0] = src[0];
    dst[1] = p01;
    dst[2] = p012;
    dst[3] = p0123;
    dst[4] = p123;
    dst[5] = p23;
    dst[6] = src[3];
}