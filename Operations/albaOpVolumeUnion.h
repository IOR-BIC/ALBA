/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpVolumeUnion
 Author: Simone Bnà
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpVolumeUnion_H__
#define __albaOpVolumeUnion_H__

#include "albaVME.h"
#include "albaOp.h"
#include "albaVMEVolume.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUI;

class albaEventBase;
class albaString;

class vtkProbeFilter;
class vtkRectilinearGrid;
class vtkProbeFilter;
class vtkImageData;
class albaVMEVolume;  

//----------------------------------------------------------------------------
// albaOpVolumeUnion :
//----------------------------------------------------------------------------
/*
Build a VME rectilinear grid volume as the union of two given rectilinear grid subvolumes.
The user must specify:
 - the two input volumes
 - the resolution of the VME output volume
*/

class ALBA_EXPORT albaOpVolumeUnion: public albaOp
{
public:
             
            	 albaOpVolumeUnion(const wxString &label=_("Volume Union"));
	virtual     ~albaOpVolumeUnion();
	/*virtual*/ void OnEvent(albaEventBase *alba_event);

  albaTypeMacro(albaOpVolumeUnion, albaOp);

  static bool VmeUnionAccept(albaVME*node) {return(node != NULL && (node->IsALBAType(albaVMEVolume)));};

  albaOp* Copy();

  /*virtual*/ void OpRun();	
	
  /** Makes the undo for the operation. */
  /*virtual*/ void OpUndo();

  /*virtual*/ void OpDo();

  /*virtual*/ void CreateGui(); 

 protected:

	 /** Return true for the acceptable vme type. */
	 bool InternalAccept(albaVME*node);

  ///*virtual*/ void OpStop(int result);	

  void BuildVolumeUnion();

  void UpdateGUI();

  bool VmeChoose(albaString title,albaEvent *e);
  
  double m_bounds[6];
  double m_resolutionXYZ[3];
  double m_spacingXYZ[3];
  bool m_vme_is_selected;

  vtkRectilinearGrid *m_VolUnionRG; ///< Pointer for Rectilinear Grid volume union
  vtkImageData *m_VolUnionRGstr;
  albaVMEVolume *m_FirstVMEVolume;
  albaVMEVolume *m_SecondVMEVolume;

};
#endif