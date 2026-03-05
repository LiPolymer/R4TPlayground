#include <STC15F2K60S2.H>
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

int segCleanI;
void cleanSegBuffer() {
    for (segCleanI = 0; segCleanI < 8; segCleanI++){
        segBuffer[segCleanI] = 0xFF;
    }
}

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

void Update(void) {
    static int curv = 0;
    static int freqC = 0;
    if (freqC < 10000) freqC++;
    else {
        freqC = 0;
        if (curv >= 8) curv = 0;
        segDisplay(0,segNumberLut[curv]);
        curv++;
    }
}


/* Timing Logic */
void main() {
    cleanSegBuffer();
    setLed(0xFF);
    while (true) {
        /* master logic */
        segDaemon();
        Update();
        //Delay10ms();
    }
}