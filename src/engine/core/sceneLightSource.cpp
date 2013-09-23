#include "sceneLightSource.h"


SceneLightSource::SceneLightSource()
{
}

SceneLightSource::~SceneLightSource()
{
}

SceneLightSource::SceneLightSource(int inId, const glm::vec3& inPosition, const glm::vec3& inLightColour)
									: SceneEntity(inId, inPosition, glm::quat(), glm::vec3(1.0)), lightColour(inLightColour)
{
}
