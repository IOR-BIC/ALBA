/*=========================================================================

 Program: MAF2
 Module: mafOpWizardWait
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafWizardWaitOp_H__
#define __mafWizardWaitOp_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGui;
class mafEvent;

/** 
  class name: mafOpWizardWait
  Simple op with only an next-step button create to insert pause during wizards.
*/
class MAF_EXPORT mafWizardWaitOp: public mafOp
{
public:
  /** constructor */
  mafWizardWaitOp(const wxString &label = "Go to text step\n");
  /** destructor */
  ~mafWizardWaitOp();

  /** RTTI macro*/
  mafTypeMacro(mafWizardWaitOp, mafOp);

  /** clone the object and retrieve a copy*/
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafVME*node);

  /** Builds operation's interface. */
  void OpRun();
  
  /**Event management*/
  void OnEvent(mafEventBase *maf_event);
protected: 
};
#endif
