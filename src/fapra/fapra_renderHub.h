#ifndef fapra_renderHub_h
#define fapra_renderHub_h
/*
/	In this class, all parts directly related to the rendering done on a single device come together.
/
/	OpenGL context, window management, scene management, aswell as communication with other engine modules
/	is handled here.
*/

#include <vector>
#include "../engine/core/renderHub.h"
#include "GLFW/glfw3.h"

class FapraRenderHub : public RenderHub
{
public:
	FapraRenderHub(void);
	~FapraRenderHub(void);

	void renderActiveScene();

private:
	std::vector<FramebufferObject> framebufferList;

	FramebufferObject *activeFramebuffer;
};

#endif
