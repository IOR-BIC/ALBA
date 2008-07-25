/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpCreateSpline.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:03:51 $
  Version:   $Revision: 1.2 $
  Authors:   Daniele Giunchi & Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
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
class mafOpCreateSpline: public mafOp
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
