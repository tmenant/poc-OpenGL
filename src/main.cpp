#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

std::string readFile(const char *filePath)
{
    std::string contenu;
    std::ifstream stream;

    stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        stream.open(filePath);
        std::stringstream fileStream;
        fileStream << stream.rdbuf();
        stream.close();
        contenu = fileStream.str();
    }
    catch (std::ifstream::failure &e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_READ (" << filePath << ")" << std::endl;
    }

    return contenu;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

uint32_t glCompileShaders()
{
    unsigned int shaderProgram = glCreateProgram();

    std::vector<unsigned int> shaders;

    for (auto &file : fs::recursive_directory_iterator("shaders"))
    {
        if (file.path().extension() == ".vert")
        {
            const std::string shaderCode = readFile(file.path().string().c_str());
            const char *codePtr = shaderCode.c_str();

            unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &codePtr, NULL);
            glCompileShader(vertexShader);
            glAttachShader(shaderProgram, vertexShader);

            shaders.push_back(vertexShader);
        }

        if (file.path().extension() == ".frag")
        {
            const std::string shaderCode = readFile(file.path().string().c_str());
            const char *codePtr = shaderCode.c_str();

            unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &codePtr, NULL);
            glCompileShader(fragmentShader);
            glAttachShader(shaderProgram, fragmentShader);

            shaders.push_back(fragmentShader);
        }
    }

    glLinkProgram(shaderProgram);

    for (int shader : shaders)
    {
        glDeleteShader(shader);
    }

    return shaderProgram;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    uint32_t shaderProgram = glCompileShaders();

    // Coordonnées des 3 sommets (X, Y, Z)
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // Bas Gauche
        0.5f,
        -0.5f,
        0.0f, // Bas Droite
        0.0f,
        0.5f,
        0.0f // Haut
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 1. Lier le Vertex Array Object (VAO)
    glBindVertexArray(VAO);

    // 2. Lier et envoyer les données dans le Vertex Buffer Object (VBO)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3. Expliquer à OpenGL comment lire ces données
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Détacher le VBO et le VAO proprement (optionnel mais bonne pratique)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // --- BOUCLE DE RENDU ---
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Nettoyer l'écran
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Couleur de fond légèrement modifiée
        glClear(GL_COLOR_BUFFER_BIT);

        // --- DESSINER LE TRIANGLE ---
        glUseProgram(shaderProgram);      // Activer notre shader
        glBindVertexArray(VAO);           // Activer notre configuration de données
        glDrawArrays(GL_TRIANGLES, 0, 3); // Action de dessin (Mode, Début, Nombre de sommets)

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- NETTOYAGE ---
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
