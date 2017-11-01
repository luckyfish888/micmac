#include "include/MMVII_all.h"

namespace MMVII
{

/**
    This file contains the implemenation of conversion between strings and 
   atomic object
*/

static char BufStrIO[1000];

   // ================  bool ==============================================

template <>  std::string cStrIO<bool>::ToStr(const bool & anI)
{
   return  anI ? "true" : "false";
}
template <>  bool cStrIO<bool>::FromStr(const std::string & aStr)
{
    if ((aStr=="1") || UCaseEqual(aStr,"true")) return true;
    if ((aStr=="0") || UCaseEqual(aStr,"false")) return false;

    MMVII_INTERNAL_ASSERT_user(false,"Bad value for boolean :["+aStr+"]");

    return false;
}

   // ================  int ==============================================

template <>  std::string cStrIO<int>::ToStr(const int & anI)
{
   sprintf(BufStrIO,"%d",anI);
   return BufStrIO;
}
template <>  int cStrIO<int>::FromStr(const std::string & aStr)
{
    int anI;
    sscanf(aStr.c_str(),"%d",&anI);
    return anI;
}

   // ================  double ==============================================

template <>  std::string cStrIO<double>::ToStr(const double & anI)
{
   sprintf(BufStrIO,"%lf",anI);
   return BufStrIO;
}
template <>  double cStrIO<double>::FromStr(const std::string & aStr)
{
    double anI;
    sscanf(aStr.c_str(),"%lf",&anI);
    return anI;
}

   // ================  std::string ==============================================

template <>  std::string cStrIO<std::string>::ToStr(const std::string & aStr)
{
   return aStr;
}
template <>  std::string cStrIO<std::string>::FromStr(const std::string & aStr)
{
    return aStr;
}


};