/*=========================================================================
Program:   AlbaMaster
Module:    mafOpInteractionOp.h
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2018 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
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
{
public:
  /** Default constructor */
  exWizardSample(const wxString &label);
  
  /** Accept function*/
  virtual bool Accept(mafVME* vme);
};
#endif // __exWizardSample_H__








