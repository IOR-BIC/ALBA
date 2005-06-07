/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateSlicer.h,v $
  Language:  C++
  Date:      $Date: 2005-06-07 14:42:04 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoCreateSlicer_H__
#define __mmoCreateSlicer_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMESlicer;
class mafGui;
class mafEvent;
//----------------------------------------------------------------------------
// mmoCreateSlicer :
//----------------------------------------------------------------------------
/** */
class mmoCreateSlicer: public mafOp
{
public:
  mmoCreateSlicer(wxString label);
 ~mmoCreateSlicer(); 
  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();
  void OpUndo();

protected: 
	void OpStop(int result);

  mafVMESlicer *m_Slicer;
};
#endif
