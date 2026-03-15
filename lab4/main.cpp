#include <cmath>
#include <cstdlib>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// --------- Scene state ---------
static float g_faceGap = 0.0f;            // separation of cube faces
static float g_orbitAngle = 0.0f;         // light angle around cube (deg)
static float g_orbitSpeed = 30.0f;        // deg/sec
static float g_orbitPlaneYaw = 25.0f;     // rotate orbit plane around Y (deg)
static float g_orbitPlanePitch = 20.0f;   // rotate orbit plane around X (deg)

static float g_camYaw = 35.0f;            // camera orbit
static float g_camPitch = 20.0f;
static float g_camDist = 6.5f;

static int g_lastTimeMs = 0;

static void setupLights(const float lightPos[4]) {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat ambient[]  = {0.15f, 0.15f, 0.15f, 1.0f};
    GLfloat diffuse[]  = {0.95f, 0.90f, 0.70f, 1.0f};
    GLfloat specular[] = {1.0f, 1.0f, 0.9f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    GLfloat matSpec[] = {0.6f, 0.6f, 0.6f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);
}

static void drawAxis(float len) {
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(len, 0, 0);
    glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, len, 0);
    glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, len);
    glEnd();
    glEnable(GL_LIGHTING);
}

static void drawOrbitCircle(float radius) {
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.9f, 0.4f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 120; ++i) {
        float a = (2.0f * 3.1415926f * i) / 120.0f;
        glVertex3f(radius * std::cos(a), 0.0f, radius * std::sin(a));
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

static void drawCubeWithGaps(float half) {
    const float h = half;
    const float g = g_faceGap;

    // +X face
    glPushMatrix();
    glTranslatef(h + g, 0, 0);
    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0);
    glColor3f(0.8f, 0.3f, 0.3f);
    glVertex3f(0, -h, -h);
    glVertex3f(0,  h, -h);
    glVertex3f(0,  h,  h);
    glVertex3f(0, -h,  h);
    glEnd();
    glPopMatrix();

    // -X face
    glPushMatrix();
    glTranslatef(-h - g, 0, 0);
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glColor3f(0.3f, 0.8f, 0.3f);
    glVertex3f(0, -h,  h);
    glVertex3f(0,  h,  h);
    glVertex3f(0,  h, -h);
    glVertex3f(0, -h, -h);
    glEnd();
    glPopMatrix();

    // +Y face
    glPushMatrix();
    glTranslatef(0, h + g, 0);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glColor3f(0.3f, 0.3f, 0.8f);
    glVertex3f(-h, 0, -h);
    glVertex3f( h, 0, -h);
    glVertex3f( h, 0,  h);
    glVertex3f(-h, 0,  h);
    glEnd();
    glPopMatrix();

    // -Y face
    glPushMatrix();
    glTranslatef(0, -h - g, 0);
    glBegin(GL_QUADS);
    glNormal3f(0, -1, 0);
    glColor3f(0.8f, 0.8f, 0.3f);
    glVertex3f(-h, 0,  h);
    glVertex3f( h, 0,  h);
    glVertex3f( h, 0, -h);
    glVertex3f(-h, 0, -h);
    glEnd();
    glPopMatrix();

    // +Z face
    glPushMatrix();
    glTranslatef(0, 0, h + g);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glColor3f(0.6f, 0.3f, 0.8f);
    glVertex3f(-h, -h, 0);
    glVertex3f( h, -h, 0);
    glVertex3f( h,  h, 0);
    glVertex3f(-h,  h, 0);
    glEnd();
    glPopMatrix();

    // -Z face
    glPushMatrix();
    glTranslatef(0, 0, -h - g);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, -1);
    glColor3f(0.3f, 0.7f, 0.8f);
    glVertex3f(-h,  h, 0);
    glVertex3f( h,  h, 0);
    glVertex3f( h, -h, 0);
    glVertex3f(-h, -h, 0);
    glEnd();
    glPopMatrix();
}

static void drawEdges(float half) {
    glDisable(GL_LIGHTING);
    glColor3f(0.05f, 0.05f, 0.05f);
    glLineWidth(2.0f);

    float h = half + g_faceGap;
    // draw the edges of the bounding box that contains separated faces
    glBegin(GL_LINES);
    // bottom square
    glVertex3f(-h, -h, -h); glVertex3f( h, -h, -h);
    glVertex3f( h, -h, -h); glVertex3f( h, -h,  h);
    glVertex3f( h, -h,  h); glVertex3f(-h, -h,  h);
    glVertex3f(-h, -h,  h); glVertex3f(-h, -h, -h);
    // top square
    glVertex3f(-h,  h, -h); glVertex3f( h,  h, -h);
    glVertex3f( h,  h, -h); glVertex3f( h,  h,  h);
    glVertex3f( h,  h,  h); glVertex3f(-h,  h,  h);
    glVertex3f(-h,  h,  h); glVertex3f(-h,  h, -h);
    // verticals
    glVertex3f(-h, -h, -h); glVertex3f(-h,  h, -h);
    glVertex3f( h, -h, -h); glVertex3f( h,  h, -h);
    glVertex3f( h, -h,  h); glVertex3f( h,  h,  h);
    glVertex3f(-h, -h,  h); glVertex3f(-h,  h,  h);
    glEnd();

    glEnable(GL_LIGHTING);
    glLineWidth(1.0f);
}

static void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // camera orbit
    glTranslatef(0, 0, -g_camDist);
    glRotatef(g_camPitch, 1, 0, 0);
    glRotatef(g_camYaw, 0, 1, 0);

    drawAxis(1.2f);

    // orbit plane transform
    glPushMatrix();
    glRotatef(g_orbitPlaneYaw, 0, 1, 0);
    glRotatef(g_orbitPlanePitch, 1, 0, 0);

    // orbit radius
    const float orbitR = 2.2f;

    // light position in orbit plane
    float lx = orbitR * std::cos(g_orbitAngle * 3.1415926f / 180.0f);
    float lz = orbitR * std::sin(g_orbitAngle * 3.1415926f / 180.0f);
    float lightPos[] = {lx, 0.0f, lz, 1.0f};

    setupLights(lightPos);

    // draw orbit circle in the plane
    drawOrbitCircle(orbitR);

    // draw the "sun"
    glPushMatrix();
    glTranslatef(lx, 0.0f, lz);
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.9f, 0.3f);
    glutSolidSphere(0.12, 20, 16);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // draw cube
    drawCubeWithGaps(0.8f);
    drawEdges(0.8f);

    glPopMatrix();

    glutSwapBuffers();
}

static void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)w / (double)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

static void update() {
    int now = glutGet(GLUT_ELAPSED_TIME);
    float dt = (now - g_lastTimeMs) / 1000.0f;
    g_lastTimeMs = now;

    g_orbitAngle += g_orbitSpeed * dt;
    if (g_orbitAngle > 360.0f) g_orbitAngle -= 360.0f;

    glutPostRedisplay();
}

static void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 27: // ESC
            std::exit(0);
            break;
        case '+':
        case '=':
            g_faceGap += 0.05f;
            if (g_faceGap > 0.8f) g_faceGap = 0.8f;
            break;
        case '-':
        case '_':
            g_faceGap -= 0.05f;
            if (g_faceGap < 0.0f) g_faceGap = 0.0f;
            break;
        case 'q':
            g_orbitSpeed += 5.0f;
            break;
        case 'e':
            g_orbitSpeed -= 5.0f;
            if (g_orbitSpeed < 0.0f) g_orbitSpeed = 0.0f;
            break;
        case 'i':
            g_orbitPlanePitch += 3.0f;
            break;
        case 'k':
            g_orbitPlanePitch -= 3.0f;
            break;
        case 'j':
            g_orbitPlaneYaw += 3.0f;
            break;
        case 'l':
            g_orbitPlaneYaw -= 3.0f;
            break;
        case 'w':
            g_camPitch += 3.0f;
            break;
        case 's':
            g_camPitch -= 3.0f;
            break;
        case 'a':
            g_camYaw += 3.0f;
            break;
        case 'd':
            g_camYaw -= 3.0f;
            break;
        case 'z':
            g_camDist += 0.3f;
            break;
        case 'x':
            g_camDist -= 0.3f;
            if (g_camDist < 2.5f) g_camDist = 2.5f;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 700);
    glutCreateWindow("Lab 3 - Cube + Orbiting Light");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_FLAT);

    glClearColor(0.95f, 0.95f, 0.98f, 1.0f);

    g_lastTimeMs = glutGet(GLUT_ELAPSED_TIME);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(update);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
