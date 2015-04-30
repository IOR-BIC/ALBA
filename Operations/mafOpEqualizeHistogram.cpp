/*=========================================================================

 Program: MAF2
 Module: mafOpEqualizeHistogram
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpEqualizeHistogram.h"
#include "wx/busyinfo.h"
#include "mafNode.h"
#include "mafGUI.h"
#include "mafGUIDialog.h"
#include "mafGUIHistogramWidget.h"
#include "mafVMEVolumeGray.h"
#include "medOpVolumeResample.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFHistogram.h"
#include "vtkImageData.h"
#include "vtkImageCast.h"
#include "vtkPointData.h"
#include "vtkStructuredPoints.h"
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
mafCxxTypeMacro(mafOpEqualizeHistogram);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpEqualizeHistogram::mafOpEqualizeHistogram(const wxString &label) :
mafOp(label)
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
mafOpEqualizeHistogram::~mafOpEqualizeHistogram()
//----------------------------------------------------------------------------
{
  mafDEL(m_VolumeOutput);
}
//----------------------------------------------------------------------------
bool mafOpEqualizeHistogram::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return ( node != NULL && node->IsA("mafVMEVolumeGray") );
}
//----------------------------------------------------------------------------
mafOp *mafOpEqualizeHistogram::Copy()   
//----------------------------------------------------------------------------
{
  return (new mafOpEqualizeHistogram(m_Label));
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
void mafOpEqualizeHistogram::OpRun()   
//----------------------------------------------------------------------------
{
  m_VolumeInput = mafVMEVolumeGray::SafeDownCast(m_Input);

  vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
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
    medOpVolumeResample *op = new medOpVolumeResample();
    op->SetInput(m_VolumeInput);
    op->TestModeOn();
    op->OpRun();
    op->AutoSpacing();
    op->Resample();
    mafVMEVolumeGray *volOut=mafVMEVolumeGray::SafeDownCast(op->GetOutput());
    volOut->GetOutput()->Update();
    volOut->Update();

    mafDEL(op);

    m_VolumeInput=volOut;
  }

  m_VolumeInput->Update();

  mafNEW(m_VolumeOutput);
  m_VolumeOutput->SetData(vtkStructuredPoints::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData()),m_VolumeInput->GetTimeStamp());
  mafString name = m_VolumeInput->GetName();
  name<<" - Equalized Histogram";
  m_VolumeOutput->SetName(name);
  m_VolumeOutput->ReparentTo(m_Input);
  m_VolumeOutput->Update();

  CreateGui();
  CreateHistogramDialog();
}
//----------------------------------------------------------------------------
void mafOpEqualizeHistogram::CreateGui()
//----------------------------------------------------------------------------
{
  // interface:
  m_Gui = new mafGUI(this);

  // ToDO: add your custom widgets...
  m_Gui->Double(ID_ALPHA,_("Alpha"),&m_Alpha,0.0,1.0);
  m_Gui->Double(ID_BETA,_("Beta"),&m_Beta,0.0,1.0);
  m_Gui->Vector(ID_RADIUS,_("Radius"),m_Radius);
  m_Gui->Divider(1);
  m_Gui->Button(ID_EXECUTE,_("Execute"));
  m_Gui->Button(ID_HISTOGRAM,_("Histogram"));
  m_Gui->Divider(1);
  m_Gui->OkCancel();

  ShowGui();
}
//----------------------------------------------------------------------------
void mafOpEqualizeHistogram::CreateHistogramDialog()
//----------------------------------------------------------------------------
{
  m_Dialog = new mafGUIDialog("Histogram", mafCLOSEWINDOW | mafRESIZABLE);

  m_Histogram = new mafGUIHistogramWidget(m_Gui,-1,wxPoint(0,0),wxSize(400,500),wxTAB_TRAVERSAL,true);
  m_Histogram->SetListener(this);
  m_Histogram->SetRepresentation(vtkMAFHistogram::BAR_REPRESENTATION);
  vtkImageData *hd = vtkImageData::SafeDownCast(m_VolumeOutput->GetOutput()->GetVTKData());
  hd->Update();
  m_Histogram->SetData(hd->GetPointData()->GetScalars());

  mafGUI *gui = new mafGUI(this);
  gui->Add(m_Histogram,1);
  gui->AddGui(m_Histogram->GetGui());
  gui->Button(ID_DIALOG_OK,_("OK"));
  gui->FitGui();
  gui->Update();

  m_Dialog->Add(gui,1);
  m_Dialog->SetMinSize(wxSize(600,600));
}
//----------------------------------------------------------------------------
void mafOpEqualizeHistogram::DeleteHistogramDialog()
//----------------------------------------------------------------------------
{
  cppDEL(m_Histogram);
  cppDEL(m_Dialog);
}
//----------------------------------------------------------------------------
void mafOpEqualizeHistogram::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
void mafOpEqualizeHistogram::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
  mafEventMacro(mafEvent(this,result));  
}
//----------------------------------------------------------------------------
void mafOpEqualizeHistogram::OpUndo()
//----------------------------------------------------------------------------
{
  if (m_VolumeOutput != NULL)
  {
    m_VolumeOutput->ReparentTo(NULL);
  }
}
//----------------------------------------------------------------------------
void mafOpEqualizeHistogram::OpDo()
//----------------------------------------------------------------------------
{
  if (m_VolumeOutput != NULL)
  {
    m_VolumeOutput->ReparentTo(m_Input);
  }
}
//----------------------------------------------------------------------------
void mafOpEqualizeHistogram::Algorithm()
//----------------------------------------------------------------------------
{
  vtkImageData *im = vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
  im->Update();

  vtkMAFSmartPointer<vtkImageCast> vtkImageToFloat;
  vtkImageToFloat->SetOutputScalarTypeToFloat ();
  vtkImageToFloat->SetInput(im);
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

  vtkMAFSmartPointer<vtkImageData> imOut;
  imOut->DeepCopy(itkTOvtk->GetOutput());
  imOut->Update();

  vtkMAFSmartPointer<vtkImageToStructuredPoints> imTosp;
  imTosp->SetInput(imOut);
  imTosp->Update();

  m_VolumeOutput->SetData(imTosp->GetOutput(),m_VolumeInput->GetTimeStamp());
  m_VolumeOutput->Update();

}
