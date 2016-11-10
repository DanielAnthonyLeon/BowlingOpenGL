#include "A5.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "lodepng.h"

using namespace glm;
using namespace irrklang;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

const unsigned int NUM_TEXTURES = 4;

const GLuint SHADOW_WIDTH = pow(2, 13), SHADOW_HEIGHT = pow(2, 13);

//----------------------------------------------------------------------------------------
// Constructor
A5::A5(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
		m_uvCoordsAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
		m_vbo_uvCoords(0),
		m_vbo_skyBox_positions(0),
		m_vao_skyBox(0)
{

}

//----------------------------------------------------------------------------------------
// Destructor
A5::~A5()
{
	delete m_bowlingObjects;
	delete [] m_textureIDs;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A5::init()
{
	// Set the background colour.
	glClearColor(1.0, 1.0, 1.0, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_meshData);
	glGenVertexArrays(1, &m_vao_skyBox);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
		getAssetFilePath("sphere_earth.obj"),
		getAssetFilePath("sphere_pink.obj"),
		getAssetFilePath("plane2.obj"),
		getAssetFilePath("bowlingpin.obj")
	});

	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();

	m_mousePressed[Left] = m_mousePressed[Middle] = m_mousePressed[Right] = false;
	m_mode = Play;

	m_mouseLeftJustReleased = m_mouseRightJustReleased = false;

	m_bowlingObjects = new BowlingObjects;
	initBowlingObjects(*m_rootNode, mat4());
	// Once we know the number of bowling objects, can set up the last collided relationships
	m_bowlingObjects->setUpLastCollided();
	m_mainBowlingBall = m_bowlingObjects->getBowlingBall(0);
	m_secondaryBowlingBall = m_bowlingObjects->getBowlingBall(1);

	m_friction = 0.0f;
	m_bowlingObjects->setFriction(m_friction);

	// Object textures
	m_textureIDs = new GLuint[NUM_TEXTURES];
	glGenTextures(NUM_TEXTURES, m_textureIDs);
	loadTexture("Assets/lanewood2_256.png", m_textureIDs[BowlingLaneTexture]);
	loadTexture("Assets/earthmap_256.png", m_textureIDs[BowlingBallEarthTexture]);
	loadTexture("Assets/pinkbowlingball_256.png", m_textureIDs[BowlingBallPinkTexture]);
	loadTexture("Assets/bowlingpin_256.png", m_textureIDs[BowlingPinTexture]);

	// Skybox
	vector<const GLchar*> faces;
	/*faces.push_back("Assets/mp_cloud9/cloud9_rt.png"); // right
	faces.push_back("Assets/mp_cloud9/cloud9_lt.png"); // left
	faces.push_back("Assets/mp_cloud9/cloud9_up.png"); // top
	faces.push_back("Assets/mp_cloud9/cloud9_dn.png"); // bottom
	faces.push_back("Assets/mp_cloud9/cloud9_bk.png"); // back
	faces.push_back("Assets/mp_cloud9/cloud9_ft.png");*/ // front

	faces.push_back("Assets/skyboxtutorial/right.png"); // right
	faces.push_back("Assets/skyboxtutorial/left.png"); // left
	faces.push_back("Assets/skyboxtutorial/top.png"); // top
	faces.push_back("Assets/skyboxtutorial/bottom.png"); // bottom
	faces.push_back("Assets/skyboxtutorial/back.png"); // back
	faces.push_back("Assets/skyboxtutorial/front.png"); // front
	m_skyBoxID = loadCubemap(faces);

	// Normal map for the bowling point
	glGenTextures(1, &m_normalMap);
	loadTexture("Assets/bowlingpinnormal_256.png", m_normalMap);

	bindShadowBuffer();

	//m_SoundEngine = irrklang::createIrrKlangDevice();
	//m_SoundEngine->play2D("Assets/breakout.mp3", GL_TRUE);

	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void A5::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could not open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A5::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shadowShader.generateProgramObject();
	m_shadowShader.attachVertexShader( getAssetFilePath("simpleDepthShader.vs").c_str() );
	m_shadowShader.attachFragmentShader( getAssetFilePath("simpleDepthShader.fs").c_str() );
	m_shadowShader.link();

	m_skyBoxShader.generateProgramObject();
	m_skyBoxShader.attachVertexShader( getAssetFilePath("skyBox.vs").c_str() );
	m_skyBoxShader.attachFragmentShader( getAssetFilePath("skyBox.fs").c_str() );
	m_skyBoxShader.link();
}

//----------------------------------------------------------------------------------------
void A5::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		// Enable the vertex shader attribute location for "uvCoords" when rendering.
		m_uvCoordsAttribLocation = m_shader.getAttribLocation("uvCoords");
		glEnableVertexAttribArray(m_uvCoordsAttribLocation);

		// Enable the shadow vertex shader attribute location for "position" when rendering.
		m_shadowPositionAttribLocation = m_shadowShader.getAttribLocation("position");
		glEnableVertexAttribArray(m_shadowPositionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Enable input slots for m_vao_skyBox
	{
		glBindVertexArray(m_vao_skyBox);

		// Enable the skybox vertex shader attribute location for "position" when rendering.
		m_skyBoxPositionAttribLocation = m_skyBoxShader.getAttribLocation("position");
		glEnableVertexAttribArray(m_skyBoxPositionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A5::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all uv coordinate data
	{
		glGenBuffers(1, &m_vbo_uvCoords);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_uvCoords);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumUVCoordsBytes(),
				meshConsolidator.getUVCoordsDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all sky box position data.
	{
		glGenBuffers(1, &m_vbo_skyBox_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_skyBox_positions);

		GLfloat skyboxVertices[] = {
    	// Positions
    	-1.0f,  1.0f, -1.0f,
    	-1.0f, -1.0f, -1.0f,
     	1.0f, -1.0f, -1.0f,
     	1.0f, -1.0f, -1.0f,
     	1.0f,  1.0f, -1.0f,
    	-1.0f,  1.0f, -1.0f,

    	-1.0f, -1.0f,  1.0f,
    	-1.0f, -1.0f, -1.0f,
    	-1.0f,  1.0f, -1.0f,
    	-1.0f,  1.0f, -1.0f,
    	-1.0f,  1.0f,  1.0f,
    	-1.0f, -1.0f,  1.0f,

     	1.0f, -1.0f, -1.0f,
     	1.0f, -1.0f,  1.0f,
     	1.0f,  1.0f,  1.0f,
     	1.0f,  1.0f,  1.0f,
     	1.0f,  1.0f, -1.0f,
     	1.0f, -1.0f, -1.0f,

    	-1.0f, -1.0f,  1.0f,
    	-1.0f,  1.0f,  1.0f,
     	1.0f,  1.0f,  1.0f,
     	1.0f,  1.0f,  1.0f,
     	1.0f, -1.0f,  1.0f,
    	-1.0f, -1.0f,  1.0f,

    	-1.0f,  1.0f, -1.0f,
     	1.0f,  1.0f, -1.0f,
     	1.0f,  1.0f,  1.0f,
     	1.0f,  1.0f,  1.0f,
    	-1.0f,  1.0f,  1.0f,
    	-1.0f,  1.0f, -1.0f,

    	-1.0f, -1.0f, -1.0f,
    	-1.0f, -1.0f,  1.0f,
     	1.0f, -1.0f, -1.0f,
     	1.0f, -1.0f, -1.0f,
    	-1.0f, -1.0f,  1.0f,
     	1.0f, -1.0f,  1.0f
		};

		glBufferData(GL_ARRAY_BUFFER, 6*6*3 * sizeof(GLfloat),
								 skyboxVertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A5::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_uvCoords" into the
	// "uvCoords" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_uvCoords);
	glVertexAttribPointer(m_uvCoordsAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_shadowPositionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Sky box

	glBindVertexArray(m_vao_skyBox);

	// Tell GL how to map data from the vertex buffer to "m_vbo_skyBox_positions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_skyBox_positions);
	glVertexAttribPointer(m_skyBoxPositionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

void A5::loadTexture(const char *filename, GLuint& textureID) {
	Image image;
	image.loadPNG(filename);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D,
							 0,
							 GL_RGBA,
							 image.width(),
							 image.height(),
							 0,
							 GL_RGBA,
							 GL_UNSIGNED_BYTE,
							 &(image.data()[0]));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	CHECK_GL_ERRORS;
}

GLuint A5::loadCubemap(std::vector<const GLchar*>& faces) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++) {
		Image image;
		image.loadPNG(faces[i]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
								 image.width(), image.height(), 0, GL_RGBA,
							 	 GL_UNSIGNED_BYTE, &(image.data()[0]));
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

void A5::bindShadowBuffer() {
	glGenFramebuffers(1, &m_depthMapFBO);

	glGenTextures(1, &m_depthMap);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT,
							 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//----------------------------------------------------------------------------------------
void A5::initPerspectiveMatrix()
{
	//float aspect = ((float)m_windowWidth) / m_windowHeight;
	float aspect = (float)m_framebufferWidth / m_framebufferHeight;
	m_perspective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A5::initViewMatrix() {
	vec3 lookFrom(0.0f, 0.0f, 0.0f);
	vec3 lookAt(0.0f, 0.0f, -1.0f);
	vec3 up(0.0f, 1.0f, 0.0f);

	m_eye = m_originalEye = lookFrom;

	m_view = m_originalView = glm::lookAt(lookFrom, lookAt, up);

	m_viewZaxis = normalize(lookAt - lookFrom);
	m_viewXaxis = normalize(cross(up, m_viewZaxis));
	m_viewYaxis = cross(m_viewZaxis, m_viewXaxis);

	m_originalViewXaxis = m_viewXaxis;
	m_viewYaxis = m_originalViewYaxis = -m_viewYaxis;
	m_viewZaxis = m_originalViewZaxis = -m_viewZaxis;
}

//----------------------------------------------------------------------------------------
void A5::initLightSources() {
	// World-space position
	m_light.position = vec3(-2.0f, 5.0f, 0.5f);
	m_light.rgbIntensity = vec3(0.8f); // White light
	m_lightMoveDirection = 1;
	m_lightSpeed = 0.0f;
}

bool A5::moveLight(float dZ, float minZ, float maxZ, int direction) {
	m_light.position.z += direction * dZ;
	if (m_light.position.z > maxZ or m_light.position.z < minZ)
		return true;
	return false;
}

//----------------------------------------------------------------------------------------
void A5::uploadCommonSceneUniforms() {
	GLfloat near_plane = 0.0f, far_plane = 100.0f;
	mat4 lightProjection = glm::ortho<float>(-100, 100, -100, 100, near_plane, far_plane);
	mat4 lightView = glm::lookAt(m_light.position, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
	mat4 lightSpaceMatrix = lightProjection * lightView;

	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perspective));
		CHECK_GL_ERRORS;


		//-- Set LightSource uniform for the scene:
		{
			location = m_shader.getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;
		}

		//-- Set background light ambient intensity
		{
			location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.05f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}

		// Set lightSpaceMatrix
		{
			location = m_shader.getUniformLocation("lightSpaceMatrix");
			glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(lightSpaceMatrix));
			CHECK_GL_ERRORS;
		}

		// Set location for tex
		{
			location = m_shader.getUniformLocation("textureMap");
			glUniform1i(location, 2);
			CHECK_GL_ERRORS;
		}

		// Set up shadow map
		{
			location = m_shader.getUniformLocation("shadowMap");
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_depthMap);
			glUniform1i(location, 0);
			CHECK_GL_ERRORS;
		}

		// Set up normal map
		{
			location = m_shader.getUniformLocation("normalMap");
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_normalMap);
			glUniform1i(location, 1);
			CHECK_GL_ERRORS;
		}

		// Set normalMapping to false by default
		{
			location = m_shader.getUniformLocation("normalMapping");
			glUniform1i(location, 0);
			CHECK_GL_ERRORS;
		}
	}
	m_shader.disable();

	m_shadowShader.enable();
	{
		// Set lightSpaceMatrix
		GLint location = m_shadowShader.getUniformLocation("lightSpaceMatrix");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(lightSpaceMatrix));
		CHECK_GL_ERRORS;
	}
	m_shadowShader.disable();

	m_skyBoxShader.enable();
	{
		// Set projection
		GLint location = m_skyBoxShader.getUniformLocation("projection");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perspective));
		CHECK_GL_ERRORS;

		// Set view
		location = m_skyBoxShader.getUniformLocation("view");
		// Remove translation so the sky box is always the same distance away
		mat4 view = mat4(mat3(m_view));
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(view));
		CHECK_GL_ERRORS;

		// Set skybox
		location = m_skyBoxShader.getUniformLocation("skybox");
		glUniform1i(location, 5);
		CHECK_GL_ERRORS;
	}
	m_skyBoxShader.disable();
}

void A5::toggleNormalMapping(bool toggle) {
	m_shader.enable();
	{
		GLint location = m_shader.getUniformLocation("normalMapping");
		glUniform1i(location, toggle);
		CHECK_GL_ERRORS;
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A5::appLogic()
{
	// Place per frame, application logic here ...
	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A5::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		// Add more gui elements here here ...


		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application (Q)" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if (ImGui::Button("Play Mode (E)"))
			m_mode = Play;
		if (ImGui::Button("Rotate View (V)"))
			m_mode = RotateView;
		ImGuiStyle& style = ImGui::GetStyle();
		if (ImGui::SliderFloat("Friction", &m_friction, 0.0f, 1.0f, "%.2f"))
			m_bowlingObjects->setFriction(m_friction);
		if (ImGui::SliderFloat("Light Speed", &m_lightSpeed, 0.0f, 1.0f, "%.2f")) { }

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix,
		bool shadow
) {

	shader.enable();
	{
		if (shadow) {
			// Set model matrix
			GLint location = shader.getUniformLocation("model");
			mat4 model = node.trans;
			glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(model));
			CHECK_GL_ERRORS;
		}

		else {
			//-- Set ModelView matrix:
			GLint location = shader.getUniformLocation("Model");
			mat4 Model = node.trans;
			glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(Model));
			CHECK_GL_ERRORS;

			//-- Set View matrix:
			location = shader.getUniformLocation("View");
			mat4 View = viewMatrix;
			glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(View));
			CHECK_GL_ERRORS;

			//-- Set NormMatrix:
			location = shader.getUniformLocation("NormalMatrix");
			mat4 modelView = View * Model;
			mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
			glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
			CHECK_GL_ERRORS;

			//-- Set Material values:
			location = shader.getUniformLocation("material.kd");
			vec3 kd = node.material.kd;
			glUniform3fv(location, 1, value_ptr(kd));
			CHECK_GL_ERRORS;
			location = shader.getUniformLocation("material.ks");
			vec3 ks = node.material.ks;
			glUniform3fv(location, 1, value_ptr(ks));
			CHECK_GL_ERRORS;
			location = shader.getUniformLocation("material.shininess");
			glUniform1f(location, node.material.shininess);
			CHECK_GL_ERRORS;
		}
	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A5::draw() {

	// Move the objects appropriately
	float framerate = 60.0f;
	m_bowlingObjects->moveObjects(1.0f / framerate);

	// Move the light
	if (moveLight(m_lightSpeed, -2.0f, 20.0f, m_lightMoveDirection))
		m_lightMoveDirection *= -1;

	// Draw the stuff

	glEnable(GL_DEPTH_TEST);

	// Draw the scene

	glBindVertexArray(m_vao_meshData);

	// 1. first render to depth map
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	renderSceneGraph(*m_rootNode, mat4(), true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. then render scene as normal with shadow mapping (using depth map)
	glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_depthMap);
	renderSceneGraph(*m_rootNode, mat4(), false);

	glBindVertexArray(0);

	// Draw the sky box
	m_skyBoxShader.enable();
	glDepthFunc(GL_LEQUAL);
	glBindVertexArray(m_vao_skyBox);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyBoxID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
	m_skyBoxShader.disable();

	glDisable( GL_DEPTH_TEST );

	CHECK_GL_ERRORS;
}

void A5::renderSceneGraph(SceneNode& root, glm::mat4 netTrans, bool shadow) {
	netTrans = netTrans * root.get_transform(); // update netTrans with the current node

	// If it is a geometry node, draw it
	if (root.m_nodeType == NodeType::GeometryNode) {
		GeometryNode * geometryNode = static_cast<GeometryNode *>(&root);

		// Swap trans with netTrans
		mat4 tmpTrans = geometryNode->get_transform();
		geometryNode->set_transform(netTrans);

		// Set the texture based on the object
		if (geometryNode->bowlingObjectType() == "BowlingLane") {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, m_textureIDs[BowlingLaneTexture]);
			toggleNormalMapping(false);
		}
		else if (geometryNode->m_name == "BowlingBall1") {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, m_textureIDs[BowlingBallEarthTexture]);
			toggleNormalMapping(false);
		}
		else if (geometryNode->m_name == "BowlingBall2") {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, m_textureIDs[BowlingBallPinkTexture]);
			toggleNormalMapping(false);
		}
		else if (geometryNode->bowlingObjectType() == "BowlingPin") {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, m_textureIDs[BowlingPinTexture]);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_normalMap);
			toggleNormalMapping(true);
		}
		else {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, 0);
			toggleNormalMapping(false);
		}

		shadow ? updateShaderUniforms(m_shadowShader, *geometryNode, m_view, shadow) :
						 updateShaderUniforms(m_shader, *geometryNode, m_view, shadow);

		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];
		//-- Now render the mesh:
		shadow ? m_shadowShader.enable() : m_shader.enable();
		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		shadow ? m_shadowShader.disable() : m_shader.disable();

		CHECK_GL_ERRORS;

		// restore the model transform
		geometryNode->set_transform(tmpTrans);
	}

	for (SceneNode *child : root.children) {
		renderSceneGraph(*child, netTrans, shadow);
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A5::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A5::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A5::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	float dx = xPos - m_prevMousePos.x;
	float dy = yPos - m_prevMousePos.y;

	switch (m_mode) {
		case Play:
			if (m_mouseLeftJustReleased) {
				glm::vec4 velocity = vec4(dx/2.0f, 0.0f, dy, 0.0f);
				m_mainBowlingBall->setVelocity(velocity);
				m_mouseLeftJustReleased = false;
			}
			else if (m_mouseRightJustReleased) {
				glm::vec4 velocity = vec4(dx/2.0f, 0.0f, dy, 0.0f);
				m_secondaryBowlingBall->setVelocity(velocity);
				m_mouseRightJustReleased = false;
			}
			break;
		case RotateView:
			float angle = dx / 100.0f;
			// rotate about the x-axis
			if (m_mousePressed[Left]) {
				m_view = translate(m_view, -m_eye);
				m_view = rotate(m_view, angle, m_viewXaxis);
				m_view = translate(m_view, m_eye);

				m_viewYaxis = rotateVector(m_viewYaxis, m_viewXaxis, -angle);
				m_viewZaxis = rotateVector(m_viewZaxis, m_viewXaxis, -angle);
			}
			// rotate about the y-axis
			if (m_mousePressed[Middle]) {
				m_view = translate(m_view, -m_eye);
				m_view = rotate(m_view, angle, m_viewYaxis);
				m_view = translate(m_view, m_eye);

				m_viewXaxis = rotateVector(m_viewXaxis, m_viewYaxis, -angle);
				m_viewZaxis = rotateVector(m_viewZaxis, m_viewYaxis, -angle);
			}
			// rotate about the z-axis
			if (m_mousePressed[Right]) {
				m_view = translate(m_view, -m_eye);
				m_view = rotate(m_view, angle, m_viewZaxis);
				m_view = translate(m_view, m_eye);

				m_viewXaxis = rotateVector(m_viewXaxis, m_viewZaxis, -angle);
				m_viewYaxis = rotateVector(m_viewYaxis, m_viewZaxis, -angle);
			}
			break;
	}
	m_prevMousePos = vec2(xPos, yPos);

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A5::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (!ImGui::IsMouseHoveringAnyWindow()) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (actions == GLFW_PRESS)
				m_mousePressed[Left] = true;
			else if (actions == GLFW_RELEASE) {
				m_mousePressed[Left] = false;
				m_mouseLeftJustReleased = true;
			}
		}
		else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			if (actions == GLFW_PRESS)
				m_mousePressed[Middle] = true;
			else if (actions == GLFW_RELEASE)
				m_mousePressed[Middle] = false;
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			if (actions == GLFW_PRESS)
				m_mousePressed[Right] = true;
			else if (actions == GLFW_RELEASE) {
				m_mousePressed[Right] = false;
				m_mouseRightJustReleased = true;
			}
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A5::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A5::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A5::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		}
		else if (key == GLFW_KEY_Q)
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		else if (key == GLFW_KEY_E)
			m_mode = Play;
		else if (key == GLFW_KEY_V)
			m_mode = RotateView;
		else if (key == GLFW_KEY_R)
			reset();
		}
		if (key == GLFW_KEY_RIGHT) {
	}

	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		float inc = 0.2;
		if (key == GLFW_KEY_LEFT) {
			vec3 displacement = -inc * m_viewXaxis;
			m_view = translate(m_view, displacement);
			m_eye += displacement;
		}
		if (key == GLFW_KEY_RIGHT) {
			vec3 displacement = inc * m_viewXaxis;
			m_view = translate(m_view, displacement);
			m_eye += displacement;
		}
		if (key == GLFW_KEY_DOWN) {
			vec3 displacement = -inc * m_viewZaxis;
			m_view = translate(m_view, displacement);
			m_eye += displacement;
		}
		if (key == GLFW_KEY_UP) {
			vec3 displacement = inc * m_viewZaxis;
			m_view = translate(m_view, displacement);
			m_eye += displacement;
		}
		if (key == GLFW_KEY_KP_0) {
			vec3 displacement = -inc * m_viewYaxis;
			m_view = translate(m_view, displacement);
			m_eye += displacement;
		}
		if (key == GLFW_KEY_KP_1) {
			vec3 displacement = inc * m_viewYaxis;
			m_view = translate(m_view, displacement);
			m_eye += displacement;
		}
	}

	return eventHandled;
}

void A5::initBowlingObjects(SceneNode& root, glm::mat4 netTrans) {
	netTrans = netTrans * root.get_transform(); // update netTrans with the current node

	if (root.bowlingObjectType() == "BowlingBall") {
		GeometryNode *geometryNode = static_cast<GeometryNode *>(&root);
		BowlingBall *bowlingBall = new BowlingBall(geometryNode, 16.0f);
		m_bowlingObjects->addBowlingObject(bowlingBall);
	}
	else if (root.bowlingObjectType() == "BowlingPin") {
		GeometryNode *geometryNode = static_cast<GeometryNode *>(&root);
		BowlingPin *bowlingPin = new BowlingPin(geometryNode);
		m_bowlingObjects->addBowlingObject(bowlingPin);
	}

	for (SceneNode *child : root.children)
		initBowlingObjects(*child, netTrans);

}

glm::vec3 A5::rotateVector(glm::vec3 v, glm::vec3 axis, float angle) {
	mat4 R;
	R = rotate(R, angle, axis);
	v = vec3(R * vec4(v, 0.0f));
	return v;
}

void A5::reset() {
	m_bowlingObjects->resetObjects();

	m_mode = Play;

	m_view = m_originalView;
	m_eye = m_originalEye;
	m_viewXaxis = m_originalViewXaxis;
	m_viewYaxis = m_originalViewYaxis;
	m_viewZaxis = m_originalViewZaxis;

	m_friction = 0;
	m_bowlingObjects->setFriction(m_friction);

	m_lightMoveDirection = 1;
	m_lightSpeed = 0.0f;
}
