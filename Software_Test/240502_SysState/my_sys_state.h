
#ifndef my_sys_state_h
#define my_sys_state_h
#endif

#ifdef __cplusplus
extern "C"{
#endif

// list of possible work modes
#define m0_Stop			0
#define m1_Calib		1
#define m2_LineFollow	2
#define m3_GoBackToLine	3
#define m4_WithinWalls	4
#define m5_ExploreToFindLine	5


class MySysState
{
	public:
	MySysState() = default;
	~MySysState();

	int mode= m0_Stop;
	bool detectColorsFlag=1, beepOnColorFlag=1, waitOnColorFlag=1;

};

#ifdef __cplusplus
} // extern "C"
#endif
