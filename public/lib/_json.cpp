#include"_json.h"

//以下是class Cjson_Object 的内容
CJson_Object::CJson_Object()
{
    m_Json = cJSON_CreateObject();
}

CJson_Object::~CJson_Object()
{
    if (m_Json != NULL)
        cJSON_Delete(m_Json);
}

int CJson_Object::ItemCount()
{
    cJSON_GetArraySize(m_Json);
}

char *CJson_Object::Printf()
{
    return cJSON_Print(m_Json);
}

char *CJson_Object::Print()
{
    return cJSON_PrintUnformatted(m_Json);
}

void CJson_Object::Add_Item(char *ItemName, char *strBuffer)
{
    cJSON_AddItemToObjectCS(m_Json, ItemName, cJSON_CreateString(strBuffer));
}

void CJson_Object::Add_Item(char *ItemName, int NumBuffer)
{
    cJSON_AddItemToObject(m_Json, ItemName, cJSON_CreateNumber(NumBuffer));
}

void CJson_Object::Add_Item(char *ItemName, double NumBuffer)
{
    cJSON_AddItemToObject(m_Json, ItemName, cJSON_CreateNumber(NumBuffer));
}

void CJson_Object::Add_Item(char *ItemName)
{
    cJSON_AddItemToObject(m_Json, ItemName, cJSON_CreateNull());
}

void CJson_Object::Add_Item(char *ItemName, const char *StrArry[], int ArryNum)
{
    cJSON_AddItemToObject(m_Json, ItemName, cJSON_CreateStringArray(StrArry, ArryNum));
}

void CJson_Object::Add_Item(char *ItemName, int IntArry[], int ArryNum)
{
    cJSON_AddItemToObject(m_Json, ItemName, cJSON_CreateIntArray(IntArry, ArryNum));
}

void CJson_Object::Add_Item(char *ItemName, double DoubleArry[], int ArryNum)
{
    cJSON_AddItemToObject(m_Json, ItemName, cJSON_CreateDoubleArray(DoubleArry, ArryNum));
}

void CJson_Object::Add_Item(char *ItemName, bool Bool)
{
    cJSON_AddItemToObject(m_Json, ItemName, cJSON_CreateBool(Bool));
}

void CJson_Object::Add_Item(char *ItemName, cJSON *Item)
{
    cJSON_AddItemToObjectCS(m_Json, ItemName, Item);
}

void CJson_Object::Add_Item(char *ItemName, class CJson_Object &Add_cJson)
{
    cJSON_AddItemReferenceToObject(m_Json, ItemName, Add_cJson.m_Json);
}

void CJson_Object::Remove_Item(char *ItemName)
{
    cJSON_DeleteItemFromObject(m_Json, ItemName);
}

void CJson_Object::Add_Item(char *ItemName, class CJson_Array &Add_cJson)
{
    cJSON_AddItemReferenceToObject(m_Json, ItemName, Add_cJson.m_cJson);
}

//以下是CJson_Array类的内容
CJson_Array::CJson_Array()
{
    m_cJson = cJSON_CreateArray();
}

CJson_Array::~CJson_Array()
{
    if (m_cJson != NULL)
        cJSON_Delete(m_cJson);
}

char *CJson_Array::Print()
{
    return cJSON_PrintUnformatted(m_cJson);
}

char *CJson_Array::Printf()
{
    return cJSON_Print(m_cJson);
}

int CJson_Array::ItemCount()
{
    return cJSON_GetArraySize(m_cJson);
}

void CJson_Array::Add_Item(class CJson_Array &Item)
{
    cJSON_AddItemReferenceToArray(m_cJson, Item.m_cJson);
}

void CJson_Array::Add_Item(class CJson_Object &Item)
{
    cJSON_AddItemReferenceToArray(m_cJson, Item.m_Json);
}

void CJson_Array::Add_Item()
{
    cJSON_AddItemToArray(m_cJson, cJSON_CreateNull());
}

void CJson_Array::Add_Item(int IntNum)
{
    cJSON_AddItemToArray(m_cJson, cJSON_CreateNumber(IntNum));
}

void CJson_Array::Add_Item(double DouNum)
{
    cJSON_AddItemToArray(m_cJson, cJSON_CreateNumber(DouNum));
}

void CJson_Array::Add_Item(bool Bool)
{
    cJSON_AddItemToArray(m_cJson, cJSON_CreateBool(Bool));
}

void CJson_Array::Add_Item(char *String)
{
    cJSON_AddItemToArray(m_cJson, cJSON_CreateString(String));
}

void CJson_Array::Add_Item(const char *StringArray[], int StringCount)
{
    cJSON_AddItemToArray(m_cJson, cJSON_CreateStringArray(StringArray, StringCount));
}

void CJson_Array::Add_Item(int IntArray[], int IntCount)
{
    cJSON_AddItemToArray(m_cJson, cJSON_CreateIntArray(IntArray, IntCount));
}

void CJson_Array::Add_Item(double DouArray[], int DouCount)
{
    cJSON_AddItemToArray(m_cJson, cJSON_CreateDoubleArray(DouArray, DouCount));
}
//关于json组装的class的内容结束
