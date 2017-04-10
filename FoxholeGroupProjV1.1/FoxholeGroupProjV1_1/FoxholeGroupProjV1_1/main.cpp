//this is absolute minimum needed

#include "GameManager.h"

int main(int argc, char *argv[]) {

	//ShowWindow(GetConsoleWindow(), SW_HIDE);

	GameManager manager;
	manager.GameInit();
	manager.GameRun();
	manager.GameEnd();

	return 0;
}