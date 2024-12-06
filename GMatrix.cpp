#include "./include/GMatrix.h"


GMatrix::GMatrix(){
    fMat[0] = 1; fMat[1] = 0; fMat[2] = 0;
    fMat[3] = 1; fMat[4] = 0; fMat[5] = 0;
}


GMatrix GMatrix::Translate(float tx, float ty){
    return GMatrix(1, 0, tx, 0, 1, ty);
}

GMatrix GMatrix::Scale(float sx, float sy){
    return GMatrix(sx, 0, 0, 0, sy, 0);
}

GMatrix GMatrix::Rotate(float radians){
    float cosVal = std::cos(radians);
    float sinVal = std::sin(radians);

    return GMatrix(cosVal, -sinVal, 0, sinVal, cosVal, 0);
}

GMatrix GMatrix::Concat(const GMatrix& a, const GMatrix& b){
    return GMatrix(
        a[0] * b[0] + a[2] * b[1],
        a[0] * b[2] + a[2] * b[3],
        a[0] * b[4] + a[2] * b[5] + a[4],
        a[1] * b[0] + a[3] * b[1],
        a[1] * b[2] + a[3] * b[3],
        a[1] * b[4] + a[3] * b[5] + a[5]
    );
}

nonstd::optional<GMatrix> GMatrix::invert() const {
    float det = fMat[0] * fMat[3] - fMat[1] * fMat[2];

    if(det == 0){
        return {};
    }

    float invDet = 1 / det;

    return GMatrix(
        fMat[3] * invDet, -fMat[2] * invDet, (fMat[2] * fMat[5] - fMat[4] * fMat[3]) * invDet,
        -fMat[1] * invDet, fMat[0] * invDet, (fMat[4] * fMat[1] - fMat[0] * fMat[5]) * invDet
    );
}

void GMatrix::mapPoints(GPoint dst[], const GPoint src[], int count) const {
    for (int i = 0; i < count; ++i) {
        const GPoint& s = src[i];

        float newX = fMat[0] * s.x + fMat[2] * s.y + fMat[4];
        float newY = fMat[1] * s.x + fMat[3] * s.y + fMat[5];

        dst[i].x = newX;
        dst[i].y = newY;
    }
}