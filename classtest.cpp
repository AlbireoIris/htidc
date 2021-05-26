#include <stdio.h>
#include "_freecplus.h"

using namespace std;

void _pro_main();
void _num_main();

int main()
{
    //_pro_main();
    _num_main();

    return 0;
}

void _pro_main()
{
    CRand Rand(0.56);

    int Return_True;
    int Return_False;
    for (int acc = 0; acc < 100000; acc++)
    {
        if (Rand.GetRandResult() == false)
            Return_False++;
        else
            Return_True++;
    }
    printf("the true num is %d\n", Return_True);
    printf("the false num is %d\n", Return_False);
}

void _num_main()
{
    CRand Rand(1, 10);
    int num1 = 0;
    int num2 = 0;
    int num3 = 0;
    int num4 = 0;
    int num5 = 0;
    int num6 = 0;
    int num7 = 0;
    int num8 = 0;
    int num9 = 0;
    int num10 = 0;

    for (int ii = 0; ii < 10000; ii++)
    {
        switch (Rand.GetRandNum())
        {
        case 1:
            num1++;
            break;
        case 2:
            num2++;
            break;
        case 3:
            num3++;
            break;
        case 4:
            num4++;
            break;
        case 5:
            num5++;
            break;
        case 6:
            num6++;
            break;
        case 7:
            num7++;
            break;
        case 8:
            num8++;
            break;
        case 9:
            num9++;
            break;
        case 10:
            num10++;
            break;

        default:
            break;
        }
    }

    printf("the num1 = %d\n", num1);
    printf("the num2 = %d\n", num2);
    printf("the num3 = %d\n", num3);
    printf("the num4 = %d\n", num4);
    printf("the num5 = %d\n", num5);
    printf("the num6 = %d\n", num6);
    printf("the num7 = %d\n", num7);
    printf("the num8 = %d\n", num8);
    printf("the num9 = %d\n", num9);
    printf("the num10 = %d\n", num10);
}
