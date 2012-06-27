/*=========================================================================

 Program: MAF2
 Module: mafOpConnectivitySurface
 Authors: Daniele Giunchi - Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpConnectivitySurface_H__
#define __mafOpConnectivitySurface_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;
class mafEvent;
class mafVME;

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,mafVME*);
#endif

//----------------------------------------------------------------------------
// mafOpConnectivitySurface :
//----------------------------------------------------------------------------
/** This operation keeps an input a surface and as output we get a number of connected surfaces. 
Extract data is based on geometric connectivity.*/
class MAF_EXPORT mafOpConnectivitySurface: public mafOp
{
public:
	mafOpConnectivitySurface(const wxString &label = "Connectivity Surface");
	~mafOpConnectivitySurface(); 
	virtual void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(mafOpConnectivitySurface, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

  /* Set the threshold */
  void SetThresold(double value){m_Thresold = value;};

  mafString GetNumberOfExtractedSurfaces(){return m_NumberOfExtractedSurfaces;};

  /** Extract all the connected surfaces. */
  void OnVtkConnect();

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  void CreateGui();

	int m_Thresold;
	std::vector<mafVME*> m_ExtractedVmes;
	vtkPolyData		*m_OriginalPolydata;
	mafString m_NumberOfExtractedSurfaces;
  mafString m_Alert;

	int m_ExtractBiggestSurface;

};
#endif
