#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include <string.h>

/**
{
    "name":	"milo",
    "age":	80,
    "professional": {专业
        "english": 4,
        "putonghua": 2,
        "computer": 4
    },
    "languages":	["C++", "C"],
    "phone":	{
        "number":	"18620823143",
        "type":	"home"
    },
    "courses":	[{
            "name":	"Linux kernel development",
            "price":	"7.7"
        }, {
            "name":	"Linux server development",
            "price":	"8.0"
        }],
    "vip":	true,
    "address":	null
}
*/

//打印CJSON
void printfJson(cJSON *json)
{
    if (NULL == json)
    {
        return;
    }
    char *cjson = cJSON_Print(json);
    printf("json:%s\n", cjson);
    free(cjson);
}

cJSON *createJson()
{
    cJSON *json_root = cJSON_CreateObject();
    cJSON_AddItemToObject(json_root, "name", cJSON_CreateString("milo"));
    cJSON_AddNumberToObject(json_root, "age", 80);

    cJSON *json_professional = cJSON_CreateObject();
    cJSON_AddNumberToObject(json_professional, "english", 4);
    cJSON_AddItemToObject(json_professional, "putonghua", cJSON_CreateNumber(2));
    cJSON_AddItemToObject(json_professional, "computer", cJSON_CreateNumber(3));

    cJSON *json_languages = cJSON_CreateArray();
    cJSON_AddItemToArray(json_languages, cJSON_CreateString("C++"));
    cJSON_AddItemToArray(json_languages, cJSON_CreateString("C"));

    cJSON *json_phone = cJSON_CreateObject();
    cJSON_AddItemToObject(json_phone, "number", cJSON_CreateString("18620823143"));
    cJSON_AddItemToObject(json_phone, "type", cJSON_CreateString("home"));

    cJSON *json_courses = cJSON_CreateArray();
    cJSON *json_pItem1 = cJSON_CreateObject();
    cJSON_AddItemToObject(json_pItem1, "name", cJSON_CreateString("Linux kernel development"));
    cJSON_AddNumberToObject(json_pItem1, "price", 7.7);
    cJSON_AddItemToArray(json_courses, json_pItem1);

    cJSON *json_pItem2 = cJSON_CreateObject();
    cJSON_AddItemToObject(json_pItem2, "name", cJSON_CreateString("Linux server development"));
    cJSON_AddNumberToObject(json_pItem2, "price", 8.8);
    cJSON_AddItemToArray(json_courses, json_pItem2);

    cJSON_AddItemToObject(json_root, "professional", json_professional);
    cJSON_AddItemToObject(json_root, "languages", json_languages);
    cJSON_AddItemToObject(json_root, "phone", json_phone);
    cJSON_AddItemToObject(json_root, "courses", json_courses);

    cJSON_AddBoolToObject(json_root, "vip", 1);
    cJSON_AddNullToObject(json_root, "address");

    return json_root;
}

void analysisJsonObj(cJSON *json)
{
    if (NULL == json)
    {
        return;
    }

    cJSON *json_name = cJSON_GetObjectItem(json, "name");
    printf("root[%s:%s]\n", json_name->string, json_name->valuestring);

    cJSON *json_age = cJSON_GetObjectItem(json, "age");
    printf("root[%s:%d]\n", json_age->string, json_age->valueint);

    cJSON *json_professional = cJSON_GetObjectItem(json, "professional");
    cJSON *json_professional_child = json_professional->child;
    while (json_professional_child != NULL)
    {
        printf("%s[%s:%d]\n", json_professional->string, json_professional_child->string, json_professional_child->valueint);
        json_professional_child = json_professional_child->next;
    }

    cJSON *json_languages = cJSON_GetObjectItem(json, "languages");
    if (json_languages)
    {
        int size = cJSON_GetArraySize(json_languages);
        printf("%s size:%d\n", json_languages->string, size);

        for (int i = 0; i < size; i++)
        {
            cJSON *json_languages_child = cJSON_GetArrayItem(json_languages, i);
            printf("%s[%d:%s]\n", json_languages->string, i, json_languages_child->valuestring);
        }
    }

    cJSON *json_phone = cJSON_GetObjectItem(json, "phone");
    cJSON *json_phone_number = cJSON_GetObjectItem(json_phone, "number");
    cJSON *json_phone_type = cJSON_GetObjectItem(json_phone, "type");
    printf("%s[%s:%s]\n", json_phone->string, json_phone_number->string, json_phone_number->valuestring);
    printf("%s[%s:%s]\n", json_phone->string, json_phone_type->string, json_phone_type->valuestring);

    cJSON *json_courses = cJSON_GetObjectItem(json, "courses");
    if (json_courses)
    {
        int size = cJSON_GetArraySize(json_courses);
        printf("%s size:%d\n", json_courses->string, size);
        for (int i = 0; i < size; i++)
        {
            cJSON *json_array = cJSON_GetArrayItem(json_courses, i);
            if (json_array)
            {
                cJSON *json_course_name = cJSON_GetObjectItem(json_array, "name");
                cJSON *json_course_price = cJSON_GetObjectItem(json_array, "price");
                printf("%s[%s:%.1lf]\n", json_courses->string, json_course_name->string, json_course_name->valuedouble);
                printf("%s[%s:%.1lf]\n", json_courses->string, json_course_price->string, json_course_price->valuedouble);
            }
        }
    }

    cJSON *json_vip = cJSON_GetObjectItem(json, "vip");
    cJSON_bool bvip = cJSON_IsBool(json_vip);
    if (bvip == cJSON_True)
    { // cJSON_False
        printf("root[%s:true]\n", json_vip->string);
    }
    else
    {
        printf("root[%s:false]\n", json_vip->string);
    }

    cJSON *json_address = cJSON_GetObjectItem(json, "address");
    if (json_address->type == cJSON_NULL)
    {
        printf("root[%s:null]\n", json_address->string);
    }
}

static void printJsonObjvalue(const cJSON *json)
{
    if (NULL == json)
    {
        printf("NULL object!\n");
        return;
    }

    switch (json->type)
    {
    case cJSON_False:
        printf("%s: false\n", json->string);
        break;
    case cJSON_True:
        printf("%s: true\n", json->string);
        break;
    case cJSON_NULL:
        printf("%s: cJSON_NULL\n", json->string);
        break;
    case cJSON_Number:
        printf("%s: %d, %f\n", json->string, json->valueint, json->valuedouble);
        break;
    case cJSON_String:
        printf("%s: %s\n", json->string, json->valuestring);
        break;
    case cJSON_Array:
        printf("%s: cJSON_Array\n", json->string);
        break;
    case cJSON_Object:
        printf("%s: cJSON_Object\n", json->string);
        break;
    default:
        printf("unknown type\n");
        break;
    }
}

void analysisJsonPrint(cJSON *json)
{
    if (NULL == json)
    {
        return;
    }

    cJSON *json_name = cJSON_GetObjectItem(json, "name");
    printJsonObjvalue(json_name);

    cJSON *json_age = cJSON_GetObjectItem(json, "age");
    printJsonObjvalue(json_age);

    cJSON *json_professional = cJSON_GetObjectItem(json, "professional");
    cJSON *json_professional_child = json_professional->child;
    while (json_professional_child != NULL)
    {
        printJsonObjvalue(json_professional_child);
        json_professional_child = json_professional_child->next;
    }

    cJSON *json_languages = cJSON_GetObjectItem(json, "languages");
    if (NULL == json_languages)
    {
        int size = cJSON_GetArraySize(json_languages);
        // printf("%s size:%d\n", json_languages->string, size);
        for (int i = 0; i < size; i++)
        {
            cJSON *json_languages_child = cJSON_GetArrayItem(json_languages, i);
            printJsonObjvalue(json_languages_child);
        }
    }

    cJSON *json_phone = cJSON_GetObjectItem(json, "phone");
    cJSON *json_phone_number = cJSON_GetObjectItem(json_phone, "number");
    cJSON *json_phone_type = cJSON_GetObjectItem(json_phone, "type");
    printJsonObjvalue(json_phone_number);
    printJsonObjvalue(json_phone_type);

    cJSON *json_courses = cJSON_GetObjectItem(json, "courses");
    if (json_courses)
    {
        int size = cJSON_GetArraySize(json_courses);
        printf("%s size:%d\n", json_courses->string, size);
        for (int i = 0; i < size; i++)
        {
            cJSON *arrayItem = cJSON_GetArrayItem(json_courses, i);
            if (NULL != arrayItem)
            {
                cJSON *json_course_name = cJSON_GetObjectItem(arrayItem, "name");
                cJSON *json_course_price = cJSON_GetObjectItem(arrayItem, "price");
                printJsonObjvalue(json_course_name);
                printJsonObjvalue(json_course_price);
            }
        }
    }

    cJSON *json_vip = cJSON_GetObjectItem(json, "vip");
    printJsonObjvalue(json_vip);

    cJSON *json_address = cJSON_GetObjectItem(json, "address");
    printJsonObjvalue(json_address);
}

void freeJson(cJSON *json)
{
    if (json != NULL)
    {
        cJSON_Delete(json);
    }
}

cJSON *readJsonFile(char *fileName)
{
    if (NULL == fileName)
    {
        return NULL;
    }

    FILE *fp = NULL;
    cJSON *json = NULL;
    char line[1024] = {0};
    char *data = NULL;

    //打开一个文本文件，文件必须存在，只允许读
    fp = fopen(fileName, "r");
    if (NULL != fp)
    {
        // seek末尾
        fseek(fp, 0, SEEK_END);
        //读取文件大小
        long len = ftell(fp);
        // seek起始位值
        fseek(fp, 0, SEEK_SET);
        data = (char *)malloc(len + 1);
        fread(data, 1, len, fp);
        fclose(fp);
    }

    printf("readJsonFile data:%s\n", data);
    cJSON *json_root = cJSON_Parse(data);

    if (NULL == json_root)
    {
        printf("cJSON_Parse error:%s\n", cJSON_GetErrorPtr());
    }

    if (NULL != data)
    {
        free(data);
    }

    return json_root;
}

//保存JSON
void writeJsonFile(char *fileName, cJSON *json)
{
    if (NULL == json || NULL == fileName)
    {
        return;
    }

    char *cjson = cJSON_Print(json);
    FILE *fp = NULL;
    //新建一个文本文件，已存在的文件将内容清空，允许读写
    fp = fopen(fileName, "w+");
    if (NULL != fp)
    {
        fwrite(cjson, strlen(cjson), 1, fp);
        fclose(fp);
    }
    if (NULL != cjson)
    {
        free(cjson);
    }
}

//已有JSON中添加
//"hobby": ["Basketball", "Football", "badminton"],
void addDataToJson(cJSON *json)
{
    if (NULL == json)
    {
        return;
    }

    cJSON *hobby = cJSON_CreateArray();
    cJSON_AddItemToArray(hobby, cJSON_CreateString("Basketball"));
    cJSON_AddItemToArray(hobby, cJSON_CreateString("Football"));
    cJSON_AddItemToArray(hobby, cJSON_CreateString("badminton"));

    /*
    CJSON_PUBLIC(void) cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item);
    */
    cJSON_AddItemToObject(json, "hobby", hobby);

    printfJson(json);
}

//修改
//"hobby": ["Basketball", "Football", "badminton"],
//修改为
//"hobby": ["Skating", "dance"],
void updateDataToJson(cJSON *json)
{
    if (NULL == json)
    {
        return;
    }

    cJSON *hobby = cJSON_CreateArray();
    cJSON_AddItemToArray(hobby, cJSON_CreateString("Skating"));
    cJSON_AddItemToArray(hobby, cJSON_CreateString("dance"));

    /*
    CJSON_PUBLIC(void) cJSON_ReplaceItemInArray(cJSON *array, int which, cJSON *newitem);
    CJSON_PUBLIC(void) cJSON_ReplaceItemInObject(cJSON *object,const char *string,cJSON *newitem);
    */
    cJSON_ReplaceItemInObject(json, "hobby", hobby);

    printfJson(json);
}

//删除
//"hobby": ["Skating", "dance"],
void deleteDataToJson(cJSON *json)
{
    if (NULL == json)
    {
        return;
    }

    /*
    CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromObject(cJSON *object, const char *string);
    */
    cJSON_DetachItemFromObject(json, "hobby");

    printfJson(json);
}

//创建JSON
cJSON *test1()
{
    printf("cJson读取文件保存文件\n");
    cJSON *json_root = createJson();
    //打印
    printfJson(json_root);

    //解析json
    analysisJsonObj(json_root);

    //解析json
    analysisJsonPrint(json_root);
    freeJson(json_root);
}

void test2()
{
    printf("cJson读取文件保存文件\n");
    char *fileName = "./person.json";

    //读取json
    cJSON *json_root = readJsonFile(fileName);
    //解析json
    analysisJsonObj(json_root);
    //解析json
    analysisJsonPrint(json_root);

    //写入json文件
    writeJsonFile(fileName, json_root);

    //释放
    freeJson(json_root);
}

//增删改
void test3()
{
    printf("cJson增删改\n");
    cJSON *json_root = createJson();

    addDataToJson(json_root);

    updateDataToJson(json_root);

    deleteDataToJson(json_root);

    freeJson(json_root);
}

int main(int argc, char *argv[])
{

    printf("\n-----------------------------------\n");
    test1();

    printf("\n-----------------------------------\n");
    test2();

    test3();
    printf("\n-----------------------------------\n");

    return 0;
}