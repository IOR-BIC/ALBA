/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpCreateWrappedMeter.h,v $
  Language:  C++
  Date:      $Date: 2007-08-20 13:49:36 $
  Version:   $Revision: 1.1 $
  Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpCreateWrappedMeter_H__
#define __medOpCreateWrappedMeter_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class medVMEWrappedMeter;
class mafGui;
class mafEvent;
//----------------------------------------------------------------------------
// medOpCreateWrappedMeter :
//----------------------------------------------------------------------------
/** */
class medOpCreateWrappedMeter: public mafOp
{
public:
  medOpCreateWrappedMeter(const wxString &label = "CreateWrappedMeter");
  ~medOpCreateWrappedMeter(); 

  mafTypeMacro(medOpCreateWrappedMeter, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
  medVMEWrappedMeter *m_Meter;
};
#endif
