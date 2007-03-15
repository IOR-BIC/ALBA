/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoScalarToSurface.h,v $
  Language:  C++
  Date:      $Date: 2007-03-15 14:22:25 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoScalarToSurface_H__
#define __mmoScalarToSurface_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVMESurface;

//----------------------------------------------------------------------------
// mmoScalarToSurface :
//----------------------------------------------------------------------------
/** */
class mmoScalarToSurface: public mafOp
{
public:
  mmoScalarToSurface(const wxString &label = "ScalarToSurface");
 ~mmoScalarToSurface(); 

  mafTypeMacro(mmoScalarToSurface, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();

protected: 
  mafVMESurface *m_Surface;
};
#endif
