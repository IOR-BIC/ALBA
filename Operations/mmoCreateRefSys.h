/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateRefSys.h,v $
  Language:  C++
  Date:      $Date: 2007-03-15 14:22:25 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoCreateRefSys_H__
#define __mmoCreateRefSys_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMERefSys;
class mafNode;

//----------------------------------------------------------------------------
// mmoCreateRefSys :
//----------------------------------------------------------------------------
/** */
class mmoCreateRefSys: public mafOp
{
public:
  mmoCreateRefSys(const wxString &label = "CreateRefSys");
 ~mmoCreateRefSys(); 

  mafTypeMacro(mmoCreateRefSys, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
  mafVMERefSys *m_RefSys;
};
#endif
