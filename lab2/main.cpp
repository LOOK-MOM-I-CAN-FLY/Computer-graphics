#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <array>
#include <utility>

using namespace std;

struct Canvas {
    int w, h;
    vector<string> buf;
    char bg;
    Canvas(int w_, int h_, char bg_=' ') : w(w_), h(h_), bg(bg_) {
        buf.assign(h, string(w, bg));
    }
    void clear() {
        for (int y = 0; y < h; ++y) buf[y].assign(w, bg);
    }
    void setPixel(int x, int y, char ch='*') {
        if (x < 0 || x >= w || y < 0 || y >= h) return;
        buf[y][x] = ch;
    }
    string asString() const {
        string out;
        for (int y = 0; y < h; ++y) {
            out += buf[y];
            out += '\n';
        }
        return out;
    }
};

void drawLineBresenham(int x0, int y0, int x1, int y1, Canvas &c, char ch='*') {
    bool steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        swap(x0, y0);
        swap(x1, y1);
    }
    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = std::abs(y1 - y0);
    int error = dx / 2;
    int ystep = (y0 < y1) ? 1 : -1;
    int y = y0;
    for (int x = x0; x <= x1; ++x) {
        if (steep) c.setPixel(y, x, ch);
        else c.setPixel(x, y, ch);
        error -= dy;
        if (error < 0) {
            y += ystep;
            error += dx;
        }
    }
}

void drawCircleBresenham(int xc, int yc, int r, Canvas &c, char ch='*') {
    if (r <= 0) return;
    int x = 0;
    int y = r;
    int d = 1 - r;
    auto plot8 = [&](int px, int py) {
        c.setPixel(xc + px, yc + py, ch);
        c.setPixel(xc - px, yc + py, ch);
        c.setPixel(xc + px, yc - py, ch);
        c.setPixel(xc - px, yc - py, ch);
        c.setPixel(xc + py, yc + px, ch);
        c.setPixel(xc - py, yc + px, ch);
        c.setPixel(xc + py, yc - px, ch);
        c.setPixel(xc - py, yc - px, ch);
    };
    while (x <= y) {
        plot8(x, y);
        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            --y;
        }
        ++x;
    }
}

void drawTriangle(const array<pair<int,int>,3>& pts, Canvas &c, char ch='*') {
    drawLineBresenham(pts[0].first, pts[0].second, pts[1].first, pts[1].second, c, ch);
    drawLineBresenham(pts[1].first, pts[1].second, pts[2].first, pts[2].second, c, ch);
    drawLineBresenham(pts[2].first, pts[2].second, pts[0].first, pts[0].second, c, ch);
}

int main() {
    const int WIDTH = 80;
    const int HEIGHT = 30;
    Canvas canvas(WIDTH, HEIGHT, ' ');
    vector<string> frames;
    frames.reserve(4);

    canvas.clear();
    drawLineBresenham(5, 3, 70, 25, canvas, '#');
    frames.push_back(canvas.asString());

    canvas.clear();
    drawLineBresenham(10, 25, 60, 2, canvas, '*');
    frames.push_back(canvas.asString());

    canvas.clear();
    int r = min(WIDTH, HEIGHT) / 4;
    drawCircleBresenham(WIDTH/2, HEIGHT/2, r, canvas, 'o');
    frames.push_back(canvas.asString());

    canvas.clear();
    array<pair<int,int>,3> tri = { make_pair(10,5), make_pair(70,8), make_pair(40,23) };
    drawTriangle(tri, canvas, 'A');
    frames.push_back(canvas.asString());

    size_t idx = 0;
    const int delay_ms = 800;
    while (true) {
        cout << "\x1B[2J\x1B[H";
        cout << frames[idx] << flush;
        idx = (idx + 1) % frames.size();
        this_thread::sleep_for(chrono::milliseconds(delay_ms));
    }
    return 0;
}


/*
(0,0) в (5,3). 

dx = 5, dy = 3, error = dx/2 = 2 

ystep = +1.
x от 0 до 5:

x=0: (0,0). error -= dy → 2 - 3 = -1. error < 0 → y=1, error += dx → -1 + 5 = 4.

x=1: (1,1). error -= 3 → 1. error >=0 .
(счётки: 4 - 3 = 1)

x=2:  (2,1). error -= 3 → -2 → y=2, error += 5 → 3

x=3: (3,2). error -= 3 → 0 → 

x=4: (4,2). error -= 3 → -3 → y=3, error +=5 → 2

x=5: (5,3).







x=0, y=5, d = 1 - r = -4.

Пока x ≤ y:

x=0: plot8(0,5). d < 0 → d += 2*x + 3 = -4 + 0 + 3 = -1. x++ → x=1.

x=1: plot8(1,5). d < 0 → d += 2*1 + 3 = -1 + 2 + 3 = 4. x=2.

x=2: plot8(2,5). d >=0 → d += 2*(x-y)+5 = 4 + 2*(2-5)+5 = 4 + 2*(-3)+5 = 4 -6 +5 = 3; y-- → y=4; x=3

x=3: plot8(3,4). d >=0 → d += 2*(3-4)+5 = 3 + 2*(-1)+5 = 3 -2 +5 = 6; y=3; x=4

Теперь x (4) > y (3) 




*/
