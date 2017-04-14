#include "SceneManager.h"


#define DEG_TO_RADIAN 0.017453293

SceneManager::SceneManager() {
	level = 1;
	eye = glm::vec3(0.0f, 1.0f, 0.0f);
	at = glm::vec3(0.0f, 1.0f, -1.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	skyboxProgram = rt3d::initShaders("../FoxholeGroupProjV1_1/cubeMap.vert", "../FoxholeGroupProjV1_1/cubeMap.frag");

	//lights - initialise first light
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
	auto level1Display = std::make_tuple("Level 1", glm::vec3(-0.6f, 0.6f, 0.0f), glm::vec3(0.2f, 0.1f, 0.0f));
	auto level2Display = std::make_tuple("Level 2", glm::vec3(0.6f, 0.6f, 0.0f), glm::vec3(0.2f, 0.1f, 0.0f));
	auto mainMenuOption = std::make_tuple("Main menu[Backspace]", glm::vec3(0.8f, 0.8f, 0.0f), glm::vec3(0.2f, 0.1f, 0.0f));

	Menu scores({ scoresDisplay, level1Display, level2Display, mainMenuOption });

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


	auto chooseNameDisplay = std::make_tuple("Choose name", glm::vec3(0.0f, 0.9f, 0.0f), glm::vec3(0.2f, 0.2f, 0.0f));
	auto chooseNameControls1 = std::make_tuple("Use arrows keys to", glm::vec3(-0.5f, 0.6f, 0.0f), glm::vec3(0.5f, 0.2f, 0.0f));
	auto chooseNameControls2 = std::make_tuple("choose name and then press space to play", glm::vec3(-0.25f, 0.3f, 0.0f), glm::vec3(0.75f, 0.2f, 0.0f));

	Menu chooseName({ chooseNameDisplay, chooseNameControls1, chooseNameControls2, mainMenuOption });

	menus.insert({ "chooseName", chooseName });


	sceneState = MAIN_MENU;
}

void SceneManager::play()
{
	if (playerNameSet) {
		BASS_Start();
		if (sceneState != PAUSE) {
			timer = std::chrono::system_clock::now();
			pauseTimer = timer;

			backgroundNoise = BASS_SampleGetChannel(sounds[0], TRUE);
			BASS_ChannelFlags(backgroundNoise, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
			if (!BASS_ChannelPlay(backgroundNoise, FALSE))
				std::cout << "Can't play sample - " << BASS_ErrorGetCode() << std::endl;

			level = 1;
			initGameObjects();
			respawnPlayer();
		}

		if (!inCountdown())
			sceneState = IN_GAME;
	}
	else {
		chooseName();
	}
}

bool SceneManager::inCountdown()
{

	int countdownTimer = std::chrono::duration<double>(std::chrono::system_clock::now() - timer).count();

	if (!paused()) {
		if (countdownTimer < 3) {
			countdown();
		}
		else {
			if (sceneState == COUNTDOWN) {
				sceneState = IN_GAME;
			}
		}
	}
	return sceneState == COUNTDOWN;
}

void SceneManager::mainMenu()
{
	BASS_ChannelStop(backgroundNoise);
	highscoreOnLevel1 = false;
	highscoreOnLevel2 = false;
	playerNameSet = false;
	playerName = "";
	sceneState = MAIN_MENU;

}

void SceneManager::loadScores() {
	std::ifstream highScores1_STREAM("../FoxholeGroupProjV1_1/highScores1.txt");
	std::ifstream highScores2_STREAM("../FoxholeGroupProjV1_1/highScores2.txt");
	
	//read both txts
	if (highScores1_STREAM && highScores2_STREAM) {
		std::string username;
		std::string userTimeStr;
		std::string hash;
		double userTime;
		std::stringstream ss;
		bool continueReading = true;
		while (continueReading) {
			std::getline(highScores1_STREAM, username, ':');

			std::getline(highScores1_STREAM, userTimeStr, ';');
			ss.str(userTimeStr);
			ss >> userTime;

			highscores1.push_back(std::make_pair(username, userTime));

			ss.str("");
			ss.clear();
			std::getline(highScores1_STREAM, hash, ';');
			if (hash != "#")
				continueReading = false;
		}
		continueReading = true;
		while (continueReading) {
			std::getline(highScores2_STREAM, username, ':');

			std::getline(highScores2_STREAM, userTimeStr, ';');
			ss.str(userTimeStr);
			ss >> userTime;

			highscores2.push_back(std::make_pair(username, userTime));

			ss.str("");
			ss.clear();
			std::getline(highScores2_STREAM, hash, ';');
			if (hash != "#")
				continueReading = false;
		}

		highScores1_STREAM.close();
		highScores2_STREAM.close();
	}

	if (highscores1.size() != highscores2.size()) {
		std::cerr << "SOMETHING WRONG YO! highscores1 and 2 have different no of scores\n";
	}
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
		addToScores();
		break;
	case CONTROLS:
		menu = menus.at("controls");

		for (auto menuObj : menu) {
			renderHUDObject(menuObj);
		}

		break;
	case CHOOSE_NAME:
		menu = menus.at("chooseName");

		for (auto menuObj : menu) {
			renderHUDObject(menuObj);
		}
		renderPlayerChars();
		break;
	default:
		break;

	}
}

void SceneManager::renderPlayerChars()
{
	std::string playerCharStr;
	playerCharStr += playerName1;
	auto playerChar1 = std::make_tuple(playerCharStr, glm::vec3(-0.5f, -0.1f, 0.0f), glm::vec3(0.1f, 0.1f, 0.0f));
	renderHUDObject(playerChar1);

	playerCharStr.clear();
	playerCharStr += playerName2;
	auto playerChar2 = std::make_tuple(playerCharStr, glm::vec3(-0.3f, -0.1f, 0.0f), glm::vec3(0.1f, 0.1f, 0.0f));
	renderHUDObject(playerChar2);

	playerCharStr.clear();
	playerCharStr += playerName3;
	auto playerChar3 = std::make_tuple(playerCharStr, glm::vec3(-0.1f, -0.1f, 0.0f), glm::vec3(0.1f, 0.1f, 0.0f));
	renderHUDObject(playerChar3);

	MenuObject charUnderline;

	switch (activeChar) {
	case 1:
		charUnderline = std::make_tuple("_", glm::vec3(-0.5f, -0.15f, 0.0f), glm::vec3(0.1f, 0.1f, 0.0f));
		renderHUDObject(charUnderline);
		break;
	case 2:
		charUnderline = std::make_tuple("_", glm::vec3(-0.3f, -0.15f, 0.0f), glm::vec3(0.1f, 0.1f, 0.0f));
		renderHUDObject(charUnderline);
		break;
	case 3:
		charUnderline = std::make_tuple("_", glm::vec3(-0.1f, -0.15f, 0.0f), glm::vec3(0.1f, 0.1f, 0.0f));
		renderHUDObject(charUnderline);
		break;
	default:
		break;
	}
}

//if anything needs to be checked every update loop - call method here
void SceneManager::playerUpdate(bool spaceUp)
{
	checkPlayerRespawn();
	checkEndLevel();
	playerFall(spaceUp);
	detectCollectableCollision();

	for (int i = 0; i < gameObjects.size(); i++) {
		if (gameObjects[i].getName() == "Car") {
			updateCar(i);
		}
	}

	updateCollectables();
}

void SceneManager::chooseNameAndPlay() {
	playerName = "";
	playerName += playerName1;
	playerName += playerName2;
	playerName += playerName3;
	playerNameSet = true;
	play();
}

void SceneManager::changeActiveChar(bool right)
{
	if (right) {
		activeChar++;
	}
	else {
		activeChar--;
	}
	if (activeChar > 3) {
		activeChar = 1;
	}
	else if (activeChar < 1) {
		activeChar = 3;
	}

}

void SceneManager::changeCurrentChar(bool up)
{
	if (up) {
		switch (activeChar) {
		case 1:
			playerName1++;
			if (playerName1 > 'Z') {
				playerName1 = 'A';
			}
			break;
		case 2:
			playerName2++;
			if (playerName2 > 'Z') {
				playerName2 = 'A';
			}
			break;
		case 3:
			playerName3++;
			if (playerName3 > 'Z') {
				playerName3 = 'A';
			}
			break;
		default:
			break;
		}
	}
	else {
		switch (activeChar) {
		case 1:
			playerName1--;
			if (playerName1 < 'A') {
				playerName1 = 'Z';
			}
			break;
		case 2:
			playerName2--;
			if (playerName2 < 'A') {
				playerName2 = 'Z';
			}
			break;
		case 3:
			playerName3--;
			if (playerName3 < 'A') {
				playerName3 = 'Z';
			}
			break;
		default:
			break;
		}
	}
}

//this method gets top five scores and displays
void SceneManager::addToScores() {
	glm::vec3 pos(-0.6f, 0.3f, 0.0f);
	glm::vec3 scale(0.2f, 0.1f, 0.0f);
	float step = 0.15f;
	std::string scoreStr;
	int scoreCounter = 0;

	if (highscoreOnLevel1)
		renderHUDObject(std::make_tuple("HIGH SCORE", glm::vec3(-0.6f, -0.6f, 0.0f), glm::vec3(0.2f, 0.2f, 0.0f)));

	if (highscoreOnLevel2)
		renderHUDObject(std::make_tuple("HIGH SCORE", glm::vec3(0.6f, -0.6f, 0.0f), glm::vec3(0.2f, 0.2f, 0.0f)));

	for (auto score : highscores1) {
		scoreStr.append(std::to_string(++scoreCounter));
		scoreStr.append(". " + score.first + "  ");
		scoreStr.append(std::to_string(score.second));
		scoreStr.append("s");
		auto scoreRender = std::make_tuple(scoreStr, pos, scale);
		renderHUDObject(scoreRender);
		pos.y -= step;
		scoreStr.clear();
	}
	pos = { 0.6f, 0.3f, 0.0f };
	scoreCounter = 0;
	for (auto score : highscores2) {
		scoreStr.append(std::to_string(++scoreCounter));
		scoreStr.append(". " + score.first + "  ");
		scoreStr.append(std::to_string(score.second));
		scoreStr.append("s");
		auto scoreRender = std::make_tuple(scoreStr, pos, scale);
		renderHUDObject(scoreRender);
		pos.y -= step;
		scoreStr.clear();
	}
}

void SceneManager::renderSkybox(glm::mat4 projection)
{
	if (level == 1) {
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
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	else if (level == 2) {
		// skybox as single cube using cube map
		glUseProgram(skyboxProgram);
		rt3d::setUniformMatrix4fv(skyboxProgram, "projection", glm::value_ptr(projection));
		glDepthMask(GL_FALSE); // make sure writing to update depth test is off
		glm::mat3 mvRotOnlyMat3 = glm::mat3(mvStack.top());
		mvStack.push(glm::mat4(mvRotOnlyMat3));
		glCullFace(GL_FRONT); // drawing inside of cube!
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox2[0]);
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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

glm::mat4 SceneManager::initRendering()
{
	glm::mat4 projection(1.0);
	projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), 800.0f / 600.0f, 1.0f, 300.0f);

	glm::mat4 modelview(1.0); // set base position for scene
	mvStack.push(modelview);

	initCamera();

	return projection;
}

void SceneManager::initCamera() {
	at = player.getPos();
	eye = moveForward(at, -cameraR, -8.0f);
	eye.y += 3.0;
	mvStack.top() = glm::lookAt(eye, at, up);
}

void SceneManager::initTTF() {
	// set up TrueType / SDL_ttf
	if (TTF_Init() == -1)
		std::cout << "TTF failed to initialise." << std::endl;

	textFont = TTF_OpenFont("../FoxholeGroupProjV1_1/MavenPro-Regular.ttf", 24);
	if (textFont == NULL)
		std::cout << "Failed to open font." << std::endl;
}

void SceneManager::init()
{

	initTTF();

	initSounds();

	shaderProgram = rt3d::initShaders("../FoxholeGroupProjV1_1/phong-tex.vert", "../FoxholeGroupProjV1_1/phong-tex.frag");
	textureProgram = rt3d::initShaders("../FoxholeGroupProjV1_1/textured.vert", "../FoxholeGroupProjV1_1/textured.frag");

	//loading skybox
	//https://opengameart.org/content/forest-skyboxes
	const char *cubeTexFiles[6] = {
		"../FoxholeGroupProjV1_1/Brudslojan/posz.bmp", "../FoxholeGroupProjV1_1/Brudslojan/negz.bmp",
		"../FoxholeGroupProjV1_1/Brudslojan/posx.bmp", "../FoxholeGroupProjV1_1/Brudslojan/negx.bmp",
		"../FoxholeGroupProjV1_1/Brudslojan/posy.bmp", "../FoxholeGroupProjV1_1/Brudslojan/posy.bmp"
	};

	loadCubeMap(cubeTexFiles, skybox);
	const char *cubeTexFiles2[6] = {
		"../FoxholeGroupProjV1_1/Town-skybox/Town_bk.bmp", "../FoxholeGroupProjV1_1/Town-skybox/Town_ft.bmp",
		"../FoxholeGroupProjV1_1/Town-skybox/Town_rt.bmp", "../FoxholeGroupProjV1_1/Town-skybox/Town_lf.bmp",
		"../FoxholeGroupProjV1_1/Town-skybox/Town_up.bmp", "../FoxholeGroupProjV1_1/Town-skybox/Town_dn.bmp"
	};
	loadCubeMap(cubeTexFiles2, skybox2);

	std::vector<GLfloat> verts;
	std::vector<GLfloat> norms;
	std::vector<GLfloat> tex_coords;
	std::vector<GLuint> indices;
	rt3d::loadObj("../FoxholeGroupProjV1_1/cube.obj", verts, norms, tex_coords, indices);
	meshIndexCount = indices.size();

	meshObjects.push_back(rt3d::createMesh(verts.size() / 3,
		verts.data(), nullptr, norms.data(),
		tex_coords.data(), meshIndexCount,
		indices.data()));

	objMeshIndexCounts.insert({ "cube.obj", meshIndexCount });

	verts.clear();
	norms.clear();
	tex_coords.clear();
	indices.clear();

	rt3d::loadObj("../FoxholeGroupProjV1_1/fox.obj", verts, norms, tex_coords, indices);
	meshIndexCount = indices.size();

	meshObjects.push_back(rt3d::createMesh(verts.size() / 3,
		verts.data(), nullptr, norms.data(),
		tex_coords.data(), meshIndexCount,
		indices.data()));

	objMeshIndexCounts.insert({ "fox.obj", meshIndexCount });

	verts.clear();
	norms.clear();
	tex_coords.clear();
	indices.clear();

	rt3d::loadObj("../FoxholeGroupProjV1_1/CartoonTree.obj", verts, norms, tex_coords, indices);
	meshIndexCount = indices.size();

	meshObjects.push_back(rt3d::createMesh(verts.size() / 3,
		verts.data(), nullptr, norms.data(),
		tex_coords.data(), meshIndexCount,
		indices.data()));

	objMeshIndexCounts.insert({ "CartoonTree.obj", meshIndexCount });

	verts.clear();
	norms.clear();
	tex_coords.clear();
	indices.clear();

	meshObjects.push_back(foxModel.ReadMD2Model("../FoxholeGroupProjV1_1/starfox.md2"));
	player.setMeshIndexCount(foxModel.getVertDataCount());


	verts.clear();
	norms.clear();
	tex_coords.clear();
	indices.clear();

	rt3d::loadObj("../FoxholeGroupProjV1_1/fence.obj", verts, norms, tex_coords, indices);
	meshIndexCount = indices.size();

	meshObjects.push_back(rt3d::createMesh(verts.size() / 3,
		verts.data(), nullptr, norms.data(),
		tex_coords.data(), meshIndexCount,
		indices.data()));

	objMeshIndexCounts.insert({ "fence.obj", meshIndexCount });

	verts.clear();
	norms.clear();
	tex_coords.clear();
	indices.clear();


	rt3d::loadObj("../FoxholeGroupProjV1_1/car2.obj", verts, norms, tex_coords, indices);
	meshIndexCount = indices.size();

	meshObjects.push_back(rt3d::createMesh(verts.size() / 3,
		verts.data(), nullptr, norms.data(),
		tex_coords.data(), meshIndexCount,
		indices.data()));

	objMeshIndexCounts.insert({ "car2.obj", meshIndexCount });

	verts.clear();
	norms.clear();
	tex_coords.clear();
	indices.clear();

	rt3d::loadObj("../FoxholeGroupProjV1_1/Chicken_Leg.obj", verts, norms, tex_coords, indices);
	meshIndexCount = indices.size();

	meshObjects.push_back(rt3d::createMesh(verts.size() / 3,
		verts.data(), nullptr, norms.data(),
		tex_coords.data(), meshIndexCount,
		indices.data()));

	objMeshIndexCounts.insert({ "chickenleg.obj", meshIndexCount });

	verts.clear();
	norms.clear();
	tex_coords.clear();
	indices.clear();

	//add more meshes with rt3d::loadObj("*.obj", verts, norms, tex_coords, indices); where * is the obj name
	//then meshIndexCount = indices.size();
	//then meshObjects.push_back(rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), meshIndexCount, indices.data()));

	textures.push_back(loadBitmap("../FoxholeGroupProjV1_1/fabric.bmp"));
	textures.push_back(loadBitmap("../FoxholeGroupProjV1_1/water.bmp"));
	textures.push_back(loadBitmap("../FoxholeGroupProjV1_1/box.bmp"));
	textures.push_back(loadBitmap("../FoxholeGroupProjV1_1/twigs.bmp"));
	textures.push_back(loadBitmap("../FoxholeGroupProjV1_1/Town-skybox/grass1.bmp"));
	textures.push_back(loadBitmap("../FoxholeGroupProjV1_1/orange-fox.bmp"));
	textures.push_back(loadBitmap("../FoxholeGroupProjV1_1/tree.bmp"));
	textures.push_back(loadBitmap("../FoxholeGroupProjV1_1/starfox.bmp"));
	textures.push_back(loadBitmap("../FoxholeGroupProjV1_1/building.bmp"));
	textures.push_back(loadBitmap("../FoxholeGroupProjV1_1/city_ground.bmp"));
	textures.push_back(loadBitmap("../FoxholeGroupProjV1_1/chicken_diffuse.bmp"));
	textures.push_back(loadBitmap("../FoxholeGroupProjV1_1/car.bmp"));
	textures.push_back(loadBitmap("../FoxholeGroupProjV1_1/metal-texture.bmp"));
	//add more textures with textures.push_back(loadBitmap("*.bmp")); where * is the bitmap name

	initGameObjects();
	initPlayer();

	loadScores();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void SceneManager::initGameObjects() {
	gameObjects.clear();
	gameObjects.shrink_to_fit();

	if (level == 1) {
		//level 1

		gameObjects.push_back(GameObject("LevelEnd", glm::vec3(0.0f, 0.0f, -180.f), glm::vec3(25.0f, 20.0f, 5.0f), NULL, NULL, 0));

		gameObjects.push_back(GameObject("InvisibleWallRight", glm::vec3(16.0f, 0.0f, -80.0f), glm::vec3(1.0f, 20.0f, 100.0f), NULL, NULL, 0));
		gameObjects.push_back(GameObject("InvisibleWallLeft", glm::vec3(-26.0f, 0.0f, -80.0f), glm::vec3(1.0f, 20.0f, 100.0f), NULL, NULL, 0));
		gameObjects.push_back(GameObject("InvisibleWallBack", glm::vec3(0.0f, 0.0f, -180.f), glm::vec3(30.0f, 20.0f, 1.0f), NULL, NULL, 0));
		gameObjects.push_back(GameObject("InvisibleWallFront", glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(30.0f, 20.0f, 0.5f), NULL, NULL, 0));

		gameObjects.push_back(GameObject("Ground", glm::vec3(-5.0f, -0.1f, -100.0f), glm::vec3(200.0f, 0.1f, 200.0f), textures[4], meshObjects[0], objMeshIndexCounts.at("cube.obj")));

		gameObjects.push_back(GameObject("Water", glm::vec3(-5.0f, 0.0f, -100.0f), glm::vec3(20.0f, 0.1f, 50.0f), textures[1], meshObjects[0], objMeshIndexCounts.at("cube.obj")));

		gameObjects.push_back(GameObject("Cube1", glm::vec3(-5.0f, 1.0f, -50.0f), glm::vec3(5.0f, 1.0f, 5.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("Cube2", glm::vec3(-5.0f, 1.0f, -60.0f), glm::vec3(1.0f, 2.0f, 1.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("Cube3", glm::vec3(0.0f, 2.0f, -66.0f), glm::vec3(1.5f, 2.0f, 1.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("Cube4", glm::vec3(-4.0f, 1.0f, -75.0f), glm::vec3(1.5f, 2.0f, 1.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("Cube5", glm::vec3(-8.0f, 1.5f, -85.0f), glm::vec3(1.5f, 2.0f, 1.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("Cube6", glm::vec3(-10.0f, 2.0f, -96.0f), glm::vec3(1.5f, 3.0f, 1.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("Cube7", glm::vec3(-2.0f, 1.0f, -105.0f), glm::vec3(1.5f, 2.0f, 1.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("Cube8", glm::vec3(-2.0f, 1.5f, -120.0f), glm::vec3(2.0f, 2.0f, 1.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("Cube9", glm::vec3(-9.0f, 1.0f, -127.0f), glm::vec3(2.0f, 2.0f, 1.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("Cube10", glm::vec3(-5.0f, 2.0f, -137.0f), glm::vec3(2.0f, 2.0f, 1.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("Cube11", glm::vec3(-1.0f, 1.0f, -145.0f), glm::vec3(2.0f, 2.0f, 1.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));

		buildTrees();

	}
	else if (level == 2) {
		// level 2

		gameObjects.push_back(GameObject("Ground", glm::vec3(-5.0f, -0.1f, -100.0f), glm::vec3(500.0f, 0.1f, 300.0f), textures[9], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		
		gameObjects.push_back(GameObject("LevelEnd", glm::vec3(0.0f, 0.0f, -355.f), glm::vec3(250.0f, 200.0f, 50.0f), NULL, NULL, 0));

		//right edge
		gameObjects.push_back(GameObject("building1", glm::vec3(250.0f, 10.0f, -10.0f), glm::vec3(5.0f, 10.0f, 50.0f), textures[8], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("building2", glm::vec3(250.0f, 10.0f, -120.0f), glm::vec3(5.0f, 10.0f, 50.0f), textures[8], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("building3", glm::vec3(250.0f, 10.0f, -230.0f), glm::vec3(5.0f, 10.0f, 50.0f), textures[8], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("building4", glm::vec3(250.0f, 10.0f, -320.0f), glm::vec3(5.0f, 10.0f, 30.0f), textures[8], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		//back edge
		gameObjects.push_back(GameObject("building1", glm::vec3(200.0f, 10.0f, -355.0f), glm::vec3(50.0f, 10.0f, 5.0f), textures[8], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("building2", glm::vec3(95.0f, 10.0f, -355.0f), glm::vec3(50.0f, 10.0f, 5.0f), textures[8], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("building3", glm::vec3(-10.0f, 10.0f, -355.0f), glm::vec3(50.0f, 10.0f, 5.0f), textures[8], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("building4", glm::vec3(-115.0f, 10.0f, -355.0f), glm::vec3(50.0f, 10.0f, 5.0f), textures[8], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("building5", glm::vec3(-150.0f, 10.0f, -355.0f), glm::vec3(20.0f, 10.0f, 5.0f), textures[8], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		//left edge
		gameObjects.push_back(GameObject("building1", glm::vec3(-180.0f, 10.0f, -10.0f), glm::vec3(5.0f, 10.0f, 50.0f), textures[8], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("building2", glm::vec3(-180.0f, 10.0f, -120.0f), glm::vec3(5.0f, 10.0f, 50.0f), textures[8], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("building3", glm::vec3(-180.0f, 10.0f, -230.0f), glm::vec3(5.0f, 10.0f, 50.0f), textures[8], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("building4", glm::vec3(-180.0f, 10.0f, -320.0f), glm::vec3(5.0f, 10.0f, 30.0f), textures[8], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		//middle
		gameObjects.push_back(GameObject("midbuilding1", glm::vec3(50.0f, 1.0f, -140.0f), glm::vec3(30.0f, 30.0f, 100.0f), textures[8], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("midbuilding2", glm::vec3(-50.0f, 1.0f, -140.0f), glm::vec3(30.0f, 30.0f, 100.0f), textures[8], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("fence", glm::vec3(18.0f, 1.0f, -50.0f), glm::vec3(0.09f, 0.015f, 0.1f), textures[12], meshObjects[4], objMeshIndexCounts.at("fence.obj")));//middle
		gameObjects.push_back(GameObject("InvisibleFence", glm::vec3(18.0f, 0.95f, -50.0f), glm::vec3(70.0f, 10.0f, 1.0f), NULL, NULL, 0));//middle

		//collectables section front
		gameObjects.push_back(GameObject("collectable1", glm::vec3(-20.0, 4.0f, 34.0), glm::vec3(0.5f, 0.5f, 0.5f), textures[10], meshObjects[6], objMeshIndexCounts.at("chickenleg.obj")));
		gameObjects.push_back(GameObject("collectable2", glm::vec3(35.0, 4.0f, -8.0), glm::vec3(0.5f, 0.5f, 0.5f), textures[10], meshObjects[6], objMeshIndexCounts.at("chickenleg.obj")));
		gameObjects.push_back(GameObject("collectable3", glm::vec3(18.0, 4.0f, -27.0), glm::vec3(0.5f, 0.5f, 0.5f), textures[10], meshObjects[6], objMeshIndexCounts.at("chickenleg.obj")));
		gameObjects.push_back(GameObject("collectable4", glm::vec3(-25.0, 4.0f, -20.0), glm::vec3(0.5f, 0.5f, 0.5f), textures[10], meshObjects[6], objMeshIndexCounts.at("chickenleg.obj")));

		//collectables section jumping
		gameObjects.push_back(GameObject("collectable5", glm::vec3(-100.0f, 5.0f, -50.0f), glm::vec3(0.5f, 0.5f, 0.5f), textures[10], meshObjects[6], objMeshIndexCounts.at("chickenleg.obj"))); //on box
		gameObjects.push_back(GameObject("collectable6", glm::vec3(-110.0f, 7.0f, -95.0f), glm::vec3(0.5f, 0.5f, 0.5f), textures[10], meshObjects[6], objMeshIndexCounts.at("chickenleg.obj"))); //on box
		gameObjects.push_back(GameObject("collectable7", glm::vec3(-135, 4.0f, -130.0), glm::vec3(0.5f, 0.5f, 0.5f), textures[10], meshObjects[6], objMeshIndexCounts.at("chickenleg.obj")));
		gameObjects.push_back(GameObject("collectable8", glm::vec3(-140.0, 4.0f, -160.0), glm::vec3(0.5f, 0.5f, 0.5f), textures[10], meshObjects[6], objMeshIndexCounts.at("chickenleg.obj")));

		//collectables section taxi
		gameObjects.push_back(GameObject("collectable9", glm::vec3(150.0, 4.0f, -50.0), glm::vec3(0.5f, 0.5f, 0.5f), textures[10], meshObjects[6], objMeshIndexCounts.at("chickenleg.obj")));
		gameObjects.push_back(GameObject("collectable10", glm::vec3(180.0, 4.0f, -85.0), glm::vec3(0.5f, 0.5f, 0.5f), textures[10], meshObjects[6], objMeshIndexCounts.at("chickenleg.obj")));
		gameObjects.push_back(GameObject("collectable11", glm::vec3(160.0, 4.0f, -130.0), glm::vec3(0.5f, 0.5f, 0.5f), textures[10], meshObjects[6], objMeshIndexCounts.at("chickenleg.obj")));
		gameObjects.push_back(GameObject("collectable12", glm::vec3(200.0, 4.0f, -180.0), glm::vec3(0.5f, 0.5f, 0.5f), textures[10], meshObjects[6], objMeshIndexCounts.at("chickenleg.obj")));
		gameObjects.push_back(GameObject("collectable13", glm::vec3(195.0, 4.0f, -150.0), glm::vec3(0.5f, 0.5f, 0.5f), textures[10], meshObjects[6], objMeshIndexCounts.at("chickenleg.obj")));
		gameObjects.push_back(GameObject("collectable14", glm::vec3(170.0, 4.0f, -70.0), glm::vec3(0.5f, 0.5f, 0.5f), textures[10], meshObjects[6], objMeshIndexCounts.at("chickenleg.obj")));

		//collectables section back
		gameObjects.push_back(GameObject("collectable15", glm::vec3(-30.0, 4.0f, -250.0), glm::vec3(0.5f, 0.5f, 0.5f), textures[10], meshObjects[6], objMeshIndexCounts.at("chickenleg.obj")));
		gameObjects.push_back(GameObject("collectable16", glm::vec3(-55.0, 4.0f, -300.0), glm::vec3(0.5f, 0.5f, 0.5f), textures[10], meshObjects[6], objMeshIndexCounts.at("chickenleg.obj")));

		collectables = 10;

		//Jumping Puzzle
		gameObjects.push_back(GameObject("fence", glm::vec3(-85.0f, 0.95f, -80.0f), glm::vec3(0.7f, 0.05f, 0.1f), textures[12], meshObjects[4], objMeshIndexCounts.at("fence.obj")));//jumping
		gameObjects.push_back(GameObject("InvisibleFence", glm::vec3(-85.0f, 0.95f, -80.0f), glm::vec3(90.0f, 10.0f, 1.0f), NULL, NULL, 0));
		gameObjects.push_back(GameObject("jumpbox1", glm::vec3(-100.0f, 1.0f, -50.0f), glm::vec3(1.5f, 1.5f, 5.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj"))); //front boxes

		gameObjects.push_back(GameObject("jumpbox2a", glm::vec3(-110.0f, 4.0f, -55.0f), glm::vec3(1.5f, 1.5f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox2b", glm::vec3(-110.0f, 1.25f, -55.0f), glm::vec3(1.5f, 1.25f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		
		gameObjects.push_back(GameObject("jumpbox3a", glm::vec3(-120.0f, 8.0f, -60.0f), glm::vec3(1.5f, 1.0f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox3b", glm::vec3(-120.0f, 5.0f, -60.0f), glm::vec3(1.5f, 2.0f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox3c", glm::vec3(-120.0f, 0.25f, -60.0f), glm::vec3(1.5f, 3.0f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));

		gameObjects.push_back(GameObject("jumpbox4a", glm::vec3(-115.0f, 12.0f, -70.0f), glm::vec3(1.5f, 2.0f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox4b", glm::vec3(-115.0f, 8.0f, -70.0f), glm::vec3(1.5f, 2.0f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox4c", glm::vec3(-115.0f, 4.0f, -70.0f), glm::vec3(1.5f, 2.0f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox4d", glm::vec3(-115.0f, 0.0f, -70.0f), glm::vec3(1.5f, 2.0f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));

		gameObjects.push_back(GameObject("jumpbox5a", glm::vec3(-108.0f, 16.0f, -75.0f), glm::vec3(1.5f, 1.5f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox5b", glm::vec3(-108.0f, 13.0f, -75.0f), glm::vec3(1.5f, 1.5f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox5c", glm::vec3(-108.0f, 10.0f, -75.0f), glm::vec3(1.5f, 1.5f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox5d", glm::vec3(-108.0f, 7.0f, -75.0f), glm::vec3(1.5f, 1.5f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox5e", glm::vec3(-108.0f, 3.0f, -75.0f), glm::vec3(1.5f, 2.5f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));

		gameObjects.push_back(GameObject("jumpbox6a", glm::vec3(-101.0f, 20.0f, -75.0f), glm::vec3(1.5f, 1.5f, 4.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox6b", glm::vec3(-101.0f, 17.0f, -75.0f), glm::vec3(1.5f, 1.5f, 4.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox6c", glm::vec3(-101.0f, 14.0f, -75.0f), glm::vec3(1.5f, 1.5f, 4.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox6d", glm::vec3(-101.0f, 11.0f, -75.0f), glm::vec3(1.5f, 1.5f, 4.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox6e", glm::vec3(-101.0f, 8.0f, -75.0f), glm::vec3(1.5f, 1.5f, 4.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox6f", glm::vec3(-101.0f, 5.0f, -75.0f), glm::vec3(1.5f, 1.5f, 4.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox6g", glm::vec3(-101.0f, 2.0f, -75.0f), glm::vec3(1.5f, 1.5f, 4.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));

		gameObjects.push_back(GameObject("jumpbox7", glm::vec3(-130.0f, 1.0f, -95.0f), glm::vec3(1.5f, 1.5f, 5.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));//back boxes

		gameObjects.push_back(GameObject("jumpbox8", glm::vec3(-120.0f, 3.0f, -90.0f), glm::vec3(1.5f, 2.5f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));

		gameObjects.push_back(GameObject("jumpbox9a", glm::vec3(-110.0f, 5.0f, -95.0f), glm::vec3(1.5f, 1.5f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox9b", glm::vec3(-110.0f, 2.0f, -95.0f), glm::vec3(1.5f, 1.5f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));

		gameObjects.push_back(GameObject("jumpbox10a", glm::vec3(-115.0f, 12.0f, -90.0f), glm::vec3(1.5f, 2.0f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox10b", glm::vec3(-115.0f, 8.0f, -90.0f), glm::vec3(1.5f, 2.0f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox10c", glm::vec3(-115.0f, 4.0f, -90.0f), glm::vec3(1.5f, 2.0f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox10d", glm::vec3(-115.0f, 0.0f, -90.0f), glm::vec3(1.5f, 2.0f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));

		gameObjects.push_back(GameObject("jumpbox11a", glm::vec3(-108.0f, 16.0f, -90.0f), glm::vec3(1.5f, 1.5f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox11b", glm::vec3(-108.0f, 13.0f, -90.0f), glm::vec3(1.5f, 1.5f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox11c", glm::vec3(-108.0f, 10.0f, -90.0f), glm::vec3(1.5f, 1.5f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox11d", glm::vec3(-108.0f, 7.0f, -90.0f), glm::vec3(1.5f, 1.5f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox11e", glm::vec3(-108.0f, 3.0f, -90.0f), glm::vec3(1.5f, 2.5f, 1.5f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));

		gameObjects.push_back(GameObject("jumpbox12a", glm::vec3(-101.0f, 20.0f, -85.0f), glm::vec3(1.5f, 1.5f, 4.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox12b", glm::vec3(-101.0f, 17.0f, -85.0f), glm::vec3(1.5f, 1.5f, 4.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox12c", glm::vec3(-101.0f, 14.0f, -85.0f), glm::vec3(1.5f, 1.5f, 4.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox12d", glm::vec3(-101.0f, 11.0f, -85.0f), glm::vec3(1.5f, 1.5f, 4.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox12e", glm::vec3(-101.0f, 8.0f, -85.0f), glm::vec3(1.5f, 1.5f, 4.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox12f", glm::vec3(-101.0f, 5.0f, -85.0f), glm::vec3(1.5f, 1.5f, 4.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));
		gameObjects.push_back(GameObject("jumpbox12g", glm::vec3(-101.0f, 2.0f, -85.0f), glm::vec3(1.5f, 1.5f, 4.0f), textures[2], meshObjects[0], objMeshIndexCounts.at("cube.obj")));

		//cars
		gameObjects.push_back(GameObject("Car", glm::vec3(100.0f, 1.0f, -50.0f), glm::vec3(5.0f, 5.0f, 2.0f), textures[11], meshObjects[5], objMeshIndexCounts.at("car2.obj")));
		gameObjects.push_back(GameObject("Car", glm::vec3(90.0f, 1.0f, -80.0f), glm::vec3(5.0f, 5.0f, 2.0f), textures[11], meshObjects[5], objMeshIndexCounts.at("car2.obj")));
		gameObjects.push_back(GameObject("Car", glm::vec3(120.0f, 1.0f, -130.0f), glm::vec3(5.0f, 5.0f, 2.0f), textures[11], meshObjects[5], objMeshIndexCounts.at("car2.obj")));
		gameObjects.push_back(GameObject("Car", glm::vec3(150.0f, 1.0f, -180.0f), glm::vec3(5.0f, 5.0f, 2.0f), textures[11], meshObjects[5], objMeshIndexCounts.at("car2.obj")));


	}

	//add more game objects with gameObjects.push_back(GameObject("Name", position, scale, texture from textures, mesh from meshObjects)); 
}

void SceneManager::updateCar(int carIndex)
{
	glm::vec3 newCarPos = gameObjects[carIndex].getPos();

	//car drives between two buildings
	if ((newCarPos.x - gameObjects[carIndex].getStartPos().x) >= 0) {
		newCarPos.x += 0.5f;

		if (gameObjects[carIndex].getRotation() != 270.0f)
			gameObjects[carIndex].setRotation(270.0f);

		if (newCarPos.x >= 200.0f) {
			newCarPos.x -= 1.0f;
		}
	}
	else if ((newCarPos.x - gameObjects[carIndex].getStartPos().x) <= 0) {
		newCarPos.x -= 0.5f;

		if (gameObjects[carIndex].getRotation() != 90.0f)
			gameObjects[carIndex].setRotation(90.0f);

		if (newCarPos.x <= 90.0f) {
			newCarPos.x += 1.0f;
		}
	}

	if (!CollisionDetector::detectCollision(gameObjects[carIndex], player)) {
		gameObjects[carIndex].setPos(newCarPos);
	}

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

void SceneManager::checkEndLevel()
{
	if (level == 2 && collectables <= 0 && player.getLastCollision() == "LevelEnd") {
		saveScores(levelTime, 2);
		playerNameSet = false;
		playerName = "";
		activeChar = 1;
		if (highscoreOnLevel1 || highscoreOnLevel2)
			scores();
		else
			mainMenu();
	}
}

void SceneManager::buildTrees() {

	glm::vec3 treePos(16.0f, 1.0f, 5.0f);
	glm::vec3 treeScale(1.0f, 5.0f, 1.0f);
	std::string treeName("Tree");
	for (int i = 0; i < 40; i++) {
		treeName.append(std::to_string((i * 1) + 1));

		gameObjects.push_back(GameObject(treeName, treePos, treeScale, textures[6], meshObjects[2], objMeshIndexCounts.at("CartoonTree.obj")));

		treeName = "Tree";
		treeName.append(std::to_string((i * 2) + 2));
		treePos.x = -26.0f;
		treePos.z -= 6.0f;

		gameObjects.push_back(GameObject(treeName, treePos, treeScale, textures[6], meshObjects[2], objMeshIndexCounts.at("CartoonTree.obj")));

		treeName = "Tree";
		treePos.x = 16.0f;

	}
	treePos = { 16.0f, 1.0f, 5.0f };
	treeName = "Tree";
	for (int i = 40; i < 51; i++) {
		treeName.append(std::to_string((i * 1) + 1));

		gameObjects.push_back(GameObject(treeName, treePos, treeScale, textures[6], meshObjects[2], objMeshIndexCounts.at("CartoonTree.obj")));

		treePos.x -= 4.0f;

		treeName = "Tree";
	}

}

void SceneManager::initPlayer() {
	player.setPlayerTexture(textures[7]);
	player.setPlayerMesh(meshObjects[3]);
}

void SceneManager::setShaderProjection(glm::mat4 projection)
{
	glUseProgram(shaderProgram);
	rt3d::setUniformMatrix4fv(shaderProgram, "projection", glm::value_ptr(projection));
}

void SceneManager::setLights()
{
	rt3d::setLight(shaderProgram, lights[0]);
	rt3d::setMaterial(shaderProgram, materials[0]);
	glm::vec4 tmp = mvStack.top()*lightPos;
	lights[0].position[0] = tmp.x;
	lights[0].position[1] = tmp.y;
	lights[0].position[2] = tmp.z;
	rt3d::setLightPos(shaderProgram, glm::value_ptr(tmp));
}

void SceneManager::renderObjects()
{
	glUseProgram(shaderProgram);
	setLights();
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
	mvStack.top() = glm::rotate(mvStack.top(), float(gObj.getRotation() * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
	mvStack.top() = glm::rotate(mvStack.top(), float(180 * DEG_TO_RADIAN), glm::vec3(1.0f, 0.0f, 0.0f));
	mvStack.top() = glm::rotate(mvStack.top(), float(180 * DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawIndexedMesh(gObj.getMesh(), gObj.getMeshIndexCount(), GL_TRIANGLES);
	mvStack.pop();


}

void SceneManager::togglePause() {

	if (sceneState == PAUSE) {
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
	}
	else if (sceneState == IN_GAME) {
		pauseTime = std::chrono::duration<double>(std::chrono::system_clock::now() - pauseTimer).count();
		std::cout << std::chrono::duration<double>(pauseTimer - timer).count() << std::endl;
		timer += std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::duration<double>(std::chrono::system_clock::now() - pauseTimer));
	}
}

void SceneManager::renderHUDObject(MenuObject menuObj) {

	glUseProgram(textureProgram);//Use texture-only shader for text rendering

	GLuint label = 0;
	label = textToTexture(std::get<0>(menuObj).c_str(), label);

	glBindTexture(GL_TEXTURE_2D, label);

	mvStack.push(glm::mat4(1.0));
	mvStack.top() = glm::translate(mvStack.top(), std::get<1>(menuObj));
	mvStack.top() = glm::scale(mvStack.top(), std::get<2>(menuObj));
	rt3d::setUniformMatrix4fv(textureProgram, "projection", glm::value_ptr(glm::mat4(1.0)));
	rt3d::setUniformMatrix4fv(textureProgram, "modelview", glm::value_ptr(mvStack.top()));

	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();
}

void SceneManager::renderHUD()
{
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

		currentAnimation = 0;

		if (level == 1) {
			renderHUDObject(std::make_tuple("Cross the pond", glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.5f, 0.25f, 0.0f)));
		}
		else if (level == 2) {
			renderHUDObject(std::make_tuple("Collect 10 chicken pieces", glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.5f, 0.25f, 0.0f)));
			renderHUDObject(std::make_tuple("and reach the other side", glm::vec3(0.0f, 0.2f, 0.0f), glm::vec3(0.5f, 0.25f, 0.0f)));
		}
	}
	else {
		totalTime -= 3;

		std::string timerStr = "Time: ";

		if (totalTime < 0)
			totalTime = 0;

		timerStr.append(std::to_string(totalTime));

		timerStr.append("s");

		renderHUDObject(std::make_tuple(timerStr, glm::vec3(-0.8f, 0.8f, 0.0f), glm::vec3(0.2f, 0.2f, 0.0f)));
		levelTime = totalTime;

	}

	if (level == 2) {
		std::string collectablesString("Chicken pieces left: ");
		collectablesString.append(std::to_string(collectables));

		renderHUDObject(std::make_tuple(collectablesString, glm::vec3(-0.5f, -0.8f, 0.0f), glm::vec3(0.5f, 0.2f, 0.0f)));

	}

	int respawnTime = std::chrono::duration<double>(temp - waterRespawnTimer).count();

	if (respawnTime > -1 && respawnTime < 2) {
		renderHUDObject(std::make_tuple("Avoid the water!", glm::vec3(0.8f, -0.8f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f)));
	}

	respawnTime = std::chrono::duration<double>(temp - carRespawnTimer).count();

	if (respawnTime > -1 && respawnTime < 2) {
		renderHUDObject(std::make_tuple("Avoid the cars!", glm::vec3(0.8f, -0.8f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f)));
	}

	glEnable(GL_DEPTH_TEST);//Re-enable depth test after HUD label
}

void SceneManager::renderPlayer()
{
	if (!paused()) {
		foxModel.Animate(currentAnimation, 0.1);
		rt3d::updateMesh(player.getMesh(), RT3D_VERTEX, foxModel.getAnimVerts(), foxModel.getVertDataSize());
	}
	glCullFace(GL_FRONT);
	glBindTexture(GL_TEXTURE_2D, player.getTexture());
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(player.getPos().x, player.getPos().y, player.getPos().z));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(0.05f, 0.05f, 0.05f));

	mvStack.top() = glm::rotate(mvStack.top(), float((player.getPlayerR() - 90.0f)*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));

	mvStack.top() = glm::rotate(mvStack.top(), float(270 * DEG_TO_RADIAN), glm::vec3(1.0f, 0.0f, 0.0f));
	mvStack.top() = glm::rotate(mvStack.top(), float(180 * DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawMesh(player.getMesh(), player.getMeshIndexCount(), GL_TRIANGLES);
	mvStack.pop();
	glCullFace(GL_BACK);
}

void SceneManager::updatePlayerR(GLfloat deltaR)
{
	if (currentAnimation != 0) {
		player.setPlayerR(player.getPlayerR() + deltaR);
	}
	cameraR += deltaR;

}

void SceneManager::detectCollectableCollision() {

	if (level == 2) {
		std::string playerColl = player.getLastCollision();
		if (player.getLastCollision().substr(0, 11) == "collectable") {

			HCHANNEL ch = BASS_SampleGetChannel(sounds[6], FALSE);
			BASS_ChannelSetAttribute(ch, BASS_ATTRIB_FREQ, 0);
			BASS_ChannelSetAttribute(ch, BASS_ATTRIB_VOL, 0.5);
			BASS_ChannelSetAttribute(ch, BASS_ATTRIB_PAN, -1);

			if (!BASS_ChannelPlay(ch, FALSE))
				std::cout << "Can't play sample " << BASS_ErrorGetCode() << std::endl;

			std::cout << "player collided with " << player.getLastCollision() << "\n";
			int index = getGameObjectIndex(player.getLastCollision());
			player.setLastCollision("");
			gameObjects.erase(gameObjects.begin() + index);
			collectables--;
		}
	}
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

	sounds.push_back(loadSounds("../FoxholeGroupProjV1_1/forest-environment.wav"));
	//http://soundbible.com/1818-Rainforest-Ambience.html

	sounds.push_back(loadSounds("../FoxholeGroupProjV1_1/splash.wav"));
	//http://soundbible.com/2100-Splash-Rock-In-Lake.html

	sounds.push_back(loadSounds("../FoxholeGroupProjV1_1/city_environment.wav"));
	//http://soundbible.com/1266-City-Ambiance.html

	sounds.push_back(loadSounds("../FoxholeGroupProjV1_1/bleep.wav"));
	//http://soundbible.com/1682-Robot-Blip.html

	sounds.push_back(loadSounds("../FoxholeGroupProjV1_1/bloop.wav"));
	//http://soundbible.com/1669-Robot-Blip-2.html

	sounds.push_back(loadSounds("../FoxholeGroupProjV1_1/walk.wav"));
	//http://soundbible.com/1796-Cowboy-With-Spurs.html

	sounds.push_back(loadSounds("../FoxholeGroupProjV1_1/pickup.wav"));
	//http://soundbible.com/1601-Mario-Jumping.html

	sounds.push_back(loadSounds("../FoxholeGroupProjV1_1/car_crash.wav"));
	//http://soundbible.com/454-Sound-Of-Tires-Screeching-1.html
}

void SceneManager::playBleep() {
	BASS_Start();
	HCHANNEL ch = BASS_SampleGetChannel(sounds[3], FALSE);
	BASS_ChannelSetAttribute(ch, BASS_ATTRIB_FREQ, 0);
	BASS_ChannelSetAttribute(ch, BASS_ATTRIB_VOL, 0.1);
	BASS_ChannelSetAttribute(ch, BASS_ATTRIB_PAN, -1);

	if (!BASS_ChannelPlay(ch, FALSE))
		std::cout << "Can't play sample " << BASS_ErrorGetCode() << std::endl;
}

void SceneManager::playBloop() {
	BASS_Start();
	HCHANNEL ch = BASS_SampleGetChannel(sounds[4], FALSE);
	BASS_ChannelSetAttribute(ch, BASS_ATTRIB_FREQ, 0);
	BASS_ChannelSetAttribute(ch, BASS_ATTRIB_VOL, 0.1);
	BASS_ChannelSetAttribute(ch, BASS_ATTRIB_PAN, -1);

	if (!BASS_ChannelPlay(ch, FALSE))
		std::cout << "Can't play sample " << BASS_ErrorGetCode() << std::endl;
}

void SceneManager::stand() {
	currentAnimation = 0;
}

void SceneManager::movePlayerForward(GLfloat delta) {

	glm::vec3 temp = player.getPos();

	player.setPlayerR(cameraR);

	player.setPos(moveForward(player.getPos(), -player.getPlayerR(), delta / getTimeScalar()));

	if (checkCollisions())
		player.setPos(temp);

	if (!player.isPlayerFalling() && !player.isPlayerJumping() && walkingNoise == NULL) {
		BASS_Start();
		walkingNoise = BASS_SampleGetChannel(sounds[5], FALSE);
		BASS_ChannelSetAttribute(walkingNoise, BASS_ATTRIB_FREQ, 0);
		BASS_ChannelSetAttribute(walkingNoise, BASS_ATTRIB_VOL, 0.5);
		BASS_ChannelSetAttribute(walkingNoise, BASS_ATTRIB_PAN, -1);
		if (!BASS_ChannelPlay(walkingNoise, FALSE))
			std::cout << "Can't play sample " << BASS_ErrorGetCode() << std::endl;
	}
	else if (!player.isPlayerJumping() && !player.isPlayerFalling()) {
		if (!BASS_ChannelPlay(walkingNoise, FALSE))
			std::cout << "Can't play sample " << BASS_ErrorGetCode() << std::endl;
	}



	currentAnimation = 1;
}

void SceneManager::movePlayerRight(GLfloat delta) {

	glm::vec3 temp = player.getPos();

	player.setPlayerR(cameraR);

	player.setPos(moveRight(player.getPos(), -player.getPlayerR(), delta / getTimeScalar()));

	if (checkCollisions())
		player.setPos(temp);

	if (!player.isPlayerFalling() && !player.isPlayerJumping() && walkingNoise == NULL) {
		BASS_Start();
		walkingNoise = BASS_SampleGetChannel(sounds[5], FALSE);
		BASS_ChannelSetAttribute(walkingNoise, BASS_ATTRIB_FREQ, 0);
		BASS_ChannelSetAttribute(walkingNoise, BASS_ATTRIB_VOL, 0.5);
		BASS_ChannelSetAttribute(walkingNoise, BASS_ATTRIB_PAN, -1);
		if (!BASS_ChannelPlay(walkingNoise, FALSE))
			std::cout << "Can't play sample " << BASS_ErrorGetCode() << std::endl;
	}
	else if (!player.isPlayerJumping() && !player.isPlayerFalling()) {
		if (!BASS_ChannelPlay(walkingNoise, FALSE))
			std::cout << "Can't play sample " << BASS_ErrorGetCode() << std::endl;
	}

	currentAnimation = 1;
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
		if (CollisionDetector::detectCollision(gameObjects[i], player)) {
			collided = true;
			break;
		}
	}
	return collided;
}

bool SceneManager::checkCollisions(GameObject &specObj) {
	bool collided = false;
	for (int i = 0; i < gameObjects.size(); i++) {
		if (CollisionDetector::detectCollision(gameObjects[i], specObj)) {
			collided = true;
			break;
		}
	}
	return collided;
}

void SceneManager::playerJump() {

	player.playerJump();
	currentAnimation = 6;

}

void SceneManager::playerFall(bool spaceUp) {

	if (spaceUp) {
		player.playerFall();
	}

	if (!player.isPlayerJumping()) {
		player.playerFall();

		if (checkCollisions()) {
			player.playerStand();
		}
	}

}

GameObject SceneManager::getGameObject(std::string objName) {
	for (GameObject gObj : gameObjects) {
		if (gObj.getName() == objName)
			return gObj;
	}
	return GameObject("null", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, 0);
}

int SceneManager::getGameObjectIndex(std::string objName) {
	for (int i = 0; i < gameObjects.size(); i++) {
		if (gameObjects[i].getName() == objName)
			return i;
	}
	return -1;
}

void SceneManager::respawnPlayer() {
	std::cout << "respawn because: " << player.getLastCollision() << std::endl;
	player.playerReset();
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

		waterRespawnTimer = std::chrono::system_clock::now();
		respawnPlayer();
	}
	else if (player.getLastCollision() == "LevelEnd" && level == 1) {
		level = 2;
		timer = std::chrono::system_clock::now();
		pauseTimer = timer;
		initGameObjects();
		respawnPlayer();
		BASS_ChannelStop(backgroundNoise);
		backgroundNoise = BASS_SampleGetChannel(sounds[2], TRUE);
		BASS_ChannelFlags(backgroundNoise, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
		if (!BASS_ChannelPlay(backgroundNoise, FALSE))
			std::cout << "Can't play sample - " << BASS_ErrorGetCode() << std::endl;
		saveScores(levelTime, 1);
	}
	else if (player.getPos().y < -10)
		respawnPlayer();
	else if (player.getLastCollision() == "Car") {
		HCHANNEL ch = BASS_SampleGetChannel(sounds[7], FALSE);
		BASS_ChannelSetAttribute(ch, BASS_ATTRIB_FREQ, 0);
		BASS_ChannelSetAttribute(ch, BASS_ATTRIB_VOL, 0.5);
		BASS_ChannelSetAttribute(ch, BASS_ATTRIB_PAN, -1);

		if (!BASS_ChannelPlay(ch, FALSE))
			std::cout << "Can't play sample" << std::endl;

		carRespawnTimer = std::chrono::system_clock::now();
		respawnPlayer();
	}
}

void SceneManager::saveScores(double levelTime, int level) {

	switch (level) {
	case 1:
		tempScore.first = levelTime;
		break;
	case 2:
		tempScore.second = levelTime;
		findHighScores();
		writeScores();
		break;
	default:
		break;
	}
}

void SceneManager::writeScores() {
	std::ofstream highScores1_STREAM("../FoxholeGroupProjV1_1/highScores1.txt");
	std::ofstream highScores2_STREAM("../FoxholeGroupProjV1_1/highScores2.txt");

	for (int i = 0; i < highscores1.size(); i++) {
		highScores1_STREAM << highscores1[i].first << ':' << highscores1[i].second;
		highScores1_STREAM << ';';
		if (i != highscores1.size() - 1) {
			highScores1_STREAM << "#;";
		}
	}

	for (int i = 0; i < highscores2.size(); i++) {
		highScores2_STREAM << highscores2[i].first << ':' << highscores2[i].second;
		highScores2_STREAM << ';';
		if (i != highscores2.size() - 1) {
			highScores2_STREAM << "#;";
		}
	}

	highScores1_STREAM.close();

	highScores2_STREAM.close();
}

void SceneManager::updateCollectables()
{
	if (collectables > 0) {
		for (int i = 0; i < gameObjects.size(); i++) {
			if (gameObjects[i].getName().substr(0, 11) == "collectable") {
				gameObjects[i].setRotation(gameObjects[i].getRotation() + 1.0f);
			}
		}
	}
}

GLuint SceneManager::loadCubeMap(const char * fname[6], GLuint * texID)
{
	glGenTextures(1, texID); // generate texture ID
	GLenum sides[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y };
	SDL_Surface *tmpSurface;

	glBindTexture(GL_TEXTURE_CUBE_MAP, *texID); // bind texture and set parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	GLuint externalFormat;
	for (int i = 0; i < 6; i++)
	{
		// load file - using core SDL library
		tmpSurface = SDL_LoadBMP(fname[i]);
		if (!tmpSurface)
		{
			std::cout << "Error loading bitmap" << std::endl;
			return *texID;
		}

		// skybox textures should not have alpha (assuming this is true!)
		SDL_PixelFormat *format = tmpSurface->format;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGB : GL_BGR;

		glTexImage2D(sides[i], 0, GL_RGB, tmpSurface->w, tmpSurface->h, 0,
			externalFormat, GL_UNSIGNED_BYTE, tmpSurface->pixels);
		// texture loaded, free the temporary buffer
		SDL_FreeSurface(tmpSurface);
	}
	return *texID;	// return value of texure ID, redundant really
}

GLuint SceneManager::loadBitmap(char * fname)
{
	GLuint texID;
	glGenTextures(1, &texID); // generate texture ID

							  // load file - using core SDL library
	SDL_Surface *tmpSurface;
	tmpSurface = SDL_LoadBMP(fname);
	if (!tmpSurface) {
		std::cout << "Error loading bitmap" << std::endl;
	}

	// bind texture and set parameters
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	SDL_PixelFormat *format = tmpSurface->format;

	GLuint externalFormat, internalFormat;
	if (format->Amask) {
		internalFormat = GL_RGBA;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGBA : GL_BGRA;
	}
	else {
		internalFormat = GL_RGB;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGB : GL_BGR;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, tmpSurface->w, tmpSurface->h, 0,
		externalFormat, GL_UNSIGNED_BYTE, tmpSurface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	SDL_FreeSurface(tmpSurface); // texture loaded, free the temporary buffer
	return texID;	// return value of texture ID
}

void SceneManager::findHighScores() {

	highscores1.push_back(std::make_pair(playerName, tempScore.first));
	highscores2.push_back(std::make_pair(playerName, tempScore.second));

	std::sort(highscores1.begin(), highscores1.end(), [](const std::pair<std::string, double> &left, const std::pair<std::string, double> &right) {
		return left.second < right.second;
	});

	std::sort(highscores2.begin(), highscores2.end(), [](const std::pair<std::string, double> &left, const std::pair<std::string, double> &right) {
		return left.second < right.second;
	});

	if (highscores1.size() > 5) {
		highscores1.erase(highscores1.begin() + 5, highscores1.end());
	}
	if (highscores2.size() > 5) {
		highscores2.erase(highscores2.begin() + 5, highscores2.end());
	}

	for (int i = 0; i < highscores1.size(); i++) {
		if (playerName == highscores1[i].first && tempScore.first == highscores1[i].second) {
			highscoreOnLevel1 = true;
			break;
		}
	}

	for (int i = 0; i < highscores2.size(); i++) {
		if (playerName == highscores2[i].first && tempScore.second == highscores2[i].second) {
			highscoreOnLevel2 = true;
			break;
		}
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
