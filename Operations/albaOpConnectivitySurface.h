/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpConnectivitySurface
 Authors: Daniele Giunchi - Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpConnectivitySurface_H__
#define __albaOpConnectivitySurface_H__

#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;
class albaEvent;
class albaVME;

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,albaVME*);
#endif

//----------------------------------------------------------------------------
// albaOpConnectivitySurface :
//----------------------------------------------------------------------------
/** This operation keeps an input a surface and as output we get a number of connected surfaces. 
Extract data is based on geometric connectivity.*/
class ALBA_EXPORT albaOpConnectivitySurface: public albaOp
{
public:
	albaOpConnectivitySurface(const wxString &label = "Connectivity Surface");
	~albaOpConnectivitySurface(); 
	virtual void OnEvent(albaEventBase *alba_event);

  albaTypeMacro(albaOpConnectivitySurface, albaOp);

  albaOp* Copy();

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

  /* Set the threshold */
  void SetThresold(double value){m_Thresold = value;};

  albaString GetNumberOfExtractedSurfaces(){return m_NumberOfExtractedSurfaces;};

  /** Extract all the connected surfaces. */
  void OnVtkConnect();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  void CreateGui();

	int m_Thresold;
	std::vector<albaVME*> m_ExtractedVmes;
	vtkPolyData		*m_OriginalPolydata;
	albaString m_NumberOfExtractedSurfaces;
  albaString m_Alert;

	int m_ExtractBiggestSurface;

};
#endif
