#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class GameObject {
public:
	GameObject(std::string name, glm::vec3 pos, glm::vec3 objScale, GLuint texId, GLuint mId, int newCount) {
		nameId = name;
		position = pos;
		startPos = pos;
		scale = objScale;
		textureId = texId;
		meshId = mId;
		lastCollisionObjName = "";
		meshIndexCount = newCount;
	}
	std::string getName() { return nameId; }
	glm::vec3 getPos() { return position; }
	glm::vec3 getStartPos() { return startPos; }
	void setStartPos(glm::vec3 newStartPos) { startPos = newStartPos; }
	glm::vec3 getScale() { return scale; }
	void setScale(glm::vec3 newScale) { scale = newScale; }
	GLuint getTexture() { return textureId; }
	GLuint getMesh() { return meshId; }
	void setPos(glm::vec3 newPos) { startPos = position; position = newPos; }
	std::string getLastCollision() { return lastCollisionObjName; }
	void setLastCollision(std::string lastCollision) { lastCollisionObjName = lastCollision; }
	void reset() { position = startPos; lastCollisionObjName = ""; }
	int getMeshIndexCount() { return meshIndexCount; }
	void setMeshIndexCount(int newCount) { meshIndexCount = newCount; }
	float getRotation() { return rotation; }
	void setRotation(float newRot) { rotation = newRot; }
protected:
	GLuint meshId;
	GLuint textureId;
	std::string nameId;
	glm::vec3 position;
	glm::vec3 startPos;
	glm::vec3 scale;
	std::string lastCollisionObjName;
	int meshIndexCount;
	float rotation = 0.0f;
};

#endif
