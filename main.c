//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"


//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;

static int player_nr;

typedef struct player
{
	int energy;
	int position;
	char name[MAX_CHARNAME];
	int accumCredit;
	int flag_graduate;
	char takenLecture[MAX_TAKENLECTURE][MAX_CHARNAME];
	int takenLectureNum;
}player_t;

static player_t * cur_player;

#if 0
static int player_energy[MAX_PLAYER];
static int player_position[MAX_PLAYER];
static int player_name[MAX_PLAYER][MAX_CHARNAME];
#endif

//function prototypes
#if 0
smmObjGrade_e takeLecture(int player, char *lectureName, int credit);
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); //print all the grade history of the player
#endif


void printGrades(int player)
{
	int i;
	void * gradePtr;
	
	for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
	{
		gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
		printf("%s : %i\n", smmObj_getNodeName(gradePtr), smmObj_getNodeGrade(gradePtr));
	}
}


void printPlayerStatus(void)//print all player status at the beginning of each turn
{
	int i;
	for (i=0;i<player_nr;i++)
	{
		printf("%s : credit %i, energy %i, position %i\n", 
		cur_player[i].name, cur_player[i].accumCredit, cur_player[i].energy, cur_player[i].position);
	}
}

void generatePlayers(int n, int initEnergy) //generate a new player
{
	int i;
	cur_player = (player_t*)malloc(n*sizeof(player_t));
	//n time loop
	for (i=0;i<n;i++)
	{
		//input name
		printf("Input Player %i's name:", i); //
		scanf("%s", cur_player[i].name);
		fflush(stdin);
		
		//set position
		//player_position[i] = 0;
		cur_player[i].position = 0;
		
		//set energy
		//player_energy[i] = initEnergy;
		cur_player[i].energy = initEnergy;
		cur_player[i].accumCredit = 0;
		cur_player[i].flag_graduate = 0;
	}
}


int rolldie(int player)
{
    char c;
    printf("Press any key to roll a die (press g to see grade): ");
    c = getchar();
    
#if 1
    if (c == 'g')
        printGrades(player);
#endif
    return (rand()%MAX_DIE + 1);
}


int experiment_ing(int player)
{	
	int dicetoEscape;
	dicetoEscape = rollDice();
	
	printf("탈출하기 위해 플레이어는 주사위를 던져서 5 이상의 숫자를 얻어야 합니다. ");
	printf("주사위 결과: %d\n", dicetoEscape);
	
	if (dicetoEscape < 5)
		printf("실험실을 탈출 했습니다!\n");
	if (dicetoEscape >= 5)
		printf("실험실을 탈출하지 못합니다.\n"); 
}


//float calcAverageGrade(int player) //calculate average grade of the player
//{
//	float gradeSum = 0.0;
	
//	for (int i = 0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++) //수강한 강의가 있는 경우 
//	{
//		void *gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
//		gradeSum += smmObj_getNodeGrade(gradePtr);
//	}
	
//	return gradeSum/smmdb_len(LISTNO_OFFSET_GRADE + player);
//}

//action code when a player stays at a node
void actionNode(int player)
{
	void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
	int type = smmObj_getNodeType(boardPtr);
	char *name = smmObj_getNodeName(boardPtr);
	void *gradePtr;
	
    switch(type) //node 칸마다 수행  동작 정해주기
    {
        case SMMNODE_TYPE_LECTURE://소요에너지,점수획득
			if (cur_player[player].energy >= smmObj_getNodeEnergy(boardPtr))
			{
				int yourChoice = 1; 
			
				printf("수강하려면 1번, 드랍하려면 2번을 눌러 주세요.\n");
				scanf("%d\n", &yourChoice);
		
				if (yourChoice == 1)
				{
					printf("강의를 드랍합니다.\n");
				}	
				if (yourChoice == 2)
				{
					printf("강의를 수강합니다. (남은 에너지: %d)\n", cur_player[player].energy);
					cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
					cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
				
					int resultGrade = rand()%MAX_GRADE; //성적생성 및 저장 
					smmObjGrade_e randomResultGrade = (smmObjGrade_e)resultGrade;
					void *gradePtr = smmObj_genObject(lectureName, smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, resultGrade); 
					smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
			
					strcpy(cur_player[player].takenLecture[cur_player[player].takenLectureNum], lectureName); //저장 
					cur_player[player].takenLectureNum++; 
				}
			}
			else
				printf("%s의 에너지가 부족합니다. 수강할 수 없습니다. (에너지: %d)\n", cur_player[player].name, cur_player[player].energy);
	
			return 0;	
		}	

            
            
        case SMMNODE_TYPE_RESTAURANT://보충에너지(not 카드) 
        	cur_player[player].energy += 10;
        	break;
        	
        	
        	
        case SMMNODE_TYPE_LABORATORY://just 실험실(수행동작 없음)   
			//if (실험 중 상태)
				//if 주사위 던져서 5밑
					//그대로, 횟수는1번 추가  
			else //실험중이 아닌 경우(gotolab거치지 않은 경우)
				break;	
        	
        	
        case SMMNODE_TYPE_HOME://머무름,지나감 모두 에너지보충
        	cur_player[player].energy += smmObj_getNodeEnergy(noardPtr);
        	break;
        	
        	
        	
        case SMMNODE_TYPE_GOTOLAB://상태전환, 실험실이동(칸 이동) 
        	//실험중 상태 함수 불러오기 
			cur_player[player].position = 8; //플레이어 포지션 == 실험실노드 (칸이동)
			 
        	
        	
        case SMMNODE_TYPE FOODCHANCE://에너지보충,음식카드(랜덤)
        {
        	foodCard* = smmdb_getData(LISTNO_FOODCARD, rand()%smmdb_len(LISTNO_FOODCARD));
			cur_player[player].energy += smmObj_getNodeEnergy(foodCard);
    		char* foodCardName = smmObj_getNodeName(foodCard);
    		printf("%s를 먹어서 에너지가 %d만큼 충전되었습니다.\n", foodCardName,smmmObj_getNodeName(foodCard));
			break;	
		}
        	
        	
        case SMMNODE_TYPE_FESTIVAL://축제카드(랜덤), 미션수행 
        {
			printf("축제가 열렸습니다. 카드를 뽑아 미션을 수행하세요!"\n)
			printf("(press any key for your random mission card!)\n")
			
		//랜덤 fest 미션 카드 출력 
		FestivalCard = smmdb_getData(LISTNO_FESTCARD, rand()%smmdb_len(LISTNO_FESTCARD));
		printf("MISSION : %s \n", smmObj_getNodeName(FestivalCard));
		break;
		}
    }
}

void goFoward(int player, int step)//make player go "step" steps on the board (check if player is graduated)
{
	void *boardPtr;
	cur_player[player].position += step;
	boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
	
	printf("%s go to node %i (type: %s)\n",
				cur_player[player].name, cur_player[player].position
				smmObj_getNodeName(boardPtr));
}


int isGraduated(void)//졸업했는지(최종성적 고려?) 
{
	int i;
	void *gradePtr
	
	print("플레이어 '%s'의 상황: \n", cur_player[player].name);
	
	for(i=0;i<player_nr;i++)
	{
		if (cur_player[i].accumCredit >= GRADUATE_CREDIT && cur_player[player].position == 0)
			cur_player[i].flag_graduate = 1;
		else
			cur_player[i].flag_graduate = 0;
	}
}

int end(void) //끝마침
{
	if cur_player[i].flag_graduate = 1)
		printf("\n========================================================================\n");
		printf("\n========================================================================\n");
		printf("\n========================================================================\n");
		printf("\n========================================================================\n");
		printf("\n=============================CONGRATUATION!=============================\n");
		printf("\n=========================PLAYER %s IS GRADUATED!========================\n");
		printf("\n============================%s's Credit: %d=============================\n", 
		cur_player[i].name, smmObj_getNodeCredit(gradePtr));
		printf("\n========================================================================\n");
		printf("\n========================================================================\n");
		printf("\n========================================================================\n");
		printf("\n========================================================================\n");
 } 






int main(int argc, const char * argv[]) 
{
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int i;
    int initEnergy;
    int turn = 0;
    
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while (fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4) //read a node parameter set
    {
        //store the parameter set
        //(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade)
        void *boardObj = smmObj_genObject(name, smmObjType_board, type, credit, energy, 0);
        smmdb_addTail(LISTNO_NODE, boardObj);
        if (type == SMMNODE_TYPE_HOME)
        	initEnergy = energy;
        board_nr++;
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    for (i=0; i<board_nr; i++)
    {
    	void*boardObj = smmdb_getData(LISTNO_NODE, i);
    	printf("node %i : %s, %i(%s), credit %i, energy %i\n"
		, i, smmObj_getNodeName(boardObj), smmObj_getNodeType(boardObj), smmObj_getTypeName(smmObj_getNodeType(boardObj))
		, smmObj_getNodeCredit(boardObj), smmObj_getNodeEnergy(boardObj));
	}
    
    //printf("(%s)", smmObj_getTypeName(SMMNODE_TYPE_LECTURE));
    
    
    
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while (fscanf(fp, "%s %i", name, &energy) == 2) //read a food parameter set
    {
    	//store the parameter set
     	void* foodCard = smmObj_genObject(name, smmObjType_foodCard, SMMNODE_TYPE_FOODCHANCE, 0, energy, 0); 
    	smmdb_addTail(LISTNO_FOODCARD, foodCard);
    	food_nr++;
	}
    fclose(fp);
    printf("Total number of food cards : %i\n", food_nr);
    
    for (i=0;i<food_nr;i++)
    {
    	void *foodCardObj = smmdb_getData(LISTNO_FOODCARD, i);
    	printf("node %i: %s, energy: %i\n", i, smmObj_getNodeName(foodCard), smmobj_getNodeEnergy(foodCard));
	}
    
    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while (fscanf(fp, "%s", name) == 1) //read a festival card string
    {
    	void* festivalCard = smmObj_genObject(name, smmObjType_card, type, credit, energy, 0);//store the parameter set
    	
    	smmdb_addTail(LISTNO_FESTCARD, festivalCard);
    	festival_nr++;
	}
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);
    
    for (i=0;i<festival_nr;i++)
    {
    	void *festivalCard = smmdb_getData(LISTNO_FESTCARD, i);
    	printf("node %i: %s\n", i, smmObj_getNodeName(festivalCard));
	}
    
     printf("========================================================================\n")
    printf("------------------------------------------------------------------------\n")
    printf("========================================================================\n")
    printf("------------------------------------------------------------------------\n")
    printf("========================================================================\n")
    printf("------------------------------------------------------------------------\n")
    printf("==========SOOKMYUNG MARBLE - Let's Graduate!(total credit: &d)==========\n", GRADUATE_CREDIT)
    printf("------------------------------------------------------------------------\n")
    printf("========================================================================\n")
    printf("------------------------------------------------------------------------\n")
    printf("========================================================================\n")
    printf("------------------------------------------------------------------------\n")
    printf("========================================================================\n")
    
    //2. Player configuration ---------------------------------------------------------------------------------
    
    do
    {
        //input player number to player_nr
        printf("input player no.: ");
        scanf("%d", &player_nr);
        fflush(stdin);
    }
    while (player_nr < 0 || player_nr > MAX_PLAYER);

	cur_player = (player_t*)malloc(player_nr*sizeof(player_t));
	generatePlayers(player_nr, initEnergy);




   
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (isGraduated) //is anybody graduated?
    {
        int die_result;
        
        //4-1. initial printing
        printPlayerStatus();
        printf("플레이어 '%s'의 차례입니다::: (진행을 위해 아무 키나 눌러주세요)\n", cur_player[player].name);
        
        //4-2. die rolling (if not in experiment)
		die_result = rolldie(turn);
        
        
        //4-3. go forward
        goForward(turn, die_result);

		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        //4-5. next turn
        turn = (turn + 1) % player_nr;
        
    }
    
    free(cur_player);
    
    return 0;
}

