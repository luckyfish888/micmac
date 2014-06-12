// File Automatically generated by eLiSe
#ifndef _cB_GCF1_cpp_
#define _cB_GCF1_cpp_

#include "general/all.h"
#include "private/all.h"
#include "cB_GCF1.h"


cB_GCF1::cB_GCF1():
    cElCompiledFonc()
{
   AddIntRef (cIncIntervale("Interv1",0,2));
   AddIntRef (cIncIntervale("Interv2",2,5));
   Close(false);
}



void cB_GCF1::ComputeVal()
{
   double tmp0_ = mCompCoord[4];

  mVal = tan(cos(tmp0_)/(1+ElSquare(sin(mLocB*tmp0_))))+mLocA;

}


void cB_GCF1::ComputeValDeriv()
{
   double tmp0_ = mCompCoord[4];
   double tmp1_ = mLocB*tmp0_;
   double tmp2_ = sin(tmp1_);
   double tmp3_ = ElSquare(tmp2_);
   double tmp4_ = 1+tmp3_;
   double tmp5_ = cos(tmp0_);
   double tmp6_ = tmp5_/(tmp4_);
   double tmp7_ = tan(tmp6_);

  mVal = tmp7_+mLocA;

  mCompDer[0] = 0;
  mCompDer[1] = 0;
  mCompDer[2] = 0;
  mCompDer[3] = 0;
  mCompDer[4] = ((-(1)*sin(tmp0_)*(tmp4_)-tmp5_*2*mLocB*cos(tmp1_)*tmp2_)/ElSquare(tmp4_))*(1+ElSquare(tmp7_));
}


void cB_GCF1::ComputeValDerivHessian()
{
   double tmp0_ = mCompCoord[4];
   double tmp1_ = mLocB*tmp0_;
   double tmp2_ = sin(tmp1_);
   double tmp3_ = ElSquare(tmp2_);
   double tmp4_ = 1+tmp3_;
   double tmp5_ = cos(tmp0_);
   double tmp6_ = tmp5_/(tmp4_);
   double tmp7_ = tan(tmp6_);
   double tmp8_ = -(1);
   double tmp9_ = cos(tmp1_);
   double tmp10_ = mLocB*tmp9_;
   double tmp11_ = 2*tmp10_;
   double tmp12_ = tmp11_*tmp2_;
   double tmp13_ = sin(tmp0_);
   double tmp14_ = tmp8_*tmp13_;
   double tmp15_ = ElSquare(tmp4_);
   double tmp16_ = tmp14_*(tmp4_);
   double tmp17_ = tmp5_*tmp12_;
   double tmp18_ = tmp16_-tmp17_;
   double tmp19_ = ElSquare(tmp7_);
   double tmp20_ = 1+tmp19_;
   double tmp21_ = (tmp18_)/tmp15_;
   double tmp22_ = (tmp21_)*(tmp20_);

  mVal = tmp7_+mLocA;

  mCompDer[0] = 0;
  mCompDer[1] = 0;
  mCompDer[2] = 0;
  mCompDer[3] = 0;
  mCompDer[4] = tmp22_;
  mCompHessian[0][0]=   mCompHessian[0][0]= 0;
  mCompHessian[1][0]=   mCompHessian[0][1]= 0;
  mCompHessian[1][1]=   mCompHessian[1][1]= 0;
  mCompHessian[2][0]=   mCompHessian[0][2]= 0;
  mCompHessian[2][1]=   mCompHessian[1][2]= 0;
  mCompHessian[2][2]=   mCompHessian[2][2]= 0;
  mCompHessian[3][0]=   mCompHessian[0][3]= 0;
  mCompHessian[3][1]=   mCompHessian[1][3]= 0;
  mCompHessian[3][2]=   mCompHessian[2][3]= 0;
  mCompHessian[3][3]=   mCompHessian[3][3]= 0;
  mCompHessian[4][0]=   mCompHessian[0][4]= 0;
  mCompHessian[4][1]=   mCompHessian[1][4]= 0;
  mCompHessian[4][2]=   mCompHessian[2][4]= 0;
  mCompHessian[4][3]=   mCompHessian[3][4]= 0;
  mCompHessian[4][4]=   mCompHessian[4][4]= ((((tmp5_*tmp8_*(tmp4_)+tmp12_*tmp14_)-(tmp14_*tmp12_+(-(mLocB)*tmp2_*mLocB*2*tmp2_+tmp10_*tmp11_)*tmp5_))*tmp15_-(tmp18_)*2*tmp12_*(tmp4_))/ElSquare(tmp15_))*(tmp20_)+2*tmp22_*tmp7_*(tmp21_);
}



void cB_GCF1::SetA(double aVal){ mLocA = aVal;}
void cB_GCF1::SetB(double aVal){ mLocB = aVal;}



double * cB_GCF1::AdrVarLocFromString(const std::string & aName)
{
   if (aName == "A") return & mLocA;
   if (aName == "B") return & mLocB;
   return 0;
}


cElCompiledFonc::cAutoAddEntry cB_GCF1::mTheAuto("cB_GCF1",cB_GCF1::Alloc);


cElCompiledFonc *  cB_GCF1::Alloc()
{  return new cB_GCF1();
}


#endif // _cB_GCF1_cpp_
