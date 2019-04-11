#include <stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include"cJSON.h"
//#include"cJSON.c"
int main()
{
    //创建对象
    cJSON*obj=cJSON_CreateObject();
    //创建子对象
    cJSON*subObj=cJSON_CreateObject();
    //天街key---value
    cJSON_AddItemToObject(subObj,"factory",cJSON_CreateString("一汽大众"));
    cJSON_AddItemToObject(subObj,"last",cJSON_CreateNumber(31));
    cJSON_AddItemToObject(subObj,"price",cJSON_CreateNumber(83));
    cJSON_AddItemToObject(subObj,"sell",cJSON_CreateNumber(49));
    cJSON_AddItemToObject(subObj,"sum",cJSON_CreateNumber(80));
    //创建json数组
    cJSON *array=cJSON_CreateArray();
    //往array中添加元素
    cJSON_AddItemToArray(array,cJSON_CreateNumber(123));
    cJSON_AddItemToArray(array,cJSON_CreateString("hello world"));
    cJSON_AddItemToArray(array,cJSON_CreateBool(1));
    // 数组中的对象；
    cJSON*subsub=cJSON_CreateObject();
    cJSON_AddItemToObject(subsub,"梅赛德斯奔驰",cJSON_CreateString("心所向，持易恒"));

    cJSON_AddItemToArray(array,subsub);
    cJSON_AddItemToObject(subObj,"other",array);
    //往obj中天街 key-value
    cJSON_AddItemToObject(obj,"奔驰",subObj);

    //数据格式化 
    //
    char *text=cJSON_Print(obj);
    FILE*fp=fopen("car.json","w");
    fwrite(text,sizeof(char),strlen(text)+1,fp);
    fclose(fp);
    return 0;
}

