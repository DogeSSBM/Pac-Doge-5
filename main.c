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
	const SDL_Scancode keys[4] = {
		SDL_SCANCODE_W,
		SDL_SCANCODE_D,
		SDL_SCANCODE_S,
		SDL_SCANCODE_A
	};
	return keyState(keys[dir]);
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

Pac movePac(Pac pac, const Map map)
{
	const Ticks now = getTicks();
	pac.power = now < pac.powerEnd;
	if(now < (pac.frozen = pac.frozenEnd))
		return pac;

	const uint scale = map.scale;
	const uint hscale = scale/2;

	bool canMove[4] = {0};
	const bool intersection = pac.toff == hscale;
	if(intersection){
		for(Direction dir = DIR_U; dir <= DIR_L; dir++){
			const Coord pos = coordShift(pac.tpos, dir, 1);
			if(!inMap(pos, map.len))
				continue;
			canMove[dir] = traversable(map.text[pos.x][pos.y]);
		}
	}else{
		const Coord fpos = coordShift(pac.tpos, pac.dir, 1);
		if(inMap(fpos, map.len)){
			canMove[pac.dir] =
				traversable(map.text[fpos.x][fpos.y]);
		}
		const Coord bpos = coordShift(pac.tpos, dirINV(pac.dir), 1);
		if(inMap(bpos, map.len)){
			canMove[dirINV(pac.dir)] =
				traversable(map.text[bpos.x][bpos.y]);
		}
	}

	if(intersection){
		if(dirKeyEx(dirROL(pac.dir)) && canMove[dirROL(pac.dir)]){
			pac.dir = dirROL(pac.dir);
			pac.toff++;
			return pac;
		}
		if(dirKeyEx(dirROR(pac.dir)) && canMove[dirROR(pac.dir)]){
			pac.dir = dirROR(pac.dir);
			pac.toff++;
			return pac;
		}
	}

	if(dirKeyEx(dirINV(pac.dir)) && canMove[dirINV(pac.dir)]){
		pac.dir = dirINV(pac.dir);
		pac.toff = pac.scale - pac.toff;
		return pac;
	}
	if(canMove[pac.dir]){
		pac.toff++;
		if(pac.toff <= pac.scale)
			return pac;
		pac.toff = 0;
		pac.tpos = coordShift(pac.tpos, pac.dir, 1);
	}
	return pac;
}

void drawPac(const Pac pac)
{
	const Coord wpos = getPacWpos(pac);
	setColor(YELLOW);
	fillCircleCoord(wpos, pac.scale - pac.scale/4);
	setColor(RED);
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
