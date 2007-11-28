/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpLabelExtractor.h,v $
  Language:  C++
  Date:      $Date: 2007-11-28 14:30:43 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani - porting Roberto Mucci 
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpLabelExtractor_H__
#define __mafOpLabelExtractor_H__


#include "mafOp.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmgCheckListBox;
class mafTagItem;
class mafVME;
class mafVMESurface;
class mafOp;
class mafGui;
class mafEvent;
class vtkDataSet;
class vtkStructuredPoints;

//----------------------------------------------------------------------------
// mafOpLabelExtractor :
//----------------------------------------------------------------------------
/** Operation to create a VMESurface draw from a Volume*/
class mafOpLabelExtractor: public mafOp
{
public:
  mafOpLabelExtractor(const wxString& label = "LabelExtractor");
 ~mafOpLabelExtractor(); 
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
  mmgCheckListBox     *m_LabelCheckBox;
  mafTagItem          *m_TagLabel;

};
#endif
