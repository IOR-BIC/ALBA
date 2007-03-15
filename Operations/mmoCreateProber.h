/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateProber.h,v $
  Language:  C++
  Date:      $Date: 2007-03-15 14:22:25 $
  Version:   $Revision: 1.4 $
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
  mmoCreateProber(const wxString &label = "CreateProber");
 ~mmoCreateProber(); 

  mafTypeMacro(mmoCreateProber, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
  mafVMEProber *m_Prober;
};
#endif
