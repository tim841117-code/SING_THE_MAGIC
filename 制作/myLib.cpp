#include "myLib.h"
#include "DG2014/DG2014_Ver3_6.h"

namespace ML
{
	//˜Œ??¯k˜Œ?????
	void  ValueControl_ToValue(float&  v_,float  to_,  float  aja_)
	{
		if(v_  ==  to_){  return;}
		else{
			if(v_  >=  to_){	v_  =  max(v_ - aja_, to_);}
			else{				v_  =  min(v_ - aja_, to_);}
		}
		
	}
	//???????ˆm??˜˜???¯k˜Œ?????
	void  VectorControl_XZ_ToValue(Vec3&  v_,float  to_,  float  aja_)
	{
		Vec3  xz(v_.x,  0,  v_.z);
		float  len  =  xz.Length( );

		if(len  ==  0){
			xz  =  ML::Vec3(0,0,0);
		}
		else{
			float  nLen  =  len;
			ValueControl_ToValue(nLen,  to_,  aja_);
			float  vr  =  nLen  /  len;	//˘ü?????
			xz  *=  vr;	//???˜˜???˜Úˆo??˜˜??????
		}
		v_.x  =  xz.x;
		v_.z  =  xz.z;
	}
	//?????˜˜???¯k˜Œ?˜Ù????ıÀ???¯k˜Œ??˜º???
	void  VectorControl_XZ_Cut(Vec3&  v_,float  to_)
	{
		Vec3  xz(v_.x,  0,  v_.z);
		float  len  =  xz.Length( );
		if(len  >  to_){
			float  vr  =  to_  /  len;	//˘ü?????
			xz  *=  vr;	//???˜˜???˜Úˆo??˜˜??????
			v_.x  =  xz.x;
			v_.z  =  xz.z;
		}
	}

	
	Vec2::Vec2( ):D3DXVECTOR2(0,0){	}
	Vec2::Vec2(const float  x_,const float  y_):D3DXVECTOR2(x_,y_){}
	Vec2::Vec2(const D3DXVECTOR2&  v_):D3DXVECTOR2(v_){}
	Vec2  Vec2::operator  = (const D3DXVECTOR2&  v_){ return  (*this)  =  Vec2(v_); }
	Vec2  Vec2::Normalize()const{ return  Vec2((*this) / this->Length()); }
	float  Vec2::Length()const{ return  D3DXVec2Length(this); }

	Vec3::Vec3( ):D3DXVECTOR3(0,0,0){	}
	Vec3::Vec3(const float  x_,const float  y_,const float  z_):D3DXVECTOR3(x_,y_,z_){}
	Vec3::Vec3(const D3DXVECTOR3&  v_):D3DXVECTOR3(v_){}
	Vec3  Vec3::operator  = (const D3DXVECTOR3&  v_){ return  (*this)  =  Vec3(v_); }
	class  Vec4  Vec3::Vec4(  ){	return  class  Vec4(this->x, this->y, this->z, 1);}
	Vec3  Vec3::Normalize()const{ return  (*this) / this->Length(); }
	float  Vec3::Length(  )const{		return  D3DXVec3Length(this);		}	

	Vec4::Vec4( ):D3DXVECTOR4(0,0,0,0){	}
	Vec4::Vec4(const float  x_,const float  y_,const float  z_,const float  w_):D3DXVECTOR4(x_,y_,z_,w_){}
	Vec4::Vec4(const D3DXVECTOR4&  v_):D3DXVECTOR4(v_){}
	Vec4  Vec4::operator  = (const D3DXVECTOR4&  v_){ return  (*this)  =  Vec4(v_); }
	class  Vec3  Vec4::Vec3(  ){	return  class  Vec3(this->x/this->w, this->y/this->w, this->z/this->w);}







	float Gravity(const  float  byM_)
	{
		//return  (9.8f * byM_) / 60.0f / 60.0f;
		const float  gra = 9.8f / 60 /60;
		return  gra * byM_;
	}

	float ToRadian(const  float  degree_)
	{
		return  degree_ * PI / 180.0f;
	}
	float ToDegree(const  float	radian_)
	{
		return  radian_ * 180.0f / PI;
	}

	int	CheckStrings(TCHAR* s_[], int n_, const string& c_)
	{
		int c;
		for(c=0; c < n_; ++c){
			if(c_ == s_[c]){return c;}
		}
		return -1;
	}
	map<string, bool>  msgboxlog;
	void  MsgBox(const string&  ms){
		if (msgboxlog[ms] == false){
			MessageBox(nullptr, ms.c_str(), nullptr, MB_OK);
			msgboxlog[ms] = true;
		}
	}


	//???????----------------------------------------------------------------
	void QT::RotAx(const  ML::Vec3&  v_,  float  r_){	D3DXQuaternionRotationAxis(this, &v_, r_);}
	void QT::RotX(float  rX_){					RotAx(ML::Vec3(1,0,0), rX_);}
	void QT::RotY(float  rY_){					RotAx(ML::Vec3(0,1,0), rY_);}
	void QT::RotZ(float  rZ_){					RotAx(ML::Vec3(0,0,1), rZ_);}
	QT::QT( ){									RotY(0);}
	QT::QT(float  rY_){							RotY(rY_);}
	QT::QT(const  ML::Vec3&  v_,  float  r_){	RotAx(v_, r_);}
	QT  QT::operator * ( const D3DXQUATERNION&  m_) const
	{
	    QT  rtv;
	    D3DXQuaternionMultiply(&rtv, this, &m_);
	    return rtv;
	}
	QT& QT::operator *= ( const D3DXQUATERNION&  m_ )
	{
	    D3DXQuaternionMultiply(this, this, &m_);
		return *this;
	}
	void QT::RotXYZ(const  ML::Vec3  rXYZ_)
	{
		QT  x, y, z;
		x.RotX(rXYZ_.x);
		y.RotY(rXYZ_.y);
		z.RotZ(rXYZ_.z);
		(*this)  =  x * y * z;
	}
	//??--------------------------------------------------------------------------
	Mat4x4::Mat4x4( ){	}
	Mat4x4::Mat4x4(const D3DXMATRIX&  v_):D3DXMATRIX(v_){	}

	Mat4x4  Mat4x4::operator  = (const D3DXMATRIX&  v_)
	{
		return  (*this)  =  Mat4x4(v_); 
	}
	//˜≤???
	void  Mat4x4::Identity( )
	{
		D3DXMatrixIdentity(this);
	}
	//?????
	void  Mat4x4::LookAtLH(const  Vec3& pos_,  const  Vec3&  target_,  const  Vec3&  up_ )
	{
		D3DXMatrixLookAtLH(	this, &pos_, &target_, &up_);
	}
	//ÙÀ???
	void  Mat4x4::PerspectiveFovLH(float  fov_,  float  aspect_,  float  nearPlane_,  float  forePlane_ )
	{
		D3DXMatrixPerspectiveFovLH(	this, fov_, aspect_, nearPlane_, forePlane_);
	}
	//?¯kÙ≤?¯†??
	void  Mat4x4::RotationX(float  r_)
	{
		D3DXMatrixRotationX(this, r_);
	}
	void  Mat4x4::RotationY(float  r_)
	{
		D3DXMatrixRotationY(this, r_);
	}
	void  Mat4x4::RotationZ(float  r_)
	{ 
		D3DXMatrixRotationZ(this, r_);
	}
	//˘f?Ù≤?¯†??
	void  Mat4x4::RotationAxis(const  Vec3&  v_, float  r_)
	{
		D3DXMatrixRotationAxis(this, &v_, r_); 
	}
	//?????????¯†??
	void  Mat4x4::RotationQuaternion(const  QT&  r_)
	{
		D3DXMatrixRotationQuaternion (this, &r_);
	}
	//???¯œ??
	void  Mat4x4::Translation(const  Vec3&  p_)
	{
		D3DXMatrixTranslation (this, p_.x, p_.y, p_.z);
	}
	//??????
	void  Mat4x4::Scaling(const  Vec3&  s_)
	{
		D3DXMatrixScaling (this, s_.x, s_.y, s_.z);
	}
	void  Mat4x4::Scaling(float  s_)
	{
		D3DXMatrixScaling (this, s_, s_, s_);
	}


	//???ˆwˆm
	Mat4x4  Mat4x4::Inverse( )
	{
		Mat4x4  w;
		D3DXMatrixInverse(&w, NULL, this);
		return  w;
	}
	//¯†˜◊??ˆwˆm
	Mat4x4  Mat4x4::Transpose( ) const
	{
		Mat4x4  w;
		D3DXMatrixTranspose(&w, this);
		return w;
	}
	//????
	Vec3  Mat4x4::TransformCoord(const Vec3&  p_)
	{
		Vec3  w;
		D3DXVec3TransformCoord(&w, &p_, this);
		return  w;
	}
	Vec3  Mat4x4::TransformNormal(const Vec3&  v_)
	{
		Vec3  w;
		D3DXVec3TransformNormal(&w, &v_, this);
		return  w;
	}
}

namespace ML
{
	namespace Collsion	//COLLISION
	{
		bool HitRayBB(
			const  ML::Vec3&  pos_, const  ML::Vec3&  dir_,
			const AABB&  bb_, float&  dist_
		)
		{
			//???Ùê¯¢?ıK¯¢??BB??¯¢??˘˜
			ML::Vec3  sPos = pos_;
			ML::Vec3  ePos = pos_ + dir_;
			float  sx = bb_.Center.x - bb_.Extents.x;
			float  ex = bb_.Center.x + bb_.Extents.x;
			float  sy = bb_.Center.y - bb_.Extents.y;
			float  ey = bb_.Center.y + bb_.Extents.y;
			float  sz = bb_.Center.z - bb_.Extents.z;
			float  ez = bb_.Center.z + bb_.Extents.z;

			//???Ùê¯¢?BB?????
			if (sx <= sPos.x && sPos.x <= ex && 
				sy <= sPos.y && sPos.y <= ey && 
				sz <= sPos.z && sPos.z <= ez) {
				dist_ = 0.0f;
				return true;
			}
			//ˆæ???????Ùê¯¢???BB???????

			//sx˜F????????????
			if (sPos.x < sx && sx < ePos.x) {//Ùê¯¢?sx????ıK¯¢?sx?ıƒ???????????
				float  toS = sx - sPos.x;//???????˜–¯¢???XÙ≤?????
				ML::Vec3  xnVec = dir_ / abs(dir_.x);//????˜˜??????????
				ML::Vec3  contactVec = (xnVec * abs(toS));//ˆúıÁ¯¢???????????
				ML::Vec3  contactPos = sPos + contactVec;//ˆúıÁ¯¢????
													 //YZ??ıƒ?¯¢????˘œ?˘D????˘›?
				if (sy <= contactPos.y && contactPos.y <= ey &&
					sz <= contactPos.z && contactPos.z <= ez) {
					dist_ = contactVec.Length();
					return true;
				}
			}
			//ex˜F????????????
			if (sPos.x > ex && ex > ePos.x) {
				float  toS = ex - sPos.x;
				ML::Vec3  xnVec = dir_ / abs(dir_.x);
				ML::Vec3  contactVec = (xnVec * abs(toS));
				ML::Vec3  contactPos = sPos + contactVec;
				if (sy <= contactPos.y && contactPos.y <= ey &&
					sz <= contactPos.z && contactPos.z <= ez) {
					dist_ = contactVec.Length();
					return true;
				}
			}
			//sy˜F????????????
			if (sPos.y < sy && sy < ePos.y) {
				float  toS = sy - sPos.y;
				ML::Vec3  ynVec = dir_ / abs(dir_.y);
				ML::Vec3  contactVec = (ynVec * abs(toS));
				ML::Vec3  contactPos = sPos + contactVec;
				if (sx <= contactPos.x && contactPos.x <= ex &&
					sz <= contactPos.z && contactPos.z <= ez) {
					dist_ = contactVec.Length();
					return true;
				}
			}
			//ey˜F????????????
			if (sPos.y > ey && ey > ePos.y) {
				float  toS = ey - sPos.y;
				ML::Vec3  ynVec = dir_ / abs(dir_.y);
				ML::Vec3  contactVec = (ynVec * abs(toS));
				ML::Vec3  contactPos = sPos + contactVec;
				if (sx <= contactPos.x && contactPos.x <= ex &&
					sz <= contactPos.z && contactPos.z <= ez) {
					dist_ = contactVec.Length();
					return true;
				}
			}
			//sz˜F????????????
			if (sPos.z < sz && sz < ePos.z) {
				float  toS = sz - sPos.z;
				ML::Vec3  znVec = dir_ / abs(dir_.z);
				ML::Vec3  contactVec = (znVec * abs(toS));
				ML::Vec3  contactPos = sPos + contactVec;
				if (sx <= contactPos.x && contactPos.x <= ex &&
					sy <= contactPos.y && contactPos.y <= ey) {
					dist_ = contactVec.Length();
					return true;
				}
			}
			//ez˜F????????????
			if (sPos.z > ez && ez > ePos.z) {
				float  toS = ez - sPos.z;
				ML::Vec3  znVec = dir_ / abs(dir_.z);
				ML::Vec3  contactVec = (znVec * abs(toS));
				ML::Vec3  contactPos = sPos + contactVec;
				if (sx <= contactPos.x && contactPos.x <= ex &&
					sy <= contactPos.y && contactPos.y <= ey) {
					dist_ = contactVec.Length();
					return true;
				}
			}
			return false;
		}

		//????????ˆúıÁ˘Ω¯k
		inline bool HitRayTriangle(
			const  ML::Vec3&  p_,  const  ML::Vec3&  d_,
			const  ML::Vec3&  v0_, const  ML::Vec3&  v1_, const  ML::Vec3&  v2_,
			float&  dist_)
		{
			{
				dist_ = 0;
				//??ˆæ
				ML::Vec3	t0 = v1_ - v0_;
				ML::Vec3	t1 = v2_ - v0_;
				ML::Vec3	nm;
				D3DXVec3Cross(&nm, &t0, &t1);
				nm = nm.Normalize();
				//ˆæ?˘Ω¯k(??????ıƒ????????
				ML::Vec3	xp = v0_ - p_;
				float   xpn = D3DXVec3Dot(&xp, &nm);
				float   vn = D3DXVec3Dot(&d_, &nm);
				if (-0.00001f <= vn) { return  false; }//?????????????
				float  t = xpn / vn;
				if (t < 0) { return  false; }//Ùê¯¢???????˜F???
				//????˘D????˘Dˆë?0?ıƒ??????˘D??
				ML::Vec3  tp = p_ + t * d_;
				{
					ML::Vec3  d0 = tp - v1_;
					ML::Vec3  d1 = v0_ - v1_;
					ML::Vec3  c;
					D3DXVec3Cross(&c, &d0, &d1);
					if (D3DXVec3Dot(&c, &nm) < 0) { return  false; }
				}
				{
					ML::Vec3  d0 = tp - v2_;
					ML::Vec3  d1 = v1_ - v2_;
					ML::Vec3  c;
					D3DXVec3Cross(&c, &d0, &d1);
					if (D3DXVec3Dot(&c, &nm) < 0) { return  false; }
				}
				{
					ML::Vec3  d0 = tp - v0_;
					ML::Vec3  d1 = v2_ - v0_;
					ML::Vec3  c;
					D3DXVec3Cross(&c, &d0, &d1);
					if (D3DXVec3Dot(&c, &nm) < 0) { return  false; }
				}
				dist_ = t;
				return  true;
			}
		}

		//??????????????ˆúıÁ˘Ω¯k?????????????˜D????
		//˜¯¯¢????????????ˆm?????????????˘Ä???
		bool HitRayTriangleA(
			const  ML::Vec3&  p_, 
			const  ML::Vec3&  d_,
			shared_ptr<DG::VertexBuffer> vb_,
			const  UINT pos_,
			float&  dist_)
		{
			BYTE  *p = (BYTE*)vb_->memory;
			p += pos_;
			UINT  st = vb_->stride;
			bool   hit = false;
			for (UINT n = 0; n < vb_->num; n += 3) {
				ML::Vec3	*p0, *p1, *p2;
				p0 = (ML::Vec3*)&p[0];
				p1 = (ML::Vec3*)&p[st];
				p2 = (ML::Vec3*)&p[st * 2];
				float  distW = 0;

				bool  rtv = HitRayTriangle(p_, d_, *p0, *p1, *p2, distW);
				if (rtv == true) {
					if (hit == false) {
						dist_ = distW;
					}
					else if (dist_ > distW) {
						dist_ = distW;
					}
					hit = true;
				}
				p += st * 3;
			}
			return  hit;
		}
		bool HitRayTriangleQ(
			const  ML::Vec3&  p_, const  ML::Vec3&  d_,
			shared_ptr<DG::VertexBuffer> vb_,
			const  UINT pos_,
			float&  dist_)
		{
			BYTE  *p = (BYTE*)vb_->memory;
			p += pos_;
			UINT  st = vb_->stride;
			bool   hit = false;
			for (UINT n = 0; n < vb_->num; n += 3) {
				ML::Vec3	*p0, *p1, *p2;
				p0 = (ML::Vec3*)&p[0];
				p1 = (ML::Vec3*)&p[st];
				p2 = (ML::Vec3*)&p[st * 2];
				float  distW = 0;

				bool  rtv = HitRayTriangle(p_, d_, *p0, *p1, *p2, distW);
				if (rtv == true) {
					if (hit == false) {
						dist_ = distW;
					}
					else if (dist_ > distW) {
						dist_ = distW;
					}
					hit = true;
					break;
				}
				p += st * 3;
			}
			return  hit;
		}
		bool HitRayTriangle(
			const  ML::Vec3&  p_, const  ML::Vec3&  d_,
			bool  qmode_,
			shared_ptr<DG::VertexBuffer>  vb_,
			const  UINT pos_,
			float&  dist_)
		{
			if (qmode_) { return  HitRayTriangleQ(p_, d_, vb_, pos_, dist_); }
			else { return  HitRayTriangleA(p_, d_, vb_, pos_, dist_); }
		}

		template<class  T>
		bool HitRayTriangleIBVBQ(
			const  ML::Vec3&  p_, const  ML::Vec3&  d_,
			shared_ptr< DG::VertexBuffer>	vb_,
			const  UINT pos_,
			shared_ptr<DG::IndexBuffer>		ib_,
			float&  dist_)
		{
			T  *idx = (T*)ib_->memory;
			BYTE  *p = (BYTE*)vb_->memory;
			p += pos_;
			UINT  st = vb_->stride;
			bool   hit = false;
			for (UINT n = 0; n < ib_->num; n += 3) {
				ML::Vec3	*p0, *p1, *p2;
				p0 = (ML::Vec3*)&p[idx[0] * st];
				p1 = (ML::Vec3*)&p[idx[1] * st];
				p2 = (ML::Vec3*)&p[idx[2] * st];
				float  distW = 0;

				bool  rtv = HitRayTriangle(p_, d_, *p0, *p1, *p2, distW);
				if (rtv == true) {
					if (hit == false) {
						dist_ = distW;
					}
					else if (dist_ > distW) {
						dist_ = distW;
					}
					hit = true;
					break;
				}
				idx += 3;
			}
			return  hit;
		}
		template<class  T>
		bool HitRayTriangleIBVBA(
			const  ML::Vec3&  p_, const  ML::Vec3&  d_,
			shared_ptr< DG::VertexBuffer>	vb_,
			const  UINT pos_,
			shared_ptr<DG::IndexBuffer>		ib_,
			float&  dist_)
		{
			T  *idx = (T*)ib_->memory;
			BYTE  *p = (BYTE*)vb_->memory;
			p += pos_;
			UINT  st = vb_->stride;
			bool   hit = false;
			for (UINT n = 0; n < ib_->num; n += 3) {
				ML::Vec3	*p0, *p1, *p2;
				p0 = (ML::Vec3*)&p[idx[0] * st];
				p1 = (ML::Vec3*)&p[idx[1] * st];
				p2 = (ML::Vec3*)&p[idx[2] * st];
				float  distW = 0;

				bool  rtv = HitRayTriangle(p_, d_, *p0, *p1, *p2, distW);
				if (rtv == true) {
					if (hit == false) {
						dist_ = distW;
					}
					else if (dist_ > distW) {
						dist_ = distW;
					}
					hit = true;
				}
				idx += 3;
			}
			return  hit;
		}
		bool HitRayTriangle(const  ML::Vec3&  p_, const  ML::Vec3&  d_, bool  qmode_,
			shared_ptr< DG::VertexBuffer>		vb_, const  UINT pos_,
			shared_ptr<DG::IndexBuffer>		ib_,
			float&  dist_)
		{
			if (ib_->stride == 2) {
				if (qmode_) { return  HitRayTriangleIBVBQ<WORD>(p_, d_, vb_, pos_, ib_, dist_); }
				else { return  HitRayTriangleIBVBA<WORD>(p_, d_, vb_, pos_, ib_, dist_); }
			}
			else if (ib_->stride == 4) {
				if (qmode_) { return  HitRayTriangleIBVBQ<DWORD>(p_, d_, vb_, pos_, ib_, dist_); }
				else { return  HitRayTriangleIBVBA<DWORD>(p_, d_, vb_, pos_, ib_, dist_); }
			}
			return false;
		}

	}
}