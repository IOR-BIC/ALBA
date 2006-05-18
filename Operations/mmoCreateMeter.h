/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateMeter.h,v $
  Language:  C++
  Date:      $Date: 2006-05-18 10:26:47 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoCreateMeter_H__
#define __mmoCreateMeter_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEMeter;
class mafGui;
class mafEvent;
//----------------------------------------------------------------------------
// mmoCreateMeter :
//----------------------------------------------------------------------------
/** */
class mmoCreateMeter: public mafOp
{
public:
  mmoCreateMeter(wxString label);
 ~mmoCreateMeter(); 
  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
  mafVMEMeter *m_Meter;
};
#endif