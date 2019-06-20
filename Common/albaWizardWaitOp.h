/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpWizardWait
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaWizardWaitOp_H__
#define __albaWizardWaitOp_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGui;
class albaEvent;

/** 
  class name: albaOpWizardWait
  Simple op with only an next-step button create to insert pause during wizards.
*/
class ALBA_EXPORT albaWizardWaitOp: public albaOp
{
public:
  /** constructor */
  albaWizardWaitOp(const wxString &label = "Go to text step\n");
  /** destructor */
  ~albaWizardWaitOp();

  /** RTTI macro*/
  albaTypeMacro(albaWizardWaitOp, albaOp);

  /** clone the object and retrieve a copy*/
  albaOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(albaVME*node);

  /** Builds operation's interface. */
  void OpRun();
  
  /**Event management*/
  void OnEvent(albaEventBase *alba_event);
protected: 
};
#endif
