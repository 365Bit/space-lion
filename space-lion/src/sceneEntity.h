#ifndef sceneEntity_h
#define sceneEntity_h

//openGL Math Lib
#include <glm/glm.hpp>
#include <glm/core/type_vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "vertexGeometry.h"
#include "material.h"

class sceneEntity
{
private:
	const int id;

	glm::vec3 position;
	//	orentation is saved as a quaternion, thus a vec4 is used
	glm::vec4 orientation;

	bool isRendered;

	vertexGeometry *geometry;
	material *mtl;
public:
	sceneEntity();
	~sceneEntity();

	sceneEntity(const int, vertexGeometry*, material*);

	const int getId() {return id;}
	void setPosition(const glm::vec3 inPosition) {position = inPosition;}
	glm::vec3 getPosition() {return position;}
	void setOrientation(const glm::vec4 inOrientation) {orientation = inOrientation;}
	glm::vec4 getOrientation() {return orientation;}
};

#endif sceneEntity
