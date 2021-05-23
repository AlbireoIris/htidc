#include "_freecplus.h"
//#include "_mylib.h"

int main()
{
    int score[3] = {85, 56, 100};
    double height[3] = {56.6, 67.8, 44.5};
    const char *name[3] = {"XiaoMin", "XiaoHong", "XiaoHei"};

    CJson_Array cJSON_Array1;
    CJson_Object CJson_Object1;

    cJSON_Array1.Add_Item(score, 3);
    cJSON_Array1.Add_Item(height, 3);
    cJSON_Array1.Add_Item(name, 3);

    CJson_Object1.Add_Item("score", score, 3);
    CJson_Object1.Add_Item("heigth", height, 3);
    CJson_Object1.Add_Item("name", name, 3);

    CJson_Object1.Add_Item("Array", cJSON_Array1);

    printf("the String is %s\n", CJson_Object1.Printf());

    //the String is {
    //        "score":        [85, 56, 100],
    //        "heigth":       [56.600000, 67.800000, 44.500000],
    //        "name": ["XiaoMin", "XiaoHong", "XiaoHei"],
    //        "Array":        [[85, 56, 100], [56.600000, 67.800000, 44.500000], ["XiaoMin", "XiaoHong", "XiaoHei"]]
    //}

    return 0;
}
