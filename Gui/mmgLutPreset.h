/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgLutPreset.h,v $
  Language:  C++
  Date:      $Date: 2005-12-01 15:22:06 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgLutPreset_H__
#define __mmgLutPreset_H__

#include "mafColor.h"
#include "vtkLookupTable.h"

/** number of the available LUT presets */
extern const int lutPresetNum;

/** names of the available LUT presets */
extern wxString LutNames[];

/** initalize a given LUT with the idx.th preset */
extern void lutPreset(int idx, vtkLookupTable *lut);

#endif


