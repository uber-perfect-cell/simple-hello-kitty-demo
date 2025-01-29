#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void HSVtoRGB(float h, float s, float v, float& r, float& g, float& b) {
    float c = v * s;
    float x = c * (1 - std::abs(std::fmod(h / 60.0f, 2) - 1));
    float m = v - c;

    if(h >= 0 && h < 60) {
        r = c; g = x; b = 0;
    } else if(h >= 60 && h < 120) {
        r = x; g = c; b = 0;
    } else if(h >= 120 && h < 180) {
        r = 0; g = c; b = x;
    } else if(h >= 180 && h < 240) {
        r = 0; g = x; b = c;
    } else if(h >= 240 && h < 300) {
        r = x; g = 0; b = c;
    } else {
        r = c; g = 0; b = x;
    }

    r += m;
    g += m;
    b += m;
}


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
	stbi_set_flip_vertically_on_load(true);  
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Window", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("HelloKitty.png", &width, &height, &nrChannels, 0);
    
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    const char* vertexShaderSource = "#version 330 core\n"
	    "layout (location = 0) in vec3 aPos;\n"
	    "layout (location = 1) in vec3 aColor;\n"
	    "layout (location = 2) in vec2 aTexCoord;\n"
	    "uniform float offsetX;\n"
	   	"uniform float offsetY;\n"
	    "out vec3 ourColor;\n"
	    "out vec2 TexCoord;\n"
	    "void main()\n"
	    "{\n"
	    "    gl_Position = vec4(aPos.x + offsetX, aPos.y + offsetY, aPos.z, 1.0);\n"
	    "    ourColor = aColor;\n"
	    "    TexCoord = aTexCoord;\n"
	    "}\n";

    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec3 ourColor;\n"
        "in vec2 TexCoord;\n"
        "uniform sampler2D ourTexture;\n"
        "void main()\n"
        "{\n"
        "    FragColor = texture(ourTexture, TexCoord);\n"
        "}\n";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        // Positions        // Colors (unnecesairy)     // Texture Coords
         0.15f,  0.3f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 
         0.15f, -0.3f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 
        -0.15f, -0.3f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 
        -0.15f,  0.3f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

	float moveSpeed = 1.0f;
	float currentX = 0.0f;
	float currentY = 0.0f;
	float directionX = 1.0f;
	float directionY = 1.0f;

	float imageWidth = 0.15f;
	float imageHeight = 0.3f;

	float rightBound = 1.0f - (imageWidth / 2.0f);
	float leftBound = -1.0f + (imageWidth / 2.0f);
	float topBound = 1.0f - (imageHeight / 2.0f);
	float downBound = -1.0f + (imageHeight / 2.0f);

	float hue = 0.0f;
	float lastFrame = 0.0f;
	float colorSpeed = 50.0f;
	
	while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

    	hue += colorSpeed * deltaTime;
    	if (hue >= 360.0f) {
    	    hue = 0.0f;
    	}
        processInput(window);
        
        currentX += moveSpeed * directionX * deltaTime;
        currentY += moveSpeed * directionY * deltaTime;
    	if (currentX >= rightBound) {
        	currentX = rightBound;
        	directionX = -1.0f;
    	} else if (currentX <= leftBound) {
        	currentX = leftBound;
        	directionX = 1.0f;
    	}
    	
    	if (currentY >= topBound) {
    		currentY = topBound;
    	    directionY = -1.0f;
    	} else if (currentY <= downBound) {
    	    currentY = downBound;
    	    directionY = 1.0f;
    	}
    	
    	float r, g, b;
    	HSVtoRGB(hue, 0.7f, 0.5f, r, g, b);
    	int offsetLocX = glGetUniformLocation(shaderProgram, "offsetX");
    	int offsetLocY = glGetUniformLocation(shaderProgram, "offsetY");

		glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


    	glUseProgram(shaderProgram);
    	glUniform1f(offsetLocX, currentX);
    	glUniform1f(offsetLocY, currentY);

	    glBindTexture(GL_TEXTURE_2D, texture);
	    glBindVertexArray(VAO);
	    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	    glfwSwapBuffers(window);
	    glfwPollEvents();
	}

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}
