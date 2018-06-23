
#include "entities/cell.h"

void Cell::draw(QPainter &painter) {
    if(!isAlive())
        return;

    int radius = getRadius() * world->scale;
    Vect worldPos = (pos + world->cameraPosition) * world->scale;
    painter.setPen(QPen(QBrush(Qt::black), 1));
    painter.setBrush(color);
    painter.drawEllipse(QPoint(worldPos.x, worldPos.y), radius, radius);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::GlobalColor(Qt::blue));

    if(type == ENGINE) {
        painter.setPen(QPen(QBrush(Qt::green), 1));
        auto pos1 = worldPos + Vect(angle + 1) * getRadius() * 0.5 * world->scale,
             pos2 = worldPos + Vect(angle) * getRadius() * 0.7 * world->scale,
             pos3 = worldPos + Vect(angle - 1) * getRadius() * 0.5 * world->scale;
        painter.drawLine(pos1.x, pos1.y, pos2.x, pos2.y);
        painter.drawLine(pos2.x, pos2.y, pos3.x, pos3.y);
    }
    if(type == EYE) {
        Vect eyePos = worldPos + Vect(angle) * getRadius() * 0.5 * world->scale;
        painter.drawEllipse(QPoint(eyePos.x, eyePos.y), (int)getRadius() * 0.25 * world->scale, (int)getRadius() * 0.25 * world->scale);
    }
}

void Joint::draw(QPainter &painter) {
    if (!isAlive())
        return;
    painter.setPen(QPen(QBrush(Qt::green), 1));
    auto &world = *cell1->world;
    auto pos1 = (cell1->pos + world.cameraPosition) * world.scale,
         pos2 = (cell2->pos + world.cameraPosition) * world.scale;
    painter.drawLine(pos1.x, pos1.y, pos2.x, pos2.y);
}
