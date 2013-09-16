/*=========================================================================

 Program: MAF2
 Module: mafOpVolumeUnion
 Author: Simone Bnà
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpVolumeUnion_H__
#define __mafOpVolumeUnion_H__

#include "mafNode.h"
#include "mafOp.h"
#include "mafVMEVolume.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUI;

class mafEventBase;
class mafString;

class vtkProbeFilter;
class vtkRectilinearGrid;
class vtkProbeFilter;
class vtkStructuredPoints;
class mafVMEVolume;  

//----------------------------------------------------------------------------
// mafOpVolumeUnion :
//----------------------------------------------------------------------------
/*
Build a VME rectilinear grid volume as the union of two given rectilinear grid subvolumes.
The user must specify:
 - the two input volumes
 - the resolution of the VME output volume
*/

class MAF_EXPORT mafOpVolumeUnion: public mafOp
{
public:
             
            	 mafOpVolumeUnion(const wxString &label=_("Volume Union"));
	virtual     ~mafOpVolumeUnion();
	/*virtual*/ void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(mafOpVolumeUnion, mafOp);

  static bool VmeUnionAccept(mafNode *node) {return(node != NULL && (node->IsMAFType(mafVMEVolume)));};

  mafOp* Copy();

  /*virtual*/ bool Accept(mafNode* node);
  /*virtual*/ void OpRun();	
	
  /** Makes the undo for the operation. */
  /*virtual*/ void OpUndo();

  /*virtual*/ void OpDo();

  /*virtual*/ void CreateGui(); 

 protected:
  ///*virtual*/ void OpStop(int result);	

  void BuildVolumeUnion();

  void UpdateGUI();

  bool VmeChoose(mafString title,mafEvent *e);
  
  double m_bounds[6];
  double m_resolutionXYZ[3];
  double m_spacingXYZ[3];
  bool m_vme_is_selected;

  vtkRectilinearGrid *m_VolUnionRG; ///< Pointer for Rectilinear Grid volume union
  mafVMEVolume *m_FirstVMEVolume;
  mafVMEVolume *m_SecondVMEVolume;

};
#endif