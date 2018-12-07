#include "rendering_command_list.h"

#include "ukive/graphics/geometry.h"


namespace ukive {

    void RenderingCommandList::save() {
        Command cmd;
        cmd.op = Operation::SAVE;
        cmds_.push_back(std::move(cmd));
    }

    void RenderingCommandList::restore() {
        Command cmd;
        cmd.op = Operation::RESTORE;
        cmds_.push_back(std::move(cmd));
    }

    void RenderingCommandList::scale(float sx, float sy) {
        Matrix m;
        m.postScale(sx, sy);

        Command cmd;
        cmd.op = Operation::MATRIX;
        cmd.matrix = m;
        cmds_.push_back(std::move(cmd));
    }

    void RenderingCommandList::scale(float sx, float sy, float cx, float cy) {
        Matrix m;
        m.postScale(sx, sy, cx, cy);

        Command cmd;
        cmd.op = Operation::MATRIX;
        cmd.matrix = m;
        cmds_.push_back(std::move(cmd));
    }

    void RenderingCommandList::rotate(float angle) {
        Matrix m;
        m.postRotate(angle);

        Command cmd;
        cmd.op = Operation::MATRIX;
        cmd.matrix = m;
        cmds_.push_back(std::move(cmd));
    }

    void RenderingCommandList::rotate(float angle, float cx, float cy) {
        Matrix m;
        m.postRotate(angle, cx, cy);

        Command cmd;
        cmd.op = Operation::MATRIX;
        cmd.matrix = m;
        cmds_.push_back(std::move(cmd));
    }

    void RenderingCommandList::translate(float dx, float dy) {
        Matrix m;
        m.postTranslate(dx, dy);

        Command cmd;
        cmd.op = Operation::MATRIX;
        cmd.matrix = m;
        cmds_.push_back(std::move(cmd));
    }

    void RenderingCommandList::setMatrix(const Matrix& matrix) {
        Command cmd;
        cmd.op = Operation::MATRIX;
        cmd.matrix = matrix;
        cmds_.push_back(std::move(cmd));
    }

    void RenderingCommandList::pushClip(const RectF& rect) {
        Command cmd;
        cmd.op = Operation::PUSH_CLIP;
        cmd.geo = RectGeometry::create(rect);
        cmds_.push_back(std::move(cmd));
    }

    void RenderingCommandList::popClip() {
        Command cmd;
        cmd.op = Operation::POP_CLIP;
        cmds_.push_back(std::move(cmd));
    }

    void RenderingCommandList::clear() {
        Command cmd;
        cmd.op = Operation::CLEAR;
        cmd.paint.setColor(Color::Transparent);
        cmds_.push_back(std::move(cmd));
    }

    void RenderingCommandList::clear(const Color& c) {
        Command cmd;
        cmd.op = Operation::CLEAR;
        cmd.paint.setColor(c);
        cmds_.push_back(std::move(cmd));
    }

    void RenderingCommandList::drawLine(
        const PointF& start, const PointF& end, const Paint& p) {

        Command cmd;
        cmd.op = Operation::DRAW_GEOMETRY;
        cmd.geo = LineGeometry::create(start, end);
        cmd.paint = p;
        cmds_.push_back(std::move(cmd));
    }

    void RenderingCommandList::drawRect(const RectF& rect, const Paint& p) {
        Command cmd;
        cmd.op = Operation::DRAW_GEOMETRY;
        cmd.geo = RectGeometry::create(rect);
        cmd.paint = p;
        cmds_.push_back(std::move(cmd));
    }

    void RenderingCommandList::drawRoundRect(
        const RectF& rect, float radius, const Paint& p) {

        Command cmd;
        cmd.op = Operation::DRAW_GEOMETRY;
        cmd.geo = RoundRectGeometry::create(rect, radius, radius);
        cmd.paint = p;
        cmds_.push_back(std::move(cmd));
    }

    void RenderingCommandList::drawCircle(
        float cx, float cy, float radius, const Paint& p) {
        drawOval(cx, cy, radius, radius, p);
    }

    void RenderingCommandList::drawOval(
        float cx, float cy, float rx, float ry, const Paint& p) {

        Command cmd;
        cmd.op = Operation::DRAW_GEOMETRY;
        cmd.geo = OvalGeometry::create(cx, cy, rx, ry);
        cmd.paint = p;
        cmds_.push_back(std::move(cmd));
    }

    void RenderingCommandList::drawGeometry(Geometry* geo, const Paint& p) {
        Command cmd;
        cmd.op = Operation::DRAW_GEOMETRY;
        cmd.geo = geo;
        cmd.paint = p;
        cmds_.push_back(std::move(cmd));
    }

    void RenderingCommandList::drawBitmap(Bitmap* bitmap) {
        drawBitmap(0, 0, bitmap);
    }

    void RenderingCommandList::drawBitmap(float x, float y, Bitmap* bitmap) {

    }

    void RenderingCommandList::drawBitmap(float opacity, Bitmap* bitmap) {

    }

    void RenderingCommandList::drawBitmap(
        const RectF& dst, float opacity, Bitmap* bitmap) {
        drawBitmap(RectF(), dst, opacity, bitmap);
    }

    void RenderingCommandList::drawBitmap(
        const RectF& src, const RectF& dst, float opacity, Bitmap* bitmap) {

        Command cmd;
        cmd.op = Operation::DRAW_BITMAP;
        cmd.bitmap = bitmap;
        cmds_.push_back(std::move(cmd));
    }

    void RenderingCommandList::drawText(
        const string16& text, const RectF& rect, const Paint& p) {

        Command cmd;
        cmd.op = Operation::DRAW_TEXT;
        cmd.text = text;
        cmd.paint = p;
        cmds_.push_back(std::move(cmd));
    }

}