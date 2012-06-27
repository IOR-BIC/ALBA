/*=========================================================================

 Program: MAF2
 Module: mafOpCreateSpline
 Authors: Daniele Giunchi & Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCreateSpline_H__
#define __mafOpCreateSpline_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEPolylineSpline;
class mafGUI;
class mafEvent;
//----------------------------------------------------------------------------
// mafOpCreateSpline :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateSpline: public mafOp
{
public:
  mafOpCreateSpline(const wxString &label = "Create Parametric Surface");
  ~mafOpCreateSpline(); 

  mafTypeMacro(mafOpCreateSpline, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
  mafVMEPolylineSpline *m_PolylineSpline;
};
#endif
