/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCropDeformableROI
 Authors: Matteo Giacomoni - Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpCropDeformableROI_H__
#define __albaOpCropDeformableROI_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"
#include "albaVME.h"
#include "albaVMEOutput.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class vtkMaskPolyDataFilter;
class albaVMEVolumeGray;
class albaVMESurface;

//----------------------------------------------------------------------------
// albaOpCropDeformableROI :
//----------------------------------------------------------------------------
/**
  class name: albaOpCropDeformableROI
   Operation that use m_MaskPolydataFilter, cropping  a volume with a output surface of a vme.
*/
class ALBA_EXPORT albaOpCropDeformableROI: public albaOp
{
public:
  /** construct */
	albaOpCropDeformableROI(const wxString &label = "CropDeformableROI");
  /** destructor */
	~albaOpCropDeformableROI(); 
  /** process events coming from other components */
	virtual void OnEvent(albaEventBase *alba_event);
  /** RTTI Macro */
	albaTypeMacro(albaOpCropDeformableROI, albaOp);

  /** acceptance static function */
	static bool OutputSurfaceAccept(albaVME*node) {return(node != NULL && node->GetOutput()->IsA("albaVMEOutputSurface"));};

  /** Return a copy of the operation.*/
	albaOp* Copy();

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  /** use  vtkMaskPolydataFilter to  achieve the output */
	void Algorithm(albaVME *vme);

  /** Select a surface as volume masking */
	void MaskSelection();

  /** Creates operation GUI*/
	void CreateGui();

	vtkMaskPolyDataFilter *m_MaskPolydataFilter;

	double m_Distance;
	double m_FillValue;
	int		m_InsideOut;
	albaVME *m_PNode;

	albaVMEVolumeGray *m_ResultVme;

};
#endif
