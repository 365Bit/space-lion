#ifndef scene_h
#define scene_h

#include <list>

#include "sceneEntity.h"
#include "staticSceneObject.h"
#include "sceneCamera.h"
#include "sceneLightSource.h"
#include "volumetricSceneObject.h"

//pragmas seem to be only necessary in windows
#ifdef _WIN32
	#pragma comment(lib,"opengl32.lib")
#endif

class scene
{
protected:
	/*
	/	The following lists contain all entities (objects if you will) that are part of the scene.
	/	The currently used std::list datastructures are to be replaced with a more sophisticated concept in the near future.
	*/
	std::list<sceneLightSource> lightSourceList;
	std::list<sceneCamera> cameraList;
	std::list<staticSceneObject> scenegraph;
	std::list<volumetricSceneObject> volumetricObjectList;

	sceneCamera* activeCamera;

public:
	scene();
	~scene();

	//	create a scene entity with default geometry and default material
	bool createStaticSceneObject(const int id, const glm::vec3 position, const glm::quat orientation, vertexGeometry* geomPtr, material* mtlPtr);
	
	/* create a volumetric scene entity */
	bool createVolumetricSceneObject(const int id, const glm::vec3 position, const glm::quat orientation, const glm::vec3 scaling, vertexGeometry* geomPtr, texture3D* volPtr, GLSLProgram* prgmPtr);
	
	//	create a scene light source
	bool createSceneLight(const int id, const glm::vec3 position, glm::vec4 lightColour);
	//	create a scene camera
	bool createSceneCamera(const int id, const glm::vec3 position, const glm::quat orientations, float aspect, float fov);

	void setActiveCamera(const int);

	void testing();

	/* render the scene */
	void render();

	/*
	/	Render the volumetric objects of the scene.
	/	This is usually be done in a sperate render pass to allow depth correct blending.
	*/
	void renderVolumetricObjects();
};

#endif
