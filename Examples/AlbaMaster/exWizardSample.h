/*=========================================================================
Program:   AlbaMaster
Module:    albaOpInteractionOp.h
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
#include "albaWizard.h"

//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class albaVME;
class albaVMEGizmo;
class mmiCompositorMouse;

// ============================================================================
class exWizardSample : public albaWizard
{
public:
  /** Default constructor */
  exWizardSample(const wxString &label);
  
  /** Accept function*/
  virtual bool Accept(albaVME* vme);
};
#endif // __exWizardSample_H__








