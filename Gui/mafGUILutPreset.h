/*=========================================================================

 Program: MAF2
 Module: mafGUILutPreset
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
MAF_EXPORT extern void lutPreset(int idx, vtkLookupTable *lut);

#endif


