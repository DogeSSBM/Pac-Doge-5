#pragma once

typedef struct{
	Coord pos;
	uint scale;
	Sprite *sprite[4];
	Direction dir;
	bool power;
	Ticks powerEnd;
	bool frozen;
	Ticks frozenEnd;
	bool warpable;
	uint lives;
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
	return keyState(wasd[dir]) || keyState(dpad[dir]);
}

bool dirKeyEx(const Direction dir)
{
	return dirKey(dir) && !dirKey(dirINV(dir));
}

Coord wposToTpos(const Coord wpos, const uint scale)
{
	return coordDiv(wpos, scale);
}

bool tileSolid(const Coord tpos, const Map map)
{
	if(!inBound(tpos.x, 0, map.len.x) || !inBound(tpos.y, 0, map.len.y))
		return true;
	const char c = map.text[tpos.x][tpos.y];
	if(c=='#'||c=='W')
		return true;
	return false;
}

bool tileWarp(const Coord tpos, const Map map)
{
	if(!inBound(tpos.x, 0, map.len.x) || !inBound(tpos.y, 0, map.len.y))
		return false;
	const char c = map.text[tpos.x][tpos.y];
	if(c >= '0' && c <='9')
		return true;
	return false;
}

Coord getOtherWarp(const Coord tpos, const Map map)
{
	const char wc = map.text[tpos.x][tpos.y];
	if(!tileWarp(tpos, map)){
		printf("tried to get other warp of non warp tile\n");
		printf("char '%c' at: (%2u,%2u)\n", wc, tpos.x, tpos.y);
		exit(0);
	}
	for(uint x = 0; x < map.len.x; x++){
		for(uint y = 0; y < map.len.y; y++){
			const Coord other = {x, y};
			if(map.text[x][y]==wc && !coordSame(other, tpos))
				return other;
		}
	}
	return tpos;
}

bool atIntersection(const Coord wpos, const uint scale)
{
	const Coord mod = coordMod(coordAdd(wpos, scale/2), scale);
	if(mod.x==0 && mod.y==0)
		return true;
	return false;
}

Pac shiftPac(Pac pac)
{
	pac.pos = coordShift(pac.pos, pac.dir, 1);
	return pac;
}

Pac movePac(Pac pac, const Map map)
{
	if(pac.frozen){
		if(pac.frozenEnd > getTicks())
			return pac;
		pac.frozen = false;
	}
	if(dirKeyEx(dirINV(pac.dir))){
		pac.dir = dirINV(pac.dir);
		return shiftPac(pac);
	}
	if(atIntersection(pac.pos, pac.scale)){
		const Coord tpos = wposToTpos(pac.pos, pac.scale);
		if(tileWarp(tpos, map)){
			if(pac.warpable){
				pac.pos = tposToWposm(
					getOtherWarp(tpos, map),map.scale
				);
				pac.warpable = false;
				return pac;
			}
		}
		if(dirKeyEx(dirROR(pac.dir)) && !tileSolid(
		coordShift(wposToTpos(pac.pos, pac.scale), dirROR(pac.dir), 1), map
		)){
			pac.dir = dirROR(pac.dir);
			return shiftPac(pac);
		}
		if(dirKeyEx(dirROL(pac.dir)) && !tileSolid(
		coordShift(wposToTpos(pac.pos, pac.scale), dirROL(pac.dir), 1), map
		)){
			pac.dir = dirROL(pac.dir);
			return shiftPac(pac);
		}
		if(tileSolid(
		coordShift(wposToTpos(pac.pos, pac.scale),pac.dir,1), map
		)){
			return pac;
		}
	}else{
		pac.warpable = true;
	}
	return shiftPac(pac);
}

Pac pacInit(const Map map)
{
	return (Pac){
		.pos = tposToWposm(getSpawnCoord(map), map.scale),
		.scale = map.scale,
		.dir = map.spawnDir,
		.power = false,
		.powerEnd = 0,
		.frozen = true,
		.frozenEnd = getTicksIn(4),
		.lives = 2
	};
}
