#ifndef UI_H
#define UI_H

#include "camera.h"
#include "shader/shader.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>

// Window settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int DIST_INTER = 1;
const unsigned int DEG_INTER = 2;

// Tunnel features
#define MAX_MAIN_DEPTH 5
#define MIN_DISTANCE 10.0F
#define MAX_DISTANCE 40.0F
#define MIN_RADIOUS 2.0F
#define MAX_RADIOUS 3.0F

Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::vec3 seed{0.f,1.f,0.f};

float deltaTime = 0.0f;	
float lastFrame = 0.0f;
bool firstMouse = true;
int lastX, lastY;

void makeWindow();
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

float degreeToRad(float degree);

// Tunnel

struct Road{
    glm::vec3 start;
    glm::vec3 dir;
    float dist;
    float radious;

    Road(glm::vec3 start_, glm::vec3 dir_, float dist_, float radious_ = 3):
        start(start_), dir(glm::normalize(dir_)), dist(dist_), radious(radious_ ) {}
};

struct Tunnel {
    std::vector<Road> main_path {};
    Tunnel* sec_path;
    glm::vec3 dir;

    Tunnel(glm::vec3 start, glm::vec3 dir = glm::sphericalRand(1.0f), bool make_sec = false){
        printf("Main path (%0.3f, %0.3f %0.3f)\n", dir.x, dir.y, dir.z);
        int depth = 0;

        // Camino principal
        while(depth++ < MAX_MAIN_DEPTH){
            float dist = glm::linearRand(MIN_DISTANCE, MAX_DISTANCE);
            float r = glm::linearRand(MIN_RADIOUS, MAX_RADIOUS);

            Road road {start, dir, dist, r};

            main_path.push_back(road);
            start += dir * dist;
            dir = glm::normalize(glm::cross(start + 2*r, start + glm::linearRand(-0.25F,0.25F) * dir));
        }
    }
};

#endif