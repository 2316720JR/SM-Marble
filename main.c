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
	
	printf("Ż���ϱ� ���� �÷��̾�� �ֻ����� ������ 5 �̻��� ���ڸ� ���� �մϴ�. ");
	printf("�ֻ��� ���: %d\n", dicetoEscape);
	
	if (dicetoEscape < 5)
		printf("������� Ż�� �߽��ϴ�!\n");
	if (dicetoEscape >= 5)
		printf("������� Ż������ ���մϴ�.\n"); 
}


//float calcAverageGrade(int player) //calculate average grade of the player
//{
//	float gradeSum = 0.0;
	
//	for (int i = 0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++) //������ ���ǰ� �ִ� ��� 
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
	
    switch(type) //node ĭ���� ����  ���� �����ֱ�
    {
        case SMMNODE_TYPE_LECTURE://�ҿ信����,����ȹ��
			if (cur_player[player].energy >= smmObj_getNodeEnergy(boardPtr))
			{
				int yourChoice = 1; 
			
				printf("�����Ϸ��� 1��, ����Ϸ��� 2���� ���� �ּ���.\n");
				scanf("%d\n", &yourChoice);
		
				if (yourChoice == 1)
				{
					printf("���Ǹ� ����մϴ�.\n");
				}	
				if (yourChoice == 2)
				{
					printf("���Ǹ� �����մϴ�. (���� ������: %d)\n", cur_player[player].energy);
					cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
					cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
				
					int resultGrade = rand()%MAX_GRADE; //�������� �� ���� 
					smmObjGrade_e randomResultGrade = (smmObjGrade_e)resultGrade;
					void *gradePtr = smmObj_genObject(lectureName, smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, resultGrade); 
					smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
			
					strcpy(cur_player[player].takenLecture[cur_player[player].takenLectureNum], lectureName); //���� 
					cur_player[player].takenLectureNum++; 
				}
			}
			else
				printf("%s�� �������� �����մϴ�. ������ �� �����ϴ�. (������: %d)\n", cur_player[player].name, cur_player[player].energy);
	
			return 0;	
		}	

            
            
        case SMMNODE_TYPE_RESTAURANT://���濡����(not ī��) 
        	cur_player[player].energy += 10;
        	break;
        	
        	
        	
        case SMMNODE_TYPE_LABORATORY://just �����(���ൿ�� ����)   
			//if (���� �� ����)
				//if �ֻ��� ������ 5��
					//�״��, Ƚ����1�� �߰�  
			else //�������� �ƴ� ���(gotolab��ġ�� ���� ���)
				break;	
        	
        	
        case SMMNODE_TYPE_HOME://�ӹ���,������ ��� ����������
        	cur_player[player].energy += smmObj_getNodeEnergy(noardPtr);
        	break;
        	
        	
        	
        case SMMNODE_TYPE_GOTOLAB://������ȯ, ������̵�(ĭ �̵�) 
        	//������ ���� �Լ� �ҷ����� 
			cur_player[player].position = 8; //�÷��̾� ������ == ����ǳ�� (ĭ�̵�)
			 
        	
        	
        case SMMNODE_TYPE FOODCHANCE://����������,����ī��(����)
        {
        	foodCard* = smmdb_getData(LISTNO_FOODCARD, rand()%smmdb_len(LISTNO_FOODCARD));
			cur_player[player].energy += smmObj_getNodeEnergy(foodCard);
    		char* foodCardName = smmObj_getNodeName(foodCard);
    		printf("%s�� �Ծ �������� %d��ŭ �����Ǿ����ϴ�.\n", foodCardName,smmmObj_getNodeName(foodCard));
			break;	
		}
        	
        	
        case SMMNODE_TYPE_FESTIVAL://����ī��(����), �̼Ǽ��� 
        {
			printf("������ ���Ƚ��ϴ�. ī�带 �̾� �̼��� �����ϼ���!"\n)
			printf("(press any key for your random mission card!)\n")
			
		//���� fest �̼� ī�� ��� 
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


int isGraduated(void)//�����ߴ���(�������� ���?) 
{
	int i;
	void *gradePtr
	
	print("�÷��̾� '%s'�� ��Ȳ: \n", cur_player[player].name);
	
	for(i=0;i<player_nr;i++)
	{
		if (cur_player[i].accumCredit >= GRADUATE_CREDIT && cur_player[player].position == 0)
			cur_player[i].flag_graduate = 1;
		else
			cur_player[i].flag_graduate = 0;
	}
}

int end(void) //����ħ
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
        printf("�÷��̾� '%s'�� �����Դϴ�::: (������ ���� �ƹ� Ű�� �����ּ���)\n", cur_player[player].name);
        
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

