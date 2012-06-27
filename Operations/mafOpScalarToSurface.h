/*=========================================================================

 Program: MAF2
 Module: mafOpScalarToSurface
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class MAF_EXPORT mafOpScalarToSurface: public mafOp
{
public:
  /** Constructor. */
  mafOpScalarToSurface(const wxString &label = "ScalarToSurface");

  /** Destructor. */
 ~mafOpScalarToSurface(); 

 /** RTTI Macro. */
  mafTypeMacro(mafOpScalarToSurface, mafOp);

  /** Copy the operation. */
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

  /** Builds the output surface. */
  void OpRun();

protected: 
  mafVMESurface *m_Surface;
};
#endif
