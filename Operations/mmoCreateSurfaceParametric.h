/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateSurfaceParametric.h,v $
  Language:  C++
  Date:      $Date: 2007-03-15 14:22:25 $
  Version:   $Revision: 1.2 $
  Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoCreateSurfaceParametric_H__
#define __mmoCreateSurfaceParametric_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMESurfaceParametric;
class mafGui;
class mafEvent;
//----------------------------------------------------------------------------
// mmoCreateSurfaceParametric :
//----------------------------------------------------------------------------
/** */
class mmoCreateSurfaceParametric: public mafOp
{
public:
  mmoCreateSurfaceParametric(const wxString &label = "Create Parametric Surface");
  ~mmoCreateSurfaceParametric(); 

  mafTypeMacro(mmoCreateSurfaceParametric, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
  mafVMESurfaceParametric *m_SurfaceParametric;
};
#endif
