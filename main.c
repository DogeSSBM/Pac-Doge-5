#include "Includes.h"

typedef struct{
	Coord tpos;
	uint toff;
	uint scale;
	Direction dir;
	Direction turn;
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
	return coordShift(coordShift(
		tposToWposm(pac.tpos, pac.scale),dirINV(pac.dir),
		pac.scale/2), pac.dir, pac.toff);
}

bool traversable(const char c)
{
	if(c == ' ' || c == '.' || c == 'o' || (c >= '0' && c <= '9'))
		return true;
	return false;
}

bool traversableAt(const Coord tpos, const Map map)
{
	if(inBound(tpos.x, 0, map.len.x) && inBound(tpos.y, 0, map.len.y))
		return traversable(map.text[tpos.x][tpos.y]);
	return false;
}

Bool4 validMoveDirs(const Pac pac, const Map map)
{
	const uint mid = pac.scale/2;
	Bool4 valid = {0};
	valid.arr[dirROR(pac.dir)] = pac.toff == mid && traversableAt(coordShift(pac.tpos, dirROR(pac.dir), 1), map);
	valid.arr[dirROL(pac.dir)] = pac.toff == mid && traversableAt(coordShift(pac.tpos, dirROL(pac.dir), 1), map);
	valid.arr[pac.dir] = pac.toff < mid || traversableAt(coordShift(pac.tpos, pac.dir, 1), map);
	valid.arr[dirINV(pac.dir)] = pac.toff > mid || traversableAt(coordShift(pac.tpos, dirINV(pac.dir), 1), map);
	return valid;
}

Bool4 pressedKeyDir(void)
{
	Bool4 adj;
	for(uint i = 0; i < 4; i++)
		adj.arr[i] = dirKeyEx(i);
	return adj;
}

Pac movePac(Pac pac, const Map map)
{
	const Ticks now = getTicks();
	pac.power = now < pac.powerEnd;
	if(now < (pac.frozen = pac.frozenEnd))
		return pac;

	Bool4 moveDir = validMoveDirs(pac, map);
	for(uint i = 0; i < 4; i++){
		setColor(moveDir.arr[i]?GREEN:RED);
		fillCircleCoord(coordShift(getPacWpos(pac), i, pac.scale/4),pac.scale/8);
	}
	Bool4 keyDir = pressedKeyDir();

	for(uint i = 0; i < 4; i++){
		if(moveDir.arr[i] && keyDir.arr[i])
			pac.dir = i;
	}

	if(moveDir.arr[pac.dir]){
		pac.toff++;
		if(pac.toff >= pac.scale){
			pac.toff = 0;
			pac.tpos = coordShift(pac.tpos, pac.dir, 1);
		}
	}

	return pac;
}

void drawPac(const Pac pac)
{
	const Coord wpos = getPacWpos(pac);
	setColor(YELLOW);
	//fillCircleCoord(wpos, pac.scale - pac.scale/4);
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
		.toff = map.scale/2,
		.scale = map.scale,
		.dir = getSpawnDir(map, pac.tpos),
		.turn = pac.dir,
		.power = false,
		.powerEnd = 0,
		.frozen = true,
		.frozenEnd = getTicksIn(3)
	};
	while(1){
		Ticks frameStart = getTicks();
		clear();

		// for(Direction dir = DIR_U; dir <= DIR_L; dir++)
		// 	pos = coordShift(pos, dir, keyState(dirKey[dir])?4:0);

		// drawTextCenteredCoord(pos, "DogeLib :3");
		pac = movePac(pac, map);
		drawMap(map);
		drawPac(pac);
		draw();
		events(frameStart + TPF);
	}
	return 0;
}
