/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoScalarToSurface.h,v $
  Language:  C++
  Date:      $Date: 2006-10-23 14:16:16 $
  Version:   $Revision: 1.1 $
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
  mmoScalarToSurface(wxString label = "ScalarToSurface");
 ~mmoScalarToSurface(); 

  mafTypeMacro(mmoScalarToSurface, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();

protected: 
  mafVMESurface *m_Surface;
};
#endif
