/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpCreateSurfaceParametric.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:03:51 $
  Version:   $Revision: 1.2 $
  Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpCreateSurfaceParametric_H__
#define __mafOpCreateSurfaceParametric_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMESurfaceParametric;
class mafGUI;
class mafEvent;
//----------------------------------------------------------------------------
// mafOpCreateSurfaceParametric :
//----------------------------------------------------------------------------
/** */
class mafOpCreateSurfaceParametric: public mafOp
{
public:
  mafOpCreateSurfaceParametric(const wxString &label = "Create Parametric Surface");
  ~mafOpCreateSurfaceParametric(); 

  mafTypeMacro(mafOpCreateSurfaceParametric, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
  mafVMESurfaceParametric *m_SurfaceParametric;
};
#endif
