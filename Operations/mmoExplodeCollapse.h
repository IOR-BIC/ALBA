/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoExplodeCollapse.h,v $
  Language:  C++
  Date:      $Date: 2007-03-15 14:22:25 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoExplodeCollapse_H__
#define __mmoExplodeCollapse_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;

//----------------------------------------------------------------------------
// mmoExplodeCollapse :
//----------------------------------------------------------------------------
/** */
class mmoExplodeCollapse: public mafOp
{
public:
  mmoExplodeCollapse(const wxString &label = "ExplodeCollapse");
 ~mmoExplodeCollapse(); 
  
  mafTypeMacro(mmoExplodeCollapse, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();
};
#endif
