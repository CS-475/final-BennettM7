#include "BennettCanvas.h"

BennettCanvas::BennettCanvas(const GBitmap& bmp) : bitmap(bmp), ctm() {}

void BennettCanvas::clear(const GColor& color){
    int a = calcA(color.a);
    int r = calcRGB(color.a, color.r);
    int g = calcRGB(color.a, color.g);
    int b = calcRGB(color.a, color.b);

    GPixel pixel = GPixel_PackARGB(a, r, g, b);

    int height = bitmap.height();
    int width = bitmap.width();

    for (int y = 0; y < height; y++){
        GPixel* addr = bitmap.getAddr(0, y);
        for(int x = 0; x < width; x++){
            addr[x] = pixel;
        }
    }
}

void BennettCanvas::drawRect(const GRect& rect, const GPaint& paint) {
    if(paint.getBlendMode() == GBlendMode::kDst) return;
    
    GPoint corners[4] = {
        {rect.left, rect.top},
        {rect.right, rect.top},
        {rect.right, rect.bottom},
        {rect.left, rect.bottom}
    };
    drawConvexPolygon(corners, 4, paint);
    return;
    GPoint transformedCorners[4];
    ctm.mapPoints(transformedCorners, corners, 4);

    int rect_x = std::max(0, GRoundToInt(transformedCorners[0].x));
    int rect_y = std::max(0, GRoundToInt(transformedCorners[0].y));
    int rect_right = std::min(bitmap.width(), GRoundToInt(transformedCorners[2].x));
    int rect_bottom = std::min(bitmap.height(), GRoundToInt(transformedCorners[2].y));

    if (rect_x >= rect_right || rect_y >= rect_bottom || rect.isEmpty()) {
        return;
    }
    GShader* fShader = paint.peekShader();
    /*  GRect rect;
        this->ctm().mapPoints((GPoint*)&rect, (const GPoint*)&srcRect, 2);
        if (auto shader = paint.peekShader()) {use shader}
    */
    switch(paint.getBlendMode()) {
        case GBlendMode::kClear: blendFunc = kClear; break;
        case GBlendMode::kDst: blendFunc = kDst; break;
        case GBlendMode::kSrc: blendFunc = erase; break;
        case GBlendMode::kSrcOver: blendFunc = srcOver; break;
        case GBlendMode::kDstOver: blendFunc = kDstOver; break;
        case GBlendMode::kDstIn: blendFunc = kDstIn; break;
        case GBlendMode::kSrcIn: blendFunc = kSrcIn; break;
        case GBlendMode::kDstOut: blendFunc = kDstOut; break;
        case GBlendMode::kSrcOut: blendFunc = kSrcOut; break;
        case GBlendMode::kDstATop: blendFunc = kDstATop; break;
        case GBlendMode::kSrcATop: blendFunc = kSrcATop; break;
        case GBlendMode::kXor: blendFunc = kXor; break;
    }

    GColor color = paint.getColor();
    int a = calcA(color.a);
    int r = calcRGB(color.a, color.r);
    int g = calcRGB(color.a, color.g);
    int b = calcRGB(color.a, color.b);

    if (fShader) {
        fShader->setContext(ctm);

        for (int y = rect_y; y < rect_bottom; y++){
            GPixel row[rect_right - rect_x];
            fShader->shadeRow(rect_x, y, rect_right - rect_x, row);
            

            GPixel* addr = bitmap.getAddr(0, y) + rect_x;
            for (int x = rect_x; x < rect_right; ++x) {
                *addr++ = blendFunc(*addr, GPixel_GetA(row[x - rect_x]), GPixel_GetR(row[x - rect_x]), GPixel_GetG(row[x - rect_x]), GPixel_GetB(row[x - rect_x]));
            }
        }
    }else{

        for (int y = rect_y; y < rect_bottom; y++){
            GPixel* addr = bitmap.getAddr(0, y) + rect_x;
            for (int x = rect_x; x < rect_right; x++){
                *addr++ = blendFunc(*addr, a, r, g, b);
            }
        }
    }
}

void BennettCanvas::drawConvexPolygon(const GPoint* points, int count, const GPaint& paint) {
    if (count < 3) return; 
    if (paint.getBlendMode() == GBlendMode::kDst) return;

    GPoint transformedPoints[count];
    ctm.mapPoints(transformedPoints, points, count);

    GShader* fShader = paint.peekShader();
    GColor color = paint.getColor();
    int a = calcA(color.a);
    int r = calcRGB(color.a, color.r);
    int g = calcRGB(color.a, color.g);
    int b = calcRGB(color.a, color.b);

    switch (paint.getBlendMode()) {
        case GBlendMode::kClear: blendFunc = kClear; break;
        case GBlendMode::kDst: blendFunc = kDst; break;
        case GBlendMode::kSrc: blendFunc = erase; break;
        case GBlendMode::kSrcOver: blendFunc = srcOver; break;
        case GBlendMode::kDstOver: blendFunc = kDstOver; break;
        case GBlendMode::kDstIn: blendFunc = kDstIn; break;
        case GBlendMode::kSrcIn: blendFunc = kSrcIn; break;
        case GBlendMode::kDstOut: blendFunc = kDstOut; break;
        case GBlendMode::kSrcOut: blendFunc = kSrcOut; break;
        case GBlendMode::kDstATop: blendFunc = kDstATop; break;
        case GBlendMode::kSrcATop: blendFunc = kSrcATop; break;
        case GBlendMode::kXor: blendFunc = kXor; break;
    }

    float minY = transformedPoints[0].y, maxY = transformedPoints[0].y;
    for (int i = 1; i < count; i++) {
        minY = std::min(minY, transformedPoints[i].y);
        maxY = std::max(maxY, transformedPoints[i].y);
    }

    int startY = std::max(0, GRoundToInt(minY));
    int endY = std::min(bitmap.height(), GRoundToInt(maxY));

    if (fShader) {
        fShader->setContext(ctm);
    }

    for (int y = startY; y < endY; ++y) {
        float xIntersections[count];
        int numIntersections = 0;

        for (int i = 0; i < count; ++i) {
            const GPoint& p0 = transformedPoints[i];
            const GPoint& p1 = transformedPoints[(i + 1) % count];
            int y0 = GRoundToInt(p0.y);
            int y1 = GRoundToInt(p1.y);
            if (y0 > y1) {
                std::swap(y0, y1);
            }

            if (y0 <= y && y1 > y) {
                float deltaX = p1.x - p0.x;
                float deltaY = p1.y - p0.y;
                float x = p0.x + (deltaX / deltaY) * (y + 0.5f - p0.y);
                xIntersections[numIntersections] = x;
                numIntersections++;
            }
        }

        if (numIntersections % 2 != 0) continue;

        std::sort(xIntersections, xIntersections + numIntersections);
        GPixel* addr = bitmap.getAddr(0, y);

        for (int i = 0; i < numIntersections; i += 2) {
            int xStart = std::max(0, GRoundToInt(xIntersections[i]));
            int xEnd = std::min(bitmap.width(), GRoundToInt(xIntersections[i + 1]));

            if (xStart <= xEnd) {
                if (fShader) {
                    GPixel row[xEnd - xStart];
                    fShader->shadeRow(xStart, y, xEnd - xStart, row);
                    
                    for (int x = xStart; x < xEnd; ++x) {
                        addr[x] = blendFunc(addr[x], GPixel_GetA(row[x - xStart]), GPixel_GetR(row[x - xStart]), GPixel_GetG(row[x - xStart]), GPixel_GetB(row[x - xStart]));
                    }
                } else {
                    for (int x = xStart; x < xEnd; ++x) {
                        addr[x] = blendFunc(addr[x], a, r, g, b);
                    }
                }
            }
        }
    }
}

void BennettCanvas::drawPath(const GPath& path, const GPaint& paint) {
    if (paint.getBlendMode() == GBlendMode::kDst) return;

    switch (paint.getBlendMode()) {
        case GBlendMode::kClear: blendFunc = kClear; break;
        case GBlendMode::kDst: blendFunc = kDst; break;
        case GBlendMode::kSrc: blendFunc = erase; break;
        case GBlendMode::kSrcOver: blendFunc = srcOver; break;
        case GBlendMode::kDstOver: blendFunc = kDstOver; break;
        case GBlendMode::kDstIn: blendFunc = kDstIn; break;
        case GBlendMode::kSrcIn: blendFunc = kSrcIn; break;
        case GBlendMode::kDstOut: blendFunc = kDstOut; break;
        case GBlendMode::kSrcOut: blendFunc = kSrcOut; break;
        case GBlendMode::kDstATop: blendFunc = kDstATop; break;
        case GBlendMode::kSrcATop: blendFunc = kSrcATop; break;
        case GBlendMode::kXor: blendFunc = kXor; break;
    }

    GShader* fShader = paint.peekShader();
    if (fShader) fShader->setContext(ctm);

    GPath::Edger edger(path); 
    GPoint pts[GPath::kMaxNextPoints];
    std::vector<Edge> edges;

    GPoint previousPoint = {0, 0};
    while (auto verb = edger.next(pts)) {
        if (verb.value() == GPathVerb::kLine) {
            Edge edge(ctm * pts[0], ctm * pts[1]);
            if (edge.topY() == edge.bottomY()) {
                continue;
            }
            edges.push_back(edge);
        } else if (verb.value() == GPathVerb::kQuad) {
            for (int i = 0; i <= 40; ++i) { // Create 8 segments for smoother curves
                float t = i / 40.0f;
                GPoint pt = evaluateQuadAt(pts, t);
                if (i > 0) {
                    Edge edge(ctm * previousPoint, ctm * pt);
                    if (edge.topY() != edge.bottomY()) {
                        edges.push_back(edge);
                    }
                }
                previousPoint = pt;
            }
        } else if (verb.value() == GPathVerb::kCubic) {
            for (int i = 0; i <= 40; ++i) { // Create 8 segments for smoother curves
                float t = i / 40.0f;
                GPoint pt = bezierCubic(t, pts[0], pts[1], pts[2], pts[3]);
                if (i > 0) {
                    Edge edge(ctm * previousPoint, ctm * pt);
                    if (edge.topY() != edge.bottomY()) {
                        edges.push_back(edge);
                    }
                }
                previousPoint = pt;
            }
        }
    }

    std::sort(edges.begin(), edges.end(), Edge::compareByYThenX);
    if(edges.empty()){
        return;
    }
    int yMin = std::max(0, edges.front().topY());
    int yMax = 0;
    for(Edge e : edges){
        yMax = std::max(yMax, e.bottomY());
    }
    if(yMax > bitmap.height()){
        yMax = bitmap.height();
    }
    for (int y = yMin; y < yMax; ++y) {
        int w = 0;
        int L;
        size_t i = 0;

        while (i < edges.size() && edges[i].isValid(y)) {
            int x = std::max(0, std::min(GRoundToInt(edges[i].computeX(y)), bitmap.width() - 1));

            if (w == 0) L = x;
            
            w += edges[i].winding;
            

            if (w == 0) {
                int R = x;
                blitSpan(L, y, R - L, paint, fShader);
            }

            if (!edges[i].isValid(y + 1)) {
                edges.erase(edges.begin() + i);
            } else {
                ++i;
            }
        }
        assert(w == 0);
        while (i < edges.size() && edges[i].isValid(y+1)) {
            i += 1;
        }
        std::sort(edges.begin(), edges.begin() + i,
            [y](const Edge& a, const Edge& b) {
                return Edge::compareByXForNextY(y + 1, a, b);
            });
    }
}


void BennettCanvas::save(){
    ctmStack.push(ctm);
}

void BennettCanvas::restore(){
    if (ctmStack.empty()){
        return;
    }
    ctm = ctmStack.top();
    ctmStack.pop();
}

void BennettCanvas::concat(const GMatrix& matrix){
    ctm = GMatrix::Concat(ctm, matrix);
}

std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap& bitmap) {
    return std::make_unique<BennettCanvas>(bitmap);
}

std::string GDrawSomething(GCanvas* canvas, GISize dim) {
    
    canvas->clear(GColor::RGBA(0.0f, 0.0f, 0.0f, 0.0f));

    canvas->fillRect(GRect::XYWH(5.0f, 5.0f, 100.0f, 100.0f), GColor::RGBA(1.0f, 0.0f, 0.0f, 1.0f));

    return "Red Square";
}

void BennettCanvas::blitSpan(int L, int y, int spanWidth, const GPaint& paint, GShader* shader) {
    GPixel* addr = bitmap.getAddr(L, y);
    if (shader) {
        GPixel row[spanWidth];
        shader->shadeRow(L, y, spanWidth, row);
        for (int i = 0; i < spanWidth; ++i) {
            addr[i] = blendFunc(addr[i], GPixel_GetA(row[i]), GPixel_GetR(row[i]),
                                GPixel_GetG(row[i]), GPixel_GetB(row[i]));
        }
    } else {
        GColor color = paint.getColor();
        int a = calcA(color.a);
        int r = calcRGB(color.a, color.r);
        int g = calcRGB(color.a, color.g);
        int b = calcRGB(color.a, color.b);

        for (int i = 0; i < spanWidth; ++i) {
            addr[i] = blendFunc(addr[i], a, r, g, b);
        }
    }
}

void BennettCanvas::drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[],
                              int count, const int indices[], const GPaint& paint) {
    if (count <= 0 || paint.getBlendMode() == GBlendMode::kDst) return;
    GPaint p = paint;
    std::shared_ptr<GShader> sh;

    for (int i = 0; i < count; i++){
        int index0 = indices[3 * i];
        int index1 = indices[3 * i + 1];
        int index2 = indices[3 * i + 2];

        GPoint poly[] = {verts[index0], verts[index1], verts[index2]};
        
        if(colors != nullptr && texs == nullptr){
            sh = interpolateColor(verts[index0], verts[index1], verts[index2], colors[index0], colors[index1], colors[index2]);
            
            p.setShader(sh);
            this->drawConvexPolygon(poly, 3, p);
        }else if(colors == nullptr && texs != nullptr){
            GPoint tex0 = texs[index0];
            GPoint tex1 = texs[index1];
            GPoint tex2 = texs[index2];

            GMatrix P = compute_basis(poly[0], poly[1], poly[2]);
            GMatrix T = compute_basis(tex0, tex1, tex2);
            auto invT = T.invert();                                  

            if (invT) {
                GMatrix extraTransform = P * *invT;

                ProxyShader proxyShader(paint.peekShader(), extraTransform);
                p.setShader(std::make_shared<ProxyShader>(proxyShader));

                this->drawConvexPolygon(poly, 3, p);
            }
        }else{
            auto colorShader = std::make_shared<BarycentricColorShader>(
                verts[index0], verts[index1], verts[index2],
                colors[index0], colors[index1], colors[index2]);

            // Create ProxyShader for texture
            GPoint tex0 = texs[index0];
            GPoint tex1 = texs[index1];
            GPoint tex2 = texs[index2];

            GMatrix P = compute_basis(poly[0], poly[1], poly[2]);
            GMatrix T = compute_basis(tex0, tex1, tex2);
            auto invT = T.invert();

            if (invT) {
                GMatrix extraTransform = P * *invT;
                auto textureShader = std::make_shared<ProxyShader>(paint.peekShader(), extraTransform);

                // Combine the two shaders
                auto composShader = std::make_shared<ComposShader>(colorShader, textureShader);
                p.setShader(composShader);

                this->drawConvexPolygon(poly, 3, p);
            }
        }
    }
}

void BennettCanvas::drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4],
              int level, const GPaint& paint) {
    int subdivisions = level * 10;
    float step = 1.0f / subdivisions;

    std::vector<GPoint> tessVerts;
    std::vector<GColor> tessColors;
    std::vector<GPoint> tessTexs;

    for (int row = 0; row <= subdivisions; ++row) {
        for (int col = 0; col <= subdivisions; ++col) {
            float u = col * step;
            float v = row * step;

            GPoint point = verts[0] * (1 - u) * (1 - v) +
                           verts[1] * u * (1 - v) +
                           verts[2] * u * v +
                           verts[3] * (1 - u) * v;
            tessVerts.push_back(point);

            if (colors) {
                GColor color = colors[0] * (1 - u) * (1 - v) +
                               colors[1] * u * (1 - v) +
                               colors[2] * u * v +
                               colors[3] * (1 - u) * v;
                tessColors.push_back(color);
            }

            if (texs) {
                GPoint tex = texs[0] * (1 - u) * (1 - v) +
                             texs[1] * u * (1 - v) +
                             texs[2] * u * v +
                             texs[3] * (1 - u) * v;
                tessTexs.push_back(tex);
            }
        }
    }

    std::vector<int> indices;
    for (int row = 0; row < subdivisions; ++row) {
        for (int col = 0; col < subdivisions; ++col) {
            int idx0 = row * (subdivisions + 1) + col;
            int idx1 = idx0 + 1;
            int idx2 = idx0 + (subdivisions + 1);
            int idx3 = idx2 + 1;

            indices.push_back(idx0); indices.push_back(idx1); indices.push_back(idx3);
            indices.push_back(idx0); indices.push_back(idx3); indices.push_back(idx2);
        }
    }

    drawMesh(tessVerts.data(),
             colors ? tessColors.data() : nullptr,
             texs ? tessTexs.data() : nullptr,
             indices.size() / 3,
             indices.data(),
             paint);
}
