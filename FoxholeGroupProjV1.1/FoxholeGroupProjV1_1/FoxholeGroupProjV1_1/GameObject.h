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
		scale = objScale;
		textureId = texId;
		meshId = mId;
	}
	std::string getName() { return nameId; }
	glm::vec3 getPos() { return position; }
	glm::vec3 getScale() { return scale; }
	void setPos(glm::vec3 newPos) { position = newPos; }
	//getDimensions??
protected:
	GLuint meshId;
	GLuint textureId;
	std::string nameId;
	glm::vec3 position;
	glm::vec3 scale;
};

#endif
