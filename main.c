#include "Includes.h"

int main(int argc, char const *argv[])
{
	Map map = readMap("Original");
	printMap(map);
	const Length window = {800, 600};
	init(window);
	setFontSize(48);
	Coord pos = coordDiv(window, 2);
	const SDL_Scancode dirKey[4] = {
		SDL_SCANCODE_W,
		SDL_SCANCODE_D,
		SDL_SCANCODE_S,
		SDL_SCANCODE_A
	};


	while(1){
		Ticks frameStart = getTicks();
		clear();

		for(Direction dir = DIR_U; dir <= DIR_L; dir++)
			pos = coordShift(pos, dir, keyState(dirKey[dir])?4:0);

		drawTextCenteredCoord(pos, "DogeLib :3");

		draw();
		events(frameStart + TPF);
	}
	return 0;
}
