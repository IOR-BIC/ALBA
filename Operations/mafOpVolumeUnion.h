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
class mafVMEVolumeGray;  //Gray or more general?

//----------------------------------------------------------------------------
// mafOpVolumeUnion :
//----------------------------------------------------------------------------
/** 
Build a vme gray volume as the union of two subvolumes  
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
 //  void UpdateGui();
  ///*virtual*/ void OpStop(int result);	

  void BuildVolumeUnion();

  bool VmeChoose(mafString title,mafEvent *e);
  
//  vtkRectilinearGrid *m_InputRG;    ///< Pointer for Rectilinear Grid volume
  vtkRectilinearGrid *m_VolUnionRG; ///< Pointer for Rectilinear Grid volume union
  //vtkStructuredPoints *m_InputSP; ///< Pointer for Structured Points volume

  double resolutionXYZ[3];

  bool vme_is_selected;

  mafVMEVolume *m_FirstVMEVolume;
  mafVMEVolume *m_SecondVMEVolume;

};
#endif