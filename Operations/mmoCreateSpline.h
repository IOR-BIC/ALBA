/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateSpline.h,v $
  Language:  C++
  Date:      $Date: 2007-02-20 10:01:23 $
  Version:   $Revision: 1.1 $
  Authors:   Daniele Giunchi & Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoCreateSpline_H__
#define __mmoCreateSpline_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEPolylineSpline;
class mafGui;
class mafEvent;
//----------------------------------------------------------------------------
// mmoCreateSpline :
//----------------------------------------------------------------------------
/** */
class mmoCreateSpline: public mafOp
{
public:
  mmoCreateSpline(wxString label = "Create Parametric Surface");
  ~mmoCreateSpline(); 

  mafTypeMacro(mmoCreateSpline, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
  mafVMEPolylineSpline *m_PolylineSpline;
};
#endif
