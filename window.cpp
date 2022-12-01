#include "window.h"
#include <iostream>

using namespace std;
using namespace glm;

void makeWindow(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Gráfica - Proyecto 3", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    glEnable(GL_DEPTH_TEST);

    Shader lightingShader("../shader/2.1.point.vs", "../shader/2.1.point.fs", "../shader/2.1.point.gs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f
    };
    vector<vec3> puntos, normales;

    auto tnl = Tunnel(glm::vec3(3,0,3), glm::sphericalRand(0.1F), true);
    Tunnel* tnls = &tnl;
    printf("Path size: %ld\n", tnl.main_path.size());
    for(auto road: tnl.main_path){
        for (float d = 0; d < road.dist; d += DIST_INTER) {
            auto point = road.start + d * road.dir;;
            auto intersect_with_seed = glm::normalize(glm::cross(seed, road.dir));
            auto p = glm::normalize(glm::cross(intersect_with_seed, road.dir));
            for (float angle = 0.0F; angle < 360.F; angle += DEG_INTER){
                auto new_start = road.radious * cos(degreeToRad(angle)) * intersect_with_seed;
                auto new_dir = road.radious * sin(degreeToRad(angle)) * p;
                auto new_point = point + new_start + new_dir;
                puntos.push_back(new_point);
                auto test = vec3(1, 1, 1);
                auto surface = normalize(test);
                normales.emplace_back(surface);
            }
        }
    }

    // First, configure the cube's VAO (and VBO)
    unsigned int VBO[2], cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(2, VBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, puntos.size() * sizeof(vec3), puntos.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, normales.size() * sizeof(vec3), normales.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);
    glPointSize(2);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();

        // View/Projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // World transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        // Render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_POINTS, 0, puntos.size());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(2, VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

float degreeToRad(float degree){
    float pi = pi = 3.14159265359;
    return degree * (pi/180);
}

int main(){
    makeWindow();
    return 0;
}