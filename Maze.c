#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DIREC_SIZE 4

typedef struct {
	int x;
	int y;
}block;

typedef struct {
	int** build;
	int height;
	int width;
	block start;
	block goal;
}maze;

typedef struct {
	block* path;
	int size;
	int top;//queue or stack
}pathRegister;

typedef struct {
	bool** build;
	int height;
	int width;
}pathRecDFS;

typedef struct {
	block** build;
	int height;
	int width;
}pathRecBFS;

static maze map;
static pathRegister pathReg;
static pathRecDFS visited;//dfs
static pathRecBFS prevMap;//bfs
static int routes = 0, distance = 0;

const block dir[DIREC_SIZE] = {
	{.x = 1,.y = 0},//0-up
	{.x = -1,.y = 0},//1-down
	{.x = 0,.y = -1},//2-left
	{.x = 0,.y = 1},//3-right
};

block walk(block now, block direc) {
	return(block) {
		.x = now.x + direc.x, .y = now.y + direc.y
	};
}

bool boundercheck(block now) {
	return now.x < map.height && now.x >= 0 && now.y < map.width && now.y >= 0;
}

void initializeMaze(char* fileName) {

	printf("read test case file \"%s\"...\n", fileName);
	FILE* pFile;
	fopen_s(&pFile, fileName, "r");
	char* input[1000];
	if (pFile == NULL) {
		perror("file error\n");
		return;
	}
		
	int height, width;
	if (fgets(input, 1000, pFile)) {
		sscanf_s(input, "%d %d", &height, &width);
	}
	else {
		perror("File format error\n");
		return;
	}
	
	map.height = height;
	map.width = width;
	map.start.x = 0;
	map.start.y = 0;
	map.goal.x = map.height - 1;
	map.goal.y = map.width - 1;
	visited.height = height;
	visited.width = width;
	prevMap.height = height;
	prevMap.width = width;
	pathReg.size = 0;
	pathReg.top = 0;
	pathReg.path = calloc(height * width, sizeof(block));

	map.build = calloc(map.height, sizeof(int*));
	visited.build = calloc(visited.height, sizeof(bool*));
	prevMap.build = calloc(prevMap.height, sizeof(block*));
	for (int i = 0; i < map.height; i++) {
		map.build[i] = calloc(map.width, sizeof(int));
		visited.build[i] = calloc(visited.width, sizeof(bool));
		prevMap.build[i] = calloc(prevMap.width, sizeof(block));
		for (int j = 0; j < map.width; j++) {
			char c_in = fgetc(pFile);
			if (c_in != '0' && c_in != '1') {
				perror("File format error\n");
				return;
			}
			map.build[i][j] = c_in - '0';
			if (fgetc(pFile)==EOF) {
				perror("File format error\n");
				return;
			}
			prevMap.build[i][j] = (block){
				.x = -1,
				.y = -1
			};
		}
	}
	printf("\n");
	fclose(pFile);
}

void initializeMaze_console() {
	
	printf("insert height and width of the maze:\n");
	int height, width;
	scanf_s("%d %d\n", &height, &width);
	map.height = height;
	map.width = width;
	map.start.x = 0;
	map.start.y = 0;
	map.goal.x = map.height - 1;
	map.goal.y = map.width - 1;
	visited.height = height;
	visited.width = width;
	prevMap.height = height;
	prevMap.width = width;
	pathReg.size = 0;
	pathReg.top = 0;
	pathReg.path = calloc(height * width, sizeof(block));

	printf("insert size %dx%d of the maze:\n", map.height, map.width);
	map.build = calloc(map.height, sizeof(int*));
	visited.build = calloc(visited.height, sizeof(bool*));
	prevMap.build = calloc(prevMap.height, sizeof(block*));
	for (int i = 0; i < map.height; i++) {
		map.build[i] = calloc(map.width, sizeof(int));
		visited.build[i] = calloc(visited.width, sizeof(bool));
		prevMap.build[i] = calloc(prevMap.width, sizeof(block));
		for (int j = 0; j < map.width; j++) {
			map.build[i][j] = getchar() - '0';
			getchar();
			prevMap.build[i][j] = (block){
			.x = -1,
			.y = -1
			};
		}
	}
	printf("\n");
}

void printMaze() {
	for (int i = 0; i < map.height; i++) {
		for (int j = 0; j < map.width; j++) {
			printf("%c ", visited.build[i][j] ? '@' : map.build[i][j] + '0');
		}
		printf("\n");
	}
	printf("\n");
}

void printPath() {
	printf("\n");
	printf("path %d:", routes);
	printf(" (%d, %d)", pathReg.path[0].x, pathReg.path[0].y);
	for (int i = 1; i < pathReg.size; i++) {
		printf(" -> (%d, %d)", pathReg.path[i].x, pathReg.path[i].y);
	}
	printf("\n");
}

void trackbackBFS(block now) {
	visited.build[now.x][now.y] = true;
	if (prevMap.build[now.x][now.y].x == now.x && prevMap.build[now.x][now.y].y == now.y) {
		return;
	}
	trackbackBFS(prevMap.build[now.x][now.y]);
	printf("(%d, %d) -> ", prevMap.build[now.x][now.y].x, prevMap.build[now.x][now.y].y);
	distance++;
}

void printShortestPath() {
	printf("\nshortest route: ");
	trackbackBFS(map.goal);
	printf("(%d, %d)\n", map.goal.x, map.goal.y);
}

void printPrevMap() {
	for (int i = 0; i < map.height; i++) {
		for (int j = 0; j < map.width; j++) {
			printf("(%d, %d)", prevMap.build[i][j].x, prevMap.build[i][j].y);
		}
		printf("\n");
	}
	printf("\n");
}

void findAllRoutesDFS(block now) {
	visited.build[now.x][now.y] = true;
	pathReg.path[pathReg.top++] = now;
	pathReg.size++;
	if (now.x == map.goal.x && now.y == map.goal.y) {
		routes++;
		printPath();
		printMaze();
		return;
	}
	block next;
	for (int i = 0; i < DIREC_SIZE; i++) {
		next = walk(now, dir[i]);//4 direction
		if (!boundercheck(next) || visited.build[next.x][next.y] || map.build[next.x][next.y] != 0) {
			continue;
		}	
		findAllRoutesDFS(next);
		visited.build[next.x][next.y] = false;
		pathReg.path[--pathReg.top] = (block){
			.x = 0,
			.y = 0
		};
		--pathReg.size;
	}
}

void findShortestRouteBFS() {
	pathReg.path[pathReg.size++] = (block){ 
		.x = map.start.x,
		.y = map.start.y
	};
	prevMap.build[map.start.x][map.start.y] = (block){
		.x = map.start.x,
		.y = map.start.y
	};
	routes = 1;
	while (pathReg.path[pathReg.top].x != map.goal.x || pathReg.path[pathReg.top].y != map.goal.y) {
		if (routes == 0) {
			return;
		}
		block prev = pathReg.path[pathReg.top++], next;
		for (int i = 0; i < DIREC_SIZE; i++) {
			next = walk(prev, dir[i]);
			if (!boundercheck(next)) {
				continue;
			}
			if (map.build[next.x][next.y] != 0 || prevMap.build[next.x][next.y].x != -1) {
				continue;
			}
			routes++;
			pathReg.path[pathReg.size++] = next;
			prevMap.build[next.x][next.y] = prev;
		}
		routes--;
	}
	printShortestPath();
	printMaze();
}

int main() {
	initializeMaze("testcase1.txt");
	printMaze();

	//findAllRoutesDFS(map.start);
	//if (routes > 0) {
	//	printf("found total %d route.\n", routes);
	//}

	findShortestRouteBFS();
	if (routes > 0) {
		printf("the shortest distance of the route: %d\n.", distance);
	}
	
	if (routes <= 0) {
		printf("the goal is unreachable.\n");
	}
}