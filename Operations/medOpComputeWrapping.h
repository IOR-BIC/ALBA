/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpComputeWrapping.h,v $
  Language:  C++
  Date:      $Date: 2008-12-02 12:00:44 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Anupam Agrawal and Hui Wei
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpComputeWrapping_H__
#define __medOpComputeWrapping_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class medVMEComputeWrapping;
class mafGui;
class mafEvent;
//----------------------------------------------------------------------------
// medOpComputeWrapping :
//----------------------------------------------------------------------------
/** */
class medOpComputeWrapping: public mafOp
{
public:
  medOpComputeWrapping(const wxString &label = "CreateWrappedMeter");
  ~medOpComputeWrapping(); 

  mafTypeMacro(medOpComputeWrapping, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
  medVMEComputeWrapping *m_Meter;
};
#endif
