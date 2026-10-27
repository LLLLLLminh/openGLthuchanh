#pragma once

#include "shader_s.h"
#include "texture.h"
#include "stb_image.h"
#include "camera.h"
#include "Model.h"
#include "LightSource.h"

class ModelLoader {
public:
	Shader* modelShader;
	Model* ourModel;
    glm::vec3 position;
	float rotationZ;

	ModelLoader(std::string path) {
		modelShader = new Shader("shaderModel.vert", "shaderModel.frag");
		ourModel = new Model(path);
        lightColor = glm::vec3(1.0f, 1.0f, 0.95f);
        diffuseColor = lightColor * glm::vec3(0.5f);
        ambientColor = diffuseColor * glm::vec3(0.2f);
		position = glm::vec3(0);
		rotationZ = 0.0f;
	}

    void SetUpPos(glm::vec3 pos, float rotZ) {
        position = pos;
        rotationZ = rotZ;
	}

    void UpdateLightColor(glm::vec3 lightCol) {
        lightColor = lightCol;
        diffuseColor = lightColor * glm::vec3(0.5f);
        ambientColor = diffuseColor * glm::vec3(0.2f);
	}

	void RenderModel(Camera* camera, glm::vec3 dirLightDirection, std::vector<LightSource> pointLights, bool isDay) {
        modelShader->use();

        projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        view = camera->GetViewMatrix();
        modelShader->SetMattrix4("projection", projection);
        modelShader->SetMattrix4("view", view);

        modelShader->SetVec3("dirLight.direction", -dirLightDirection);
        modelShader->SetVec3("dirLight.ambient", ambientColor);
        modelShader->SetVec3("dirLight.diffuse", diffuseColor);
        modelShader->SetVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

        for (unsigned int i = 0; i < pointLights.size(); i++) {
            std::string number = std::to_string(i);
            modelShader->SetVec3("pointLights[" + number + "].position", pointLights[i].position);
            modelShader->SetVec3("pointLights[" + number + "].ambient", ambientColor);
            modelShader->SetVec3("pointLights[" + number + "].diffuse", diffuseColor);
            modelShader->SetVec3("pointLights[" + number + "].specular", 1.0f, 1.0f, 1.0f);
            modelShader->SetFloat("pointLights[" + number + "].constant", pointLights[i].attenuation.x);
            modelShader->SetFloat("pointLights[" + number + "].linear", pointLights[i].attenuation.y);
            modelShader->SetFloat("pointLights[" + number + "].quadratic", pointLights[i].attenuation.z);
		}


        modelShader->SetInt("material.diffuse", 0);
        modelShader->SetVec3("material.specular", ourModel->mat.Specular);
        modelShader->SetFloat("material.shininess", 32.0f);

        model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(rotationZ), glm::vec3(0, 1, 0)); //y tuong ung voi truc z
        model = glm::translate(model, position);
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader->SetMattrix4("model", model);
        modelShader->SetBool("isDay", isDay);
        ourModel->Draw(*modelShader);
	}
private:
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 model;
    glm::vec3 lightColor;
    glm::vec3 diffuseColor;
    glm::vec3 ambientColor;
};