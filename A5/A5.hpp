#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"

#include <glm/glm.hpp>
#include <memory>

#include "BowlingObjects.hpp"
#include "Image.hpp"

#include <irrklang/irrKlang.h>

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};

enum Mode {
	Play,
	RotateView
};

enum MouseButton {
	Left,
	Middle,
	Right
};

enum Texture {
	BowlingLaneTexture,
	BowlingBallEarthTexture,
	BowlingBallPinkTexture,
	BowlingPinTexture
};


class A5 : public CS488Window {
public:
	A5(const std::string & luaSceneFile);
	virtual ~A5();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void bindShadowBuffer();
	void loadTexture(const char *filename, GLuint& textureID);
	GLuint loadCubemap(std::vector<const GLchar*>& faces);
	void initViewMatrix();
	void initLightSources();

	bool moveLight(float dZ, float minZ, float maxZ, int direction);

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void toggleNormalMapping(bool toggle);

	void renderReflection(SceneNode& root, bool reflection);
	void renderBowlingLane(SceneNode& root, bool reflection);
	void renderBowlingBalls(SceneNode& root, bool reflection);
	void renderBowlingPins(SceneNode& root, bool reflection);
	void renderObject(SceneNode& root, std::string objectName, bool reflection);
	void renderSceneGraph(SceneNode& root, glm::mat4 netTrans, bool shadow);

	void initBowlingObjects(SceneNode& root, glm::mat4 netTrans);
	void moveObject(SceneNode& object, float timeInterval);

	glm::vec3 rotateVector(glm::vec3 v, glm::vec3 axis, float angle);

	void reset();

	glm::mat4 m_perspective;
	glm::mat4 m_view, m_originalView;
	glm::vec3 m_eye, m_originalEye;;
	glm::vec3 m_viewXaxis, m_originalViewXaxis;
	glm::vec3 m_viewYaxis, m_originalViewYaxis;
	glm::vec3 m_viewZaxis, m_originalViewZaxis;

	LightSource m_light;
	int m_lightMoveDirection;
	float m_lightSpeed;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;

	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLuint m_vbo_uvCoords; // uv coordinates for texture

	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	GLint m_uvCoordsAttribLocation;

	// Shaders
	ShaderProgram m_shader;

	// GL resources for texture mapping
	std::vector<unsigned char> m_textureBuffer;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> m_rootNode;

	bool m_mouseLeftJustReleased, m_mouseRightJustReleased;
	bool m_mousePressed[3];
	glm::vec2 m_prevMousePos;

	BowlingObjects *m_bowlingObjects;
	BowlingBall *m_mainBowlingBall;
	BowlingBall *m_secondaryBowlingBall;

	Mode m_mode;

	int m_lastCollided;

	GLuint *m_textureIDs;

	float m_friction;

	// Shadow mapping
	GLuint m_depthMapFBO;
	GLuint m_depthMap;
	ShaderProgram m_shadowShader;
	GLint m_shadowPositionAttribLocation;

	// Normal mapping
	GLuint m_normalMap;

	irrklang::ISoundEngine *m_SoundEngine;

	// Skybox
	GLuint m_skyBoxID;
	GLuint m_vbo_skyBox_positions;
	GLuint m_vao_skyBox;
	GLint m_skyBoxPositionAttribLocation;
	std::vector<GLfloat> m_skyBox_vertices;
	ShaderProgram m_skyBoxShader;
};
