/*=========================================================================

 Program: MAF2
 Module: mafOpCropDeformableROI
 Authors: Matteo Giacomoni - Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCropDeformableROI_H__
#define __mafOpCropDeformableROI_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"
#include "mafVME.h"
#include "mafVMEOutput.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class vtkMaskPolyDataFilter;
class mafVMEVolumeGray;
class mafVMESurface;

//----------------------------------------------------------------------------
// mafOpCropDeformableROI :
//----------------------------------------------------------------------------
/**
  class name: mafOpCropDeformableROI
   Operation that use m_MaskPolydataFilter, cropping  a volume with a output surface of a vme.
*/
class MAF_EXPORT mafOpCropDeformableROI: public mafOp
{
public:
  /** construct */
	mafOpCropDeformableROI(const wxString &label = "CropDeformableROI");
  /** destructor */
	~mafOpCropDeformableROI(); 
  /** process events coming from other components */
	virtual void OnEvent(mafEventBase *maf_event);
  /** RTTI Macro */
	mafTypeMacro(mafOpCropDeformableROI, mafOp);

  /** acceptance static function */
	static bool OutputSurfaceAccept(mafNode *node) {return(node != NULL && ((mafVME*)node)->GetOutput()->IsA("mafVMEOutputSurface"));};

  /** Return a copy of the operation.*/
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  /** use  vtkMaskPolydataFilter to  achieve the output */
	void Algorithm(mafVME *vme);

  /** Select a surface as volume masking */
	void MaskSelection();

  /** Creates operation GUI*/
	void CreateGui();

	vtkMaskPolyDataFilter *m_MaskPolydataFilter;

	double m_Distance;
	double m_FillValue;
	int		m_InsideOut;
	mafNode *m_PNode;

	mafVMEVolumeGray *m_ResultVme;

};
#endif
