/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpLabelExtractor
 Authors: Paolo Quadrani - porting Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpLabelExtractor_H__
#define __albaOpLabelExtractor_H__


#include "albaOp.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUICheckListBox;
class albaTagItem;
class albaVME;
class albaVMESurface;
class albaOp;
class albaGUI;
class albaEvent;
class vtkDataSet;
class vtkImageData;

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,wxString);
EXPORT_STL_VECTOR(ALBA_EXPORT,bool);
#endif

//----------------------------------------------------------------------------
// albaOpLabelExtractor :
//----------------------------------------------------------------------------
/** Operation to create a VMESurface draw from a Volume*/
class ALBA_EXPORT albaOpLabelExtractor: public albaOp
{
public:
  albaOpLabelExtractor(const wxString& label = "LabelExtractor");
 ~albaOpLabelExtractor(); 

  albaTypeMacro(albaOpLabelExtractor, albaOp);


	virtual void OnEvent(albaEventBase *alba_event);
  albaOp* Copy();

  void OpRun();

  /** If labels tags are present retrieve them. */
  bool RetrieveTag();

  /** Fill the vector of label. */
  void FillLabelVector(wxString name, bool checked = true);

  // Set the label value
  void SetLabel(double labelValue);

  //Set if smooth mode is true
  void SmoothMode(bool smoothMode);

  // Create a VMESurface draw from a Volume
	void ExtractLabel();

protected: 

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  /** Get the dataset from linked node. */
  void UpdateDataLabel();

  /** Generate the volume depending on the labels selected. */
  void GenerateLabeledVolume();

  std::vector<wxString> m_LabelNameVector;
  std::vector<bool>     m_CheckedVector;

	double   m_ValLabel;
  int      m_SmoothVolume;
  float    m_RadiusFactor;
  float    m_StdDev[3];
  float    m_RadiusFactorAfter;
  float    m_StdDevAfter[3];
  int      m_SamplingRate[3];
	wxString m_SurfaceName;

  vtkImageData *m_OutputData;
  vtkDataSet          *m_Ds;
	albaVMESurface       *m_Vme;
  albaGUICheckListBox     *m_LabelCheckBox;
  albaTagItem          *m_TagLabel;

};
#endif
