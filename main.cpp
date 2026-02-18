// day_night.cpp
// Компиляция: см. инструкции внизу
// Использует GLFW + OpenGL fixed-function (compatible with OpenGL 2.1)

#include <GLFW/glfw3.h>
#include <cmath>
#include <string>
#include <iostream>

// --- Настройки окна ---
const int WIN_W = 900;
const int WIN_H = 600;

// Утилиты
float lerp(float a, float b, float t) { return a + (b - a) * t; }

struct Color { float r,g,b; };
Color mix(Color a, Color b, float t) {
    return { lerp(a.r,b.r,t), lerp(a.g,b.g,t), lerp(a.b,b.b,t) };
}

// Проверка клика в прямоугольнике (в координатах окна, origin top-left)
bool pointInRect(double px, double py, double rx, double ry, double rw, double rh) {
    return px >= rx && px <= rx + rw && py >= ry && py <= ry + rh;
}

// --- Состояние анимаций ---
bool paused = false;
bool growAnimating = false;
float growProgress = 0.0f; // 0..1
double lastTime = 0.0;

// dayCycle: угол движения светила вдоль дуги (в радиан). PI = слева, 0 = справа
float cycleAngle = M_PI; // start at left
float cycleSpeed = 0.35f; // radians per second (регулирует скорость солнца/луны)

// переходный параметр при смене дня/ночи [0..1] (0 = день, 1 = ночь)
float nightFactor = 0.0f;
bool inDayToNightTransition = false;
bool inNightToDayTransition = false;
float transitionSpeed = 0.5f;



// --- Рисовалки простыми полигонами (immediate mode) ---
// Нарисовать круг (filled)
void drawCircle(float cx, float cy, float r, int segments = 40) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i=0;i<=segments;i++){
        float a = (float)i / segments * 2.0f * M_PI;
        glVertex2f(cx + cosf(a)*r, cy + sinf(a)*r);
    }
    glEnd();
}

// Простейшие объекты: дом, дерево, поляна, растение
void drawHouse() {
    // стены (опущены на землю)
    glBegin(GL_QUADS);
      glVertex2f(-0.7f, -0.35f);
      glVertex2f(-0.2f, -0.35f);
      glVertex2f(-0.2f,  0.0f);
      glVertex2f(-0.7f,  0.0f);
    glEnd();
    // крыша
    glBegin(GL_TRIANGLES);
      glVertex2f(-0.75f, 0.0f);
      glVertex2f(-0.45f, 0.3f);
      glVertex2f(-0.15f, 0.0f);
    glEnd();
    // окно
    glBegin(GL_QUADS);
      glVertex2f(-0.6f, -0.2f);
      glVertex2f(-0.4f, -0.2f);
      glVertex2f(-0.4f, -0.05f);
      glVertex2f(-0.6f, -0.05f);
    glEnd();
}

void drawTree(float x, float baseY, float scale = 1.0f) {
    // trunk
    glBegin(GL_QUADS);
      glVertex2f(x-0.03f*scale, baseY);
      glVertex2f(x+0.03f*scale, baseY);
      glVertex2f(x+0.03f*scale, baseY + 0.12f*scale);
      glVertex2f(x-0.03f*scale, baseY + 0.12f*scale);
    glEnd();
    // leaves (three circles)
    drawCircle(x, baseY + 0.22f*scale, 0.08f*scale, 24);
    drawCircle(x-0.06f*scale, baseY + 0.16f*scale, 0.06f*scale, 20);
    drawCircle(x+0.06f*scale, baseY + 0.16f*scale, 0.06f*scale, 20);
}

void drawMeadow() {
    glBegin(GL_QUADS);
      glVertex2f(-1.0f, -1.0f);
      glVertex2f( 1.0f, -1.0f);
      glVertex2f( 1.0f, -0.35f);
      glVertex2f(-1.0f, -0.35f);
    glEnd();
}

// draw a simple plant that can grow (stem + two leaves)
void drawPlant(float baseX, float baseY, float progress) {
    // stem grows up from baseY to baseY + 0.25*progress
    float topY = baseY + 0.25f * progress;
    glBegin(GL_LINES);
      glVertex2f(baseX, baseY);
      glVertex2f(baseX, topY);
    glEnd();
    if (progress>0.2f) {
        // leaves (scale with progress)
        float s = progress;
        glBegin(GL_TRIANGLES);
          glVertex2f(baseX, baseY + 0.08f*s);
          glVertex2f(baseX - 0.06f*s, baseY + 0.03f*s);
          glVertex2f(baseX - 0.03f*s, baseY + 0.18f*s);
        glEnd();
        glBegin(GL_TRIANGLES);
          glVertex2f(baseX, baseY + 0.08f*s);
          glVertex2f(baseX + 0.06f*s, baseY + 0.03f*s);
          glVertex2f(baseX + 0.03f*s, baseY + 0.18f*s);
        glEnd();
    }
}

// draw simple stars (for night)
void drawStar(float x, float y, float s=0.006f) {
    glBegin(GL_TRIANGLES);
      glVertex2f(x, y+s);
      glVertex2f(x-s, y);
      glVertex2f(x+s, y);
    glEnd();
}

// --- Main render function ---
void renderScene(float nightFactorLocal) {
    // Background sky color day -> night
    Color daySky = {0.53f, 0.81f, 0.98f}; // light blue
    Color nightSky = {0.04f, 0.06f, 0.25f}; // deep blue
    Color sky = mix(daySky, nightSky, nightFactorLocal);
    glClearColor(sky.r, sky.g, sky.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Meadow color
    Color dayMeadow = {0.51f, 0.78f, 0.32f};
    Color nightMeadow = {0.08f, 0.15f, 0.06f};
    Color meadowC = mix(dayMeadow, nightMeadow, nightFactorLocal);
    glColor3f(meadowC.r, meadowC.g, meadowC.b);
    drawMeadow();

    // Distant trees (slightly darker at night)
    Color treeDay = {0.12f, 0.45f, 0.15f};
    Color treeNight = {0.03f, 0.12f, 0.06f};
    Color treeC = mix(treeDay, treeNight, nightFactorLocal);
    glColor3f(treeC.r, treeC.g, treeC.b);
    drawTree(-0.2f, -0.35f, 1.2f);
    drawTree(0.5f, -0.35f, 1.0f);
    drawTree(0.15f, -0.35f, 0.9f);

    // House
    Color houseDay = {0.96f, 0.80f, 0.65f};
    Color houseNight = {0.32f, 0.25f, 0.20f};
    Color houseC = mix(houseDay, houseNight, nightFactorLocal);
    glColor3f(houseC.r, houseC.g, houseC.b);
    drawHouse();

    // Plant (growing)
    Color plantDay = {0.10f, 0.55f, 0.12f};
    Color plantNight = {0.06f, 0.12f, 0.05f};
    Color plantC = mix(plantDay, plantNight, nightFactorLocal);
    glColor3f(plantC.r, plantC.g, plantC.b);
    drawPlant(0.7f, -0.35f, growProgress);

    // If night, draw stars
    if (nightFactorLocal > 0.05f) {
        glColor3f(1.0f, 1.0f, 1.0f);
        drawStar(-0.5f, 0.6f);
        drawStar(-0.1f, 0.8f, 0.007f);
        drawStar(0.3f, 0.7f);
        drawStar(0.65f, 0.85f, 0.005f);
    }
}

// --- Draw sun or moon at given normalized coordinates ---
void drawSunOrMoon(float cx, float cy, bool isMoon, float nightFactorLocal) {
    if (!isMoon) {
        // Sun: bright yellow, slightly dim at dusk
        Color sunDay = {1.0f, 0.94f, 0.2f};
        Color sunNight = {0.9f, 0.8f, 0.2f};
        Color col = mix(sunDay, sunNight, nightFactorLocal);
        glColor3f(col.r, col.g, col.b);
        drawCircle(cx, cy, 0.07f, 36);
    } else {
        // Moon: pale
        Color moon = {0.95f, 0.95f, 1.0f};
        Color moonNight = {0.85f, 0.9f, 1.0f};
        Color col = mix(moon, moonNight, nightFactorLocal);
        glColor3f(col.r, col.g, col.b);
        drawCircle(cx, cy, 0.06f, 36);
        // simple crater (cut)
        glColor3f(col.r - 0.15f, col.g - 0.12f, col.b - 0.12f);
        drawCircle(cx+0.02f, cy+0.02f, 0.018f, 20);
    }
}

// Управление осуществляется через клавиатуру (P для паузы, G для роста растения)

// UI функция больше не используется (кнопки скрыты)

// --- Main ---
int main() {
    if (!glfwInit()) {
        std::cerr<<"GLFW init failed\n";
        return -1;
    }

    // Request default compatibility profile (do NOT ask for core profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    GLFWwindow* window = glfwCreateWindow(WIN_W, WIN_H, "Day/Night Demo (C++ + OpenGL 2.1)", nullptr, nullptr);
    if (!window) {
        std::cerr<<"Window creation failed\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    lastTime = glfwGetTime();

    // mouse callback handling (we'll poll on click)
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

    std::cout << "Controls:\n";
    std::cout << "  Press 'P' to pause/resume animation\n";
    std::cout << "  Press 'G' to toggle plant growth\n";
    std::cout << "  Close window to exit\n";

    // main loop
    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        double dt = now - lastTime;
        lastTime = now;
        if (dt > 0.1) dt = 0.1;

        // input: keyboard
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) { 
            paused = !paused; 
            glfwWaitEventsTimeout(0.12); 
        }
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) { 
            growAnimating = !growAnimating; 
            glfwWaitEventsTimeout(0.12); 
        }

        if (!paused) {
            // advance cycleAngle (движение солнца/луны слева направо)
            cycleAngle -= cycleSpeed * (float)dt; // PI -> 0
            
            // Когда светило уходит за правый край (cycleAngle < 0), начинаем переход
            static bool wasAboveZero = true;
            bool isAboveZero = (cycleAngle > 0.0f);
            
            // Переход день->ночь (солнце заходит)
            if (wasAboveZero && !isAboveZero && !inDayToNightTransition && nightFactor < 0.5f) {
                inDayToNightTransition = true;
            }
            
            // Переход ночь->день (луна заходит)
            if (wasAboveZero && !isAboveZero && !inNightToDayTransition && nightFactor > 0.5f) {
                inNightToDayTransition = true;
            }
            
            wasAboveZero = isAboveZero;
            
            // Обнуляем угол (бесконечный цикл)
            if (cycleAngle < -0.05f) {
                cycleAngle += 2.0f * M_PI;
            }

            // grow animation
            if (growAnimating) {
                growProgress += (float)dt * 0.2f;
                if (growProgress > 1.0f) growProgress = 1.0f;
            } else {
                growProgress -= (float)dt * 0.06f;
                if (growProgress < 0.0f) growProgress = 0.0f;
            }
        }

        // Плавный переход день->ночь
        if (inDayToNightTransition) {
            nightFactor += transitionSpeed * (float)dt;
            if (nightFactor >= 1.0f) {
                nightFactor = 1.0f;
                inDayToNightTransition = false;
            }
        }
        // Плавный переход ночь->день
        else if (inNightToDayTransition) {
            nightFactor -= transitionSpeed * (float)dt;
            if (nightFactor <= 0.0f) {
                nightFactor = 0.0f;
                inNightToDayTransition = false;
            }
        }

        // compute sun/moon position along arc: center below top so arc above horizon
        float cx = 0.0f;
        float cy = -0.15f;
        float radius = 0.9f;
        float sx = cx + radius * cosf(cycleAngle);
        float sy = cy + radius * sinf(cycleAngle);

        // decide whether current moving body is sun or moon:
        // We'll treat daytime when nightFactor < 0.5 as sun visible; else moon visible
        bool isMoon = (nightFactor > 0.5f);

        // render scene with night factor
        renderScene(nightFactor);

        // draw sun/moon on top
        drawSunOrMoon(sx, sy, isMoon, nightFactor);

        // swap
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
