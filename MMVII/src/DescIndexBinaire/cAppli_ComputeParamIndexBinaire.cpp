#include "include/MMVII_all.h"
#include "IndexBinaire.h"
#include "include/MMVII_Tpl_Images.h"

// #include "include/MMVII_2Include_Serial_Tpl.h"
// #include<map>

/** \file cCalcul_IndexBinaire.cpp
    \brief Command for computing the parameters of 

*/


namespace MMVII
{

/* ==================================================== */
/*                                                      */
/*           cAppli_ComputeParamIndexBinaire            */
/*                                                      */
/* ==================================================== */

/**
    Application for computing optimized binary indexes
*/




cCollecSpecArg2007 & cAppli_ComputeParamIndexBinaire::ArgObl(cCollecSpecArg2007 & anArgObl)
{
   return 
      anArgObl  
          <<   Arg2007(mDirGlob,"Directory where data are loaded",{})
   ;
}

cCollecSpecArg2007 & cAppli_ComputeParamIndexBinaire::ArgOpt(cCollecSpecArg2007 & anArgOpt)
{
   return 
      anArgOpt
         << AOpt2007(mPatPCar,"PatPCar","Pattern for P Carac for ex \"eLap.*\"",{})
         << AOpt2007(mPatInvRad,"PatIR","Pattern of radial invariants",{eTA2007::HDV})
         << AOpt2007(mPropFile,"PropF","Prop of File selected (tuning, faster in test)",{})
         << AOpt2007(mNbIterBits,"NIBC","Number of iteration for bits computation",{eTA2007::HDV})
         << AOpt2007(mNbEigenVal,"NbEiV","Number of Eigen Val",{eTA2007::HDV})
         << AOpt2007(mNbTestCombInit,"NbComb0","Number of test on initial combinaison",{eTA2007::HDV})
         << AOpt2007(mNbOptCombLoc,"NbCombLoc","Number of test for local combinatory opt",{eTA2007::HDV})
         << AOpt2007(mQuickBits,"QB","Set all parameter of opt to low def value (for fast tuning)",{eTA2007::HDV})
         << AOpt2007(mZoomImg,"ZoomImg","Zoom of decimation of images",{eTA2007::HDV})
         << AOpt2007(mSaveFileSelVectIR,"SaveFSVIR","File to save selected radial inv",{})
   ;
}


cAppli_ComputeParamIndexBinaire::~cAppli_ComputeParamIndexBinaire()
{
}

cAppli_ComputeParamIndexBinaire::cAppli_ComputeParamIndexBinaire
(
       const std::vector<std::string> & aVArgs,
       const cSpecMMVII_Appli & aSpec
) :
  cMMVII_Appli (aVArgs,aSpec),
  mPatPCar     ("eTPR_LaplMax"),
  mPatInvRad   ("eTVIR_ACG."),
  mNbPixTot    (0.0),
  mPropFile    (1.0),
  mNbIterBits  (3),
  mNbEigenVal  (100),
  mNbTestCombInit (500),
  mNbOptCombLoc  (800),
  mQuickBits     (false),
  mZoomImg       (1),
  mNbValByP    (0),
  mTmpVect     (1),    // need to initialize, but do not know the size
  mStat2       (1),     // idem
  mLSQOpt      (1),     // and again 
  mEigen       (nullptr),
  mSaveFileSelVectIR    ("")
{
}

const std::string & cAppli_ComputeParamIndexBinaire::DirCurPC() const  {return mDirCurPC;}
double  cAppli_ComputeParamIndexBinaire::PropFile() const {return mPropFile;}
cVecInvRad*  cAppli_ComputeParamIndexBinaire::IR(int aK) { return  mVIR.at(aK).get(); }
cDenseVect<tREAL8> &    cAppli_ComputeParamIndexBinaire::TmpVect() {return mTmpVect;}
const cStrStat2<tREAL8> & cAppli_ComputeParamIndexBinaire::Stat2() { return mStat2;}
int cAppli_ComputeParamIndexBinaire::ZoomImg() const {return mZoomImg;}

const cResulSymEigenValue<tREAL8> &  cAppli_ComputeParamIndexBinaire::Eigen() const
{
   MMVII_INTERNAL_ASSERT_medium(mEigen!=nullptr,"cAppli_ComputeParamIndexBinaire::Eigen()");
   return *mEigen;
}


int cAppli_ComputeParamIndexBinaire::Exe()
{
   // If we want just a quick execution of bit vector for tuning, set non specified 
   // parameters to value diff from defaults
   if (mQuickBits)
   {
       if (! IsInit(&mPropFile)) mPropFile = 0.1;
       if (! IsInit(&mNbIterBits)) mNbIterBits = 1;
       if (! IsInit(&mNbEigenVal)) mNbEigenVal = 50;
       if (! IsInit(&mNbTestCombInit)) mNbTestCombInit = 100;
       if (! IsInit(&mNbOptCombLoc)) mNbOptCombLoc = 200;
       if (! IsInit(&mZoomImg)) mZoomImg = 2;
   }
   // Transform inout pattern into a vector  of enum radial  invariant
   mVecTyIR = SubOfPat<eTyInvRad>(mPatInvRad,false);
  // Extract list of folder of  Pts carac corresponding to input pattern
   mLDirPC = GetSubDirFromDir(mDirGlob,BoostAllocRegex(mPatPCar));


   // If only one p carac process it
   if (mLDirPC.size() ==1)
   {
       ProcessOneDir(mLDirPC[0]);
   }
   // If multiple p carac, exec each one in parallel using mecanism of auto recal
   else if (mLDirPC.size() > 1)
   {
       ExeMultiAutoRecallMMVII 
       (
              "PatPCar",  // Arg that must be modified/specified on command
              mLDirPC,    // List of value that must be subsitued in PatPCar=...
              cColStrAOpt(),  // Possible supplementary modification (here none)
              eTyModeRecall::eTMR_Parall  // Do it in parallel (when it will be implemanted)
       );
   }
   else
   {
       //  here mLDirPC.size()==0 ,  probably user did not want "no Pts Carac"
       MMVII_UsersErrror(eTyUEr::eEmptyPattern,"No directory of Pt carac found");
   }

   return EXIT_SUCCESS;
}


void  cAppli_ComputeParamIndexBinaire::ProcessOneDir(const std::string & aDir)
{
    StdOut() << "================== " << mPatPCar << " ===========\n";
    mDirCurPC = mDirGlob + aDir + DirSeparator(); // Compute full name of folder for data

    cDataOneInvRad * aLast = nullptr;
    // For all type of invariant create a structure from the folder
    for (int aK=0 ; aK<int(mVecTyIR.size()) ; aK++)
    {
       // Creation will read all the files contained in the folder
       cDataOneInvRad * aNew = new cDataOneInvRad (*this,aLast,mVecTyIR[aK]);
       // mVecDIR.push_back(std::unique_ptr<cDataOneInvRad> (new cDataOneInvRad (*this,mVecTyIR[aK])));
       mVecDIR.push_back(std::unique_ptr<cDataOneInvRad> (aNew));
       mNbPixTot += mVecDIR.back()->NbPixTot();
       mNbValByP += mVecDIR.back()->NbValByP();
       aLast = aNew;
    }

    // Check coher of any relatively to the first one
    for (int aK=1 ; aK<int(mVecDIR.size()) ; aK++)
    {
       mVecDIR[0]->CheckCoherence(*(mVecDIR[aK]));
    }

    // Allocate the vector for car point
    mNbPts = mVecDIR[0]->NbPatch();
    mVIR.reserve(mNbPts);
    for (int aK=0 ; aK<mNbPts ; aK++)
    {
       mVIR.push_back(tPtVIR(new cVecInvRad(mNbValByP)));
    }


    StdOut() 
           << " NbValByP= " << mNbValByP 
           << " NbPts= " << mNbPts 
           << " NbPixTot= " << mNbPixTot 
           << "\n";

   // Fill the vect with data from files
   for (auto & aDIR : mVecDIR)
   {
       aDIR->AddPCar();
   }

   // In index binaire mode
   ComputeIndexBinaire();
}

cIm2D<tU_INT1> cAppli_ComputeParamIndexBinaire::MakeImSz(cIm2D<tU_INT1> aImg)
{
   if (mZoomImg != 1)
      return aImg.GaussDeZoom(mZoomImg);
   return aImg;
}


void  cAppli_ComputeParamIndexBinaire::ComputeIndexBinaire()
{
  // Number of selected eigen val cannot be over nuber of value in one patch
   mNbEigenVal = std::min(mNbValByP,mNbEigenVal);
   for (int aKIter=0 ; aKIter<mNbIterBits; aKIter++)
       OneIterComputeIndexBinaire();
}

void  cAppli_ComputeParamIndexBinaire::OneIterComputeIndexBinaire()
{
    // Compute pairs of true and false tieP
    {
        mVecTrueP.clear();
        mVecFalseP.clear();

        for (int aK=0 ; aK<mNbPts ; aK++)
        {
            // corresponding samples are store 2N, 2N+1 , they are true pair
            if (     ((aK%2)==0) 
                  && (mVIR[aK]->mSelected)
               )
               mVecTrueP.push_back(cPt2di(aK,aK+1));

            mVecFalseP.push_back(cPt2di(aK,RandUnif_N(mNbPts)));
            mVecFalseP.push_back(cPt2di(aK,RandUnif_N(mNbPts)));
        }
    }

    mVVBool.clear();

    // Allocate data for stats
    mTmpVect = cDenseVect<tREAL8>(mNbValByP);
    mStat2 = cStrStat2<tREAL8>(mNbValByP);
    mLSQOpt =  cLeasSqtAA<tREAL8>(mNbValByP);

    // Fill the stats
    int aPerAff = 10000;
    for (int aK=0 ; aK< mNbPts ;aK++)
    {
        if ((aK+1)%aPerAff ==0)
        {
            StdOut() << "Fill cov, Remain " << (mNbPts-aK) << " on " << mNbPts << "\n";
        }
        if ( mVIR[aK]->mSelected)
        {
            CopyIn(mTmpVect.DIm(),mVIR.at(aK)->mVec.DIm());
            mStat2.Add(mTmpVect);
        }
    }
    mStat2.Normalise();
    mEigen = &mStat2.DoEigen();


    // Create bit vector corresponding to mNbEigenVal highest eigen value (they are sorted)
    for (int aK=mNbValByP-1 ; aK>=mNbValByP-mNbEigenVal; aK--)
    {
        StdOut() << "EV["<< aK<< "]=" <<  mEigen->EigenValues()(aK) << "\n";
        mVVBool.push_back(tPtVBool ( new cVecBool (new cIB_LinearFoncBool(*this,aK,0), mVIR)));
    }

    TestRandom();

    int mTestSelBit = 3;
    int aNbOk =0;
    for (int aK=0 ; aK<mNbPts ; aK+=2)
    {
        if (NbbBitDif(mBestVB,cPt2di(aK,aK+1))<=mTestSelBit)
        {
            mVIR.at(aK  )->mSelected = false;
            mVIR.at(aK+1)->mSelected = false;
        }
        if (!mVIR.at(aK  )->mSelected)
           aNbOk += 2;
    }
    StdOut() << "Prop Deteteced " << aNbOk / double(mNbPts) << "\n";

    // TestNbBit();
    // SaveFileData();
}

/*
void cAppli_ComputeParamIndexBinaire::SaveFileData()
{
}
*/

std::vector<const cVecBool*> cAppli_ComputeParamIndexBinaire::IndexToVB(const std::vector<int>& aSet) const
{
   std::vector<const cVecBool*> aRes;
   for (const auto & aI : aSet)
       aRes.push_back(mVVBool.at(aI).get());
   return aRes;
}

double  cAppli_ComputeParamIndexBinaire::ScoreAPrioiri(const std::vector<int>& aSet) const
{
    double aSom = 0;
    for (const auto & aI  : aSet)
        aSom += 1/(2.0 - mVVBool.at(aI)->Score()); // Formula due the fact that 2 is the best exepcable score
    return aSom;
}


std::vector<const cVecBool*> 
    cAppli_ComputeParamIndexBinaire::GenereVB(int aNbTest,std::vector<int> & aSet,int aK,int aNb) const
{
   double aBestSc = -1;
   for (int aKTest=0 ; aKTest<aNbTest ; aKTest++)
   {
      std::vector<int>  aTestSet = RandSet(aK,aNb);
      double aScore = ScoreAPrioiri(aTestSet);
      if (aScore > aBestSc)
      {
          aBestSc = aScore;
          aSet = aTestSet;
      }
   }
   return IndexToVB(aSet);
}

void cAppli_ComputeParamIndexBinaire::TestNewSol
     (
           const  std::vector<int> & aVI,
           const  std::vector<const cVecBool*> &aNewVB
     )
{
   cStatDifBits aStTrue(mVecTrueP,aNewVB);
   cStatDifBits aStFalse(mVecFalseP,aNewVB);

   int aKMax;
   double aSc = aStTrue.Score(aStFalse,100.0,aKMax);
   if (aSc>mBestSc)
   {
       mBestSc = aSc;
       mBestVB    = aNewVB;
       mBestIndex = aVI;

       StdOut() << "========== Score : " << aSc  << "\n";
       aStTrue.Show(aStFalse,0,aKMax+3);
   }
}


void cAppli_ComputeParamIndexBinaire::TestRandom()
{
   mBestSc=-1e10;
   int aNbTot  = int(mVVBool.size());
   int aCpt=0;
   int aNbVecBit=24;
   double aT0= SecFromT0();
   while (aCpt < mNbTestCombInit)
   {
        std::vector<int> aNewIndex;
        std::vector<const cVecBool*> aNewVB = GenereVB(1<< (aCpt%8) ,aNewIndex,aNbVecBit,mVVBool.size());
        // First test, just select  the K Best
        if (aCpt==0)
        {
           std::vector<cPt2dr> aVP;
           for (int aK=0 ; aK<aNbTot ; aK++)
               aVP.push_back(cPt2dr(aK,-mVVBool.at(aK)->Score()));
           std::sort(aVP.begin(),aVP.end(),CmpCoord<double,2,1>);
           aNewIndex.clear();
           for (int aK=0 ; aK<aNbVecBit ; aK++)
           {
               aNewIndex.push_back(round_ni(aVP[aK].x()));
           }
           aNewVB = IndexToVB(aNewIndex);
        }

        TestNewSol(aNewIndex,aNewVB);
        aCpt++;
        if (aCpt%10==0) 
        {
           MMVII::StdOut() << "Random Init " << aCpt 
                           << " AverTim=" << (SecFromT0()-aT0)/aCpt 
                           << "\n";
        }
   }

   for (int aKC=0 ; aKC< mNbOptCombLoc ; aKC++)
   {
        if ((aKC%10)==0) 
            StdOut() << "Chnage cphase " << mNbOptCombLoc - aKC  << "\n";

        // int aPer = 7;
        // int aDelta = mVVBool.size() -aNbVecBit;
        // int aNbMax = aNbVecBit + (aDelta * Square(2+aKC%aPer)) / Square(aPer+1);
        
        for (int aNbC=1 ; aNbC<=4 ; aNbC++)
        {
             int aNbIterLoc = 1 << (aNbC + (aKC%4));
             std::vector<int>  aNewIndex = RandNeighSet(aNbC,mVVBool.size(),mBestIndex);
             double aBestSc =  ScoreAPrioiri(aNewIndex);

             for (int aKIter=0 ; aKIter<aNbIterLoc ; aKIter++)
             {
                  std::vector<int>  aTestIndex = RandNeighSet(aNbC,mVVBool.size(),mBestIndex);
                  double aScore =  ScoreAPrioiri(aTestIndex);
                  if (aScore>aBestSc)
                  {
                     aBestSc = aScore;
                     aNewIndex = aTestIndex;
                  }
             }

             std::vector<const cVecBool*>  aNewVB = IndexToVB(aNewIndex);
             TestNewSol(aNewIndex,aNewVB);
        }
   }
}


void cAppli_ComputeParamIndexBinaire::TestNbBit() const
{
    for (int aNb=16 ; aNb<=24 ; aNb+=2)
    {
       TestNbBit(aNb);
    }

    for (int aNb=30 ; aNb<=100 ; aNb+=10)
       TestNbBit(aNb);
}

void cAppli_ComputeParamIndexBinaire::TestNbBit(int aNb) const
{
    std::vector<int>             aIndexRes;
    std::vector<const cVecBool*> aVVB = GenereVB(1,aIndexRes,aNb,aNb);
/*
    std::vector<const cVecBool*> aVVB ;
    for (int aK=0 ; aK<aNb ; aK++)
    {
         aVVB.push_back(mVVBool.at(aK).get());
    }
*/
    cStatDifBits aStTrue(mVecTrueP,aVVB);
    cStatDifBits aStFalse(mVecFalseP,aVVB);

    MMVII::StdOut() << "================= " << aNb << "==============\n";
    for (int aK =0 ; aK <= aNb ; aK++)
        MMVII::StdOut() << aK << " " << aStTrue.mStatRCum[aK] << " " << aStFalse.mStatRCum[aK]<< "\n";
}



     // =====================

tMMVII_UnikPApli Alloc_ComputeParamIndexBinaire(const std::vector<std::string> & aVArgs,const cSpecMMVII_Appli & aSpec)
{
   return tMMVII_UnikPApli(new cAppli_ComputeParamIndexBinaire(aVArgs,aSpec));
}

cSpecMMVII_Appli  TheSpec_ComputeParamIndexBinaire
(
     "CompPIB",
      Alloc_ComputeParamIndexBinaire,
      "This command is used compute Parameter of Binary Index",
      {eApF::TieP},
      {eApDT::FileSys},
      {eApDT::Xml,eApDT::Console},
      __FILE__
);

};

