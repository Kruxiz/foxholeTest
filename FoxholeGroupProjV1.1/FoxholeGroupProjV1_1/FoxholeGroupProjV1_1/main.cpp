//this is absolute minimum needed

#include "GameManager.h"

// Program entry point - SDL manages the actual WinMain entry point for us
int main(int argc, char *argv[]) {

	GameManager manager;
	manager.GameInit();
	manager.GameRun();
	manager.GameEnd();

	return 0;
}