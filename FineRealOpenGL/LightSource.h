#pragma once

#include "shader_s.h"
#include "texture.h"
#include "stb_image.h"
#include "camera.h"
#include <vector>

static float vertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};
class LightSource {
public:
    Shader* lightSourceShader;
	glm::vec3 position;
    glm::vec3 attenuation;
    glm::vec3 lightColor;

    LightSource() {
        lightSourceShader = new Shader("shaderLight.vert", "shaderLight.frag");
        lightColor = glm::vec3(1.0f, 1.0f, 0.95f);
        prepare();
    }

    void prepare() {
        glGenVertexArrays(1, &lightVAO);
        glGenBuffers(1, &lightVBO);
        glBindVertexArray(lightVAO);
        glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }
    void use(Camera* camera, glm::vec3 lightPos, glm::vec3 scale, glm::vec3 atten) {
        position = lightPos;
		attenuation = atten;
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

        model = glm::translate(model, lightPos);
        model = glm::scale(model, scale);

        lightSourceShader->use();
        lightSourceShader->SetMattrix4("model", model);
        lightSourceShader->SetMattrix4("view", view);
        lightSourceShader->SetMattrix4("projection", projection);
        lightSourceShader->SetVec3("lightColor", lightColor);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

    }
private:
    unsigned int lightVAO, lightVBO;
};