/*==============================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEDummy.h,v $
Language:  C++
Date:      $Date: 2011-05-25 11:58:32 $
Version:   $Revision: 1.1.2.2 $
Authors:   Matteo Giacomoni
================================================================================
Copyright (c) 2009 Cineca, UK (www.cineca.it)
All rights reserved.
===============================================================================*/

#include "mafDefines.h" 
//------------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//------------------------------------------------------------------------------

#include "mafVME.h"

//----------------------------------------------------------------------------
class mafVMEDummy : public mafVME
//----------------------------------------------------------------------------
{
public:
  mafVMEDummy(){};
  ~mafVMEDummy(){};

  mafTypeMacro(mafVMEDummy,mafVME);

  /*virtual*/ void SetMatrix(const mafMatrix &mat){};
  /*virtual*/ void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes){};

protected:
private:
};

mafCxxTypeMacro(mafVMEDummy);