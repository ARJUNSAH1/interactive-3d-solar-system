#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define PI 3.14159265358979323846

int windowWidth = 1200;
int windowHeight = 800;

bool paused = false;
float animationSpeed = 1.0f;

float cameraDistance = 850.0f;
float cameraAngleX = 25.0f;
float cameraAngleY = 0.0f;

bool cameraFollowing = false;
float targetCameraDistance = 850.0f;

bool showOrbits = true;
bool showStars = true;
bool showLabels = true;
bool showAsteroids = true;
bool showAtmosphere = true;
bool showMoons = true;

int selectedPlanet = -1;


// =====================================================
// PLANET DATA
// =====================================================

struct Planet
{
    const char* name;

    float distance;
    float radius;

    float revolutionSpeed;
    float rotationSpeed;

    float r;
    float g;
    float b;

    float revolutionAngle;
    float rotationAngle;

    const char* diameter;
    const char* distanceFromSun;
    const char* moons;
    const char* rotationPeriod;
    const char* revolutionPeriod;
};


Planet planets[] =
{
    {"Mercury",65.0f,5.0f,4.15f,5.0f,
     0.55f,0.55f,0.55f,0.0f,0.0f,
     "4,879 km","57.9 million km","0",
     "58.6 days","88 days"},

    {"Venus",100.0f,8.0f,1.62f,2.0f,
     0.85f,0.55f,0.20f,45.0f,0.0f,
     "12,104 km","108.2 million km","0",
     "243 days","224.7 days"},

    {"Earth",145.0f,9.0f,1.00f,8.0f,
     0.10f,0.35f,0.85f,90.0f,0.0f,
     "12,742 km","149.6 million km","1",
     "23.9 hours","365.25 days"},

    {"Mars",185.0f,7.0f,0.53f,7.0f,
     0.80f,0.20f,0.10f,130.0f,0.0f,
     "6,779 km","227.9 million km","2",
     "24.6 hours","687 days"},

    {"Jupiter",255.0f,18.0f,0.084f,12.0f,
     0.75f,0.50f,0.25f,170.0f,0.0f,
     "139,820 km","778.5 million km","95+",
     "9.9 hours","11.86 years"},

    {"Saturn",330.0f,15.0f,0.034f,10.0f,
     0.85f,0.70f,0.40f,210.0f,0.0f,
     "116,460 km","1.43 billion km","140+",
     "10.7 hours","29.45 years"},

    {"Uranus",400.0f,12.0f,0.012f,8.0f,
     0.25f,0.75f,0.80f,250.0f,0.0f,
     "50,724 km","2.87 billion km","27",
     "17.2 hours","84 years"},

    {"Neptune",465.0f,12.0f,0.006f,8.0f,
     0.10f,0.25f,0.80f,290.0f,0.0f,
     "49,244 km","4.50 billion km","14",
     "16.1 hours","164.8 years"}
};

const int planetCount =
    sizeof(planets) / sizeof(planets[0]);


// =====================================================
// ASTEROIDS
// =====================================================

const int asteroidCount = 450;

float asteroidRadius[asteroidCount];
float asteroidAngle[asteroidCount];
float asteroidHeight[asteroidCount];
float asteroidSize[asteroidCount];


// =====================================================
// TEXT
// =====================================================

void drawText(
    float x,
    float y,
    const char* text,
    void* font = GLUT_BITMAP_HELVETICA_12)
{
    glRasterPos2f(x,y);

    while(*text)
    {
        glutBitmapCharacter(font,*text);
        text++;
    }
}


// =====================================================
// ASTEROID INITIALIZATION
// =====================================================

void initializeAsteroids()
{
    srand(25);

    for(int i=0;i<asteroidCount;i++)
    {
        asteroidRadius[i] =
            210.0f + (rand()%45);

        asteroidAngle[i] =
            rand()%360;

        asteroidHeight[i] =
            ((rand()%100)-50)/10.0f;

        asteroidSize[i] =
            0.7f + (rand()%10)/10.0f;
    }
}


// =====================================================
// STARS
// =====================================================

void drawStars()
{
    if(!showStars)
        return;

    glDisable(GL_LIGHTING);

    glPointSize(1.5f);

    glBegin(GL_POINTS);

    srand(10);

    for(int i=0;i<1000;i++)
    {
        float x =
            (rand()%2400)-1200;

        float y =
            (rand()%1800)-900;

        float z =
            (rand()%1600)-800;

        float brightness =
            0.5f + (rand()%50)/100.0f;

        glColor3f(
            brightness,
            brightness,
            brightness
        );

        glVertex3f(x,y,z);
    }

    glEnd();

    glEnable(GL_LIGHTING);
}


// =====================================================
// ORBITS
// =====================================================

void drawOrbit(float radius)
{
    if(!showOrbits)
        return;

    glDisable(GL_LIGHTING);

    glColor3f(
        0.20f,
        0.20f,
        0.28f
    );

    glBegin(GL_LINE_LOOP);

    for(int i=0;i<240;i++)
    {
        float angle =
            2.0f*PI*i/240.0f;

        glVertex3f(
            radius*cosf(angle),
            0.0f,
            radius*sinf(angle)
        );
    }

    glEnd();

    glEnable(GL_LIGHTING);
}


// =====================================================
// ASTEROID BELT
// =====================================================

void drawAsteroids()
{
    if(!showAsteroids)
        return;

    glDisable(GL_LIGHTING);

    for(int i=0;i<asteroidCount;i++)
    {
        float rad =
            asteroidAngle[i]*PI/180.0f;

        float x =
            asteroidRadius[i]*cosf(rad);

        float z =
            asteroidRadius[i]*sinf(rad);

        glPushMatrix();

        glTranslatef(
            x,
            asteroidHeight[i],
            z
        );

        glColor3f(
            0.35f,
            0.30f,
            0.25f
        );

        glutSolidSphere(
            asteroidSize[i],
            7,
            7
        );

        glPopMatrix();
    }

    glEnable(GL_LIGHTING);
}


// =====================================================
// SUN GLOW
// =====================================================

void drawSunGlow()
{
    glDisable(GL_LIGHTING);

    glEnable(GL_BLEND);

    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE
    );

    // Outer glow
    glColor4f(
        1.0f,
        0.45f,
        0.05f,
        0.08f
    );

    glutSolidSphere(
        55.0f,
        40,
        40
    );

    glColor4f(
        1.0f,
        0.65f,
        0.10f,
        0.12f
    );

    glutSolidSphere(
        45.0f,
        40,
        40
    );

    glDisable(GL_BLEND);

    glEnable(GL_LIGHTING);
}


// =====================================================
// SUN
// =====================================================

void drawSun()
{
    drawSunGlow();

    glDisable(GL_LIGHTING);

    glPushMatrix();

    glColor3f(
        1.0f,
        0.65f,
        0.05f
    );

    glutSolidSphere(
        35.0f,
        60,
        60
    );

    glPopMatrix();

    glEnable(GL_LIGHTING);
}


// =====================================================
// EARTH SURFACE
// =====================================================

void drawEarthDetails()
{
    // Ocean base is the normal Earth sphere.

    glDisable(GL_LIGHTING);

    // Continents represented by small surface patches.
    // These are graphical approximations.

    glColor3f(
        0.08f,
        0.55f,
        0.12f
    );

    glPushMatrix();

    glTranslatef(
        4.0f,
        4.0f,
        5.5f
    );

    glScalef(
        2.8f,
        1.2f,
        0.4f
    );

    glutSolidSphere(
        1.0f,
        16,
        16
    );

    glPopMatrix();


    glPushMatrix();

    glTranslatef(
        -4.5f,
        2.0f,
        6.0f
    );

    glScalef(
        2.0f,
        1.0f,
        0.5f
    );

    glutSolidSphere(
        1.0f,
        16,
        16
    );

    glPopMatrix();


    glPushMatrix();

    glTranslatef(
        1.0f,
        -4.0f,
        6.5f
    );

    glScalef(
        2.5f,
        1.0f,
        0.4f
    );

    glutSolidSphere(
        1.0f,
        16,
        16
    );

    glPopMatrix();

    glEnable(GL_LIGHTING);
}


// =====================================================
// EARTH ATMOSPHERE
// =====================================================

void drawEarthAtmosphere()
{
    if(!showAtmosphere)
        return;

    glDisable(GL_LIGHTING);

    glEnable(GL_BLEND);

    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA
    );

    glColor4f(
        0.20f,
        0.60f,
        1.0f,
        0.18f
    );

    glutSolidSphere(
        10.2f,
        40,
        40
    );

    glDisable(GL_BLEND);

    glEnable(GL_LIGHTING);
}


// =====================================================
// SATURN RINGS
// =====================================================

void drawSaturnRings()
{
    glDisable(GL_LIGHTING);

    glEnable(GL_BLEND);

    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA
    );

    for(int ring=0;ring<7;ring++)
    {
        float radius =
            20.0f + ring*2.2f;

        if(ring%2==0)
            glColor4f(
                0.75f,
                0.65f,
                0.45f,
                0.85f
            );
        else
            glColor4f(
                0.45f,
                0.38f,
                0.25f,
                0.65f
            );

        glBegin(GL_LINE_LOOP);

        for(int i=0;i<150;i++)
        {
            float angle =
                2.0f*PI*i/150.0f;

            glVertex3f(
                radius*cosf(angle),
                0.0f,
                radius*sinf(angle)
            );
        }

        glEnd();
    }

    glDisable(GL_BLEND);

    glEnable(GL_LIGHTING);
}


// =====================================================
// MOON FUNCTION
// =====================================================

void drawSmallMoon(
    float distance,
    float angle,
    float size)
{
    float rad =
        angle*PI/180.0f;

    float x =
        distance*cosf(rad);

    float z =
        distance*sinf(rad);

    glPushMatrix();

    glTranslatef(
        x,
        0.0f,
        z
    );

    GLfloat moonMaterial[] =
    {
        0.55f,
        0.55f,
        0.58f,
        1.0f
    };

    glMaterialfv(
        GL_FRONT,
        GL_AMBIENT_AND_DIFFUSE,
        moonMaterial
    );

    glutSolidSphere(
        size,
        18,
        18
    );

    glPopMatrix();
}


// =====================================================
// PLANET MOONS
// =====================================================

void drawPlanetMoons(int index)
{
    if(!showMoons)
        return;

    // Earth
    if(index==2)
    {
        drawSmallMoon(
            20.0f,
            planets[index].revolutionAngle*4.0f,
            3.0f
        );
    }


    // Jupiter - four visible moons
    if(index==4)
    {
        drawSmallMoon(
            27.0f,
            planets[index].revolutionAngle*5.0f,
            2.5f
        );

        drawSmallMoon(
            34.0f,
            planets[index].revolutionAngle*3.7f+90.0f,
            2.2f
        );

        drawSmallMoon(
            41.0f,
            planets[index].revolutionAngle*2.8f+180.0f,
            2.0f
        );

        drawSmallMoon(
            48.0f,
            planets[index].revolutionAngle*2.1f+270.0f,
            1.8f
        );
    }


    // Saturn
    if(index==5)
    {
        drawSmallMoon(
            28.0f,
            planets[index].revolutionAngle*4.0f,
            2.5f
        );

        drawSmallMoon(
            35.0f,
            planets[index].revolutionAngle*3.0f+70.0f,
            2.0f
        );

        drawSmallMoon(
            43.0f,
            planets[index].revolutionAngle*2.4f+160.0f,
            1.8f
        );

        drawSmallMoon(
            50.0f,
            planets[index].revolutionAngle*1.8f+240.0f,
            1.6f
        );
    }


    // Uranus
    if(index==6)
    {
        drawSmallMoon(
            22.0f,
            planets[index].revolutionAngle*3.0f,
            2.0f
        );

        drawSmallMoon(
            29.0f,
            planets[index].revolutionAngle*2.0f+180.0f,
            1.7f
        );
    }


    // Neptune
    if(index==7)
    {
        drawSmallMoon(
            23.0f,
            planets[index].revolutionAngle*3.0f,
            2.0f
        );
    }
}


// =====================================================
// PLANET HIGHLIGHT
// =====================================================

void drawPlanetHighlight(int index)
{
    if(index!=selectedPlanet)
        return;

    Planet& p =
        planets[index];

    float rad =
        p.revolutionAngle*PI/180.0f;

    float x =
        p.distance*cosf(rad);

    float z =
        p.distance*sinf(rad);

    glDisable(GL_LIGHTING);

    glColor3f(
        1.0f,
        1.0f,
        0.0f
    );

    glPushMatrix();

    glTranslatef(
        x,
        0.0f,
        z
    );

    glutWireSphere(
        p.radius+4.0f,
        30,
        30
    );

    glPopMatrix();

    glEnable(GL_LIGHTING);
}


// =====================================================
// PLANET
// =====================================================

void drawPlanet(int index)
{
    Planet& p =
        planets[index];

    float rad =
        p.revolutionAngle*PI/180.0f;

    float x =
        p.distance*cosf(rad);

    float z =
        p.distance*sinf(rad);

    drawPlanetHighlight(index);

    glPushMatrix();

    glTranslatef(
        x,
        0.0f,
        z
    );

    glRotatef(
        p.rotationAngle,
        0.0f,
        1.0f,
        0.0f
    );


    GLfloat material[] =
    {
        p.r,
        p.g,
        p.b,
        1.0f
    };

    glMaterialfv(
        GL_FRONT,
        GL_AMBIENT_AND_DIFFUSE,
        material
    );


    glutSolidSphere(
        p.radius,
        50,
        50
    );


    // Earth details
    if(index==2)
    {
        drawEarthDetails();
    }


    // Atmosphere
    if(index==2)
    {
        drawEarthAtmosphere();
    }


    // Saturn rings
    if(index==5)
    {
        drawSaturnRings();
    }


    // Moons
    drawPlanetMoons(index);

    glPopMatrix();


    // Labels
    if(showLabels)
    {
        glDisable(GL_LIGHTING);

        glColor3f(
            1.0f,
            1.0f,
            1.0f
        );

        glRasterPos3f(
            x+p.radius+5,
            8.0f,
            z
        );

        const char* text =
            p.name;

        while(*text)
        {
            glutBitmapCharacter(
                GLUT_BITMAP_HELVETICA_12,
                *text
            );

            text++;
        }

        glEnable(GL_LIGHTING);
    }
}


// =====================================================
// INFORMATION PANEL
// =====================================================

void drawInformationPanel()
{
    if(selectedPlanet==-1)
        return;

    Planet& p =
        planets[selectedPlanet];

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // Background
    glColor3f(
        0.02f,
        0.02f,
        0.05f
    );

    glBegin(GL_QUADS);

    glVertex2f(
        windowWidth-360,
        windowHeight-70
    );

    glVertex2f(
        windowWidth-20,
        windowHeight-70
    );

    glVertex2f(
        windowWidth-20,
        140
    );

    glVertex2f(
        windowWidth-360,
        140
    );

    glEnd();


    // Border
    glColor3f(
        1.0f,
        1.0f,
        0.0f
    );

    glLineWidth(2.0f);

    glBegin(GL_LINE_LOOP);

    glVertex2f(
        windowWidth-360,
        windowHeight-70
    );

    glVertex2f(
        windowWidth-20,
        windowHeight-70
    );

    glVertex2f(
        windowWidth-20,
        140
    );

    glVertex2f(
        windowWidth-360,
        140
    );

    glEnd();


    int x =
        windowWidth-335;

    int y =
        windowHeight-105;


    // Planet name
    glColor3f(
        1.0f,
        1.0f,
        0.0f
    );

    drawText(
        x,
        y,
        p.name,
        GLUT_BITMAP_HELVETICA_18
    );


    glColor3f(
        1.0f,
        1.0f,
        1.0f
    );

    char text[200];


    y-=35;

    sprintf(
        text,
        "Diameter: %s",
        p.diameter
    );

    drawText(x,y,text);


    y-=25;

    sprintf(
        text,
        "Distance from Sun: %s",
        p.distanceFromSun
    );

    drawText(x,y,text);


    y-=25;

    sprintf(
        text,
        "Moons: %s",
        p.moons
    );

    drawText(x,y,text);


    y-=25;

    sprintf(
        text,
        "Rotation: %s",
        p.rotationPeriod
    );

    drawText(x,y,text);


    y-=25;

    sprintf(
        text,
        "Revolution: %s",
        p.revolutionPeriod
    );

    drawText(x,y,text);


    y-=40;

    glColor3f(
        0.8f,
        0.8f,
        0.8f
    );

    drawText(
        x,
        y,
        "1-8: Select planet"
    );

    y-=20;

    drawText(
        x,
        y,
        "Arrow keys: Camera"
    );

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}


// =====================================================
// DISPLAY
// =====================================================

void display()
{
    glClear(
        GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT
    );

    glMatrixMode(
        GL_MODELVIEW
    );

    glLoadIdentity();


    // Camera position

    float camX =
        cameraDistance *
        sinf(cameraAngleY*PI/180.0f) *
        cosf(cameraAngleX*PI/180.0f);

    float camY =
        cameraDistance *
        sinf(cameraAngleX*PI/180.0f);

    float camZ =
        cameraDistance *
        cosf(cameraAngleY*PI/180.0f) *
        cosf(cameraAngleX*PI/180.0f);


    // Camera target
    float targetX=0.0f;
    float targetY=0.0f;
    float targetZ=0.0f;


    if(cameraFollowing &&
       selectedPlanet!=-1)
    {
        Planet& p =
            planets[selectedPlanet];

        float rad =
            p.revolutionAngle*PI/180.0f;

        targetX =
            p.distance*cosf(rad);

        targetZ =
            p.distance*sinf(rad);
    }


    gluLookAt(
        camX,
        camY,
        camZ,

        targetX,
        targetY,
        targetZ,

        0.0f,
        1.0f,
        0.0f
    );


    // =================================================
    // 3D SCENE
    // =================================================

    drawStars();


    for(int i=0;i<planetCount;i++)
        drawOrbit(
            planets[i].distance
        );


    drawAsteroids();


    drawSun();


    for(int i=0;i<planetCount;i++)
        drawPlanet(i);


    // =================================================
    // 2D UI
    // =================================================

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);

    glPushMatrix();

    glLoadIdentity();

    gluOrtho2D(
        0,
        windowWidth,
        0,
        windowHeight
    );


    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();

    glLoadIdentity();


    // Title
    glColor3f(
        1.0f,
        1.0f,
        1.0f
    );

    drawText(
        20,
        windowHeight-30,
        "INTERACTIVE 3D SOLAR SYSTEM",
        GLUT_BITMAP_HELVETICA_18
    );


    // Controls
    drawText(
        20,
        windowHeight-55,
        "1-8 Planet | SPACE Pause | +/- Speed | A Asteroids | O Orbits | S Stars | L Labels | R Reset | ESC Exit"
    );


    // Selected
    if(selectedPlanet!=-1)
    {
        char selectedText[100];

        sprintf(
            selectedText,
            "Selected: %s",
            planets[selectedPlanet].name
        );

        glColor3f(
            1.0f,
            1.0f,
            0.0f
        );

        drawText(
            20,
            windowHeight-85,
            selectedText
        );
    }


    // Pause
    if(paused)
    {
        glColor3f(
            1.0f,
            0.2f,
            0.2f
        );

        drawText(
            windowWidth/2-30,
            windowHeight-30,
            "PAUSED",
            GLUT_BITMAP_HELVETICA_18
        );
    }


    drawInformationPanel();


    glPopMatrix();


    glMatrixMode(
        GL_PROJECTION
    );

    glPopMatrix();


    glMatrixMode(
        GL_MODELVIEW
    );


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);


    glutSwapBuffers();
}


// =====================================================
// UPDATE
// =====================================================

void update(int value)
{
    if(!paused)
    {
        for(int i=0;i<planetCount;i++)
        {
            planets[i].revolutionAngle +=
                planets[i].revolutionSpeed *
                animationSpeed *
                0.15f;

            planets[i].rotationAngle +=
                planets[i].rotationSpeed *
                animationSpeed *
                0.2f;


            if(planets[i].revolutionAngle>=360.0f)
                planets[i].revolutionAngle-=360.0f;


            if(planets[i].rotationAngle>=360.0f)
                planets[i].rotationAngle-=360.0f;
        }


        for(int i=0;i<asteroidCount;i++)
        {
            asteroidAngle[i]+=
                0.05f*animationSpeed;

            if(asteroidAngle[i]>=360.0f)
                asteroidAngle[i]-=360.0f;
        }
    }


    // Smooth camera zoom
    if(cameraFollowing &&
       selectedPlanet!=-1)
    {
        cameraDistance +=
            (targetCameraDistance-
             cameraDistance)*0.08f;
    }


    glutPostRedisplay();


    glutTimerFunc(
        16,
        update,
        0
    );
}


// =====================================================
// KEYBOARD
// =====================================================

void keyboard(
    unsigned char key,
    int x,
    int y)
{
    switch(key)
    {
        case '1':
            selectedPlanet=0;
            cameraFollowing=true;
            targetCameraDistance=180.0f;
            break;

        case '2':
            selectedPlanet=1;
            cameraFollowing=true;
            targetCameraDistance=200.0f;
            break;

        case '3':
            selectedPlanet=2;
            cameraFollowing=true;
            targetCameraDistance=190.0f;
            break;

        case '4':
            selectedPlanet=3;
            cameraFollowing=true;
            targetCameraDistance=190.0f;
            break;

        case '5':
            selectedPlanet=4;
            cameraFollowing=true;
            targetCameraDistance=250.0f;
            break;

        case '6':
            selectedPlanet=5;
            cameraFollowing=true;
            targetCameraDistance=250.0f;
            break;

        case '7':
            selectedPlanet=6;
            cameraFollowing=true;
            targetCameraDistance=240.0f;
            break;

        case '8':
            selectedPlanet=7;
            cameraFollowing=true;
            targetCameraDistance=240.0f;
            break;


        case ' ':
            paused=!paused;
            break;


        case '+':
        case '=':

            animationSpeed+=0.2f;

            if(animationSpeed>5.0f)
                animationSpeed=5.0f;

            break;


        case '-':
        case '_':

            animationSpeed-=0.2f;

            if(animationSpeed<0.2f)
                animationSpeed=0.2f;

            break;


        case 'a':
        case 'A':

            showAsteroids=
                !showAsteroids;

            break;


        case 'o':
        case 'O':

            showOrbits=
                !showOrbits;

            break;


        case 's':
        case 'S':

            showStars=
                !showStars;

            break;


        case 'l':
        case 'L':

            showLabels=
                !showLabels;

            break;


        case 'm':
        case 'M':

            showMoons=
                !showMoons;

            break;


        case 'g':
        case 'G':

            showAtmosphere=
                !showAtmosphere;

            break;


        case 'r':
        case 'R':

            animationSpeed=1.0f;

            paused=false;

            selectedPlanet=-1;

            cameraFollowing=false;

            cameraDistance=850.0f;

            cameraAngleX=25.0f;

            cameraAngleY=0.0f;

            targetCameraDistance=850.0f;

            showAsteroids=true;

            showOrbits=true;

            showStars=true;

            showLabels=true;

            showMoons=true;

            showAtmosphere=true;


            for(int i=0;i<planetCount;i++)
            {
                planets[i].revolutionAngle=
                    i*45.0f;

                planets[i].rotationAngle=
                    0.0f;
            }

            break;


        case 27:

            exit(0);

            break;
    }


    glutPostRedisplay();
}


// =====================================================
// SPECIAL KEYBOARD
// =====================================================

void specialKeyboard(
    int key,
    int x,
    int y)
{
    switch(key)
    {
        case GLUT_KEY_UP:

            cameraFollowing=false;

            cameraAngleX+=3.0f;

            if(cameraAngleX>80.0f)
                cameraAngleX=80.0f;

            break;


        case GLUT_KEY_DOWN:

            cameraFollowing=false;

            cameraAngleX-=3.0f;

            if(cameraAngleX<-20.0f)
                cameraAngleX=-20.0f;

            break;


        case GLUT_KEY_LEFT:

            cameraFollowing=false;

            cameraAngleY-=3.0f;

            break;


        case GLUT_KEY_RIGHT:

            cameraFollowing=false;

            cameraAngleY+=3.0f;

            break;
    }


    glutPostRedisplay();
}


// =====================================================
// MOUSE
// =====================================================

void mouse(
    int button,
    int state,
    int x,
    int y)
{
    if(state!=GLUT_DOWN)
        return;


    if(button==3)
    {
        cameraFollowing=false;

        cameraDistance-=40.0f;

        if(cameraDistance<80.0f)
            cameraDistance=80.0f;
    }


    if(button==4)
    {
        cameraFollowing=false;

        cameraDistance+=40.0f;

        if(cameraDistance>1500.0f)
            cameraDistance=1500.0f;
    }


    glutPostRedisplay();
}


// =====================================================
// RESHAPE
// =====================================================

void reshape(
    int w,
    int h)
{
    if(h==0)
        h=1;


    windowWidth=w;
    windowHeight=h;


    glViewport(
        0,
        0,
        w,
        h
    );


    glMatrixMode(
        GL_PROJECTION
    );

    glLoadIdentity();


    gluPerspective(
        45.0,
        (double)w/(double)h,
        1.0,
        3000.0
    );


    glMatrixMode(
        GL_MODELVIEW
    );
}


// =====================================================
// LIGHTING
// =====================================================

void setupLighting()
{
    glEnable(GL_LIGHTING);

    glEnable(GL_LIGHT0);


    GLfloat lightPosition[] =
    {
        0.0f,
        0.0f,
        0.0f,
        1.0f
    };


    GLfloat lightAmbient[] =
    {
        0.08f,
        0.08f,
        0.08f,
        1.0f
    };


    GLfloat lightDiffuse[] =
    {
        1.0f,
        0.85f,
        0.55f,
        1.0f
    };


    GLfloat lightSpecular[] =
    {
        1.0f,
        1.0f,
        1.0f,
        1.0f
    };


    glLightfv(
        GL_LIGHT0,
        GL_POSITION,
        lightPosition
    );


    glLightfv(
        GL_LIGHT0,
        GL_AMBIENT,
        lightAmbient
    );


    glLightfv(
        GL_LIGHT0,
        GL_DIFFUSE,
        lightDiffuse
    );


    glLightfv(
        GL_LIGHT0,
        GL_SPECULAR,
        lightSpecular
    );


    glEnable(GL_COLOR_MATERIAL);


    glColorMaterial(
        GL_FRONT,
        GL_AMBIENT_AND_DIFFUSE
    );


    glShadeModel(
        GL_SMOOTH
    );
}


// =====================================================
// INITIALIZATION
// =====================================================

void init()
{
    glClearColor(
        0.003f,
        0.003f,
        0.015f,
        1.0f
    );


    glEnable(
        GL_DEPTH_TEST
    );


    glEnable(
        GL_NORMALIZE
    );


    setupLighting();

    initializeAsteroids();
}


// =====================================================
// MAIN
// =====================================================

int main(
    int argc,
    char** argv)
{
    glutInit(
        &argc,
        argv
    );


    glutInitDisplayMode(
        GLUT_DOUBLE |
        GLUT_RGB |
        GLUT_DEPTH
    );


    glutInitWindowSize(
        windowWidth,
        windowHeight
    );


    glutInitWindowPosition(
        100,
        50
    );


    glutCreateWindow(
        "Interactive 3D Solar System"
    );


    init();


    glutDisplayFunc(
        display
    );


    glutReshapeFunc(
        reshape
    );


    glutKeyboardFunc(
        keyboard
    );


    glutSpecialFunc(
        specialKeyboard
    );


    glutMouseFunc(
        mouse
    );


    glutTimerFunc(
        0,
        update,
        0
    );


    glutMainLoop();


    return 0;
}
