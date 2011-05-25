/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpCreateSpline.h,v $
  Language:  C++
  Date:      $Date: 2011-05-25 11:49:23 $
  Version:   $Revision: 1.2.2.1 $
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
