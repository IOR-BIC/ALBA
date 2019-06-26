/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpVOIDensity
 Authors: Matteo Giacomoni & Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpVOIDensity_H__
#define __albaOpVOIDensity_H__

#include "albaOp.h"
#include "albaVME.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkDoubleArray;

//----------------------------------------------------------------------------
// albaOpVOIDensity :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpVOIDensity: public albaOp
{
public:
	albaOpVOIDensity(const wxString &label = "VOIDensity");
	~albaOpVOIDensity(); 
	virtual void OnEvent(albaEventBase *alba_event);
  
  albaTypeMacro(albaOpVOIDensity, albaOp);

  /** return a copy of itself, this needs to put the operation into the undo stack. */
	albaOp* Copy();
	
	/** Return true for the acceptable vme type. */
	bool Accept(albaVME* Node);

	/** Set the input vme for the operation. */
	void OpRun();

	/** Builds operation's interface. */
	void OpDo();

	/** Execute the operation. */
	void OpUndo();

	/**	Set surface of input in test mode*/
	void SetSurface(albaVME *Surface){m_Surface=Surface;};

	/**	Return the min scalar*/
	double GetMinScalar(){return m_MinScalar;};

	/**	Return the MAX scalar*/
	double GetMaxScalar(){return m_MaxScalar;};

	/**	Return the mean scalar*/
	double GetMeanScalar(){return m_MeanScalar;};
	
	/**	Return the number of scalars*/
	int GetNumberScalars(){return m_NumberOfScalars;};

	/**	Return Standard Deviation*/
	double GetStandardDeviation(){return m_StandardDeviation;};

	/** Extract scalars from input volume that are inside the choosed surface. */
	void ExtractVolumeScalars();

 	static bool OutputSurfaceAccept(albaVME* Node) {return(Node != NULL && (Node->GetOutput()->IsA("albaVMEOutputSurface")));};

protected:
  albaVME        *m_Surface;
  vtkDoubleArray *m_VOIScalars;
  albaString       m_NumberOfScalarsString;
  int             m_NumberOfScalars;
  albaString       m_MeanScalarString;
  double          m_MeanScalar;
  albaString       m_MaxScalarString;
  albaString       m_MinScalarString;
  double          m_MaxScalar;
  double          m_MinScalar;
  albaString       m_StandardDeviationString;
  double          m_StandardDeviation;
	wxListBox			 *m_VoxelList;
};
#endif
