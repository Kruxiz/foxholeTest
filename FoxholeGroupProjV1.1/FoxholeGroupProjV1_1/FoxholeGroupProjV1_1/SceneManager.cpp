#include "SceneManager.h"


#define DEG_TO_RADIAN 0.017453293

SceneManager::SceneManager() {
	eye = glm::vec3(0.0f, 1.0f, 0.0f);
	at = glm::vec3(0.0f, 1.0f, -1.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	skyboxProgram = rt3d::initShaders("cubeMap.vert", "cubeMap.frag");

	//lights - initialise first light - can possibly be read in from file using rt3d::load file
	lights.push_back({
		{ 0.3f, 0.3f, 0.3f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ -10.0f, 10.0f, 10.0f, 1.0f } });

	//materials - same as above
	materials.push_back({
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // ambient
		{ 0.5f, 1.0f, 0.5f, 1.0f }, // diffuse
		{ 0.0f, 0.1f, 0.0f, 1.0f }, // specular
		2.0f  // shininess
	}
	);

	materials.push_back({
		{ 0.0f, 0.0f, 0.0f, 1.0f }, // ambient
		{ 0.5f, 1.0f, 0.5f, 1.0f }, // diffuse
		{ 0.0f, 0.1f, 0.0f, 1.0f }, // specular
		2.0f  // shininess
	}
	);

	lightPos = { 0.0f, 2.0f, -6.0f, 1.0f };
}

void SceneManager::renderSkybox(glm::mat4 projection)
{
	// skybox as single cube using cube map
	glUseProgram(skyboxProgram);
	rt3d::setUniformMatrix4fv(skyboxProgram, "projection", glm::value_ptr(projection));
	glDepthMask(GL_FALSE); // make sure writing to update depth test is off
	glm::mat3 mvRotOnlyMat3 = glm::mat3(mvStack.top());
	mvStack.push(glm::mat4(mvRotOnlyMat3));
	glCullFace(GL_FRONT); // drawing inside of cube!
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox[0]);
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(1.5f, 1.5f, 1.5f));
	rt3d::setUniformMatrix4fv(skyboxProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();
	glCullFace(GL_BACK); // drawing inside of cube!
						 // back to remainder of rendering
	glDepthMask(GL_TRUE); // make sure depth test is on
}

void SceneManager::clearScreen()
{
	// clear the screen
	glEnable(GL_CULL_FACE);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

glm::mat4 SceneManager::initRendering()
{
	glm::mat4 projection(1.0);
	projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), 800.0f / 600.0f, 1.0f, 150.0f);

	//GLfloat scale(1.0f); // just to allow easy scaling of complete scene

	glm::mat4 modelview(1.0); // set base position for scene
	mvStack.push(modelview);

	initCamera();

	return projection;
}

void SceneManager::initCamera() {
	//init camera???
	at = player.getPos();
	eye = moveForward(at, -player.getPlayerR(), -8.0f);
	eye.y += 3.0;
	mvStack.top() = glm::lookAt(eye, at, up);
}

void SceneManager::init()
{
	shaderProgram = rt3d::initShaders("phong-tex.vert", "phong-tex.frag");
	textureProgram = rt3d::initShaders("textured.vert", "textured.frag");
	modelProgram = rt3d::initShaders("modelLoading.vert", "modelLoading.frag");

	foxModel = new Model("animations.FBX");

	rt3d::setLight(shaderProgram, lights[0]);
	rt3d::setMaterial(shaderProgram, materials[0]);

	//matching textureUnits
	GLuint uniformIndex = glGetUniformLocation(shaderProgram, "textureUnit0");
	glUniform1i(uniformIndex, 0);
	uniformIndex = glGetUniformLocation(shaderProgram, "textureUnit1");
	glUniform1i(uniformIndex, 1);
	uniformIndex = glGetUniformLocation(shaderProgram, "textureUnit2");
	glUniform1i(uniformIndex, 2);

	//skybox program needed in draw method
	//GLuint skyboxProgram = rt3d::initShaders("cubeMap.vert", "cubeMap.frag");

	//doesn't appear to be used anywhere else question mark question mark question mark
	//GLuint textureProgram = rt3d::initShaders("textured.vert", "textured.frag");

	//loading skybox
	const char *cubeTexFiles[6] = {
		"Town-skybox/grass1.bmp", "Town-skybox/grass1.bmp", "Town-skybox/grass1.bmp", "Town-skybox/grass1.bmp", "Town-skybox/grass1.bmp", "Town-skybox/grass1.bmp"
	};

	SDLManager::loadCubeMap(cubeTexFiles, skybox);

	std::vector<GLfloat> verts;
	std::vector<GLfloat> norms;
	std::vector<GLfloat> tex_coords;
	std::vector<GLuint> indices;
	rt3d::loadObj("cube.obj", verts, norms, tex_coords, indices);
	meshIndexCount = indices.size();

	meshObjects.push_back(rt3d::createMesh(verts.size() / 3,
		verts.data(), nullptr, norms.data(),
		tex_coords.data(), meshIndexCount,
		indices.data()));

	verts.clear();
	norms.clear();
	tex_coords.clear();
	indices.clear();

	rt3d::loadObj("fox.obj", verts, norms, tex_coords, indices);
	meshIndexCount = indices.size();
	player.setMeshIndexCount(indices.size());

	meshObjects.push_back(rt3d::createMesh(verts.size() / 3,
		verts.data(), nullptr, norms.data(),
		tex_coords.data(), meshIndexCount,
		indices.data()));

	verts.clear();
	norms.clear();
	tex_coords.clear();
	indices.clear();

	rt3d::loadObj("CartoonTree.obj", verts, norms, tex_coords, indices);
	meshIndexCount = indices.size();

	meshObjects.push_back(rt3d::createMesh(verts.size() / 3,
		verts.data(), nullptr, norms.data(),
		tex_coords.data(), meshIndexCount,
		indices.data()));

	//add more meshes with rt3d::loadObj("*.obj", verts, norms, tex_coords, indices); where * is the obj name
	//then meshIndexCount = indices.size();
	//then meshObjects.push_back(rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), meshIndexCount, indices.data()));

	textures.push_back(SDLManager::loadBitmap("fabric.bmp"));
	textures.push_back(SDLManager::loadBitmap("water.bmp"));
	//add more textures with textures.push_back(SDLManager::loadBitmap("*.bmp")); where * is the bitmap name

	initGameObjects();
	initPlayer();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void SceneManager::initGameObjects() {
	gameObjects.push_back(GameObject("Ground", glm::vec3(-5.0f, -0.1f, -100.0f), glm::vec3(20.0f, 0.1f, 200.0f), textures[0], meshObjects[0]));

	gameObjects.push_back(GameObject("Water", glm::vec3(-5.0f, 0.0f, -100.0f), glm::vec3(20.0f, 0.1f, 50.0f), textures[1], meshObjects[0]));

	gameObjects.push_back(GameObject("Cube1", glm::vec3(-5.0f, 1.0f, -50.0f), glm::vec3(5.0f, 1.0f, 5.0f), textures[0], meshObjects[0]));
	gameObjects.push_back(GameObject("Cube2", glm::vec3(-5.0f, 1.0f, -60.0f), glm::vec3(1.0f, 2.0f, 1.0f), textures[0], meshObjects[0]));
	gameObjects.push_back(GameObject("Cube3", glm::vec3(0.0f, 2.0f, -66.0f), glm::vec3(1.5f, 2.0f, 1.0f), textures[0], meshObjects[0]));
	gameObjects.push_back(GameObject("Cube4", glm::vec3(-4.0f, 1.0f, -75.0f), glm::vec3(1.5f, 2.0f, 1.0f), textures[0], meshObjects[0]));
	gameObjects.push_back(GameObject("Cube5", glm::vec3(-8.0f, 1.5f, -85.0f), glm::vec3(1.5f, 2.0f, 1.0f), textures[0], meshObjects[0]));
	gameObjects.push_back(GameObject("Cube6", glm::vec3(-10.0f, 2.0f, -96.0f), glm::vec3(1.5f, 3.0f, 1.0f), textures[0], meshObjects[0]));
	gameObjects.push_back(GameObject("Cube7", glm::vec3(-2.0f, 1.0f, -105.0f), glm::vec3(1.5f, 2.0f, 1.0f), textures[0], meshObjects[0]));
	gameObjects.push_back(GameObject("Cube8", glm::vec3(-2.0f, 1.5f, -120.0f), glm::vec3(2.0f, 2.0f, 1.0f), textures[0], meshObjects[0]));
	gameObjects.push_back(GameObject("Cube9", glm::vec3(-9.0f, 1.0f, -127.0f), glm::vec3(2.0f, 2.0f, 1.0f), textures[0], meshObjects[0]));
	gameObjects.push_back(GameObject("Cube10", glm::vec3(-5.0f, 2.0f, -137.0f), glm::vec3(2.0f, 2.0f, 1.0f), textures[0], meshObjects[0]));
	gameObjects.push_back(GameObject("Cube11", glm::vec3(-1.0f, 1.0f, -145.0f), glm::vec3(2.0f, 2.0f, 1.0f), textures[0], meshObjects[0]));

	gameObjects.push_back(GameObject("Tree1", glm::vec3(2.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), textures[0], meshObjects[2]));

	//add more game objects with gameObjects.push_back(GameObject("Name", position, scale, texture from textures, mesh from meshObjects)); 
}

void SceneManager::initPlayer() {
	player.setPlayerTexture(textures[0]);
	player.setPlayerMesh(meshObjects[1]);
}

void SceneManager::setShaderProjection(glm::mat4 projection)
{
	glUseProgram(shaderProgram);
	rt3d::setUniformMatrix4fv(shaderProgram, "projection", glm::value_ptr(projection));
}

void SceneManager::setLights()
{
	glm::vec4 tmp = mvStack.top()*lightPos;
	lights[0].position[0] = tmp.x;
	lights[0].position[1] = tmp.y;
	lights[0].position[2] = tmp.z;
	rt3d::setLightPos(shaderProgram, glm::value_ptr(tmp));
}

void SceneManager::renderObjects()
{
	for (int i = 0; i < gameObjects.size(); i++)
		renderObject(gameObjects[i]);

	
	//foxModel.Draw();

	renderPlayer();
}

void SceneManager::renderObject(GameObject gObj)
{
	glBindTexture(GL_TEXTURE_2D, gObj.getTexture());
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), gObj.getPos());
	mvStack.top() = glm::scale(mvStack.top(), gObj.getScale());
	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	//rt3d::setMaterial(shaderProgram, materials[0]); // don't use materials??????
	rt3d::drawIndexedMesh(gObj.getMesh(), meshIndexCount, GL_TRIANGLES);
	mvStack.pop();
}

void SceneManager::renderPlayer()
{
	//player cube
	//glBindTexture(GL_TEXTURE_2D, player.getTexture());
	//mvStack.push(mvStack.top());
	//mvStack.top() = glm::translate(mvStack.top(), glm::vec3(player.getPos().x, player.getPos().y, player.getPos().z));
	//mvStack.top() = glm::rotate(mvStack.top(), float(player.getPlayerR()*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
	//mvStack.top() = glm::rotate(mvStack.top(), float(180 * DEG_TO_RADIAN), glm::vec3(1.0f, 0.0f, 0.0f));
	//mvStack.top() = glm::rotate(mvStack.top(), float(180 * DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
	//rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	////rt3d::setMaterial(shaderProgram, materials[1]);
	//rt3d::drawIndexedMesh(player.getMesh(), player.getMeshIndexCount(), GL_TRIANGLES);
	//mvStack.pop();



}

void SceneManager::updatePlayerR(GLfloat deltaR)
{
	player.setPlayerR(player.getPlayerR() + deltaR);
}

void SceneManager::movePlayerForward(GLfloat delta) {
	glm::vec3 temp = player.getPos();

	player.setPos(moveForward(player.getPos(), -player.getPlayerR(), delta / getTimeScalar()));

	if (checkCollisions())
		player.setPos(temp);
}

void SceneManager::movePlayerRight(GLfloat delta) {
	glm::vec3 temp = player.getPos();

	player.setPos(moveRight(player.getPos(), -player.getPlayerR(), delta / getTimeScalar()));

	if (checkCollisions())
		player.setPos(temp);
}

glm::vec3 SceneManager::moveForward(glm::vec3 pos, GLfloat angle, GLfloat d)
{
	return glm::vec3(pos.x + d*std::sin(angle*DEG_TO_RADIAN), pos.y, pos.z - d*std::cos(angle*DEG_TO_RADIAN));
}

glm::vec3 SceneManager::moveRight(glm::vec3 pos, GLfloat angle, GLfloat d)
{
	return glm::vec3(pos.x + d*std::cos(angle*DEG_TO_RADIAN), pos.y, pos.z + d*std::sin(angle*DEG_TO_RADIAN));
}

bool SceneManager::checkCollisions()
{
	bool collided = false;
	for (int i = 0; i < gameObjects.size(); i++) {
		if (CollisionDetector::detectCollision(gameObjects[i], player))
			collided = true;
	}
	return collided;
}

bool SceneManager::checkCollisions(GameObject &specObj) {
	bool collided = false;
	for (int i = 0; i < gameObjects.size(); i++) {
		if (CollisionDetector::detectCollision(gameObjects[i], specObj))
			collided = true;
	}
	return collided;
}

void SceneManager::playerJump() {
	if (player.getJumpCounter() < player.getJumpMax()) {
		player.jump(true);
		glm::vec3 newPos = player.getPos();

		newPos.y += player.getJumpIncrement();

		player.incrementJumpCounter();
		player.setPos(newPos);
	}
	else if (player.getJumpCounter() >= player.getJumpMax()) {
		player.jump(false);
		if (player.isOnObject())
			player.resetJumpCounter();
	}
}

void SceneManager::playerFall() {

	if (CollisionDetector::detectCollision(getGameObject("Cube1"), player))
		std::cout << "player collided with cube1\n";

	if (!checkCollisions()) {
		glm::vec3 newPos = player.getPos();
		newPos.y -= player.getFallIncrement();

		GameObject tempObj(player);

		tempObj.setPos(newPos);

		if (!checkCollisions(tempObj)) { player.setIsOnObj(false);}
	}

	if (!player.isJumping() && !player.isOnObject()) {

		glm::vec3 newPos = player.getPos();

		if (checkCollisions()) {
			//change player y to y of object y plus object scale
			GameObject gObj = getGameObject(player.getLastCollision());
			std::cout << "player last coll:\t " << player.getLastCollision() << std::endl;
			newPos.y = gObj.getPos().y + gObj.getScale().y + player.getScale().y;

			player.setIsOnObj(true);
		}
		else {
			newPos.y -= player.getFallIncrement();
		}

		std::cout << "player y:\t" << player.getPos().y << std::endl;
		player.setPos(newPos);
		//CHANGE THIS ^^^^^ CREATE TEMP OBJECT ////MAYBE?????? LOLIDK
	}
}

GameObject SceneManager::getGameObject(std::string objName) {
	for (GameObject gObj : gameObjects) {
		if (gObj.getName() == objName)
			return gObj;
	}
	return GameObject("null", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0, 0);
}

void SceneManager::setPlayerJumpFalse()
{
	player.jump(false);
	player.maxJumpCounter();
}

double SceneManager::getTimeScalar() {
	static unsigned int lastTicks = 0;

	int ticks = SDL_GetTicks();
	int ticksSince = ticks - lastTicks;

	double scalar = (double)ticksSince / (double) 60.0f;

	lastTicks = ticks;

	if (scalar < 0.2 || scalar > 1.0)
		scalar = 0.28333;

	return scalar;
}
