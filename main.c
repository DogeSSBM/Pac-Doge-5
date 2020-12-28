#include "Includes.h"

int main(int argc, char const *argv[])
{
	Map map = readMap("Original");
	printMap(map);
	const Length window = coordMul(map.len, map.scale);
	init(window);
	setFontSize(map.scale);

	Pac pac = {
		.pos = tposToWposm(getSpawnCoord(map), map.scale),
		.scale = map.scale,
		.dir = getSpawnDir(map,getSpawnCoord(map)),
		.power = false,
		.powerEnd = 0,
		.frozen = false,
		.frozenEnd = 0
	};

	while(1){
		Ticks frameStart = getTicks();
		clear();

		pac = movePac(pac, map);
		drawMap(map);
		drawPac(pac);

		draw();
		events(frameStart + TPF);
	}
	return 0;
}
