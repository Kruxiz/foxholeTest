#include "SceneManager.h"


#define DEG_TO_RADIAN 0.017453293

SceneManager::SceneManager() {
	level = 1;
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
		{ 0.5f, 0.5f, 0.5f, 1.0f }, // diffuse
		{ 0.1f, 0.1f, 0.1f, 1.0f }, // specular
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
	if (level == 1) {
		glUseProgram(skyboxProgram);				// skybox as single cube using cube map
		rt3d::setUniformMatrix4fv(skyboxProgram, "projection", glm::value_ptr(projection));
		glUseProgram(skyboxProgram);
		glDepthMask(GL_FALSE); // make sure writing to update depth test is off	
		rt3d::setUniformMatrix4fv(skyboxProgram, "projection", glm::value_ptr(projection));
		glm::mat3 mvRotOnlyMat3 = glm::mat3(mvStack.top());
		glDepthMask(GL_FALSE); // make sure writing to update depth test is off
		mvStack.push(glm::mat4(mvRotOnlyMat3));
		/*glm::mat3*/ mvRotOnlyMat3 = glm::mat3(mvStack.top());
		glCullFace(GL_FRONT); // drawing inside of cube!
		mvStack.push(glm::mat4(mvRotOnlyMat3));
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox[0]);
		glCullFace(GL_FRONT); // drawing inside of cube!
		mvStack.top() = glm::scale(mvStack.top(), glm::vec3(1.5f, 1.5f, 1.5f));	
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox[0]);
		rt3d::setUniformMatrix4fv(skyboxProgram, "modelview", glm::value_ptr(mvStack.top()));
		mvStack.top() = glm::scale(mvStack.top(), glm::vec3(1.5f, 1.5f, 1.5f));
		rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
		rt3d::setUniformMatrix4fv(skyboxProgram, "modelview", glm::value_ptr(mvStack.top()));
		mvStack.pop();
		rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
		glCullFace(GL_BACK); // drawing inside of cube!
		mvStack.pop();
							 // back to remainder of rendering
		glCullFace(GL_BACK); // drawing inside of cube!

		//glDepthMask(GL_TRUE); // make sure depth test is on									 // back to remainder of rendering
		glDepthMask(GL_TRUE); // make sure depth test is on
	}
	else if (level == 2) {
		// skybox as single cube using cube map
		glUseProgram(skyboxProgram);
		rt3d::setUniformMatrix4fv(skyboxProgram, "projection", glm::value_ptr(projection));
		glDepthMask(GL_FALSE); // make sure writing to update depth test is off
		glm::mat3 mvRotOnlyMat3 = glm::mat3(mvStack.top());
		mvStack.push(glm::mat4(mvRotOnlyMat3));
		glCullFace(GL_FRONT); // drawing inside of cube!
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox[1]);
		mvStack.top() = glm::scale(mvStack.top(), glm::vec3(1.5f, 1.5f, 1.5f));
		rt3d::setUniformMatrix4fv(skyboxProgram, "modelview", glm::value_ptr(mvStack.top()));
		rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
		mvStack.pop();
		glCullFace(GL_BACK); // drawing inside of cube!
							 // back to remainder of rendering
		glDepthMask(GL_TRUE); // make sure depth test is on
	}
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

void SceneManager::initTTF() {
	// set up TrueType / SDL_ttf
	if (TTF_Init() == -1)
		std::cout << "TTF failed to initialise." << std::endl;

	textFont = TTF_OpenFont("MavenPro-Regular.ttf", 24);
	if (textFont == NULL)
		std::cout << "Failed to open font." << std::endl;
}

void SceneManager::init()
{

	initTTF();

	initSounds();

	shaderProgram = rt3d::initShaders("phong-tex.vert", "phong-tex.frag");
	//textureProgram = rt3d::initShaders("textured.vert", "textured.frag");
	//modelProgram = rt3d::initShaders("modelLoading.vert", "modelLoading.frag");

	//foxModel = new Model("fox.obj");

	rt3d::setLight(shaderProgram, lights[0]);
	rt3d::setMaterial(shaderProgram, materials[0]);

	//matching textureUnits
	GLuint uniformIndex = glGetUniformLocation(shaderProgram, "textureUnit0");
	glUniform1i(uniformIndex, 0);
	uniformIndex = glGetUniformLocation(shaderProgram, "textureUnit1");
	glUniform1i(uniformIndex, 1);
	uniformIndex = glGetUniformLocation(shaderProgram, "textureUnit2");
	glUniform1i(uniformIndex, 2);
	uniformIndex = glGetUniformLocation(shaderProgram, "textureUnit3");
	glUniform1i(uniformIndex, 3);
	uniformIndex = glGetUniformLocation(shaderProgram, "textureUnit4");
	glUniform1i(uniformIndex, 4);

	//skybox program needed in draw method
	//GLuint skyboxProgram = rt3d::initShaders("cubeMap.vert", "cubeMap.frag");

	//doesn't appear to be used anywhere else question mark question mark question mark
	GLuint textureProgram = rt3d::initShaders("textured.vert", "textured.frag");

	//loading skybox
	const char *cubeTexFiles[6] = {
		"Town-skybox/grass1.bmp", "Town-skybox/grass1.bmp", "Town-skybox/grass1.bmp", "Town-skybox/grass1.bmp", "Town-skybox/grass1.bmp", "Town-skybox/grass1.bmp"
	};

	SDLManager::loadCubeMap(cubeTexFiles, skybox);
	const char *cubeTexFiles2[6] = {
		"Town-skybox/Town_up.bmp", "Town-skybox/Town_up.bmp", "Town-skybox/Town_up.bmp", "Town-skybox/Town_up.bmp", "Town-skybox/Town_up.bmp", "Town-skybox/Town_up.bmp"
	};
	SDLManager::loadCubeMap(cubeTexFiles2, skybox);

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
	textures.push_back(SDLManager::loadBitmap("box.bmp"));
	textures.push_back(SDLManager::loadBitmap("twigs.bmp"));
	textures.push_back(SDLManager::loadBitmap("Town-skybox/grass1.bmp"));
	textures.push_back(SDLManager::loadBitmap("orange-fox.bmp"));
	//add more textures with textures.push_back(SDLManager::loadBitmap("*.bmp")); where * is the bitmap name

	initGameObjects();
	initPlayer();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}



void SceneManager::initGameObjects() {
	gameObjects.clear();
	gameObjects.shrink_to_fit();
	timer = std::chrono::steady_clock::now();

	if (level == 1) {
		//level 1

		gameObjects.push_back(GameObject("LevelEnd", glm::vec3(0.0f, 0.0f, -180.f), glm::vec3(25.0f, 2.0f, 5.0f), NULL, NULL));
		gameObjects.push_back(GameObject("Ground", glm::vec3(-5.0f, -0.1f, -100.0f), glm::vec3(25.0f, 0.1f, 200.0f), textures[4], meshObjects[0]));

		gameObjects.push_back(GameObject("Water", glm::vec3(-5.0f, 0.0f, -100.0f), glm::vec3(25.0f, 0.1f, 50.0f), textures[1], meshObjects[0]));

		gameObjects.push_back(GameObject("Cube1", glm::vec3(-5.0f, 1.0f, -50.0f), glm::vec3(5.0f, 1.0f, 5.0f), textures[2], meshObjects[0]));
		gameObjects.push_back(GameObject("Cube2", glm::vec3(-5.0f, 1.0f, -60.0f), glm::vec3(1.0f, 2.0f, 1.0f), textures[2], meshObjects[0]));
		gameObjects.push_back(GameObject("Cube3", glm::vec3(0.0f, 2.0f, -66.0f), glm::vec3(1.5f, 2.0f, 1.0f), textures[2], meshObjects[0]));
		gameObjects.push_back(GameObject("Cube4", glm::vec3(-4.0f, 1.0f, -75.0f), glm::vec3(1.5f, 2.0f, 1.0f), textures[2], meshObjects[0]));
		gameObjects.push_back(GameObject("Cube5", glm::vec3(-8.0f, 1.5f, -85.0f), glm::vec3(1.5f, 2.0f, 1.0f), textures[2], meshObjects[0]));
		gameObjects.push_back(GameObject("Cube6", glm::vec3(-10.0f, 2.0f, -96.0f), glm::vec3(1.5f, 3.0f, 1.0f), textures[2], meshObjects[0]));
		gameObjects.push_back(GameObject("Cube7", glm::vec3(-2.0f, 1.0f, -105.0f), glm::vec3(1.5f, 2.0f, 1.0f), textures[2], meshObjects[0]));
		gameObjects.push_back(GameObject("Cube8", glm::vec3(-2.0f, 1.5f, -120.0f), glm::vec3(2.0f, 2.0f, 1.0f), textures[2], meshObjects[0]));
		gameObjects.push_back(GameObject("Cube9", glm::vec3(-9.0f, 1.0f, -127.0f), glm::vec3(2.0f, 2.0f, 1.0f), textures[2], meshObjects[0]));
		gameObjects.push_back(GameObject("Cube10", glm::vec3(-5.0f, 2.0f, -137.0f), glm::vec3(2.0f, 2.0f, 1.0f), textures[2], meshObjects[0]));
		gameObjects.push_back(GameObject("Cube11", glm::vec3(-1.0f, 1.0f, -145.0f), glm::vec3(2.0f, 2.0f, 1.0f), textures[2], meshObjects[0]));

		gameObjects.push_back(GameObject("Tree1", glm::vec3(2.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), textures[0], meshObjects[2]));
		buildTrees();

	}
	else if (level == 2) {
		// level 2
		//hole-y ground
		for (int a = 0; a < 100; a++) {

			int randomNum1 = rand() % 100 + 1;
			int randomNum2 = rand() % 100 + 1;
			gameObjects.push_back(GameObject("trap_ground", glm::vec3(randomNum1, 0.0f, randomNum2), glm::vec3(5.0f, 1.0f, 5.0f), textures[4], meshObjects[0]));

		}

		//collectables
		std::string collectableId("collectable");
		for (int b = 0; b < 10; b++) {
			collectableId.append(std::to_string(b + 1));
			int randomNum3 = rand() % 100 + 1;
			int randomNum4 = rand() % 100 + 1;
			gameObjects.push_back(GameObject(collectableId, glm::vec3(randomNum3, 4.0f, randomNum4), glm::vec3(0.5f, 0.5f, 0.5f), textures[3], meshObjects[0]));
			collectableId = "collectable";
		}
	}




	//add more game objects with gameObjects.push_back(GameObject("Name", position, scale, texture from textures, mesh from meshObjects)); 
}

void SceneManager::sound()
{
	//*****declairation*******
	//HSAMPLE *samples = NULL;
	//**************stand-alone******
	/*HSAMPLE loadSample(char * filename)
	{
	HSAMPLE sam;
	if (sam = BASS_SampleLoad(FALSE, filename, 0, 0, 3, BASS_SAMPLE_OVER_POS))
	cout << "sample " << filename << " loaded!" << endl;
	else
	{
	cout << "Can't load sample";
	exit(0);
	}
	return sam;
	}*/
	//*********************init********
	///* Initialize default output device */
	//if (!BASS_Init(-1, 44100, 0, 0, NULL))
	//	cout << "Can't initialize device";
	//samples = new HSAMPLE[2];
	//// Following comment is from source basstest file!
	///* Load a sample from "file" and give it a max of 3 simultaneous
	//playings using playback position as override decider */
	//samples[0] = loadSample("X.wav");  //background music (edit with 3/4)
	//samples[1] = loadSample("X.wav");  //single play sound for death/collectable - code at player--collition
	//	//ect..
	//cout << "Press 3 and 4 to stop/start music sounds." << endl;
	//**update****//
	/*if (keys[SDL_SCANCODE_4]) {
	BASS_Pause();
	}
	if (keys[SDL_SCANCODE_3]) {
	BASS_Start();
	}
	if (player==collition)
	{
	cout << "Collition with sound: "  << endl;
	HCHANNEL ch = BASS_SampleGetChannel(samples[1], FALSE);
	BASS_ChannelSetAttribute(ch, BASS_ATTRIB_FREQ, 0);
	BASS_ChannelSetAttribute(ch, BASS_ATTRIB_VOL, 0.5);
	BASS_ChannelSetAttribute(ch, BASS_ATTRIB_PAN, -1);
	BASS_Stop();
	}*/
}

// textToTexture
GLuint SceneManager::textToTexture(const char * str, GLuint textID) {
	GLuint texture = textID;
	TTF_Font * font = textFont;

	SDL_Surface * stringImage = TTF_RenderText_Blended(font, str, { 255, 255, 255 });

	if (stringImage == NULL) {
		std::cout << "String surface not created." << std::endl;
	}

	if (texture == 0) {
		glGenTextures(1, &texture);//This avoids memory leakage, only initialise //first time
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stringImage->w, stringImage->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, stringImage->pixels);
	glBindTexture(GL_TEXTURE_2D, NULL);

	SDL_FreeSurface(stringImage);
	return texture;
}

void SceneManager::textOnScreen() {
	//	//delaire
	//	TTF_Font * textFont;
	//	GLuint textToTexture(const char * str/*, TTF_Font *font, SDL_Color colour, GLuint &w,GLuint &h */);
	//	GLuint labels[11];
	//
	//
	//
	//
	//	//under textToTexture (predefined from labs)
	//	void clearTextTexture(GLuint textID) {
	//		if (textID != NULL) {
	//			glDeleteTextures(1, &textID);
	//		}
	//	}
	//
	//
	//
	//
	//
	//	//init()
	//	// set up TrueType / SDL_ttf
	//	if (TTF_Init() == -1)
	//		cout << "TTF failed to initialise." << endl;
	//
	//	textFont = TTF_OpenFont("FAFERS.ttf", 48);
	//	if (textFont == NULL)
	//		cout << "Failed to open font." << endl;
	//
	//	labels[0] = 0;
	//
	//
	//
	//
	//
	//
	//	//draw/objectManagers()	
	//
	//#ifndef label
	//#define label
	//	//point label
	//	if (point == 0)
	//	{
	//		glUseProgram(skyboxProgram);//Use texture-only shader for text rendering
	//		glDisable(GL_DEPTH_TEST);//Disable depth test for HUD label
	//		labels[0] = textToTexture(" 0/0 ", labels[0]);
	//		glBindTexture(GL_TEXTURE_2D, labels[0]);
	//		mvStack.push(glm::mat4(1.0));
	//		mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-0.8f, 0.8f, 0.0f));
	//		mvStack.top() = glm::scale(mvStack.top(), glm::vec3(0.20f, 0.2f, 0.0f));
	//		rt3d::setUniformMatrix4fv(skyboxProgram, "projection", glm::value_ptr(glm::mat4(1.0)));
	//		rt3d::setUniformMatrix4fv(skyboxProgram, "modelview", glm::value_ptr(mvStack.top()));
	//
	//		rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	//		mvStack.pop();
	//	}
	//	else if (point == 1)
	//	{
	//		glUseProgram(skyboxProgram);//Use texture-only shader for text rendering
	//		glDisable(GL_DEPTH_TEST);//Disable depth test for HUD label
	//		labels[1] = textToTexture(" 1/10 ", labels[1]);
	//		glBindTexture(GL_TEXTURE_2D, labels[1]);
	//
	//		...
	//
	//			//3d text on ground
	//			glUseProgram(skyboxProgram);//Use texture-only shader for text rendering
	//		labels[0] = textToTexture(" End of Demo! ", labels[0]);
	//		glBindTexture(GL_TEXTURE_2D, labels[0]);
	//		mvStack.push(mvStack.top());
	//		mvStack.top() = glm::translate(mvStack.top(), glm::vec3(position.x, 2.0f, position.z));
	//		mvStack.top() = glm::scale(mvStack.top(), glm::vec3(1.0f, 1.0f, 0.0f));
	//		rt3d::setUniformMatrix4fv(skyboxProgram, "modelview", glm::value_ptr(mvStack.top()));
	//		rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	//		mvStack.pop();
	//	}
	//#endif label
}

void SceneManager::checkSwitchLevel()
{
	if (level == 2 && countCollectables() == 0) {
		level = 1;
		initGameObjects();
		respawnPlayer();
	}
}

void SceneManager::buildTrees() {

	glm::vec3 treePos(14.0f, 1.0f, 1.0f);
	glm::vec3 treeScale(1.0f, 1.0f, 1.0f);
	std::string treeName("Tree");
	for (int i = 0; i < 10; i++) {
		treeName.append(std::to_string((i * 1) + 1));

		gameObjects.push_back(GameObject(treeName, treePos, treeScale, textures[0], meshObjects[2]));

		treeName = "Tree";
		treeName.append(std::to_string((i * 2) + 2));
		treePos.x = -19.0f;
		treePos.z -= (i + 1) * 5;

		gameObjects.push_back(GameObject(treeName, treePos, treeScale, textures[0], meshObjects[2]));

		treeName = "Tree";
		treePos.x = 14.0f;

	}


}

void SceneManager::initPlayer() {
	player.setPlayerTexture(textures[5]);
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
	for (int i = 0; i < gameObjects.size(); i++) {
		if (gameObjects[i].getTexture() != NULL && gameObjects[i].getMesh() != NULL)
			renderObject(gameObjects[i]);
	}
	renderPlayer();
	renderHUD();
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

void SceneManager::renderHUD()
{
	////////////////////////////////////////////////////////////////////
	//This renders a HUD label
	////////////////////////////////////////////////////////////////////

	//timer = std::chrono::steady_clock::now();
	auto temp = std::chrono::steady_clock::now();
	auto totalTime = std::chrono::duration<double>(temp - timer).count();
	std::string timerStr = "Time: ";
	timerStr.append(std::to_string(totalTime));
	timerStr.append("s");

	glUseProgram(shaderProgram);//Use texture-only shader for text rendering
	glDisable(GL_DEPTH_TEST);//Disable depth test for HUD label
	labels[0] = textToTexture(timerStr.c_str(), labels[0]);
	glBindTexture(GL_TEXTURE_2D, labels[0]);
	mvStack.push(glm::mat4(1.0));
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-0.8f, 0.8f, 0.0f));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(0.20f, 0.2f, 0.0f));
	rt3d::setUniformMatrix4fv(shaderProgram, "projection", glm::value_ptr(glm::mat4(1.0)));
	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));

	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();
	glEnable(GL_DEPTH_TEST);//Re-enable depth test after HUD label

}

void SceneManager::renderPlayer()
{
	//player cube
	glBindTexture(GL_TEXTURE_2D, player.getTexture());
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(player.getPos().x, player.getPos().y, player.getPos().z));
	mvStack.top() = glm::rotate(mvStack.top(), float(player.getPlayerR()*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
	mvStack.top() = glm::rotate(mvStack.top(), float(180 * DEG_TO_RADIAN), glm::vec3(1.0f, 0.0f, 0.0f));
	mvStack.top() = glm::rotate(mvStack.top(), float(180 * DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	//rt3d::setMaterial(shaderProgram, materials[1]);
	rt3d::drawIndexedMesh(player.getMesh(), player.getMeshIndexCount(), GL_TRIANGLES);
	mvStack.pop();

	//foxModel->Draw(modelProgram);

}

void SceneManager::updatePlayerR(GLfloat deltaR)
{
	player.setPlayerR(player.getPlayerR() + deltaR);
}

void SceneManager::detectCollectableCollision() {

	if (level == 2) {
		std::string playerColl = player.getLastCollision();
		if (player.getLastCollision().substr(0, 11) == "collectable") {
			playerColl = playerColl.substr(11);

			for (GameObject gObj : gameObjects) {
				if (gObj.getName().substr(0, 11) == "collectable" && gObj.getName().substr(11) == playerColl) {
					std::cout << "player collided with collectable\n";
					int index = getGameObjectIndex(player.getLastCollision());
					player.setLastCollision("");
					gameObjects.erase(gameObjects.begin() + index);
					break;
				}
			}
		}
	}
}

int SceneManager::countCollectables() {
	int count = 0;
	for (GameObject obj : gameObjects) {
		if (obj.getName().substr(0, 11) == "collectable")
			count++;
	}
	return count;
}

HSAMPLE SceneManager::loadSounds(char * filename)
{
	HSAMPLE sam;
	if (sam = BASS_SampleLoad(FALSE, filename, 0, 0, 3, BASS_SAMPLE_OVER_POS))
		std::cout << "sample " << filename << " loaded!" << std::endl;
	else
	{
		std::cout << "Can't load sample";
		exit(0);
	}
	return sam;
}

void SceneManager::initSounds()
{
	/* Initialize default output device */
	if (!BASS_Init(-1, 44100, 0, 0, NULL))
		std::cout << "Can't initialize device";

	sounds.push_back(loadSounds("forest-environment.wav"));
	//http://soundbible.com/1818-Rainforest-Ambience.html

	sounds.push_back(loadSounds("splash.wav"));
	//http://soundbible.com/2100-Splash-Rock-In-Lake.html

	HCHANNEL ch = BASS_SampleGetChannel(sounds[0], FALSE);
	BASS_ChannelSetAttribute(ch, BASS_ATTRIB_FREQ, 0);
	BASS_ChannelSetAttribute(ch, BASS_ATTRIB_VOL, 0.5);
	BASS_ChannelSetAttribute(ch, BASS_ATTRIB_PAN, -1);
	BASS_ChannelFlags(ch, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
	if (!BASS_ChannelPlay(ch, FALSE))
		std::cout << "Can't play sample" << std::endl;
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

		if (!checkCollisions(tempObj)) { player.setIsOnObj(false); }
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

int SceneManager::getGameObjectIndex(std::string objName) {
	for (int i = 0; i < gameObjects.size(); i++) {
		if (gameObjects[i].getName() == objName)
			return i;
	}
	return -1;
}

void SceneManager::setPlayerJumpFalse()
{
	player.jump(false);
	player.maxJumpCounter();
}

void SceneManager::respawnPlayer() {
	std::cout << "respawn because: " << player.getLastCollision() << std::endl;
	player.reset();

}

void SceneManager::checkPlayerRespawn()
{
	if (player.getLastCollision() == "Water") {

		HCHANNEL ch = BASS_SampleGetChannel(sounds[1], FALSE);
		BASS_ChannelSetAttribute(ch, BASS_ATTRIB_FREQ, 0);
		BASS_ChannelSetAttribute(ch, BASS_ATTRIB_VOL, 0.5);
		BASS_ChannelSetAttribute(ch, BASS_ATTRIB_PAN, -1);

		if (!BASS_ChannelPlay(ch, FALSE))
			std::cout << "Can't play sample" << std::endl;

		respawnPlayer();
	}
	else if (player.getLastCollision() == "LevelEnd") {
		level = 2;
		initGameObjects();
		respawnPlayer();
	}
}

void SceneManager::freeBass()
{
	BASS_Free();
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
