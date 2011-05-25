/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpCreateMeter.h,v $
  Language:  C++
  Date:      $Date: 2011-05-25 11:49:23 $
  Version:   $Revision: 1.2.2.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpCreateMeter_H__
#define __mafOpCreateMeter_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEMeter;
class mafGUI;
class mafEvent;
//----------------------------------------------------------------------------
// mafOpCreateMeter :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateMeter: public mafOp
{
public:
  mafOpCreateMeter(const wxString &label = "CreateMeter");
  ~mafOpCreateMeter(); 

  mafTypeMacro(mafOpCreateMeter, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
  mafVMEMeter *m_Meter;
};
#endif
