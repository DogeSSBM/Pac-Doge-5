#pragma once

typedef struct{
	Coord pos;
	uint scale;
	Img *sprite;
	Direction dir;
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
	if(dirKeyEx(dirINV(pac.dir))){
		pac.dir = dirINV(pac.dir);
		return shiftPac(pac);
	}
	if(atIntersection(pac.pos, pac.scale)){
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
	}
	return shiftPac(pac);
}
