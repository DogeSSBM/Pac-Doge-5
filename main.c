#include "Includes.h"

typedef struct{
	Coord pos;
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
	return keyState(wasd[dir]) || keyState(dpad[dir]);
}

bool dirKeyEx(const Direction dir)
{
	return dirKey(dir) && !dirKey(dirINV(dir));
}

Coord wposToTpos(const Pac pac)
{
	return coordDiv(pac.pos, pac.scale);
}

bool traversable(const Coord tpos, const Map map)
{
	if(!inBound(tpos.x, 0, map.len.x) || !inBound(tpos.y, 0, map.len.y))
		return false;
	const char c = map.text[tpos.x][tpos.y];
	if(c==' '||c=='.'||c=='o'||c=='f'||c=='F'||c=='P'||(c >= '0' && c <= '9'))
		return true;
	return false;
}

Bool4 validAdjDir(const Pac pac, const Map map)
{
	char buf[] = " ";
	Bool4 adjt = {0};
	for(uint i = 0; i < 4; i++){
		const Coord t = coordShift(wposToTpos(pac), i, 1);
		adjt.arr[i] = traversable(t, map);
		setFontColor(adjt.arr[i]?GREEN:RED);
		sprintf(buf, "%c", map.text[t.x][t.y]);
		drawTextCenteredCoord(coordShift(pac.pos,i,map.scale*2), buf);
	}
	return adjt;
}

Coord tileOffset(const Pac pac)
{
	return coordMod(coordAdd(pac.pos, pac.scale/2), pac.scale);
}

bool atIntersection(const Pac pac)
{
	const Coord mod = tileOffset(pac);
	if(mod.x==0 && mod.y==0)
		return true;
	return false;
}

Bool4 getDirExKeys(void)
{
	Bool4 dirKeys = {0};
	for(uint i = 0; i < 4; i++)
		dirKeys.arr[i] = dirKeyEx(i);
	return dirKeys;
}

void drawDbg(const Pac pac, const Bool4 validAdjT, const Bool4 dirKeys, const bool intersection)
{
	setColor(WHITE);
	fillCircleCoord(pac.pos, pac.scale/8);
	for(uint i = 0; i < 4; i++){
		setColor(validAdjT.arr[i]?(intersection?GREEN:BLUE):RED);
		fillCircleCoord(coordShift(pac.pos, i, pac.scale/2), pac.scale/8);
		setFontColor(dirKeys.arr[i]?GREEN:RED);
		drawTextCenteredCoord(coordShift(pac.pos, i, pac.scale),getDirKeyStr(i));
	}
}

Pac movePac(Pac pac, const Map map)
{
	const Bool4 validDir = validAdjDir(pac, map);
	const Bool4 dirKeys = getDirExKeys();
	const bool intersection = atIntersection(pac);
	const Coord mod = tileOffset(pac);
	drawDbg(pac, validDir, dirKeys, atIntersection(pac));
	const Direction oldDir = pac.dir;
	if(dirKeys.arr[dirINV(pac.dir)] && validDir.arr[dirINV(pac.dir)])
		pac.dir = dirINV(pac.dir);
	if(intersection){
		if(dirKeys.arr[dirROL(pac.dir)] && validDir.arr[dirROL(pac.dir)])
			pac.dir = dirROL(pac.dir);
		if(dirKeys.arr[dirROR(pac.dir)] && validDir.arr[dirROR(pac.dir)])
			pac.dir = dirROR(pac.dir);
	}
	char buf[32] = {0};
	// setFontColor(local?GREEN:RED);
	setFontColor(WHITE);
	sprintf(buf, "(% 3d,% 3d)", mod.x, mod.y);
	drawTextCoord((Coord){16,16}, buf);

	static bool fadv = false;
	if(keyPressed(SDL_SCANCODE_P))
		fadv = !fadv;
	if(!fadv || keyPressed(SDL_SCANCODE_SPACE)){
		if(validDir.arr[pac.dir] || (!intersection&&dirAXP(oldDir, pac.dir)))
			pac.pos = coordShift(pac.pos, pac.dir, 1);
	}
	return pac;
}

void drawPac(const Pac pac)
{
	setColor(YELLOW);
	fillCircleCoord(pac.pos, pac.scale - pac.scale/4);
	setColor(WHITE);
	drawLineCoords(pac.pos, coordShift(pac.pos, pac.dir, pac.scale - pac.scale/4));
}

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
		.dir = DIR_L,
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
