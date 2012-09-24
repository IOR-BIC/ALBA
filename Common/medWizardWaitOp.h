/*=========================================================================

 Program: MAF2Medical
 Module: medOpComputeWrapping
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medWizardWaitOp_H__
#define __medWizardWaitOp_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medCommonDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGui;
class mafEvent;

/** 
  class name: medOpWizardWait
  Simple op with only an next-step button create to insert pause during wizards.
*/
class MED_COMMON_EXPORT medWizardWaitOp: public mafOp
{
public:
  /** constructor */
  medWizardWaitOp(const wxString &label = "WizardWait");
  /** destructor */
  ~medWizardWaitOp();

  /** RTTI macro*/
  mafTypeMacro(medWizardWaitOp, mafOp);

  /** clone the object and retrieve a copy*/
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);
  /** Builds operation's interface. */
  void OpRun();
  
  /**Event management*/
  void OnEvent(mafEventBase *maf_event);
protected: 
};
#endif
