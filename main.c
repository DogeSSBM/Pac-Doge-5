#include "Includes.h"

int main(int argc, char const *argv[])
{
	Map map = readMap("Original", 20);
	printMap(map);
	const Length window = coordMul(map.len, map.scale);
	init(window);
	setFontSize(map.scale);
	Pac pac = pacInit(map);

	while(1){
		Ticks frameStart = getTicks();
		clear();

		pac = movePac(pac, map);
		drawMap(map);
		drawPac(pac, map);

		draw();
		events(frameStart + TPF);
	}
	return 0;
}
