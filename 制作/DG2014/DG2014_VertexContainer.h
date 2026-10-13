#pragma warning(disable:4996)
#pragma once
#pragma warning( disable : 4005 )
#include <d3dx10.h>
#include <d3dCompiler.h>
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d10.lib")
#pragma comment(lib,"d3dx10.lib")
#pragma comment(lib,"d3dCompiler.lib")
#pragma warning( default : 4005 )


#include "../myLib.h"
#include <string>

#include "DG2014_VertexBuffer.h"

namespace DG
{
	//-----------------------------------------------------------------------
	//???????????????
	class VB_Container
	{
	//-----------------------------------------------------------------------
	private:
		D3D10_PRIMITIVE_TOPOLOGY			priTopo;
		vector<VertexBuffer::SP>			vbs;
	//-----------------------------------------------------------------------
	public:
		using SP = shared_ptr<VB_Container>;
	//-----------------------------------------------------------------------
	private:
		VB_Container( );
		bool Initialize(D3D10_PRIMITIVE_TOPOLOGY  pt_);
	//-----------------------------------------------------------------------
	public:
		~VB_Container( );
		static SP  Create(D3D10_PRIMITIVE_TOPOLOGY  pt_);
	//-----------------------------------------------------------------------
	public:
		bool PushBack(BYTE da_[], UINT str_, UINT n_);	//	??????õÏ??ø²????ùD?øŒ?????öwöm?
		bool  IASet(UINT  ofs_);	//ofs_???????strideùŸ
		UINT  Num( );				//÷øø¢??????????????????
		VertexBuffer::SP  GetVB(UINT  n_);//	ô™økùÖ???????????????
	};
}
