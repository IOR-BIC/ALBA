/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUILutPreset
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUILutPreset_H__
#define __albaGUILutPreset_H__

#include "albaColor.h"
#include "vtkLookupTable.h"

/** number of the available LUT presets */
extern const int lutPresetNum;

/** names of the available LUT presets */
extern wxString LutNames[];

/** initialize a given LUT with the idx.th preset */
ALBA_EXPORT extern void lutPreset(int idx, vtkLookupTable *lut);

/** Given a lut return the index of the relative presets, -1 if the lut was not found in presets */
ALBA_EXPORT int presetsIdxByLut(vtkLookupTable *lut);

#endif


