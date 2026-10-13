#include "MyPG.h"
#include "MyGameMain.h"

//
namespace Title
{
	
	int					logoPosY;
	DG::Image::SP       img;

	//-----------------------------------------------------------------------------
	
	//-----------------------------------------------------------------------------
	void Initialize()
	{
		img = DG::Image::Create("./data/image/title.png");
		logoPosY = -270; 

		
		ge->dgi->EffectState().param.bgColor = ML::Color(1, 0, 0, 1);
	}
	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	void Finalize()
	{
		img.reset( );
	}
	//-----------------------------------------------------------------------------
	
	//-----------------------------------------------------------------------------
	TaskFlag UpDate()
	{
		auto inp = ge->in1->GetState();
		logoPosY += 9;
		if (logoPosY  >  0) { logoPosY = 0; }
		//logoPosY = min(logoPosY + 9, 0);

		TaskFlag rtv = TaskFlag::Title;//
		if (logoPosY >= 0) {
			if (true == inp.B1.down) {
				rtv = TaskFlag::Game;
			}
		}
		return rtv;
	}
	//-----------------------------------------------------------------------------
	
	//-----------------------------------------------------------------------------
	void Render()
	{
		ML::Box2D  draw(0, 0, 640, 450);
		draw.Offset(0, logoPosY);
		ML::Box2D  src(0, 0, 640, 450);

		img->Draw(draw, src);
	}
}