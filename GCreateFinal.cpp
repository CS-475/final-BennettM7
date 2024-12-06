#include "GCreateFinal.h"

std::shared_ptr<GShader> BennettFinal::createVoronoiShader(const GPoint points[], const GColor colors[], int count){
    if (count < 1){
        return nullptr;
    }
    return std::make_shared<VoronoiShader>(points, colors, count);
}

std::shared_ptr<GShader> BennettFinal::createSweepGradient(GPoint center, float startRadians, const GColor colors[], int count){
    return nullptr;
}

std::shared_ptr<GShader> BennettFinal::createLinearPosGradient(GPoint p0, GPoint p1, const GColor colors[], const float pos[], int count){
    return nullptr;
}

std::shared_ptr<GShader> BennettFinal::createColorMatrixShader(const GColorMatrix&, GShader* realShader){
    return nullptr;
}

std::shared_ptr<GPath> BennettFinal::strokePolygon(const GPoint[], int count, float width, bool isClosed){
    return nullptr;
}

void BennettFinal::drawQuadraticCoons(GCanvas*, const GPoint pts[8], const GPoint tex[4], int level, const GPaint&){
    return;
}


std::unique_ptr<GFinal> GCreateFinal(){
    return std::make_unique<BennettFinal>();
}