
#include "entities/cell.h"
#include "world.h"

void World::draw(Cell &cell, QPainter &painter) {
    if(!cell.isAlive())
        return;

    int radius = cell.radius * scale;
    Vect worldPos = (cell + cameraPosition) * scale;
    painter.setPen(QPen(QBrush(Qt::black), 1));
    painter.setBrush(cell.color);
    painter.drawEllipse(QPoint(worldPos.x, worldPos.y), radius, radius);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::GlobalColor(Qt::blue));

    if(cell.type == ENGINE) {
        painter.setPen(QPen(QBrush(Qt::green), 1));
        auto pos1 = worldPos + Vect(cell.angle + 1) * cell.radius * 0.5 * scale,
             pos2 = worldPos + Vect(cell.angle) * cell.radius * 0.7 * scale,
             pos3 = worldPos + Vect(cell.angle - 1) * cell.radius * 0.5 * scale;
        painter.drawLine(pos1.x, pos1.y, pos2.x, pos2.y);
        painter.drawLine(pos2.x, pos2.y, pos3.x, pos3.y);
    }
    if(cell.type == EYE) {
        Vect eyePos = worldPos + Vect(cell.angle) * cell.radius * 0.5 * scale;
        painter.drawEllipse(eyePos.x, eyePos.y, (int)cell.radius * 0.25 * scale, (int)cell.radius * 0.25 * scale);
    }
}

void World::draw(Joint &joint, QPainter &painter) {
    if (!joint.isAlive())
        return;
    painter.setPen(QPen(QBrush(Qt::black), 1));
    auto pos1 = (*joint.cell1 + cameraPosition) * scale,
         pos2 = (*joint.cell2 + cameraPosition) * scale;
    painter.drawLine(pos1.x, pos1.y, pos2.x, pos2.y);
}
