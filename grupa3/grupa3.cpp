﻿// Nagłówki
//#include "stdafx.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/System/Time.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(disable:4996)

//#include <Mouse.hpp>
using namespace std;
using namespace sf;
// Kody shaderów
const GLchar* vertexSource = R"glsl(
#version 150 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

in vec3 position;
in vec3 color;
in vec2 aTexCoord;
in vec3 aNormal;

out vec2 TexCoord;
out vec3 Color;
out vec3 Normal;
out vec3 FragPos;


void main(){
gl_Position = proj * view * model * vec4(position, 1.0);
Color = vec3(1.0, 1.0, 1.0);
TexCoord = aTexCoord;
// ####################################################################
// LAB_9
Normal = aNormal;
// ####################################################################
}
)glsl";

const GLchar* fragmentSource = R"glsl(
#version 150 core
in vec3 Color;
out vec4 outColor;
in vec2 TexCoord; //do tesktur
uniform sampler2D texture1; //do tesktur
// ####################################################################
// LAB_9
in vec3 Normal;
in vec3 FragPos;
in vec3 lightPos;
float ambientStrength = 0.5;
// ####################################################################
void main()
{
// ####################################################################
// LAB_9
vec3 ambientlightColor = vec3(1.0,1.0,1.0);
vec4 ambient = ambientStrength * vec4(ambientlightColor,1.0);
vec3 difflightColor = vec3(1.0,1.0,1.0);
vec3 norm = normalize(Normal);
vec3 lightDir = normalize(lightPos - FragPos);
float diff = max(dot(norm, lightDir), 0.0);
vec3 diffuse = diff * difflightColor;

outColor = (ambient + vec4(diffuse, 1.0)) * texture(texture1, TexCoord);
// ####################################################################
// outColor = vec4(Color, 1.0);
//outColor = texture(texture1, TexCoord);
}
)glsl";

// ####################################################################
// LAB_5
glm::mat4 model;
glm::mat4 proj;
glm::mat4 view;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
// ####################################################################
// LAB_9
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
// ####################################################################
double rotation = 0;
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraPositionUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraPositionFront = glm::vec3(0.0f, 0.0f, -1.0f);

bool fMouse = true;
int last_x, last_y;
double yaw = -90;
double pitch = 0;

void ustawKamereKlawisze(GLint view, float time) {
    float cameraSpeed = 0.000001f * time;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        cameraPosition += cameraSpeed * cameraPositionFront;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        cameraPosition -= cameraSpeed * cameraPositionFront;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        cameraPosition -= glm::normalize(glm::cross(cameraPositionFront, cameraPositionUp)) * cameraSpeed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        cameraPosition += glm::normalize(glm::cross(cameraPositionFront, cameraPositionUp)) * cameraSpeed;
    }
    glm::mat4 thisView;
    thisView = glm::lookAt(cameraPosition, cameraPosition + cameraPositionFront, cameraPositionUp);
    glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(thisView));
}

void ustawKamereMysz(GLint uniformView, float time, const sf::Window& window) {

    sf::Vector2i localPos = sf::Mouse::getPosition(window);
    sf::Vector2i position;
    bool reloc = false;

    if (localPos.x <= 0) {
        position.x = window.getSize().x;
        position.y = localPos.y;
        reloc = true;
    }
    if (localPos.x >= window.getSize().x - 1) {
        position.x = 0;
        position.y = localPos.y;
        reloc = true;
    }
    if (localPos.y <= 0) {
        position.y = window.getSize().y;
        position.x = localPos.x;
        reloc = true;
    }
    if (localPos.y >= window.getSize().y - 1) {
        position.y = 0;
        position.x = localPos.x;
        reloc = true;
    }
    if (reloc) {
        sf::Mouse::setPosition(position, window);
        fMouse = true;
    }
    localPos = sf::Mouse::getPosition(window);

    if (fMouse) {
        last_x = localPos.x;
        last_y = localPos.y;
        fMouse = false;
    }

    float offset_x = localPos.x - last_x;
    float offset_y = localPos.y - last_y;
    last_x = localPos.x;
    last_y = localPos.y;

    double sensit = 0.8f;
    double cameraSpeed = 0.00002f * time;
    offset_x *= sensit;
    offset_y *= sensit;

    yaw += offset_x * cameraSpeed;
    pitch -= offset_y * cameraSpeed;
    if (yaw > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch - -89.0f;

    glm::vec3 front;

    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraPositionFront = glm::normalize(front);

    glm::mat4 view;
    view = glm::lookAt(cameraPosition, cameraPosition + cameraPositionFront, cameraPositionUp);
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
}
// ###################################################################
// LAB_10 ????
std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
std::vector< glm::vec3 > temp_vertices;
std::vector< glm::vec2 > temp_uvs;
std::vector< glm::vec3 > temp_normals;

// ###################################################################
// LAB_10
bool loadOBJ(
    const char* path,
    std::vector < glm::vec3 >& out_vertices,
    std::vector < glm::vec2 >& out_uvs,
    std::vector < glm::vec3 >& out_normals
) {

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Impossible to open the file !\n");
        return false;
    }
    while (1) {
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.
        if (strcmp(lineHeader, "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0) {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0) {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0) {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9) {
                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
            for (unsigned int i = 0; i < vertexIndices.size(); i++) {
                unsigned int vertexIndex = vertexIndices[i];
                glm::vec3 vertex = temp_vertices[vertexIndex - 1];
                out_vertices.push_back(vertex);
            }

            for (unsigned int i = 0; i < uvIndices.size(); i++) {
                unsigned int uvIndex = uvIndices[i];
                glm::vec2 uv = temp_uvs[uvIndex - 1];
                out_uvs.push_back(uv);
            }

            for (unsigned int i = 0; i < normalIndices.size(); i++) {
                unsigned int normalIndex = normalIndices[i];
                glm::vec3 normal = temp_vertices[normalIndex - 1];
                out_normals.push_back(normal);
            }
        }
    }
}
// ###################################################################

int main() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    // Okno renderingu
    sf::Window window(sf::VideoMode(1000, 1000, 32), "OpenGL", sf::Style::Titlebar | sf::Style::Close, settings);

    window.setMouseCursorVisible(false);
    window.setMouseCursorGrabbed(true);

    // Inicjalizacja GLEW
    glewExperimental = GL_TRUE;
    glewInit();
    // Utworzenie VAO (Vertex Array Object)
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    int punkty_ = 36;

    float cameraSpeed = 0.00002f;
    float obrot = 0.005f;

    
    // ###################################################################
    // LAB_10
    std::vector< glm::vec3 > vertices;
    std::vector< glm::vec2 > uvs;
    std::vector< glm::vec3 > normals; // Won't be used at the moment.
    bool res = loadOBJ("obj.obj", vertices, uvs, normals);
    // ###################################################################
    GLuint vbo;
    glGenBuffers(6, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // ###################################################################
    // LAB_10
        /*glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * punkty_ * 6, vertices,
        GL_STATIC_DRAW);*/
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    // ###################################################################
    // obsługa tekstur
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis

    unsigned char* data = stbi_load("paint.jpg", &width, &height, &nrChannels, 0);

    glBindTexture(GL_TEXTURE_2D, texture1);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // Utworzenie i skompilowanie shadera wierzchołków
    GLuint vertexShader =
        glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    // Utworzenie i skompilowanie shadera fragmentów
    GLuint fragmentShader =
        glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    GLint isVertexCompiled = 0, isfragmentCompiled = 0, maxLength = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isVertexCompiled);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isfragmentCompiled);
    if (isfragmentCompiled != GL_FALSE) {
        cout << "Compilation fragmentShader - OK\n";
    }
    else {
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
        vector<GLchar> errorLogFragment(maxLength);
        cout << "Compilation fragmentShader - ERROR\n";
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &errorLogFragment[0]);
    }
    if (isVertexCompiled != GL_FALSE) {
        cout << "Compilation and vertexShader - OK\n";
    }
    else {
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
        vector<GLchar> errorLogVertex(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &errorLogVertex[0]);
        cout << "Compilation fragmentShader and vertexShader - ERROR\n";
    }
    // ####################################################################
    // Zlinkowanie obu shaderów w jeden wspólny program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    // Specifikacja formatu danych wierzchołkowych
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), 0); // 5 - > 6 - > 8 i 2 - > 3
    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // 5 - > 6 - > 8 

    GLint TexCoord = glGetAttribLocation(shaderProgram, "aTexCoord");	//do obsługi tekstur
    glEnableVertexAttribArray(TexCoord);
    glVertexAttribPointer(TexCoord, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    // ####################################################################
    // LAB_9
    GLint NorAttrib = glGetAttribLocation(shaderProgram, "aNormal");
    glEnableVertexAttribArray(NorAttrib);
    glVertexAttribPointer(NorAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    GLint uniLightPos = glGetUniformLocation(shaderProgram, "lightPos");
    glUniform3fv(uniLightPos, 1, &lightPos[0]);
    // ####################################################################

    bool running = true;
    sf::Mouse myszka;
    GLenum prymityw = GL_TRIANGLES;

    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    GLint uniTrans = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(model));

    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    GLint uniView = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

    proj = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.06f, 100.0f);
    GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

    window.setMouseCursorGrabbed(true); //przechwycenie kursora myszy w oknie ------------
    window.setMouseCursorVisible(false); //ukrycie kursora myszy
    // ###################################################################
    // LAB_9
    window.setFramerateLimit(20);
    // ###################################################################
    sf::Time time;
    sf::Clock clock;
    // ###################################################################
    // LAB_9
    int counter = 0;
    // ###################################################################




    while (running) {
        time = clock.getElapsedTime();
        time = clock.restart();



        // ####################################################################
    // LAB_9
        counter++;
        float fps = 1000000 / time.asMicroseconds();
        if (counter > fps) {
            window.setTitle(std::to_string(fps));
            counter = 0;
        }
        // ####################################################################
        sf::Event windowEvent;
        while (window.pollEvent(windowEvent)) {
            switch (windowEvent.type) {
            case sf::Event::Closed:
                running = false;
                break;
            }
        }

        sf::Event::KeyPressed;
        switch (windowEvent.key.code) {
        case sf::Keyboard::Escape:
            running = false;
            break;
        case sf::Keyboard::Num1:
            prymityw = GL_POINTS;
            break;
        case sf::Keyboard::Num2:
            running = GL_LINES;
            break;
        case sf::Keyboard::Num3:
            running = GL_LINE_STRIP;
            break;
        case sf::Keyboard::Num4:
            running = GL_LINE_LOOP;
            break;
        case sf::Keyboard::Num5:
            running = GL_TRIANGLES;
            break;
        case sf::Keyboard::Num6:
            running = GL_TRIANGLE_STRIP;
            break;
        case sf::Keyboard::Num7:
            running = GL_TRIANGLE_FAN;
            break;
        case sf::Keyboard::Num8:
            running = GL_QUADS;
            break;
        case sf::Keyboard::Num9:
            running = GL_QUAD_STRIP;
            break;
        case sf::Keyboard::Num0:
            running = GL_POLYGON;
            break;
        }
        sf::Vector2i mouse = myszka.getPosition(window);
        sf::Event::MouseMoved;
        switch (windowEvent.type)
        {
        case sf::Event::MouseMoved:
            ustawKamereMysz(uniView, time.asMicroseconds(), window);
            break;
        }
        // Nadanie scenie koloru czarnego
        ustawKamereKlawisze(uniView, time.asMicroseconds());
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // ####################################################################
        // LAB_9
        glBindTexture(GL_TEXTURE_2D, texture1);
        // ####################################################################
        glDrawArrays(prymityw, 0, punkty_);
        window.display();
    }
    // Kasowanie programu i czyszczenie buforów
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    // Zamknięcie okna renderingu
    window.close();
    return 0;
}