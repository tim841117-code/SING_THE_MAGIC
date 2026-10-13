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


#include <memory>
#include <tchar.h>
#include <windows.h>
#include <string.h>
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map>
#include <unordered_map>
//////////////////////////////#include "atlstr.h"
using namespace std;
using namespace std::tr1;

namespace ML
{
	float Gravity(const  float  byM_);

	static const float  PI = 3.14159265358979323846f;
	float ToRadian(const  float	degree_);
	float ToDegree(const  float	radian_);

	//?????????????
	class Vec2 : public  D3DXVECTOR2
	{
	public:
		Vec2( );
		Vec2(const float  x_,const float  y_);
		Vec2(const D3DXVECTOR2&  v_);
		Vec2  operator  = (const D3DXVECTOR2&  v_);
		//?????öt??
		Vec2  Normalize(  )const;
		//?????÷÷?????
		float  Length()const;
	};
	class Vec3 : public  D3DXVECTOR3
	{
	public:
		Vec3( );
		Vec3(const float  x_,const float  y_,const float  z_);
		Vec3(const D3DXVECTOR3&  v_);
		Vec3  operator  = (const D3DXVECTOR3&  v_);
		//???????
		class  Vec4  Vec4(  );
		//?????öt??
		Vec3  Normalize()const;
		//?????÷÷?????
		float  Length()const;
	};
	class Vec4 : public  D3DXVECTOR4
	{
	public:
		Vec4( );
		Vec4(const float  x_,const float  y_,const float  z_,const float  w_);
		Vec4(const D3DXVECTOR4&  v_);
		Vec4  operator  = (const D3DXVECTOR4&  v_);
		//???????
		class  Vec3  Vec3(  );
	};
	//???????
	//????????(0,0,0,0)????÷Î????????
	class QT : public  D3DXQUATERNION
	{
	public:
		QT( );
		QT(float  rY_);
		QT(const  ML::Vec3&  v_,  float  r_);
		void RotAx(const  ML::Vec3&  v_,  float  r_);
		void RotX(float  rX_);
		void RotY(float  rY_);
		void RotZ(float  rZ_);
		void RotXYZ(const  ML::Vec3  rXYZ_);
		QT operator * ( const D3DXQUATERNION& ) const;
		QT& operator *= ( const D3DXQUATERNION& );
	};
	//??
	class Mat4x4 : public D3DXMATRIX
	{
	public:
		Mat4x4( );
		Mat4x4(const D3DXMATRIX&  v_);
		Mat4x4  operator  = (const D3DXMATRIX&  v_);
		//÷²???
		void  Identity( );
		//?????
		void  LookAtLH(const  Vec3& pos_,  const  Vec3&  target_,  const  Vec3&  up_ );
		//ôË???
		void  PerspectiveFovLH(float  fov_,  float  aspect_,  float  nearPlane_,  float  forePlane_ );
		//?økô²?ø ??
		void  RotationX(float  r_);
		void  RotationY(float  r_);
		void  RotationZ(float  r_);
		//ùf?ô²?ø ??
		void  RotationAxis(const  Vec3&  v_, float  r_);
		//?????????ø ??
		void  RotationQuaternion(const  QT&  r_);
		//???øÏ??
		void  Translation(const  Vec3&  p_);
		//??????
		void  Scaling(const  Vec3&  s_);
		void  Scaling(float  s_);

		//???öwöm
		Mat4x4 Inverse( );
		//ø ÷×??öwöm
		Mat4x4 Transpose( ) const;
		//????
		Vec3  TransformCoord(const Vec3&  p_);
		Vec3  TransformNormal(const Vec3&  v_);
	};

	//÷Î??øk÷Î?????
	void  ValueControl_ToValue(float&  v_,float  to_,  float  aja_);
	//???????öm??÷÷???øk÷Î?????
	void  VectorControl_XZ_ToValue(Vec3&  v_,float  to_,  float  aja_);
	//?????÷÷???øk÷Î?÷ô????õË???øk÷Î??÷¼???
	void  VectorControl_XZ_Cut(Vec3&  v_,float  to_);

	//õæ
	class Color
	{
	public:
		float  c[4];
		void Set(float  a_,float  r_,float  g_,float  b_){	c[3]=a_; c[2]=b_; c[1]=g_; c[0]=r_;}
		Color(float  a_,float  r_,float  g_,float  b_){		Set(a_, r_, g_, b_);}
		Color(float  c_[4]){								Set(c_[3], c_[2], c_[1], c_[0]);}
		Color( ){											Set(1, 0, 0, 0);}

		const float*  Ptr( ) const{return c;}
		D3DXCOLOR  D3D( ) const{	return  D3DXCOLOR(c[0], c[1], c[2], c[3]);} 
		bool  operator==(const  Color&  obj){
			return	   (c[0] == obj.c[0] &&
						c[1] == obj.c[1] &&
						c[2] == obj.c[2] &&
						c[3] == obj.c[3] );
		}
		bool  operator!=(const  Color&  obj){
			return	  !((*this) == obj );
		}
	};

	//?????????????????????????????????????????????
	//?????????????????????????ôæ?õq?							??
	//???????????????????õË??øÏ???õÂ???							??
	//???		?2010/01/15	ö^??ùy	?öm										??
	//?????????????????????????????????????????????
	static string GetPath(const string& fp_)
	{
		string s = fp_;
		string::size_type id;
		id = s.find_last_of("/");
		if(string::npos == id){	id = s.find_last_of("\\");}
		if(string::npos != id){	s = s.substr(0, id);}
		return s;
	}
	//base?øĞ????fn??öm
	static string CreateFilePath(const string&  base_, const string&  fn_)
	{
		string s = GetPath(base_);
		s += "/";
		s += fn_;
		return s;
	}

	//	?????Release???????????????
	//	Release()????ôÀ???ùD??delete?????????
	template<class T>
	static void SafeRelease(T** obj_)
	{
		if(*obj_ != nullptr)
		{
			(*obj_)->Release();
			(*obj_) = nullptr;
		}
	}
	template<class T>
	static void SafeDelete(T** obj_)
	{
		if(*obj_ != nullptr)
		{
			delete (*obj_);
			(*obj_) = nullptr;
		}
	}
	template<class T>
	static void SafeDeleteArray(T** obj_)
	{
		if(*obj_ != nullptr)
		{
			delete[] (*obj_);
			(*obj_) = nullptr;
		}
	}

	struct Point {
		int  x;
		int  y;
	};
	struct Rect {
		int  left;
		int  top;
		int  right;
		int  bottom;
	};


	class Box2D
	{
	public:
		int x, y, w, h;
		Box2D( )
		:x(0), y(0), w(0), h(0)
		{
		}
		Box2D(int x_, int y_, int w_, int h_)
		:x(x_), y(y_), w(w_), h(h_)
		{
		}
		Box2D(const Box2D& b_)
		:x(b_.x), y(b_.y), w(b_.w), h(b_.h)
		{
		}
		bool Hit(const  Box2D& b_) const
		{
			if( x		< b_.x + b_.w	&&
				b_.x	< x + w			&&
				y		< b_.y + b_.h	&&
				b_.y	< y + h			){
				return true;
			}
			return false;
		}
		bool Hit(const  Vec2& b_) const
		{
			if( x		< b_.x	&&
				b_.x	< x + w	&&
				y		< b_.y	&&
				b_.y	< y + h	){
				return true;
			}
			return false;
		}
		bool Hit(const  ML::Point& b_) const
		{
			if( x		< b_.x	&&
				b_.x	< x + w	&&
				y		< b_.y	&&
				b_.y	< y + h	){
				return true;
			}
			return false;
		}
		void Offset(const  Vec2&  v_)
		{
			x += (int)v_.x;
			y += (int)v_.y;
		}
		void Offset(float x_, float y_)
		{
			x += (int)x_;
			y += (int)y_;
		}
		void Offset(int x_, int y_)
		{
			x += x_;
			y += y_;
		}
		void Offset(ML::Point  p_)
		{
			x += p_.x;
			y += p_.y;
		}
		RECT Rect( ) const
		{
			RECT r = {x,y,x+w,y+h};
			return r;
		}
		Box2D OffsetCopy(int x_, int y_) const
		{
			Box2D r(*this);
			r.Offset(x_, y_);
			return r;
		}
		Box2D OffsetCopy(ML::Point p_) const
		{
			Box2D r(*this);
			r.Offset(p_);
			return r;
		}
		Box2D OffsetCopy(float x_, float y_) const
		{
			Box2D r(*this);
			r.Offset(x_, y_);
			return r;
		}
		Box2D OffsetCopy(const  Vec2&  v_) const
		{
			Box2D r(*this);
			r.Offset(v_);
			return r;
		}
	};


	class Box3D
	{
	public:
		int x, y, z, w, h, d;
		Box3D( )
		:x(0), y(0), z(0), w(0), h(0), d(0)
		{
		}
		Box3D(int x_, int y_, int z_, int w_, int h_, int d_)
		:x(x_), y(y_), z(z_), w(w_), h(h_), d(d_)
		{
		}
		Box3D(const Box3D&  b_)
		:x(b_.x), y(b_.y), z(b_.z), w(b_.w), h(b_.h), d(b_.d)
		{
		}
		bool Hit(const  Box3D&  b_) const
		{
			if( x		< b_.x + b_.w	&&
				b_.x	< x + w			&&
				y		< b_.y + b_.h	&&
				b_.y	< y + h			&&
				z		< b_.z + b_.d	&&
				b_.z	< z + d			){
				return true;
			}
			return false;
		}
		bool Hit(const  Vec3&  b_) const
		{
			if( x		< b_.x	&&
				b_.x	< x + w &&
				y		< b_.y	&&
				b_.y	< y + h	&&
				z		< b_.z	&&
				b_.z	< z + d	){
				return true;
			}
			return false;
		}
		void Offset(int x_, int y_, int z_){	x += x_;	y += y_;	z += z_;	}
		void Offset(float x_, float y_, float z_){	x += (int)x_;	y += (int)y_;	z += (int)z_;	}
		void Offset(const  Vec3&  v_){	x += (int)v_.x;	y += (int)v_.y;	z += (int)v_.z;	}
		Box3D OffsetCopy(int x_, int y_, int z_) const
		{
			Box3D r(*this);
			r.Offset(x_, y_, z_);
			return r;
		}
		Box3D OffsetCopy(float x_, float y_, float z_) const
		{
			Box3D r(*this);
			r.Offset(x_, y_, z_);
			return r;
		}
		Box3D OffsetCopy(const  Vec3&  v_) const
		{
			Box3D r(*this);
			r.Offset(v_);
			return r;
		}
	};
	int	CheckStrings(TCHAR* s_[], int n_, const string& c_);
	extern map<string, bool>  msgboxlog;
	void  MsgBox(const string&  ms);

}

namespace DG{
	class  VertexBuffer;
	class  IndexBuffer;
}

namespace ML
{
	namespace Collsion	//COLLISION
	{
		struct AABB {
			Vec3  Center;
			Vec3  Extents;
		};
		//????????öœõçù½øk
		bool HitRayTriangle(
			const  ML::Vec3&  p_,		//??ôø¢
			const  ML::Vec3&  d_,		//????????
			const  ML::Vec3&  v0_,		//????÷øø¢
			const  ML::Vec3&  v1_,		//????÷øø¢
			const  ML::Vec3&  v2_,		//????÷øø¢
			float&  dist_);			//öœõçø¢????????÷Îfalse??ô‰????


									//??????????????öœõçù½øk?????????????÷D????
		//÷øø¢????????????öm?????????????ù€???
		bool HitRayTriangle(
			const  ML::Vec3&  p_,						//??ôø¢
			const  ML::Vec3&  d_,						//????????
			bool  qmode_,								//????????öœõç????????????÷HõK??
			shared_ptr<DG::VertexBuffer>		vb_,	//??????????
			const  UINT pos_,							//÷øø¢õÏ?õÄ?÷øø¢????÷×??????
			float&  dist_);							//öœõçø¢????????÷Îfalse??ô‰????
	
													//??????????????öœõçù½øk
		//÷øø¢?????????õË?
		bool HitRayTriangle(
			const  ML::Vec3&  p_,						//??ôø¢
			const  ML::Vec3&  d_,						//????????
			bool  qmode_,								//????????öœõç????????????÷HõK??
			shared_ptr<DG::VertexBuffer>		vb_,	//??????????
			const  UINT pos_,							//÷øø¢õÏ?õÄ?÷øø¢????÷×??????
			shared_ptr<DG::IndexBuffer>			ib_,	//??????????
			float&  dist_);							//öœõçø¢????????÷Îfalse??ô‰????
		bool HitRayBB(
			const  ML::Vec3&  pos_, const  ML::Vec3&  dir_, 
			const AABB&  bb_,   float&  dist_
		);
	}
}