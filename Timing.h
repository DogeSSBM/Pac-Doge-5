#pragma once
#define FPS		60
#define TPF		(1000/FPS)
#define TPS		((TPF)*(FPS))

Ticks secToTicks(const uint sec)
{
	return sec*TPS;
}

Ticks getTicks(void)
{
	return SDL_GetTicks();
}

Ticks getTicksIn(const uint sec)
{
	return getTicks()+secToTicks(sec);
}
