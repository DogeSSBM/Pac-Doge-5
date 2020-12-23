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
	return coordInRangePair(tpos,
			(RangePair){(Range){0,map.len.x},(Range){0,map.len.y}}) &&
		traversable(map.text[tpos.x][tpos.y]);
}

bool* traversableAdjTiles(const Coord tpos, const Map map)
{
	static bool adj[4];
	memset(adj, 0, sizeof(bool));
	for(Direction d = DIR_U; d <= DIR_L; d++)
		adj[d] = traversableAt(coordShift(tpos, d, 1), map);
	return adj;
}

bool* validMoveDirs(const Pac pac, const Map map)
{
	const uint mid = pac.scale/2;
	bool* valid = traversableAdjTiles(pac.tpos, map);
	if(pac.toff != mid){
		valid[dirROL(pac.dir)] = false;
		valid[dirROR(pac.dir)] = false;
	}
	valid[pac.dir] = valid[pac.dir] || pac.toff < mid;
	valid[dirINV(pac.dir)] = valid[dirINV(pac.dir)] || pac.toff > mid;
	return valid;
}

bool* pressedKeyDir(void)
{
	static bool adj[4];
	memset(adj, 0, sizeof(bool));
	for(uint i = 0; i < 4; i++)
		adj[i] = dirKeyEx(i);
	return adj;
}

Pac movePac(Pac pac, const Map map)
{
	const Ticks now = getTicks();
	pac.power = now < pac.powerEnd;
	if(now < (pac.frozen = pac.frozenEnd))
		return pac;

	bool *moveDir = validMoveDirs(pac, map);
	for(uint i = 0; i < 4; i++){
		setColor(moveDir[i]?GREEN:RED);
		fillCircleCoord(coordShift(getPacWpos(pac), i, pac.scale/4),pac.scale/8);
	}
	bool *keyDir = pressedKeyDir();

	for(uint i = 0; i < 4; i++){
		if(moveDir[i] && keyDir[i])
			pac.dir = i;
	}

	if(moveDir[pac.dir]){
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
