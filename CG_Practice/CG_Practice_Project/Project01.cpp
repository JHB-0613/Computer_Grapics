//전역 상수 및 구조체 정의
#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION
#define MAZE_SIZE 20
#define WALL_SIZE 5.0f
#define TERRAIN_SIZE 20
#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include "stb_image.h"

float heights[TERRAIN_SIZE][TERRAIN_SIZE] = { 0.0f };
const float RAMP_START_X = WALL_SIZE * 0.5f;
const float RAMP_START_Z = WALL_SIZE * 11.0f;
const float RAMP_LENGTH_Z = WALL_SIZE * 5.0f;
const float RAMP_WIDTH_X = WALL_SIZE;
const float RAMP_HEIGHT = 3.0f;


int maze_map[MAZE_SIZE][MAZE_SIZE] = {
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1},
    {1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1},
    {1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1},
    {1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1},
    {1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1},
    {1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1},
    {1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1}
};

float camX = WALL_SIZE * 1.0f;
float camY = 1.7f;
float camZ = WALL_SIZE * 0.0f;

float dirX = 0.0f;
float dirY = 0.0f;
float dirZ = -1.0f;

float angle = 90.0f;
static int lastTime = 0;
float moveSpeed = 8.0f;
float mouseSensitivity = 0.05f;
float pitchAngle = 0.0f;

GLuint wall_texture_id;
GLuint floor_texture_id;
GLuint ceiling_texture_id;
GLuint image_texture_id;
GLuint barrel_tex_id;
GLuint drum_tex_id;
GLuint box_tex_id;
GLuint chest_tex_id;
GLuint win_texture_id;
int window_width = 800;
int window_height = 600;

class xPoint3D {
public:
    float x, y, z, w;
    xPoint3D() { x = y = z = 0; w = 1; };
};

struct MyFace {
    int ip[3];
};

struct ModelData {
    std::vector<xPoint3D> vertices;
    std::vector<MyFace> faces;
};
std::vector<ModelData> loadedModels;

struct ModelInstance {
    float x;
    float y;
    float z;
    bool isVisible;
    bool isCollected;
    int dataIndex;
    GLuint textureID;
    float scaleX;
    float scaleY;
    float scaleZ;
};

std::vector<ModelInstance> models;

float modelCollisionRadius = 3.0f;
float playerRadius = 0.2f;
bool isGameOver = false;

const float EXIT_X = WALL_SIZE * 17.0f;
const float EXIT_Z = WALL_SIZE * 19.0f;
const float EXIT_RADIUS_SQ = 1.0f * 1.0f;

bool isWPressed = false;
bool isSPressed = false;
bool isAPressed = false;
bool isDPressed = false;

void UpdateDirection();
//초기화 및 리소스 로딩
GLuint LoadTexture(const char* filename) {
    GLuint textureID = 0;
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

    if (data) {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format = GL_RGB;
        if (nrChannels == 4)
            format = GL_RGBA;
        else if (nrChannels == 3)
            format = GL_RGB;

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            width,
            height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data
        );

        std::cout << "DEBUG: Texture ID " << textureID << " loaded for " << filename
            << " (" << width << "x" << height << ", Channels: " << nrChannels << ")" << std::endl;

        stbi_image_free(data);
    }
    else {
        std::cerr << "ERROR: Failed to load texture: " << filename << ". Check file existence and path." << std::endl;
    }

    return textureID;
}

void LoadAllTextures() {

    wall_texture_id = LoadTexture("textures/brick.bmp");
    floor_texture_id = LoadTexture("textures/floor.bmp");
    ceiling_texture_id = LoadTexture("textures/ceiling.png");
    image_texture_id = LoadTexture("textures/hud_image.png");
    win_texture_id = LoadTexture("textures/win_screen.png");
    barrel_tex_id = LoadTexture("textures/barrel.jpg");
    drum_tex_id = LoadTexture("textures/Drum.jpg");
    box_tex_id = LoadTexture("textures/box.png");
    chest_tex_id = LoadTexture("textures/chest.png");

    if (wall_texture_id > 0 && floor_texture_id > 0) {
        std::cout << "All primary textures loaded successfully." << std::endl;
        glEnable(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Texture loading failed." << std::endl;
    }
}

int LoadSORModel(const char* filename) {
    ModelData newModel;
    std::ifstream fin(filename);

    if (!fin.is_open()) {
        std::cerr << "ERROR: Failed to open SOR model file: " << filename << std::endl;
        return -1;
    }

    std::string tag;
    char equal_sign;
    int count;

    if (fin >> tag && tag == "VERTEX" && fin >> equal_sign && equal_sign == '=' && fin >> count) {
        newModel.vertices.reserve(count);
        for (int i = 0; i < count; ++i) {
            xPoint3D pt;
            fin >> pt.x >> pt.y >> pt.z;
            newModel.vertices.push_back(pt);
        }
    }

    if (fin >> tag && tag == "FACE" && fin >> equal_sign && equal_sign == '=' && fin >> count) {
        newModel.faces.reserve(count);
        for (int i = 0; i < count; ++i) {
            MyFace face;
            fin >> face.ip[0] >> face.ip[1] >> face.ip[2];
            newModel.faces.push_back(face);
        }
    }

    fin.close();
    std::cout << "DEBUG: Loaded " << filename << ". Vertices: " << newModel.vertices.size() << ", Faces: " << newModel.faces.size() << std::endl;

    loadedModels.push_back(newModel);
    return loadedModels.size() - 1;
}

void InitModels(int barrelIdx, int drumIdx, int boxIdx, int chestIdx, GLuint barrelTexID, GLuint drumTexID, GLuint boxTexID, GLuint chestTexID) {
    models.clear();

    models.push_back({
        WALL_SIZE * 1.0f, 0.0f, WALL_SIZE * 3.0f, true, false,
        barrelIdx, barrelTexID,
        0.03f, 0.02f, 0.03f
        });

    models.push_back({
        WALL_SIZE * 15.0f, 0.0f, WALL_SIZE * 1.0f, true, false,
        drumIdx, drumTexID,
        0.03f, 0.02f, 0.03f
        });

    models.push_back({
        WALL_SIZE * 9.0f, 0.75f, WALL_SIZE * 7.0f, true, false,
        boxIdx, boxTexID,
        1.5f, 1.5f, 1.5f
        });
    models.push_back({
    WALL_SIZE * 17.0f, 0.0f, WALL_SIZE * 11.0f, true, false,
    chestIdx, chestTexID,
    0.05f, 0.05f, 0.05f
        });

    std::cout << "DEBUG: " << models.size() << " models initialized." << std::endl;
}

void init() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SMOOTH);

    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    LoadAllTextures();
    loadedModels.clear();
    int barrelModelIndex = LoadSORModel("Bin/Barrel_Model.dat");
    int DrumModelIndex = LoadSORModel("Bin/Gas_Drum.dat");
    int boxModelIndex = -1;
    int chestModelIndex = LoadSORModel("Bin/Treasure_Chest.dat");


    UpdateDirection();

    InitModels(barrelModelIndex, DrumModelIndex, boxModelIndex, chestModelIndex, barrel_tex_id, drum_tex_id, box_tex_id, chest_tex_id);
    lastTime = glutGet(GLUT_ELAPSED_TIME);
}

//렌더링 함수
void DrawTexturedWall(float size) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wall_texture_id);
    glColor3f(1.0f, 1.0f, 1.0f);

    float s2 = size / 2.0f;
    float repeat = 1.0f;

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s2, -s2, s2);
    glTexCoord2f(repeat, 0.0f); glVertex3f(s2, -s2, s2);
    glTexCoord2f(repeat, repeat); glVertex3f(s2, s2, s2);
    glTexCoord2f(0.0f, repeat); glVertex3f(-s2, s2, s2);

    glTexCoord2f(repeat, 0.0f); glVertex3f(-s2, -s2, -s2);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(s2, -s2, -s2);
    glTexCoord2f(0.0f, repeat); glVertex3f(s2, s2, -s2);
    glTexCoord2f(repeat, repeat); glVertex3f(-s2, s2, -s2);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(s2, -s2, s2);
    glTexCoord2f(repeat, 0.0f); glVertex3f(s2, -s2, -s2);
    glTexCoord2f(repeat, repeat); glVertex3f(s2, s2, -s2);
    glTexCoord2f(0.0f, repeat); glVertex3f(s2, s2, s2);

    glTexCoord2f(repeat, 0.0f); glVertex3f(-s2, -s2, s2);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s2, -s2, -s2);
    glTexCoord2f(0.0f, repeat); glVertex3f(-s2, s2, -s2);
    glTexCoord2f(repeat, repeat); glVertex3f(-s2, s2, s2);

    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void DrawTexturedCube(float size, GLuint textureID) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glColor3f(1.0f, 1.0f, 1.0f);

    float s2 = size / 2.0f;
    float repeat = 1.0f;

    glBegin(GL_QUADS);

    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s2, -s2, s2);
    glTexCoord2f(repeat, 0.0f); glVertex3f(s2, -s2, s2);
    glTexCoord2f(repeat, repeat); glVertex3f(s2, s2, s2);
    glTexCoord2f(0.0f, repeat); glVertex3f(-s2, s2, s2);

    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(repeat, 0.0f); glVertex3f(-s2, -s2, -s2);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(s2, -s2, -s2);
    glTexCoord2f(0.0f, repeat); glVertex3f(s2, s2, -s2);
    glTexCoord2f(repeat, repeat); glVertex3f(-s2, s2, -s2);

    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(s2, -s2, s2);
    glTexCoord2f(repeat, 0.0f); glVertex3f(s2, -s2, -s2);
    glTexCoord2f(repeat, repeat); glVertex3f(s2, s2, -s2);
    glTexCoord2f(0.0f, repeat); glVertex3f(s2, s2, s2);

    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(repeat, 0.0f); glVertex3f(-s2, -s2, s2);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s2, -s2, -s2);
    glTexCoord2f(0.0f, repeat); glVertex3f(-s2, s2, -s2);
    glTexCoord2f(repeat, repeat); glVertex3f(-s2, s2, s2);

    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, repeat); glVertex3f(-s2, s2, -s2);
    glTexCoord2f(repeat, repeat); glVertex3f(s2, s2, -s2);
    glTexCoord2f(repeat, 0.0f); glVertex3f(s2, s2, s2);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s2, s2, s2);

    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s2, -s2, -s2);
    glTexCoord2f(repeat, 0.0f); glVertex3f(s2, -s2, -s2);
    glTexCoord2f(repeat, repeat); glVertex3f(s2, -s2, s2);
    glTexCoord2f(0.0f, repeat); glVertex3f(-s2, -s2, s2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void RenderModel(int dataIndex, GLuint textureID) {
    if (dataIndex < 0 || dataIndex >= loadedModels.size()) return;
    const ModelData& modelData = loadedModels[dataIndex];

    if (modelData.faces.empty()) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glColor3f(1.0f, 1.0f, 1.0f);

    const float Y_MIN = 0.0f;
    const float Y_MAX = 155.0f;

    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < modelData.faces.size(); ++i) {
        const MyFace& face = modelData.faces[i];

        for (int j = 0; j < 3; ++j) {
            const xPoint3D& vertex = modelData.vertices[face.ip[j]];

            float v = (vertex.y - Y_MIN) / (Y_MAX - Y_MIN);
            float angle_rad = std::atan2(vertex.z, vertex.x);
            float u = (angle_rad / (2.0f * M_PI));
            if (u < 0.0f) { u += 1.0f; }

            glTexCoord2f(u, v);
            glVertex3f(vertex.x, vertex.y, vertex.z);
        }
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void RenderRamp() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, floor_texture_id);
    glColor3f(1.0f, 1.0f, 1.0f);

    const int SEGMENTS = 50;
    float dz = RAMP_LENGTH_Z / (float)SEGMENTS;
    float ramp_half_width = RAMP_WIDTH_X / 2.0f;
    float texture_repeat_z = RAMP_LENGTH_Z / WALL_SIZE;

    float centerX = RAMP_START_X + WALL_SIZE / 2.0f;

    for (int i = 0; i < SEGMENTS; ++i) {
        glBegin(GL_QUAD_STRIP);

        for (int j = 0; j <= 1; ++j) {

            float z_offset_curr = (float)i * dz;
            float z_offset_next = (float)(i + 1) * dz;

            float normalized_z_curr = z_offset_curr / RAMP_LENGTH_Z;
            float normalized_z_next = z_offset_next / RAMP_LENGTH_Z;

            float angle_curr = normalized_z_curr * 2.0f * M_PI;
            float angle_next = normalized_z_next * 2.0f * M_PI;

            float y_curr = RAMP_HEIGHT * (1.0f - std::cos(angle_curr)) / 2.0f;
            float y_next = RAMP_HEIGHT * (1.0f - std::cos(angle_next)) / 2.0f;


            if (i < SEGMENTS) {
                float x_left = centerX - ramp_half_width;
                float x_right = centerX + ramp_half_width;

                glTexCoord2f(0.0f, normalized_z_curr * texture_repeat_z);
                glVertex3f(x_left, y_curr, RAMP_START_Z + z_offset_curr);

                glTexCoord2f(1.0f, normalized_z_curr * texture_repeat_z);
                glVertex3f(x_right, y_curr, RAMP_START_Z + z_offset_curr);

                glTexCoord2f(0.0f, normalized_z_next * texture_repeat_z);
                glVertex3f(x_left, y_next, RAMP_START_Z + z_offset_next);

                glTexCoord2f(1.0f, normalized_z_next * texture_repeat_z);
                glVertex3f(x_right, y_next, RAMP_START_Z + z_offset_next);
            }
        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

void RenderMaze() {
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            if (maze_map[i][j] == 1) {
                glPushMatrix();
                float xPos = j * WALL_SIZE;
                float zPos = i * WALL_SIZE;

                glTranslatef(xPos, WALL_SIZE / 2.0f, zPos);
                DrawTexturedWall(WALL_SIZE);
                glPopMatrix();
            }
        }
    }

    float maze_size_world = MAZE_SIZE * WALL_SIZE;
    float texture_repeat = MAZE_SIZE;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, floor_texture_id);
    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
    glTranslatef((MAZE_SIZE - 1) * WALL_SIZE / 2.0f, 0.0f, (MAZE_SIZE - 1) * WALL_SIZE / 2.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, texture_repeat);
    glVertex3f(-maze_size_world / 2.0f, 0.0f, maze_size_world / 2.0f);
    glTexCoord2f(texture_repeat, texture_repeat);
    glVertex3f(maze_size_world / 2.0f, 0.0f, maze_size_world / 2.0f);
    glTexCoord2f(texture_repeat, 0.0f);
    glVertex3f(maze_size_world / 2.0f, 0.0f, -maze_size_world / 2.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-maze_size_world / 2.0f, 0.0f, -maze_size_world / 2.0f);
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    RenderRamp();


    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ceiling_texture_id);
    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
    glTranslatef((MAZE_SIZE - 1) * WALL_SIZE / 2.0f, WALL_SIZE, (MAZE_SIZE - 1) * WALL_SIZE / 2.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, texture_repeat);
    glVertex3f(-maze_size_world / 2.0f, 0.0f, maze_size_world / 2.0f);
    glTexCoord2f(texture_repeat, texture_repeat);
    glVertex3f(maze_size_world / 2.0f, 0.0f, maze_size_world / 2.0f);
    glTexCoord2f(texture_repeat, 0.0f);
    glVertex3f(maze_size_world / 2.0f, 0.0f, -maze_size_world / 2.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-maze_size_world / 2.0f, 0.0f, -maze_size_world / 2.0f);
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void RenderBitmapString(float x, float y, void* font, const char* string) {
    glColor3f(91.0f, 247.0f, 48.0f);
    glRasterPos2f(x, y);
    while (*string) {
        glutBitmapCharacter(font, *string++);
    }
}

void RenderImageOnBottom() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, image_texture_id);
    glColor3f(1.0f, 1.0f, 1.0f);

    int image_width = 512;
    int image_height = 256;
    int padding_x = 144;
    int padding_y = 0;

    float x1 = padding_x;
    float y1 = padding_y;
    float x2 = padding_x + image_width;
    float y2 = padding_y + image_height;

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x1, y1);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(x2, y1);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(x2, y2);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(x1, y2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void RenderWinScreen() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, win_texture_id);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(window_width, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(window_width, window_height);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, window_height);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(
        camX, camY, camZ,
        camX + dirX, camY + dirY, camZ + dirZ,
        0.0f, 1.0f, 0.0f
    );

    RenderMaze();


    for (const auto& model : models) {
        if (model.isVisible) {
            glPushMatrix();

            glTranslatef(model.x, model.y, model.z);

            if (model.dataIndex == -1) {
                DrawTexturedCube(model.scaleX, model.textureID);
            }
            else {
                glScalef(model.scaleX, model.scaleY, model.scaleZ);
                RenderModel(model.dataIndex, model.textureID);
            }
            glPopMatrix();
        }
    }


    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, window_width, 0, window_height, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (isGameOver) {
        RenderWinScreen();
    }
    else {
        RenderImageOnBottom();

        int totalModels = models.size();
        int collectedCount = 0;
        for (const auto& model : models) {
            if (model.isCollected) {
                collectedCount++;
            }
        }

        std::string statusText = "Score: " + std::to_string(collectedCount) + " / " + std::to_string(totalModels);

        RenderBitmapString(
            0,
            550,
            GLUT_BITMAP_HELVETICA_18,
            statusText.c_str()
        );
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
}

void reshape(int w, int h) {
    window_width = w;
    window_height = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60, (GLfloat)w / (GLfloat)h, 0.1, 500.0);
}
//게임 로직 및 입력 처리
float GetRampHeight(float x, float z) {

    float minX = RAMP_START_X;
    float maxX = RAMP_START_X + RAMP_WIDTH_X;

    float minZ = RAMP_START_Z;
    float maxZ = RAMP_START_Z + RAMP_LENGTH_Z;

    if (x >= minX && x <= maxX && z >= minZ && z <= maxZ) {
        float normalized_z = (z - RAMP_START_Z) / RAMP_LENGTH_Z;

        float angle_rad = normalized_z * 2.0f * M_PI;
        float y_height = RAMP_HEIGHT * (1.0f - std::cos(angle_rad)) / 2.0f;

        return y_height;
    }

    return 0.0f;
}

void UpdateDirection() {
    float yawRadian = angle * M_PI / 180.0f;
    float pitchRadian = pitchAngle * M_PI / 180.0f;

    dirX = std::cos(pitchRadian) * std::cos(yawRadian);
    dirY = std::sin(pitchRadian);
    dirZ = std::cos(pitchRadian) * std::sin(yawRadian);
}

bool CheckCollision(float x, float z) {
    int j_cell = (int)(x / WALL_SIZE);
    int i_cell = (int)(z / WALL_SIZE);

    for (int i = i_cell - 1; i <= i_cell + 1; ++i) {
        for (int j = j_cell - 1; j <= j_cell + 1; ++j) {

            if (i < 0 || i >= MAZE_SIZE || j < 0 || j >= MAZE_SIZE) continue;

            if (maze_map[i][j] == 1) {

                float wallMinX = j * WALL_SIZE - WALL_SIZE / 2.0f;
                float wallMaxX = j * WALL_SIZE + WALL_SIZE / 2.0f;
                float wallMinZ = i * WALL_SIZE - WALL_SIZE / 2.0f;
                float wallMaxZ = i * WALL_SIZE + WALL_SIZE / 2.0f;

                float closestX = std::max(wallMinX, std::min(x, wallMaxX));
                float closestZ = std::max(wallMinZ, std::min(z, wallMaxZ));

                float dx = x - closestX;
                float dz = z - closestZ;
                float distanceSq = dx * dx + dz * dz;

                if (distanceSq < playerRadius * playerRadius) {
                    return true;
                }
            }
        }
    }

    if (i_cell < 0 || i_cell >= MAZE_SIZE || j_cell < 0 || j_cell >= MAZE_SIZE) {
        return true;
    }

    return false;
}

void CheckGameState() {
    int collectedCount = 0;
    bool allModelsCollected = true;

    for (size_t i = 0; i < models.size(); ++i) {
        ModelInstance& model = models[i];

        if (model.isVisible) {
            float dx = camX - model.x;
            float dy = camY - model.y;
            float dz = camZ - model.z;
            float distanceSq = dx * dx + dy * dy + dz * dz;

            if (distanceSq <= modelCollisionRadius * modelCollisionRadius) {
                model.isVisible = false;
                model.isCollected = true;
                std::cout << "DEBUG: Model " << i + 1 << " 획득! 남은 모델: " << models.size() - collectedCount - 1 << std::endl;
            }
        }

        if (!model.isCollected) {
            allModelsCollected = false;
        }
        else {
            collectedCount++;
        }
    }

    if (collectedCount == models.size() && !isGameOver) {
        float dx_exit = camX - EXIT_X;
        float dz_exit = camZ - EXIT_Z;
        float distanceSq_exit = dx_exit * dx_exit + dz_exit * dz_exit;

        if (distanceSq_exit <= EXIT_RADIUS_SQ) {
            isGameOver = true;
            std::cout << "INFO: 게임 승리! 모든 모델 획득 후 출구에 도착했습니다." << std::endl;
        }
    }
}

void Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w': case 'W': isWPressed = true; break;
    case 's': case 'S': isSPressed = true; break;
    case 'a': case 'A': isAPressed = true; break;
    case 'd': case 'D': isDPressed = true; break;
    case 27:
        std::cout << "INFO: ESC key pressed. Exiting game." << std::endl;
        exit(0);
        break;
    default: return;
    }

    glutPostRedisplay();
}

void KeyboardUp(unsigned char key, int x, int y) {
    switch (key) {
    case 'w': case 'W': isWPressed = false; break;
    case 's': case 'S': isSPressed = false; break;
    case 'a': case 'A': isAPressed = false; break;
    case 'd': case 'D': isDPressed = false; break;
    default: return;
    }
    glutPostRedisplay();
}

void MouseMotion(int x, int y) {
    if (isGameOver) return;

    int centerX = window_width / 2;
    int centerY = window_height / 2;

    int dx = x - centerX;
    int dy = y - centerY;

    if (dx != 0 || dy != 0) {
        angle += dx * mouseSensitivity;

        pitchAngle -= dy * mouseSensitivity;

        if (pitchAngle > 89.0f) pitchAngle = 89.0f;
        if (pitchAngle < -89.0f) pitchAngle = -89.0f;

        if (angle >= 360.0f) angle -= 360.0f;
        if (angle < 0.0f) angle += 360.0f;

        UpdateDirection();

        if (x != centerX || y != centerY) {
            glutWarpPointer(centerX, centerY);
        }

        glutPostRedisplay();
    }

}

void IdleFunc(void) {
    if (isGameOver) {
        glutPostRedisplay();
        return;
    }

    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    int deltaTimeMs = currentTime - lastTime;
    float deltaTime = deltaTimeMs / 1000.0f;
    lastTime = currentTime;

    float currentMoveDistance = moveSpeed * deltaTime;

    bool hasMoved = false;
    float nextCamX = camX;
    float nextCamZ = camZ;

    float strafeX = dirZ;
    float strafeZ = -dirX;

    if (isWPressed) { nextCamX += dirX * currentMoveDistance; nextCamZ += dirZ * currentMoveDistance; hasMoved = true; }
    if (isSPressed) { nextCamX -= dirX * currentMoveDistance; nextCamZ -= dirZ * currentMoveDistance; hasMoved = true; }
    if (isDPressed) { nextCamX -= strafeX * currentMoveDistance; nextCamZ -= strafeZ * currentMoveDistance; hasMoved = true; }
    if (isAPressed) { nextCamX += strafeX * currentMoveDistance; nextCamZ += strafeZ * currentMoveDistance; hasMoved = true; }

    if (hasMoved) {

        if (!CheckCollision(nextCamX, nextCamZ)) {
            camX = nextCamX;
            camZ = nextCamZ;
        }

        float ground_height = GetRampHeight(camX, camZ);
        camY = ground_height + 1.7f;

        CheckGameState();
        glutPostRedisplay();
    }
}

//메인함수
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Maze Navigation with Textures and SOR Model");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(Keyboard);
    glutKeyboardUpFunc(KeyboardUp);
    glutPassiveMotionFunc(MouseMotion);

    glutIdleFunc(IdleFunc);

    glutMainLoop();
    return 0;
}
