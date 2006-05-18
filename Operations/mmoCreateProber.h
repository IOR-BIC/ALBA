/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateProber.h,v $
  Language:  C++
  Date:      $Date: 2006-05-18 10:26:47 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoCreateProber_H__
#define __mmoCreateProber_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEProber;
class mafGui;
class mafEvent;
//----------------------------------------------------------------------------
// mmoCreateProber :
//----------------------------------------------------------------------------
/** */
class mmoCreateProber: public mafOp
{
public:
  mmoCreateProber(wxString label);
 ~mmoCreateProber(); 
  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
  mafVMEProber *m_Prober;
};
#endif
