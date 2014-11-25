/*=========================================================================

Program: MAF2Medical
Module: medGUIMDIFrame
Authors: Gianluigi Crimi

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __medGUIMDIFrame_H__
#define __medGUIMDIFrame_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafGUIMDIFrame.h"
//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: medGUIMDIFrame.
  Represents the main frame of a MED Application. On this frame  they'll be plugged  toolbars,  panels, progress bar etc...
  Closing this frame is equal to close the application.
  This frame extend mafGUIMDIFrame with wizard specific events
*/
class MAF_EXPORT medGUIMDIFrame : public mafGUIMDIFrame
{
 public:
  /** constructor. */
  medGUIMDIFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
  /** destructor. */
 ~medGUIMDIFrame(); 
  
protected:
 
	
	/** Send the 'Wizard' men event. */
  void OnMenuWizard(wxCommandEvent& e);
	
  /** Event Table Declaration*/
  DECLARE_EVENT_TABLE()
};

#endif
