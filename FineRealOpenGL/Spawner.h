#pragma once

#include "ModelLoader.h"
#include <cstdlib>
#include <ctime>

struct offset {
	int modelIndex;
	float offsetValue;
};


class Spawner {
public:
	std::vector<ModelLoader*> modelList;
	std::vector<offset> offsetList;

	Spawner(glm::vec3 position, float rotZ) {
		modelList = std::vector<ModelLoader*>();
		offsetList = std::vector<offset>();
		this->position = position;
		this->rotationZ = rotZ;
	}

	void Update(Camera* camera, glm::vec3 dirLightDirection, std::vector<LightSource> pointLights, bool isDay, bool isStopped) {
		if (!isStopped)
			timer++;

		if (timer >= spawnInterval) {
			timer = 0;

			offset newOffset = { rand() % modelList.size(), 0.0f };
			offsetList.push_back(newOffset);
		}
		for (offset& o : offsetList) {
			if (!isStopped)
				o.offsetValue += speed;
			modelList[o.modelIndex]->SetUpPos(position, rotationZ);
			modelList[o.modelIndex]->position.x = position.x + o.offsetValue;
			modelList[o.modelIndex]->RenderModel(camera, dirLightDirection, pointLights, isDay);
		}
		if (offsetList.size() > 0 && offsetList[0].offsetValue > distance) {
			offsetList.erase(offsetList.begin());
		}
	}
private:
	glm::vec3 position;
	float rotationZ;
	int timer = 0;
	int spawnInterval = 500;

	float speed = 0.1;
	float distance = 110.0f;	

	std::string modelPath;
	std::string modelPath2;
};
