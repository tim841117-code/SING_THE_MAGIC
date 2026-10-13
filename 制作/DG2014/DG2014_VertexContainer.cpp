#include "DG2014_Ver3_6.h"


namespace DG
{
	//-----------------------------------------------------------------------
	//???????????????
	//----------------------------------------------------------------------
	VB_Container::VB_Container( )
	{
	}
	//----------------------------------------------------------------------
	VB_Container::~VB_Container( )
	{
	}	
	//----------------------------------------------------------------------
	bool VB_Container::Initialize(D3D10_PRIMITIVE_TOPOLOGY  pt_)
	{
		priTopo = pt_;
		return true;
	}
	//----------------------------------------------------------------------
	//	öwöm??
	VB_Container::SP  VB_Container::Create(D3D10_PRIMITIVE_TOPOLOGY  pt_)
	{
		if( VB_Container::SP ob = VB_Container::SP(new VB_Container( )) ){
			if( ob->Initialize(pt_) ){
				return ob;
			}
		}
		return nullptr;
	}
	//----------------------------------------------------------------------
	bool  VB_Container::IASet(UINT ofs_)	//ofs_???????strideùŸ
	{
		auto  dgi = DG::DGObject::GetInst( );
		if(dgi == 0){	return false; }

		//????????????????
		dgi->Device( ).IASetPrimitiveTopology( priTopo );
		auto  it = vbs.begin( );
		for(int c=0; it != vbs.end( ); ++c, ++it)
		{
			//???????????????
			UINT  ofs = ofs_ * (*it)->stride;
			dgi->Device( ).IASetVertexBuffers( c, 1, &(*it)->buf, &(*it)->stride, &ofs);
		}
		return true;
	}
	//----------------------------------------------------------------------
	//÷øø¢????
	UINT  VB_Container::Num( )
	{
		auto  it = vbs.begin( );
		if(it != vbs.end( )){
			return  (*it)->num;
		}
		return 0;
	}
	//----------------------------------------------------------------------
	//	??????õÏ??ø²????ùD?øŒ?????öwöm?
	bool  VB_Container::PushBack(BYTE da_[], UINT str_, UINT n_)
	{
		DG::VertexBuffer::SP  w = DG::VertexBuffer::Create(da_, str_, n_);
		if(!w){	return false;	}
		vbs.push_back( w );
		return true;
	}
	//----------------------------------------------------------------------
	//	ô™økùÖ???????????????
	VertexBuffer::SP  VB_Container::GetVB(UINT  n_)
	{
		return  vbs[n_];
	}
}
