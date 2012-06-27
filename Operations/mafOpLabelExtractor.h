/*=========================================================================

 Program: MAF2
 Module: mafOpLabelExtractor
 Authors: Paolo Quadrani - porting Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpLabelExtractor_H__
#define __mafOpLabelExtractor_H__


#include "mafOp.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUICheckListBox;
class mafTagItem;
class mafVME;
class mafVMESurface;
class mafOp;
class mafGUI;
class mafEvent;
class vtkDataSet;
class vtkStructuredPoints;

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,wxString);
EXPORT_STL_VECTOR(MAF_EXPORT,bool);
#endif

//----------------------------------------------------------------------------
// mafOpLabelExtractor :
//----------------------------------------------------------------------------
/** Operation to create a VMESurface draw from a Volume*/
class MAF_EXPORT mafOpLabelExtractor: public mafOp
{
public:
  mafOpLabelExtractor(const wxString& label = "LabelExtractor");
 ~mafOpLabelExtractor(); 

  mafTypeMacro(mafOpLabelExtractor, mafOp);


	virtual void OnEvent(mafEventBase *maf_event);
  mafOp* Copy();

  bool Accept(mafNode *vme);
  void OpRun();

  /** If labels tags are present retrieve them. */
  bool RetrieveTag();

  /** Fill the vector of label. */
  void FillLabelVector(wxString name, bool checked = TRUE);

  // Set the label value
  void SetLabel(double labelValue);

  //Set if smooth mode is true
  void SmoothMode(bool smoothMode);

  // Create a VMESurface draw from a Volume
	void ExtractLabel();

protected: 

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

  vtkStructuredPoints *m_OutputData;
  vtkDataSet          *m_Ds;
	mafVMESurface       *m_Vme;
  mafGUICheckListBox     *m_LabelCheckBox;
  mafTagItem          *m_TagLabel;

};
#endif
