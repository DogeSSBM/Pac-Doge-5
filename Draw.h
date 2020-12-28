#pragma once

void drawWall(const Map map, const Coord coord)
{
	setColor(BLUE);
	const Coord mid = tposToWposm(coord, map.scale);
	for(Direction dir = DIR_U; dir <= DIR_L; dir++){
		if(checkDir(
			map,(Coord){coord.x,coord.y},
			dir,map.text[coord.x][coord.y])
		){
			fillRectCoords(
				coordShift(
					mid,
					dirROR(dir),
					map.scale/8
				),
				coordShift(
					coordShift(mid,dir,map.scale/2),
					dirROL(dir),
					map.scale/8
				)
			);
		}
	}
}

void drawDot(const Map map, const Coord coord, const bool big)
{
	setColor(YELLOW);
	fillCircleCoord(
		tposToWposm(coord, map.scale),
		big ? map.scale/3 : map.scale/6
	);
}

void drawMap(const Map map)
{
	for(uint y = 0; y < map.len.y; y++){
		for(uint x = 0; x < map.len.x; x++){
			switch(map.text[x][y]){
				case '#':
					drawWall(map, (Coord){x,y});
					break;
				case '.':
					drawDot(map, (Coord){x,y}, false);
					break;
				case 'o':
					drawDot(map, (Coord){x,y}, true);
					break;
			}
		}
	}
}

void drawPac(const Pac pac)
{
	setColor(YELLOW);
	fillCircleCoord(pac.pos, pac.scale/2);
	setColor(WHITE);
	drawLineCoords(pac.pos, coordShift(pac.pos, pac.dir, pac.scale - pac.scale/4));
}
