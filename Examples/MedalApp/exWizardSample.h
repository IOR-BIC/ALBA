/*=========================================================================

 Program: MAF2
 Module: exWizard Sample
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
#include "mafWizard.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafVME;
class mafVMEGizmo;
class mmiCompositorMouse;

// ============================================================================
class exWizardSample : public mafWizard
// ============================================================================
{
public:
  /** Default constructor */
  exWizardSample(const wxString &label);
  
  /** Accept function*/
  virtual bool Accept(mafVME* vme);

};
#endif // __exWizardSample_H__








