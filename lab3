#include <GLFW/glfw3.h>
#include <cmath>
#include <string>
#include <iostream>
#include <vector> // Добавлено для динамических массивов вершин (кругов)

const int WIN_W = 900;
const int WIN_H = 600;

float lerp(float a, float b, float t) { return a + (b - a) * t; }

struct Color { float r, g, b; };
Color mix(Color a, Color b, float t) {
    return { lerp(a.r, b.r, t), lerp(a.g, b.g, t), lerp(a.b, b.b, t) };
}

bool paused = false;
bool growAnimating = false;
float growProgress = 0.0f; 
double lastTime = 0.0;

float cycleAngle = M_PI; 
float cycleSpeed = 0.35f; 

float nightFactor = 0.0f;
bool inDayToNightTransition = false;
bool inNightToDayTransition = false;
float transitionSpeed = 0.5f;

// --- Функции отрисовки через массивы вершин ---

void drawCircle(float cx, float cy, float r, int segments = 40) {
    std::vector<float> vertices;
    // Центральная точка для TRIANGLE_FAN
    vertices.push_back(cx); 
    vertices.push_back(cy);
    
    // Точки по окружности
    for (int i = 0; i <= segments; i++) {
        float a = (float)i / segments * 2.0f * M_PI;
        vertices.push_back(cx + cosf(a) * r);
        vertices.push_back(cy + sinf(a) * r);
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertices.data());
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void drawHouse() {
    glEnableClientState(GL_VERTEX_ARRAY);

    // Основание дома
    float base[] = {
        -0.7f, -0.35f,
        -0.2f, -0.35f,
        -0.2f,  0.0f,
        -0.7f,  0.0f
    };
    glVertexPointer(2, GL_FLOAT, 0, base);
    glDrawArrays(GL_QUADS, 0, 4);

    // Крыша
    float roof[] = {
        -0.75f, 0.0f,
        -0.45f, 0.3f,
        -0.15f, 0.0f
    };
    glVertexPointer(2, GL_FLOAT, 0, roof);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Дверь
    float door[] = {
        -0.6f, -0.2f,
        -0.4f, -0.2f,
        -0.4f, -0.05f,
        -0.6f, -0.05f
    };
    glVertexPointer(2, GL_FLOAT, 0, door);
    glDrawArrays(GL_QUADS, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
}

void drawTree(float x, float baseY, float scale = 1.0f) {
    // Ствол
    float trunk[] = {
        x - 0.03f * scale, baseY,
        x + 0.03f * scale, baseY,
        x + 0.03f * scale, baseY + 0.12f * scale,
        x - 0.03f * scale, baseY + 0.12f * scale
    };
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, trunk);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);

    // Крона (круги)
    drawCircle(x, baseY + 0.22f * scale, 0.08f * scale, 24);
    drawCircle(x - 0.06f * scale, baseY + 0.16f * scale, 0.06f * scale, 20);
    drawCircle(x + 0.06f * scale, baseY + 0.16f * scale, 0.06f * scale, 20);
}

void drawMeadow() {
    float meadow[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f, -0.35f,
        -1.0f, -0.35f
    };
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, meadow);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void drawPlant(float baseX, float baseY, float progress) {
    glEnableClientState(GL_VERTEX_ARRAY);

    // Стебель
    float topY = baseY + 0.25f * progress;
    float stem[] = {
        baseX, baseY,
        baseX, topY
    };
    glVertexPointer(2, GL_FLOAT, 0, stem);
    glDrawArrays(GL_LINES, 0, 2);

    // Листья
    if (progress > 0.2f) {
        float s = progress;
        float leaves[] = {
            // Левый лист
            baseX, baseY + 0.08f * s,
            baseX - 0.06f * s, baseY + 0.03f * s,
            baseX - 0.03f * s, baseY + 0.18f * s,
            // Правый лист
            baseX, baseY + 0.08f * s,
            baseX + 0.06f * s, baseY + 0.03f * s,
            baseX + 0.03f * s, baseY + 0.18f * s
        };
        glVertexPointer(2, GL_FLOAT, 0, leaves);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glDisableClientState(GL_VERTEX_ARRAY);
}

void drawStar(float x, float y, float s = 0.006f) {
    float star[] = {
        x, y + s,
        x - s, y,
        x + s, y
    };
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, star);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableClientState(GL_VERTEX_ARRAY);
}

// --- Логика сцены (без изменений, кроме вызовов отрисовки) ---

void renderScene(float nightFactorLocal) {
    Color daySky = {0.53f, 0.81f, 0.98f}; 
    Color nightSky = {0.04f, 0.06f, 0.25f}; 
    Color sky = mix(daySky, nightSky, nightFactorLocal);
    glClearColor(sky.r, sky.g, sky.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Color dayMeadow = {0.51f, 0.78f, 0.32f};
    Color nightMeadow = {0.08f, 0.15f, 0.06f};
    Color meadowC = mix(dayMeadow, nightMeadow, nightFactorLocal);
    glColor3f(meadowC.r, meadowC.g, meadowC.b);
    drawMeadow();

    Color treeDay = {0.12f, 0.45f, 0.15f};
    Color treeNight = {0.03f, 0.12f, 0.06f};
    Color treeC = mix(treeDay, treeNight, nightFactorLocal);
    glColor3f(treeC.r, treeC.g, treeC.b);
    drawTree(-0.2f, -0.35f, 1.2f);
    drawTree(0.5f, -0.35f, 1.0f);
    drawTree(0.15f, -0.35f, 0.9f);

    Color houseDay = {0.96f, 0.80f, 0.65f};
    Color houseNight = {0.32f, 0.25f, 0.20f};
    Color houseC = mix(houseDay, houseNight, nightFactorLocal);
    glColor3f(houseC.r, houseC.g, houseC.b);
    drawHouse();

    Color plantDay = {0.10f, 0.55f, 0.12f};
    Color plantNight = {0.06f, 0.12f, 0.05f};
    Color plantC = mix(plantDay, plantNight, nightFactorLocal);
    glColor3f(plantC.r, plantC.g, plantC.b);
    drawPlant(0.7f, -0.35f, growProgress);

    if (nightFactorLocal > 0.05f) {
        glColor3f(1.0f, 1.0f, 1.0f);
        drawStar(-0.5f, 0.6f);
        drawStar(-0.1f, 0.8f, 0.007f);
        drawStar(0.3f, 0.7f);
        drawStar(0.65f, 0.85f, 0.005f);
    }
}

void drawSunOrMoon(float cx, float cy, bool isMoon, float nightFactorLocal) {
    if (!isMoon) {
        Color sunDay = {1.0f, 0.94f, 0.2f};
        Color sunNight = {0.9f, 0.8f, 0.2f};
        Color col = mix(sunDay, sunNight, nightFactorLocal);
        glColor3f(col.r, col.g, col.b);
        drawCircle(cx, cy, 0.07f, 36);
    } else {
        Color moon = {0.95f, 0.95f, 1.0f};
        Color moonNight = {0.85f, 0.9f, 1.0f};
        Color col = mix(moon, moonNight, nightFactorLocal);
        glColor3f(col.r, col.g, col.b);
        drawCircle(cx, cy, 0.06f, 36);
        glColor3f(col.r - 0.15f, col.g - 0.12f, col.b - 0.12f);
        drawCircle(cx + 0.02f, cy + 0.02f, 0.018f, 20);
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    GLFWwindow* window = glfwCreateWindow(WIN_W, WIN_H, "Day/Night Demo (Vertex Arrays)", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    lastTime = glfwGetTime();
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

    std::cout << "Controls:\n";
    std::cout << "  Press 'P' to pause/resume animation\n";
    std::cout << "  Press 'G' to toggle plant growth\n";
    std::cout << "  Close window to exit\n";

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        double dt = now - lastTime;
        lastTime = now;
        if (dt > 0.1) dt = 0.1;

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) { 
            paused = !paused; 
            glfwWaitEventsTimeout(0.12); 
        }
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) { 
            growAnimating = !growAnimating; 
            glfwWaitEventsTimeout(0.12); 
        }

        if (!paused) {
            cycleAngle -= cycleSpeed * (float)dt; 
            
            static bool wasAboveZero = true;
            bool isAboveZero = (cycleAngle > 0.0f);
            
            if (wasAboveZero && !isAboveZero && !inDayToNightTransition && nightFactor < 0.5f) {
                inDayToNightTransition = true;
            }
            
            if (wasAboveZero && !isAboveZero && !inNightToDayTransition && nightFactor > 0.5f) {
                inNightToDayTransition = true;
            }
            
            wasAboveZero = isAboveZero;
            
            if (cycleAngle < -0.05f) {
                cycleAngle += 2.0f * M_PI;
            }

            if (growAnimating) {
                growProgress += (float)dt * 0.2f;
                if (growProgress > 1.0f) growProgress = 1.0f;
            } else {
                growProgress -= (float)dt * 0.06f;
                if (growProgress < 0.0f) growProgress = 0.0f;
            }
        }

        if (inDayToNightTransition) {
            nightFactor += transitionSpeed * (float)dt;
            if (nightFactor >= 1.0f) {
                nightFactor = 1.0f;
                inDayToNightTransition = false;
            }
        }
        else if (inNightToDayTransition) {
            nightFactor -= transitionSpeed * (float)dt;
            if (nightFactor <= 0.0f) {
                nightFactor = 0.0f;
                inNightToDayTransition = false;
            }
        }

        float cx = 0.0f;
        float cy = -0.15f;
        float radius = 0.9f;
        float sx = cx + radius * cosf(cycleAngle);
        float sy = cy + radius * sinf(cycleAngle);

        bool isMoon = (nightFactor > 0.5f);

        renderScene(nightFactor);
        drawSunOrMoon(sx, sy, isMoon, nightFactor);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
