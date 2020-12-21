#include "Includes.h"

Coord tileScreenMid(const Coord tile, const uint scale)
{
	return coordAdd(coordMul(tile, scale), scale/2);
}

bool checkDir(const Map map, const Coord coord, const Direction dir)
{
	const RangePair bound={(Range){0, map.len.x},(Range){0, map.len.y}};
	const Coord adj = coordShift(coord, dir, 1);
	if(!coordInRangePair(adj, bound))
		return false;
	return map.text[adj.x][adj.y] == map.text[coord.x][coord.y];
}

void drawWall(const Map map, const Coord coord)
{
	setColor(BLUE);
	const Coord mid = tileScreenMid(coord, map.scale);
	for(Direction dir = DIR_U; dir <= DIR_L; dir++){
		if(checkDir(map, (Coord){coord.x,coord.y}, dir)){
			drawLineCoords(
				mid,
				coordShift(mid,dir,map.scale/2)
			);
		}
	}
}

void drawDot(const Map map, const Coord coord, const bool big)
{
	setColor(YELLOW);
	fillCircleCoord(
		tileScreenMid(coord, map.scale),
		big ? map.scale/3 : map.scale/6
	);
}

void drawMap(const Map map)
{
	for(uint y = 0; y < map.len.y; y++){
		for(uint x = 0; x < map.len.x; x++){
			switch(map.text[x][y]){
				case '#':
					drawWall(map, (Coord){x,y});
					break;
				case '.':
					drawDot(map, (Coord){x,y}, false);
					break;
				case 'o':
					drawDot(map, (Coord){x,y}, true);
					break;
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
