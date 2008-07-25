/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUILutPreset.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGUILutPreset_H__
#define __mafGUILutPreset_H__

#include "mafColor.h"
#include "vtkLookupTable.h"

/** number of the available LUT presets */
extern const int lutPresetNum;

/** names of the available LUT presets */
extern wxString LutNames[];

/** initalize a given LUT with the idx.th preset */
extern void lutPreset(int idx, vtkLookupTable *lut);

#endif


