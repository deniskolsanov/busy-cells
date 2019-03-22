#ifndef HSVTORGB_H
#define HSVTORGB_H

static int hsv2rgb(const double h, const double s, const double v) {
    if(s <= 0.0) {
        return v * (256 * 256 + 256 + 1);
    }
    const long i = (long)(h / 60);
    const double ff = h / 60 - i;
    const int p = v * (256 - s) / 256;
    const int q = v * (256 - (s * ff)) / 256;
    const int t = v * (256 - (s * (1.0 - ff))) / 256;

    // p t v q
    switch(i) {
    case 0:
        return ((v * 256 + t) * 256 + p);
    case 1:
        return ((q * 256 + v) * 256 + p);
    case 2:
        return ((p * 256 + v) * 256 + t);
    case 3:
        return ((p * 256 + q) * 256 + v);
    case 4:
        return ((t * 256 + p) * 256 + v);
    default:
        return ((v * 256 + p) * 256 + q);
    }
}

#endif // HSVTORGB_H
