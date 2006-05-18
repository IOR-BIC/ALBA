/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateSlicer.h,v $
  Language:  C++
  Date:      $Date: 2006-05-18 10:26:47 $
  Version:   $Revision: 1.4 $
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

protected: 
  mafVMESlicer *m_Slicer;
};
#endif
