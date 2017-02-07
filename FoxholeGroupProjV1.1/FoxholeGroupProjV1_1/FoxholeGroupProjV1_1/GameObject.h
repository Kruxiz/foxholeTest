#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class GameObject {
public:
	GameObject(std::string name, glm::vec3 pos, glm::vec3 objScale, GLuint texId, GLuint mId) {
		nameId = name;
		position = pos;
		startPos = pos;
		scale = objScale;
		textureId = texId;
		meshId = mId;
		lastCollisionObjName = "";
	}
	std::string getName() { return nameId; }
	glm::vec3 getPos() { return position; }
	glm::vec3 getScale() { return scale; }
	GLuint getTexture() { return textureId; }
	GLuint getMesh() { return meshId; }
	void setPos(glm::vec3 newPos) { position = newPos; }
	std::string getLastCollision() { return lastCollisionObjName; }
	void setLastCollision(std::string lastCollision) { lastCollisionObjName = lastCollision; }
	void reset() { position = startPos; lastCollisionObjName = "";}
	//getDimensions??
	//meshIndexCount getters and setters
protected:
	GLuint meshId;
	GLuint textureId;
	std::string nameId;
	glm::vec3 position;
	glm::vec3 startPos;
	glm::vec3 scale;
	std::string lastCollisionObjName;
};

#endif
