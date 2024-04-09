/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpEqualizeHistogram
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaOpEqualizeHistogram.h"
#include "wx/busyinfo.h"
#include "albaVME.h"
#include "albaGUI.h"
#include "albaGUIDialog.h"
#include "albaGUIHistogramWidget.h"
#include "albaVMEVolumeGray.h"
#include "albaOpVolumeResample.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBAHistogram.h"
#include "vtkImageData.h"
#include "vtkImageCast.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkImageToStructuredPoints.h"

#include "itkVTKImageToImageFilter.h"
#include "itkImageToVTKImageFilter.h"
#include "itkAdaptiveHistogramEqualizationImageFilter.h"

const unsigned int Dimension = 3;

typedef float InputPixelTypeFloat;
typedef itk::Image< InputPixelTypeFloat,Dimension > InputImageTypeFloat;
typedef itk::VTKImageToImageFilter< InputImageTypeFloat > ConvertervtkTOitk;
typedef itk::ImageToVTKImageFilter< InputImageTypeFloat > ConverteritkTOvtk;
typedef itk::AdaptiveHistogramEqualizationImageFilter<InputImageTypeFloat> HistogramEqualizationType;

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpEqualizeHistogram);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpEqualizeHistogram::albaOpEqualizeHistogram(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo	= true;

  m_InputPreserving = true;

  m_Alpha = 0.0;
  m_Beta = 0.0;
  m_Radius[0] = m_Radius[1] = m_Radius[2] = 5;

  m_VolumeInput = NULL;
  m_VolumeOutput = NULL;
}
//----------------------------------------------------------------------------
albaOpEqualizeHistogram::~albaOpEqualizeHistogram()
//----------------------------------------------------------------------------
{
  albaDEL(m_VolumeOutput);
}
//----------------------------------------------------------------------------
bool albaOpEqualizeHistogram::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return ( node != NULL && node->IsA("albaVMEVolumeGray") );
}
//----------------------------------------------------------------------------
albaOp *albaOpEqualizeHistogram::Copy()   
//----------------------------------------------------------------------------
{
  return (new albaOpEqualizeHistogram(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum U_OP_EQUALIZE_HISTOGRAM_ID
{
  ID_MY_OP = MINID,
  // ToDO: add your custom IDs...
  ID_ALPHA,
  ID_BETA,
  ID_RADIUS,
  ID_EXECUTE,
  ID_HISTOGRAM,
  ID_DIALOG_OK,
};
//----------------------------------------------------------------------------
void albaOpEqualizeHistogram::OpRun()   
//----------------------------------------------------------------------------
{
  m_VolumeInput = albaVMEVolumeGray::SafeDownCast(m_Input);

  vtkImageData *sp = vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
  //vtkImageData *im = vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
  if (sp == NULL)
  {
    int answer = wxMessageBox(_("The data will be resampled! Proceed?"),_("Confirm"), wxYES_NO|wxICON_EXCLAMATION , NULL);
    if(answer == wxNO)
    {
      OpStop(OP_RUN_CANCEL);
      return;
    }
    wxBusyInfo wait_info1("Resampling...");
    albaOpVolumeResample *op = new albaOpVolumeResample();
    op->SetInput(m_VolumeInput);
    op->TestModeOn();
    op->OpRun();
    op->AutoSpacing();
    op->Resample();
    albaVMEVolumeGray *volOut=albaVMEVolumeGray::SafeDownCast(op->GetOutput());
    volOut->GetOutput()->Update();
    volOut->Update();

    albaDEL(op);

    m_VolumeInput=volOut;
  }

  m_VolumeInput->Update();

  albaNEW(m_VolumeOutput);
  m_VolumeOutput->SetData(vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData()),m_VolumeInput->GetTimeStamp());
  albaString name = m_VolumeInput->GetName();
  name<<" - Equalized Histogram";
  m_VolumeOutput->SetName(name);
  m_VolumeOutput->ReparentTo(m_Input);
  m_VolumeOutput->Update();

  CreateGui();
  CreateHistogramDialog();
}
//----------------------------------------------------------------------------
void albaOpEqualizeHistogram::CreateGui()
//----------------------------------------------------------------------------
{
  // interface:
  m_Gui = new albaGUI(this);

	m_Gui->Label("");

  // ToDO: add your custom widgets...
  m_Gui->Double(ID_ALPHA,_("Alpha"),&m_Alpha,0.0,1.0);
  m_Gui->Double(ID_BETA,_("Beta"),&m_Beta,0.0,1.0);
  m_Gui->Vector(ID_RADIUS,_("Radius"),m_Radius);
  m_Gui->Divider(1);
  m_Gui->Button(ID_EXECUTE,_("Execute"));
  m_Gui->Button(ID_HISTOGRAM,_("Histogram"));

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	ShowGui();
}
//----------------------------------------------------------------------------
void albaOpEqualizeHistogram::CreateHistogramDialog()
//----------------------------------------------------------------------------
{
  m_Dialog = new albaGUIDialog("Histogram", albaCLOSEWINDOW | albaRESIZABLE);

  m_Histogram = new albaGUIHistogramWidget(m_Gui,-1,wxPoint(0,0),wxSize(400,500),wxTAB_TRAVERSAL,true);
  m_Histogram->SetListener(this);
  vtkImageData *hd = vtkImageData::SafeDownCast(m_VolumeOutput->GetOutput()->GetVTKData());
  m_Histogram->SetData(hd->GetPointData()->GetScalars());

  albaGUI *gui = new albaGUI(this);
  gui->Add(m_Histogram,1);
  gui->AddGui(m_Histogram->GetGui());
  gui->Button(ID_DIALOG_OK,_("OK"));
  gui->FitGui();
  gui->Update();

  m_Dialog->Add(gui,1);
  m_Dialog->SetMinSize(wxSize(600,600));
}
//----------------------------------------------------------------------------
void albaOpEqualizeHistogram::DeleteHistogramDialog()
//----------------------------------------------------------------------------
{
  cppDEL(m_Histogram);
  cppDEL(m_Dialog);
}
//----------------------------------------------------------------------------
void albaOpEqualizeHistogram::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
    case ID_DIALOG_OK:
      {
        m_Dialog->EndModal(wxID_OK);
      }
      break;
    case ID_EXECUTE:
      {
        Algorithm();
      }
      break;
    case ID_HISTOGRAM:
      {
        CreateHistogramDialog();
        m_Dialog->ShowModal();
        DeleteHistogramDialog();
      }
      break;
    case wxOK:
      OpStop(OP_RUN_OK);        
      break;
    case wxCANCEL:
      OpStop(OP_RUN_CANCEL);        
      break;
    }
  }
}
//----------------------------------------------------------------------------
void albaOpEqualizeHistogram::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
  albaEventMacro(albaEvent(this,result));  
}
//----------------------------------------------------------------------------
void albaOpEqualizeHistogram::OpUndo()
//----------------------------------------------------------------------------
{
  if (m_VolumeOutput != NULL)
  {
    m_VolumeOutput->ReparentTo(NULL);
  }
}
//----------------------------------------------------------------------------
void albaOpEqualizeHistogram::OpDo()
//----------------------------------------------------------------------------
{
  if (m_VolumeOutput != NULL)
  {
    m_VolumeOutput->ReparentTo(m_Input);
  }
}
//----------------------------------------------------------------------------
void albaOpEqualizeHistogram::Algorithm()
//----------------------------------------------------------------------------
{
  vtkImageData *im = vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());

  vtkALBASmartPointer<vtkImageCast> vtkImageToFloat;
  vtkImageToFloat->SetOutputScalarTypeToFloat ();
  vtkImageToFloat->SetInputData(im);
  vtkImageToFloat->Modified();
  vtkImageToFloat->Update();

  ConvertervtkTOitk::Pointer vtkTOitk = ConvertervtkTOitk::New();
  vtkTOitk->SetInput( vtkImageToFloat->GetOutput() );
  vtkTOitk->Update();

  HistogramEqualizationType::Pointer histeqFilter = HistogramEqualizationType::New();
  histeqFilter->GetOutput()->ReleaseDataFlagOn(); 
  HistogramEqualizationType::ImageSizeType radius;
  radius[0] = m_Radius[0];
  radius[1] = m_Radius[1];
  radius[2] = m_Radius[2];
  radius.Fill(0);
  histeqFilter->SetRadius(radius);   
  histeqFilter->SetAlpha(m_Alpha);
  histeqFilter->SetBeta(m_Beta); 
  histeqFilter->SetInput(vtkTOitk->GetOutput());
  histeqFilter->Update();

  ConverteritkTOvtk::Pointer itkTOvtk = ConverteritkTOvtk::New();
  itkTOvtk->SetInput( histeqFilter->GetOutput() );
  itkTOvtk->Update();

  vtkALBASmartPointer<vtkImageData> imOut;
  imOut->DeepCopy(itkTOvtk->GetOutput());

  vtkALBASmartPointer<vtkImageToStructuredPoints> imTosp;
  imTosp->SetInputData(imOut);
  imTosp->Update();

  m_VolumeOutput->SetData((vtkImageData *)imTosp->GetOutput(),m_VolumeInput->GetTimeStamp());
  m_VolumeOutput->Update();

}
