#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

#ifdef APPLE
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <GL/glu.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdio>

#include <cstring>


float randomFloat(float minValue, float maxValue)
{
    if (minValue > maxValue)
    {
        float temp = minValue;
        minValue = maxValue;
        maxValue = temp;
    }

    return minValue +
           (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) *
           (maxValue - minValue);
}




enum GameState
{
    PLAYING,
    PAUSED,
    GAMEOVER
};

GameState gameState = PLAYING;


float playerX = 0.0f;
float playerY = 0.0f;
float playerZ = 6.0f;
bool firstPersonView = false;

float laneX[3] =
{
    -3.0f,
     0.0f,
     3.0f
};

int currentLane = 1;

bool jumping = false;
float jumpHeight = 0.0f;
float jumpVelocity = 0.0f;

float runTime = 0.0f;


int score = 0;
int highScore = 0;
int coinsCollected = 0;

int level = 1;

float gameSpeed = 0.22f;

float targetPlayerX = 0.0f;
bool laneChanging = false;
const float LANE_CHANGE_SPEED = 0.14f;

float destinationZ = -180.0f;
int completedLevels = 0;

int configuredVehicleCount = 0;
int configuredObstacleCount = 0;



int lives = 3;
float fuel = 100.0f;
float health = 100.0f;
float stamina = 100.0f;




float worldTime = 0.0f;

float dayCycleSpeed = 0.00035f;

bool raining = false;
bool fogEnabled = false;




struct Brick
{
    int lane;
    float z;

    int type;

    bool active;
    bool saved;

    float rotation;
};

const int MAX_BRICKS = 32;

Brick bricks[MAX_BRICKS];



struct Coin
{
    int lane;
    float z;

    bool active;

    float rotation;
};

const int MAX_COINS = 40;

Coin coins[MAX_COINS];


enum PowerType
{
    POWER_SHIELD,
    POWER_SPEED,
    POWER_MAGNET,
    POWER_DESTROYER,
    POWER_SUPERJUMP,
    POWER_EXTRALIFE,
    POWER_SLOWMO,
    POWER_AUTOSAVE,
    POWER_COUNT
};

struct PowerUp
{
    int lane;
    float z;

    int type;

    bool active;

    float rotation;
};

const int MAX_POWERUPS = 10;

PowerUp powerUps[MAX_POWERUPS];

bool shieldActive = false;
bool speedBoostActive = false;
bool magnetActive = false;
bool destroyerActive = false;
bool superJumpActive = false;
bool slowMotionActive = false;
bool autoSaveActive = false;

float shieldTimer = 0.0f;
float speedBoostTimer = 0.0f;
float magnetTimer = 0.0f;
float destroyerTimer = 0.0f;
float superJumpTimer = 0.0f;
float slowMotionTimer = 0.0f;
float autoSaveTimer = 0.0f;




enum VehicleType
{
    CAR,
    TAXI,
    BUS,
    POLICE,
    AMBULANCE,
    TRUCK
};

struct Vehicle
{
    int lane;
    float z;

    int type;

    bool active;

    float speedMultiplier;
};

const int MAX_VEHICLES = 8;

Vehicle vehicles[MAX_VEHICLES];




enum ObstacleType
{
    CONE,
    BARRIER,
    POTHOLE,
    FIRE
};

struct Obstacle
{
    int lane;
    float z;

    int type;

    bool active;
};

const int MAX_OBSTACLES = 15;

Obstacle obstacles[MAX_OBSTACLES];




const int BUILDING_COUNT = 36;

float buildingZ[BUILDING_COUNT];
int buildingSide[BUILDING_COUNT];
int buildingType[BUILDING_COUNT];



const int TREE_COUNT = 45;

float treeZ[TREE_COUNT];
int treeSide[TREE_COUNT];




const int LAMP_COUNT = 35;

float lampZ[LAMP_COUNT];
int lampSide[LAMP_COUNT];




const int BILLBOARD_COUNT = 15;

float billboardZ[BILLBOARD_COUNT];
int billboardSide[BILLBOARD_COUNT];




const int TRAFFIC_COUNT = 14;

float trafficZ[TRAFFIC_COUNT];




struct Pedestrian
{
    float x;
    float z;

    int direction;
    float animation;
    float crossingSpeed;
    int crossingIndex;

    bool crossing;
    bool active;
};

const int MAX_PEDESTRIANS = 18;
Pedestrian pedestrians[MAX_PEDESTRIANS];

const int ZEBRA_COUNT = 5;
float zebraZ[ZEBRA_COUNT];




struct Bird
{
    float x;
    float y;
    float z;
    float speed;
    float wing;
    float phase;
    bool active;
};

const int BIRD_COUNT = 14;
Bird birds[BIRD_COUNT];




void drawBird(float x, float y, float z, float wing)
{
    glDisable(GL_LIGHTING);
    glColor3f(0.08f, 0.09f, 0.11f);

    glPushMatrix();
    glTranslatef(x, y, z);

    glPushMatrix();
    glScalef(0.20f, 0.10f, 0.42f);
    glutSolidSphere(1.0f, 10, 8);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.02f, 0.34f);
    glutSolidSphere(0.12f, 10, 8);
    glPopMatrix();

    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, 0.02f, 0.48f);
    glVertex3f(0.0f, -0.01f, 0.66f);
    glVertex3f(0.07f, 0.00f, 0.48f);
    glEnd();

    glPushMatrix();
    glTranslatef(-0.16f, 0.02f, 0.0f);
    glRotatef(wing, 0.0f, 0.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, 0.0f, 0.12f);
    glVertex3f(-0.95f, 0.10f, -0.05f);
    glVertex3f(-0.75f, 0.0f, -0.35f);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.16f, 0.02f, 0.0f);
    glRotatef(-wing, 0.0f, 0.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, 0.0f, 0.12f);
    glVertex3f(0.95f, 0.10f, -0.05f);
    glVertex3f(0.75f, 0.0f, -0.35f);
    glEnd();
    glPopMatrix();

    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void drawBirds()
{
    for (int i = 0; i < BIRD_COUNT; ++i)
    {
        if (birds[i].active)
            drawBird(
                birds[i].x,
                birds[i].y,
                birds[i].z,
                birds[i].wing
            );
    }
}

void updateBirds()
{
    for (int i = 0; i < BIRD_COUNT; ++i)
    {
        if (!birds[i].active)
            continue;

        birds[i].x += birds[i].speed;
        birds[i].z += gameSpeed * 0.10f;
        birds[i].phase += 0.10f;

        birds[i].wing = sin(birds[i].phase) * 28.0f;

        birds[i].y += sin(birds[i].phase * 0.35f) * 0.006f;

        if (birds[i].x > 48.0f || birds[i].x < -48.0f)
        {
            birds[i].x = (birds[i].speed > 0.0f) ? -48.0f : 48.0f;
            birds[i].y = randomFloat(15.0f, 29.0f);
            birds[i].z = randomFloat(-120.0f, -20.0f);
        }

        if (birds[i].z > 20.0f)
            birds[i].z = randomFloat(-130.0f, -50.0f);
    }
}


const int RAIN_COUNT = 300;

float rainX[RAIN_COUNT];
float rainY[RAIN_COUNT];
float rainZ[RAIN_COUNT];



const int STAR_COUNT = 100;

float starX[STAR_COUNT];
float starY[STAR_COUNT];
float starZ[STAR_COUNT];



const int CLOUD_COUNT = 12;

float cloudX[CLOUD_COUNT];
float cloudY[CLOUD_COUNT];
float cloudZ[CLOUD_COUNT];

float trainZ = -38.0f;
float boatZ = -28.0f;
float tunnelZ = -68.0f;
float featureTime = 0.0f;

float planeX = -42.0f;
float planeY = 25.0f;
float planeZ = -82.0f;
float planeSpeed = 0.12f;



struct Particle
{
    float x;
    float y;
    float z;

    float vx;
    float vy;
    float vz;

    float life;

    bool active;
};

const int MAX_PARTICLES = 180;

Particle particles[MAX_PARTICLES];



bool saveEffect = false;

float effectX = 0.0f;
float effectY = 0.0f;
float effectZ = 0.0f;

float effectTime = 0.0f;



float cameraShake = 0.0f;



int bricksSavedTotal = 0;
int carsAvoided = 0;

bool mission1Complete = false;
bool mission2Complete = false;
bool mission3Complete = false;



void resetGame();

void createBrick(int index, float z);
void createCoin(int index, float z);
void createVehicle(int index, float z);
void createObstacle(int index, float z);
void createPowerUp(int index, float z);
void activatePower(int type);
void destroyNearbyThreats();
void autoSaveAllNearbyBricks();

void moveLeft();
void moveRight();

void saveBrick();
void autoSaveAfterLaneChange();

void updateGame();
void updateLevelDifficulty(bool forceUpdate = false);
int getLevelFromScore();
float getLevelBaseSpeed(int currentLevel);
int getVehicleCountForLevel(int currentLevel);
int getObstacleCountForLevel(int currentLevel);

void display();
void updateBirds();
void drawBird(float x, float y, float z, float wing);
void drawBirds();
void drawPlane();
void updatePlane();
void drawRainbow();
void drawWorldFeatures();
void updateWorldFeatures();
void updateClouds();
void drawNormalRoadSurface();
void drawDriverInsideCar();
void drawFirstPersonCockpit();
void startCarAudio();
void playHorn();
void updatePedestrianCrossings();
bool shouldStopForPedestrians(float z);
bool trafficLightIsRed(float z);
void drawZebraCrossings();
void drawDestinationMarker();
float roadCurveX(float z);
void updateSmoothLaneChange();
bool objectsTooClose(float z, int ignoreObstacle);
void startNextLevel();







bool isNight()
{
    return (worldTime >= 0.55f ||
            worldTime <= 0.10f);
}


bool isSunset()
{
    return (worldTime > 0.42f &&
            worldTime < 0.55f);
}



void drawCylinder(float radius, float height)
{
    GLUquadric* quad =
        gluNewQuadric();

    if (quad == NULL)
        return;

    gluCylinder(
        quad,
        radius,
        radius,
        height,
        16,
        8
    );

    gluDeleteQuadric(quad);
}




void drawSphere(float radius)
{
    glutSolidSphere(
        radius,
        16,
        12
    );
}



void drawCube(
    float x,
    float y,
    float z
)
{
    glPushMatrix();

    glScalef(x, y, z);

    glutSolidCube(1.0f);

    glPopMatrix();
}




void drawHumanLimb(float x, float y, float z,
                   float length, float thickness,
                   float angle, bool arm)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(angle, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, -length * 0.5f, 0.0f);
    if (arm) drawCylinder(thickness, length);
    else
    {
        glScalef(thickness, length * 0.5f, thickness);
        glutSolidSphere(1.0f, 14, 10);
    }
    glPopMatrix();
}

void drawRealisticHuman(float x, float z, float animation,
                        float scale = 1.0f, int outfit = 0)
{
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glScalef(scale, scale, scale);

    float swing = sin(animation * 8.0f) * 25.0f;
    float opposite = -swing;

    glColor3f(0.025f,0.025f,0.03f);
    for (int side=-1; side<=1; side+=2)
    {
        glPushMatrix();
        glTranslatef(side*0.23f,0.12f,0.10f);
        glScalef(0.25f,0.14f,0.48f);
        glutSolidSphere(1.0f,14,10);
        glPopMatrix();
    }

    glColor3f(0.08f,0.10f,0.15f);
    for (int side=-1; side<=1; side+=2)
    {
        float a = (side < 0) ? swing : opposite;
        glPushMatrix();
        glTranslatef(side*0.22f,0.82f,0.0f);
        glRotatef(a,1,0,0);
        glTranslatef(0,-0.42f,0);
        drawCube(0.23f,0.85f,0.28f);
        glPopMatrix();
    }


    glColor3f(0.035f,0.035f,0.04f);
    glPushMatrix();
    glTranslatef(0,1.25f,0);
    drawCube(0.62f,0.13f,0.38f);
    glPopMatrix();

    if (outfit == 1) glColor3f(0.15f,0.33f,0.70f);
    else if (outfit == 2) glColor3f(0.65f,0.12f,0.10f);
    else glColor3f(0.12f,0.32f,0.72f);

    glPushMatrix();
    glTranslatef(0,1.75f,0);
    drawCube(0.75f,1.05f,0.48f);
    glPopMatrix();

    glColor3f(0.12f,0.32f,0.72f);
    for (int side=-1; side<=1; side+=2)
    {
        glPushMatrix();
        glTranslatef(side*0.52f,2.05f,0.0f);
        glScalef(0.16f,0.16f,0.16f);
        glutSolidSphere(1.0f,12,10);
        glPopMatrix();
    }

    glColor3f(0.10f,0.12f,0.18f);
    for (int side=-1; side<=1; side+=2)
    {
        glPushMatrix();
        glTranslatef(side*0.22f,0.55f,0.12f);
        glScalef(0.12f,0.14f,0.10f);
        glutSolidSphere(1.0f,10,8);
        glPopMatrix();
    }

    glColor3f(0.82f,0.84f,0.88f);
    glPushMatrix();
    glTranslatef(0.0f,2.25f,0.245f);
    glScalef(0.30f,0.06f,0.035f);
    glutSolidCube(1.0f);
    glPopMatrix();


    glColor3f(0.72f,0.45f,0.30f);
    glPushMatrix();
    glTranslatef(0,2.35f,0);
    glScalef(0.18f,0.28f,0.18f);
    glutSolidSphere(1.0f,14,10);
    glPopMatrix();

    for (int side=-1; side<=1; side+=2)
    {
        float a = (side < 0) ? opposite : swing;

        glColor3f(0.10f,0.22f,0.55f);
        glPushMatrix();
        glTranslatef(side*0.50f,2.02f,0);
        glRotatef(a,1,0,0);
        glTranslatef(0,-0.34f,0);
        drawCube(0.22f,0.68f,0.22f);
        glPopMatrix();

        glColor3f(0.72f,0.45f,0.30f);
        glPushMatrix();
        glTranslatef(side*0.50f,1.67f,0);
        glRotatef(a,1,0,0);
        glTranslatef(0,-0.30f,0);
        drawCube(0.19f,0.60f,0.19f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(side*0.50f,1.30f,0);
        glScalef(0.14f,0.16f,0.14f);
        glutSolidSphere(1.0f,12,8);
        glPopMatrix();
    }


    glColor3f(0.76f,0.49f,0.34f);
    glPushMatrix();
    glTranslatef(0,2.72f,0);
    glScalef(0.36f,0.42f,0.34f);
    glutSolidSphere(1.0f,20,16);
    glPopMatrix();


    glColor3f(0.035f,0.025f,0.02f);
    glPushMatrix();
    glTranslatef(0,3.02f,-0.01f);
    glScalef(0.38f,0.22f,0.36f);
    glutSolidSphere(1.0f,18,12);
    glPopMatrix();


    glColor3f(0.70f,0.42f,0.29f);
    for (int side=-1; side<=1; side+=2)
    {
        glPushMatrix();
        glTranslatef(side*0.36f,2.72f,0);
        glScalef(0.07f,0.12f,0.10f);
        glutSolidSphere(1,10,8);
        glPopMatrix();
    }

    glDisable(GL_LIGHTING);
    glColor3f(0.02f,0.02f,0.02f);
    for (int side=-1; side<=1; side+=2)
    {
        glPushMatrix();
        glTranslatef(side*0.13f,2.79f,0.315f);
        glScalef(0.045f,0.045f,0.025f);
        glutSolidSphere(1,10,8);
        glPopMatrix();
    }


    glPushMatrix();
    glTranslatef(0,2.70f,0.35f);
    glScalef(0.055f,0.07f,0.12f);
    glutSolidSphere(1,10,8);
    glPopMatrix();


    glBegin(GL_LINE_STRIP);
    glVertex3f(-0.08f,2.59f,0.335f);
    glVertex3f(0.0f,2.57f,0.35f);
    glVertex3f(0.08f,2.59f,0.335f);
    glEnd();
    glEnable(GL_LIGHTING);

    glPopMatrix();
}


void drawPlayerCar()
{
    glPushMatrix();

    glColor3f(0.08f, 0.32f, 0.78f);
    glPushMatrix();
    glTranslatef(0.0f, 0.42f, 0.0f);
    glScalef(0.88f, 0.38f, 1.48f);
    glutSolidSphere(1.0f, 24, 16);
    glPopMatrix();

    glColor3f(0.035f, 0.05f, 0.08f);
    glPushMatrix();
    glTranslatef(0.0f, 0.30f, -1.43f);
    drawCube(1.40f, 0.20f, 0.16f);
    glPopMatrix();


    glColor3f(0.10f, 0.38f, 0.90f);
    glPushMatrix();
    glTranslatef(0.0f, 0.72f, -0.70f);
    drawCube(1.38f, 0.16f, 0.95f);
    glPopMatrix();

    glColor3f(0.055f, 0.18f, 0.38f);
    glPushMatrix();
    glTranslatef(0.0f, 0.92f, 0.18f);
    glTranslatef(0.0f, 0.36f, 0.0f);
    drawCube(1.30f, 0.18f, 1.55f);
    glPopMatrix();

    glColor3f(0.035f, 0.07f, 0.12f);
    for (int side = -1; side <= 1; side += 2)
    {
        for (int frontRear = -1; frontRear <= 1; frontRear += 2)
        {
            glPushMatrix();
            glTranslatef(side * 0.60f, 1.10f, 0.18f + frontRear * 0.63f);
            glScalef(0.09f, 0.45f, 0.09f);
            glutSolidCube(1.0f);
            glPopMatrix();
        }
    }

    glDisable(GL_LIGHTING);
    glColor4f(0.04f, 0.16f, 0.23f, 0.32f);
    glPushMatrix();
    glTranslatef(0.0f, 1.00f, -0.53f);
    glScalef(0.98f, 0.36f, 0.05f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(0.035f, 0.12f, 0.18f, 0.32f);
    glPushMatrix();
    glTranslatef(0.0f, 1.00f, 0.82f);
    glScalef(0.98f, 0.34f, 0.05f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(0.035f, 0.12f, 0.18f, 0.28f);
    for (int side = -1; side <= 1; side += 2)
    {
        glPushMatrix();
        glTranslatef(side * 0.67f, 1.00f, 0.18f);
        glScalef(0.035f, 0.28f, 0.58f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    glEnable(GL_LIGHTING);
    glColor3f(0.04f, 0.06f, 0.09f);
    for (int side = -1; side <= 1; side += 2)
    {
        glPushMatrix();
        glTranslatef(side * 0.76f, 0.88f, -0.30f);
        glScalef(0.10f, 0.10f, 0.22f);
        glutSolidSphere(1.0, 10, 8);
        glPopMatrix();
    }

    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.95f, 0.62f);
    for (int side = -1; side <= 1; side += 2)
    {
        glPushMatrix();
        glTranslatef(side * 0.48f, 0.48f, -1.53f);
        glScalef(0.25f, 0.13f, 0.035f);
        glutSolidSphere(1.0, 12, 8);
        glPopMatrix();
    }

    glColor3f(0.95f, 0.05f, 0.03f);
    for (int side = -1; side <= 1; side += 2)
    {
        glPushMatrix();
        glTranslatef(side * 0.50f, 0.50f, 1.40f);
        glScalef(0.22f, 0.12f, 0.035f);
        glutSolidSphere(1.0, 12, 8);
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);

    glColor3f(0.025f, 0.025f, 0.03f);
    const float wheelX = 0.82f;
    const float wheelZ = 0.86f;
    for (int side = -1; side <= 1; side += 2)
    {
        for (int frontRear = -1; frontRear <= 1; frontRear += 2)
        {
            glPushMatrix();
            glTranslatef(side * wheelX, 0.32f, frontRear * wheelZ);
            glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
            glutSolidTorus(0.16, 0.31, 12, 18);
            glPopMatrix();
        }
    }

    glColor3f(0.55f, 0.58f, 0.62f);
    for (int side = -1; side <= 1; side += 2)
    {
        for (int frontRear = -1; frontRear <= 1; frontRear += 2)
        {
            glPushMatrix();
            glTranslatef(side * 0.83f, 0.32f, frontRear * wheelZ);
            glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
            glutSolidTorus(0.055, 0.13, 10, 14);
            glPopMatrix();
        }
    }

    glColor3f(0.035f, 0.06f, 0.10f);
    glPushMatrix();
    glTranslatef(0.0f, 0.92f, 1.30f);
    drawCube(1.05f, 0.10f, 0.18f);
    glPopMatrix();

    glDisable(GL_LIGHTING);
    glColor3f(0.15f, 0.85f, 1.0f);
    for (int side = -1; side <= 1; side += 2)
    {
        glPushMatrix();
        glTranslatef(side * 0.79f, 0.53f, -0.05f);
        drawCube(0.025f, 0.055f, 1.55f);
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void drawDriverInsideCar()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glColor3f(0.06f, 0.08f, 0.12f);
    glPushMatrix();
    glTranslatef(0.0f, 0.78f, 0.48f);
    glScalef(0.38f, 0.58f, 0.18f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.08f, 0.20f, 0.62f);
    glPushMatrix();
    glTranslatef(0.0f, 0.96f, 0.08f);
    glScalef(0.34f, 0.50f, 0.22f);
    glutSolidSphere(1.0f, 18, 14);
    glPopMatrix();

    glColor3f(0.76f, 0.49f, 0.34f);
    glPushMatrix();
    glTranslatef(0.0f, 1.30f, -0.02f);
    glScalef(0.10f, 0.14f, 0.10f);
    glutSolidSphere(1.0f, 16, 12);
    glPopMatrix();

    glColor3f(0.76f, 0.49f, 0.34f);
    glPushMatrix();
    glTranslatef(0.0f, 1.58f, -0.06f);
    glScalef(0.25f, 0.30f, 0.21f);
    glutSolidSphere(1.0f, 20, 16);
    glPopMatrix();

    glColor3f(0.035f, 0.025f, 0.02f);
    glPushMatrix();
    glTranslatef(0.0f, 1.78f, -0.08f);
    glScalef(0.27f, 0.12f, 0.22f);
    glutSolidSphere(1.0f, 16, 10);
    glPopMatrix();

    glColor3f(0.02f, 0.02f, 0.02f);
    for (int side = -1; side <= 1; side += 2)
    {
        glPushMatrix();
        glTranslatef(side * 0.09f, 1.63f, -0.26f);
        glScalef(0.035f, 0.035f, 0.025f);
        glutSolidSphere(1.0f, 10, 8);
        glPopMatrix();
    }

    glColor3f(0.08f, 0.20f, 0.62f);
    for (int side = -1; side <= 1; side += 2)
    {
        glPushMatrix();
        glTranslatef(side * 0.25f, 1.12f, -0.08f);
        glRotatef(side * 18.0f, 0.0f, 0.0f, 1.0f);
        glScalef(0.10f, 0.30f, 0.10f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    glColor3f(0.76f, 0.49f, 0.34f);
    for (int side = -1; side <= 1; side += 2)
    {
        glPushMatrix();
        glTranslatef(side * 0.20f, 0.92f, -0.48f);
        glRotatef(side * 24.0f, 0.0f, 0.0f, 1.0f);
        glScalef(0.075f, 0.32f, 0.075f);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(side * 0.18f, 0.78f, -0.68f);
        glScalef(0.10f, 0.10f, 0.10f);
        glutSolidSphere(1.0f, 12, 8);
        glPopMatrix();
    }

    glColor3f(0.025f, 0.025f, 0.03f);
    glPushMatrix();
    glTranslatef(0.0f, 0.78f, -0.70f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidTorus(0.055, 0.22, 12, 20);
    glPopMatrix();

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

void drawFirstPersonCockpit()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glColor3f(0.035f, 0.045f, 0.06f);
    glPushMatrix();
    glTranslatef(playerX, 0.82f, playerZ - 0.70f);
    drawCube(2.2f, 0.28f, 0.70f);
    glPopMatrix();

    glColor3f(0.05f, 0.12f, 0.16f);
    glPushMatrix();
    glTranslatef(playerX, 1.02f, playerZ - 0.92f);
    drawCube(0.70f, 0.24f, 0.06f);
    glPopMatrix();

    glColor3f(0.15f, 0.80f, 0.95f);
    glPushMatrix();
    glTranslatef(playerX, 1.03f, playerZ - 0.97f);
    drawCube(0.20f, 0.08f, 0.025f);
    glPopMatrix();

    glColor3f(0.015f, 0.018f, 0.022f);
    glPushMatrix();
    glTranslatef(playerX, 1.02f, playerZ - 1.02f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidTorus(0.07, 0.30, 14, 24);
    glPopMatrix();

    glColor3f(0.76f, 0.49f, 0.34f);
    for (int side = -1; side <= 1; side += 2)
    {
        glPushMatrix();
        glTranslatef(playerX + side * 0.22f, 1.17f, playerZ - 1.00f);
        glScalef(0.11f, 0.11f, 0.11f);
        glutSolidSphere(1.0f, 12, 8);
        glPopMatrix();
    }

    glColor3f(0.025f, 0.035f, 0.05f);
    for (int side = -1; side <= 1; side += 2)
    {
        glPushMatrix();
        glTranslatef(playerX + side * 0.92f, 1.42f, playerZ - 0.52f);
        glRotatef(side * 12.0f, 0.0f, 0.0f, 1.0f);
        drawCube(0.09f, 1.15f, 0.10f);
        glPopMatrix();
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}


void drawPlayer()
{
    glPushMatrix();
    glTranslatef(playerX, playerY, playerZ);

    drawPlayerCar();

    drawDriverInsideCar();

    if (shieldActive)
    {
        glDisable(GL_LIGHTING);
        glColor4f(0.1f, 0.8f, 1.0f, 0.25f);
        glutWireSphere(2.0f, 24, 24);
        glEnable(GL_LIGHTING);
    }

    glPopMatrix();
}


void drawBrick(
    float x,
    float z,
    int type
)
{
    glPushMatrix();

    glTranslatef(
        x,
        0.65f,
        z
    );

    if (type == 0)
    {
        glColor3f(
            0.85f,
            0.08f,
            0.05f
        );
    }
    else if (type == 1)
    {
        glColor3f(
            1.0f,
            0.45f,
            0.02f
        );
    }
    else if (type == 2)
    {
        glColor3f(
            0.55f,
            0.05f,
            0.85f
        );
    }
    else
    {
        glColor3f(
            1.0f,
            0.75f,
            0.05f
        );
    }

    glRotatef(
        5.0f,
        0.0f,
        1.0f,
        0.0f
    );

    drawCube(
        1.15f,
        1.0f,
        1.0f
    );

    glColor3f(
        0.25f,
        0.10f,
        0.05f
    );

    glPushMatrix();

    glTranslatef(
        0.0f,
        0.0f,
        0.51f
    );

    drawCube(
        0.8f,
        0.15f,
        0.03f
    );

    glPopMatrix();

    glPopMatrix();
}



bool crossingHasPedestrians(int crossingIndex)
{
    for (int i = 0; i < MAX_PEDESTRIANS; ++i)
    {
        if (!pedestrians[i].active || !pedestrians[i].crossing)
            continue;

        if (pedestrians[i].crossingIndex != crossingIndex)
            continue;

        if (fabs(pedestrians[i].x) < 5.2f)
            return true;
    }
    return false;
}

bool shouldStopForPedestrians(float z)
{
    for (int c = 0; c < ZEBRA_COUNT; ++c)
    {
        if (!crossingHasPedestrians(c))
            continue;

        if (fabs(zebraZ[c] - z) < 9.0f)
            return true;
    }
    return false;
}

bool trafficLightIsRed(float z)
{
    float cycle = fmod(runTime, 6.0f);
    if (cycle >= 2.5f)
        return false;

    for (int i = 0; i < TRAFFIC_COUNT; ++i)
    {
        if (fabs(trafficZ[i] - z) < 5.5f)
            return true;
    }
    return false;
}

void drawZebraCrossings()
{
    glDisable(GL_LIGHTING);

    for (int c = 0; c < ZEBRA_COUNT; ++c)
    {
        for (int stripe = -4; stripe <= 4; ++stripe)
        {
            glColor3f(0.92f, 0.92f, 0.90f);
            glPushMatrix();
            float crossingZ = zebraZ[c] + stripe * 0.72f;
            glTranslatef(roadCurveX(crossingZ), 0.025f, crossingZ);
            drawCube(4.70f, 0.018f, 0.24f);
            glPopMatrix();
        }
    }

    glEnable(GL_LIGHTING);
}

void drawNormalRoadSurface()
{
    glDisable(GL_LIGHTING);
    glColor3f(0.42f, 0.43f, 0.45f);
    glBegin(GL_QUADS);
    glVertex3f(-5.0f, 0.015f, 0.0f);
    glVertex3f(5.0f, 0.015f, 0.0f);
    glVertex3f(5.0f, 0.015f, -180.0f);
    glVertex3f(-5.0f, 0.015f, -180.0f);
    glEnd();

    glColor3f(0.96f, 0.96f, 0.90f);
    for (int i = 0; i < 26; ++i)
    {
        float z = -i * 7.0f - 1.5f;
        glBegin(GL_QUADS);
        glVertex3f(-0.06f, 0.025f, z - 1.1f);
        glVertex3f(0.06f, 0.025f, z - 1.1f);
        glVertex3f(0.06f, 0.025f, z + 1.1f);
        glVertex3f(-0.06f, 0.025f, z + 1.1f);
        glEnd();
    }

    glEnable(GL_LIGHTING);
}

void startCarAudio()
{
#ifdef _WIN32

#else

    system("sh -c 'while true; do paplay car_engine.wav; done' >/dev/null 2>&1 &");
#endif
}

void playHorn()
{
#ifdef _WIN32

#else
    system("paplay car_horn.wav >/dev/null 2>&1 &");
#endif
}

void drawRoad()
{
    drawNormalRoadSurface();

    glDisable(GL_LIGHTING);
    glColor3f(0.42f, 0.43f, 0.45f);

    glBegin(GL_QUADS);
    glVertex3f(-10.0f, -0.10f, 0.0f);
    glVertex3f(10.0f, -0.10f, 0.0f);
    glVertex3f(10.0f, -0.10f, -180.0f);
    glVertex3f(-10.0f, -0.10f, -180.0f);
    glEnd();

    glBegin(GL_QUAD_STRIP);
    for (int segment = 0; segment <= 60; ++segment)
    {
        float roadZ = -segment * 3.0f;
        float centerX = roadCurveX(roadZ);
        glVertex3f(centerX - 5.0f, -0.20f, roadZ);
        glVertex3f(centerX + 5.0f, -0.20f, roadZ);
    }
    glEnd();

    glDisable(GL_LIGHTING);

    for (int i = 0; i < 55; ++i)
    {
        float z = -i * 7.0f;

        glColor3f(0.95f, 0.94f, 0.78f);

        glPushMatrix();
        glTranslatef(-1.5f + roadCurveX(z), 0.025f, z);
        drawCube(0.055f, 0.018f, 2.2f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(1.5f + roadCurveX(z), 0.025f, z);
        drawCube(0.055f, 0.018f, 2.2f);
        glPopMatrix();
    }

    glColor3f(0.90f, 0.88f, 0.68f);
    for (int side = -1; side <= 1; side += 2)
    {
        for (int i = 0; i < 55; ++i)
        {
            float z = -i * 3.5f;
            glPushMatrix();
            glTranslatef(side * 4.75f + roadCurveX(z), 0.025f, z);
            drawCube(0.07f, 0.018f, 1.5f);
            glPopMatrix();
        }
    }

    glColor3f(0.48f, 0.49f, 0.50f);
    for (int side = -1; side <= 1; side += 2)
    {
        for (int segment = 0; segment < 60; ++segment)
        {
            float curbZ = -segment * 3.0f - 1.5f;
            glPushMatrix();
            glTranslatef(side * 5.75f + roadCurveX(curbZ), -0.12f, curbZ);
            drawCube(0.28f, 0.42f, 3.2f);
            glPopMatrix();
        }
    }

    glColor3f(0.28f, 0.29f, 0.31f);
    for (int side = -1; side <= 1; side += 2)
    {
        glPushMatrix();
        glTranslatef(side * 7.0f, -0.10f, -80.0f);
        drawCube(2.1f, 0.38f, 180.0f);
        glPopMatrix();
    }

    glColor3f(0.20f, 0.21f, 0.23f);
    for (int i = 0; i < 38; ++i)
    {
        float z = -i * 5.0f;
        for (int side = -1; side <= 1; side += 2)
        {
            glPushMatrix();
            glTranslatef(side * 7.0f, 0.105f, z);
            drawCube(2.0f, 0.012f, 0.035f);
            glPopMatrix();
        }
    }

    glColor3f(0.08f, 0.09f, 0.10f);
    for (int i = 0; i < 28; ++i)
    {
        float z = -i * 9.0f - 2.0f;
        for (int side = -1; side <= 1; side += 2)
        {
            glPushMatrix();
            glTranslatef(side * 5.25f, 0.035f, z);
            drawCube(0.42f, 0.025f, 0.55f);
            glPopMatrix();
        }
    }

    drawZebraCrossings();

    glEnable(GL_LIGHTING);
}


void drawIllustratedLeafCluster(float x, float y, float z,
                                float width, float height,
                                float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    glVertex3f(x - width * 0.50f, y + height * 0.02f, z);
    glVertex3f(x - width * 0.43f, y + height * 0.34f, z);
    glVertex3f(x - width * 0.25f, y + height * 0.48f, z);
    glVertex3f(x - width * 0.05f, y + height * 0.42f, z);
    glVertex3f(x + width * 0.14f, y + height * 0.56f, z);
    glVertex3f(x + width * 0.36f, y + height * 0.38f, z);
    glVertex3f(x + width * 0.50f, y + height * 0.08f, z);
    glVertex3f(x + width * 0.43f, y - height * 0.24f, z);
    glVertex3f(x + width * 0.23f, y - height * 0.42f, z);
    glVertex3f(x - width * 0.02f, y - height * 0.35f, z);
    glVertex3f(x - width * 0.25f, y - height * 0.48f, z);
    glVertex3f(x - width * 0.44f, y - height * 0.25f, z);
    glEnd();

    glColor3f(r * 1.35f, g * 1.25f, b * 1.20f);
    glBegin(GL_POLYGON);
    glVertex3f(x - width * 0.30f, y + height * 0.08f, z - 0.012f);
    glVertex3f(x - width * 0.19f, y + height * 0.30f, z - 0.012f);
    glVertex3f(x + width * 0.02f, y + height * 0.35f, z - 0.012f);
    glVertex3f(x + width * 0.20f, y + height * 0.18f, z - 0.012f);
    glVertex3f(x + width * 0.10f, y - height * 0.04f, z - 0.012f);
    glVertex3f(x - width * 0.12f, y - height * 0.16f, z - 0.012f);
    glEnd();
}

void drawTree(float x, float z)
{
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glDisable(GL_LIGHTING);

    glColor3f(0.38f, 0.16f, 0.055f);
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.18f, 1.0f, 0.0f); glVertex3f(-1.55f, 0.05f, 0.0f); glVertex3f(-0.55f, 0.34f, 0.0f);
    glVertex3f(0.18f, 1.0f, 0.0f); glVertex3f(1.55f, 0.05f, 0.0f); glVertex3f(0.55f, 0.34f, 0.0f);
    glVertex3f(0.0f, 0.9f, 0.02f); glVertex3f(-0.45f, -0.02f, 0.02f); glVertex3f(0.35f, 0.12f, 0.02f);
    glEnd();

    glColor3f(0.36f, 0.14f, 0.045f);
    glBegin(GL_POLYGON);
    glVertex3f(-0.62f, 0.18f, 0.0f);
    glVertex3f(-0.42f, 1.35f, 0.0f);
    glVertex3f(-0.48f, 2.75f, 0.0f);
    glVertex3f(-0.16f, 3.72f, 0.0f);
    glVertex3f(0.18f, 2.80f, 0.0f);
    glVertex3f(0.48f, 1.25f, 0.0f);
    glVertex3f(0.62f, 0.18f, 0.0f);
    glVertex3f(0.10f, 0.42f, 0.0f);
    glVertex3f(-0.12f, 0.40f, 0.0f);
    glEnd();

    glColor3f(0.63f, 0.29f, 0.09f);
    glBegin(GL_POLYGON);
    glVertex3f(-0.34f, 0.25f, -0.01f);
    glVertex3f(-0.22f, 1.45f, -0.01f);
    glVertex3f(-0.25f, 2.55f, -0.01f);
    glVertex3f(-0.10f, 3.18f, -0.01f);
    glVertex3f(0.02f, 2.40f, -0.01f);
    glVertex3f(0.12f, 1.10f, -0.01f);
    glVertex3f(0.22f, 0.28f, -0.01f);
    glEnd();

    glColor3f(0.30f, 0.105f, 0.03f);
    glBegin(GL_QUADS);
    glVertex3f(-0.28f, 2.45f, -0.03f); glVertex3f(-0.05f, 2.62f, -0.03f); glVertex3f(-2.50f, 4.05f, -0.03f); glVertex3f(-2.65f, 3.82f, -0.03f);
    glVertex3f(0.15f, 2.45f, -0.04f); glVertex3f(0.36f, 2.62f, -0.04f); glVertex3f(2.50f, 4.05f, -0.04f); glVertex3f(2.65f, 3.82f, -0.04f);
    glVertex3f(-0.12f, 2.82f, -0.05f); glVertex3f(0.12f, 2.84f, -0.05f); glVertex3f(-0.82f, 5.00f, -0.05f); glVertex3f(-1.02f, 4.92f, -0.05f);
    glVertex3f(0.0f, 2.90f, -0.06f); glVertex3f(0.22f, 2.84f, -0.06f); glVertex3f(1.05f, 5.08f, -0.06f); glVertex3f(0.84f, 5.02f, -0.06f);
    glEnd();

    glColor3f(0.42f, 0.16f, 0.045f);
    glLineWidth(5.0f);
    glBegin(GL_LINES);
    glVertex3f(-1.1f, 3.35f, -0.07f); glVertex3f(-2.7f, 4.55f, -0.07f);
    glVertex3f(1.0f, 3.40f, -0.07f); glVertex3f(2.7f, 4.55f, -0.07f);
    glVertex3f(-0.65f, 4.1f, -0.07f); glVertex3f(-1.8f, 5.15f, -0.07f);
    glVertex3f(0.65f, 4.1f, -0.07f); glVertex3f(1.8f, 5.15f, -0.07f);
    glEnd();

    drawIllustratedLeafCluster(-2.25f, 4.20f, 0.02f, 2.20f, 1.35f, 0.035f, 0.25f, 0.045f);
    drawIllustratedLeafCluster(2.25f, 4.20f, 0.02f, 2.20f, 1.35f, 0.035f, 0.25f, 0.045f);
    drawIllustratedLeafCluster(-1.25f, 5.05f, 0.02f, 2.30f, 1.45f, 0.045f, 0.34f, 0.055f);
    drawIllustratedLeafCluster(1.25f, 5.05f, 0.02f, 2.30f, 1.45f, 0.045f, 0.34f, 0.055f);
    drawIllustratedLeafCluster(0.0f, 5.55f, 0.02f, 2.30f, 1.50f, 0.055f, 0.40f, 0.065f);

    drawIllustratedLeafCluster(-2.70f, 4.70f, -0.10f, 1.60f, 1.20f, 0.12f, 0.52f, 0.10f);
    drawIllustratedLeafCluster(-1.65f, 5.60f, -0.10f, 1.65f, 1.15f, 0.15f, 0.60f, 0.12f);
    drawIllustratedLeafCluster(-0.40f, 6.00f, -0.10f, 1.50f, 1.10f, 0.18f, 0.66f, 0.13f);
    drawIllustratedLeafCluster(0.85f, 5.88f, -0.10f, 1.70f, 1.18f, 0.16f, 0.62f, 0.12f);
    drawIllustratedLeafCluster(2.00f, 5.55f, -0.10f, 1.70f, 1.20f, 0.12f, 0.54f, 0.10f);
    drawIllustratedLeafCluster(2.75f, 4.72f, -0.10f, 1.55f, 1.16f, 0.10f, 0.47f, 0.09f);
    drawIllustratedLeafCluster(-1.95f, 4.00f, -0.12f, 1.55f, 1.08f, 0.10f, 0.45f, 0.08f);
    drawIllustratedLeafCluster(1.90f, 4.05f, -0.12f, 1.60f, 1.10f, 0.10f, 0.48f, 0.08f);

    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}



void drawModernSkyline()
{
    glPushMatrix();

    for (int side = -1; side <= 1; side += 2)
    {
        for (int i = 0; i < 9; ++i)
        {
            float x = side * (16.0f + i * 2.5f);
            float z = -25.0f - i * 22.0f;
            float h = 8.0f + (i % 5) * 3.0f;
            float w = 2.2f + (i % 3) * 0.6f;

            glColor3f(
                0.10f + (i % 3) * 0.025f,
                0.12f + (i % 4) * 0.02f,
                0.16f + (i % 2) * 0.025f
            );

            glPushMatrix();
            glTranslatef(x, h * 0.5f - 0.35f, z);
            drawCube(w, h, 2.8f);
            glPopMatrix();

            glDisable(GL_LIGHTING);
            glColor3f(0.20f, 0.38f, 0.52f);

            for (int k = -1; k <= 1; ++k)
            {
                glPushMatrix();
                glTranslatef(
                    x + k * (w * 0.45f),
                    h * 0.52f,
                    z - 1.43f
                );
                drawCube(0.045f, h * 0.82f, 0.025f);
                glPopMatrix();
            }

            glEnable(GL_LIGHTING);

            if (i % 3 == 0)
            {
                glColor3f(0.20f, 0.21f, 0.23f);
                glPushMatrix();
                glTranslatef(x, h + 1.0f, z);
                drawCylinder(0.035f, 2.0f);
                glPopMatrix();
            }
        }
    }

    glPopMatrix();
}


void drawBuilding(
    float x,
    float z,
    int type,
    int index
)
{
    float width = 3.0f;
    float height = 8.0f;
    float depth = 3.2f;

    if (type == 1)
    {
        width = 3.6f;
        height = 12.0f;
        depth = 3.8f;
    }
    else if (type == 2)
    {
        width = 4.2f;
        height = 17.0f;
        depth = 4.2f;
    }
    else if (type == 3)
    {
        width = 5.0f;
        height = 10.0f;
        depth = 4.4f;
    }
    else if (type == 4)
    {
        width = 3.2f;
        height = 20.0f;
        depth = 3.5f;
    }

    if (type == 0) glColor3f(0.34f,0.36f,0.40f);
    else if (type == 1) glColor3f(0.20f,0.25f,0.32f);
    else if (type == 2) glColor3f(0.14f,0.18f,0.25f);
    else if (type == 3) glColor3f(0.42f,0.32f,0.25f);
    else glColor3f(0.25f,0.27f,0.31f);

    glPushMatrix();
    glTranslatef(x,height/2.0f-0.3f,z);
    drawCube(width,height,depth);
    glPopMatrix();

    glColor3f(0.10f,0.11f,0.13f);
    for (int side=-1; side<=1; side+=2)
    {
        glPushMatrix();
        glTranslatef(x + side*(width/2.0f-0.10f),
                     height/2.0f-0.25f,
                     z + depth/2.0f+0.035f);
        drawCube(0.16f,height-0.25f,0.08f);
        glPopMatrix();
    }


    glColor3f(0.08f,0.09f,0.11f);
    for (int r=0; r<10; r++)
    {
        float yy = 0.55f + r*1.65f;
        if (yy > height-0.25f) break;

        glPushMatrix();
        glTranslatef(x,yy,z+depth/2.0f+0.055f);
        drawCube(width+0.08f,0.07f,0.10f);
        glPopMatrix();
    }


    int rows = (int)(height/1.65f);
    if (rows > 10) rows = 10;

    for (int r=0; r<rows; r++)
    {
        float wy = 1.0f + r*1.65f;
        int columns = (type == 3) ? 4 : 3;

        for (int c=0; c<columns; c++)
        {
            float normalized = (columns==1) ? 0.0f :
                               (float)c/(float)(columns-1)*2.0f-1.0f;
            float wx = x + normalized*(width*0.34f);

            bool windowOn =
                ((index*19 + r*13 + c*7) % 11) < 8;

            glColor3f(0.025f,0.035f,0.045f);
            glPushMatrix();
            glTranslatef(wx,wy,z+depth/2.0f+0.07f);
            drawCube(0.48f,0.70f,0.07f);
            glPopMatrix();


            if (isNight())
            {
                glDisable(GL_LIGHTING);
                if (windowOn)
                    glColor3f(1.0f,0.72f,0.20f);
                else
                    glColor3f(0.025f,0.05f,0.08f);
            }
            else
            {
                glColor3f(0.10f,0.42f,0.62f);
            }

            glPushMatrix();
            glTranslatef(wx,wy,z+depth/2.0f+0.115f);
            drawCube(0.36f,0.56f,0.025f);
            glPopMatrix();


            glColor3f(0.08f,0.09f,0.10f);
            glPushMatrix();
            glTranslatef(wx,wy,z+depth/2.0f+0.145f);
            drawCube(0.035f,0.56f,0.025f);
            glPopMatrix();

            if (isNight())
                glEnable(GL_LIGHTING);
        }
    }

    for (int r=0; r<rows; r++)
    {
        float wy = 1.0f+r*1.65f;
        for (int side=-1; side<=1; side+=2)
        {
            bool windowOn = ((index*23+r*5+side*3)%9) < 6;

            if (isNight())
            {
                glDisable(GL_LIGHTING);
                if (windowOn) glColor3f(1.0f,0.68f,0.18f);
                else glColor3f(0.025f,0.045f,0.07f);
            }
            else glColor3f(0.08f,0.30f,0.48f);

            glPushMatrix();
            glTranslatef(x+side*(width/2.0f+0.045f),
                         wy,z);
            drawCube(0.035f,0.55f,0.42f);
            glPopMatrix();

            if (isNight()) glEnable(GL_LIGHTING);
        }
    }


    if (type == 1 || type == 2)
    {
        for (int r=1; r<rows; r+=2)
        {
            float yy = 1.05f+r*1.65f;

            glColor3f(0.13f,0.14f,0.16f);
            glPushMatrix();
            glTranslatef(x,yy,z+depth/2.0f+0.38f);
            drawCube(width*0.48f,0.10f,0.72f);
            glPopMatrix();


            for (int p=-1; p<=1; p++)
            {
                glPushMatrix();
                glTranslatef(x+p*(width*0.20f),yy+0.42f,
                             z+depth/2.0f+0.68f);
                drawCube(0.035f,0.75f,0.035f);
                glPopMatrix();
            }

            glPushMatrix();
            glTranslatef(x,yy+0.42f,z+depth/2.0f+0.68f);
            drawCube(width*0.48f,0.035f,0.035f);
            glPopMatrix();
        }
    }


    glDisable(GL_LIGHTING);
    glColor3f(0.025f,0.035f,0.045f);
    glPushMatrix();
    glTranslatef(x,1.0f,z+depth/2.0f+0.10f);
    drawCube(0.75f,1.9f,0.08f);
    glPopMatrix();

    glColor3f(0.08f,0.35f,0.48f);
    glPushMatrix();
    glTranslatef(x,1.05f,z+depth/2.0f+0.15f);
    drawCube(0.56f,1.55f,0.025f);
    glPopMatrix();


    glColor3f(0.08f,0.09f,0.11f);
    glPushMatrix();
    glTranslatef(x,2.05f,z+depth/2.0f+0.20f);
    drawCube(1.15f,0.10f,0.55f);
    glPopMatrix();

    if (type == 3)
    {
        glColor3f(0.04f,0.04f,0.05f);
        glPushMatrix();
        glTranslatef(x,3.0f,z+depth/2.0f+0.12f);
        drawCube(3.0f,0.42f,0.08f);
        glPopMatrix();

        glColor3f(0.05f,0.75f,0.95f);
        glPushMatrix();
        glTranslatef(x,3.05f,z+depth/2.0f+0.18f);
        drawCube(2.35f,0.16f,0.035f);
        glPopMatrix();
    }

    for (int r=1; r<rows; r+=3)
    {
        float yy = 1.05f+r*1.65f;
        glColor3f(0.55f,0.57f,0.58f);

        glPushMatrix();
        glTranslatef(x+width*0.38f,yy,z+depth/2.0f+0.17f);
        drawCube(0.38f,0.25f,0.22f);
        glPopMatrix();

        glColor3f(0.18f,0.19f,0.20f);
        glPushMatrix();
        glTranslatef(x+width*0.38f,yy,z+depth/2.0f+0.30f);
        drawCube(0.23f,0.08f,0.03f);
        glPopMatrix();
    }


    glColor3f(0.10f,0.11f,0.12f);
    glPushMatrix();
    glTranslatef(x,height+0.25f,z);
    drawCube(width*0.60f,0.35f,depth*0.55f);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(x,height+0.75f,z);
    drawCube(0.035f,1.0f,0.035f);
    glPopMatrix();

    glColor3f(0.75f,0.75f,0.78f);
    glPushMatrix();
    glTranslatef(x,height+1.20f,z);
    drawSphere(0.10f);
    glPopMatrix();

    if (isNight())
    {
        glDisable(GL_LIGHTING);
        glColor3f(1.0f,0.05f,0.03f);
        glPushMatrix();
        glTranslatef(x,height+1.30f,z);
        drawSphere(0.07f);
        glPopMatrix();
        glEnable(GL_LIGHTING);
    }

    glEnable(GL_LIGHTING);
}





void drawStreetLamp(
    float x,
    float z
)
{
    glPushMatrix();

    glTranslatef(
        x,
        0.0f,
        z
    );

    glColor3f(
        0.08f,
        0.08f,
        0.09f
    );

    glPushMatrix();

    glTranslatef(
        0.0f,
        3.0f,
        0.0f
    );

    drawCube(
        0.12f,
        6.0f,
        0.12f
    );

    glPopMatrix();


    glPushMatrix();

    glTranslatef(
        x < 0 ? 0.45f : -0.45f,
        5.8f,
        0.0f
    );

    glScalef(
        x < 0 ? 1.0f : -1.0f,
        1.0f,
        1.0f
    );

    drawCube(
        0.7f,
        0.1f,
        0.1f
    );

    glPopMatrix();


    glDisable(GL_LIGHTING);

    if (isNight())
    {
        glColor4f(
            1.0f,
            0.8f,
            0.15f,
            0.15f
        );

        glPushMatrix();

        glTranslatef(
            x < 0 ? 0.65f : -0.65f,
            5.65f,
            0.0f
        );

        drawSphere(0.55f);

        glPopMatrix();


        glColor3f(
            1.0f,
            0.95f,
            0.55f
        );

        glPushMatrix();

        glTranslatef(
            x < 0 ? 0.65f : -0.65f,
            5.65f,
            0.0f
        );

        drawSphere(0.22f);

        glPopMatrix();
    }

    glEnable(GL_LIGHTING);

    glPopMatrix();
}




void drawVehicle(
    float x,
    float z,
    int type
)
{
    float r = 0.15f;
    float g = 0.25f;
    float b = 0.85f;

    if (type == TAXI)
    {
        r = 1.0f;
        g = 0.75f;
        b = 0.05f;
    }
    else if (type == BUS)
    {
        r = 0.1f;
        g = 0.55f;
        b = 0.9f;
    }
    else if (type == POLICE)
    {
        r = 0.08f;
        g = 0.08f;
        b = 0.12f;
    }
    else if (type == AMBULANCE)
    {
        r = 0.9f;
        g = 0.9f;
        b = 0.9f;
    }
    else if (type == TRUCK)
    {
        r = 0.5f;
        g = 0.28f;
        b = 0.1f;
    }


    glPushMatrix();

    glTranslatef(
        x,
        0.25f,
        z
    );


    glColor3f(
        r,
        g,
        b
    );

    drawCube(
        1.15f,
        0.55f,
        2.0f
    );


    glColor3f(
        r * 0.8f,
        g * 0.8f,
        b * 0.8f
    );

    glPushMatrix();

    glTranslatef(
        0.0f,
        0.48f,
        -0.1f
    );

    drawCube(
        0.9f,
        0.45f,
        1.25f
    );

    glPopMatrix();


    glColor3f(
        0.04f,
        0.12f,
        0.20f
    );

    glPushMatrix();

    glTranslatef(
        0.0f,
        0.7f,
        -0.1f
    );

    drawCube(
        0.75f,
        0.25f,
        1.0f
    );

    glPopMatrix();


    for (int side = -1;
         side <= 1;
         side += 2)
    {
        for (int front = -1;
             front <= 1;
             front += 2)
        {
            glColor3f(
                0.02f,
                0.02f,
                0.02f
            );

            glPushMatrix();

            glTranslatef(
                side * 1.15f,
                -0.15f,
                front * 1.25f
            );

            glRotatef(
                90.0f,
                0.0f,
                1.0f,
                0.0f
            );

            drawCylinder(
                0.3f,
                0.18f
            );

            glPopMatrix();
        }
    }


    glDisable(GL_LIGHTING);

    glColor3f(
        1.0f,
        0.95f,
        0.6f
    );

    glPushMatrix();

    glTranslatef(
        -0.55f,
        0.3f,
        -2.03f
    );

    drawSphere(0.12f);

    glPopMatrix();

    glPushMatrix();

    glTranslatef(
        0.55f,
        0.3f,
        -2.03f
    );

    drawSphere(0.12f);

    glPopMatrix();


    glColor3f(
        1.0f,
        0.05f,
        0.03f
    );

    glPushMatrix();

    glTranslatef(
        -0.55f,
        0.3f,
        2.03f
    );

    drawSphere(0.11f);

    glPopMatrix();

    glPushMatrix();

    glTranslatef(
        0.55f,
        0.3f,
        2.03f
    );

    drawSphere(0.11f);

    glPopMatrix();

    if (type == POLICE)
    {
        glColor3f(
            0.1f,
            0.3f,
            1.0f
        );

        glPushMatrix();

        glTranslatef(
            -0.3f,
            0.95f,
            0.0f
        );

        drawSphere(0.12f);

        glPopMatrix();


        glColor3f(
            1.0f,
            0.05f,
            0.05f
        );

        glPushMatrix();

        glTranslatef(
            0.3f,
            0.95f,
            0.0f
        );

        drawSphere(0.12f);

        glPopMatrix();
    }


    if (type == AMBULANCE)
    {
        glColor3f(
            1.0f,
            0.0f,
            0.0f
        );

        glPushMatrix();

        glTranslatef(
            0.0f,
            0.78f,
            -1.0f
        );

        drawCube(
            0.08f,
            0.35f,
            0.04f
        );

        glPopMatrix();

        glPushMatrix();

        glTranslatef(
            0.0f,
            0.78f,
            -1.0f
        );

        drawCube(
            0.35f,
            0.08f,
            0.04f
        );

        glPopMatrix();
    }

    glEnable(GL_LIGHTING);

    glPopMatrix();
}




void drawTrafficLight(
    float x,
    float z
)
{
    glPushMatrix();

    glTranslatef(
        x,
        0.0f,
        z
    );

    glColor3f(
        0.05f,
        0.05f,
        0.05f
    );

    glPushMatrix();

    glTranslatef(
        0.0f,
        3.0f,
        0.0f
    );

    drawCube(
        0.18f,
        6.0f,
        0.18f
    );

    glPopMatrix();


    glPushMatrix();

    glTranslatef(
        0.0f,
        5.3f,
        0.0f
    );

    drawCube(
        0.7f,
        1.8f,
        0.35f
    );

    glPopMatrix();


    glDisable(GL_LIGHTING);

    float cycle =
        fmod(runTime, 6.0f);

    if (cycle < 2.5f)
        glColor3f(1.0f, 0.0f, 0.0f);
    else
        glColor3f(0.12f, 0.0f, 0.0f);

    glPushMatrix();

    glTranslatef(
        0.0f,
        5.75f,
        0.38f
    );

    drawSphere(0.18f);

    glPopMatrix();


    if (cycle >= 2.5f &&
        cycle < 3.5f)
        glColor3f(1.0f, 0.8f, 0.0f);
    else
        glColor3f(0.15f, 0.10f, 0.0f);

    glPushMatrix();

    glTranslatef(
        0.0f,
        5.25f,
        0.38f
    );

    drawSphere(0.18f);

    glPopMatrix();


    if (cycle >= 3.5f)
        glColor3f(0.0f, 1.0f, 0.15f);
    else
        glColor3f(0.0f, 0.15f, 0.02f);

    glPushMatrix();

    glTranslatef(
        0.0f,
        4.75f,
        0.38f
    );

    drawSphere(0.18f);

    glPopMatrix();

    glEnable(GL_LIGHTING);

    glPopMatrix();
}


void drawPedestrian(
    float x,
    float z,
    float animation
)
{
    int outfit = ((int)(fabs(x)*10.0f) + (int)fabs(z)) % 2;
    drawRealisticHuman(x,z,animation,0.78f,outfit);
}




void drawCoin(
    float x,
    float z,
    float rotation
)
{
    glDisable(GL_LIGHTING);

    glColor3f(
        1.0f,
        0.75f,
        0.05f
    );

    glPushMatrix();

    glTranslatef(
        x,
        1.4f,
        z
    );

    glRotatef(
        rotation,
        0.0f,
        1.0f,
        0.0f
    );

    glScalef(
        0.12f,
        0.12f,
        0.12f
    );

    glutSolidTorus(
        0.8,
        1.0,
        12,
        20
    );

    glPopMatrix();

    glEnable(GL_LIGHTING);
}




void drawPowerUp(float x, float z, int type, float rotation)
{
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(x, 1.5f, z);
    glRotatef(rotation, 0.0f, 1.0f, 0.0f);

    if (type == POWER_SHIELD) glColor3f(0.05f,0.8f,1.0f);
    else if (type == POWER_SPEED) glColor3f(1.0f,0.15f,0.03f);
    else if (type == POWER_MAGNET) glColor3f(0.85f,0.1f,1.0f);
    else if (type == POWER_DESTROYER) glColor3f(1.0f,0.45f,0.02f);
    else if (type == POWER_SUPERJUMP) glColor3f(0.15f,1.0f,0.25f);
    else if (type == POWER_EXTRALIFE) glColor3f(1.0f,0.15f,0.35f);
    else if (type == POWER_SLOWMO) glColor3f(0.35f,0.55f,1.0f);
    else glColor3f(1.0f,0.9f,0.05f);

    glutWireTorus(0.18,0.62,12,24);
    glPushMatrix();
    glRotatef(90.0f,1.0f,0.0f,0.0f);
    if (type == POWER_SHIELD) glutWireSphere(0.45,12,12);
    else if (type == POWER_SPEED) {
        glBegin(GL_TRIANGLES);
        glVertex3f(0,0.55f,0); glVertex3f(-0.28f,-0.05f,0); glVertex3f(0.05f,-0.05f,0);
        glVertex3f(0.05f,-0.05f,0); glVertex3f(-0.12f,-0.55f,0); glVertex3f(0.28f,0.05f,0); glEnd();
    } else if (type == POWER_MAGNET) {
        glBegin(GL_LINE_STRIP); glVertex3f(-0.35f,0.35f,0); glVertex3f(-0.35f,-0.2f,0); glVertex3f(0.35f,-0.2f,0); glVertex3f(0.35f,0.35f,0); glEnd();
    } else if (type == POWER_DESTROYER) {
        glutSolidSphere(0.38,12,12); glBegin(GL_LINES);
        glVertex3f(-0.5f,0,0); glVertex3f(0.5f,0,0); glVertex3f(0,-0.5f,0); glVertex3f(0,0.5f,0); glEnd();
    } else if (type == POWER_SUPERJUMP) {
        glBegin(GL_LINE_STRIP); glVertex3f(-0.45f,-0.35f,0); glVertex3f(0,0.45f,0); glVertex3f(0.45f,-0.35f,0); glEnd();
    } else if (type == POWER_EXTRALIFE) {
        glBegin(GL_LINE_STRIP); glVertex3f(0,-0.45f,0); glVertex3f(-0.48f,0.1f,0); glVertex3f(-0.35f,0.45f,0); glVertex3f(0,0.2f,0); glVertex3f(0.35f,0.45f,0); glVertex3f(0.48f,0.1f,0); glVertex3f(0,-0.45f,0); glEnd();
    } else if (type == POWER_SLOWMO) {
        glutWireSphere(0.42,12,12); glBegin(GL_LINES); glVertex3f(0,0,0); glVertex3f(0,0.28f,0); glVertex3f(0,0,0); glVertex3f(0.23f,-0.12f,0); glEnd();
    } else {
        glBegin(GL_LINES); glVertex3f(-0.5f,0,0); glVertex3f(0.5f,0,0); glVertex3f(0,-0.5f,0); glVertex3f(0,0.5f,0); glEnd();
    }
    glPopMatrix(); glPopMatrix(); glEnable(GL_LIGHTING);
}



void drawObstacle(
    float x,
    float z,
    int type
)
{
    glPushMatrix();

    glTranslatef(
        x,
        0.3f,
        z
    );

    if (type == CONE)
    {
        glColor3f(
            1.0f,
            0.3f,
            0.02f
        );

        glutSolidCone(
            0.45,
            1.0,
            16,
            10
        );
    }
    else if (type == BARRIER)
    {
        glColor3f(
            0.95f,
            0.1f,
            0.05f
        );

        drawCube(
            1.3f,
            0.7f,
            0.35f
        );

        glColor3f(
            1.0f,
            0.85f,
            0.05f
        );

        glPushMatrix();

        glTranslatef(
            0.0f,
            0.0f,
            -0.37f
        );

        drawCube(
            1.0f,
            0.15f,
            0.04f
        );

        glPopMatrix();
    }
    else if (type == POTHOLE)
    {
        glDisable(GL_LIGHTING);

        glColor3f(
            0.01f,
            0.01f,
            0.01f
        );

        glScalef(
            1.0f,
            0.05f,
            0.8f
        );

        glutSolidSphere(
            0.9f,
            16,
            10
        );

        glEnable(GL_LIGHTING);
    }
    else
    {
        glDisable(GL_LIGHTING);

        glColor3f(
            1.0f,
            0.1f,
            0.0f
        );

        glutSolidCone(
            0.8,
            1.3,
            16,
            10
        );

        glEnable(GL_LIGHTING);
    }

    glPopMatrix();
}



void drawBillboard(
    float x,
    float z
)
{
    glColor3f(
        0.08f,
        0.08f,
        0.10f
    );

    glPushMatrix();

    glTranslatef(
        x,
        3.0f,
        z
    );

    drawCube(
        0.15f,
        6.0f,
        0.15f
    );

    glPopMatrix();


    glDisable(GL_LIGHTING);

    if (isNight())
    {
        glColor3f(
            0.05f,
            0.4f,
            1.0f
        );
    }
    else
    {
        glColor3f(
            0.05f,
            0.3f,
            0.75f
        );
    }

    glPushMatrix();

    glTranslatef(
        x,
        5.8f,
        z
    );

    drawCube(
        3.0f,
        1.7f,
        0.12f
    );

    glPopMatrix();



    glColor3f(
        1.0f,
        0.2f,
        0.05f
    );

    glPushMatrix();

    glTranslatef(
        x,
        6.2f,
        z + 0.15f
    );

    drawCube(
        2.2f,
        0.12f,
        0.03f
    );

    glPopMatrix();

    glEnable(GL_LIGHTING);
}



void drawPlane()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glColor3f(0.92f, 0.94f, 0.98f);

    glPushMatrix();
    glTranslatef(planeX, planeY, planeZ);
    glScalef(1.45f, 1.45f, 1.45f);
    glRotatef(-8.0f, 0.0f, 1.0f, 0.0f);

    glPushMatrix();
    glScalef(2.4f, 0.20f, 0.28f);
    drawCube(1.0f, 1.0f, 1.0f);
    glPopMatrix();

    glColor3f(0.72f, 0.78f, 0.88f);
    glBegin(GL_TRIANGLES);
    glVertex3f(2.45f, 0.0f, 0.0f);
    glVertex3f(1.70f, 0.20f, 0.0f);
    glVertex3f(1.70f, -0.20f, 0.0f);
    glEnd();

    glColor3f(0.80f, 0.86f, 0.94f);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.55f, 0.0f, 0.0f);
    glVertex3f(-0.80f, 0.0f, 2.10f);
    glVertex3f(-0.15f, 0.0f, 0.0f);
    glVertex3f(0.55f, 0.0f, 0.0f);
    glVertex3f(-0.80f, 0.0f, -2.10f);
    glVertex3f(-0.15f, 0.0f, 0.0f);
    glEnd();

    glColor3f(0.68f, 0.74f, 0.86f);
    glBegin(GL_TRIANGLES);
    glVertex3f(-1.65f, 0.0f, 0.0f);
    glVertex3f(-2.15f, 0.0f, 0.72f);
    glVertex3f(-1.85f, 0.0f, 0.0f);
    glVertex3f(-1.65f, 0.0f, 0.0f);
    glVertex3f(-2.15f, 0.0f, -0.72f);
    glVertex3f(-1.85f, 0.0f, 0.0f);
    glEnd();

    glColor3f(0.08f, 0.28f, 0.55f);
    for (int i = -2; i <= 1; ++i)
    {
        glPushMatrix();
        glTranslatef(i * 0.38f, 0.22f, -0.29f);
        drawSphere(0.075f);
        glPopMatrix();
    }
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void updatePlane()
{
    planeX += planeSpeed;
    planeZ += gameSpeed * 0.025f;
    if (planeX > 45.0f)
    {
        planeX = -45.0f;
        planeY = randomFloat(22.0f, 30.0f);
        planeZ = randomFloat(-105.0f, -55.0f);
    }
}

void drawRainbow()
{
    if (isNight() || raining)
        return;

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glLineWidth(5.0f);
    const float colors[7][3] = {
        {0.95f, 0.08f, 0.08f},
        {1.00f, 0.42f, 0.04f},
        {1.00f, 0.88f, 0.05f},
        {0.10f, 0.75f, 0.18f},
        {0.08f, 0.48f, 0.95f},
        {0.20f, 0.18f, 0.75f},
        {0.62f, 0.16f, 0.78f}
    };

    for (int band = 0; band < 7; ++band)
    {
        glColor3fv(colors[band]);
        float radius = 15.0f - band * 0.48f;
        float yCenter = 11.0f;
        float z = -92.0f + band * 0.025f;
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 40; ++i)
        {
            float angle = 3.1415926f * (float)i / 40.0f;
            glVertex3f(cos(angle) * radius,
                       yCenter + sin(angle) * radius,
                       z);
        }
        glEnd();
    }
    glLineWidth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void drawWorldFeatures()
{
    glDisable(GL_LIGHTING);

    glColor3f(0.04f, 0.34f, 0.62f);
    glPushMatrix();
    glTranslatef(-14.0f, -0.08f, -38.0f);
    drawCube(8.0f, 0.12f, 95.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(14.0f, -0.08f, -38.0f);
    drawCube(8.0f, 0.12f, 95.0f);
    glPopMatrix();

    glColor3f(0.20f, 0.70f, 0.88f);
    glBegin(GL_LINES);
    for (int i = 0; i < 7; ++i)
    {
        float z = -18.0f - i * 10.0f;
        glVertex3f(-17.0f, 0.02f, z);
        glVertex3f(-11.0f, 0.02f, z);
    }
    glEnd();

    glColor3f(0.25f, 0.72f, 0.92f);
    glBegin(GL_QUADS);
    glVertex3f(-17.0f, 8.0f, -48.0f); glVertex3f(-12.0f, 8.0f, -48.0f);
    glVertex3f(-12.0f, 0.0f, -48.0f); glVertex3f(-17.0f, 0.0f, -48.0f);
    glEnd();

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, tunnelZ + 68.0f);

    glColor3f(0.28f, 0.30f, 0.34f);
    glPushMatrix(); glTranslatef(-6.2f, 3.5f, -68.0f); drawCube(2.4f, 7.0f, 5.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(6.2f, 3.5f, -68.0f); drawCube(2.4f, 7.0f, 5.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, 6.65f, -68.0f); drawCube(15.0f, 1.7f, 5.0f); glPopMatrix();

    glColor3f(0.015f, 0.018f, 0.025f);
    glPushMatrix(); glTranslatef(0.0f, 3.2f, -108.0f); drawCube(10.0f, 6.4f, 0.18f); glPopMatrix();

    glColor3f(0.42f, 0.43f, 0.45f);
    glPushMatrix();
    glTranslatef(0.0f, 0.02f, -88.0f);
    drawCube(9.0f, 0.08f, 40.0f);
    glPopMatrix();

    glColor3f(0.95f, 0.78f, 0.12f);
    glPushMatrix(); glTranslatef(-4.0f, 0.09f, -88.0f); drawCube(0.10f, 0.025f, 40.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(4.0f, 0.09f, -88.0f); drawCube(0.10f, 0.025f, 40.0f); glPopMatrix();
    glColor3f(0.90f, 0.90f, 0.86f);
    for (int i = 0; i < 12; ++i)
    {
        float laneZ = -70.0f - i * 3.0f;
        glPushMatrix(); glTranslatef(-1.5f, 0.10f, laneZ); drawCube(0.08f, 0.025f, 1.2f); glPopMatrix();
        glPushMatrix(); glTranslatef(1.5f, 0.10f, laneZ); drawCube(0.08f, 0.025f, 1.2f); glPopMatrix();
    }

    glColor3f(0.12f, 0.13f, 0.16f);
    glPushMatrix(); glTranslatef(-5.2f, 3.4f, -88.0f); drawCube(1.2f, 6.8f, 40.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(5.2f, 3.4f, -88.0f); drawCube(1.2f, 6.8f, 40.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, 6.7f, -88.0f); drawCube(10.4f, 1.0f, 40.0f); glPopMatrix();

    glColor3f(1.0f, 0.82f, 0.28f);
    for (int i = 0; i < 8; ++i)
    {
        float lampZ = -70.0f - i * 5.0f;
        glPushMatrix();
        glTranslatef(0.0f, 6.12f, lampZ);
        drawCube(0.75f, 0.10f, 0.28f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-4.55f, 5.55f, lampZ);
        drawSphere(0.16f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(4.55f, 5.55f, lampZ);
        drawSphere(0.16f);
        glPopMatrix();
    }

    glColor3f(0.95f, 0.90f, 0.45f);
    for (int i = 0; i < 8; ++i)
    {
        float markerZ = -70.0f - i * 5.0f;
        glPushMatrix(); glTranslatef(-4.58f, 1.0f, markerZ); drawCube(0.08f, 0.18f, 0.35f); glPopMatrix();
        glPushMatrix(); glTranslatef(4.58f, 1.0f, markerZ); drawCube(0.08f, 0.18f, 0.35f); glPopMatrix();
    }

    glColor3f(1.0f, 0.82f, 0.18f);
    for (int side = -1; side <= 1; side += 2)
    {
        glPushMatrix(); glTranslatef(side * 4.6f, 5.4f, -65.1f); drawSphere(0.20f); glPopMatrix();
        glPushMatrix(); glTranslatef(side * 4.6f, 3.8f, -65.1f); drawSphere(0.20f); glPopMatrix();
    }

    glPopMatrix();

    glColor3f(0.42f, 0.28f, 0.16f);
    glPushMatrix(); glTranslatef(-7.2f, 3.1f, -34.0f); drawCube(1.6f, 6.2f, 4.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(7.2f, 3.1f, -34.0f); drawCube(1.6f, 6.2f, 4.0f); glPopMatrix();
    glColor3f(0.24f, 0.25f, 0.28f);
    glPushMatrix(); glTranslatef(0.0f, 6.25f, -34.0f); drawCube(16.0f, 0.70f, 4.0f); glPopMatrix();
    glColor3f(0.65f, 0.67f, 0.70f);
    glPushMatrix(); glTranslatef(0.0f, 6.85f, -34.0f); drawCube(15.5f, 0.18f, 0.18f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, 6.85f, -35.6f); drawCube(15.5f, 0.18f, 0.18f); glPopMatrix();


    glColor3f(0.10f, 0.10f, 0.12f);
    for (int i = -2; i <= 2; ++i)
    {
        glPushMatrix(); glTranslatef(0.0f, 0.08f, -18.0f + i * 0.45f); drawCube(13.0f, 0.10f, 0.10f); glPopMatrix();
    }
    glColor3f(0.95f, 0.08f, 0.03f);
    glPushMatrix(); glTranslatef(-5.7f, 1.2f, -18.0f); glRotatef(-28.0f, 0.0f, 0.0f, 1.0f); drawCube(0.18f, 2.4f, 0.18f); glPopMatrix();
    glPushMatrix(); glTranslatef(5.7f, 1.2f, -18.0f); glRotatef(28.0f, 0.0f, 0.0f, 1.0f); drawCube(0.18f, 2.4f, 0.18f); glPopMatrix();


    glColor3f(0.65f, 0.08f, 0.05f);
    glPushMatrix(); glTranslatef(-13.5f, 1.3f, trainZ); drawCube(4.0f, 2.6f, 8.0f); glPopMatrix();
    glColor3f(0.12f, 0.16f, 0.22f);
    glPushMatrix(); glTranslatef(-13.5f, 1.75f, trainZ - 4.05f); drawCube(3.0f, 0.65f, 0.08f); glPopMatrix();


    glColor3f(0.55f, 0.20f, 0.06f);
    glPushMatrix(); glTranslatef(-14.0f, 0.35f, boatZ); drawCube(1.5f, 0.35f, 3.0f); glPopMatrix();
    glColor3f(0.90f, 0.90f, 0.82f);
    glPushMatrix(); glTranslatef(-14.0f, 1.0f, boatZ); drawCube(0.12f, 1.2f, 0.12f); glPopMatrix();

    glEnable(GL_LIGHTING);
}

void updateWorldFeatures()
{
    featureTime += 0.016f;
    trainZ += gameSpeed * 0.75f;
    boatZ += gameSpeed * 0.35f;
    tunnelZ += gameSpeed;
    if (trainZ > 20.0f) trainZ = -150.0f;
    if (boatZ > 20.0f) boatZ = -120.0f;
    if (tunnelZ > 20.0f) tunnelZ = -150.0f;
}

void updateClouds()
{
    for (int i = 0; i < CLOUD_COUNT; ++i)
    {
        // Every cloud has a slightly different speed and direction.
        float wind = 0.018f + (i % 4) * 0.006f;
        if (i % 5 == 0)
            wind = -0.014f;

        cloudX[i] += wind;
        cloudZ[i] += gameSpeed * 0.008f;

        if (cloudX[i] > 48.0f)
        {
            cloudX[i] = -48.0f;
            cloudY[i] = randomFloat(17.0f, 27.0f);
            cloudZ[i] = randomFloat(-105.0f, -35.0f);
        }
        else if (cloudX[i] < -48.0f)
        {
            cloudX[i] = 48.0f;
            cloudY[i] = randomFloat(17.0f, 27.0f);
            cloudZ[i] = randomFloat(-105.0f, -35.0f);
        }
    }
}

void drawSky()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    if (isNight())
    {
        glColor3f(
            0.005f,
            0.008f,
            0.03f
        );
    }
    else if (isSunset())
    {
        glColor3f(
            0.45f,
            0.15f,
            0.08f
        );
    }
    else
    {
        glColor3f(0.25f, 0.55f, 0.95f);
    }

    glPushMatrix();

    glTranslatef(
        0.0f,
        30.0f,
        -80.0f
    );

    drawCube(
        100.0f,
        60.0f,
        100.0f
    );

    glPopMatrix();



    if (isNight())
    {
        glPointSize(2.0f);

        glBegin(GL_POINTS);

        glColor3f(
            1.0f,
            1.0f,
            1.0f
        );

        for (int i = 0;
             i < STAR_COUNT;
             i++)
        {
            glVertex3f(
                starX[i],
                starY[i],
                starZ[i]
            );
        }

        glEnd();



        glColor3f(
            1.0f,
            1.0f,
            0.8f
        );

        glPushMatrix();

        glTranslatef(
            20.0f,
            24.0f,
            -60.0f
        );

        drawSphere(3.0f);

        glPopMatrix();
    }
    else
    {

        glColor3f(
            1.0f,
            0.85f,
            0.15f
        );

        glPushMatrix();

        glTranslatef(
            -18.0f,
            25.0f,
            -65.0f
        );

        drawSphere(3.0f);

        glPopMatrix();


        glColor4f(
            1.0f,
            1.0f,
            1.0f,
            0.75f
        );

        for (int i = 0;
             i < CLOUD_COUNT;
             i++)
        {
            glPushMatrix();

            glTranslatef(
                cloudX[i],
                cloudY[i],
                cloudZ[i]
            );

            drawSphere(1.5f);

            glTranslatef(
                1.4f,
                0.0f,
                0.0f
            );

            drawSphere(1.1f);

            glTranslatef(
                -2.5f,
                0.1f,
                0.0f
            );

            drawSphere(1.0f);

            glPopMatrix();
        }
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}



void drawRain()
{
    if (!raining)
        return;

    glDisable(GL_LIGHTING);

    glColor4f(
        0.5f,
        0.7f,
        1.0f,
        0.65f
    );

    glBegin(GL_LINES);

    for (int i = 0;
         i < RAIN_COUNT;
         i++)
    {
        glVertex3f(
            rainX[i],
            rainY[i],
            rainZ[i]
        );

        glVertex3f(
            rainX[i] + 0.1f,
            rainY[i] - 1.2f,
            rainZ[i] + 0.2f
        );
    }

    glEnd();

    glEnable(GL_LIGHTING);
}




void spawnParticles(
    float x,
    float y,
    float z
)
{
    for (int i = 0;
         i < MAX_PARTICLES;
         i++)
    {
        if (!particles[i].active)
        {
            particles[i].active = true;

            particles[i].x = x;
            particles[i].y = y;
            particles[i].z = z;

            particles[i].vx =
                randomFloat(-0.08f, 0.08f);

            particles[i].vy =
                randomFloat(0.03f, 0.16f);

            particles[i].vz =
                randomFloat(-0.08f, 0.08f);

            particles[i].life =
                randomFloat(0.4f, 1.0f);

            break;
        }
    }
}


void drawParticles()
{
    glDisable(GL_LIGHTING);

    glPointSize(5.0f);

    glBegin(GL_POINTS);

    glColor3f(
        0.1f,
        1.0f,
        0.25f
    );

    for (int i = 0;
         i < MAX_PARTICLES;
         i++)
    {
        if (particles[i].active)
        {
            glVertex3f(
                particles[i].x,
                particles[i].y,
                particles[i].z
            );
        }
    }

    glEnd();

    glEnable(GL_LIGHTING);
}




void drawSaveEffect()
{
    if (!saveEffect)
        return;

    glDisable(GL_LIGHTING);

    glColor4f(
        0.0f,
        1.0f,
        0.3f,
        0.35f
    );

    glPushMatrix();

    glTranslatef(
        effectX,
        effectY,
        effectZ
    );

    glutWireSphere(
        0.7f + effectTime * 2.0f,
        20,
        20
    );

    glPopMatrix();

    glEnable(GL_LIGHTING);
}




void createBrick(
    int index,
    float z
)
{
    if (index < 0 ||
        index >= MAX_BRICKS)
        return;

    bricks[index].lane =
        rand() % 3;

    bricks[index].z =
        z;

    bricks[index].type =
        rand() % 4;

    bricks[index].active =
        true;

    bricks[index].saved =
        false;

    bricks[index].rotation =
        randomFloat(0.0f, 360.0f);
}



void createCoin(
    int index,
    float z
)
{
    if (index < 0 ||
        index >= MAX_COINS)
        return;

    coins[index].lane =
        rand() % 3;

    coins[index].z =
        z;

    coins[index].active =
        true;

    coins[index].rotation =
        randomFloat(0.0f, 360.0f);
}




void createVehicle(
    int index,
    float z
)
{
    if (index < 0 ||
        index >= MAX_VEHICLES)
        return;

    vehicles[index].lane =
        rand() % 3;

    vehicles[index].z =
        z;

    vehicles[index].type =
        rand() % 6;

    if (index == 0)
        vehicles[index].type = POLICE;
    else if (index == 1)
        vehicles[index].type = AMBULANCE;

    vehicles[index].active =
        true;

    vehicles[index].speedMultiplier =
        randomFloat(
            0.65f,
            1.15f
        );
}


void createObstacle(
    int index,
    float z
)
{
    if (index < 0 ||
        index >= MAX_OBSTACLES)
        return;

    obstacles[index].lane =
        rand() % 3;

    obstacles[index].z =
        z;

    obstacles[index].type =
        rand() % 4;

    obstacles[index].active =
        true;
}




void createPowerUp(
    int index,
    float z
)
{
    if (index < 0 ||
        index >= MAX_POWERUPS)
        return;

    powerUps[index].lane =
        rand() % 3;

    powerUps[index].z =
        z;

    powerUps[index].type =
        rand() % POWER_COUNT;

    powerUps[index].active =
        true;

    powerUps[index].rotation =
        0.0f;
}




float getFarthestBrickZ()
{
    float farthest =
        -20.0f;

    for (int i = 0;
         i < MAX_BRICKS;
         i++)
    {
        if (bricks[i].active)
        {
            if (bricks[i].z < farthest)
                farthest = bricks[i].z;
        }
    }

    return farthest;
}



void saveSpecificBrick(
    int index
)
{
    if (index < 0 ||
        index >= MAX_BRICKS)
        return;

    if (!bricks[index].active ||
        bricks[index].saved)
        return;


    bricks[index].saved =
        true;

    bricks[index].active =
        false;


    bricksSavedTotal++;


    int gained =
        1;

    if (bricks[index].type == 3)
        gained = 4;

    score += gained;


    if (score > highScore)
        highScore = score;


    coinsCollected += 0;


    updateLevelDifficulty(false);


    effectX =
        laneX[bricks[index].lane];

    effectY =
        0.8f;

    effectZ =
        bricks[index].z;

    effectTime =
        0.0f;

    saveEffect =
        true;


    for (int p = 0;
         p < 25;
         p++)
    {
        spawnParticles(
            effectX,
            effectY,
            effectZ
        );
    }


    float farthest =
        getFarthestBrickZ();

    float distance =
        7.0f -
        score * 0.015f;

    if (distance < 4.5f)
        distance = 4.5f;


    createBrick(
        index,
        farthest - distance
    );
}




void saveBrick()
{
    if (gameState != PLAYING)
        return;

    int nearest = -1;

    float bestDistance =
        4.0f;


    for (int i = 0;
         i < MAX_BRICKS;
         i++)
    {
        if (!bricks[i].active ||
            bricks[i].saved)
            continue;

        if (bricks[i].lane != currentLane)
            continue;

        float d =
            fabs(
                bricks[i].z -
                playerZ
            );

        if (d < bestDistance)
        {
            bestDistance = d;
            nearest = i;
        }
    }


    if (nearest != -1)
        saveSpecificBrick(nearest);
}




void autoSaveAfterLaneChange()
{
    if (gameState != PLAYING)
        return;


    int nearest = -1;

    float bestDistance =
        4.0f;


    for (int i = 0;
         i < MAX_BRICKS;
         i++)
    {
        if (!bricks[i].active ||
            bricks[i].saved)
            continue;

        if (bricks[i].lane != currentLane)
            continue;


        float d =
            fabs(
                bricks[i].z -
                playerZ
            );


        if (d < bestDistance)
        {
            bestDistance = d;
            nearest = i;
        }
    }


    if (nearest != -1)
        saveSpecificBrick(nearest);
}




void moveLeft()
{
    if (gameState != PLAYING)
        return;


    if (currentLane > 0)
    {
        currentLane--;
        targetPlayerX = laneX[currentLane];
        laneChanging = true;

        autoSaveAfterLaneChange();
    }
}




void moveRight()
{
    if (gameState != PLAYING)
        return;


    if (currentLane < 2)
    {
        currentLane++;
        targetPlayerX = laneX[currentLane];
        laneChanging = true;

        autoSaveAfterLaneChange();
    }
}



bool checkBrickCollision()
{
    for (int i = 0;
         i < MAX_BRICKS;
         i++)
    {
        if (!bricks[i].active)
            continue;

        if (bricks[i].saved)
            continue;

        if (bricks[i].lane != currentLane)
            continue;


        float distance =
            fabs(
                bricks[i].z -
                playerZ
            );


        if (distance < 1.15f)
        {
            if (jumpHeight >= (superJumpActive ? 1.55f : 1.15f))
                continue;

            return true;
        }
    }

    return false;
}



bool checkVehicleCollision()
{
    for (int i = 0;
         i < MAX_VEHICLES;
         i++)
    {
        if (!vehicles[i].active)
            continue;

        if (vehicles[i].lane != currentLane)
            continue;


        float distance =
            fabs(
                vehicles[i].z -
                playerZ
            );


        if (distance < 1.7f)
            return true;
    }

    return false;
}


bool checkObstacleCollision()
{
    for (int i = 0;
         i < MAX_OBSTACLES;
         i++)
    {
        if (!obstacles[i].active)
            continue;

        if (obstacles[i].lane != currentLane)
            continue;


        float distance =
            fabs(
                obstacles[i].z -
                playerZ
            );


        if (distance < 1.25f)
            return true;
    }

    return false;
}

void collectCoins()
{
    for (int i = 0;
         i < MAX_COINS;
         i++)
    {
        if (!coins[i].active)
            continue;

        bool sameLane =
            coins[i].lane ==
            currentLane;


        float distance =
            fabs(
                coins[i].z -
                playerZ
            );


        if (magnetActive)
        {
            if (distance < 5.0f)
            {
                coins[i].lane =
                    currentLane;
            }
        }


        if (sameLane &&
            distance < 1.2f)
        {
            coins[i].active =
                false;

            coinsCollected++;

            score++;

            if (score > highScore)
                highScore = score;
        }
    }
}


void activatePower(int type)
{
    if (gameState != PLAYING) return;
    switch(type)
    {
        case POWER_SHIELD: shieldActive=true; shieldTimer=10.0f; break;
        case POWER_SPEED: speedBoostActive=true; speedBoostTimer=6.0f; break;
        case POWER_MAGNET: magnetActive=true; magnetTimer=10.0f; break;
        case POWER_DESTROYER: destroyerActive=true; destroyerTimer=6.0f; destroyNearbyThreats(); break;
        case POWER_SUPERJUMP: superJumpActive=true; superJumpTimer=8.0f; break;
        case POWER_EXTRALIFE: lives++; score+=25; break;
        case POWER_SLOWMO: slowMotionActive=true; slowMotionTimer=6.0f; break;
        case POWER_AUTOSAVE: autoSaveActive=true; autoSaveTimer=8.0f; autoSaveAllNearbyBricks(); break;
    }
}

void destroyNearbyThreats()
{
    for(int i=0;i<MAX_BRICKS;i++)
        if(bricks[i].active && !bricks[i].saved && fabs(bricks[i].z-playerZ)<8.0f)
        { bricks[i].saved=true; bricks[i].active=false; bricksSavedTotal++; score+=3; }
    for(int i=0;i<MAX_OBSTACLES;i++)
        if(obstacles[i].active && fabs(obstacles[i].z-playerZ)<9.0f)
        { obstacles[i].active=false; score+=2; }
    for(int i=0;i<MAX_VEHICLES;i++)
        if(vehicles[i].active && fabs(vehicles[i].z-playerZ)<9.0f)
        { vehicles[i].active=false; carsAvoided++; score+=2; }
    if(score>highScore) highScore=score;
}

void autoSaveAllNearbyBricks()
{
    for(int i=0;i<MAX_BRICKS;i++)
        if(bricks[i].active && !bricks[i].saved && fabs(bricks[i].z-playerZ)<7.0f)
            saveSpecificBrick(i);
}

void collectPowerUps()
{
    for(int i=0;i<MAX_POWERUPS;i++)
    {
        if(!powerUps[i].active || powerUps[i].lane!=currentLane) continue;
        if(fabs(powerUps[i].z-playerZ)<1.4f)
        { int t=powerUps[i].type; powerUps[i].active=false; activatePower(t); }
    }
}



void playerHit()
{
    if (shieldActive || destroyerActive)
    {
        shieldActive =
            false;

        shieldTimer =
            0.0f;

        cameraShake =
            0.4f;

        return;
    }


    lives--;
    health -= 25.0f;
    if (health < 0.0f) health = 0.0f;

    cameraShake =
        0.8f;


    if (lives <= 0)
    {
        gameState =
            GAMEOVER;

        return;
    }



    score -= 2;

    if (score < 0)
        score = 0;
}


void resetGame()
{
    gameState = PLAYING;

    playerX = 0.0f;

    playerY = 0.0f;

    playerZ = 6.0f;
    currentLane = 1;
    targetPlayerX = laneX[1];
    laneChanging = false;

    destinationZ = -180.0f;
    completedLevels = 0;

    score = 0;

    coinsCollected = 0;

    bricksSavedTotal =0;

    carsAvoided =0;

    lives =3;

    fuel = 100.0f;

    level = 1;

    gameSpeed = 0.22f;

    configuredVehicleCount = 0;
    configuredObstacleCount = 0;

    runTime = 0.0f;

    worldTime = 0.0f;

    shieldActive = false;

    speedBoostActive = false;

    magnetActive = false;
    destroyerActive = false;
    superJumpActive = false;
    slowMotionActive = false;
    autoSaveActive = false;

    raining = false;

    fogEnabled = false;

    shieldTimer = 0.0f;
    speedBoostTimer = 0.0f;

    magnetTimer = 0.0f;
    destroyerTimer = 0.0f;
    slowMotionTimer = 0.0f;
    autoSaveTimer = 0.0f;


    saveEffect = false;

    effectTime = 0.0f;

    cameraShake = 0.0f;


    for (int i = 0; i < MAX_BRICKS; i++)
    {
        bricks[i].active = false;
        bricks[i].saved = true;
    }



    for (int i = 0;
         i < MAX_COINS;
         i++)
    {
        createCoin(
            i,
            -10.0f -
            i * 5.0f
        );
    }


    for (int i = 0;
         i < MAX_VEHICLES;
         i++)
    {
        createVehicle(
            i,
            -90.0f -
            i * 32.0f
        );
        vehicles[i].active = false;
    }

    for (int i = 0;
         i < MAX_OBSTACLES;
         i++)
    {
        createObstacle(
            i,
            -110.0f -
            i * 30.0f
        );
        obstacles[i].active = false;
    }

    updateLevelDifficulty(true);


    for (int i = 0;
         i < MAX_POWERUPS;
         i++)
    {
        createPowerUp(
            i,
            -80.0f -
            i * 35.0f
        );
    }


    for (int i = 0;
         i < BUILDING_COUNT;
         i++)
    {
        buildingZ[i] =
            -20.0f -
            i * 12.0f;

        buildingSide[i] =
            (i % 2 == 0)
            ? -1
            : 1;

        buildingType[i] =
            rand() % 5;
    }


    // Trees
    for (int i = 0;
         i < TREE_COUNT;
         i++)
    {
        treeZ[i] =
            -15.0f -
            i * 9.0f;

        treeSide[i] =
            (i % 2 == 0)
            ? -1
            : 1;
    }


    for (int i = 0;
         i < LAMP_COUNT;
         i++)
    {
        lampZ[i] =
            -15.0f -
            i * 13.0f;

        lampSide[i] =
            (i % 2 == 0)
            ? -1
            : 1;
    }

    for (int i = 0;
         i < BILLBOARD_COUNT;
         i++)
    {
        billboardZ[i] =
            -40.0f -
            i * 35.0f;

        billboardSide[i] =
            (i % 2 == 0)
            ? -1
            : 1;
    }


    for (int i = 0;
         i < TRAFFIC_COUNT;
         i++)
    {
        trafficZ[i] =
            -35.0f -
            i * 35.0f;
    }



    for (int c = 0; c < ZEBRA_COUNT; ++c)
        zebraZ[c] = -42.0f - c * 62.0f;


    for (int i = 0; i < MAX_PEDESTRIANS; i++)
    {
        pedestrians[i].animation = randomFloat(0.0f, 10.0f);
        pedestrians[i].active = true;
        pedestrians[i].crossing = false;
        pedestrians[i].crossingIndex = -1;
        pedestrians[i].crossingSpeed = 0.045f;
        pedestrians[i].direction = (i % 2 == 0) ? 1 : -1;

        if (i < 6)
        {

            pedestrians[i].crossing = true;
            pedestrians[i].crossingIndex = i / 2;
            pedestrians[i].direction = (i % 2 == 0) ? 1 : -1;
            pedestrians[i].x = (pedestrians[i].direction > 0) ? -5.8f : 5.8f;
            pedestrians[i].z = zebraZ[pedestrians[i].crossingIndex];
        }
        else
        {

            pedestrians[i].x = (i % 2 == 0) ? 7.25f : -7.25f;
            pedestrians[i].z = -20.0f - i * 12.0f;
        }
    }


    for (int i = 0; i < BIRD_COUNT; ++i)
    {
        birds[i].x = randomFloat(-35.0f, 35.0f);
        birds[i].y = randomFloat(15.0f, 29.0f);
        birds[i].z = randomFloat(-120.0f, -20.0f);
        birds[i].speed = (i % 2 == 0) ? 0.055f : -0.045f;
        birds[i].wing = 0.0f;
        birds[i].phase = randomFloat(0.0f, 6.28f);
        birds[i].active = true;
    }


    for (int i = 0;
         i < RAIN_COUNT;
         i++)
    {
        rainX[i] =
            randomFloat(
                -15.0f,
                15.0f
            );

        rainY[i] =
            randomFloat(
                2.0f,
                25.0f
            );

        rainZ[i] =
            randomFloat(
                -100.0f,
                20.0f
            );
    }



    for (int i = 0;
         i < STAR_COUNT;
         i++)
    {
        starX[i] =
            randomFloat(
                -80.0f,
                80.0f
            );

        starY[i] =
            randomFloat(
                10.0f,
                50.0f
            );

        starZ[i] =
            randomFloat(
                -100.0f,
                -10.0f
            );
    }



    for (int i = 0;
         i < CLOUD_COUNT;
         i++)
    {
        cloudX[i] =
            randomFloat(
                -35.0f,
                35.0f
            );

        cloudY[i] =
            randomFloat(
                16.0f,
                25.0f
            );

        cloudZ[i] =
            randomFloat(
                -100.0f,
                -20.0f
            );
    }



    for (int i = 0;
         i < MAX_PARTICLES;
         i++)
    {
        particles[i].active =
            false;
    }
}



void updateParticles()
{
    for (int i = 0;
         i < MAX_PARTICLES;
         i++)
    {
        if (!particles[i].active)
            continue;


        particles[i].x +=
            particles[i].vx;

        particles[i].y +=
            particles[i].vy;

        particles[i].z +=
            particles[i].vz;

        particles[i].vy -=
            0.004f;

        particles[i].life -=
            0.016f;


        if (particles[i].life <= 0.0f)
            particles[i].active =
                false;
    }
}



void updateRain()
{
    if (!raining)
        return;


    for (int i = 0;
         i < RAIN_COUNT;
         i++)
    {
        rainY[i] -=
            0.6f;

        rainZ[i] +=
            gameSpeed * 2.0f;


        if (rainY[i] < 0.0f)
        {
            rainY[i] =
                randomFloat(
                    15.0f,
                    25.0f
                );

            rainZ[i] =
                randomFloat(
                    -100.0f,
                    20.0f
                );
        }
    }
}



void updateBricks()
{
    for (int i = 0;
         i < MAX_BRICKS;
         i++)
    {
        if (!bricks[i].active)
            continue;


        bricks[i].z +=
            gameSpeed;


        bricks[i].rotation +=
            1.5f;


        if (bricks[i].z > 15.0f)
        {
            float farthest =
                getFarthestBrickZ();

            createBrick(
                i,
                farthest - 7.0f
            );
        }
    }
}



void updateCoins()
{
    for (int i = 0;
         i < MAX_COINS;
         i++)
    {
        if (!coins[i].active)
            continue;


        coins[i].z +=
            gameSpeed;

        coins[i].rotation +=
            5.0f;


        if (coins[i].z > 15.0f)
        {
            coins[i].active =
                true;

            coins[i].lane =
                rand() % 3;

            coins[i].z =
                -100.0f -
                rand() % 80;
        }
    }
}



void updateVehicles()
{
    for (int i = 0; i < MAX_VEHICLES; i++)
    {
        if (!vehicles[i].active)
            continue;

        if (shouldStopForPedestrians(vehicles[i].z))
            continue;

        if (trafficLightIsRed(vehicles[i].z))
            continue;

        float vehicleSpeed = vehicles[i].speedMultiplier;
        if (vehicles[i].type == POLICE)
            vehicleSpeed = 1.35f;
        else if (vehicles[i].type == AMBULANCE)
            vehicleSpeed = 1.20f;

        vehicles[i].z += gameSpeed * vehicleSpeed;

        if (vehicles[i].z > 15.0f)
        {
            vehicles[i].lane = rand() % 3;
            vehicles[i].z = -125.0f - rand() % 80;
            vehicles[i].type = rand() % 6;
            carsAvoided++;
        }
    }
}



void updateObstacles()
{
    for (int i = 0;
         i < MAX_OBSTACLES;
         i++)
    {
        if (!obstacles[i].active)
            continue;


        obstacles[i].z +=
            gameSpeed;


        if (obstacles[i].z > 15.0f)
        {
            obstacles[i].lane =
                rand() % 3;

            obstacles[i].type =
                rand() % 4;

            obstacles[i].z =
                -125.0f -
                rand() % 90;
        }
    }
}



void updatePowerUps()
{
    for (int i = 0;
         i < MAX_POWERUPS;
         i++)
    {
        if (!powerUps[i].active)
            continue;


        powerUps[i].z +=
            gameSpeed;

        powerUps[i].rotation +=
            3.0f;


        if (powerUps[i].z > 15.0f)
        {
            powerUps[i].lane =
                rand() % 3;

            powerUps[i].type =
                rand() % POWER_COUNT;

            powerUps[i].z =
                -150.0f -
                rand() % 100;
        }
    }
}



void updateEnvironment()
{
    updateBirds();
    updatePlane();
    updateWorldFeatures();
    updateClouds();

    for (int i = 0;
         i < BUILDING_COUNT;
         i++)
    {
        buildingZ[i] +=
            gameSpeed;


        if (buildingZ[i] > 20.0f)
        {
            buildingZ[i] =
                -180.0f -
                rand() % 100;

            buildingSide[i] =
                rand() % 2 == 0
                ? -1
                : 1;

            buildingType[i] =
                rand() % 5;
        }
    }


    for (int i = 0;
         i < TREE_COUNT;
         i++)
    {
        treeZ[i] +=
            gameSpeed;


        if (treeZ[i] > 20.0f)
        {
            treeZ[i] =
                -180.0f -
                rand() % 100;
        }
    }


    for (int i = 0;
         i < LAMP_COUNT;
         i++)
    {
        lampZ[i] +=
            gameSpeed;


        if (lampZ[i] > 20.0f)
        {
            lampZ[i] =
                -180.0f -
                rand() % 100;
        }
    }


    for (int i = 0;
         i < BILLBOARD_COUNT;
         i++)
    {
        billboardZ[i] +=
            gameSpeed;


        if (billboardZ[i] > 20.0f)
        {
            billboardZ[i] =
                -200.0f -
                rand() % 120;
        }
    }


    for (int i = 0;
         i < TRAFFIC_COUNT;
         i++)
    {
        trafficZ[i] +=
            gameSpeed;


        if (trafficZ[i] > 20.0f)
        {
            trafficZ[i] =
                -250.0f -
                rand() % 120;
        }
    }


    for (int c = 0; c < ZEBRA_COUNT; ++c)
    {
        zebraZ[c] += gameSpeed;

        if (zebraZ[c] > 20.0f)
        {
            zebraZ[c] = -180.0f - rand() % 80;


            for (int p = 0; p < MAX_PEDESTRIANS; ++p)
            {
                if (pedestrians[p].active &&
                    pedestrians[p].crossingIndex == c)
                {
                    pedestrians[p].crossing = true;
                    pedestrians[p].direction = (p % 2 == 0) ? 1 : -1;
                    pedestrians[p].x = (pedestrians[p].direction > 0) ? -5.8f : 5.8f;
                    pedestrians[p].crossingSpeed = randomFloat(0.035f, 0.055f);
                    pedestrians[p].z = zebraZ[c];
                }
            }
        }
    }

    for (int i = 0; i < MAX_PEDESTRIANS; ++i)
    {
        pedestrians[i].animation += 0.016f;

        if (pedestrians[i].crossing)
        {
            pedestrians[i].z = zebraZ[pedestrians[i].crossingIndex];
            pedestrians[i].x += pedestrians[i].direction * pedestrians[i].crossingSpeed;

            if (pedestrians[i].x > 6.0f || pedestrians[i].x < -6.0f)
            {
                pedestrians[i].crossing = false;
                pedestrians[i].x = (pedestrians[i].direction > 0) ? 6.7f : -6.7f;
                pedestrians[i].z = zebraZ[pedestrians[i].crossingIndex];
            }
        }
        else
        {
            pedestrians[i].z += gameSpeed * 0.8f;

            if (pedestrians[i].z > 20.0f)
                pedestrians[i].z = -150.0f - rand() % 100;
        }
    }

}



void updatePowerTimers()
{
    const float dt=0.016f;
    if(shieldActive && (shieldTimer-=dt)<=0) { shieldTimer=0; shieldActive=false; }
    if(speedBoostActive && (speedBoostTimer-=dt)<=0) { speedBoostTimer=0; speedBoostActive=false; }
    if(magnetActive && (magnetTimer-=dt)<=0) { magnetTimer=0; magnetActive=false; }
    if(destroyerActive) { destroyerTimer-=dt; if(destroyerTimer>0) destroyNearbyThreats(); else {destroyerTimer=0; destroyerActive=false;} }
    if(slowMotionActive && (slowMotionTimer-=dt)<=0) { slowMotionTimer=0; slowMotionActive=false; }
    if(autoSaveActive) { autoSaveTimer-=dt; if(autoSaveTimer>0) autoSaveAllNearbyBricks(); else {autoSaveTimer=0; autoSaveActive=false;} }
}





int getLevelFromScore()
{

    return level;
}

float getLevelBaseSpeed(int currentLevel)
{
    switch (currentLevel)
    {
        case 1: return 0.22f;
        case 2: return 0.32f;
        case 3: return 0.40f;
        case 4: return 0.48f;
        default: return 0.55f;
    }
}

int getVehicleCountForLevel(int currentLevel)
{
    if (currentLevel <= 1) return 2;
    if (currentLevel == 2) return 4;
    if (currentLevel == 3) return 5;
    if (currentLevel == 4) return 6;
    return MAX_VEHICLES;
}

int getObstacleCountForLevel(int currentLevel)
{
    if (currentLevel <= 1) return 2;
    if (currentLevel == 2) return 5;
    if (currentLevel == 3) return 7;
    if (currentLevel == 4) return 10;
    return 12;
}

float roadCurveX(float z)
{

    return 0.0f;
}

bool objectsTooClose(float z, int ignoreObstacle)
{
    const float MIN_OBJECT_GAP = 24.0f;

    for (int i = 0; i < MAX_VEHICLES; ++i)
        if (vehicles[i].active && fabs(vehicles[i].z - z) < MIN_OBJECT_GAP)
            return true;

    for (int i = 0; i < MAX_OBSTACLES; ++i)
        if (i != ignoreObstacle && obstacles[i].active &&
            fabs(obstacles[i].z - z) < MIN_OBJECT_GAP)
            return true;

    return false;
}

void updateSmoothLaneChange()
{
    float dx = targetPlayerX - playerX;

    if (fabs(dx) < 0.03f)
    {
        playerX = targetPlayerX;
        laneChanging = false;
        return;
    }

    playerX += dx * LANE_CHANGE_SPEED;
    laneChanging = true;
}

void drawDestinationMarker()
{
    glDisable(GL_LIGHTING);

    glColor3f(0.05f, 0.85f, 0.25f);

    glPushMatrix();
    glTranslatef(roadCurveX(destinationZ), 3.0f, destinationZ);
    drawCube(0.28f, 6.0f, 0.28f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(roadCurveX(destinationZ) + 7.8f, 3.0f, destinationZ);
    drawCube(0.28f, 6.0f, 0.28f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(roadCurveX(destinationZ) + 3.9f, 5.9f, destinationZ);
    drawCube(8.0f, 0.30f, 0.30f);
    glPopMatrix();


    glColor3f(1.0f, 0.85f, 0.05f);
    glPushMatrix();
    glTranslatef(roadCurveX(destinationZ) + 3.9f, -0.02f, destinationZ);
    drawCube(8.0f, 0.025f, 0.35f);
    glPopMatrix();

    glEnable(GL_LIGHTING);
}

void startNextLevel()
{
    completedLevels++;

    if (level < 5)
        level++;
    else
        level = 1;

    destinationZ = (level == 1) ? -180.0f :
                   (level == 2) ? -250.0f :
                   (level == 3) ? -290.0f :
                   (level == 4) ? -330.0f : -370.0f;


    currentLane = 1;
    targetPlayerX = laneX[1];
    laneChanging = true;

    updateLevelDifficulty(true);


    for (int i = 0; i < MAX_VEHICLES; ++i)
        if (vehicles[i].active && vehicles[i].z > -70.0f)
            vehicles[i].z = -110.0f - i * 34.0f;

    for (int i = 0; i < MAX_OBSTACLES; ++i)
        if (obstacles[i].active && obstacles[i].z > -70.0f)
            obstacles[i].z = -125.0f - i * 30.0f;
}

void updateLevelDifficulty(bool forceUpdate)
{
    int newLevel = getLevelFromScore();
    int wantedVehicles = getVehicleCountForLevel(newLevel);
    int wantedObstacles = getObstacleCountForLevel(newLevel);

    bool levelChanged = (newLevel != level);
    level = newLevel;

    if (!forceUpdate && !levelChanged &&
        wantedVehicles == configuredVehicleCount &&
        wantedObstacles == configuredObstacleCount)
    {
        return;
    }

    for (int i = configuredVehicleCount;
         i < wantedVehicles && i < MAX_VEHICLES;
         i++)
    {
        vehicles[i].lane = rand() % 3;
        vehicles[i].z = -95.0f - i * 32.0f - rand() % 30;
        vehicles[i].type = rand() % 6;
        vehicles[i].speedMultiplier = randomFloat(0.65f, 1.15f);
        vehicles[i].active = true;
    }

    for (int i = configuredObstacleCount;
         i < wantedObstacles && i < MAX_OBSTACLES;
         i++)
    {
        obstacles[i].lane = rand() % 3;
        obstacles[i].type = rand() % 4;
        obstacles[i].z = -105.0f - i * 30.0f - rand() % 35;
        obstacles[i].active = true;
    }

    configuredVehicleCount = wantedVehicles;
    configuredObstacleCount = wantedObstacles;
}



void updateGame()
{
    if (gameState != PLAYING)
        return;


    runTime +=0.016f;


    worldTime += dayCycleSpeed;

    if (worldTime >= 1.0f)
        worldTime -= 1.0f;


    if ((rand() % 1500) == 1)
    {
        raining =
            !raining;
    }


    if ((rand() % 2200) == 1)
    {
        fogEnabled =
            !fogEnabled;
    }

    updateLevelDifficulty(false);

    float baseSpeed = getLevelBaseSpeed(level);
    float actualSpeed = baseSpeed;
    if (speedBoostActive) actualSpeed *= 1.8f;
    if (slowMotionActive) actualSpeed *= 0.45f;


    if (shouldStopForPedestrians(playerZ))
        actualSpeed = 0.0f;

    if (trafficLightIsRed(playerZ))
        actualSpeed = 0.0f;

    gameSpeed = actualSpeed;

    fuel -= 0.012f * (actualSpeed / 0.22f);
    if (fuel < 0.0f) fuel = 0.0f;
    if (fuel <= 0.0f) actualSpeed *= 0.55f;
    if (actualSpeed > 0.0f)
        stamina -= 0.035f;
    else
        stamina += 0.08f;
    if (stamina < 0.0f) stamina = 0.0f;
    if (stamina > 100.0f) stamina = 100.0f;
    if (stamina <= 0.0f) actualSpeed *= 0.65f;
    gameSpeed = actualSpeed;

    updateSmoothLaneChange();

    destinationZ += gameSpeed;
    if (destinationZ >= playerZ - 1.5f)
        startNextLevel();



    updateCoins();

    updateVehicles();

    updateObstacles();

    updatePowerUps();

    updateEnvironment();

    updateRain();

    updatePowerTimers();

    updateParticles();


    collectCoins();

    collectPowerUps();


    if (checkBrickCollision())
    {
        playerHit();
    }


    if (checkVehicleCollision())
    {
        playerHit();
    }


    if (checkObstacleCollision())
    {
        playerHit();
    }


    if (cameraShake > 0.0f)
    {
        cameraShake -= 0.025f;

        if (cameraShake < 0.0f)
            cameraShake = 0.0f;
    }


    if (saveEffect)
    {
        effectTime += 0.016f;

        if (effectTime > 0.7f)
        {
            saveEffect = false;
        }
    }



    if (carsAvoided >= 10)
        mission1Complete = true;

    if (coinsCollected >= 20)
        mission2Complete = true;

}




void setupLighting()
{
    glEnable(GL_LIGHTING);

    glEnable(GL_LIGHT0);

    glEnable(GL_LIGHT1);

    glEnable(GL_COLOR_MATERIAL);

    glColorMaterial(
        GL_FRONT_AND_BACK,
        GL_AMBIENT_AND_DIFFUSE
    );


    GLfloat light0Pos[] =
    {
        -10.0f,
        18.0f,
        10.0f,
        1.0f
    };


    GLfloat light0Diffuse[] =
    {
        1.0f,
        1.0f,
        1.0f,
        1.0f
    };


    glLightfv(
        GL_LIGHT0,
        GL_POSITION,
        light0Pos
    );


    glLightfv(
        GL_LIGHT0,
        GL_DIFFUSE,
        light0Diffuse
    );


    GLfloat light1Pos[] =
    {
        10.0f,
        10.0f,
        -20.0f,
        1.0f
    };


    GLfloat light1Diffuse[] =
    {
        0.35f,
        0.45f,
        1.0f,
        1.0f
    };

    glLightfv(GL_LIGHT1,GL_POSITION,light1Pos);

    glLightfv(GL_LIGHT1,GL_DIFFUSE,light1Diffuse);
}

void updateAtmosphere()
{
    GLfloat fogColor[4];
    if (isNight())
    {
        fogColor[0] = 0.015f;
        fogColor[1] = 0.025f;
        fogColor[2] = 0.070f;
    }
    else if (isSunset())
    {
        fogColor[0] = 0.52f;
        fogColor[1] = 0.28f;
        fogColor[2] = 0.20f;
    }
    else
    {
        fogColor[0] = 0.58f;
        fogColor[1] = 0.72f;
        fogColor[2] = 0.86f;
    }
    fogColor[3] = 1.0f;

    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_START, 38.0f);
    glFogf(GL_FOG_END, 155.0f);
    glHint(GL_FOG_HINT, GL_NICEST);
}




void drawText(float x,float y,const char* text)
{
    glDisable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);

    glPushMatrix();

    glLoadIdentity();

    gluOrtho2D(0,1200,0,700);


    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();

    glLoadIdentity();


    glColor3f(1.0f,1.0f,1.0f);


    glRasterPos2f(x,y);


    for (int i = 0;text[i] != '\0';i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,text[i]);
    }


    glPopMatrix();


    glMatrixMode(GL_PROJECTION);

    glPopMatrix();


    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_LIGHTING);
}


void drawHUD()
{
    char buffer[256];

    sprintf(buffer,"TOUR RIDER   SCORE: %d   HIGH SCORE: %d",score,highScore);
    drawText(25,665,buffer);

    sprintf(buffer,"LEVEL: %d   COINS: %d",level,coinsCollected);
    drawText(25,635,buffer);

    sprintf(buffer,"LIVES: %d",lives);
    drawText(25,605,buffer);

    sprintf(buffer, "FUEL: %.0f%%", fuel);
    drawText(25, 575, buffer);




    if (shieldActive) { sprintf(buffer,"SHIELD: %.1f",shieldTimer); drawText(900,665,buffer); }
    if (speedBoostActive) { sprintf(buffer,"SPEED BOOST: %.1f",speedBoostTimer); drawText(900,635,buffer); }
    if (magnetActive) { sprintf(buffer,"MAGNET: %.1f",magnetTimer); drawText(900,605,buffer); }
    if (destroyerActive) { sprintf(buffer,"DESTROYER: %.1f",destroyerTimer); drawText(900,575,buffer); }
    if (superJumpActive) { sprintf(buffer,"SUPER JUMP: %.1f",superJumpTimer); drawText(900,545,buffer); }
    if (slowMotionActive) { sprintf(buffer,"SLOW MOTION: %.1f",slowMotionTimer); drawText(900,515,buffer); }
    if (autoSaveActive) { sprintf(buffer,"AUTO SAVE: %.1f",autoSaveTimer); drawText(900,485,buffer); }

    drawText(25,45,"Arrow = Smooth Lane Change | 1-8 = Power | P = Pause | R = Restart");

    sprintf(buffer, "DESTINATION: LEVEL %d FINISH", level);
    drawText(25,65, buffer);


    if (mission1Complete)
        drawText(950,80,"MISSION 1 COMPLETE!");

    if (mission2Complete)
        drawText(950,55,"MISSION 2 COMPLETE!");

    if (mission3Complete)
        drawText(950,30,"MISSION 3 COMPLETE!");
}



void drawGameState()
{
    if (gameState == PLAYING)
        return;


    glDisable(GL_LIGHTING);


    glMatrixMode(GL_PROJECTION);

    glPushMatrix();

    glLoadIdentity();

    gluOrtho2D(0,1200,0,700);


    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();

    glLoadIdentity();


    glColor4f(0.0f,0.0f,0.0f,0.65f);


    glBegin(GL_QUADS);

    glVertex2f(0,0);
    glVertex2f(1200,0);
    glVertex2f(1200,700);
    glVertex2f(0,700);

    glEnd();


    glColor3f(1.0f,1.0f,1.0f);


    const char* message;


    if (gameState == GAMEOVER)
    {
        message = "GAME OVER";

        glRasterPos2f(520,380);
    }
    else
    {
        message = "PAUSED";

        glRasterPos2f(540,380);
    }


    for (int i = 0;message[i] != '\0';i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,message[i]);
    }


    if (gameState == GAMEOVER)
    {
        glRasterPos2f(475,330);

        const char* restart ="Press R to Restart";

        for (int i = 0;restart[i] != '\0';i++)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,restart[i]);
        }
    }


    glPopMatrix();


    glMatrixMode(GL_PROJECTION);

    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_LIGHTING);
}


void display()
{
    glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);


    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();


    float shakeX = randomFloat(-cameraShake,cameraShake);

    float shakeY = randomFloat(-cameraShake,cameraShake);


    if (firstPersonView)
    {
        gluLookAt(playerX + shakeX,1.38f + shakeY,playerZ + 0.48f,
                  playerX + roadCurveX(playerZ - 18.0f),1.18f,playerZ - 22.0f,0.0f,1.0f,0.0f);
    }
    else
    {
        gluLookAt(shakeX,5.5f + shakeY,14.5f,0.0f,1.5f,-18.0f,0.0f,1.0f,0.0f);
    }


    if (isNight())
    {
        GLfloat nightAmbient[] = {0.08f,0.08f,0.16f,1.0f};

        glLightModelfv(GL_LIGHT_MODEL_AMBIENT,nightAmbient);
    }
    else
    {
        GLfloat dayAmbient[] ={0.35f,0.35f,0.35f,1.0f};

        glLightModelfv(GL_LIGHT_MODEL_AMBIENT,dayAmbient);
    }

    updateAtmosphere();


    drawSky();
    drawRainbow();
    drawPlane();
    drawModernSkyline();
    drawBirds();

    drawRoad();
    drawDestinationMarker();


    for (int i = 0;i < BUILDING_COUNT;i++)
    {
        float x = buildingSide[i] *(10.0f +(i % 3) * 2.0f);

        drawBuilding(x,buildingZ[i],buildingType[i],i);
    }



    for (int i = 0;i < TREE_COUNT;i++)
    {
        float x =treeSide[i] *8.5f;

        drawTree(x,treeZ[i]);
    }

    for (int i = 0;i < LAMP_COUNT;i++)
    {
        float x =lampSide[i] *6.8f;

        drawStreetLamp(x,lampZ[i]);
    }

    for (int i = 0;i < BILLBOARD_COUNT;i++)
    {
        float x =billboardSide[i] *9.0f;

        drawBillboard(x,billboardZ[i]);
    }


    for (int i = 0;i < TRAFFIC_COUNT;i++)
    {
        drawTrafficLight(-5.8f,trafficZ[i]);

        drawTrafficLight(5.8f,trafficZ[i]);
    }

    for (int i = 0;i < MAX_PEDESTRIANS;i++)
    {
        if (pedestrians[i].active)
        {
            drawPedestrian(pedestrians[i].x,pedestrians[i].z,pedestrians[i].animation);
        }
    }

    for (int i = 0;i < MAX_VEHICLES;i++)
    {
        if (vehicles[i].active)
        {
            drawVehicle(laneX[vehicles[i].lane] + roadCurveX(vehicles[i].z),vehicles[i].z,vehicles[i].type);
        }
    }


    for (int i = 0;i < MAX_OBSTACLES;i++)
    {
        if (obstacles[i].active)
        {
            drawObstacle(laneX[obstacles[i].lane] + roadCurveX(obstacles[i].z),obstacles[i].z,obstacles[i].type);
        }
    }

    for (int i = 0;i < MAX_COINS;i++)
    {
        if (coins[i].active)
        {
            drawCoin(laneX[coins[i].lane] + roadCurveX(coins[i].z),coins[i].z,coins[i].rotation);
        }
    }

    for (int i = 0;i < MAX_POWERUPS;i++)
    {
        if (powerUps[i].active)
        {
            drawPowerUp(laneX[powerUps[i].lane] + roadCurveX(powerUps[i].z),powerUps[i].z,powerUps[i].type,powerUps[i].rotation);
        }
    }


    if (firstPersonView)
        drawFirstPersonCockpit();
    else
        drawPlayer();


    drawParticles();

    drawSaveEffect();

    drawRain();

    drawWorldFeatures();

    glDisable(GL_FOG);

    drawHUD();

    drawGameState();


    glutSwapBuffers();
}



void reshape(int width,int height)
{
    if (height == 0)
        height = 1;


    float aspect =(float)width /(float)height;


    glViewport(0,0,width,height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60.0,aspect,0.1,300.0);
    glMatrixMode(GL_MODELVIEW);
}



void keyboard(unsigned char key,int x,int y)
{
    switch (key)
    {
        case 27:
            exit(0);
            break;


        case 'p':
        case 'P':
            if (gameState == PLAYING)
            {
                gameState = PAUSED;
            }
            else if (gameState == PAUSED)
            {
                gameState = PLAYING;
            }
            break;


        case 'v':
        case 'V':
            firstPersonView = !firstPersonView;
            break;


        case 'h':
        case 'H':
            playHorn();
            break;


        case 'r':
        case 'R':
            resetGame();
            break;


        case '1': activatePower(POWER_SHIELD); break;
        case '2': activatePower(POWER_SPEED); break;
        case '3': activatePower(POWER_MAGNET); break;
        case '4': activatePower(POWER_DESTROYER); break;
        case '5': activatePower(POWER_SUPERJUMP); break;
        case '6': activatePower(POWER_EXTRALIFE); break;
        case '7': activatePower(POWER_SLOWMO); break;
        case '8': activatePower(POWER_AUTOSAVE); break;
    }
}



void specialKeyboard(int key,int x,int y)
{
    if (key == GLUT_KEY_LEFT)
    {
        moveLeft();
    }
    else if (key == GLUT_KEY_RIGHT)
    {
        moveRight();
    }
}



void timer(int value)
{
    updateGame();
    glutPostRedisplay();
    glutTimerFunc(16,timer,0);
}



int main(int argc,char** argv)
{
    srand((unsigned int)time(NULL));
    glutInit(&argc,argv);

    glutInitDisplayMode(GLUT_DOUBLE |GLUT_RGB |GLUT_DEPTH);
    glutInitWindowSize(1200,700);
    glutInitWindowPosition(80,40);


    glutCreateWindow("CITY RIDER - 3D CITY TOUR");

    glEnable(GL_DEPTH_TEST);
    //glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    setupLighting();
    resetGame();
    startCarAudio();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);

    glutTimerFunc(16,timer,0);


    glutMainLoop();


    return 0;
}
