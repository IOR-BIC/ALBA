/*=========================================================================

 Program: MAF2
 Module: mafOpCreateSurfaceParametric
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class MAF_EXPORT mafOpCreateSurfaceParametric: public mafOp
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
