/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoFilterVolume.cpp,v $
  Language:  C++
  Date:      $Date: 2005-12-01 11:13:14 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmoFilterVolume.h"
#include <wx/busyinfo.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgGui.h"

#include "mafVMEVolumeGray.h"

#include "vtkMAFSmartPointer.h"
#include "vtkImageData.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkImageMedian3D.h"

//----------------------------------------------------------------------------
mmoFilterVolume::mmoFilterVolume(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_InputPreserving = false;

  m_PreviewResultFlag	= false;
	m_ClearInterfaceFlag= false;
  
	m_ResultImageData	  = NULL;
	m_OriginalImageData = NULL;

  m_Dimensionality  = 3;
  m_SmoothRadius[0] = m_SmoothRadius[1] = m_SmoothRadius[2] = 1.5;
  m_StandardDeviation[0] = m_StandardDeviation[1] = m_StandardDeviation[2] = 2.0;

  m_KernelSize[0] = m_KernelSize[1] = m_KernelSize[2] = 1;
}
//----------------------------------------------------------------------------
mmoFilterVolume::~mmoFilterVolume()
//----------------------------------------------------------------------------
{
	vtkDEL(m_ResultImageData);
	vtkDEL(m_OriginalImageData);
}
//----------------------------------------------------------------------------
bool mmoFilterVolume::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMEVolumeGray));
}
//----------------------------------------------------------------------------
mafOp *mmoFilterVolume::Copy()
//----------------------------------------------------------------------------
{
  return (new mmoFilterVolume(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum FILTER_SURFACE_ID
{
	ID_SMOOTH = MINID,
  ID_STANDARD_DEVIATION,
  ID_RADIUS_FACTOR,
  ID_MEDIAN,
  ID_KERNEL_SIZE,
	ID_PREVIEW,
	ID_CLEAR,
};
//----------------------------------------------------------------------------
void mmoFilterVolume::OpRun()   
//----------------------------------------------------------------------------
{  
	wxBusyInfo wait("initializing volume please wait...");

  vtkNEW(m_ResultImageData);
	m_ResultImageData->DeepCopy((vtkImageData*)((mafVME *)m_Input)->GetOutput()->GetVTKData());
	
	vtkNEW(m_OriginalImageData);
	m_OriginalImageData->DeepCopy((vtkImageData*)((mafVME *)m_Input)->GetOutput()->GetVTKData());
	
	// interface:
	m_Gui = new mmgGui(this);
	m_Gui->SetListener(this);

	m_Gui->Label("");
	m_Gui->Label("smooth",true);
	m_Gui->Vector(ID_STANDARD_DEVIATION,"sd: ",m_StandardDeviation,0.1,100,2,"standard deviation for smooth filter");
  m_Gui->Vector(ID_RADIUS_FACTOR,"radius: ",m_SmoothRadius,1,10,2,"radius for smooth filter");
	m_Gui->Button(ID_SMOOTH,"apply smooth");

  m_Gui->Label("");
  m_Gui->Label("median",true);
  m_Gui->Vector(ID_KERNEL_SIZE,"kernel: ",m_KernelSize,1,10,"size of kernel");
  m_Gui->Button(ID_MEDIAN,"apply median");

  m_Gui->Divider(2);
	m_Gui->Button(ID_PREVIEW,"preview");
	m_Gui->Button(ID_CLEAR,"clear");
	m_Gui->OkCancel();
	m_Gui->Enable(wxOK,false);

	m_Gui->Enable(ID_PREVIEW,false);
	m_Gui->Enable(ID_CLEAR,false);
	ShowGui();
}
//----------------------------------------------------------------------------
void mmoFilterVolume::OpDo()
//----------------------------------------------------------------------------
{
	((mafVMEVolumeGray *)m_Input)->SetData(m_ResultImageData,((mafVME *)m_Input)->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mmoFilterVolume::OpUndo()
//----------------------------------------------------------------------------
{
  ((mafVMEVolumeGray *)m_Input)->SetData(m_OriginalImageData,((mafVME *)m_Input)->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mmoFilterVolume::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {	
      case ID_SMOOTH:
        OnSmooth();
      break;
      case ID_MEDIAN:
        OnMedian();
      break;
      case ID_PREVIEW:
        OnPreview(); 
      break;
      case ID_CLEAR:
        OnClear(); 
      break;
      case wxOK:
        if(m_PreviewResultFlag)
          OnPreview();
        OpStop(OP_RUN_OK);        
      break;
      case wxCANCEL:
        if(m_ClearInterfaceFlag)
          OnClear();
        OpStop(OP_RUN_CANCEL);        
      break;
    }
  }
}
//----------------------------------------------------------------------------
void mmoFilterVolume::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	delete m_Gui;
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void mmoFilterVolume::OnSmooth()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;
	m_Gui->Enable(ID_SMOOTH,false);
  m_Gui->Enable(ID_RADIUS_FACTOR,false);
  m_Gui->Enable(ID_RADIUS_FACTOR,false);
	m_Gui->Update();

  vtkMAFSmartPointer<vtkImageGaussianSmooth> smoothFilter;
	smoothFilter->SetInput(m_ResultImageData);
  smoothFilter->SetDimensionality(m_Dimensionality);
  smoothFilter->SetRadiusFactors(m_SmoothRadius);
  smoothFilter->SetStandardDeviations(m_StandardDeviation);
	smoothFilter->Update();

	m_ResultImageData->DeepCopy(smoothFilter->GetOutput());

	m_Gui->Enable(ID_SMOOTH,true);
	m_Gui->Enable(ID_RADIUS_FACTOR,true);
  m_Gui->Enable(ID_RADIUS_FACTOR,true);

	m_Gui->Enable(ID_PREVIEW,true);
	m_Gui->Enable(ID_CLEAR,true);
	m_Gui->Enable(wxOK,true);

	m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void mmoFilterVolume::OnMedian()
//----------------------------------------------------------------------------
{
  wxBusyCursor wait;
  m_Gui->Enable(ID_MEDIAN,false);
  m_Gui->Enable(ID_KERNEL_SIZE,false);
  m_Gui->Update();

  vtkMAFSmartPointer<vtkImageMedian3D> medianFilter;
  medianFilter->SetInput(m_ResultImageData);
  medianFilter->SetKernelSize(m_KernelSize[0],m_KernelSize[1],m_KernelSize[2]);
  medianFilter->Update();

  m_ResultImageData->DeepCopy(medianFilter->GetOutput());

  m_Gui->Enable(ID_MEDIAN,true);
  m_Gui->Enable(ID_KERNEL_SIZE,true);

  m_Gui->Enable(ID_PREVIEW,true);
  m_Gui->Enable(ID_CLEAR,true);
  m_Gui->Enable(wxOK,true);

  m_PreviewResultFlag = true;
}
//----------------------------------------------------------------------------
void mmoFilterVolume::OnPreview()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;
	
  ((mafVMEVolumeGray *)m_Input)->SetData(m_ResultImageData,((mafVME *)m_Input)->GetTimeStamp());

	m_Gui->Enable(ID_PREVIEW,false);
	m_Gui->Enable(ID_CLEAR,true);
	m_Gui->Enable(wxOK,true);

	m_PreviewResultFlag   = false;
	m_ClearInterfaceFlag	= true;

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mmoFilterVolume::OnClear()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

  ((mafVMEVolumeGray *)m_Input)->SetData(m_OriginalImageData,((mafVME *)m_Input)->GetTimeStamp());
	
	m_ResultImageData->DeepCopy(m_OriginalImageData);

	m_Gui->Enable(ID_SMOOTH,true);
	m_Gui->Enable(ID_RADIUS_FACTOR,true);
  m_Gui->Enable(ID_STANDARD_DEVIATION,true);

	m_Gui->Enable(ID_PREVIEW,false);
	m_Gui->Enable(ID_CLEAR,false);
	m_Gui->Enable(wxOK,false);
	m_Gui->Update();

	m_PreviewResultFlag = false;
	m_ClearInterfaceFlag= false;

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
