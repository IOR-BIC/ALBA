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
#include "albaVect3d.h"

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

	/** Set the input vme for the operation. */
	void OpRun();

	/** Builds operation's interface. */
	void OpDo();

	/** Execute the operation. */
	void OpUndo();

	/** Called on Op Stop */
	virtual void OpStop(int result);

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

	/** Extract scalars from input volume that are inside the chosen surface. */
	void EvaluateSurface();

	void UpdateStrings();

	/** Set Surface */
	int SetSurface(albaVME *surface);
	
 	static bool OutputSurfaceAccept(albaVME* Node) {return(Node != NULL && (Node->GetOutput()->IsA("albaVMEOutputSurface")));};

	void CreateCSVFile(albaString file);


	/** Returns CreatePointCloudOutput */
	int GetCreatePointCloudOutput() const { return m_CreatePointCloudOutput; }

	/** Sets CreatePointCloudOutput */
	void SetCreatePointCloudOutput(int createPointCloudOutput) { m_CreatePointCloudOutput = createPointCloudOutput; }

protected:

	void WriteReport();

	
	void GetTags();

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** Stores a tag on the selected surface */
	void SetDoubleTag(wxString tagName, double value);

	void CreatePointSamplingOutput();

	void CalculateSurfaceArea();

	void CreateSegmentationOutput();

	double GetMedian(vtkDoubleArray *valuesArray);
	
	static int Cmpfunc(const void * a, const void * b);
	virtual void CreateGui();

  albaVME        *m_Surface;
  vtkDoubleArray *m_VOIScalars;
	std::vector<unsigned int> m_VOIIds;
	std::vector<albaVect3d> m_VOICoords;
  albaString    m_NumberOfScalarsString;
  int           m_NumberOfScalars;
  albaString    m_MeanScalarString;
  double        m_MeanScalar;
  albaString    m_MaxScalarString;
  albaString    m_MinScalarString;
  double        m_MaxScalar;
  double        m_MinScalar;
  albaString    m_StandardDeviationString;
  double        m_StandardDeviation;
	albaString    m_MedianString;
	double				m_Median;
	albaString		m_SurfaceAreaString;
	double				m_SurfaceArea;
	wxListBox			*m_VoxelList;
	int						m_EvaluateInSubRange;
	double				m_SubRange[2];
	int						m_CreateSegOutput;
	int						m_CreatePointCloudOutput;
	bool					m_ImagedataVol;

	albaString m_PatientName;
	albaString m_PatientCode;
	albaString m_PatientBirthdate;
	albaString m_PatientCenter;
	albaString m_PatientExamDate;
};
#endif
