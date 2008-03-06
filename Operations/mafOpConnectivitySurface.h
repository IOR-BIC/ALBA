/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpConnectivitySurface.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
  Version:   $Revision: 1.1 $
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
/** */
class mafOpConnectivitySurface: public mafOp
{
public:
	mafOpConnectivitySurface(const wxString &label = "FilterSurface");
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

  /* Set the thresold */
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
