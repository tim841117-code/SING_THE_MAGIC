//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include "Task_LC.h"
#include "MyGameMain.h"

namespace  LC
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//????????
	bool  Resource::Initialize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//???????
	bool  Resource::Finalize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//????????öwöm??????????
	bool  Object::Initialize()
	{
		//??????????
		__super::Initialize(defGroupName, defName, true);
		//???????öwömor??????
		this->res = Resource::Create();

		//???????
		MyGameMain_Initialize();

		//?????öwöm

		return  true;
	}
	//-------------------------------------------------------------------
	//?õK?????õ¢???????????
	bool  Object::Finalize()
	{
		//??????????
		MyGameMain_Finalize();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//?????????öwöm
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//??õö????????????
	void  Object::UpDate()
	{
		MyGameMain_UpDate();
	}
	//-------------------------------------------------------------------
	//?????????????????
	void  Object::Render2D_AF()
	{
		MyGameMain_Render2D();
	}

	//??????????????????????????????????????????
	//?????øŒ??????????
	//??????????????????????????????????????????
	//-------------------------------------------------------------------
	//???öwöm??
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//????????ø²?
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//????????ô¸ù—???Kill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize()
	{
		return  this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = this->Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() {	}
	//-------------------------------------------------------------------
	//????????öwöm
	Resource::SP  Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new  Resource());
			if (sp) {
				sp->Initialize();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource() {}
	//-------------------------------------------------------------------
	Resource::~Resource() { this->Finalize(); }
}