#include "include/glad/glad.h"

#include <GLFW/glfw3.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#include "include/camera.h"
#define MODEL_IMPLEMENTATION
#include "include/model.h"
#define SHADER_IMPLEMENTATION
#include "include/shader_s.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 720

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
unsigned int loadTexture(char const * path);
unsigned int create_vao_vbo(Vertices vertices, Indices indices);

Camera camera;
bool firstMouse = true;
float lastX = 600;
float lastY = 360;
float deltaTime = 0.0;
float lastFrame = 0.0;

int main(void) {
    Camera_init(&camera, (vec3) {0.0, 0.0, -3.0}, (vec3) {0.0, 1.0, 0.0}, 90.0, 0.0);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Template", NULL, NULL);

    if (window == NULL){
        printf("failed to open window\n");
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    
    Shader shader = {
        "./resources/shaders/vertex.glsl",
        "./resources/shaders/fragment.glsl",
    };
    ShaderInit(&shader);

    char *obj = "resources/default_cube.obj";
    Vertices obj_vertices;
    Indices obj_indices;
    Textures obj_tex;
    parse_obj(obj, &obj_vertices, &obj_indices, &obj_tex);

    unsigned int modelVAO = create_vao_vbo(obj_vertices, obj_indices);
    unsigned int container = loadTexture("resources/container.jpg");
    vec3 lightPos = {2, 4, 0};
    mat4 projection = GLM_MAT4_IDENTITY;
    mat4 view = GLM_MAT4_IDENTITY;
    mat4 model = GLM_MAT4_IDENTITY;

    glm_perspective(camera.Zoom, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
                    0.1f, 10000.f, projection);


    ShaderUse(shader);
    glUniform3f(glGetUniformLocation(shader.ID, "light.ambient"), 0.2, 0.2, 0.2);
    glUniform3f(glGetUniformLocation(shader.ID, "light.position"), lightPos[0], lightPos[1], lightPos[2]);
    glUniform3f(glGetUniformLocation(shader.ID, "light.diffuse"), 0.9, 0.9, 0.9);
    glUniform3f(glGetUniformLocation(shader.ID, "light.specular"), 1.0, 1.0, 1.0);
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1,
                       GL_FALSE, &projection[0][0]);

    while (!glfwWindowShouldClose(window)){
        // input
        float currentFrame = (float) glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        // clear background
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, container);

        ShaderUse(shader);
        GetViewMatrix(&camera, view);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE,
                           &view[0][0]);
        glBindVertexArray(modelVAO);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE,
                     &model[0][0]);
        for (size_t i = 0; i < obj_tex.count; i++) {
            glUniform3f(glGetUniformLocation(shader.ID, "material.diffuse"), obj_tex.data[i].diffuse[0], obj_tex.data[i].diffuse[1], obj_tex.data[i].diffuse[2]);
            glUniform3f(glGetUniformLocation(shader.ID, "material.specular"), obj_tex.data[i].specular[0], obj_tex.data[i].specular[1], obj_tex.data[i].specular[2]);
            glUniform1f(glGetUniformLocation(shader.ID, "material.shininess"), obj_tex.data[i].shininess);
        }
        glDrawElements(GL_TRIANGLES, obj_indices.size, GL_UNSIGNED_INT, 0);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}


void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}


void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    ProcessKeyboard(&camera, FORWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    ProcessKeyboard(&camera, BACKWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    ProcessKeyboard(&camera, LEFT, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    ProcessKeyboard(&camera, RIGHT, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    ProcessKeyboard(&camera, UP, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    ProcessKeyboard(&camera, DOWN, deltaTime);
  }
}


void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    float xpos = (float) xposIn;
    float ypos = (float) yposIn;
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;
  ProcessMouseMovement(&camera, xoffset, yoffset, true);
}


unsigned int create_vao_vbo(Vertices vertices, Indices indices)
{
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO); // Generate the Element Buffer Object

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size * sizeof(Vertex), vertices.data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size * sizeof(unsigned int), indices.data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Vertices));
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Note: EBO binding is part of VAO state
    return VAO;
}



unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = 0;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        printf("Texture failed to load at path: %s\n", path);
        stbi_image_free(data);
    }

    return textureID;
}
