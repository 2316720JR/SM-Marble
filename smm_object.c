//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODETYPE    7
#define MAX_GRADE       9
#define MAX_NODE		100


static char smmNodeName[SMMNODE_TYPE_MAX][MAX_CHARNAME] = 
{
	"강의",
	"식당",
	"실험실",
	"집",
	"실험실로이동",
	"음식찬스",
	"축제!"
};



//1.구조체 형식 정의 
type struct smmObject
{
	char name[MAX_CHARNAME];
	smmObjType_e objType;
	smmObjGrade_e grade;
	int type;
	int credit;
	int energy;
} smmObject_t;

//2.구조체 배열 변수 정의
//struct smmObject_t smm_node[MAX_NODE];
//static int smmObj_noNode = 0;


//3.관련 함수 변경 
//object generation
smmObject_t* smmObj_genObject(char* name, int type, int credit, int energy, smmObjType_e objtype, smmObjGrade_e grade;)
{
	smmObject_t* ptr;
	
	ptr = (smmObject_t*)malloc(sizeof(smmObject_t));
	
	strcpy(ptr->name, name);
	ptr->objType = objType;
	ptr->credit = credit;
	ptr->energy = energy;
	ptr->grade = grade;
	ptr->type = type;
	
	return ptr;
}


char* smmObj_getNodeName(void* obj)
{
	smmObject_t* ptr = (smmObject_t*)obj;
	return ptr->name;
}

char* smmObj_getGradeName(smmObjGrade_e grade)
{
	return smmGreadeName[grade];
}

int smmObj_getNodeType(int node_nr)
{
	return smm_node[node_nr].type;
}


int smmObj_getNodeCredit(int node_nr)
{
    return smm_node[node_nr].credit;
}

int smmObj_getNodeEnergy(int node_nr)
{
    return smm_node[node_nr].energy;
}

