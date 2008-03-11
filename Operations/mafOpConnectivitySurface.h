/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpConnectivitySurface.h,v $
  Language:  C++
  Date:      $Date: 2008-03-11 09:51:49 $
  Version:   $Revision: 1.2 $
  Authors:   Daniele Giunchi - Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
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

//----------------------------------------------------------------------------
// mafOpConnectivitySurface :
//----------------------------------------------------------------------------
/** This operation keeps an input a surface and as output we get a number of connected surfaces. 
Extract data is based on geometric connectivity.*/
class mafOpConnectivitySurface: public mafOp
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

	double m_Thresold;
	std::vector<mafVME*> m_ExtractedVmes;
	vtkPolyData		*m_OriginalPolydata;
	mafString m_NumberOfExtractedSurfaces;
  mafString m_Alert;

	int m_ExtractBiggestSurface;

};
#endif
