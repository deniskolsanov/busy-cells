#ifndef VECT_H
#define VECT_H

#include <math.h>
using namespace std;

/*
 2D vector algebra implementation
 Denis Kolsanov, 2018
*/

class Vect {
public:
    double x = 0, y = 0;
    Vect() {}
    Vect(double x, double y) : x(x), y(y) {}
    Vect(double angle) {
        x = cos(angle);
        y = sin(angle);
    }
    Vect add(Vect &vect) {
        x += vect.x;
        y += vect.y;
        return *this;
    }
    Vect sub(Vect &vect) {
        x -= vect.x;
        y -= vect.y;
        return *this;
    }
    Vect add(double px, double py) {
        x += px;
        y += py;
        return *this;
    }
    Vect sub(double px, double py) {
        x -= px;
        y -= py;
        return *this;
    }
    Vect mul(double scale) {
        x *= scale;
        y *= scale;
        return *this;
    }
    double size() {
        return sqrt(x*x + y*y);
    }
    Vect setSize(double scale) {
        double cur = size();
        if(cur > 0) {
            x *= scale / cur;
            y *= scale / cur;
        } else {
            x = scale;
        }
        return *this;
    }
    double getAngle() {
        return atan2(y, x);
    }
    void setAngle(double angle) {
        double sz = size();
        x = sz * cos(angle);
        y = sz * sin(angle);
    }
    void rotate(double angle) {
        angle += atan2(y, x);
        double sz = size();
        x = sz * cos(angle);
        y = sz * sin(angle);
    }
    void rotateWithCenter(Vect &vect, double angle) {
        angle += atan2(y - vect.y, x - vect.x);
        double sz = sqrt((x - vect.x)*(x - vect.x) + (y - vect.y)*(y - vect.y));
        x = sz * cos(angle) + vect.x;
        y = sz * sin(angle) + vect.y;
    }
    double distTo(const Vect &vect) {
        return sqrt((x - vect.x) * (x - vect.x) +
                    (y - vect.y) * (y - vect.y));
    }
    double distTo(double px, double py) {
        return sqrt((x - px) * (x - px) +
                    (y - py) * (y - py));
    }
    double dist2To(const Vect &vect) {
        return (x - vect.x) * (x - vect.x) +
                (y - vect.y) * (y - vect.y);
    }
    double dist2To(double px, double py) {
        return (x - px) * (x - px) +
                (y - py) * (y - py);
    }
    double angleTo(const Vect &vect) {
        return atan2(vect.y - y, vect.x - x);
    }
    Vect norm() {
        double len = size();
        if(len > 0) {
            x /= len;
            y /= len;
        } else {
            x = 0;
            y = 0;
        }
        return *this;
    }
    Vect copy() {
        return *this;
    }
    void moveTo(Vect &vect, double len) {
        double dx = vect.x - x,
               dy = vect.y - y;
        double sqDist = dx * dx + dy * dy;
        if(sqDist < len * len) {
            x = vect.x;
            y = vect.y;
        } else {
            double k = len / sqrt(sqDist);
            x += dx * k;
            y += dy * k;
        }
    }
    bool aabb(double px, double py, double tx, double ty) {
        return (x >= px && y >= py && x <= tx && y <= ty);
    }
    Vect angleToVect(double angle);

    Vect& operator+= (const Vect &b) {
        x += b.x;
        y += b.y;
        return *this;
    }
    Vect& operator-= (const Vect &b) {
        x -= b.x;
        y -= b.y;
        return *this;
    }
    Vect operator+ (const Vect &b) {
        return Vect(x + b.x, y + b.y);
    }
    Vect operator- (const Vect &b) {
        return Vect(x - b.x, y - b.y);
    }
    Vect operator* (const double &b) {
        return Vect(x * b, y * b);
    }
    double operator* (const Vect &b) {
        return x * b.x + y * b.y;
    }
    Vect operator/ (const double &b) {
        return Vect(x / b, y / b);
    }
    bool operator== (const Vect &b) {
        return pow(x - b.x, 2) +
                pow(y - b.y, 2) < 1e-9;
    }
    bool operator!= (const Vect &b) {
        return !(pow(x - b.x, 2) +
                pow(y - b.y, 2) < 1e-9);
    }
    Vect& operator*= (const double &b) {
        x *= b;
        y *= b;
        return *this;
    }
    Vect& operator/= (const double &b) {
        if(b) {
            x /= b;
            y /= b;
        }
        return *this;
    }
    void set(const double &px, const double &py) {
        x = px;
        y = py;
    }
    void set(const Vect &a) {
        x = a.x;
        y = a.y;
    }
};

#endif // VECT_H
