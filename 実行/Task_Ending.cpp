#include "MyPG.h"
#include "MyGameMain.h"


namespace Ending
{
	
	int					logoPosY;
	DG::Image::SP       img;
	DG::Font::SP font;
	//-----------------------------------------------------------------------------
	
	//-----------------------------------------------------------------------------
	void Initialize()
	{
		img = DG::Image::Create("./data/image/end.png");
		logoPosY = +270;
		font = DG::Font::Create("MS Gothic", 16, 32);
	}
	//-----------------------------------------------------------------------------
	
	//-----------------------------------------------------------------------------
	void Finalize()
	{
		img.reset( );
		font.reset();
	}
	//-----------------------------------------------------------------------------
	
	//-----------------------------------------------------------------------------
	TaskFlag UpDate()
	{
		auto inp = ge->in1->GetState();
		logoPosY -= 9;
		if (logoPosY  <  0) { logoPosY = 0; }

		TaskFlag rtv = TaskFlag::Ending;//
		if (logoPosY <= 0) {
			if (true == inp.B1.down) {
				rtv = TaskFlag::Title;	//
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
		char buf[64];
		sprintf_s(buf, "SCORE:%d", gResultScore);

		// point
		font->Draw(ML::Box2D(260, 170, 300, 40), buf);
	}
}