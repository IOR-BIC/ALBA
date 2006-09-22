/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoExplodeCollapse.h,v $
  Language:  C++
  Date:      $Date: 2006-09-22 10:11:57 $
  Version:   $Revision: 1.2 $
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
  mmoExplodeCollapse(wxString label = "ExplodeCollapse");
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
