#ifndef VECT_H
#define VECT_H

#include <math.h>
#include <algorithm>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
using namespace std;

/*
 2D vector algebra implementation
 Denis Kolsanov, 2018
*/

#if __cplusplus <= 201402L
/*template<typename T> T clamp(T a, T from, T to) {
    return max(from, min(a, to));
}*/
#endif

template<typename T> T normAngle(T x){
    x = fmod(x + M_PI, M_PI * 2);
    if (x < 0)
        return x + M_PI;
    else
        return x - M_PI;
}

template<typename T> T randRange(T fMin, T fMax) {
    return fMin + static_cast<T> (rand()) * (fMax - fMin) / RAND_MAX;
}

template<typename T> constexpr T square(const T a) {
    return a * a;
}

namespace Denis {

template<typename T> struct Vect {
public:
    // Constructors
    T x = 0, y = 0;
    constexpr inline Vect() {}
    constexpr inline Vect(const T x, const T y) : x(x), y(y) {}
    inline Vect(const T angle) {
        x = cos(angle);
        y = sin(angle);
    }

    // Const methods
    constexpr inline T size2() const {
        return x*x + y*y;
    }
    inline T size() const {
        return sqrt(size2());
    }
    inline T getAngle() const {
        return atan2(y, x);
    }
    constexpr inline T dist2To(const T px, const T py) const {
        return (x - px) * (x - px) +
                (y - py) * (y - py);
    }
    constexpr inline T dist2To(const Vect &vect) const {
        return dist2To(vect.x, vect.y);
    }
    inline T distTo(const Vect &vect) const {
        return sqrt(dist2To(vect));
    }
    inline T distTo(const T px, const T py) const {
        return sqrt(dist2To(px, py));
    }
    constexpr inline T dot(const Vect &vec3) const {
        return x * vec3.x + y * vec3.y;
    }
    constexpr inline Vect copy() const {
        return *this;
    }
    inline T cosTo(const Vect &vect) const {
        return dot(vect) / sqrt(size2() * vect.size2());
    }
    inline T angleTo(const Vect &vect) const {
        return acos(cosTo(vect));
    }
    inline Vect clamp(T scale) const {
        T cur = size2();
        if(cur > scale * scale && cur > 1e-15)
            return *this * scale / sqrt(cur);
        else
            return *this;
    }
    inline Vect norm() const {
        return *this / size();
    }
    inline string str() const {
        string s = "{";
        s += to_string(x); s += " ";
        s += to_string(y); s += "}";
        return s;
    }
    /*inline T kTo(const Vect v2, const T len) const {
        const T a = v2.size2();
        const T b = 2 * dot(v2);
        const T c = size2() - square(len);

        return (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
    }*/
    inline T kTo(const Vect v2, const T len) const {
        return (sqrt(dot(v2) * dot(v2) - v2.size2() * (size2() - square(len))) - dot(v2)) / v2.size2();
    }
    inline T kTo2(const Vect v2, const T len) const {
        const T sq = sqrt(dot(v2) * dot(v2) - v2.size2() * (size2() - square(len)));
        return (min(-dot(v2), dot(v2)) + sq) / v2.size2();
    }

    // Non const methods
    inline Vect& get() {
        return *this;
    }
    inline Vect setSize(const T scale) {
        T cur = size();
        if(cur > 0) {
            x *= scale / cur;
            y *= scale / cur;
        } else {
            x = scale;
        }
        return *this;
    }
    inline void setAngle(const T angle) {
        T sz = size();
        x = sz * cos(angle);
        y = sz * sin(angle);
    }
    inline Vect& rotate(const T angle) {
        T t = x;
        x = x * cos(angle) - y * sin(angle);
        y = t * sin(angle) + y * cos(angle);

        return *this;
    }
    inline void rotateWithCenter(Vect &vect, T angle) {
        angle += atan2(y - vect.y, x - vect.x);
        T sz = distTo(vect);
        x = sz * cos(angle) + vect.x;
        y = sz * sin(angle) + vect.y;
    }
    inline void moveTo(Vect &vect, T len) {
        Vect diff = vect - *this;
        T sqDist = diff.size2();
        if(sqDist < len * len) {
            x = vect.x;
            y = vect.y;
        } else {
            T k = len / sqrt(sqDist);
            *this += diff * k;
        }
    }

    // Operators
    inline Vect& operator+= (const Vect &b) {
        x += b.x;
        y += b.y;
        return *this;
    }
    inline Vect& operator-= (const Vect &b) {
        x -= b.x;
        y -= b.y;
        return *this;
    }
    constexpr inline Vect operator+ (const Vect &b) const {
        return Vect(x + b.x, y + b.y);
    }
    constexpr inline Vect operator- (const Vect &b) const {
        return Vect(x - b.x, y - b.y);
    }
    constexpr inline Vect operator* (const T b) const {
        return Vect(x * b, y * b);
    }
    constexpr inline T operator* (const Vect &b) const {
        return dot(b);
    }
    constexpr inline Vect operator/ (const T b) const {
        return Vect(x / b, y / b);
    }
    constexpr inline bool operator== (const Vect &b) const {
        return pow(x - b.x, 2) +
                pow(y - b.y, 2) < 1e-15;
    }
    constexpr inline bool operator!= (const Vect &b) const {
        return !(pow(x - b.x, 2) +
                pow(y - b.y, 2) < 1e-15);
    }
    inline Vect& operator*= (const T b) {
        x *= b;
        y *= b;
        return *this;
    }
    inline Vect& operator/= (const T b) {
        if(b > 1e-15) {
            x /= b;
            y /= b;
        }
        return *this;
    }
    inline void set(const T px, const T py) {
        x = px;
        y = py;
    }
    inline void set(const Vect &a) {
        x = a.x;
        y = a.y;
    }
    inline bool operator<= (const Vect &b) {
        return x <= b.x && y <= b.y;
    }
    inline bool operator< (const Vect &b) {
        return *this <= b;
    }
    inline bool operator>= (const Vect &b) {
        return x >= b.x && y >= b.y;
    }
    inline bool operator> (const Vect &b) {
        return *this >= b;
    }
    friend ostream& operator<< (ostream &os, const Vect &b) {
        os << b.str();
        return os;
    }
};

}

using Vect = Denis::Vect<double>;


namespace std {
    template<typename T> class hash<Denis::Vect<T>> {
    public:
        size_t operator()(const Denis::Vect<T> &vect) const {
            return (hash<T>()(vect.x) << 1) ^ hash<T>()(vect.y);
        }
    };
}

#endif // VECT_H
