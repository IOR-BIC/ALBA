/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateMeter.h,v $
  Language:  C++
  Date:      $Date: 2005-06-21 11:35:29 $
  Version:   $Revision: 1.2 $
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
	void OpStop(int result);

  mafVMEMeter *m_Meter;
};
#endif