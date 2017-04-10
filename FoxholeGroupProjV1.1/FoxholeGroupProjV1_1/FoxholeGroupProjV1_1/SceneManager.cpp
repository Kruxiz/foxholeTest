#include "SceneManager.h"


#define DEG_TO_RADIAN 0.017453293

SceneManager::SceneManager() {
	level = 1;
	eye = glm::vec3(0.0f, 1.0f, 0.0f);
	at = glm::vec3(0.0f, 1.0f, -1.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	skyboxProgram = rt3d::initShaders("../FoxholeGroupProjV1_1/cubeMap.vert", "../FoxholeGroupProjV1_1/cubeMap.frag");

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

			HCHANNEL ch = BASS_SampleGetChannel(sounds[0], TRUE); //todo true i think??
			BASS_ChannelFlags(ch, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
			if (!BASS_ChannelPlay(ch, TRUE))
				std::cout << "Can't play sample - " << BASS_ErrorGetCode() << std::endl;

			level = 1;
			initGameObjects();
			respawnPlayer();
		}
		else {
			HCHANNEL ch = BASS_SampleGetChannel(sounds[0], FALSE); //todo true i think??
			BASS_ChannelFlags(ch, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
			if (!BASS_ChannelPlay(ch, TRUE))
				std::cout << "Can't play sample - " << BASS_ErrorGetCode() << std::endl;

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
	BASS_Pause();
	highscoreOnLevel1 = false;
	highscoreOnLevel2 = false;
	sceneState = MAIN_MENU;
	/*if (highscores1.size() == 0 && highscores2.size() == 0)
		loadScores();*/

}

void SceneManager::loadScores() {
	//todo syntax = 3 chars then ; then double level time
	std::ifstream highScores1_STREAM("../FoxholeGroupProjV1_1/highScores1.txt");
	std::ifstream highScores2_STREAM("../FoxholeGroupProjV1_1/highScores2.txt");

	//todo add error checking
	if (highScores1_STREAM && highScores2_STREAM) {
		std::string username;
		std::string userTimeStr;
		//std::string blank;
		std::string hash;
		double userTime;
		std::stringstream ss;
		bool continueReading = true;
		//char name1, name2, name3;
		while (continueReading) {
			std::getline(highScores1_STREAM, username, ':');

			std::getline(highScores1_STREAM, userTimeStr, ';');
			ss.str(userTimeStr);
			ss >> userTime;

			highscores1.push_back(std::make_pair(username, userTime));

			ss.str("");
			ss.clear();
			//std::getline(highScores1_STREAM, blank);
			std::getline(highScores1_STREAM, hash, ';');
			if (hash != "#")
				continueReading = false;
		}
		//ss >> name1 >> name2 >> name3;
		continueReading = true;
		while (continueReading) {
			std::getline(highScores2_STREAM, username, ':');

			std::getline(highScores2_STREAM, userTimeStr, ';');
			ss.str(userTimeStr);
			ss >> userTime;

			highscores2.push_back(std::make_pair(username, userTime));

			ss.str("");
			ss.clear();
			//std::getline(highScores2_STREAM, blank);
			std::getline(highScores2_STREAM, hash, ';');
			if (hash != "#")
				continueReading = false;
		}
		//ss >> name1 >> name2 >> name3;

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
	default://todo maybe add something else here
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

	MenuObject charUnderline;/* = std::make_tuple("_", glm::vec3(-0.1f, -0.1f, 0.0f), glm::vec3(0.1f, 0.1f, 0.0f));
	//renderHUDObject(charUnderline);*/

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

void SceneManager::playerUpdate(bool spaceUp)
{
	checkPlayerRespawn();
	playerFall(spaceUp);
	detectCollectableCollision();
	checkEndLevel();
}

void SceneManager::chooseNameAndPlay() {
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

//todo call this method when player selects scores menu
//this method gets top five scores and displays
void SceneManager::addToScores() {
	//todo addToScores - only top 5?
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
		//renderHUDObject(nameRender);
		//pos.x -= 0.2f;
		//auto scoreRender = std::make_tuple(std::to_string(score.second), pos, scale);
		renderHUDObject(scoreRender);
		pos.y -= step;
		//pos.x += 0.2f;
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
		//renderHUDObject(nameRender);
		//pos.x -= 0.2f;
		//auto scoreRender = std::make_tuple(std::to_string(score.second), pos, scale);
		renderHUDObject(scoreRender);
		pos.y -= step;
		//pos.x += 0.2f;
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

	textFont = TTF_OpenFont("../FoxholeGroupProjV1_1/MavenPro-Regular.ttf", 24);
	if (textFont == NULL)
		std::cout << "Failed to open font." << std::endl;
}

void SceneManager::init()
{

	initTTF();

	initSounds(); //todo play menu music - play forest sounds when playing game

	shaderProgram = rt3d::initShaders("../FoxholeGroupProjV1_1/phong-tex.vert", "../FoxholeGroupProjV1_1/phong-tex.frag");
	textureProgram = rt3d::initShaders("../FoxholeGroupProjV1_1/textured.vert", "../FoxholeGroupProjV1_1/textured.frag");

	//rt3d::setLight(shaderProgram, lights[0]);
	//rt3d::setMaterial(shaderProgram, materials[0]);

	//loading skybox
	//https://opengameart.org/content/forest-skyboxes
	const char *cubeTexFiles[6] = {
		"../FoxholeGroupProjV1_1/Brudslojan/posz.bmp", "../FoxholeGroupProjV1_1/Brudslojan/negz.bmp",
		"../FoxholeGroupProjV1_1/Brudslojan/posx.bmp", "../FoxholeGroupProjV1_1/Brudslojan/negx.bmp", 
		"../FoxholeGroupProjV1_1/Brudslojan/posy.bmp", "../FoxholeGroupProjV1_1/Brudslojan/posy.bmp"
	};

	GameManager::loadCubeMap(cubeTexFiles, skybox);
	const char *cubeTexFiles2[6] = {
		"../FoxholeGroupProjV1_1/Town-skybox/Town_bk.bmp", "../FoxholeGroupProjV1_1/Town-skybox/Town_ft.bmp", 
		"../FoxholeGroupProjV1_1/Town-skybox/Town_rt.bmp", "../FoxholeGroupProjV1_1/Town-skybox/Town_lf.bmp",
		"../FoxholeGroupProjV1_1/Town-skybox/Town_up.bmp", "../FoxholeGroupProjV1_1/Town-skybox/Town_dn.bmp"
	};
	GameManager::loadCubeMap(cubeTexFiles2, skybox2);

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

	verts.clear();
	norms.clear();
	tex_coords.clear();
	indices.clear();

	rt3d::loadObj("../FoxholeGroupProjV1_1/fox.obj", verts, norms, tex_coords, indices);
	meshIndexCount = indices.size();
	//player.setMeshIndexCount(indices.size());

	meshObjects.push_back(rt3d::createMesh(verts.size() / 3,
		verts.data(), nullptr, norms.data(),
		tex_coords.data(), meshIndexCount,
		indices.data()));

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

	//md2model tmpmodel;
	meshObjects.push_back(foxModel.ReadMD2Model("../FoxholeGroupProjV1_1/starfox.md2"));
	player.setMeshIndexCount(foxModel.getVertDataCount());

	//add more meshes with rt3d::loadObj("*.obj", verts, norms, tex_coords, indices); where * is the obj name
	//then meshIndexCount = indices.size();
	//then meshObjects.push_back(rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), meshIndexCount, indices.data()));

	textures.push_back(GameManager::loadBitmap("../FoxholeGroupProjV1_1/fabric.bmp"));
	textures.push_back(GameManager::loadBitmap("../FoxholeGroupProjV1_1/water.bmp"));
	textures.push_back(GameManager::loadBitmap("../FoxholeGroupProjV1_1/box.bmp"));
	textures.push_back(GameManager::loadBitmap("../FoxholeGroupProjV1_1/twigs.bmp"));
	textures.push_back(GameManager::loadBitmap("../FoxholeGroupProjV1_1/Town-skybox/grass1.bmp"));
	textures.push_back(GameManager::loadBitmap("../FoxholeGroupProjV1_1/orange-fox.bmp"));
	textures.push_back(GameManager::loadBitmap("../FoxholeGroupProjV1_1/tree.bmp"));
	textures.push_back(GameManager::loadBitmap("../FoxholeGroupProjV1_1/starfox.bmp"));
	//add more textures with textures.push_back(SDLManager::loadBitmap("*.bmp")); where * is the bitmap name

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
	//timer = std::chrono::system_clock::now(); //todo init timer when starting to actually play game
	//pauseTimer = timer;

	if (level == 1) {
		//level 1

		gameObjects.push_back(GameObject("LevelEnd", glm::vec3(0.0f, 0.0f, -180.f), glm::vec3(25.0f, 20.0f, 5.0f), NULL, NULL));

		gameObjects.push_back(GameObject("InvisibleWallRight", glm::vec3(16.0f, 0.0f, -80.0f), glm::vec3(1.0f, 20.0f, 100.0f), NULL, NULL));
		gameObjects.push_back(GameObject("InvisibleWallLeft", glm::vec3(-26.0f, 0.0f, -80.0f), glm::vec3(1.0f, 20.0f, 100.0f), NULL, NULL));
		gameObjects.push_back(GameObject("InvisibleWallBack", glm::vec3(0.0f, 0.0f, -180.f), glm::vec3(30.0f, 20.0f, 1.0f), NULL, NULL));
		gameObjects.push_back(GameObject("InvisibleWallFront", glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(30.0f, 20.0f, 0.5f), NULL, NULL));

		gameObjects.push_back(GameObject("Ground", glm::vec3(-5.0f, -0.1f, -100.0f), glm::vec3(200.0f, 0.1f, 200.0f), textures[4], meshObjects[0]));

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
		gameObjects.push_back(GameObject("trap_ground", glm::vec3(0.0f, -1.0f, 1.5f), glm::vec3(5.0f, 1.0f, 5.0f), textures[4], meshObjects[0]));
		for (int a = 0; a < 99; a++) {

			int randomNum1 = rand() % 100 + 1;
			int randomNum2 = rand() % 100 + 1;
			gameObjects.push_back(GameObject("trap_ground", glm::vec3(randomNum1, -1.0f, randomNum2), glm::vec3(5.0f, 1.0f, 5.0f), textures[4], meshObjects[0]));

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

//todo may deprecate
void SceneManager::checkEndLevel()
{
	if (level == 2 && collectables == 0) {
		//level = 1;
		//timer = std::chrono::system_clock::now();
		//pauseTimer = timer;
		//initGameObjects();
		//respawnPlayer();
		saveScores(levelTime, 2);
		playerNameSet = false;
		playerName = "";
		activeChar = 1;
		if (highscoreOnLevel1 || highscoreOnLevel2)
			scores();
		else
			mainMenu();
		//todo change scene state here ?? maybe wait no - call method that 
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
	//todo add countdown for death respawn and initial play

	////////////////////////////////////////////////////////////////////
	//This renders a HUD label
	////////////////////////////////////////////////////////////////////
	//glUseProgram(textureProgram);//Use texture-only shader for text rendering
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

		//glUseProgram(textureProgram);//Use texture-only shader for text rendering
		renderHUDObject(std::make_tuple(countdownStr, glm::vec3(0.0f, -0.1f, 0.0f), glm::vec3(0.25f, 0.25f, 0.0f)));

		currentAnimation = 0;
	}
	else {
		totalTime -= 3;

		std::string timerStr = "Time: ";

		if (totalTime < 0)
			totalTime = 0;

		timerStr.append(std::to_string(totalTime));

		timerStr.append("s");

		//glUseProgram(textureProgram);//Use texture-only shader for text rendering
		renderHUDObject(std::make_tuple(timerStr, glm::vec3(-0.8f, 0.8f, 0.0f), glm::vec3(0.2f, 0.2f, 0.0f)));
		levelTime = totalTime;

	}

	if (level == 2) {
		std::string collectablesString("Collectables left: ");
		collectablesString.append(std::to_string(collectables));

		//glUseProgram(textureProgram);//Use texture-only shader for text rendering
		renderHUDObject(std::make_tuple(collectablesString, glm::vec3(-0.5f, -0.8f, 0.0f), glm::vec3(0.5f, 0.2f, 0.0f)));

	}

	int respawnTime = std::chrono::duration<double>(temp - respawnTimer).count();

	if (respawnTime > -1 && respawnTime < 2) {
		//glUseProgram(textureProgram);//Use texture-only shader for text rendering
		renderHUDObject(std::make_tuple("Avoid the water!", glm::vec3(0.8f, -0.8f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f)));
	}

	glEnable(GL_DEPTH_TEST);//Re-enable depth test after HUD label
	//setLights();
}

void SceneManager::renderPlayer()
{
	if (!paused()) {
		foxModel.Animate(currentAnimation, 0.1);
		rt3d::updateMesh(player.getMesh(), RT3D_VERTEX, foxModel.getAnimVerts(), foxModel.getVertDataSize());
	}
	//player cube
	glCullFace(GL_FRONT);
	glBindTexture(GL_TEXTURE_2D, player.getTexture());
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(player.getPos().x, player.getPos().y, player.getPos().z));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(0.05f, 0.05f, 0.05f));

	//change r based on current r
	mvStack.top() = glm::rotate(mvStack.top(), float((player.getPlayerR() - 90.0f)*DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
	mvStack.top() = glm::rotate(mvStack.top(), float(270 * DEG_TO_RADIAN), glm::vec3(1.0f, 0.0f, 0.0f));
	mvStack.top() = glm::rotate(mvStack.top(), float(180 * DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));

	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	//rt3d::setMaterial(shaderProgram, materials[1]);
	rt3d::drawMesh(player.getMesh(), player.getMeshIndexCount(), GL_TRIANGLES);
	mvStack.pop();
	glCullFace(GL_BACK);

	//foxModel->Draw(modelProgram);
	/*
	// Animate the md2 model, and update the mesh with new vertex data
	foxModel.Animate(currentAnim,0.1);
	rt3d::updateMesh(player.getMesh(),RT3D_VERTEX,foxModel.getAnimVerts(),foxModel.getVertDataSize());
	*/
}

void SceneManager::updatePlayerR(GLfloat deltaR)
{
	player.setPlayerR(player.getPlayerR() + deltaR);
}

void SceneManager::detectCollectableCollision() {

	if (level == 2) {
		std::string playerColl = player.getLastCollision();
		if (player.getLastCollision().substr(0, 11) == "collectable") {
			//playerColl = playerColl.substr(11);

			//for (GameObject gObj : gameObjects) {
				//if (gObj.getName().substr(0, 11) == "collectable" && gObj.getName().substr(11) == playerColl) {
			std::cout << "player collided with collectable\n";
			int index = getGameObjectIndex(player.getLastCollision());
			player.setLastCollision("");
			gameObjects.erase(gameObjects.begin() + index);
			collectables--;
			//break;
		//}
	//}
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

	sounds.push_back(loadSounds("../FoxholeGroupProjV1_1/forest-environment.wav"));
	//http://soundbible.com/1818-Rainforest-Ambience.html

	sounds.push_back(loadSounds("../FoxholeGroupProjV1_1/splash.wav"));
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

void SceneManager::standingAnimation() {
	currentAnimation = 0;
}

void SceneManager::movePlayerForward(GLfloat delta) {

	glm::vec3 temp = player.getPos();

	player.setPos(moveForward(player.getPos(), -player.getPlayerR(), delta / getTimeScalar()));

	if (checkCollisions())
		player.setPos(temp);

	currentAnimation = 1;
}

void SceneManager::movePlayerRight(GLfloat delta) {

	glm::vec3 temp = player.getPos();

	player.setPos(moveRight(player.getPos(), -player.getPlayerR(), delta / getTimeScalar()));

	if (checkCollisions())
		player.setPos(temp);

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
	//Player tempPlayer = player;
	//tempPlayer.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
	//tempPlayer.setPos(glm::vec3(tempPlayer.getPos().x, tempPlayer.getPos().y + 1.0f, tempPlayer.getPos().z));
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
	/*if (player.getJumpCounter() < player.getJumpMax()) {
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
	}*/

	player.playerJump();

	/*if (player.isPlayerJumping()) {
		player.setPos(glm::vec3(player.getPos().x, player.getPos().y + player.getJumpIncrement(), player.getPos().z));
	}*/
}

void SceneManager::playerFall(bool spaceUp) {

	//if (CollisionDetector::detectCollision(getGameObject("Cube1"), player))
		//std::cout << "player collided with cube1\n";
	//if (player.isPlayerFalling()) {
	//	if (!checkCollisions()) {
	//		glm::vec3 newPos = player.getPos();
	//		newPos.y -= player.getFallIncrement();

	//		GameObject tempObj(player);

	//		//tempObj.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
	//		tempObj.setPos(newPos);
	//		//tempObj.setPos(glm::vec3(tempObj.getPos().x, tempObj.getPos().y+1.0f, tempObj.getPos().z));

	//		if (!checkCollisions(tempObj)) { player.setIsOnObj(false); }
	//	}

	//	//if (!player.isJumping() && !player.isOnObject()) {

	//		glm::vec3 newPos = player.getPos();

	//		if (checkCollisions()) {
	//			//change player y to y of object y plus object scale
	//			GameObject gObj = getGameObject(player.getLastCollision());
	//			std::cout << "player last coll:\t " << player.getLastCollision() << std::endl;
	//			newPos.y = gObj.getPos().y + gObj.getScale().y + player.getScale().y;

	//			player.setIsOnObj(true);
	//		}
	//		else {
	//			newPos.y -= player.getFallIncrement();
	//		}

	//		std::cout << "player y:\t" << player.getPos().y << std::endl;
	//		player.setPos(newPos);
	//		//CHANGE THIS ^^^^^ CREATE TEMP OBJECT ////MAYBE?????? LOLIDK
	//	//}
	//}
	////

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
	return GameObject("null", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0, 0);
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
	}//for demonstration purposes
	else if (player.getLastCollision() == "LevelEnd" /*|| (!inCountdown() && */) {
		level = 2;
		timer = std::chrono::system_clock::now();
		pauseTimer = timer;
		initGameObjects();
		respawnPlayer();
		saveScores(levelTime, 1);
	}
	else if (player.getPos().y < -10)
		respawnPlayer();
}

void SceneManager::saveScores(double levelTime, int level) {
	//std::ofstream highScores1("highScores1.txt", std::ios_base::app);
	//std::ofstream highScores2("highScores2.txt", std::ios_base::app);
	//highScores << levelTime, /*input name function elsewhere and have the string passed here for use*/"\n";
	//highScores << /*scorevariable here*/ "\n";

	//todo change this so that it saves level time in temp score (add param to know whether to store
	// as first or second)
	// after level 2 is complete, save to file.
	// read file and check if either score is in top five
	// if so, let player choose 3 letters to save score
	// to do this, add new scene state with accompanying menu
	// todo figure out later

	switch (level) {
	case 1:
		tempScore.first = levelTime;
		break;
	case 2:
		tempScore.second = levelTime;
		//highScores1 << tempScore.first << "\n";
		//highScores2 << tempScore.second << "\n";

		//highScores1.close();
		//highScores2.close();

		//call method to 
		findHighScores();
		writeScores();
		break;
	default:
		break;
	}

	//todo not sure about below
	//if(highScores1.is_open())
		//highScores1.close();

	//if (highScores2.is_open())
		//highScores2.close();
}

void SceneManager::writeScores() {
	std::ofstream highScores1_STREAM("../FoxholeGroupProjV1_1/highScores1.txt");
	std::ofstream highScores2_STREAM("../FoxholeGroupProjV1_1/highScores2.txt");


	//highScores1_STREAM << 

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

	//todo not sure about below
	//if (highScores1_STREAM.is_open())
	highScores1_STREAM.close();

	//if (highScores2_STREAM.is_open())
	highScores2_STREAM.close();
}

void SceneManager::findHighScores() {
	//std::ofstream highScores1_STREAM("highScores1.txt", std::ios_base::app);
	//std::ofstream highScores2_STREAM("highScores2.txt", std::ios_base::app);

	//if (highScores1.size() == 0 && highScores2.size() == 0) {
		//loadScores();
	//}

	/*if (!highScores1) {
		std::cerr << "highScores1 failed to load\n";
	}

	if (!highScores2) {
		std::cerr << "highScores2 failed to load\n";
	}*/

	//add to temp vectors of scores
	//sort them


	//todo not sure about below
	//if (highScores1_STREAM.is_open())
		//highScores1_STREAM.close();

	//if (highScores2_STREAM.is_open())
		//highScores2_STREAM.close();

	//

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

	//todo clear tempscore
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
