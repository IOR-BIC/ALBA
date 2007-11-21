/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpLabelExtractor.h,v $
  Language:  C++
  Date:      $Date: 2007-11-21 15:14:47 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani - porting Roberto Mucci 
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpLabelExtractor_H__
#define __mafOpLabelExtractor_H__


#include "mafOp.h"
/*
#ifdef __GNUG__
    #pragma interface "mafOpLabelExtractor.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif*/

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMESurface;
class mafOp;
class mafGui;
class mafEvent;

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
  void OpDo();
  void OpUndo();

  // Set the label value
  void SetLabel(double labelValue);

  //Set if smooth mode is true
  void SmoothMode(bool smoothMode);

protected: 
  // Ccreate a VMESurface draw from a Volume
	void ExtractLabel();

	double   m_ValLabel;
  int      m_SmoothVolume;
  float    m_RadiusFactor;
  float    m_StdDev[3];
  float    m_RadiusFactorAfter;
  float    m_StdDevAfter[3];
  int      m_SamplingRate[3];
	wxString m_SurfaceName;
	mafVMESurface *m_Vme;
};
#endif
