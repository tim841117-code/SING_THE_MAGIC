#pragma once
#include "MyPG.h"

extern  void  MyGameMain_Finalize( );
extern  void  MyGameMain_Initialize( );
extern  void  MyGameMain_UpDate( );
extern  void  MyGameMain_Render2D( );

enum class TaskFlag
{
	Non,		//?????öÊ÷“õÔ÷v
	Title,		//????
	Game,		//?????
	Ending,

	//????????øJ?
};
extern int gResultScore;