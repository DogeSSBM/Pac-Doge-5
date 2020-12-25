#pragma once

struct{
	Offset vec;
	Coord pos;
	u32 state;
	struct{
		Coord pos;
		u32 state;
	}prev;
}mouse = {0};

struct{
	u8 key[SDL_NUM_SCANCODES];
	u8 prev[SDL_NUM_SCANCODES];
}keys = {0};

bool keyState(const SDL_Scancode key)
{
	return keys.key[key];
}

bool keyPressed(const SDL_Scancode key)
{
	return keys.key[key] && !keys.prev[key];
}

bool keyHeld(const SDL_Scancode key)
{
	return keys.key[key] && keys.prev[key];
}

bool keyReleased(const SDL_Scancode key)
{
	return !keys.key[key] && keys.prev[key];
}

char getDirKeyChar(const Direction dir)
{
	const char dchar[4] = {'U', 'R', 'D', 'L'};
	return dchar[dir];
}

const char* getDirKeyStr(const Direction dir)
{
	const char *dstr[4] = {"U", "R", "D", "L"};
	return dstr[dir];
}
