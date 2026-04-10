#include <STC15F2K60S2.H>
#include <ds1302.c>
#define true 1
#define false 0 

/* Gp0 Mux */
#define P0F_NONE 0x00
#define P0F_LED 0x80
#define P0F_ULN 0xA0
#define P0F_DPos 0xC0
#define P0F_DSeg 0xE0

/// @brief 设定 `GP0*` 复用器选择
/// @param mode 外设模式, `P0F_*`
void funcSel(char mode) {
    P2 = (P2 & 0x1F) | mode;
}

/* Led Operator */
/// @brief 设定LED配布
void setLed(char arrange) {
    P0 = arrange;
    funcSel(P0F_LED);
	funcSel(P0F_NONE);
}

/* Segment Displayer */
char segBuffer[8] = {0};

/// @brief 准备段码缓存
void cleanSegBuffer() {
    int segCleanI;
    for (segCleanI = 0; segCleanI < 8; segCleanI++){
        segBuffer[segCleanI] = 0xFF;
    }
}

/// @brief 段码数字查找表
data char segNumberLut[10] = {
    0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90
};

void Delay10ms(void)	//@11.0592MHz
{
	unsigned char data i, j;

	i = 108;
	j = 145;
	do
	{
		while (--j);
	} while (--i);
}

/// @brief 设定段码内容
/// @param pos 位置
/// @param content 段码
void segDisplay(char pos, char content) {
    segBuffer[pos] = content;
}

/// @brief 段码屏守护程序 放置在主逻辑
void segDaemon() {
    static int curPos = 0;
    //一次执行只维护一位
    P0 = 0xFF;
	funcSel(P0F_DSeg);
	funcSel(P0F_NONE);
	P0 = 1 << curPos;
	funcSel(P0F_DPos);
	funcSel(P0F_NONE);
	P0 = segBuffer[curPos];
	funcSel(P0F_DSeg);
	funcSel(P0F_NONE);
	
	curPos++;
	if(curPos >= 8) curPos = 0;
}

/* Keyboard Handler */

int xdata btnDat[4][4] = {0}; 

void btnScanCol(char col) {
    if (!P30) {
        btnDat[col][0]++;
    } else btnDat[col][0] = 0;
    if (!P31) {
        btnDat[col][1]++;
    } else btnDat[col][1] = 0;
    if (!P32) {
        btnDat[col][2]++;
    } else btnDat[col][2] = 0;
    if (!P33) {
        btnDat[col][3]++;
    } else btnDat[col][3] = 0;
}

/// @brief 清理键盘输入针脚
void cleanMatrixScanner(){
    P30 = 1;
	P31 = 1;
	P32 = 1;
	P33 = 1;
}

/// @brief 键盘守护程序 放置在主逻辑
void btnDaemon() {
    static char curCol = 0;
    //依旧一次只扫一列
    char dcols[4] = {1,1,1,1};
    dcols[curCol] = 0;
    P44 = dcols[0];
    P42 = dcols[1];
    P35 = dcols[2];
    P34 = dcols[3];
    btnScanCol(curCol);
    curCol++;
    if (curCol >= 4) curCol = 0;
}

/// @brief 设定按键状态
/// @param x 
/// @param y 
/// @param stat 
void setBtnStat(char x, char y, int stat){
    btnDat[x][y] = stat;
}

/// @brief 获取按键状态
/// @param x 
/// @param y 
/// @return x,y 处按键状态
int getBtnStat(char x, char y) {
    return btnDat[x][y];
}



void Update(void) {
    char nu = getBtnStat(2,3)/2000;
    segDisplay(0,segNumberLut[nu<9 ? nu : 8]);
}


/* Timing Logic */
void main() {
    cleanSegBuffer();
    cleanMatrixScanner();
    setLed(0xFF);
    while (true) {
        /* main cycle */
        btnDaemon();
        Update();
        segDaemon();
        //Delay10ms();
    }
}