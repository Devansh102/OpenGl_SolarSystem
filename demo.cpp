#include <iostream>
#include <cmath>
#include <GLUT/glut.h>
#include <FreeImage.h>


#define PI 3.14

static GLfloat spin = 0.0, angle = 0.0;
static GLfloat planetSpin = 0.0;
static GLfloat spinSpeed = 0.5;
static GLfloat revolutionSpeed = 1.0;
static const int numPlanets = 9;
int selectedPlanet = -1;

static GLfloat planetRadius[numPlanets] = {0.04, 0.08, 0.16, 0.12, 0.3, 0.26, 0.19, 0.12, 0.05};
static GLfloat planetOrbitRadius[numPlanets] = {0.7, 1.2, 1.5, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};
// static GLfloat planetSpinSpeeds[numPlanets] = {.09, 0.08, 0.05, 0.03, 0.02, 0.015, 0.01, 0.07, 0.001};
static GLfloat planetSpinSpeeds[numPlanets][2] = {
    {0.1, 0.1},  // Planet 0: spin speed, revolution speed
    {0.0, 0.0},  // Planet 1: spin speed, revolution speed
    {0.1 , 0.9},  // Planet 2: spin speed, revolution speed
    {0.0, 0.0},  // Planet 3: spin speed, revolution speed
    {0.0, 0.0},  // Planet 4: spin speed, revolution speed
    {0., 0.6},  // Planet 5: spin speed, revolution speed
    {0.0, 0.0},  // Planet 6: spin speed, revolution speed
    {0.0, 0.0},  // Planet 7: spin speed, revolution speed
    {0.0, 0.0}   // Planet 8: spin speed, revolution speed
};

GLfloat planetColors[numPlanets][4] = {
    {0.8, 0.1, 0.1, 1.0},   // Red
    {0.1, 0.1, 0.8, 1.0},   // Blue
    {0.1, 0.8, 0.1, 1.0},   // Green
    {0.8, 0.8, 0.1, 1.0},   // Yellow
    {0.5, 0.5, 0.5, 1.0},   // Gray
    {0.9, 0.5, 0.1, 1.0},   // Orange
    {0.5, 0.1, 0.9, 1.0},   // Purple
    {0.1, 0.9, 0.5, 1.0},   // Teal
    {0.2, 0.2, 0.2, 1.0}    // Dark Gray
};

GLuint planetTextures[numPlanets];
GLuint sunTextureID;
GLuint ringTextureID;

const int numStars = 1000;
static GLfloat xAxis[numStars];
static GLfloat yAxis[numStars];

void sunlight() {
    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat sunDiffuseMaterial[4] = {0.9, 0.7, 0.1, 1.0};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, sunDiffuseMaterial);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 10.0);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
}

void init(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    sunlight();
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);

    const char* sunFilename = "textures/sun.jpg";
    FIBITMAP* sunBitmap = FreeImage_Load(FIF_JPEG, sunFilename);
    if (!sunBitmap) {
        std::cerr << "Error loading texture image for the sun" << std::endl;
        // Handle error
    }

    FIBITMAP* sunImage = FreeImage_ConvertTo32Bits(sunBitmap);
    int sunWidth = FreeImage_GetWidth(sunImage);
    int sunHeight = FreeImage_GetHeight(sunImage);

    glGenTextures(1, &sunTextureID);
    glBindTexture(GL_TEXTURE_2D, sunTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, sunWidth, sunHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(sunImage));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    FreeImage_Unload(sunBitmap);
    FreeImage_Unload(sunImage);


    const char* ringFilename = "textures/ring.jpg";
    FIBITMAP* ringBitmap = FreeImage_Load(FIF_JPEG, ringFilename);
    if (!sunBitmap) {
        std::cerr << "Error loading texture image for the sun" << std::endl;
        // Handle error
    }

    FIBITMAP* ringImage = FreeImage_ConvertTo32Bits(ringBitmap);
    int ringWidth = FreeImage_GetWidth(ringImage);
    int ringHeight = FreeImage_GetHeight(ringImage);

    glGenTextures(1, &ringTextureID);
    glBindTexture(GL_TEXTURE_2D, ringTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ringWidth, ringHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(ringImage));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    FreeImage_Unload(ringBitmap);
    FreeImage_Unload(ringImage);



    // Load texture images
    for (int i = 0; i < numPlanets; ++i) {
    const char* filename = ("textures/" + std::to_string(i) + ".jpg").c_str();

    // Load the image using FreeImage
    FIBITMAP* bitmap = FreeImage_Load(FIF_JPEG, filename);
    if (!bitmap) {
        std::cerr << "Error loading texture image for planet " << i << std::endl;
        continue;  // Skip this texture and continue with the next one
    }

    // Convert the image to 32 bits
    FIBITMAP* pImage = FreeImage_ConvertTo32Bits(bitmap);
    int nWidth = FreeImage_GetWidth(pImage);
    int nHeight = FreeImage_GetHeight(pImage);

    // Generate texture and set parameters
    glGenTextures(1, &planetTextures[i]);
    glBindTexture(GL_TEXTURE_2D, planetTextures[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Unload the images
    FreeImage_Unload(bitmap);
    FreeImage_Unload(pImage);

    glPopMatrix();  // Move this line here
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
}

}

GLfloat dot(const GLfloat a[3], const GLfloat b[3]) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}


void drawPlanetName(int planetIndex, bool highlight) {
    std::string planetName = "Planet " + std::to_string(planetIndex + 1);

    if (highlight) {
        glColor3f(1.0, 1.0, 0.0);  // Yellow color for highlighting
        glBegin(GL_QUADS);
        glVertex2f(-0.2, -0.1);  // Adjust these coordinates as needed
        glVertex2f(0.2, -0.1);
        glVertex2f(0.2, 0.1);
        glVertex2f(-0.2, 0.1);
        glEnd();
    }

    glColor3f(1.0, 1.0, 1.0);  // White color for text
    for (char& c : planetName) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void drawPlanet(float orbitRadius, float planetRadius, float planetSpin, GLuint textureID, bool hasRings = false) {
    glPushMatrix();
    glRotatef(planetSpin, 0, 0, 1);
    glTranslatef(orbitRadius, 0.0, 0.0);
    glRotatef(planetSpin, 0, 0, 1);

    // Draw the planet
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLUquadricObj* planetQuadric = gluNewQuadric();
    gluQuadricTexture(planetQuadric, GL_TRUE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Set up light properties
    GLfloat lightPosition[] = {0.0, 1.0, 0.5, 0.0};  // Light from behind the viewer
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    gluQuadricTexture(planetQuadric, GL_TRUE);
    gluQuadricNormals(planetQuadric, GLU_SMOOTH);

    // Set up material properties for the planet
    GLfloat planetColor[] = {1.0, 1.0, 1.0, 1.0};  // White color
    glMaterialfv(GL_FRONT, GL_AMBIENT, planetColor);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, planetColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, planetColor);
    glMaterialf(GL_FRONT, GL_SHININESS, 0.0);  // No shininess

    gluSphere(planetQuadric, planetRadius, 30, 30);

    gluDeleteQuadric(planetQuadric);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    if (hasRings) {
    // Draw textured rings for Saturn using a torus
        glPushMatrix();
        glRotatef(0.0, 1, 0, 0);  // Rotate the rings at a 30-degree angle

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, ringTextureID);  // Use the texture ID for the rings

        // Set up material properties for the rings
        GLfloat ringColor[] = {0.2, 0.2, 0.2, 1.0};
        glMaterialfv(GL_FRONT, GL_AMBIENT, ringColor);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, ringColor);
        glMaterialfv(GL_FRONT, GL_SPECULAR, ringColor);
        glMaterialf(GL_FRONT, GL_SHININESS, 0.0);  // No shininess

        // Draw the textured rings using a torus
        // const float ringInnerRadius = planetRadius + 0.4;  // Adjust inner radius of the rings
        // const float ringOuterRadius = planetRadius + 0.8;  // Adjust outer radius of the rings
        const int ringSides = 2;  // Adjust the number of sides for smoother rings
        const int ringRings = 30;  // Adjust the number of rings for smoother rings

        glutSolidTorus(0.12, 0.3, ringSides, ringRings);

        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    }

    glPopMatrix();
}



void generateRandomPoints() {
    for (int i = 0; i < numStars; ++i) {
        xAxis[i] = (rand() % 1300 - 650) / 100.0;  // Random x coordinate between -6.5 and 6.5
        yAxis[i] = (rand() % 900 - 450) / 100.0;   // Random y coordinate between -4.5 and 4.5
    }
}

void spinsphere(void) {
    spin = spin + spinSpeed;
    if (spin > 360.0)
        spin = spin - 360.0;
    angle += revolutionSpeed;
    if (angle > 360.0)
        angle = 0.0;

    for (int i = 0; i < numPlanets; ++i) {
        planetSpin += planetSpinSpeeds[i][0];  // Spin speed for planet i
        planetSpinSpeeds[i][1] += planetSpinSpeeds[i][1];  // Revolution speed for planet i

        if (planetSpinSpeeds[i][1] > 360.0)
            planetSpinSpeeds[i][1] = 0.0;
    }

    glutPostRedisplay();
}

void drawStars() {
    glPushMatrix();

    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 1.0f, 1.0f);  // Yellow
    glVertex2f(0.3f, -0.6f);
    glVertex2f(0.4f, -0.6f);
    glVertex2f(0.35f, -0.5f);
    glColor3f(1.0f, 1.0f, 1.0f);  // Yellow
    glVertex2f(0.3f, -0.55f);
    glVertex2f(0.4f, -0.55f);
    glVertex2f(0.35f, -0.65f);
    glEnd();

    glPopMatrix();
}

void drawDottedCircle(float r) {
    glLineWidth(1.5);
    glBegin(GL_LINES);
    for (int i = 0; i < 200; i++) {
        float A = (i * 2 * PI) / 100;
        float x = r * cos(A);
        float y = r * sin(A);
        glVertex2f(x, y);
    }
    glEnd();
}



void drawSun(GLuint textureID) {
    GLfloat sunlight_position[] = {1.0, -5.0, 5.0, 0.0};
    GLfloat sunDiffuseMaterial[4] = {1.0, 1.0, 1.0, 1.0};
    GLfloat sunSpecularMaterial[4] = {1.0, 1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, sunDiffuseMaterial);
    glMaterialfv(GL_FRONT, GL_SPECULAR, sunSpecularMaterial);
    glMaterialf(GL_FRONT, GL_SHININESS, 60.0);

    glEnable(GL_LIGHTING);

    GLfloat ambientLight[] = {0.8, 0.8, 0.6, 0.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

    glShadeModel(GL_SMOOTH);

    GLfloat shadowColor[] = {0.8, 0.8, 0.2, 1.0};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, shadowColor);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glPushMatrix();
    glRotatef(spin, 0, 0, 1);

    const float sunRadius = 0.5; // Adjust this as needed
    const int slices = 20;
    const int stacks = 20;

    // Draw the textured sun sphere
    for (int i = 0; i <= stacks; ++i) {
        float lat0 = PI * (-0.5 + (float)(i - 1) / stacks);
        float z0 = sin(lat0);
        float zr0 = cos(lat0);

        float lat1 = PI * (-0.5 + (float)i / stacks);
        float z1 = sin(lat1);
        float zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float lng = 2 * PI * (float)(j - 1) / slices;
            float x = cos(lng);
            float y = sin(lng);

            glNormal3f(x * zr0, y * zr0, z0);
            glTexCoord2f((float)j / slices, (float)(i - 1) / stacks);
            glVertex3f(x * zr0 * sunRadius, y * zr0 * sunRadius, z0 * sunRadius);

            glNormal3f(x * zr1, y * zr1, z1);
            glTexCoord2f((float)j / slices, (float)i / stacks);
            glVertex3f(x * zr1 * sunRadius, y * zr1 * sunRadius, z1 * sunRadius);
        }
        glEnd();
    }

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
}




void display(void) {

    for (int i = 0; i < numPlanets; ++i) {
        glPushMatrix();

        if (i == selectedPlanet) {
            glTranslatef(planetOrbitRadius[i] + planetRadius[i] + 0.3, 0.0, 0.0);
            drawPlanetName(i, true);
        }

        // Draw the planet
        drawPlanet(planetOrbitRadius[i], planetRadius[i], planetSpin * (i + 1), planetTextures[i], false);

        glPopMatrix();
    }


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0.0, 0.0, -15.0);
    glPushMatrix();
    glPushMatrix();
    glBegin(GL_POINTS);
    glColor3f(1.0, 1.0, 1.0);
    for (int i = 0; i < 800; i++) {
        glVertex2f(xAxis[i], yAxis[i]);
    }
    glEnd();
    drawStars();
    glPopMatrix();

    glRotatef(-60, 1, 0, 0);
    glRotatef(-30, 0, 1, 0);

    glColor3f(.5, .5, .5);
    glPushMatrix();
    drawDottedCircle(0.7);
    drawDottedCircle(1.2);
    drawDottedCircle(1.5);
    drawDottedCircle(2.0);
    drawDottedCircle(3.0);
    drawDottedCircle(4.0);
    drawDottedCircle(5.0);
    drawDottedCircle(6.0);
    drawDottedCircle(7.0);

    drawSun(sunTextureID);
    GLfloat planetColors[numPlanets][4] = {
        {0.8, 0.1, 0.1, 1.0},   // Red
        {0.1, 0.1, 0.8, 1.0},   // Blue
        {0.1, 0.8, 0.1, 1.0},   // Green
        {0.8, 0.8, 0.1, 1.0},   // Yellow
        {0.5, 0.5, 0.5, 1.0},   // Gray
        {0.9, 0.5, 0.1, 1.0},   // Orange
        {0.5, 0.1, 0.9, 1.0},   // Purple
        {0.1, 0.9, 0.5, 1.0},   // Teal
        {0.2, 0.2, 0.2, 1.0}    // Dark Gray
    };

    for (int i = 0; i < numPlanets; ++i) {
        if(i==5){
            drawPlanet(planetOrbitRadius[i], planetRadius[i], planetSpin * (i + 1), planetTextures[i],true);

        }
        drawPlanet(planetOrbitRadius[i], planetRadius[i], planetSpin * (i + 1), planetTextures[i], false);
}


    glPopMatrix();
    glPopMatrix();
    glutSwapBuffers();
    glFlush();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}
void keyboardFunc(unsigned char key, int x, int y) {
    if (key >= '0' && key <= '8') {
        selectedPlanet = key - '0';
        glutPostRedisplay();  // Trigger a redraw to reflect the change
    }
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 900);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Solar System");

    init();

    generateRandomPoints();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(spinsphere);
    glutKeyboardFunc(keyboardFunc);  // Register keyboard callback function

    glutMainLoop();
    return 0;
}