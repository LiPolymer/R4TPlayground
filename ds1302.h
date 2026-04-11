typedef struct sTime {
	unsigned char s;
	unsigned char min;
	unsigned char hr;
} Time;

void setDsTime(Time time);

Time getDsTime();