/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCrop.h,v $
  Language:  C++
  Date:      $Date: 2006-12-19 11:43:44 $
  Version:   $Revision: 1.6 $
  Authors:   Matteo Giacomoni & Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoCrop_H__
#define __mmoCrop_H__

#include "mafNode.h"
#include "mafOp.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmgGui;

class mafEventBase;
class mafString;

class mafVMEVolumeGray;
class mafVMESurface;
class mafVMEGizmo;

class vtkRectilinearGrid;
class vtkProbeFilter;
class vtkStructuredPoints;
class mafGizmoROI;

//----------------------------------------------------------------------------
// mmoCrop :
//----------------------------------------------------------------------------
/** 
Crop a vme gray volume  
*/

class mmoCrop: public mafOp
{
public:
             
            	 mmoCrop(wxString label = "Crop");
	virtual     ~mmoCrop();
	virtual void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(mmoCrop, mafOp);

  mafOp* Copy();

	bool Accept(mafNode* Node);
	void OpRun();	
	
  /**	Extract the volume cropped*/
	void Crop();

  void OpDo();

  virtual void CreateGui(); 

	/**	Set cropping area*/
	void SetCroppingBoxBounds(double bounds[]);

protected:
  void UpdateGui();
  void OpStop(int result);	
  
  mafGizmoROI *m_GizmoROI;
  
	vtkRectilinearGrid *m_InputRG;
	vtkStructuredPoints *m_InputSP;
	
	//mafNode *m_Vme;
	mafVMEVolumeGray *m_Vme;
	double m_XSpacing;
	double m_YSpacing;
	double m_ZSpacing;
	double m_CroppingBoxBounds[6];	
	double m_InputBounds[6];	

	double m_XminXmax[2];
	double m_YminYmax[2];
	double m_ZminZmax[2];

  int m_ShowHandles;
  int m_ShowROI;
  
};


#endif

