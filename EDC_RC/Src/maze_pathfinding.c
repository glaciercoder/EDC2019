//#include <stdio.h>
#include "maze_pathfinding.h"

#define STABLE 0
#define INF_MAX 1000
#define UNVISITED 0
#define VISITED 1
#define BALLMODE 1
#define PERSONMODE 0

#define ARRIVED 0
#define TURNLEFT 14
#define TURNRIGHT 15
#define TURNBACK 16
#define MOVEFORE 11

#define DIRECTLEFT 17
#define DIRECTRIGHT 18
#define DIRECTBACK 19


enum DIRECTION{//maze��ľ��Է���
	UP = 1, LEFT, DOWN, RIGHT
};

struct mazenode{
	char wall;	//0-3λ�ֱ����ϣ����£��Ҳ������ϰ�����λ=0

	//·���滮��ʹ�õı���
	struct mazenode* parent;
	int priority;
	char visited;
	char wallcomplete;
};

//�����Թ���ά���飨���߽����һ�㣩
struct mazenode maze[8][8];

//���ǽ�ڣ�����λ�ã���0/��1/��2/��3��������λ�ò���Ҫ�ظ����
void AddWall (char x, char y, char side){
	switch(side){
	case UP:
		maze[y][x].wall |= 1;
		maze[y + 1][x].wall |= 4;
		break;
	case LEFT:
		maze[y][x].wall |= 2;
		maze[y][x - 1].wall |= 8;
		break;
	case DOWN:
		maze[y][x].wall |= 4;
		maze[y - 1][x].wall |= 1;
		break;
	case RIGHT:
		maze[y][x].wall |= 8;
		maze[y][x + 1].wall |= 2;
		break;
	default: break;
	}
}


//��ǽ����Ϣȫ�����
void ClearWalls (void){
	for (int i = 0; i < 8; i++){
		for (int j = 0; j < 8; j++){
			maze[i][j].wall = 0;
			maze[i][j].wallcomplete = 0;
			//����߽�Χǽ��Ϣ
			if (j == 1) maze[i][j].wall |= 2;
			if (j == 6) maze[i][j].wall |= 8;
			if (i == 1) maze[i][j].wall |= 4;
			if (i == 6) maze[i][j].wall |= 1;
		}
	}
	//��ӳ��طֽ絲����Ϣ
	for (int i = 1; i < 6; i++){
		AddWall(i, 6 - i, UP);
		AddWall(i, 6 - i, RIGHT);
	}
}

void ReturnFirstStep(char currentDir, char posx, char posy, char aimx, char aimy, unsigned char* dec);
void MakeDecision(char currentDir, char posx, char posy, char aimx, char aimy, unsigned char* dec, char* straightforward);

//��������ǰ����xy���꣬Ŀ��λ��xy���꣨��Χ1-6,Ҫ��ԭ��������ϼ�1������ǰλ���ϰ������0-2λ�ֱ��Ǻ��ң�ǰ����������ϰ���
unsigned char MakePath (char currentDir, char posx, char posy, char aimx, char aimy, char wallrefresh, int* pri_total, char mode,char* straightforward){
	unsigned char decision = 255;

	//����յ��غϷ���ARRIVED = 0
	if (posx == aimx && posy == aimy) return ARRIVED;

	//����ǽ����Ϣ
	switch(currentDir){
	case RIGHT:
		if (wallrefresh & 8) AddWall(posx,posy,LEFT);
		if (wallrefresh & 4) AddWall(posx,posy,UP);
		if (wallrefresh & 2) AddWall(posx,posy,RIGHT);
		if (wallrefresh & 1) AddWall(posx,posy,DOWN);
		break;
	case UP:
		if (wallrefresh & 8) AddWall(posx,posy,DOWN);
		if (wallrefresh & 4) AddWall(posx,posy,LEFT);
		if (wallrefresh & 2) AddWall(posx,posy,UP);
		if (wallrefresh & 1) AddWall(posx,posy,RIGHT);
		break;
	case DOWN:
		if (wallrefresh & 8) AddWall(posx,posy,UP);
		if (wallrefresh & 4) AddWall(posx,posy,RIGHT);
		if (wallrefresh & 2) AddWall(posx,posy,DOWN);
		if (wallrefresh & 1) AddWall(posx,posy,LEFT);
		break;
	case LEFT:
		if (wallrefresh & 8) AddWall(posx,posy,RIGHT);
		if (wallrefresh & 4) AddWall(posx,posy,DOWN);
		if (wallrefresh & 2) AddWall(posx,posy,LEFT);
		if (wallrefresh & 1) AddWall(posx,posy,UP);
		break;
	default: break;
	}
	maze[posy][posx].wallcomplete = 1;

	//��չ滮·�ߵ��м���Ϣ
	for (int i = 0; i < 8; i++){
		for (int j = 0; j < 8; j++){
			maze[i][j].parent = 0;
			maze[i][j].priority = INF_MAX;	//��ʼ���ȼ��ø�
			maze[i][j].visited = UNVISITED;
		}
	}

	//Dijkstra�滮·��
	maze[posy][posx].priority = 0;
	struct mazenode* selectednode = 0;
	struct mazenode* old_snode = 0;
	while (1){
		//ѡ��δ���ʽڵ��е�������ȼ�����
		int shortest = INF_MAX;
		for (int i = 1; i < 7; i++){
			for (int j = 1; j < 7; j++){
				if (maze[i][j].visited == UNVISITED && maze[i][j].priority < shortest){
					shortest = maze[i][j].priority;
					selectednode = &maze[i][j];
				}
			}
		}
		if (old_snode == selectednode) return 255;	//��ѭ��return
		selectednode -> visited = VISITED;
		//��Ŀ��㱻ѡ��ʱѭ����ֹ
		if (maze[aimy][aimx].visited == VISITED) break;
		//�Դ˽ڵ���ھ����ȼ�������
		struct mazenode* rightnode = (selectednode + 1);
		struct mazenode* leftnode  = (selectednode - 1);
		struct mazenode* downnode  = (selectednode - 8);
		struct mazenode* upnode    = (selectednode + 8);
		//dirjudge
		int selectindex = selectednode - &maze[0][0];
		int parentindex = (selectednode -> parent) ? selectednode -> parent - &maze[0][0] : selectindex;
		int dirjudge = selectindex - parentindex;
		int pathweight = INF_MAX;
		//right
		if ((selectednode -> wall & 8) != 0) pathweight = INF_MAX;
		else if (dirjudge == 0 || dirjudge == 1) pathweight = 3;
		else if (dirjudge == -1) pathweight = 5;
		else  pathweight = 5;
		if (selectednode -> priority + pathweight < rightnode -> priority){
			rightnode -> priority = selectednode -> priority + pathweight;
			rightnode -> parent = selectednode;
		}
		//left
		if ((selectednode -> wall & 2) != 0) pathweight = INF_MAX;
		else if (dirjudge == 0 || dirjudge == -1) pathweight = 3;
		else if (dirjudge == 1) pathweight = 5;
		else  pathweight = 5;
		if (selectednode -> priority + pathweight < leftnode -> priority){
			leftnode -> priority = selectednode -> priority + pathweight;
			leftnode -> parent = selectednode;
		}
		//down
		if ((selectednode -> wall & 4) != 0) pathweight = INF_MAX;
		else if (dirjudge == 0 || dirjudge == -8) pathweight = 3;
		else if (dirjudge == 8) pathweight = 5;
		else  pathweight = 5;
		if (selectednode -> priority + pathweight < downnode -> priority){
			downnode -> priority = selectednode -> priority + pathweight;
			downnode -> parent = selectednode;
		}
		//up
		if ((selectednode -> wall & 1) != 0) pathweight = INF_MAX;
		else if (dirjudge == 0 || dirjudge == 8) pathweight = 3;
		else if (dirjudge == -8) pathweight = 5;
		else  pathweight = 5;
		if (selectednode -> priority + pathweight < upnode -> priority){
			upnode -> priority = selectednode -> priority + pathweight;
			upnode -> parent = selectednode;
		}
		//startpoint
		if (dirjudge == 0){
			switch(currentDir){
			case UP:
				downnode -> priority += 2;//2;
				rightnode -> priority += 2;
				leftnode -> priority += 2;
				break;
			case DOWN:
				upnode -> priority += 2;//2;
				rightnode -> priority += 2;
				leftnode -> priority += 2;
				break;
			case LEFT:
				rightnode -> priority += 2;//2;
				upnode -> priority += 2;
				downnode -> priority += 2;
				break;
			case RIGHT:
				leftnode -> priority += 2;//2;
				upnode -> priority += 2;
				downnode -> priority += 2;
				break;
			default: break;
			}
		}
		//�ж���ѭ��
		old_snode = selectednode;
	}

	//���߽��
	/*path test print
	int path[6][6] = {0};
	for (mazenode* node = &maze[aimy][aimx]; node != NULL; node = node->parent){
	int x = (node - &maze[0][0]) / 8 - 1;
	int y = (node - &maze[0][0]) % 8 - 1;
	path[x][y] = node->priority;
	}
	printf("path found:\n");
	for (int i = 5; i >= 0; i--){
	for (int j = 0; j < 6; j++){
	printf("%d ",path[i][j]);
	}
	printf("\n");
	}
	printf("################################\n");*/

	//���ص�һ��
	if (mode == BALLMODE){
		ReturnFirstStep(currentDir,posx,posy,aimx,aimy,&decision);
	}
	else if (mode == PERSONMODE){
		MakeDecision(currentDir,posx,posy,aimx,aimy,&decision,straightforward);
	}
	else return 255;

	*pri_total = maze[aimy][aimx].priority;

	//printf("decision: %d\n",(int)decision);
	return decision;
}

/*********************************************************************************/
/*********************************************************************************/

//����ƽ��ʱ���ص�һ��
void ReturnFirstStep(char currentDir, char posx, char posy, char aimx, char aimy, unsigned char* dec){
	unsigned char decision;
	//���ص�һ��
	struct mazenode* node = &maze[aimy][aimx];
	struct mazenode* startnode = &maze[posy][posx];
	while (1){
		if (node ->parent == 0){
			decision = 255;
			return;
		}
		if (node -> parent == startnode){
			char x = (node - &maze[0][0]) % 8;
			char y = (node - &maze[0][0]) / 8;
			if (x - posx == 1){//right
				switch (currentDir){
				case UP:	decision = TURNRIGHT; break;
				case LEFT:	decision = TURNBACK;  break;
				case DOWN:	decision = TURNLEFT;  break;
				case RIGHT:	decision = MOVEFORE;  break;
				default: break;
				}
			}
			else if (x - posx == -1){//left
				switch (currentDir){
				case UP:	decision = TURNLEFT;  break;
				case LEFT:	decision = MOVEFORE;  break;
				case DOWN:	decision = TURNRIGHT; break;
				case RIGHT:	decision = TURNBACK;  break;
				default: break;
				}
			}
			else if (y - posy == 1){//up
				switch (currentDir){
				case UP:	decision = MOVEFORE;  break;
				case LEFT:	decision = TURNRIGHT; break;
				case DOWN:	decision = TURNBACK;  break;
				case RIGHT:	decision = TURNLEFT;  break;
				default: break;
				}
			}
			else if (y - posy == -1){//down
				switch (currentDir){
				case UP:	decision = TURNBACK;  break;
				case LEFT:	decision = TURNLEFT;  break;
				case DOWN:	decision = MOVEFORE;  break;
				case RIGHT:	decision = TURNRIGHT; break;
				default: break;
				}
			}
			break;
		}
		node = node->parent;
	}
	*dec = decision;
}

//����ƽ��ת��
void MakeDecision (char currentDir, char posx, char posy, char aimx, char aimy, unsigned char* dec, char* straightforward){
	unsigned char decision;
	//��ԭ·��
	char pathx[21] = {0};
	char pathy[21] = {0};
	int total_point = 0;	//��㵽�յ��·���а����Ľڵ����
	for (struct mazenode* node = &maze[aimy][aimx]; node != 0; node = node->parent){
		pathy[total_point] = (node - &maze[0][0]) / 8;
		pathx[total_point] = (node - &maze[0][0]) % 8;
		total_point++;
	}

	char goLEFT = TURNLEFT;
	char goRIGHT = TURNRIGHT;
	char goBACK = TURNBACK;

	//�ҵ���һ���յ�
	int turnindex = 0;
	for (int i = total_point - 1; i >= 0; i--){
		if (pathx[i - 2] - pathx[i - 1] != pathx[i - 1] - pathx[i] 
		|| pathy[i - 2] - pathy[i - 1] != pathy[i - 1] - pathy[i]){
			turnindex = i - 1;
			break;
		}
	}
	//��·���ϵ���һ����ǽ����Ϣ��֪�Ҳ��ǹյ�
	if (turnindex != total_point - 2 
		&& maze[pathy[total_point - 2]][pathx[total_point - 2]].wallcomplete == 1){
		*straightforward = 1;
	}
	else{
		*straightforward = 0;
	}

	if (total_point <= 3){//Ŀ�ĵ������֮�����<=2ʱֱ��ƽ��
		goBACK = DIRECTBACK;
		goLEFT = DIRECTLEFT;
		goRIGHT = DIRECTRIGHT;
	}
	else{
		//������ƽ��
		if (total_point - turnindex <= 4){
			goBACK = DIRECTBACK;
			goLEFT = DIRECTLEFT;
			goRIGHT = DIRECTRIGHT;
		}
		/*//������ת������һ��ת������������ͬʱ��ƽ��
		if (total_point - turnindex <= 3){
			//goBACK = DIRECTBACK;
			char nextDir = 0;
			if (pathx[turnindex - 1] - pathx[turnindex] == 1) nextDir = RIGHT;
			else if (pathx[turnindex - 1] - pathx[turnindex] == -1) nextDir = LEFT;
			else if (pathy[turnindex - 1] - pathy[turnindex] == 1) nextDir = UP;
			else if (pathy[turnindex - 1] - pathy[turnindex] == -1) nextDir = DOWN;

			if (nextDir == currentDir){
				goLEFT = DIRECTLEFT;
				goRIGHT = DIRECTRIGHT;
			}
		}*/
	}

#if STABLE == 1
	goLEFT = TURNLEFT;
	goRIGHT = TURNRIGHT;
	goBACK = TURNBACK;
#endif

	if (pathx[total_point - 2] - pathx[total_point - 1] == 1){//right
		switch (currentDir){
		case UP:	decision = goRIGHT;		break;
		case LEFT:	decision = goBACK;		break;
		case DOWN:	decision = goLEFT;		break;
		case RIGHT:	decision = MOVEFORE;	break;
		default: break;
		}
	}
	else if (pathx[total_point - 2] - pathx[total_point - 1] == -1){//left
		switch (currentDir){
		case UP:	decision = goLEFT;		break;
		case LEFT:	decision = MOVEFORE;	break;
		case DOWN:	decision = goRIGHT; 	break;
		case RIGHT:	decision = goBACK;  	break;
		default: break;
		}
	}
	else if (pathy[total_point - 2] - pathy[total_point - 1] == 1){//up
		switch (currentDir){
		case UP:	decision = MOVEFORE;	break;
		case LEFT:	decision = goRIGHT;		break;
		case DOWN:	decision = goBACK;		break;
		case RIGHT:	decision = goLEFT;  	break;
		default: break;
		}
	}
	else if (pathy[total_point - 2] - pathy[total_point - 1] == -1){//down
		switch (currentDir){
		case UP:	decision = goBACK;  	break;
		case LEFT:	decision = goLEFT;  	break;
		case DOWN:	decision = MOVEFORE;	break;
		case RIGHT:	decision = goRIGHT; 	break;
		default: break;
		}
	}
	*dec = decision;
}

/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/

unsigned char PathFinding(char currentDir, char posx, char posy, char aimx, char aimy, char wallrefresh){
	int pri = 0;
	char useless;
	unsigned char decision = MakePath(currentDir,posx,posy,aimx,aimy,wallrefresh,&pri,BALLMODE,&useless);
	return decision;
}

unsigned char PersonFinding (char currentDir, char posx, char posy, char p1bx, char p1by, char p2bx, char p2by, char wallrefresh, char* straightforward){
	unsigned char decision1 = 255, decision2 = 255;
	char straight1 = 0, straight2 = 0;
	int pri_1 = INF_MAX, pri_2 = INF_MAX;
	decision1 = MakePath(currentDir,posx,posy,p1bx,p1by,wallrefresh,&pri_1,PERSONMODE,&straight1);
	decision2 = MakePath(currentDir,posx,posy,p2bx,p2by,wallrefresh,&pri_2,PERSONMODE,&straight2);

	//printf("****straightforward: %d\n",(int)straight1);
	/*printf("pri compare:%d %d ����",pri_1,pri_2);
	if (pri_1 < pri_2) printf("1\n");
	else printf("2\n");*/

	if (pri_1 < pri_2){
		*straightforward = straight1;
		return decision1;
	}
	else{
		*straightforward = straight2;
		return decision2;
	}
}

/*int main(void){
	ClearWalls();

	//AddWall(1,3,UP);
	//PathFinding(RIGHT,1,5,5,1,0);
	char straight = 0;
	PersonFinding(DOWN,1,2,1,3,1,3,0,&straight);
	PersonFinding(DOWN,1,3,1,1,1,1,0,&straight);

	//print wall condition
	printf("wall condition:\n");
	for (int i = 7; i >= 0; i--){
		for (int j = 0; j < 8; j++){
			printf("%d\t",(int)maze[i][j].wall);
		}
		printf("\n");
	}
	return 0;
}*/
