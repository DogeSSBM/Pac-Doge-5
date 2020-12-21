#pragma once

typedef struct{
	Length len;
	uint scale;
	char **text;
}Map;

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

Map readMap(const char *mapName)
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
		.scale = 32,
		.text = parseMap(mapFile, map.len)
	};
	return map;
}
