/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateSlicer.h,v $
  Language:  C++
  Date:      $Date: 2007-03-15 14:22:25 $
  Version:   $Revision: 1.6 $
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
  mmoCreateSlicer(const wxString &label = "CreateSlicer");
 ~mmoCreateSlicer(); 

  mafTypeMacro(mmoCreateSlicer, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
  mafVMESlicer *m_Slicer;
};
#endif
