#include <windows.h>
#include <stdio.h>
#include "../inc/Robot.h"

extern rbNode bCastInfo[NROBOT];
extern dataPack dataInfo[NROBOT];

u8 GetRobotID(u8 id){
    return (bCastInfo[id - 1].nodeID);
}

robotPos GetCoordinate(u8 id){
    return (bCastInfo[id - 1].rpos);
}

float GetNorthAngle(u8 id){
    return (bCastInfo[id - 1].angle2n);
}

u8 GetActiveInfo(u8 id){
    return (bCastInfo[id - 1].isActive);
}

u8 GetReadyInfo(u8 id){
    return (bCastInfo[id - 1].isReady);
}

u8 GetPathFoundInfo(u8 id){
    return (bCastInfo[id - 1].isFoundPath);
}

u8 GetRobotType(u8 id){
    return (bCastInfo[id - 1].type);
}

u8 GetStopInfo(u8 id){
    return (bCastInfo[id - 1].isStop);
}

void DispNbrList(u8 id){
    printf("[");
    for(int i = 0; i < NROBOT; i++){
        printf("%X ",bCastInfo[id - 1].nbrList[i]);
    }
    printf("]");
}

void dispInfo(int id){ 
	for (int i = 0; i < NROBOT; i++){
		printf("%X ", bCastInfo[id - 1].isLeaderOK);
	}
	return;
}

void DispPackInfo(int id,FILE* fp){
//    if(id <= 0){
//        printf("id error!\n");
//        return;
//    }
    int index = id - 1;
    char str[256] = {'\0'};
#if 1
/*##########################Received Data###############################*/
   sprintf_s(str,"%X: {(%f,%f),(%f),(%X),",id,bCastInfo[index].rpos.x,bCastInfo[index].rpos.y,bCastInfo[index].angle2n,bCastInfo[index].isReady);
   //sprintf(str,"%X: {(%f,%f),(%f),(%X),",id,bCastInfo[index].rpos.x,bCastInfo[index].rpos.y,bCastInfo[index].angle2n,bCastInfo[index].type);
   fprintf_s(fp, "%s", str);
   printf("%s",str);
   DispNbrList(id);
   //dispInfo(id); //wj test
   printf("}\n");
   fprintf_s(fp, "}\n");
/*##########################Collect Data###############################*/
#endif // 1
//   Sleep(200);
#if 0
/*##########################Collect Data###############################*/
    //sprintf(str,"%d: {%d,(%f,%f),(%f),(%ld,%ld)}\n",dataInfo[index].nodeID,dataInfo[index].timeTick,dataInfo[index].r2B1,dataInfo[index].r2B2,
    sprintf(str,"%d:%d,%f,%f,%f,%ld,%ld\n",dataInfo[index].nodeID,dataInfo[index].timeTick,dataInfo[index].r2B1,dataInfo[index].r2B2,
    dataInfo[index].angle2n,dataInfo[index].xValue,dataInfo[index].yValue);
    printf("%s",str);
/*##########################Collect Data###############################*/
#endif // 0
}

void Log(char *str){
   printf("%s",str);  
   FILE *fp = fopen("log.txt", "w");
   if(fp == NULL){
    printf("Open failed!\n");
    return;
   }
   fclose(fp);
}

