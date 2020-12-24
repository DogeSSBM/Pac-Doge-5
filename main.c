#include "Includes.h"

typedef struct{
	Coord tpos;
	Coord toff;
	uint scale;
	Direction dir;
	bool power;
	Ticks powerEnd;
	bool frozen;
	Ticks frozenEnd;
	bool warpable;
}Pac;

typedef union{
	bool arr[4];
	struct{
		bool u;
		bool r;
		bool d;
		bool l;
	};
}Bool4;

bool dirKey(const Direction dir)
{
	const SDL_Scancode wasd[4] = {
		SDL_SCANCODE_W,
		SDL_SCANCODE_D,
		SDL_SCANCODE_S,
		SDL_SCANCODE_A
	};
	const SDL_Scancode dpad[4] = {
		SDL_SCANCODE_UP,
		SDL_SCANCODE_RIGHT,
		SDL_SCANCODE_DOWN,
		SDL_SCANCODE_LEFT
	};
	return keyState(wasd[dir] || dpad[dir]);
}

bool dirKeyEx(const Direction dir)
{
	return dirKey(dir) && !dirKey(dirINV(dir));
}

Coord getPacWpos(const Pac pac)
{
	return coordOffset(tposToWposm(pac.tpos, pac.scale),pac.toff);
}

bool traversable(const char c)
{
	if(c == ' ' || c == '.' || c == 'o' || (c >= '0' && c <= '9'))
		return true;
	return false;
}

Bool4 validAdjTiles(const Pac pac, const Map map)
{
	Bool4 adjt = {0};
	for(uint i = 0; i < 4; i++){
		const Coord t = coordShift(pac.tpos, i, 1);
		adjt.arr[i] =
			inBound(t.x, 0, map.len.x) &&
			inBound(t.y, 0, map.len.y) &&
			traversable(map.text[t.x][t.y]);
	}
	return adjt;
}

Bool4 getDirExKeys(void)
{
	Bool4 dirKeys = {0};
	for(uint i = 0; i < 4; i++)
		dirKeys.arr[i] = dirKeyEx(i);
	return dirKeys;
}

void drawDbg(const Pac pac, const Bool4 validAdjT, const Bool4 dirKeys)
{
	const Coord wpos = getPacWpos(pac);
	setColor(WHITE);
	fillCircleCoord(wpos, pac.scale/8);
	for(uint i = 0; i < 4; i++){
		setColor(validAdjT.arr[i]?GREEN:RED);
		fillCircleCoord(coordShift(wpos, i, pac.scale/8), pac.scale/8);
		setFontColor(dirKeys.arr[i]?GREEN:RED);
		drawTextCenteredCoord(coordShift(wpos, i, pac.scale/4),getDirKeyStr(i));
	}
}

Pac movePac(Pac pac, const Map map)
{
	const Bool4 validAdjT = validAdjTiles(pac, map);
	const Bool4 dirKeys = getDirExKeys();
	drawDbg(pac, validAdjT, dirKeys);
	return pac;
}

void drawPac(const Pac pac)
{
	const Coord wpos = getPacWpos(pac);
	setColor(YELLOW);
	fillCircleCoord(wpos, pac.scale - pac.scale/4);
	setColor(WHITE);
	drawLineCoords(wpos, coordShift(wpos, pac.dir, pac.scale - pac.scale/4));
}

int main(int argc, char const *argv[])
{
	Map map = readMap("Original");
	printMap(map);
	const Length window = coordMul(map.len, map.scale);
	init(window);
	setFontSize(map.scale);

	Pac pac = {
		.tpos = getSpawnCoord(map),
		.toff = {-map.scale/2, 0},
		.scale = map.scale,
		.dir = getSpawnDir(map, pac.tpos),
		.power = false,
		.powerEnd = 0,
		.frozen = false,
		.frozenEnd = 0
	};

	while(1){
		Ticks frameStart = getTicks();
		clear();

		// for(Direction dir = DIR_U; dir <= DIR_L; dir++)
		// 	pos = coordShift(pos, dir, keyState(dirKey[dir])?4:0);

		// drawTextCenteredCoord(pos, "DogeLib :3");
		pac = movePac(pac, map);
		drawMap(map);
		// drawPac(pac);
		draw();
		events(frameStart + TPF);
	}
	return 0;
}
