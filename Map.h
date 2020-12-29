#pragma once

typedef struct{
	Length len;
	uint scale;
	char **text;
	Coord spawn;
	Direction spawnDir;
	Coord fruitpos;
}Map;

bool inMap(const Coord coord, const Length len)
{
	return coordInRangePair(
		coord,
		(RangePair){(Range){0, len.x},(Range){0, len.y}}
	);
}

Coord tposToWposm(const Coord tile, const uint scale)
{
	return coordAdd(coordMul(tile, scale), scale/2);
}

Coord getFruitSpawn(const Map map)
{
	bool found = false;
	CoordPair pair = {(Coord){0,0}, (Coord){0,0}};
	for(uint y = 0; y < map.len.y; y++){
		for(uint x = 0; x < map.len.x; x++){
			const Coord coord = (Coord){x, y};
			if(map.text[x][y] == 'F'){
				if(found){
					if(!coordSame(coord, pair.pos1) &&
					!coordSame(coord, pair.pos2)){
						printf("Incorrect F found at: (%2u,%2u)\n",
						x, y);
						exit(0);
					}
				}else{
					pair.pos1.x = x;
					pair.pos1.y = y;
					for(uint i = 0; i < 4; i++){
						const Coord adj = coordShift(coord,i,1);
						if(inMap(adj,map.len) &&
						map.text[adj.x][adj.y]=='F'){
							pair.pos2 = adj;
							found = true;
							break;
						}
					}
				}
			}
		}
	}
	if(!found){
		printf("Could not find Fruit\n");
		exit(0);
	}
	pair.pos1 = tposToWposm(pair.pos1, map.scale);
	pair.pos2 = tposToWposm(pair.pos2, map.scale);
	return coordPairMid(pair);
}

Coord getSpawnCoord(const Map map)
{
	bool found = false;
	Coord ret = {-1, -1};
	for(uint y = 0; y < map.len.y; y++){
		for(uint x = 0; x < map.len.x; x++){
			if(map.text[x][y] == 'P'){
				if(found){
					printf("There can be only one spawn\n");
					printf("Spawn 1: (%2u,%2u)\n", ret.x, ret.y);
					printf("Spawn 2: (%2u,%2u)\n", x, y);
					exit(0);
				}
				ret.x = x;
				ret.y = y;
				found = true;
			}
		}
	}
	if(!found){
		printf("Could not find spawn\n");
		exit(0);
	}
	return ret;
}

Direction getSpawnDir(const Map map)
{
	for(Direction dir = DIR_U; dir <= DIR_L; dir++){
		const Coord adj = coordShift(map.spawn, dir, 1);
		if(inMap(adj, map.len) && map.text[adj.x][adj.y] == 'f')
			return dir;
	}
	printf("Spawn must have an adjacent 'f'\n");
	exit(0);
	return DIR_L;
}

uint getMapLineLen(File *mapFile)
{
	int c;
	uint len = 0;
	while((c = fgetc(mapFile)) != '|'){
		len++;
		if(c == EOF){
			printf("Error reached unexpected end of map file\n");
			exit(0);
		}
	}
	if((c = fgetc(mapFile)) != '\n'){
		printf("Expected newline after '|'\n");
		exit(0);
	}
	return len;
}

Length getMapLen(File *mapFile)
{
	Length len = {0, 0};
	int c;
	while(1){
		c = fgetc(mapFile);
		if(c == '$')
			break;
		len.y++;
		if(c != '|'){	// '|' should be first char of line
			printf("Error reading map file line %u\n", len.y);
			printf("Exceeded '|' at start of line\n");
			exit(0);
		}
		const uint xlen = getMapLineLen(mapFile);
		// printf("line %2u len: %2u\n", len.y, xlen);
		if(len.x != 0){
			if(xlen != len.x){
				printf("Error reading map file line %u\n", len.y);
				printf("Exceeded expected line length: %u\n", len.x);
				exit(0);
			}
		}else{
			len.x = xlen;
		}
	}
	printf("File len (%2u,%2u)\n", len.x, len.y);
	return len;
}

void printMap(const Map map)
{
	for(uint y = 0; y < map.len.y; y++){
		for(uint x = 0; x < map.len.x; x++){
			fputc(map.text[x][y], stdout);
		}
		fputc('\n', stdout);
	}
	fputc('\n', stdout);
}

char** parseMap(File *mapFile, const Length len)
{
	printf("Allocating mem\n");

	char **text = malloc(sizeof(char*)*len.x);
	for(uint x = 0; x < len.x; x++){
		text[x] = malloc(sizeof(char)*len.y);
		memset(text[x], 0, sizeof(char)*len.y);
	}

	rewind(mapFile);
	for(uint y = 0; y < len.y; y++){
		if(fgetc(mapFile) != '|'){
			printf("asdf1\n");
			exit(0);
		}
		for(uint x = 0; x < len.x; x++){
			text[x][y] = fgetc(mapFile);
		}
		if(fgetc(mapFile) != '|'){
			printf("asdf2\n");
			exit(0);
		}
		if(fgetc(mapFile) != '\n'){
			printf("asdf1\n");
			exit(0);
		}
	}
	if(fgetc(mapFile) != '$'){
		printf("asdf3\n");
		exit(0);
	}
	fclose(mapFile);
	return text;
}

Map readMap(const char *mapName, const uint scale)
{
	char mapPath[32] = {0};
	sprintf(mapPath, "%s%s", "./Maps/", mapName);
	if(access(mapPath, R_OK) != 0){
		printf("Map file not found at %s\n", mapPath);
		exit(0);
	}
	printf("Map file found at %s\n", mapPath);
	File *mapFile = fopen(mapPath, "r");
	Map map = {
		.len = getMapLen(mapFile),
		.scale = scale,
		.text = parseMap(mapFile, map.len)
	};
	map.spawn = getSpawnCoord(map);
	map.spawnDir = getSpawnDir(map);
	map.fruitpos = getFruitSpawn(map);
	return map;
}
