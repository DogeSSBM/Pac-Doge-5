#include "Includes.h"

bool checkDir(const Map map, const Coord coord, const Direction dir)
{
	const RangePair bound={(Range){0, map.len.x},(Range){0, map.len.y}};
	const Coord adj = coordShift(coord, dir, 1);
	if(!coordInRangePair(adj, bound))
		return false;
	return map.text[adj.x][adj.y] == map.text[coord.x][coord.y];
}

void drawMap(const Map map)
{
	setColor(BLUE);
	for(uint y = 0; y < map.len.y; y++){
		for(uint x = 0; x < map.len.x; x++){
			if(map.text[x][y]!='#')
				continue;
			const Coord mid = {
				x*map.scale+map.scale/2,
				y*map.scale+map.scale/2
			};
			for(Direction dir = DIR_U; dir <= DIR_L; dir++){
				if(checkDir(map, (Coord){x,y}, dir)){
					drawLineCoords(
						mid,
						coordShift(mid,dir,map.scale/2)
					);
				}
			}
		}
	}
}

int main(int argc, char const *argv[])
{
	Map map = readMap("Original");
	printMap(map);
	const Length window = coordMul(map.len, map.scale);
	init(window);
	setFontSize(map.scale);

	const SDL_Scancode dirKey[4] = {
		SDL_SCANCODE_W,
		SDL_SCANCODE_D,
		SDL_SCANCODE_S,
		SDL_SCANCODE_A
	};

	while(1){
		Ticks frameStart = getTicks();
		clear();

		// for(Direction dir = DIR_U; dir <= DIR_L; dir++)
		// 	pos = coordShift(pos, dir, keyState(dirKey[dir])?4:0);

		// drawTextCenteredCoord(pos, "DogeLib :3");
		drawMap(map);
		draw();
		events(frameStart + TPF);
	}
	return 0;
}
