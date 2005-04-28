/*=========================================================================
  Program:   Multisense HipOpMS
  Module:    $RCSfile: mafInteractionDecl.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-28 16:10:12 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden  & Marco Petrone
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafInteractionDecl.h"

//----------------------------------------------------------------------------
// MFL Events
//----------------------------------------------------------------------------

MFL_EVT_IMP(CameraUpdateChannel);
MFL_EVT_IMP(CameraUpdateEvent);
MFL_EVT_IMP(ResetCameraEvent);
MFL_EVT_IMP(ViewSelectedEvent);
//MFL_EVT_IMP(CameraModifiedEvent);
MFL_EVT_IMP(VMESelectEvent);
MFL_EVT_IMP(VMEHideEvent);
MFL_EVT_IMP(CameraFitEvent);
MFL_EVT_IMP(ShowContextualMenuEvent);
MFL_EVT_IMP(WrapMafEvent);
