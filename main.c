#include "reg52.h"
#include "intrins.h"    // _nop_()

#ifndef __C51__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif // __C51__

#define uint unsigned int
#define uchar unsigned char
#define null 0

// 下面这个条件编译屏蔽Keil编译器的检测行为
// 并为CLion语法检测提供伪类型对象
#ifndef __C51__
#define code
#define data
#define idata
#define pdata
#define xdata
// STC89C516RD+ 在使用扩展区的FFH以外区域时使用xdata指令 且xdata不可超过1280
//      使用pdata只能访问FFH以内区域   在使用xdata时需要在keil中开启“使用片内扩展RAM”
// STC89C52RC   直接使用pdata即可 pdata无法超过256
// 在使用片内扩展时都必须开启烧写器的“使用片内扩展RAM”的设置
typedef uchar sbit;
//extern sfr P0 = 0xFF;
//extern sfr P1 = 0xFF;
//extern sfr P2 = 0xFF;
//extern sfr P3 = 0xFF;

//  存储器类型
//  DATA 直接寻址的片内数据存储器(128B),访问速度最快
//  BDATA 可位寻址的片内数据存储器(16B),允许位与字节混合访问
//  IDATA 间接访问的片内数据存储器(256B),允许访问全部片内地址
//  PDATA 分页寻址的片外数据存储器(256B),用MOVX @Ri指令访问
//  XDATA 片外数据存储器(64KB),用MOVX @DPTR指令访问
//  CODE 程序存储器(64KB),用MOVC @A+DPTR指令访问

#endif // __C51__


void delayms(uint x)
{
    uint k;
    uchar i, j;
    for (k = x; k > 0; --k)
    {
        // 参数由 STC-ISP 计算得来
        // @22.1184MHz
        _nop_();
        i = 4;
        j = 146;
        do
        {
            while (--j);
        } while (--i);
    }
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
        case 11:    // +
            return 0x70;
        case 12:    // -
            return 0x40;
        case 13:    // *
            return 0x76;
        case 14:    // /
            return 0x64;
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


sbit row0 = P2 ^4;
sbit row1 = P2 ^5;
sbit row2 = P2 ^6;
sbit row3 = P2 ^7;

sbit col0 = P2 ^3;
sbit col1 = P2 ^2;
sbit col2 = P2 ^1;
sbit col3 = P2 ^0;

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
        }
    }
}


// error C249: 'DATA': SEGMENT TOO LARGE
// 按键回调函数附属信息结构体
typedef struct
{
    uchar edge; // 按下沿1或松开沿0
} CallBack_Key_Struct, *CallBack_Key_Struct_Ptr;

// 按键回调函数类型定义
// 传入附属信息结构体指针
typedef void (CallBack_Key)(CallBack_Key_Struct_Ptr CBKSP, uchar keynum);

// 按键回调函数附属信息结构体表
xdata
CallBack_Key_Struct KBKeySList[20] = {0};

// 按键回调函数表
// 前16个为矩阵键盘 0~15 后4个为板载键盘 16~19
xdata
CallBack_Key *CBKeyList[20] = {null};

// 板载键盘状态表
uchar OnBoardKeyState[4] = {0, 0, 0, 0};

// 调用按键回调函数
void CallKeyCallBackFunction()
{
    uchar i, o2, o3;
    // 先检查板载键盘
    for (i = 0; i != 4; ++i)
    {
        if (OnBoardKeyState[i])
        {
            if (KeyIsUp((uchar) (i + 1)))
            {
                OnBoardKeyState[i] = 0;
                KBKeySList[i + 16].edge = 0;
                (CBKeyList[i + 16])(&KBKeySList[i + 16], (char) (i + 16));
            }
        }
        else
        {
            if (KeyIsDown((uchar) (i + 1)))
            {
                OnBoardKeyState[i] = 1;
                KBKeySList[i + 16].edge = 1;
                (CBKeyList[i + 16])(&KBKeySList[i + 16], (char) (i + 16));
            }
        }
    }
    // 矩阵键盘
    for (i = 0; i != 16; ++i)
    {
        // 如果矩阵键盘现态与上态不同
        o2 = (uchar) (AKstate[i / 4][i % 4] & 0x2 ? 1 : 0);
        o3 = (uchar) (AKstate[i / 4][i % 4] & 0x3 ? 1 : 0);
        if (o2 != o3)
        {
            AKstate[i / 4][i % 4] ^= 0x3;   // 翻转上态使其与现态相等
            KBKeySList[i].edge = (uchar) (AKstate[i / 4][i % 4] & 0x3 ? 1 : 0);
            (CBKeyList[i])(&KBKeySList[i], i);
        }
    }
}

sbit beep = P3 ^5;
uchar BEEPFlag = 0;

void BEEP()
{
    beep = 1;
    BEEPFlag = 5;
}

void BEEPflush()
{
    if (BEEPFlag > 0)
    {
        --BEEPFlag;
    }
    else
    {
        beep = 0;
    }
}

//uchar LEDStateNum = 0;

void cbkf(CallBack_Key_Struct_Ptr CBKSP, uchar keynum)
{
    if (CBKSP->edge)
    {
//        LEDStateNum = keynum;
    }
}


long ThisNumber = 0;

uchar lock = 0;

uchar counted = 0;

void CB_Backspace(CallBack_Key_Struct_Ptr CBKSP, uchar keynum)
{
    if (CBKSP->edge)
    {
        ThisNumber /= 10;
    }
}

// 7 8 9 +
// 4 5 6 -
// 1 2 3 *
// 0 B = /

// < > C D

void CB_Test(CallBack_Key_Struct_Ptr CBKSP, uchar keynum)
{
    if (CBKSP->edge)
    {
        BEEP();
    }
}

uchar CountNumberLenth(long number)
{
    uchar len = 0;
    while (number)
    {
        number /= 10;
        ++len;
    }
    if (0 == len)
    {
        len = 1;    // 显示0
    }
    return len;
}

// LED总长
int LEDlong = 0;
// LED下标
int LEDindex = 0;

void CB_move(CallBack_Key_Struct_Ptr CBKSP, uchar keynum)
{
    if (CBKSP->edge)
    {
        switch (keynum)
        {
            case 16:    // >>
                --LEDindex;
                break;
            case 17:    // <<
                ++LEDindex;
                break;
            default:
                return;
        }
    }
}

// 符号栈与数字栈最大深度
#define StackDeep 32

//////    输入符号时  符号和当前数字同时压栈

// 数字列表
xdata
long NumberStack[StackDeep] = {0};
xdata
uchar NumberLenStack[StackDeep] = {0};
// 表头位置 表头总是在第一个空位处
uchar NumberStackHead = 0;

// 符号列表
xdata
uchar FlagStack[StackDeep] = {0};
// 表头位置
uchar FlagStackHead = 0;

// 栈内信息总长   显示总长=栈内总长+This总长
int InStackDataLenth = 0;

// 栈操作函数
void PushNumber(long number)
{
    uchar len = CountNumberLenth(number);
    NumberStack[NumberStackHead] = number;
    NumberLenStack[NumberStackHead] = len;
    InStackDataLenth += len;
    ++NumberStackHead;
}

long TopNumber()
{
    return NumberStack[NumberStackHead];
}

void PopNumber()
{
    --NumberStackHead;
    InStackDataLenth -= NumberLenStack[NumberStackHead];
}

void PushFlag(uchar flag)
{
    FlagStack[FlagStackHead] = flag;
    ++InStackDataLenth;
    ++FlagStackHead;
}

uchar TopFlag()
{
    return FlagStack[FlagStackHead];
}

void PopFlag()
{
    --FlagStackHead;
    --InStackDataLenth;
}

void StackClear()
{
    NumberStackHead = 0;
    FlagStackHead = 0;
    InStackDataLenth = 0;
}

void flushLEDil()
{
    LEDlong = CountNumberLenth(ThisNumber) + InStackDataLenth;
    if (LEDindex < 0)
    {
        LEDindex = 0;
    }
    if (LEDlong < 0)
    {
        LEDlong = 0;
    }
    if (LEDlong <= 3)
    {
        LEDindex = 0;
        return;
    }
    if (LEDlong < LEDindex)
    {
        LEDindex = LEDlong;
        return;
    }
}

void CB_Clear(CallBack_Key_Struct_Ptr CBKSP, uchar keynum)
{
    if (CBKSP->edge)
    {
        // TODO LED lock & 栈满
        lock = 0;
        ThisNumber = 0;
        StackClear();
    }
}

void CB_Delete(CallBack_Key_Struct_Ptr CBKSP, uchar keynum)
{
    if (CBKSP->edge)
    {
        // TODO LED lock
        lock = 0;
        ThisNumber = 0;
    }
}


void CB_Count(CallBack_Key_Struct_Ptr CBKSP, uchar keynum)
{
    uchar i;
    long temp;
    if (CBKSP->edge)
    {
        PushNumber(ThisNumber);
        // 将此当作计算寄存器
        ThisNumber = NumberStack[0];
        if (0 != NumberStackHead)
        {
            // 开始计算
            for (i = 1; i != NumberStackHead; ++i)
            {
                switch (FlagStack[i - 1])
                {
                    case 11:
                        ThisNumber += NumberStack[i];
                        break;
                    case 12:
                        ThisNumber -= NumberStack[i];
                        break;
                    case 13:
                        ThisNumber *= NumberStack[i];
                        break;
                    case 14:
                        ThisNumber /= NumberStack[i];
                        break;
                    default:
                        // 跳过
                        break;
                }
            }
        }
        counted = 1;
        lock = 0;
        StackClear();
    }
}

void CB_appendN(CallBack_Key_Struct_Ptr CBKSP, uchar keynum)
{
    uchar n = 0;
    if (CBKSP->edge)
    {
        if (lock)
        {
            BEEP();
            return;
        }
        if (counted)
        {
            ThisNumber = 0;
            counted = 0;
        }
        if (0 == ThisNumber && StackDeep == NumberStackHead)
        {
            // TODO LED 栈满
            BEEP();
            return;
        }
        switch (keynum)
        {
            case 0:
                n = 7;
                break;
            case 1:
                n = 8;
                break;
            case 2:
                n = 9;
                break;
            case 4:
                n = 4;
                break;
            case 5:
                n = 5;
                break;
            case 6:
                n = 6;
                break;
            case 8:
                n = 1;
                break;
            case 9:
                n = 2;
                break;
            case 10:
                n = 3;
                break;
            case 12:
                n = 0;
                break;
            default:
                return;
        }
        // 溢出计算
        // 裁掉最高位
        if (214748355L <= ThisNumber)
        {
            // TODO LED lock
            BEEP();
            // 锁定计算
            lock = 1;
        }
        else
        {
            ThisNumber *= 10;
            ThisNumber += n;
        }
    }
}

void CB_Flag(CallBack_Key_Struct_Ptr CBKSP, uchar keynum)
{
    uchar ThisFlag;
    if (CBKSP->edge)
    {
        if (StackDeep == NumberStackHead)
        {
            // TODO LED 栈满
            BEEP();
            return;
        }
        switch (keynum)
        {
            case 3:
                // + 11
                ThisFlag = 11;
                break;
            case 7:
                // - 12
                ThisFlag = 12;
                break;
            case 11:
                // * 13
                ThisFlag = 13;
                break;
            case 15:
                // / 14
                ThisFlag = 14;
                break;
            default:
                return;
        }
        PushNumber(ThisNumber);
        PushFlag(ThisFlag);
    }
}

void ShowLED()
{
    uchar i, j;
    long lsn;
    //TODO
    lsn = ThisNumber;
    // 计算下标初值
    for (i = 0; i != LEDindex; ++i)
    {
        lsn /= 10;
    }
    // 高位消隐
    for (j = 0; i != LEDlong && j != 3; ++i, ++j)
    {
        SetLED(j, (uchar) (lsn % 10));
        delayms(5);
        lsn /= 10;
    }

}

void init_key_list()
{
    CBKeyList[0] = CB_appendN;
    CBKeyList[1] = CB_appendN;
    CBKeyList[2] = CB_appendN;
    CBKeyList[3] = CB_Flag;    // +

    CBKeyList[4] = CB_appendN;
    CBKeyList[5] = CB_appendN;
    CBKeyList[6] = CB_appendN;
    CBKeyList[7] = CB_Flag;    // -

    CBKeyList[8] = CB_appendN;
    CBKeyList[9] = CB_appendN;
    CBKeyList[10] = CB_appendN;
    CBKeyList[11] = CB_Flag;   // *

    CBKeyList[12] = CB_appendN;
    CBKeyList[13] = CB_Backspace;   // B
    CBKeyList[14] = CB_Count;   // =
    CBKeyList[15] = CB_Flag;   // /

    CBKeyList[16] = CB_move;   // <<
    CBKeyList[17] = CB_move;   // >>
    CBKeyList[18] = CB_Clear;   // C
    CBKeyList[19] = CB_Delete;    // D
}


void main()
{

    beep = 0;
    init_key_list();

    while (1)
    {
        ArrayKeyScan();

        // 事件响应前置操作
        BEEPflush();

        // 回调函数调用响应事件
        CallKeyCallBackFunction();

        // 事件响应后置操作
        flushLEDil();

        ShowLED();


    }

}



// + - * / LED debug
//uchar Flag = 0;
//
//void CB_FlagTest(CallBack_Key_Struct_Ptr CBKSP, uchar keynum)
//{
//    if (CBKSP->edge)
//    {
//        Flag = (uchar) (keynum - 5);
//    }
//}
//
//
//void main()
//{
//
//    beep = 0;
//
//    CBKeyList[0] = cbkf;
//    CBKeyList[1] = cbkf;
//    CBKeyList[2] = cbkf;
//    CBKeyList[3] = cbkf;
//    CBKeyList[4] = cbkf;
//    CBKeyList[5] = cbkf;
//    CBKeyList[6] = cbkf;
//    CBKeyList[7] = cbkf;
//    CBKeyList[8] = cbkf;
//    CBKeyList[9] = cbkf;
//    CBKeyList[10] = cbkf;
//    CBKeyList[11] = cbkf;
//    CBKeyList[12] = cbkf;
//    CBKeyList[13] = cbkf;
//    CBKeyList[14] = cbkf;
//    CBKeyList[15] = cbkf;
//    CBKeyList[16] = CB_FlagTest;
//    CBKeyList[17] = CB_FlagTest;
//    CBKeyList[18] = CB_FlagTest;
//    CBKeyList[19] = CB_FlagTest;
//
//
//    while (1)
//    {
//
//        CallKeyCallBackFunction();
//
//        SetLED(0, Flag);
//
//        delayms(10);
//
//
//    }
//
//}


//void main()
//{
//    uchar i, j;
//    long lsn;
//
//    beep = 0;
//    init_key_list();
//
//    while (1)
//    {
//        ArrayKeyScan();
//
//        // 事件响应前置操作
//        BEEPflush();
//
//        // 回调函数调用响应事件
//        CallKeyCallBackFunction();
//
//        // 事件响应后置操作
//        LEDlong = CountNumberLenth(ThisNumber);
//        flushLEDli();
//
//        lsn = ThisNumber;
//        // 计算下标初值
//        for (i = 0; i != LEDindex; ++i)
//        {
//            lsn /= 10;
//        }
//        // 高位消隐
//        for (j = 0; i != LEDlong && j != 3; ++i, ++j)
//        {
//            SetLED(j, (uchar) (lsn % 10));
//            delayms(5);
//            lsn /= 10;
//        }
//
////        // debug
////        SetLED(0, (uchar) (LEDindex % 10));
////        delayms(5);
////        SetLED(1, (uchar) (LEDlong % 10));
////        delayms(5);
//
//
//
//
//    }
//
//}










//void main()
//{
//
//    // Key1~4
//    uchar kflag1 = 0;
//
//    char i, ie;
//    uchar temp;
//
//    while (1)
//    {
//        if (kflag1 && KeyIsUp(1))
//        {
//            kflag1 = 0;
//        }
//        if (!kflag1 && KeyIsDown(1))
//        {
//            kflag1 = 1;
//        }
//
//        ArrayKeyScan();
//        delayms(10);
//        ArrayKeyScan();
//
//
//        //debug
////        if (AKstate[0][0] & 0x2)
////        {
////            P1 = 0xFE;
////        }
////        else
////        {
////            P1 = 0xFF;
////        }
//
//        if (kflag1) // 扫描AKstate前半段或后半段
//        {
//            i = 8;
//            ie = 16;
//        }
//        else
//        {
//            i = 0;
//            ie = 8;
//        }
//
//        temp = 0;
//        for (; i != ie; ++i)
//        {
//            temp <<= 1;
//            if (AKstate[i / 4][i % 4] & 0x2) temp |= 0x1;
//        }
//        P1 = ~temp;
//
//
//    }
//}







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





#ifndef __C51__
#pragma clang diagnostic pop
#endif // __C51__
