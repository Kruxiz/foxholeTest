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

	//menus
	auto mainMenuDisplay = std::make_tuple("Main menu", glm::vec3(0.0f, 0.9f, 0.0f), glm::vec3(0.2f, 0.2f, 0.0f));
	auto playOption = std::make_tuple("Play[Enter]", glm::vec3(0.0f, 0.6f, 0.0f), glm::vec3(0.1f, 0.1f, 0.0f));
	auto scoresOption = std::make_tuple("Scores[S]", glm::vec3(0.0f, 0.4f, 0.0f), glm::vec3(0.1f, 0.1f, 0.0f));
	auto controlsOption = std::make_tuple("Controls[C]", glm::vec3(0.0f, 0.2f, 0.0f), glm::vec3(0.1f, 0.1f, 0.0f));
	auto quitOption = std::make_tuple("Quit[Esc]", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.0f));

	Menu mainMenu({ mainMenuDisplay, playOption, scoresOption, controlsOption, quitOption });

	menus.insert({ "mainMenu", mainMenu });


	auto scoresDisplay = std::make_tuple("Scores", glm::vec3(0.0f, 0.9f, 0.0f), glm::vec3(0.2f, 0.2f, 0.0f));
	auto mainMenuOption = std::make_tuple("Main menu[Backspace]", glm::vec3(0.8f, 0.8f, 0.0f), glm::vec3(0.2f, 0.1f, 0.0f));

	Menu scores({ scoresDisplay, mainMenuOption });

	menus.insert({ "scores", scores });



	auto pauseDisplay = std::make_tuple("Pause", glm::vec3(0.0f, 0.7f, 0.0f), glm::vec3(0.45f, 0.45f, 0.0f));

	Menu pause({ pauseDisplay, mainMenuOption });

	menus.insert({ "pause", pause });




	auto controlsDisplay = std::make_tuple("Controls", glm::vec3(0.0f, 0.9f, 0.0f), glm::vec3(0.2f, 0.2f, 0.0f));
	auto movementControl = std::make_tuple("WASD - movement", glm::vec3(0.0f, 0.6f, 0.0f), glm::vec3(0.2f, 0.1f, 0.0f));
	auto lookControl = std::make_tuple("Mouse - look", glm::vec3(0.0f, 0.4f, 0.0f), glm::vec3(0.2f, 0.1f, 0.0f));
	auto pauseControl = std::make_tuple("Tab - pause", glm::vec3(0.0f, 0.2f, 0.0f), glm::vec3(0.2f, 0.1f, 0.0f));
	auto jumpControl = std::make_tuple("Space - jump", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.1f, 0.0f));

	Menu controls({ controlsDisplay, movementControl, lookControl, pauseControl, jumpControl, mainMenuOption });

	menus.insert({ "controls", controls });

	sceneState = MAIN_MENU;
}

void SceneManager::play()
{
	BASS_Start();
	if (sceneState != PAUSE) {
		timer = std::chrono::system_clock::now();
		pauseTimer = timer;

		HCHANNEL ch = BASS_SampleGetChannel(sounds[0], TRUE); //todo true i think??
		if (!BASS_ChannelPlay(ch, TRUE))
			std::cout << "Can't play sample - " << BASS_ErrorGetCode() << std::endl;

		level = 1;
		initGameObjects();
		respawnPlayer();
	}
	else {
		HCHANNEL ch = BASS_SampleGetChannel(sounds[0], FALSE); //todo true i think??
		if (!BASS_ChannelPlay(ch, TRUE))
			std::cout << "Can't play sample - " << BASS_ErrorGetCode() << std::endl;

	}

	if (!inCountdown())
		sceneState = IN_GAME;

}

bool SceneManager::inCountdown()
{

	int countdownTimer = std::chrono::duration<double>(std::chrono::system_clock::now() - timer).count();

	if (!paused()) {
		if (countdownTimer < 3) {
			countdown();
		}
		else {
			//;//sceneState = IN_GAME; //todo make mechanism to end countdown
			if (sceneState == COUNTDOWN) {
				sceneState = IN_GAME;
			}
		}
	}
	return sceneState == COUNTDOWN;
}

void SceneManager::mainMenu()
{
	/*if (sceneState == PAUSE) {
		HCHANNEL ch = BASS_SampleGetChannel(sounds[0], TRUE);
		BASS_ChannelPause(ch);
	}*/

	sceneState = MAIN_MENU;

}

void SceneManager::renderMenus() {
	Menu menu;
	switch (sceneState) {
	case MAIN_MENU:
		menu = menus.at("mainMenu");

		for (auto menuObj : menu) {
			renderHUDObject(menuObj);
		}

		break;
	case PAUSE:
		menu = menus.at("pause");

		for (auto menuObj : menu) {
			renderHUDObject(menuObj);
		}

		break;
	case SCORES:
		menu = menus.at("scores");

		for (auto menuObj : menu) {
			renderHUDObject(menuObj);
		}

		break;
	case CONTROLS:
		menu = menus.at("controls");

		for (auto menuObj : menu) {
			renderHUDObject(menuObj);
		}

		break;
	default://todo maybe add something else here
		break;

	}
}

void SceneManager::addToScores() {
	//todo addToScores - only top 5?
	glm::vec3 pos(0.6f, 0.5f, 0.0f);
	glm::vec3 scale(0.2f, 0.2f, 0.0f);
	float step = 0.1f;
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
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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

	initSounds(); //todo play menu music - play forest sounds when playing game

	shaderProgram = rt3d::initShaders("phong-tex.vert", "phong-tex.frag");
	//textureProgram = rt3d::initShaders("textured.vert", "textured.frag");
	//modelProgram = rt3d::initShaders("modelLoading.vert", "modelLoading.frag");

	//foxModel = new Model("fox.obj");

	rt3d::setLight(shaderProgram, lights[0]);
	rt3d::setMaterial(shaderProgram, materials[0]);

	////matching textureUnits
	//GLuint uniformIndex = glGetUniformLocation(shaderProgram, "textureUnit0");
	//glUniform1i(uniformIndex, 0);
	//uniformIndex = glGetUniformLocation(shaderProgram, "textureUnit1");
	//glUniform1i(uniformIndex, 1);
	//uniformIndex = glGetUniformLocation(shaderProgram, "textureUnit2");
	//glUniform1i(uniformIndex, 2);
	//uniformIndex = glGetUniformLocation(shaderProgram, "textureUnit3");
	//glUniform1i(uniformIndex, 3);
	//uniformIndex = glGetUniformLocation(shaderProgram, "textureUnit4");
	//glUniform1i(uniformIndex, 4);
	//^^ above is nonsense

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
	textures.push_back(SDLManager::loadBitmap("tree.bmp"));
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
	//timer = std::chrono::system_clock::now(); //todo init timer when starting to actually play game
	//pauseTimer = timer;

	if (level == 1) {
		//level 1

		gameObjects.push_back(GameObject("LevelEnd", glm::vec3(0.0f, 0.0f, -180.f), glm::vec3(25.0f, 20.0f, 5.0f), NULL, NULL));
		gameObjects.push_back(GameObject("Ground", glm::vec3(-5.0f, -0.1f, -100.0f), glm::vec3(25.0f, 0.1f, 200.0f), textures[4], meshObjects[0]));

		gameObjects.push_back(GameObject("Water", glm::vec3(-5.0f, 0.0f, -100.0f), glm::vec3(20.0f, 0.1f, 50.0f), textures[1], meshObjects[0]));

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

		//gameObjects.push_back(GameObject("Tree1", glm::vec3(2.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), textures[6], meshObjects[2]));
		buildTrees();

	}
	else if (level == 2) {
		// level 2
		//hole-y ground
		gameObjects.push_back(GameObject("trap_ground", glm::vec3(0.0f, 0.2f, 1.5f), glm::vec3(5.0f, 1.0f, 5.0f), textures[4], meshObjects[0]));
		for (int a = 0; a < 99; a++) {

			int randomNum1 = rand() % 100 + 1;
			int randomNum2 = rand() % 100 + 1;
			gameObjects.push_back(GameObject("trap_ground", glm::vec3(randomNum1, 0.2f, randomNum2), glm::vec3(5.0f, 1.0f, 5.0f), textures[4], meshObjects[0]));

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
		collectables = 10;
	}




	//add more game objects with gameObjects.push_back(GameObject("Name", position, scale, texture from textures, mesh from meshObjects)); 
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

void SceneManager::checkSwitchLevel()
{
	if (level == 2 && collectables == 0) {
		level = 1;
		timer = std::chrono::system_clock::now();
		pauseTimer = timer;
		initGameObjects();
		respawnPlayer();
	}
}

void SceneManager::buildTrees() {

	glm::vec3 treePos(16.0f, 1.0f, 1.0f);
	glm::vec3 treeScale(1.0f, 5.0f, 1.0f);
	std::string treeName("Tree");
	for (int i = 0; i < 20; i++) {
		treeName.append(std::to_string((i * 1) + 1));

		gameObjects.push_back(GameObject(treeName, treePos, treeScale, textures[6], meshObjects[2]));

		treeName = "Tree";
		treeName.append(std::to_string((i * 2) + 2));
		treePos.x = -26.0f;
		treePos.z -= (i + 1);

		gameObjects.push_back(GameObject(treeName, treePos, treeScale, textures[6], meshObjects[2]));

		treeName = "Tree";
		treePos.x = 16.0f;

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
	renderPlayer(); //NO DIFFERENCE BETWEEN THIS AND RENDER OBJECT
	renderHUD();
}

void SceneManager::renderObject(GameObject gObj)
{
	glBindTexture(GL_TEXTURE_2D, gObj.getTexture());
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), gObj.getPos());
	mvStack.top() = glm::scale(mvStack.top(), gObj.getScale());
	mvStack.top() = glm::rotate(mvStack.top(), float(0 * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
	mvStack.top() = glm::rotate(mvStack.top(), float(180 * DEG_TO_RADIAN), glm::vec3(1.0f, 0.0f, 0.0f));
	mvStack.top() = glm::rotate(mvStack.top(), float(180 * DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	//rt3d::setMaterial(shaderProgram, materials[0]); // don't use materials??????
	rt3d::drawIndexedMesh(gObj.getMesh(), meshIndexCount, GL_TRIANGLES);
	mvStack.pop();


}

void SceneManager::togglePause() {

	if (sceneState == PAUSE) {
		//sceneState = IN_GAME;
		play();
	}
	else if (sceneState == IN_GAME ^ sceneState == COUNTDOWN) {
		sceneState = PAUSE;
	}

	if (sceneState == PAUSE) {
		pauseTime = 0;
		pauseTimer = std::chrono::system_clock::now();
		std::cout << std::chrono::duration<double>(pauseTimer - timer).count() << std::endl;
		BASS_Pause();
		/*HCHANNEL ch = BASS_SampleGetChannel(sounds[0], FALSE);
		if (!BASS_ChannelPause(ch))
			std::cout << "Can't pause sample - " << BASS_ErrorGetCode() << std::endl;
		else
			std::cout << "Pausing sample\n";*/
	}
	else if (sceneState == IN_GAME) {
		pauseTime = std::chrono::duration<double>(std::chrono::system_clock::now() - pauseTimer).count();
		std::cout << std::chrono::duration<double>(pauseTimer - timer).count() << std::endl;
		timer += std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::duration<double>(std::chrono::system_clock::now() - pauseTimer));

		//BASS_Start();
		//HCHANNEL ch = BASS_SampleGetChannel(sounds[0], FALSE);
		//BASS_ChannelPlay(ch, FALSE);
	}
}

void SceneManager::renderHUDObject(MenuObject menuObj) {
	GLuint label = 0;
	label = textToTexture(std::get<0>(menuObj).c_str(), label);

	glBindTexture(GL_TEXTURE_2D, label);

	mvStack.push(glm::mat4(1.0));
	mvStack.top() = glm::translate(mvStack.top(), std::get<1>(menuObj));
	mvStack.top() = glm::scale(mvStack.top(), std::get<2>(menuObj));
	rt3d::setUniformMatrix4fv(shaderProgram, "projection", glm::value_ptr(glm::mat4(1.0)));
	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));

	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();
}

void SceneManager::renderHUD()
{
	//todo add countdown for death respawn and initial play

	////////////////////////////////////////////////////////////////////
	//This renders a HUD label
	////////////////////////////////////////////////////////////////////
	glUseProgram(shaderProgram);//Use texture-only shader for text rendering
	glDisable(GL_DEPTH_TEST);//Disable depth test for HUD label

	if (sceneState == PAUSE) {
		renderMenus();
	}
	auto temp = std::chrono::system_clock::now();

	double totalTime;

	if (sceneState == PAUSE) {
		totalTime = std::chrono::duration<double>(pauseTimer - timer).count();
	}
	else {
		totalTime = std::chrono::duration<double>(temp - timer).count();
	}

	if (inCountdown()) {

		int countdownTimer = (int)totalTime;

		if (countdownTimer == 0) {
			std::cout << "3\n";
			countdownTimer = 3;
		}
		else if (countdownTimer == 1) {
			std::cout << "2\n";
			countdownTimer = 2;
		}
		else if (countdownTimer == 2) {
			std::cout << "1\n";
			countdownTimer = 1;
		}
		else if (countdownTimer == 3) {
			std::cout << "GO!\n";
		}
		else {
			std::cout << "still in countdown for some reason\n";
		}
		std::string countdownStr(std::to_string(countdownTimer));

		renderHUDObject(std::make_tuple(countdownStr, glm::vec3(0.0f, -0.1f, 0.0f), glm::vec3(0.25f, 0.25f, 0.0f)));

	}
	else {
		totalTime -= 3;

		std::string timerStr = "Time: ";

		if (totalTime < 0)
			totalTime = 0;

		timerStr.append(std::to_string(totalTime));

		timerStr.append("s");

		renderHUDObject(std::make_tuple(timerStr, glm::vec3(-0.8f, 0.8f, 0.0f), glm::vec3(0.2f, 0.2f, 0.0f)));

	}

	if (level == 2) {
		std::string collectablesString("Collectables left: ");
		collectablesString.append(std::to_string(collectables));

		renderHUDObject(std::make_tuple(collectablesString, glm::vec3(-0.5f, -0.8f, 0.0f), glm::vec3(0.5f, 0.2f, 0.0f)));

	}

	int respawnTime = std::chrono::duration<double>(temp - respawnTimer).count();

	if (respawnTime > -1 && respawnTime < 2) {
		renderHUDObject(std::make_tuple("Avoid the water!", glm::vec3(0.8f, -0.8f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f)));
	}

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
					collectables--;
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
	int flag = BASS_ChannelFlags(ch, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
	std::cout << flag << std::endl;
	/*if (!BASS_ChannelPlay(ch, FALSE))
		std::cout << "Can't play sample" << std::endl;*/
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
	player.setPlayerR(0.0f);

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

		respawnTimer = std::chrono::system_clock::now();
		respawnPlayer();
	}
	else if (player.getLastCollision() == "LevelEnd") {
		level = 2;
		timer = std::chrono::system_clock::now();
		pauseTimer = timer;
		initGameObjects();
		respawnPlayer();
	}
	else if (player.getPos().y < -10)
		respawnPlayer();
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
