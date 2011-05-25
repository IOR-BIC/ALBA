/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpCreateRefSys.h,v $
  Language:  C++
  Date:      $Date: 2011-05-25 11:49:23 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpCreateRefSys_H__
#define __mafOpCreateRefSys_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMERefSys;
class mafNode;

//----------------------------------------------------------------------------
// mafOpCreateRefSys :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateRefSys: public mafOp
{
public:
  mafOpCreateRefSys(const wxString &label = "CreateRefSys");
 ~mafOpCreateRefSys(); 

  mafTypeMacro(mafOpCreateRefSys, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
  mafVMERefSys *m_RefSys;
};
#endif
