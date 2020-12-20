#pragma once

#define dirROR(d)			(((d)+1)&0b11)
#define dirROL(d)			(((d)-1)&0b11)

//#define dirROR(d)			(((d)&1)<<1^((d)^1))
//#define dirROL(d)			(((d)^1)<<1^((d)^1))
#define dirINV(d)			((d)^0b10)
#define dirLR(d)			((d)&1)
#define dirUD(d)			(!((d)&1))
#define dirPOS(d)			((d)==DIR_R||(d)==DIR_D)
#define dirNEG(d)			((d)==DIR_L||(d)==DIR_U)

static inline
int coordMin(const Coord coord)
{
	return coord.c1 < coord.c2 ? coord.c1 : coord.c2;
}

static inline
int coordMax(const Coord coord)
{
	return coord.c1 > coord.c2 ? coord.c1 : coord.c2;
}

static inline
bool coordSame(const Coord pos1, const Coord pos2)
{
	return pos1.x == pos2.x && pos1.y == pos2.y;
}

// Bounds n between min (inclusive) and max (exclusive)
static inline
int clamp(const int n, const int min, const int max)
{
	if(n < min)
		return min;
	if(n >= max)
		return max-1;
	return n;
}

// Returns true if n is between min (inclusive) and max (exclusive)
static inline
bool inBound(const int n, const int min, const int max)
{
	return n >= min && n < max;
}

// Returns true if n is in range
static inline
bool inRange(const int n, const Range range)
{
	return n >= range.min && n < range.max;
}

static inline
int wrap(const int n, const int min, const int max)
{
	if(n < min)
		return max-1;
	if(n >= max)
		return min;
	return n;
}

// Returns true if both axies of coord are in range
static inline
bool coordInRange(const Coord coord, const Range range)
{
	return inRange(coord.x, range) && inRange(coord.y, range);
}

// Returns true if coord is in RangePair
static inline
bool coordInRangePair(const Coord coord, const RangePair range)
{
	return inRange(coord.x, range.x) && inRange(coord.y, range.y);
}

static inline
Rect	coordPairToRect(const Coord coord1, const Coord coord2)
{
	return (Rect){coord1.x,coord1.y,coord2.x-coord1.x,coord2.y-coord1.y};
}

static inline
CoordPair rectToCoordPair(const Rect rect)
{
	return (CoordPair){
		(Coord){rect.x, rect.x+rect.w},
		(Coord){rect.y, rect.y+rect.h}
	};
}


// Returns true if coord is in rect
static inline
bool coordInRect(const Coord coord, const Rect rect)
{
	return coordInRangePair(coord, rectToCoordPair(rect));
}

static inline
bool coordNz(const Coord coord)
{
	return coord.x||coord.y;
}

static inline
Coord coordMul(const Coord coord, const int num)
{
	const Coord ret = {coord.x*num, coord.y*num};
	return ret;
}

static inline
Coord coordOffsetMul(const Coord coord1, const Coord coord2)
{
	return (Coord){coord1.x*coord2.x,coord1.y*coord2.y};
}

static inline
Coordf coordfOffsetMul(const Coordf coord1, const Coordf coord2)
{
	return (Coordf){coord1.x*coord2.x,coord1.y*coord2.y};
}

static inline
Coord coordInv(const Coord coord)
{
	return (Coord){-coord.x, -coord.y};
}

static inline
Coord coordAdd(const Coord coord1, const Coord coord2)
{
	return (Coord){coord1.x+coord2.x,coord1.y+coord2.y};
}

static inline
Coord coordSub(const Coord coord1, const Coord coord2)
{
	return (Offset){coord1.x-coord2.x,coord1.y-coord2.y};
}

static inline
float coordfDist(const Coordf coord1, const Coordf coord2)
{
	return sqrtf(powf(coord2.x-coord1.x,2.0f)+powf(coord2.y-coord1.y,2.0f));
}

static inline
Coordf coordfDiv(const Coordf coord, const float num)
{
	const Coordf ret = {coord.x/num, coord.y/num};
	return ret;
}

static inline
Coordf coordfNormalize(const Coordf coord)
{
	return coordfDiv(coord, coordfDist((Coordf){0.0f,0.0f}, coord));
}

static inline
uint coordDistSq(const Coord coord1, const Coord coord2)
{
	Coord distvec = coordSub(coord2, coord1);
	distvec = coordOffsetMul(distvec, distvec);
	return distvec.x + distvec.y;
}

static inline
Coord coordDiv(const Coord coord, const int num)
{
	const Coord ret = {coord.x/num, coord.y/num};
	return ret;
}

static inline
Coord coordWrap(const Coord coord, const Range x, const Range y)
{
	const Coord ret = {
		wrap(coord.x, x.min, x.max),
		wrap(coord.y, y.min, y.max)
	};
	return ret;
}

static inline
Coord coordClamp(const Coord coord, const Range x, const Range y)
{
	const Coord ret = {
		clamp(coord.x, x.min, x.max),
		clamp(coord.y, y.min, y.max)
	};
	return ret;
}

static inline
Coord coordShift(const Coord coord, const Direction dir, const int units)
{
	Coord ret = coord;
	if(dirUD(dir)){
		ret.y += dirPOS(dir)? units : -units;
	}else{
		ret.x += dirPOS(dir)? units : -units;
	}
	return ret;
}

static inline
Coord coordOffset(const Coord coord, const Offset off)
{
	return coordShift(
		coordShift(
			coord,
			DIR_R,
			off.x
		),
		DIR_D,
		off.y
	);
}

static inline
Offset offsetRor(const Offset off)
{
	const Offset ret = {-off.y, off.x};
	return ret;
}

static inline
Offset offsetRol(const Offset off)
{
	const Offset ret = {off.y, -off.x};
	return ret;
}

static inline
Offset offsetFlip(const Offset off)
{
	const Offset ret = {-off.x, -off.y};
	return ret;
}
