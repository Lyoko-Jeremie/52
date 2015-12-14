#include "reg52.h"
#include "intrins.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

#define uint unsigned int
#define uchar unsigned char


void delayms(uint x)
{
    uint i, j;
    for (i = x; i > 0; --i)
        for (j = 110; j > 0; --j)
            /*empty*/  ;
}







// LED 管脚
// 片选从左往右 P1.3~P1.0 HI有效
// 管脚a~j~dp P0.0~P0.7  HI有效

uchar TranslateFlag(uchar F)
{
    switch (F)
    {
        case 0:
            return 0xF9;
        case 1:
            return 0xFA;
        case 2:
            return 0xFC;
        default:
            return 0xF8;
    }
}

uchar GetNextFlag(uchar *F)
{
    ++*F;
    *F %= 3;
    return TranslateFlag(*F);
}


uchar TranslateNumber(uchar Num)
{
    switch (Num)
    {
        case 0:
            return 0x3F;
        case 1:
            return 0x06;
        case 2:
            return 0x5B;
        case 3:
            return 0x4F;
        case 4:
            return 0x66;
        case 5:
            return 0x6D;
        case 6:
            return 0x7D;
        case 7:
            return 0x07;
        case 8:
            return 0x7F;
        case 9:
            return 0x67;
        default:
            return 0x00;
    }
}

uchar GetNextNumber(uchar *N)
{
    ++*N;
    *N %= 10;
    return TranslateNumber(*N);
}


void SetLED(uchar F, uchar Num)
{
    P1 = TranslateFlag(F);
    P0 = TranslateNumber(Num);
}


sbit db1 = P1 ^0;
sbit db2 = P1 ^1;
sbit db3 = P1 ^2;
sbit db4 = P1 ^3;


void setDB(uchar n, uchar mode)
{
    switch (n)
    {
        case 1:
            db1 = mode;
            break;
        case 2:
            db2 = mode;
            break;
        case 3:
            db3 = mode;
            break;
        case 4:
            db4 = mode;
            break;
        default:
            return;
    }
}


sbit key1 = P3 ^3;
sbit key2 = P3 ^4;
sbit key3 = P3 ^6;
sbit key4 = P3 ^7;

uchar KeyIsDown(uchar n)
{
    switch (n)
    {
        case 1:
            if (key1)
            {
                delayms(10);
                if (key1)
                {
                    return 1;
                }
            }
            break;
        case 2:
            if (key2)
            {
                delayms(10);
                if (key2)
                {
                    return 1;
                }
            }
            break;
        case 3:
            if (key3)
            {
                delayms(10);
                if (key3)
                {
                    return 1;
                }
            }
            break;
        case 4:
            if (key4)
            {
                delayms(10);
                if (key4)
                {
                    return 1;
                }
            }
            break;
        default:
            return 0;
    }
    return 0;
}

uchar KeyIsUp(uchar n)
{
    switch (n)
    {
        case 1:
            if (!key1)
            {
                delayms(10);
                if (!key1)
                {
                    return 1;
                }
            }
            break;
        case 2:
            if (!key2)
            {
                delayms(10);
                if (!key2)
                {
                    return 1;
                }
            }
            break;
        case 3:
            if (!key3)
            {
                delayms(10);
                if (!key3)
                {
                    return 1;
                }
            }
            break;
        case 4:
            if (!key4)
            {
                delayms(10);
                if (!key4)
                {
                    return 1;
                }
            }
            break;
        default:
            return 0;
    }
    return 0;
}


sbit row0 = P2 ^7;
sbit row1 = P2 ^6;
sbit row2 = P2 ^5;
sbit row3 = P2 ^4;

sbit col0 = P2 ^0;
sbit col1 = P2 ^1;
sbit col2 = P2 ^2;
sbit col3 = P2 ^3;

// 0x2 是否确实按下
// 0x1 是否程检测到过一次与是否按下状态不符
char AKstate[4][4] = {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
};

uchar GetColState(char n)
{
    switch (n)
    {
        case 0:
            return (uchar) col0;
        case 1:
            return (uchar) col1;
        case 2:
            return (uchar) col2;
        case 3:
            return (uchar) col3;
        default:
            return 0;
    }
}

void ArrayKeyScan()
{
    char i, j;
    for (i = 0; i != 4; ++i)
    {
        row0 = 1;
        row1 = 1;
        row2 = 1;
        row3 = 1;
        if (0 == i) row0 = 0;
        if (1 == i) row1 = 0;
        if (2 == i) row2 = 0;
        if (3 == i) row3 = 0;
        for (j = 0; j != 4; ++j)
        {
            if (!GetColState(j))
            {
                // 检测到接通
                if (AKstate[i][j] & 0x2)
                {
                    // 且本就为按下状态
                    AKstate[i][j] &= 0xFE;  // 清除flag0x1
                }
                else
                {
                    // 且本不为按下状态
                    if (AKstate[i][j] & 0x1)
                    {
                        // 上次已经标记
                        AKstate[i][j] ^= 0x3; // 与0x1|0x2异或 去除0x1 标记0x2
                    }
                    else
                    {
                        // 标记flag0x1
                        AKstate[i][j] |= 0x1;
                    }
                }
            }
            else
            {
                // 检测到未接通
                if (!(AKstate[i][j] & 0x2))
                {
                    // 且本就为未按下状态
                    AKstate[i][j] &= 0xFE;  // 清除flag0x1
                }
                else
                {
                    // 且本为按下状态
                    if (AKstate[i][j] & 0x1)
                    {
                        // 上次已经标记
                        AKstate[i][j] &= 0xFC;  // 清除 0x1|0x2
                    }
                    else
                    {
                        // 标记flag0x1
                        AKstate[i][j] |= 0x1;   // 标记
                    }
                }
            }

            // 旧版 有隐藏的时序问题
//            // 如果标记为“未按下”却检测到“接通”
//            if (!(AKstate[i][j] & 0x2) && !GetColState(j))
//            {
//                if (AKstate[i][j] & 0x1)
//                {
//                    // 将上次标记为接通的设为按下
//                    AKstate[i][j] = 0;
//                    AKstate[i][j] |= 0x2;
//                }
//                else
//                {
//                    // 将上次未标记为接通的设为接通
//                    AKstate[i][j] |= 0x1;
//                }
//            }
//            // 如果标记为“按下”却检测到“未接通”
//            if ((AKstate[i][j] & 0x2) && GetColState(j))
//            {
//                if (AKstate[i][j] & 0x1)
//                {
//                    // 将上次标记为未接通的设为未按下
//                    AKstate[i][j] = 0;
//                    AKstate[i][j] |= 0x0;
//                }
//                else
//                {
//                    // 将上次未标记为未接通的设为未接通
//                    AKstate[i][j] |= 0x1;
//                }
//            }
        }
    }
}
























void main()
{

    // Key1~4
    uchar kflag1 = 0;

    char i, ie;
    uchar temp;

    while (1)
    {
        if (kflag1 && KeyIsUp(1))
        {
            kflag1 = 0;
        }
        if (!kflag1 && KeyIsDown(1))
        {
            kflag1 = 1;
        }

        ArrayKeyScan();
        delayms(10);
        ArrayKeyScan();


        //debug
//        if (AKstate[0][0] & 0x2)
//        {
//            P1 = 0xFE;
//        }
//        else
//        {
//            P1 = 0xFF;
//        }

        if (kflag1) // 扫描AKstate前半段或后半段
        {
            i = 8;
            ie = 16;
        }
        else
        {
            i = 0;
            ie = 8;
        }

        temp = 0;
        for (; i != ie; ++i)
        {
            temp <<= 1;
            if (AKstate[i / 4][i % 4] & 0x2) temp |= 0x1;
        }
        P1 = ~temp;


    }
}







//sbit a = P2 ^0;
//sbit b = P2 ^1;
//sbit c = P2 ^6;
//sbit d = P2 ^7;
//
//void main()
//{
//    a = 1;
//    b = 0;
//    while (1)
//    {
//        delayms(500);
//        setDB(1, 0);
//        setDB(2, 0);
//        if (c)
//        {
//            setDB(1, 1);
//        }
//        if (d)
//        {
//            setDB(2, 1);
//        }
//        a = ~a;
//        b = ~b;
//
//    }
//}







//void main()
//{
//    uchar F = 0;
//    row0 = 0;
//    row1 = 1;
//    row2 = 1;
//    row3 = 1;
//    while (1)
//    {
//        if (!F && !GetColState(0))
//        {
//            delayms(10);
//            if (!GetColState(0))
//            {
//                F = 1;
//                setDB(1, 0);
//            }
//        }
//        if (F && GetColState(0))
//        {
//            delayms(10);
//            if (GetColState(0))
//            {
//                F = 0;
//                setDB(1, 1);
//            }
//        }
//    }
//}















//sbit beep = P3 ^5;
//
//void main()
//{
//
//
//
//    // Key1~4
//    int n = 0;
//    uchar kflag1 = 0;
//    uchar kflag2 = 0;
//    int temp;
//    uchar i;
//
//
//
//    // init
//    beep = 0;
//
//    while (1)
//    {
//        // Key1~4 begin
//        if (kflag1 && KeyIsUp(1))
//        {
//            kflag1 = 0;
//            beep = 1;
//        }
//        if (!kflag1 && KeyIsDown(1))
//        {
//            kflag1 = 1;
//            beep = 0;
//            ++n;
//            n %= 1000;
//        }
//        if (kflag2 && KeyIsUp(2))
//        {
//            kflag2 = 0;
//        }
//        if (!kflag2 && KeyIsDown(2))
//        {
//            kflag2 = 1;
//            if (n > 0)
//            {
//                --n;
//            }
//            else
//            {
//                n = 999;
//            }
//        }
//        temp = n;
//        for (i = 0; i != 3; ++i)
//        {
//            delayms(5);
//            SetLED(i, (uchar) (temp % 10));
//            temp /= 10;
//        }
//        // Key1~4 end
//    }
//}














//sbit db1 = P1 ^0;
//sbit db2 = P1 ^1;
//sbit db3 = P1 ^2;
//sbit db4 = P1 ^3;
//
//
//
//void setDB(uchar n, uchar mode)
//{
//    switch (n)
//    {
//        case 1:
//            db1 = mode;
//            break;
//        case 2:
//            db2 = mode;
//            break;
//        case 3:
//            db3 = mode;
//            break;
//        case 4:
//            db4 = mode;
//            break;
//        default:
//            return;
//    }
//}
//
//
//void main()
//{
//    uchar F[] = {0, 0, 0, 0};
//    uchar i;
//    while (1)
//    {
//        for (i = 1; i != 5; ++i)
//        {
//            if (F[i - 1] && KeyIsUp(i))
//            {
//                F[i - 1] = 0;
//                setDB(i, 0);
//            }
//            if (!F[i - 1] && KeyIsDown(i))
//            {
//                F[i - 1] = 1;
//                setDB(i, 1);
//            }
//        }
//    }
//}









//uchar num;
//
//void main()
//{
//    uchar f = 2;
//    uchar n = 0;
//
//    TMOD = 0x01;
//    TH0 = (0xFFFF - 0xB330) / 256;
//    TL0 = (0xFFFF - 0xB330) % 256;
//    EA = 1;
//    ET0 = 1;
//    TR0 = 1;
//
//
//    while (1)
//    {
//        if (20 == num)
//        {
//            num = 0;
//
//            P0 = 0x80;
//            P1 = GetNextFlag(&f);
//            P0 = GetNextNumber(&n);
//            n %= 3;
//        }
//    }
//}
//
//
//void T0_time() interrupt 1
//{
//    TH0 = (0xFFFF - 0xB330) / 256;
//    TL0 = (0xFFFF - 0xB330) % 256;
//    ++num;
//}















// LED 123
//void main()
//{
//    uchar f = 2;
//    uchar n = 0;
//    while (1)
//    {
//        delayms(5);
//        P0 = 0x80;
//        P1 = GetNextFlag(&f);
//        P0 = GetNextNumber(&n);
//        n %= 3;
//    }
//
//}
















// LED flush loop
//void main()
//{
//    uchar f = 2;
//    uchar n = 9;
//    while (1)
//    {
//        delayms(500);
//        P1 = GetNextFlag(&f);
//        P0 = GetNextNumber(&n);
//    }
//
//}











//sbit led1 = P1 ^0;
//void main()
//{
//    uchar aa;
//    aa = 0xfe;
//    while (1)
//    {
////        led1 = 0;
////        delayms(500);
////        led1 = 1;
//        delayms(500);
//        P1 = aa;
//        aa = _crol_(aa, 1);
//    }
//
//}





#pragma clang diagnostic pop