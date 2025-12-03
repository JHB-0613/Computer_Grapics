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
#include "stb_image.h"

float heights[TERRAIN_SIZE][TERRAIN_SIZE] = { 0.0f }; // 높이 데이터 배열
const float HEIGHT_SCALE = 1.0f; // 높이를 월드 좌표로 변환할 스케일 팩터
const float RAMP_START_X = WALL_SIZE * 0.5f; // 경사로 시작 X 
const float RAMP_START_Z = WALL_SIZE * 11.0f; // 경사로 시작 Z
const float RAMP_LENGTH_Z = WALL_SIZE * 5.0f; // Z축으로 2칸 길이
const float RAMP_WIDTH_X = WALL_SIZE; // X축 너비 
const float RAMP_HEIGHT = 3.0f; // 경사로 최대 높이

void GenerateSimpleHeightmap();
void RenderTerrain();


// 미로 맵 데이터 (1: 벽, 0: 통로)
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

// --- 네비게이션 변수 ---
float camX = WALL_SIZE * 1.0f; //  초기 시작 지점 X (1, 1 통로 중앙)
float camY = 1.7f; // 시점 높이
float camZ = WALL_SIZE * 0.0f; // 초기 시작 지점 Z (1, 1 통로 중앙)

float dirX = 0.0f;
float dirY = 0.0f;
float dirZ = -1.0f;

float angle = 90.0f;
float moveSpeed = 0.05f;
float rotateSpeed = 4.0f;
float mouseSensitivity = 0.05f; // 마우스 움직임에 따른 회전 감도 (조절 가능)
int lastMouseX; // 마우스 커서의 마지막 X 좌표 저장
float pitchAngle = 0.0f; //  수직 시점 각도

// --- 텍스처 및 HUD 변수 ---
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

// --- SOR 모델링 데이터 구조 및 변수 추가 ---
// 3D Point (정점) 구조체 정의
class xPoint3D {
public:
    float x, y, z, w;
    xPoint3D() { x = y = z = 0; w = 1; };
};

// 삼각형 면(Face) 구조체 정의
struct MyFace {
    int ip[3]; // 정점 3개의 인덱스
};

// 렌더링될 실제 모델 데이터를 담는 구조체
struct ModelData {
    std::vector<xPoint3D> vertices;
    std::vector<MyFace> faces;
};
// 모든 모델 데이터를 저장하는 벡터
std::vector<ModelData> loadedModels;

// 미로에 배치될 모델 인스턴스 구조체
struct ModelInstance {
    float x;
    float y;
    float z;
    bool isVisible;
    bool isCollected;
    int dataIndex; // 어떤 모델 데이터를 사용할지 지정
    GLuint textureID;
    float scaleX;
    float scaleY;
    float scaleZ;
};

// 배치될 모든 모델 인스턴스
std::vector<ModelInstance> models; // <--- 이제 전역 영역에서 확실하게 선언됨

// *******************************************************************
//  텍스처 로딩 함수
// *******************************************************************
GLuint LoadTexture(const char* filename) {
    GLuint textureID = 0;
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

    if (data) {
        // 1. OpenGL 텍스처 객체 생성
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // 2. 텍스처 필터링 및 래핑 설정
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 3. 이미지 포맷 결정 및 OpenGL에 데이터 전송
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

        // 로드된 이미지 데이터 메모리 해제
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

// ----------------------------------------------------
// 1. 렌더링 함수
// ----------------------------------------------------

// 플레이어 위치(x, z)에서 지형의 높이를 얻는 함수
float GetTerrainHeight(float x, float z) {
    // 플레이어의 현재 월드 좌표를 미로 셀 인덱스로 변환
    int j_cell = (int)(x / WALL_SIZE);
    int i_cell = (int)(z / WALL_SIZE);

    // 배열 경계 체크
    if (i_cell < 0 || i_cell >= TERRAIN_SIZE || j_cell < 0 || j_cell >= TERRAIN_SIZE) {
        return 0.0f; // 경계 밖이면 기본 높이 0 반환
    }

    return heights[i_cell][j_cell];
}

// 경사로를 렌더링하는 함수 (삼각형 스트립 사용)
void RenderRamp() {
    glEnable(GL_TEXTURE_2D);
    // 바닥 텍스처를 경사로에도 사용
    glBindTexture(GL_TEXTURE_2D, floor_texture_id);
    glColor3f(1.0f, 1.0f, 1.0f);

    const int SEGMENTS = 50; // Z축을 50개 세그먼트로 분할하여 부드럽게 표현
    float dz = RAMP_LENGTH_Z / (float)SEGMENTS;
    float dx = RAMP_WIDTH_X;
    float ramp_half_width = RAMP_WIDTH_X / 2.0f;
    float texture_repeat_z = RAMP_LENGTH_Z / WALL_SIZE; // 텍스처 반복 횟수

    // 경사로의 중앙 X 좌표
    float centerX = RAMP_START_X + WALL_SIZE / 2.0f;

    for (int i = 0; i < SEGMENTS; ++i) {
        glBegin(GL_QUAD_STRIP);

        for (int j = 0; j <= 1; ++j) {

            // 0: 현재 세그먼트, 1: 다음 세그먼트
            float z_offset_curr = (float)i * dz;
            float z_offset_next = (float)(i + 1) * dz;

            // 정규화된 Z 위치 (0.0 ~ 1.0)
            float normalized_z_curr = z_offset_curr / RAMP_LENGTH_Z;
            float normalized_z_next = z_offset_next / RAMP_LENGTH_Z;

            // Z 위치에 따른 Y 높이 계산 (COS 함수를 사용하여 부드러운 곡선 생성)
            // 0.0에서 시작 -> 최대 높이 -> 0.0으로 끝남
            float angle_curr = normalized_z_curr * 2.0f * M_PI; // 0 ~ 2*PI
            float angle_next = normalized_z_next * 2.0f * M_PI; // 0 ~ 2*PI

            // y = RAMP_HEIGHT * (1 - cos(angle)) / 2  (0에서 RAMP_HEIGHT까지)
            float y_curr = RAMP_HEIGHT * (1.0f - std::cos(angle_curr)) / 2.0f;
            float y_next = RAMP_HEIGHT * (1.0f - std::cos(angle_next)) / 2.0f;


            // 1. 현재 세그먼트의 왼쪽/오른쪽 정점
            // (X, Y, Z) = (centerX -/+ half_width, y_curr, RAMP_START_Z + z_offset_curr)
            float x_left = centerX - ramp_half_width;
            float x_right = centerX + ramp_half_width;

            // Z 좌표가 끝났을 때 렌더링을 멈춤
            if (i < SEGMENTS) {
                // 왼쪽 정점 (현재 Z)
                glTexCoord2f(0.0f, normalized_z_curr * texture_repeat_z);
                glVertex3f(x_left, y_curr, RAMP_START_Z + z_offset_curr);

                // 오른쪽 정점 (현재 Z)
                glTexCoord2f(1.0f, normalized_z_curr * texture_repeat_z);
                glVertex3f(x_right, y_curr, RAMP_START_Z + z_offset_curr);

                // 다음 세그먼트의 왼쪽/오른쪽 정점
                // 왼쪽 정점 (다음 Z)
                glTexCoord2f(0.0f, normalized_z_next * texture_repeat_z);
                glVertex3f(x_left, y_next, RAMP_START_Z + z_offset_next);

                // 오른쪽 정점 (다음 Z)
                glTexCoord2f(1.0f, normalized_z_next * texture_repeat_z);
                glVertex3f(x_right, y_next, RAMP_START_Z + z_offset_next);
            }
        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

// 텍스처가 적용된 단일 벽 (큐브)을 그리는 함수
void DrawTexturedWall(float size) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wall_texture_id);
    glColor3f(1.0f, 1.0f, 1.0f); // 텍스처의 색상을 유지하기 위해 흰색 설정

    float s2 = size / 2.0f;
    float repeat = 1.0f;

    glBegin(GL_QUADS);
    // 정면 (Z = +s2)
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s2, -s2, s2);
    glTexCoord2f(repeat, 0.0f); glVertex3f(s2, -s2, s2);
    glTexCoord2f(repeat, repeat); glVertex3f(s2, s2, s2);
    glTexCoord2f(0.0f, repeat); glVertex3f(-s2, s2, s2);

    // 후면 (Z = -s2)
    glTexCoord2f(repeat, 0.0f); glVertex3f(-s2, -s2, -s2);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(s2, -s2, -s2);
    glTexCoord2f(0.0f, repeat); glVertex3f(s2, s2, -s2);
    glTexCoord2f(repeat, repeat); glVertex3f(-s2, s2, -s2);

    // 우측면 (X = +s2)
    glTexCoord2f(0.0f, 0.0f); glVertex3f(s2, -s2, s2);
    glTexCoord2f(repeat, 0.0f); glVertex3f(s2, -s2, -s2);
    glTexCoord2f(repeat, repeat); glVertex3f(s2, s2, -s2);
    glTexCoord2f(0.0f, repeat); glVertex3f(s2, s2, s2);

    // 좌측면 (X = -s2)
    glTexCoord2f(repeat, 0.0f); glVertex3f(-s2, -s2, s2);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s2, -s2, -s2);
    glTexCoord2f(0.0f, repeat); glVertex3f(-s2, s2, -s2);
    glTexCoord2f(repeat, repeat); glVertex3f(-s2, s2, s2);

    // 윗면 (Y = +s2), 아랫면 (Y = -s2)도 필요에 따라 구현
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void GenerateSimpleHeightmap() {
    for (int i = 0; i < TERRAIN_SIZE; ++i) {
        for (int j = 0; j < TERRAIN_SIZE; ++j) {
            // 미로 맵 데이터 (1: 벽, 0: 통로)를 그대로 높이로 사용
            heights[i][j] = maze_map[i][j] * HEIGHT_SCALE; // 벽은 1.0f 높이, 통로는 0.0f 높이

            // 높이 변화를 좀 더 부드럽게 만들기 위해,
            // 주변 통로와 벽의 경계에 있는 지점의 높이를 조정할 수 있습니다.
            // 여기서는 단순하게 미로 맵 데이터만 사용합니다.
        }
    }
    std::cout << "DEBUG: Simple heightmap generated." << std::endl;
}

// 높이 맵 기반 지형 렌더링 함수
void RenderTerrain() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, floor_texture_id); // 바닥 텍스처 사용
    glColor3f(1.0f, 1.0f, 1.0f);

    float scale = WALL_SIZE;
    float texture_repeat = TERRAIN_SIZE;

    // 지형의 각 셀을 삼각형 스트립으로 그립니다.
    for (int i = 0; i < TERRAIN_SIZE - 1; ++i) {
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j < TERRAIN_SIZE; ++j) {

            // 1. (i, j) 정점
            float x1 = j * scale;
            float z1 = i * scale;
            float y1 = heights[i][j];

            glTexCoord2f((float)j / TERRAIN_SIZE * texture_repeat, (float)i / TERRAIN_SIZE * texture_repeat);
            glVertex3f(x1, y1, z1);

            // 2. (i+1, j) 정점
            float x2 = j * scale;
            float z2 = (i + 1) * scale;
            float y2 = heights[i + 1][j];

            glTexCoord2f((float)j / TERRAIN_SIZE * texture_repeat, (float)(i + 1) / TERRAIN_SIZE * texture_repeat);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

// 미로 전체 렌더링 함수
void RenderMaze() {
    // 1. 벽 렌더링
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

    // --- 2. 바닥 렌더링 (평평한 바닥을 다시 추가) ---
    // 플레이어의 Y 위치를 0.0f 기준으로 고정
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, floor_texture_id);
    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
    // 바닥의 중심을 미로 중앙으로 이동
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


    // --- 3. 천장 렌더링 (기존 코드 유지) ---
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ceiling_texture_id);
    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
    // 천장의 중심을 미로 중앙으로 이동시키고, 높이를 WALL_SIZE로 설정
    glTranslatef((MAZE_SIZE - 1) * WALL_SIZE / 2.0f, WALL_SIZE, (MAZE_SIZE - 1) * WALL_SIZE / 2.0f);

    glBegin(GL_QUADS);
    // Z축과 X축이 미로의 범위에 맞도록 텍스처 좌표 설정
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

// SOR 모델 렌더링 함수
void RenderModel(int dataIndex, GLuint textureID) {
    if (dataIndex < 0 || dataIndex >= loadedModels.size()) return;
    const ModelData& modelData = loadedModels[dataIndex];

    if (modelData.faces.empty()) return;

    // 인자로 받은 텍스처 ID를 바인딩합니다.
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glColor3f(1.0f, 1.0f, 1.0f);

    // 모델의 정점 범위 정의
    const float Y_MIN = 0.0f;
    const float Y_MAX = 155.0f;

    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < modelData.faces.size(); ++i) {
        const MyFace& face = modelData.faces[i];

        for (int j = 0; j < 3; ++j) {
            const xPoint3D& vertex = modelData.vertices[face.ip[j]];

            // 텍스처 좌표 계산 (X-Z 기반 구형 매핑)
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

// 여러 모델의 위치를 초기화하는 함수
void InitModels(int barrelIdx, int drumIdx, int boxIdx, int chestIdx, GLuint barrelTexID, GLuint drumTexID, GLuint boxTexID, GLuint chestTexID) {
    // 💡 미로 통로의 중앙 좌표를 기준으로 모델을 배치합니다 (X/Z는 *.5f * WALL_SIZE)
    models.clear();

    // 배럴 모델
    models.push_back({
        WALL_SIZE * 1.0f, 0.0f, WALL_SIZE * 3.0f, true, false,
        barrelIdx, barrelTexID,
        0.03f, 0.02f, 0.03f
        });

    // 드럼 모델
    models.push_back({
        WALL_SIZE * 15.0f, 0.0f, WALL_SIZE * 1.0f, true, false,
        drumIdx, drumTexID,
        0.03f, 0.02f, 0.03f
        });

    // 상자 모델
    models.push_back({
        WALL_SIZE * 9.0f, 0.75f, WALL_SIZE * 7.0f, true, false,
        boxIdx, boxTexID,
        1.5f, 1.5f, 1.5f
        });
    // 보물상자모델
    models.push_back({
    WALL_SIZE * 17.0f, 0.0f, WALL_SIZE * 11.0f, true, false,
    chestIdx, chestTexID,
    0.05f, 0.05f, 0.05f
        });

    std::cout << "DEBUG: " << models.size() << " models initialized." << std::endl;
}

// 화면 하단 HUD 이미지 렌더링 함수
void RenderImageOnBottom() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, image_texture_id);
    glColor3f(1.0f, 1.0f, 1.0f); // 텍스처 색상 사용

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
//승리 이미지 렌더링 함수
void RenderWinScreen() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, win_texture_id);
    glColor3f(1.0f, 1.0f, 1.0f);

    // 화면 전체를 덮는 사각형을 그림
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(window_width, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(window_width, window_height);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, window_height);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
// 텍스처가 적용된 박스를 그리는 함수
void DrawTexturedCube(float size, GLuint textureID) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glColor3f(1.0f, 1.0f, 1.0f);

    float s2 = size / 2.0f;
    float repeat = 1.0f; // 한 면에 텍스처가 한 번만 나타나도록 설정

    glBegin(GL_QUADS);

    // 정면 (Z = +s2)
    // 텍스처 좌표 (0,0) (1,0) (1,1) (0,1)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s2, -s2, s2);
    glTexCoord2f(repeat, 0.0f); glVertex3f(s2, -s2, s2);
    glTexCoord2f(repeat, repeat); glVertex3f(s2, s2, s2);
    glTexCoord2f(0.0f, repeat); glVertex3f(-s2, s2, s2);

    // 후면 (Z = -s2)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(repeat, 0.0f); glVertex3f(-s2, -s2, -s2);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(s2, -s2, -s2);
    glTexCoord2f(0.0f, repeat); glVertex3f(s2, s2, -s2);
    glTexCoord2f(repeat, repeat); glVertex3f(-s2, s2, -s2);

    // 우측면 (X = +s2)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(s2, -s2, s2);
    glTexCoord2f(repeat, 0.0f); glVertex3f(s2, -s2, -s2);
    glTexCoord2f(repeat, repeat); glVertex3f(s2, s2, -s2);
    glTexCoord2f(0.0f, repeat); glVertex3f(s2, s2, s2);

    // 좌측면 (X = -s2)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(repeat, 0.0f); glVertex3f(-s2, -s2, s2);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s2, -s2, -s2);
    glTexCoord2f(0.0f, repeat); glVertex3f(-s2, s2, -s2);
    glTexCoord2f(repeat, repeat); glVertex3f(-s2, s2, s2);

    // 윗면 (Y = +s2)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, repeat); glVertex3f(-s2, s2, -s2);
    glTexCoord2f(repeat, repeat); glVertex3f(s2, s2, -s2);
    glTexCoord2f(repeat, 0.0f); glVertex3f(s2, s2, s2);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s2, s2, s2);

    // 아랫면 (Y = -s2)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s2, -s2, -s2);
    glTexCoord2f(repeat, 0.0f); glVertex3f(s2, -s2, -s2);
    glTexCoord2f(repeat, repeat); glVertex3f(s2, -s2, s2);
    glTexCoord2f(0.0f, repeat); glVertex3f(-s2, -s2, s2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

// --- 충돌 감지 및 모델 상태 변수 추가 ---
bool isModelVisible = true; // 모델의 표시 여부
float modelCollisionRadius = 3.0f; // 모델의 충돌 감지 반경
float playerRadius = 0.2f; // 플레이어 충돌 반경
// 모델의 월드 좌표를 저장
bool hasWonModel = false; //총 획득 개수
bool isGameOver = false;  // 게임 종료(승리) 상태 여부

// 미로 출구 좌표 (예: i=4, j=9 통로 중앙)
const float EXIT_X = WALL_SIZE * 17.0f;
const float EXIT_Z = WALL_SIZE * 19.0f;
const float EXIT_RADIUS_SQ = 1.0f * 1.0f; // 출구 감지 반경 제곱 (1.0f)

// ----------------------------------------------------
// 2. 네비게이션 및 충돌 체크
// ----------------------------------------------------

// 카메라 방향 벡터 업데이트
void UpdateDirection() {
    float yawRadian = angle * M_PI / 180.0f;
    float pitchRadian = pitchAngle * M_PI / 180.0f;

    // 방향 벡터 계산
    dirX = std::cos(pitchRadian) * std::cos(yawRadian);
    dirY = std::sin(pitchRadian);
    dirZ = std::cos(pitchRadian) * std::sin(yawRadian);
}

// 충돌 체크 함수
bool CheckCollision(float x, float z) {
    // 1. 플레이어의 현재 위치가 속한 미로 셀 (i_cell, j_cell)을 찾음
    int j_cell = (int)(x / WALL_SIZE);
    int i_cell = (int)(z / WALL_SIZE);

    // 2. 주변 9개 셀을 반복하며 벽과의 충돌을 검사
    for (int i = i_cell - 1; i <= i_cell + 1; ++i) {
        for (int j = j_cell - 1; j <= j_cell + 1; ++j) {

            // 미로 경계 밖이면 무시
            if (i < 0 || i >= MAZE_SIZE || j < 0 || j >= MAZE_SIZE) continue;

            // 해당 셀이 벽인 경우
            if (maze_map[i][j] == 1) {

                // 벽 셀의 경계 (Min/Max World Coordinates)
                float wallMinX = j * WALL_SIZE - WALL_SIZE / 2.0f;
                float wallMaxX = j * WALL_SIZE + WALL_SIZE / 2.0f;
                float wallMinZ = i * WALL_SIZE - WALL_SIZE / 2.0f;
                float wallMaxZ = i * WALL_SIZE + WALL_SIZE / 2.0f;

                // 3. 벽 AABB에 가장 가까운 플레이어의 X, Z 좌표를 찾음 (Closest Point)
                // x, z 좌표가 wallMin/Max 범위를 벗어날 경우, 가장 가까운 경계값으로 클램핑(Clamping)
                float closestX = std::max(wallMinX, std::min(x, wallMaxX));
                float closestZ = std::max(wallMinZ, std::min(z, wallMaxZ));

                // 4. 플레이어 중심과 가장 가까운 점 사이의 거리 계산
                float dx = x - closestX;
                float dz = z - closestZ;
                float distanceSq = dx * dx + dz * dz;

                // 5. 거리가 플레이어 반경보다 작으면 충돌
                if (distanceSq < playerRadius * playerRadius) {
                    return true; // 충돌
                }
            }
        }
    }

    // 미로의 가장자리(바깥 경계) 충돌 처리
    if (i_cell < 0 || i_cell >= MAZE_SIZE || j_cell < 0 || j_cell >= MAZE_SIZE) {
        return true;
    }

    return false;
}

void CheckGameState() {
    int collectedCount = 0;
    bool allModelsCollected = true; // 모든 모델 획득 플래그

    // **[수정]** 1. SOR 모델 획득 체크 (벡터 순회)
    for (size_t i = 0; i < models.size(); ++i) {
        ModelInstance& model = models[i]; // 벡터의 요소를 참조로 받아 수정 가능하게 함

        if (model.isVisible) {
            float dx = camX - model.x;
            float dy = camY - model.y;
            float dz = camZ - model.z;
            float distanceSq = dx * dx + dy * dy + dz * dz;

            if (distanceSq <= modelCollisionRadius * modelCollisionRadius) {
                // 모델 획득
                model.isVisible = false;
                model.isCollected = true;
                std::cout << "DEBUG: Model " << i + 1 << " 획득! 남은 모델: " << models.size() - collectedCount - 1 << std::endl;
            }
        }

        if (!model.isCollected) {
            allModelsCollected = false; // 하나라도 획득하지 못했으면 false
        }
        else {
            collectedCount++;
        }
    }

    // 2. 승리 조건 체크: 모든 모델을 획득했고, 출구 근처에 도달했는지 확인
    if (collectedCount == models.size() && !isGameOver) { // 모든 모델 획득 상태 확인
        float dx_exit = camX - EXIT_X;
        float dz_exit = camZ - EXIT_Z;
        float distanceSq_exit = dx_exit * dx_exit + dz_exit * dz_exit;

        if (distanceSq_exit <= EXIT_RADIUS_SQ) {
            // 최종 승리!
            isGameOver = true;
            std::cout << "INFO: 게임 승리! 모든 모델 획득 후 출구에 도착했습니다." << std::endl;
        }
    }
}

// 키 상태를 추적할 플래그
bool isWPressed = false;
bool isSPressed = false;
bool isAPressed = false;
bool isDPressed = false;

// 키보드 이벤트 처리 함수
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

//마우스 움직임 이벤트 처리 함수
void MouseMotion(int x, int y) {
    if (isGameOver) return;

    // 1. 윈도우 중앙 좌표 계산
    int centerX = window_width / 2;
    int centerY = window_height / 2;

    // 2. 마우스와 중앙의 거리 차이 계산
    int dx = x - centerX; // X축 이동
    int dy = y - centerY; // Y축 이동 

    // 3. 각도 업데이트
    if (dx != 0 || dy != 0) { // X 또는 Y 변화가 있을 경우에만 처리
        // A. 수평 각도 (Yaw) 업데이트
        angle += dx * mouseSensitivity;

        // B. 수직 각도 (Pitch) 업데이트
        // 마우스를 아래로 내리면(y 증가) 시점은 아래로 내려가므로 dy에 마이너스(-)를 곱하여 시점 방향을 맞춤
        pitchAngle -= dy * mouseSensitivity;

        // **[추가]** 수직 각도 제한 (시야가 뒤집히는 것을 방지: -89도 ~ 89도)
        if (pitchAngle > 89.0f) pitchAngle = 89.0f;
        if (pitchAngle < -89.0f) pitchAngle = -89.0f;

        // 각도 범위 제한 및 방향 벡터 업데이트
        if (angle >= 360.0f) angle -= 360.0f;
        if (angle < 0.0f) angle += 360.0f;

        UpdateDirection(); // Yaw와 Pitch를 모두 반영하여 dirX, dirY, dirZ 업데이트

        // 4. 마우스 커서 재배치 (커서를 윈도우 중앙으로 고정)
        if (x != centerX || y != centerY) {
            glutWarpPointer(centerX, centerY);
        }

        glutPostRedisplay();
    }

}

// --- SOR 모델링 데이터 로드 함수 추가 ---
int LoadSORModel(const char* filename) {
    ModelData newModel;
    std::ifstream fin(filename);

    if (!fin.is_open()) {
        std::cerr << "ERROR: Failed to open SOR model file: " << filename << std::endl;
        return -1; // 로드 실패
    }

    // --- 1. 정점(VERTEX) 데이터 읽기 ---
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

    // --- 2. 면(FACE) 데이터 읽기 ---
    if (fin >> tag && tag == "FACE" && fin >> equal_sign && equal_sign == '=' && fin >> count) {
        newModel.faces.reserve(count);
        for (int i = 0; i < count; ++i) {
            MyFace face;
            fin >> face.ip[0] >> face.ip[1] >> face.ip[2];
            // 인덱스 1부터 시작 시 조정 필요 (현재 파일은 0부터 시작하는 것으로 가정)
            newModel.faces.push_back(face);
        }
    }

    fin.close();
    std::cout << "DEBUG: Loaded " << filename << ". Vertices: " << newModel.vertices.size() << ", Faces: " << newModel.faces.size() << std::endl;

    // 로드된 데이터를 전역 벡터에 추가하고 인덱스 반환
    loadedModels.push_back(newModel);
    return loadedModels.size() - 1;
}


// ----------------------------------------------------
// 3. 메인 OpenGL 루프
// ----------------------------------------------------
void RenderBitmapString(float x, float y, void* font, const char* string) {
    glColor3f(91.0f, 247.0f, 48.0f);
    glRasterPos2f(x, y);
    while (*string) {
        glutBitmapCharacter(font, *string++);
    }
}
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1. 3D 렌더링 설정
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(
        camX, camY, camZ,
        camX + dirX, camY + dirY, camZ + dirZ,
        0.0f, 1.0f, 0.0f
    );

    RenderMaze(); // 미로 (3D) 렌더링



    // SOR 객체 배치
    for (const auto& model : models) {
        if (model.isVisible) {
            glPushMatrix();

            // 각 모델의 고유 위치로 이동
            glTranslatef(model.x, model.y, model.z);

            // 모델 인스턴스에 설정된 개별 스케일 적용
            if (model.dataIndex == -1) {
                DrawTexturedCube(model.scaleX, model.textureID);
            }
            else {
                // SOR 모델인 경우 (배럴, 드럼), 기존 RenderModel 사용
                glScalef(model.scaleX, model.scaleY, model.scaleZ);
                RenderModel(model.dataIndex, model.textureID);
            }
            glPopMatrix();
        }
    }


    // 2. 2D HUD 렌더링 시작
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
        RenderImageOnBottom(); // 평상시 HUD 렌더링

        // --- 획득해야 할 모델 개수 계산 및 표시 ---
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

    // 3. 3D 렌더링 환경 복귀
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
}

void reshape(int w, int h) {
    window_width = w; // 윈도우 크기 업데이트
    window_height = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60, (GLfloat)w / (GLfloat)h, 0.1, 500.0);
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

    GenerateSimpleHeightmap();

    UpdateDirection();

    // 모델 인덱스와 텍스처 ID를 InitModels에 모두 전달
    InitModels(barrelModelIndex, DrumModelIndex, boxModelIndex, chestModelIndex, barrel_tex_id, drum_tex_id, box_tex_id, chest_tex_id);
}

float GetRampHeight(float x, float z) {

    // 경사로의 X 경계
    float minX = RAMP_START_X;
    float maxX = RAMP_START_X + RAMP_WIDTH_X;

    // 경사로의 Z 경계
    float minZ = RAMP_START_Z;
    float maxZ = RAMP_START_Z + RAMP_LENGTH_Z;

    // 1. 경사로 영역 안에 있는지 확인
    if (x >= minX && x <= maxX && z >= minZ && z <= maxZ) {
        // 2. Z축 위치를 정규화 (0.0 ~ 1.0)
        float normalized_z = (z - RAMP_START_Z) / RAMP_LENGTH_Z;

        // 3. Y 높이 계산 (RenderRamp와 동일한 공식 사용)
        float angle_rad = normalized_z * 2.0f * M_PI;
        float y_height = RAMP_HEIGHT * (1.0f - std::cos(angle_rad)) / 2.0f;

        return y_height;
    }

    // 경사로 영역 밖이라면 기본 바닥 높이인 0.0f 반환
    return 0.0f;
}

void IdleFunc(void) {
    if (isGameOver) {
        glutPostRedisplay();
        return;
    }

    bool hasMoved = false;
    float nextCamX = camX;
    float nextCamZ = camZ;
    float prevCamX = camX;
    float prevCamZ = camZ;

    float currentMoveSpeed = moveSpeed;
    float strafeX = dirZ;
    float strafeZ = -dirX;

    // ... (키 상태에 따른 nextCamX/Z 계산 로직 유지) ...
    // 이 부분에서 hasMoved가 true로 설정되었다고 가정합니다.

    // 키 상태 확인 및 이동 벡터 계산 (예시)
    if (isWPressed) { nextCamX += dirX * currentMoveSpeed; nextCamZ += dirZ * currentMoveSpeed; hasMoved = true; }
    if (isSPressed) { nextCamX -= dirX * currentMoveSpeed; nextCamZ -= dirZ * currentMoveSpeed; hasMoved = true; }
    if (isDPressed) { nextCamX -= strafeX * currentMoveSpeed; nextCamZ -= strafeZ * currentMoveSpeed; hasMoved = true; }
    if (isAPressed) { nextCamX += strafeX * currentMoveSpeed; nextCamZ += strafeZ * currentMoveSpeed; hasMoved = true; }

    // 2. 충돌 체크 및 위치 업데이트
    if (hasMoved) {
        float movedSuccessfully = false; // **[추가]** 실제 이동 성공 여부
        if (!CheckCollision(nextCamX, nextCamZ)) {
            camX = nextCamX;
            camZ = nextCamZ;
            movedSuccessfully = true; // 이동 성공
        }

        // 카메라 Y 높이 업데이트
        float ground_height = GetRampHeight(camX, camZ);
        camY = ground_height + 1.7f;

        // 이동에 성공했을 때만 속도 값을 기록합니다.
        if (hasMoved) {

            CheckGameState();
        }

        // 3. 화면 갱신 요청
        glutPostRedisplay();
    }
}

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