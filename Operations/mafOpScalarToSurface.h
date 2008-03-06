/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpScalarToSurface.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpScalarToSurface_H__
#define __mafOpScalarToSurface_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVMESurface;

//----------------------------------------------------------------------------
// mafOpScalarToSurface :
//----------------------------------------------------------------------------
/** */
class mafOpScalarToSurface: public mafOp
{
public:
  mafOpScalarToSurface(const wxString &label = "ScalarToSurface");
 ~mafOpScalarToSurface(); 

  mafTypeMacro(mafOpScalarToSurface, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();

protected: 
  mafVMESurface *m_Surface;
};
#endif
