/*=========================================================================

 Program: MAF2Medical
 Module: medOpInteractionOp
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __exWizardSample_H__
#define __exWizardSample_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medWizard.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafVME;
class mafVMEGizmo;
class mmiCompositorMouse;

// ============================================================================
class exWizardSample : public medWizard
// ============================================================================
{
public:
  /** Default constructor */
  exWizardSample(const wxString &label);
  
  /** Accept function*/
  virtual bool Accept(mafNode* vme);

};
#endif // __medOpInteraction_H__








