/*=========================================================================
Program:   LHP
Module:    $RCSfile: medOpSegmentation.cpp,v $
Language:  C++
Date:      $Date: 2011-07-18 12:11:35 $
Version:   $Revision: 1.1.2.8 $
Authors:   Eleonora Mambrini - Matteo Giacomoni, Gianluigi Crimi
==========================================================================
Copyright (c) 2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#include "medDefines.h" 

//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "medOpSegmentation.h"
#include "wx/busyinfo.h"

#include "mafMatrix.h"
#include "mafAbsMatrixPipe.h"
#include "mafAction.h"
#include "mafDeviceManager.h"
#include "mafEventSource.h"
#include "mafGUI.h"
#include "mafGUIButton.h"
#include "mafGUIDialog.h"
#include "mafGUIFloatSlider.h"
#include "mafGUILutPreset.h"
#include "mafGUILutSlider.h"
#include "medGUILutHistogramSwatch.h"
#include "mafGUIRollOut.h"
#include "mafGUIValidator.h"
#include "mafInteractor.h"
#include "mafInteractionFactory.h"
#include "mafInteractorPicker.h"
#include "mafInteractorCompositorMouse.h"
#include "mafInteractorCameraMove.h"
#include "mafInteractorSER.h"
#include "mafInteractorPER.h"
#include "mafInteractorGenericMouse.h"
#include "mafRWI.h"
#include "mafTagArray.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEImage.h"
#include "mafGUILutSlider.h"
#include "mafVMESurface.h"

#include "mmaVolumeMaterial.h"
#include "mmaMaterial.h"

#include "medDeviceButtonsPadMouseDialog.h"
#include "medViewSliceGlobal.h"
#include "medVMESegmentationVolume.h"

#include "medInteractorPERScalarInformation.h"
#include "medInteractorSegmentationPicker.h"

#include "vtkDoubleArray.h"
#include "vtkMath.h"
#include "vtkImageCast.h"
#include "vtkLookupTable.h"
#include "vtkPoints.h"
#include "vtkRectilinearGrid.h"
#include "vtkStructuredPoints.h"
#include "vtkSphereSource.h"
#include "vtkStructuredPointsWriter.h"

#include "vtkRenderer.h"
#include "vtkTextMapper.h"
#include "vtkActor2D.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"
#include "vtkPointData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"

#include "itkImage.h"
#include "itkImageToVTKImageFilter.h"
#include "itkVTKImageToImageFilter.h"
#include "itkVotingBinaryHoleFillingImageFilter.h"
#include "itkVotingBinaryIterativeHoleFillingImageFilter.h"

#include "medInteractorPERBrushFeedback.h"
#include "mafVMEItemVTK.h"
#include "mafTransformFrame.h"
#include "medOpVolumeResample.h"

#define max(a,b)(((a) > (b)) ? (a) : (b))
#define round(x) (x<0?ceil((x)-0.5):floor((x)+0.5))

#define SPACING_PERCENTAGE_BOUNDS 0.1

enum PLANE_TYPE
{
  XY = 0,
  XZ,
  YZ,
};

enum THRESHOLD_TYPE
{
  GLOBAL = 0,
  RANGE,
};

typedef  itk::Image< float, 3> RealImage;
typedef  itk::Image< unsigned char, 3> UCharImage;

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpSegmentation);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpSegmentation::medOpSegmentation(const wxString &label) : mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo	= true;  

  m_Volume              = NULL;
  m_VolumeDimensions[0] = m_VolumeDimensions[1] = m_VolumeDimensions[2] = 0;
  m_VolumeSpacing[0] = m_VolumeSpacing[1] = m_VolumeSpacing[2] = 0.0;
  m_VolumeBounds[0] = m_VolumeBounds[1] = m_VolumeBounds[2] = m_VolumeBounds[3] = m_VolumeBounds[4] = m_VolumeBounds[5] = 0.0;
  m_VolumeParametersInitialized = false;

  m_CurrentSliceIndex   = 1;
  m_CameraPositionId    = CAMERA_OS_Z;
  m_CurrentSlicePlane = XY;

  m_NumSliceSliderEvents = 0;
  m_SliceSlider = NULL;


  m_CurrentOperation = 0;
  m_OldOperation     = 0;

  m_Dialog       = NULL;
  m_View         = NULL;      
  m_OkButton     = NULL;
  m_CancelButton = NULL;

  m_LutWidget = NULL;
  m_LutSlider = NULL;
  m_ColorLUT  = NULL;

  m_SegmentationColorLUT = NULL;
  m_ManualColorLUT = NULL;

  m_OldVolumeParent = NULL;

  m_ThresholdVolume           = NULL;
  m_OutputVolume              = NULL;

  m_SER = NULL;
  m_DeviceManager = NULL;


  for(int i=0;i<4;i++)
  { 
    m_SegmentationOperationsRollOut[i] = NULL;
    m_SegmentationOperationsGui[i] = NULL;
  }

  m_SegmentatedVolume = NULL;

  //////////////////////////////////////////////////////////////////////////
  //Manual initializations
  //////////////////////////////////////////////////////////////////////////

  m_ManualSegmentationMode = MANUAL_SEGMENTATION_MOVE;
  m_ManualEditingActionComboBox = NULL;
  m_ManualBrushShapeRadioBox = NULL;
  m_ManualBrushShape = CIRCLE_BRUSH_SHAPE;
  m_ManualBrushSize = 0;
  m_ManualRefinementRegionsSize = 1;

  m_ManualBrushSizeText = NULL;
  m_ManualBrushSizeSlider = NULL;

  m_ManualRefinementComboBox = NULL;
  m_ManualRefinementRegionSizeText = NULL;

  m_ManualSelectionScalarValue = 100;

  m_ManualPicker = NULL;
  m_ManualContinuousPickingOn = 0;

  m_ManualVolumeMask = NULL;
  m_ManualVolumeSlice = NULL;

  m_ManualPER = NULL;
  //m_ManualStandardPER = NULL;

  m_ManualModifiedWithoutApplied = false;
  m_ManualUndoList.clear();
  m_ManualRedoList.clear();

  m_ManualUndoCounter = 1;
  //////////////////////////////////////////////////////////////////////////
  //Automatic initializations
  //////////////////////////////////////////////////////////////////////////
  m_AutomaticThreshold = 0.0;
  m_AutomaticUpperThreshold = 0.0;
  m_AutomaticLabel = 0.0;
  m_AutomaticRanges.clear();
  m_AutomaticListOfRange = NULL;
  m_AutomaticThresholdTextActor = NULL;
  m_AutomaticThresholdTextMapper = NULL;
  m_AutomaticSliceTextActor = NULL;
  m_AutomaticSliceTextMapper = NULL;
  m_AutomaticScalarTextActor = NULL;
  m_AutomaticScalarTextMapper = NULL;
  m_AutomaticRangeSlider = NULL;
  m_AutomaticPicker = NULL;
  m_AutomaticPER = NULL;
  m_AutomaticGlobalThreshold = GLOBAL;

  //////////////////////////////////////////////////////////////////////////
  //Refinement initializations
  //////////////////////////////////////////////////////////////////////////
  m_RefinementVolumeMask = NULL;

  m_RefinementUndoList.clear();
  m_RefinementRedoList.clear();
  //////////////////////////////////////////////////////////////////////////

}
//----------------------------------------------------------------------------
medOpSegmentation::~medOpSegmentation()
//----------------------------------------------------------------------------
{

  if(m_ThresholdVolume)
  {
    mafDEL(m_ThresholdVolume);
  }
  if(m_SegmentatedVolume)
  {
    mafDEL(m_SegmentatedVolume);
  }

  if(m_ManualVolumeSlice)
  {
    mafDEL(m_ManualVolumeSlice);
  }

  if(m_ManualVolumeMask)
  {
    mafDEL(m_ManualVolumeMask);
  }

  ResetManualUndoList();

  ResetManualRedoList();

  ResetRefinementUndoList();

  ResetRefinementRedoList();

  Superclass;
}
//----------------------------------------------------------------------------
bool medOpSegmentation::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsA("mafVMEVolumeGray") );//&& mafVME::SafeDownCast(node)->GetOutput()->GetVTKData()->IsA("vtkStructuredPoints"));
}
//----------------------------------------------------------------------------
mafOp *medOpSegmentation::Copy()   
//----------------------------------------------------------------------------
{
  return (new medOpSegmentation(m_Label));
}
//----------------------------------------------------------------------------
void medOpSegmentation::OpRun()   
//----------------------------------------------------------------------------
{
  
  //////////////////////////////////////////////////////////////////////////
  //Initialize of the volume matrix to the indentity
  //////////////////////////////////////////////////////////////////////////
  m_Volume=mafVMEVolumeGray::SafeDownCast(m_Input);
  m_Volume->Update();
  m_Matrix = m_Volume->GetMatrixPipe()->GetMatrix();
  mafMatrix matrixNoTransf;
  matrixNoTransf.Identity();

  m_Volume->SetAbsMatrix(matrixNoTransf);
  m_Volume->GetOutput()->Update();
  //////////////////////////////////////////////////////////////////////////

  InitVolumeDimensions();

  // interface:
  CreateOpDialog();
  InitializeViewSlice();
  InitGui();

  m_OldVolumeParent = m_Volume->GetParent();
  m_Volume->SetParent(m_Volume->GetRoot());

  m_View->VmeAdd(m_Volume);
  m_View->VmeShow(m_Volume, true);
  m_View->UpdateSlicePos(0.0);
  m_View->CameraReset();
  m_View->CameraUpdate();
  
  OnNextStep();
  
  int result = m_Dialog->ShowModal() == wxID_OK ? OP_RUN_OK : OP_RUN_CANCEL;

  DeleteOpDialog();
  m_Volume->SetMatrix(m_Matrix);
  m_Volume->GetOutput()->Update();
  OpStop(result);
}
//----------------------------------------------------------------------------
void medOpSegmentation::OpDo()
//----------------------------------------------------------------------------
{
  m_ThresholdVolume->ReparentTo(m_Volume->GetParent());
  m_ThresholdVolume->SetName(_("Segmentation Output"));
  lutPreset(4,m_ThresholdVolume->GetMaterial()->m_ColorLut);
  m_SegmentatedVolume->ReparentTo(m_Volume->GetParent());
  mafOp::OpDo();
}
//----------------------------------------------------------------------------
void medOpSegmentation::OpUndo()
//----------------------------------------------------------------------------
{
  m_ThresholdVolume->ReparentTo(NULL);
  m_SegmentatedVolume->ReparentTo(NULL);
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medOpSegmentation::RemoveVMEs()
//----------------------------------------------------------------------------
{

#ifndef _DEBUG
  if(m_ManualVolumeSlice)
  {
    m_ManualVolumeSlice->ReparentTo(NULL);
    mafDEL(m_ManualVolumeSlice);
  }
#endif

}
//----------------------------------------------------------------------------
void medOpSegmentation::OpStop(int result)
//----------------------------------------------------------------------------
{
  RemoveVMEs();

  if (result == OP_RUN_CANCEL)
  {
    if(m_ThresholdVolume)
    {
      m_ThresholdVolume->ReparentTo(NULL);
      mafDEL(m_ThresholdVolume);
    }
    if(m_SegmentatedVolume)
    {
      m_SegmentatedVolume->ReparentTo(NULL);
      mafDEL(m_SegmentatedVolume);
    }
  }

  mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void medOpSegmentation::CreateOpDialog()
//----------------------------------------------------------------------------
{
  wxBusyCursor wait;
  wxPoint defPos = wxDefaultPosition;
  wxSize defSize = wxDefaultSize;

  m_Dialog = new mafGUIDialog(m_Label, mafCLOSEWINDOW|mafRESIZABLE);  
  m_Dialog->SetListener(this);

  m_GuiDialog = new mafGUI(this);
  m_GuiDialog->Reparent(m_Dialog);

  //Change default frame to our dialog
  wxWindow* oldFrame = mafGetFrame();
  mafSetFrame(m_Dialog);

  //Create rendering view   
  m_View = new medViewSliceGlobal("Volume Slice",CAMERA_OS_Z,true,false,false,0,false,false);  
  m_View->Create();
  m_View->GetGui();

  m_View->VmeAdd(m_Input->GetRoot()); //add Root

  //////////////////////////////////////////////////////////////////////////
  //Label to indicate the threshold of the slice
  //////////////////////////////////////////////////////////////////////////
  vtkNEW(m_AutomaticThresholdTextMapper);
  m_AutomaticThresholdTextMapper->SetInput(_("Threshold  = "));
  m_AutomaticThresholdTextMapper->GetTextProperty()->SetColor(1.0,0.0,0.0);
  m_AutomaticThresholdTextMapper->GetTextProperty()->AntiAliasingOff();

  vtkNEW(m_AutomaticThresholdTextActor);
  m_AutomaticThresholdTextActor->SetMapper(m_AutomaticThresholdTextMapper);
  m_AutomaticThresholdTextActor->SetPosition(3,5);
  m_AutomaticThresholdTextActor->GetProperty()->SetColor(1.0,0.0,0.0);

  m_View->GetFrontRenderer()->AddActor(m_AutomaticThresholdTextActor);
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Label to indicate the scalar value
  //////////////////////////////////////////////////////////////////////////
  vtkNEW(m_AutomaticScalarTextMapper);
  m_AutomaticScalarTextMapper->SetInput(_("Scalar  = "));
  m_AutomaticScalarTextMapper->GetTextProperty()->SetColor(0.0,1.0,0.0);
  m_AutomaticScalarTextMapper->GetTextProperty()->AntiAliasingOff();

  vtkNEW(m_AutomaticScalarTextActor);
  m_AutomaticScalarTextActor->SetMapper(m_AutomaticScalarTextMapper);
  m_AutomaticScalarTextActor->SetPosition(550,620);
  m_AutomaticScalarTextActor->GetProperty()->SetColor(0.0,1.0,0.0);

  m_View->GetFrontRenderer()->AddActor(m_AutomaticScalarTextActor);
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Label to indicate the current slice
  //////////////////////////////////////////////////////////////////////////
  vtkNEW(m_AutomaticSliceTextMapper);
  mafString text = "Slice = ";
  text<<m_CurrentSliceIndex;
  text<<" of ";
  if (m_CurrentSlicePlane == XY)
  {
    text<<m_VolumeDimensions[2];
  } 
  else if (m_CurrentSlicePlane == XZ)
  {
    text<<m_VolumeDimensions[1];
  }
  else if (m_CurrentSlicePlane == YZ)
  {
    text<<m_VolumeDimensions[0];
  }
  m_AutomaticSliceTextMapper->SetInput(text.GetCStr());
  m_AutomaticSliceTextMapper->GetTextProperty()->SetColor(1.0,1.0,0.0);
  m_AutomaticSliceTextMapper->GetTextProperty()->AntiAliasingOff();

  vtkNEW(m_AutomaticSliceTextActor);
  m_AutomaticSliceTextActor->SetMapper(m_AutomaticSliceTextMapper);
  m_AutomaticSliceTextActor->SetPosition(550,5);
  m_AutomaticSliceTextActor->GetProperty()->SetColor(1.0,1.0,0.0);

  m_View->GetFrontRenderer()->AddActor(m_AutomaticSliceTextActor);
  //////////////////////////////////////////////////////////////////////////

  //InitializeInteractors();

  mafSetFrame(oldFrame);

  //Display window      
  m_View->m_Rwi->SetSize(0,0,650,650);
  m_View->m_Rwi->Show(true);

  m_LutSlider = new mafGUILutSlider(m_Dialog,-1,wxPoint(0,0),wxSize(650,24));
  m_LutSlider->SetListener(this);
  m_LutSlider->SetSize(650,24);
  m_LutSlider->SetMinSize(wxSize(650,24));


  wxBoxSizer * hSz1 = new wxBoxSizer(wxHORIZONTAL);

  wxBoxSizer * vSz1 = new wxBoxSizer(wxVERTICAL);
  vSz1->Add(m_View->m_Rwi->m_RwiBase, 1, wxEXPAND | wxALL, 5 );
  vSz1->Add(m_LutSlider, 0,wxLEFT );

  m_LutSlider->Update();


  wxBoxSizer * vSz2 = new wxBoxSizer(wxVERTICAL);

  mafVMEOutputVolume *volumeOutput = mafVMEOutputVolume::SafeDownCast(m_Volume->GetOutput());
  m_ColorLUT = volumeOutput->GetMaterial()->m_ColorLut;

  m_LutWidget = new medGUILutHistogramSwatch(m_GuiDialog,m_GuiDialog->GetWidgetId(ID_LUT_CHOOSER), "LUT", m_Volume->GetOutput()->GetVTKData(), m_Volume->GetMaterial() );
  m_LutWidget->SetEditable(true);

  /////////////////////////////////////////////////////
  m_GuiDialog->Divider();
  m_GuiDialog->Divider();
  m_GuiDialog->Divider(0);

  m_GuiDialog->SetMinSize(wxSize(200,650));

  vSz2->Add(m_GuiDialog,0,wxRIGHT);

  hSz1->Add(vSz1,0,wxRIGHT);
  hSz1->Add(vSz2,0,wxRIGHT);

  wxBoxSizer * hSz2 = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer * hSz3 = new wxBoxSizer(wxHORIZONTAL);
  wxPoint p = wxDefaultPosition;
  m_SnippetsLabel = new wxStaticText(m_Dialog, -1, "", p, wxSize(500,18), wxALIGN_LEFT | wxST_NO_AUTORESIZE );
  hSz3->Add(m_SnippetsLabel,0,wxEXPAND | wxALL);

  m_OkButton = new mafGUIButton(m_Dialog,ID_OK,_("Ok"),defPos);
  m_OkButton->SetListener(this);
  m_OkButton->SetValidator(mafGUIValidator(this,ID_OK,m_OkButton));

  m_CancelButton = new mafGUIButton(m_Dialog,ID_CANCEL,_("Cancel"),defPos);
  m_CancelButton->SetListener(this);
  m_CancelButton->SetValidator(mafGUIValidator(this,ID_CANCEL,m_CancelButton));

  /*m_LoadSegmentationButton = new mafGUIButton(m_Dialog,ID_LOAD_SEGMENTATION,_("Load"),defPos);
  m_LoadSegmentationButton->SetListener(this);
  m_LoadSegmentationButton->SetValidator(mafGUIValidator(this,ID_LOAD_SEGMENTATION,m_LoadSegmentationButton));*/

  hSz2->Add(m_OkButton,0,wxEXPAND | wxALL);
  hSz2->Add(m_CancelButton,0,wxEXPAND | wxALL);
  //hSz2->Add(m_LoadSegmentationButton,0,wxEXPAND | wxALL);

  wxBoxSizer * hSzDiv = new wxBoxSizer(wxHORIZONTAL);
  hSzDiv->SetMinSize(wxSize(10,10));
  m_Dialog->Add(hSz1);
  m_Dialog->Add(hSzDiv);
  m_Dialog->Add(hSz3);
  m_Dialog->Add(hSz2);

  m_GuiDialog->Divider();

  CreateSliceNavigationGui();

  wxString choices[4];
  choices[0] = wxString("Automatic");
  choices[1] = wxString("Manual");
  choices[2] = wxString("Refinement");
  choices[3] = wxString("Load");

  CreateAutoSegmentationGui();
  CreateManualSegmentationGui();
  CreateRefinementGui();
  CreateLoadSegmentationGui();

  InitManualSegmentationGui();

  m_SegmentationOperationsRollOut[AUTOMATIC_SEGMENTATION]   = m_GuiDialog->RollOut(ID_AUTO_SEGMENTATION, "Automatic Segmentation", m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION], false);
  m_SegmentationOperationsRollOut[MANUAL_SEGMENTATION]      = m_GuiDialog->RollOut(ID_MANUAL_SEGMENTATION, "Manual Segmentation", m_SegmentationOperationsGui[MANUAL_SEGMENTATION], false);
  m_SegmentationOperationsRollOut[REFINEMENT_SEGMENTATION]  = m_GuiDialog->RollOut(ID_REFINEMENT, "Segmentation Refinement", m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION], false);
  m_SegmentationOperationsRollOut[LOAD_SEGMENTATION]        = m_GuiDialog->RollOut(ID_LOAD_SEGMENTATION, "Load Segmentation", m_SegmentationOperationsGui[LOAD_SEGMENTATION], false);

  m_GuiDialog->Enable(ID_AUTO_SEGMENTATION, false);
  m_GuiDialog->Enable(ID_MANUAL_SEGMENTATION,false);
  m_GuiDialog->Enable(ID_REFINEMENT,false);
  m_GuiDialog->Enable(ID_LOAD_SEGMENTATION,false);

  m_GuiDialog->TwoButtons(ID_BUTTON_PREV,ID_BUTTON_NEXT,_("Prev"),_("Next"));

  m_GuiDialog->FitGui();
  m_GuiDialog->Update();

  m_GuiDialog->Enable(ID_BUTTON_PREV,false);
  m_GuiDialog->Enable(ID_BUTTON_NEXT,true);

  int x_pos,y_pos,w,h;
  mafGetFrame()->GetPosition(&x_pos,&y_pos);
  m_Dialog->GetSize(&w,&h);
  m_Dialog->SetSize(x_pos+5,y_pos+5,w+50,h+50);

  UpdateWindowing();

  OnChangeThresholdType();

}
//----------------------------------------------------------------------------
void medOpSegmentation::DeleteOpDialog()
//----------------------------------------------------------------------------
{
  if (m_ThresholdVolume)
  {
    m_View->VmeShow(m_ThresholdVolume,false);
    m_View->VmeRemove(m_ThresholdVolume);
  }
  if(m_ManualVolumeSlice)
  {
    m_View->VmeShow(m_ManualVolumeSlice,false);
    m_View->VmeRemove(m_ManualVolumeSlice);
  }
  m_Volume->ReparentTo(m_OldVolumeParent);
  m_Volume->SetBehavior(m_OldBehavior);
  m_Volume->Update();
  m_View->VmeShow(m_Volume,false);
  m_View->VmeRemove(m_Volume);

  //////////////////////////////////////////////////////////////////////////
  //Remove the threshold label
  //////////////////////////////////////////////////////////////////////////
  if (m_AutomaticThresholdTextActor)
  {
    m_View->GetFrontRenderer()->RemoveActor(m_AutomaticThresholdTextActor);
  }
  vtkDEL(m_AutomaticThresholdTextActor);
  vtkDEL(m_AutomaticThresholdTextMapper);
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Remove the scalar label
  //////////////////////////////////////////////////////////////////////////
  if (m_AutomaticScalarTextActor)
  {
    m_View->GetFrontRenderer()->RemoveActor(m_AutomaticScalarTextActor);
  }
  vtkDEL(m_AutomaticScalarTextActor);
  vtkDEL(m_AutomaticScalarTextMapper);
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Remove the slice label
  //////////////////////////////////////////////////////////////////////////
  if (m_AutomaticSliceTextActor)
  {
    m_View->GetFrontRenderer()->RemoveActor(m_AutomaticSliceTextActor);
  }
  vtkDEL(m_AutomaticSliceTextActor);
  vtkDEL(m_AutomaticSliceTextMapper);
  //////////////////////////////////////////////////////////////////////////

  cppDEL(m_View);      
  cppDEL(m_OkButton);
  cppDEL(m_CancelButton);
  cppDEL(m_LutWidget);
  cppDEL(m_LutSlider);
  cppDEL(m_ManualBrushShapeRadioBox);
  cppDEL(m_ManualBrushSizeSlider);

  for(int i=0;i<4;i++)
  {
    cppDEL(m_SegmentationOperationsRollOut[i]);
    cppDEL(m_SegmentationOperationsGui[i]);
  }

  cppDEL(m_SliceSlider);

  cppDEL(m_GuiDialog);
  cppDEL(m_Dialog);

  mafDEL(m_ManualPicker);
  mafDEL(m_AutomaticPicker);

  if (m_DeviceManager)
  {
    m_DeviceManager->Shutdown();
  }
  mafDEL(m_DeviceManager);
  mafDEL(m_AutomaticPER);
  mafDEL(m_ManualPER);
  mafDEL(m_SER);

}
//----------------------------------------------------------------------------
bool medOpSegmentation::Resample()
//----------------------------------------------------------------------------
{

  int answer = wxMessageBox(_("The data will be resampled! Proceed?"),_("Confirm"), wxYES_NO|wxICON_EXCLAMATION , NULL);
  if(answer == wxNO)
  {
    m_Dialog->EndModal(wxID_CANCEL);
    return false;
  }
  bool checkSpacing = true;

  if ((m_VolumeSpacing[0]/(m_VolumeBounds[1] - m_VolumeBounds[0]))*100 < SPACING_PERCENTAGE_BOUNDS)
  {
    checkSpacing = false;
  }
  if ((m_VolumeSpacing[1]/(m_VolumeBounds[3] - m_VolumeBounds[2]))*100 < SPACING_PERCENTAGE_BOUNDS)
  {
    checkSpacing = false;
  }
  if ((m_VolumeSpacing[2]/(m_VolumeBounds[5] - m_VolumeBounds[4]))*100 < SPACING_PERCENTAGE_BOUNDS)
  {
    checkSpacing = false;
  }

  if (!checkSpacing)
  {
    answer = wxMessageBox( "Spacing values are too little and could generate memory problems - Continue?", "Warning", wxYES_NO, NULL);
    if (answer == wxNO)
    {
      return false;
    }
  }

  m_View->VmeShow(m_Volume,false);
  m_View->VmeRemove(m_Volume);
  wxBusyInfo wait_info1("Resampling...");
  medOpVolumeResample *op = new medOpVolumeResample();
  op->SetInput(m_Volume);
  op->TestModeOn();
  op->OpRun();
  op->SetSpacing(m_VolumeSpacing);
  op->Resample();
  op->OpDo();
  mafVMEVolumeGray *volOut=mafVMEVolumeGray::SafeDownCast(op->GetOutput());
  volOut->GetOutput()->Update();
  volOut->Update();
  mafDEL(op);

  m_Volume = volOut;

  m_View->VmeAdd(m_Volume);
  m_View->VmeShow(m_Volume,true);

  return true;
}
//----------------------------------------------------------------------------
bool medOpSegmentation::Refinement()
//----------------------------------------------------------------------------
{

  bool checkRadius = true;

  if ( m_CurrentSlicePlane == XY && (m_RefinementRegionsSize>m_VolumeDimensions[0] || m_RefinementRegionsSize>m_VolumeDimensions[1]) )
  {
    checkRadius = false;
  }
  if ( m_CurrentSlicePlane == YZ && (m_RefinementRegionsSize>m_VolumeDimensions[1] || m_RefinementRegionsSize>m_VolumeDimensions[1]) )
  {
    checkRadius = false;
  }
  if ( m_CurrentSlicePlane == XZ && (m_RefinementRegionsSize>m_VolumeDimensions[0] || m_RefinementRegionsSize>m_VolumeDimensions[2]) )
  {
    checkRadius = false;
  }

  if(! checkRadius)
  {
    wxMessageBox("Region size is bigger than slice dimension - Choose another value.");
    return false;
  }

  // threshold values empirically assigned

  if( m_RefinementRegionsSize > 5)
    checkRadius = false;

  if( m_RefinementRegionsSize>=3 && (m_RefinementIterative || m_RefinementEverySlice) )
    checkRadius = false;

  if (!checkRadius)
  {
    int answer = wxMessageBox( "Region size is too big, it could take a long time  - Continue?", "Warning", wxYES_NO, NULL);
    if (answer == wxNO)
    {
      return false;
    }
  }

  wxBusyCursor wait_cursor;
  wxBusyInfo wait(_("Wait! The algorithm could take long time!"));

  vtkDataSet *inputDataSet = vtkDataSet::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_ThresholdVolume)->GetOutput()->GetVTKData());

  if (inputDataSet)
  {
    inputDataSet->Update();

    long progress = 0;
    mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));

    vtkMAFSmartPointer<vtkUnsignedCharArray> newScalars;
    newScalars->SetName("SCALARS");
    newScalars->SetNumberOfTuples(m_VolumeDimensions[0]*m_VolumeDimensions[1]*m_VolumeDimensions[2]);

    double point[3];
    inputDataSet->GetPoint(m_CurrentSliceIndex*m_VolumeDimensions[0]*m_VolumeDimensions[1], point);

    vtkDataArray *inputScalars = inputDataSet->GetPointData()->GetScalars();
    vtkMAFSmartPointer<vtkUnsignedCharArray> scalars;
    scalars->SetName("SCALARS");
    scalars->SetNumberOfTuples(m_VolumeDimensions[0]*m_VolumeDimensions[1]);

    if(m_RefinementEverySlice)
    {
      for (int i= 0;i<m_VolumeDimensions[2];i++)
      {
        progress = (i*100/m_VolumeDimensions[2]);
        mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));

        for (int k=0;k<(m_VolumeDimensions[0]*m_VolumeDimensions[1]);k++)
        {
          unsigned char value = inputScalars->GetTuple1(k+i*(m_VolumeDimensions[0]*m_VolumeDimensions[1]));
          scalars->SetTuple1(k,value);
        }

        vtkMAFSmartPointer<vtkStructuredPoints> im;
        im->SetDimensions(m_VolumeDimensions[0],m_VolumeDimensions[1],1);
        im->SetSpacing(m_VolumeSpacing[0],m_VolumeSpacing[1],0.0);
        im->GetPointData()->AddArray(scalars);
        im->GetPointData()->SetActiveScalars("SCALARS");
        im->SetScalarTypeToUnsignedChar();
        im->Update();

        vtkMAFSmartPointer<vtkStructuredPoints> filteredImage;
        if(ApplyRefinementFilter(im, filteredImage) && filteredImage)
        {
          vtkDataArray *binaryScalars = filteredImage->GetPointData()->GetScalars();

          for (int k=0;k<filteredImage->GetNumberOfPoints();k++)
          {
            unsigned char value = binaryScalars->GetTuple1(k);
            newScalars->SetTuple1(k+i*(m_VolumeDimensions[0]*m_VolumeDimensions[1]),value);
          }
        }
      }

    }
    else
    {
      for (int i= 0;i<m_VolumeDimensions[2];i++)
      {
        if(i != m_CurrentSliceIndex-1)
          //if(i != zID)
        {
          for (int k=0;k<(m_VolumeDimensions[0]*m_VolumeDimensions[1]);k++)
          {
            unsigned char value = inputScalars->GetTuple1(k+i*(m_VolumeDimensions[0]*m_VolumeDimensions[1]));
            newScalars->SetTuple1(k+i*(m_VolumeDimensions[0]*m_VolumeDimensions[1]),value);
          }
        }
        else
        {
          for (int k=0;k<(m_VolumeDimensions[0]*m_VolumeDimensions[1]);k++)
          {
            unsigned char value = inputScalars->GetTuple1(k+i*(m_VolumeDimensions[0]*m_VolumeDimensions[1]));
            scalars->SetTuple1(k,value);
          }

          vtkMAFSmartPointer<vtkStructuredPoints> im;
          im->SetDimensions(m_VolumeDimensions[0],m_VolumeDimensions[1],1);
          im->SetSpacing(m_VolumeSpacing[0],m_VolumeSpacing[1],0.0);
          im->GetPointData()->AddArray(scalars);
          im->GetPointData()->SetActiveScalars("SCALARS");
          im->SetScalarTypeToUnsignedChar();
          im->Update();

          vtkMAFSmartPointer<vtkStructuredPoints> filteredImage;
          if(ApplyRefinementFilter(im, filteredImage) && filteredImage)
          {
            vtkDataArray *binaryScalars = filteredImage->GetPointData()->GetScalars();

            for (int k=0;k<filteredImage->GetNumberOfPoints();k++)
            {
              unsigned char value = binaryScalars->GetTuple1(k);
              newScalars->SetTuple1(k+i*(m_VolumeDimensions[0]*m_VolumeDimensions[1]),value);
            }
          }

        }

      }

    }    

    if (inputDataSet->IsA("vtkStructuredPoints"))
    {
      vtkMAFSmartPointer<vtkStructuredPoints> newSP;
      newSP->CopyStructure(vtkStructuredPoints::SafeDownCast(inputDataSet));
      newSP->Update();
      newSP->GetPointData()->AddArray(newScalars);
      newSP->GetPointData()->SetActiveScalars("SCALARS");
      newSP->SetScalarTypeToUnsignedChar();
      newSP->Update();

      m_ThresholdVolume->SetData(newSP,mafVME::SafeDownCast(m_Volume)->GetTimeStamp());
      vtkStructuredPoints *spVME = vtkStructuredPoints::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_ThresholdVolume)->GetOutput()->GetVTKData());
      spVME->Update();

    }

    else
    {
      vtkMAFSmartPointer<vtkRectilinearGrid> newRG;
      newRG->CopyStructure(vtkRectilinearGrid::SafeDownCast(inputDataSet));
      newRG->Update();
      newRG->GetPointData()->AddArray(newScalars);
      newRG->GetPointData()->SetActiveScalars("SCALARS");
      newRG->Update();

      m_ThresholdVolume->SetData(newRG,mafVME::SafeDownCast(m_Volume)->GetTimeStamp());
      vtkRectilinearGrid *rgVME = vtkRectilinearGrid::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_ThresholdVolume)->GetOutput()->GetVTKData());
      rgVME->Update();
    }

    m_ThresholdVolume->Update();

    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));


    m_View->VmeShow(m_ThresholdVolume,false);
    m_View->VmeShow(m_ThresholdVolume,true);

    m_View->CameraUpdate();
  }

  return true;
}


//----------------------------------------------------------------------------
bool medOpSegmentation::ManualVolumeSliceRefinement()
//----------------------------------------------------------------------------
{

  bool checkRadius = true;

  if ( m_CurrentSlicePlane == XY && (m_ManualRefinementRegionsSize>m_VolumeDimensions[0] || m_ManualRefinementRegionsSize>m_VolumeDimensions[1]) )
  {
    checkRadius = false;
  }
  if ( m_CurrentSlicePlane == YZ && (m_ManualRefinementRegionsSize>m_VolumeDimensions[1] || m_ManualRefinementRegionsSize>m_VolumeDimensions[1]) )
  {
    checkRadius = false;
  }
  if ( m_CurrentSlicePlane == XZ && (m_ManualRefinementRegionsSize>m_VolumeDimensions[0] || m_ManualRefinementRegionsSize>m_VolumeDimensions[2]) )
  {
    checkRadius = false;
  }

  if(! checkRadius)
  {
    wxMessageBox("Region size is bigger than slice dimension - Choose another value.");
    return false;
  }

  // threshold values empirically assigned

  if (!checkRadius)
  {
    int answer = wxMessageBox( "Region size is too big, it could take a long time  - Continue?", "Warning", wxYES_NO, NULL);
    if (answer == wxNO)
    {
      return false;
    }
  }

  wxBusyCursor wait_cursor;
  wxBusyInfo wait(_("Wait! The algorithm could take long time!"));

  vtkDataSet *inputDataSet = vtkDataSet::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_ManualVolumeSlice)->GetOutput()->GetVTKData());

  if (inputDataSet)
  {
    inputDataSet->Update();

    long progress = 0;
    mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));

    vtkMAFSmartPointer<vtkUnsignedCharArray> newScalars;
    newScalars->SetName("SCALARS");
    newScalars->SetNumberOfTuples(m_VolumeDimensions[0]*m_VolumeDimensions[1]*2);

    vtkDataArray *inputScalars = inputDataSet->GetPointData()->GetScalars();
    vtkMAFSmartPointer<vtkUnsignedCharArray> scalars;
    scalars->SetName("SCALARS");
    scalars->SetNumberOfTuples(m_VolumeDimensions[0]*m_VolumeDimensions[1]);

    for (int i=0;i<2;i++)
    {
      for (int k=0;k<(m_VolumeDimensions[0]*m_VolumeDimensions[1]);k++)
      {
        unsigned char value = inputScalars->GetTuple1(k+i*(m_VolumeDimensions[0]*m_VolumeDimensions[1]));
        scalars->SetTuple1(k,value);
      }

      vtkMAFSmartPointer<vtkStructuredPoints> im;
      im->SetDimensions(m_VolumeDimensions[0],m_VolumeDimensions[1],1);
      im->SetSpacing(m_VolumeSpacing[0],m_VolumeSpacing[1],0.0);
      im->GetPointData()->AddArray(scalars);
      im->GetPointData()->SetActiveScalars("SCALARS");
      im->SetScalarTypeToUnsignedChar();
      im->Update();

      vtkMAFSmartPointer<vtkStructuredPoints> filteredImage;
      if(ApplyRefinementFilter(im, filteredImage) && filteredImage)
      {
        vtkDataArray *binaryScalars = filteredImage->GetPointData()->GetScalars();

        for (int k=0;k<filteredImage->GetNumberOfPoints();k++)
        {
          unsigned char value = binaryScalars->GetTuple1(k);
          newScalars->SetTuple1(k+i*(m_VolumeDimensions[0]*m_VolumeDimensions[1]),value);
        }
      }
    }

    if (inputDataSet->IsA("vtkStructuredPoints"))
    {
      vtkMAFSmartPointer<vtkStructuredPoints> newSP;
      newSP->CopyStructure(vtkStructuredPoints::SafeDownCast(inputDataSet));
      newSP->Update();
      newSP->GetPointData()->AddArray(newScalars);
      newSP->GetPointData()->SetActiveScalars("SCALARS");
      newSP->SetScalarTypeToUnsignedChar();
      newSP->Update();

      m_ManualVolumeSlice->SetData(newSP,mafVME::SafeDownCast(m_ManualVolumeSlice)->GetTimeStamp());
      vtkStructuredPoints *spVME = vtkStructuredPoints::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_ManualVolumeSlice)->GetOutput()->GetVTKData());
      spVME->Update();

    }

    else
    {
      vtkMAFSmartPointer<vtkRectilinearGrid> newRG;
      newRG->CopyStructure(vtkRectilinearGrid::SafeDownCast(inputDataSet));
      newRG->Update();
      newRG->GetPointData()->AddArray(newScalars);
      newRG->GetPointData()->SetActiveScalars("SCALARS");
      newRG->Update();

      m_ManualVolumeSlice->SetData(newRG,mafVME::SafeDownCast(m_Volume)->GetTimeStamp());
      vtkRectilinearGrid *rgVME = vtkRectilinearGrid::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_ManualVolumeSlice)->GetOutput()->GetVTKData());
      rgVME->Update();
    }

    m_ManualVolumeSlice->Update();

    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));


    m_View->VmeShow(m_ManualVolumeSlice,false);
    m_View->VmeShow(m_ManualVolumeSlice,true);


    m_View->CameraUpdate();
  }

  return true;
}

//----------------------------------------------------------------------------
bool medOpSegmentation::ApplyRefinementFilter(vtkStructuredPoints *inputImage, vtkStructuredPoints *outputImage)
//----------------------------------------------------------------------------
{
  typedef itk::VotingBinaryHoleFillingImageFilter<UCharImage, UCharImage> ITKVotingHoleFillingFilter;
  ITKVotingHoleFillingFilter::Pointer holeFillingFilter = ITKVotingHoleFillingFilter::New();

  typedef itk::VotingBinaryIterativeHoleFillingImageFilter<UCharImage> ITKVotingIterativeHoleFillingFilter;
  ITKVotingIterativeHoleFillingFilter::Pointer iterativeHoleFillingFilter = ITKVotingIterativeHoleFillingFilter::New();

  vtkMAFSmartPointer<vtkStructuredPoints> refinedImage;


  vtkMAFSmartPointer<vtkImageCast> vtkImageToFloat;
  vtkImageToFloat->SetOutputScalarTypeToUnsignedChar();
  vtkImageToFloat->SetInput(inputImage);
  vtkImageToFloat->Modified();
  vtkImageToFloat->Update();
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //ITK pipeline to fill holes
  //////////////////////////////////////////////////////////////////////////
  typedef itk::VTKImageToImageFilter< UCharImage > ConvertervtkTOitk;
  ConvertervtkTOitk::Pointer vtkTOitk = ConvertervtkTOitk::New();
  vtkTOitk->SetInput( vtkImageToFloat->GetOutput() );
  vtkTOitk->Update();

  RealImage::SizeType indexRadius;
  indexRadius[0] = m_RefinementRegionsSize; // radius along x
  indexRadius[1] = m_RefinementRegionsSize; // radius along y
  indexRadius[2] = m_RefinementRegionsSize; // radius along z

  bool iterative = m_RefinementIterative && m_RefinementSegmentationAction==ID_REFINEMENT_ISLANDS_REMOVE;

  unsigned char scalarValue = 255;

  if(iterative)
  {
    iterativeHoleFillingFilter->SetInput( ((UCharImage*)vtkTOitk->GetOutput()) );
    iterativeHoleFillingFilter->SetRadius( indexRadius );
    if(m_RefinementSegmentationAction == ID_REFINEMENT_HOLES_FILL)
    {
      iterativeHoleFillingFilter->SetBackgroundValue( 0 );
      iterativeHoleFillingFilter->SetForegroundValue( scalarValue );
    }
    else
    {
      iterativeHoleFillingFilter->SetBackgroundValue( scalarValue );
      iterativeHoleFillingFilter->SetForegroundValue( 0 );
    }

  }
  else
  {
    holeFillingFilter->SetInput( ((UCharImage*)vtkTOitk->GetOutput()) );
    holeFillingFilter->SetRadius( indexRadius );

    //holeFillingFilter->SetMajorityThreshold(m_RefinementRegionsSize*2);

    if(m_RefinementSegmentationAction == ID_REFINEMENT_HOLES_FILL)
    {
      holeFillingFilter->SetBackgroundValue( 0 );
      holeFillingFilter->SetForegroundValue( scalarValue );
    }
    else
    {
      holeFillingFilter->SetBackgroundValue( scalarValue );
      holeFillingFilter->SetForegroundValue( 0 );
    }
  }

  try
  {
    if(iterative)
      iterativeHoleFillingFilter->Update();
    else
      holeFillingFilter->Update();
  }
  catch ( itk::ExceptionObject &err )
  {
    std::cout << "ExceptionObject caught !" << std::endl; 
    std::cout << err << std::endl; 
  }

  typedef itk::ImageToVTKImageFilter< UCharImage > ConverteritkTOvtk;
  ConverteritkTOvtk::Pointer itkTOvtk = ConverteritkTOvtk::New();
  if(iterative)
    itkTOvtk->SetInput( iterativeHoleFillingFilter->GetOutput() ); 
  else
    itkTOvtk->SetInput( holeFillingFilter->GetOutput() );
  itkTOvtk->Update();
  //////////////////////////////////////////////////////////////////////////

  refinedImage = ((vtkStructuredPoints*)itkTOvtk->GetOutput());
  refinedImage->Update();

  outputImage->DeepCopy(refinedImage);

  return true;
}

//----------------------------------------------------------------------------
void medOpSegmentation::CreateSliceNavigationGui()
//----------------------------------------------------------------------------
{
  if(!m_GuiDialog)
    return;

  //////////////////////////////////////////////////////////////////////////
  // SLICE PLANES
  //////////////////////////////////////////////////////////////////////////

  wxString planes[3];
  planes[0] = wxString("XY");
  planes[1] = wxString("XZ");
  planes[2] = wxString("YZ");

  m_GuiDialog->Combo(ID_SLICE_PLANE, "Slice Plane", &m_CurrentSlicePlane, 3, planes)->SetSelection(m_CurrentSlicePlane);

  ////////////////////////////////////////////////////////////////////////
  // SLICE SLIDER
  ////////////////////////////////////////////////////////////////////////

  // m_SliceSlider = m_GuiDialog->Slider(ID_SLICE_SLIDER, "Slice", &m_CurrentSliceIndex, 1, 1);

  wxPoint p = wxDefaultPosition;
  wxSize size = wxDefaultSize;

  int id_slider = m_GuiDialog->GetWidgetId(ID_SLICE_SLIDER);
  int id_text = m_GuiDialog->GetWidgetId(ID_SLICE_TEXT);

  // Slices ----------
  wxTextCtrl *slice_text = NULL;
  wxStaticText *slice_lab  = NULL;
  wxStaticText *slice_foo = NULL;
  wxBoxSizer *slice_sizer = new wxBoxSizer(wxHORIZONTAL);
  slice_lab = new wxStaticText (m_GuiDialog, -1, "Slice: ", p, wxSize(55,18), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  slice_foo = new wxStaticText (m_GuiDialog, -1, "", p, wxSize(8,18), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  slice_text = new wxTextCtrl(m_GuiDialog, ID_SLICE_SLIDER, "", p, wxSize(30,18), wxTE_LEFT/*|wxTE_READONLY*/);
  slice_text->Enable(true);
  m_SliceText = slice_text;

  m_CurrentSliceIndex = 1;
  m_SliceSlider = new wxSlider(m_GuiDialog, ID_SLICE_SLIDER,  m_CurrentSliceIndex, 1, m_VolumeDimensions[2], p, wxSize(64,18));
  mafGUIButton *b_incr_slice = new mafGUIButton(m_GuiDialog, ID_SLICE_NEXT, ">",	p,wxSize(18, 18));
  mafGUIButton *b_decr_slice = new mafGUIButton(m_GuiDialog, ID_SLICE_PREV, "<",	p,wxSize(18, 18));
  slice_text->SetValidator(mafGUIValidator(this,ID_SLICE_TEXT,slice_text,&m_CurrentSliceIndex,m_SliceSlider,1,m_VolumeDimensions[2]));
  m_SliceSlider->SetValidator(mafGUIValidator(this, ID_SLICE_SLIDER, m_SliceSlider,&m_CurrentSliceIndex,slice_text));

  b_incr_slice->SetValidator(mafGUIValidator(this, ID_SLICE_NEXT,b_incr_slice));
  b_decr_slice->SetValidator(mafGUIValidator(this, ID_SLICE_PREV,b_decr_slice));
  slice_sizer->Add(slice_lab,0, wxRIGHT, 5);
  slice_sizer->Add(slice_text,0);
  slice_sizer->Add(slice_foo,0);
  slice_sizer->Add(b_decr_slice,0);
  slice_sizer->Add(m_SliceSlider,0);
  slice_sizer->Add(b_incr_slice,0);

  m_GuiDialog->Add(slice_sizer,0,wxALL, 1); 

  m_SliceSlider->SetValue(1);

  m_SliceSlider->Update();
  m_GuiDialog->Update();

}

//----------------------------------------------------------------------------
void medOpSegmentation::CreatePreSegmentationGui()
//----------------------------------------------------------------------------
{
  mafGUI *currentGui = new mafGUI(this);

  currentGui->Label("Volume Spacing",false);

  //InitVolumeSpacing();

  medOpVolumeResample *op = new medOpVolumeResample();
  op->SetInput(m_Volume);
  op->TestModeOn();
  op->AutoSpacing();
  op->GetSpacing(m_VolumeSpacing);
  mafDEL(op);

  currentGui->Vector(ID_PRE_VOLUME_SPACING, "", this->m_VolumeSpacing,MINFLOAT,MAXFLOAT,4,"output volume spacing");
  //currentGui->Button(ID_VOLUME_AUTOSPACING,"AutoSpacing","","compute auto spacing by rotating original spacing");

  currentGui->Label("");

  currentGui->Label("");

  m_SegmentationOperationsGui[PRE_SEGMENTATION] = currentGui;
}

//----------------------------------------------------------------------------
void medOpSegmentation::CreateAutoSegmentationGui()
//----------------------------------------------------------------------------
{
  mafGUI *currentGui = new mafGUI(this);

  currentGui->Label(_("Threshold"),true);
  //currentGui->Label(_("(CTRL + left click on the slice "),false,true);
  //currentGui->Label(_("to select a scalar value)"));
  std::vector<const char*> increaseLabels;
  increaseLabels.push_back("+");
  increaseLabels.push_back("+");
  increaseLabels.push_back("+");
  std::vector<const char*> decreaseLabels;
  decreaseLabels.push_back("-");
  decreaseLabels.push_back("-");
  decreaseLabels.push_back("-");
  double sr[2];
  m_Volume->GetOutput()->GetVTKData()->GetScalarRange(sr);
  

  //Threshold 
  //[ + ] [ + ] [ + ]
  //[min][range][max]
  //[ - ] [ - ] [ - ] 
  m_AutomaticThreshold=sr[0];
  m_AutomaticUpperThreshold=sr[1];
  m_AutomaticThresholdSlider = new mafGUILutSlider(currentGui,-1,wxPoint(0,0),wxSize(300,24));
  m_AutomaticThresholdSlider->SetListener(this);
  m_AutomaticThresholdSlider->SetText(1,"Threshold");  
  m_AutomaticThresholdSlider->SetRange(sr[0],sr[1]);
  m_AutomaticThresholdSlider->SetSubRange(sr[0],sr[1]);

  std::vector<int> increaseTrIDs;
  increaseTrIDs.push_back(ID_AUTOMATIC_INCREASE_MIN_THRESHOLD);
  increaseTrIDs.push_back(ID_AUTOMATIC_INCREASE_MIDDLE_THRESHOLD);
  increaseTrIDs.push_back(ID_AUTOMATIC_INCREASE_MAX_THRESHOLD);
  currentGui->MultipleButtons(3,3,increaseTrIDs,increaseLabels);

  currentGui->Add(m_AutomaticThresholdSlider);

  std::vector<int> decreaseTrIDs;
  decreaseTrIDs.push_back(ID_AUTOMATIC_DECREASE_MIN_THRESHOLD);
  decreaseTrIDs.push_back(ID_AUTOMATIC_DECREASE_MIDDLE_THRESHOLD);
  decreaseTrIDs.push_back(ID_AUTOMATIC_DECREASE_MAX_THRESHOLD);
  currentGui->MultipleButtons(3,3,decreaseTrIDs,decreaseLabels);

  //end Threshold

  wxString choices[2] = {_("Global"),_("Range")};
  currentGui->Label(_("Threshold type:"));
  currentGui->Radio(ID_AUTOMATIC_GLOBAL_THRESHOLD,"",&m_AutomaticGlobalThreshold,2,choices);
  currentGui->Button(ID_AUTOMATIC_GLOBAL_PREVIEW,_("preview"));
  currentGui->Enable(ID_AUTOMATIC_GLOBAL_PREVIEW,m_AutomaticGlobalThreshold==GLOBAL);
  currentGui->Label(_("Slice range settings"),true);
  currentGui->Label(_("1)Move the sliding button"));
  currentGui->Label(_("to set a slices' range."));
  currentGui->Label(_("2)right click to manually"));
  currentGui->Label(_("enter slice's values."));

 

  //Slides Range
  //[ + ] [ + ] [ + ]
  //[min][range][max]
  //[ - ] [ - ] [ - ] 

  m_AutomaticRangeSlider = new mafGUILutSlider(currentGui,-1,wxPoint(0,0),wxSize(300,24));
  m_AutomaticRangeSlider->SetListener(currentGui);
  m_AutomaticRangeSlider->SetText(1,"range");  
  m_AutomaticRangeSlider->SetRange(1,m_VolumeDimensions[2]);
  m_AutomaticRangeSlider->SetSubRange(1,m_VolumeDimensions[2]);

  currentGui->Label("");
  std::vector<int> increaseIDs;
  increaseIDs.push_back(ID_AUTOMATIC_INCREASE_MIN_RANGE_VALUE);
  increaseIDs.push_back(ID_AUTOMATIC_INCREASE_MIDDLE_RANGE_VALUE);
  increaseIDs.push_back(ID_AUTOMATIC_INCREASE_MAX_RANGE_VALUE);
  currentGui->MultipleButtons(3,3,increaseIDs,increaseLabels);
  
  currentGui->Add(m_AutomaticRangeSlider);
  
  std::vector<int> decreaseIDs;
  decreaseIDs.push_back(ID_AUTOMATIC_DECREASE_MIN_RANGE_VALUE);
  decreaseIDs.push_back(ID_AUTOMATIC_DECREASE_MIDDLE_RANGE_VALUE);
  decreaseIDs.push_back(ID_AUTOMATIC_DECREASE_MAX_RANGE_VALUE);
  currentGui->MultipleButtons(3,3,decreaseIDs,decreaseLabels);
  currentGui->Label("");
  //End

  currentGui->TwoButtons(ID_AUTOMATIC_ADD_RANGE,ID_AUTOMATIC_REMOVE_RANGE,("Add"),_("Remove"));
  currentGui->Button(ID_AUTOMATIC_UPDATE_RANGE,("Update"));
  m_AutomaticListOfRange = currentGui->ListBox(ID_AUTOMATIC_LIST_OF_RANGE,"");

  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION] = currentGui;
}

//----------------------------------------------------------------------------
void medOpSegmentation::CreateLoadSegmentationGui()
//----------------------------------------------------------------------------
{
  mafGUI *currentGui = new mafGUI(this);

  m_SegmentationOperationsGui[LOAD_SEGMENTATION] = currentGui;

}
//----------------------------------------------------------------------------
void medOpSegmentation::CreateManualSegmentationGui()
//----------------------------------------------------------------------------
{

  mafGUI *currentGui = new mafGUI(this);

  //////////////////////////////////////////////////////////////////////////
  // Radio button box to select mode
  //////////////////////////////////////////////////////////////////////////
  wxString modes[3];
  modes[0] = wxString("Move Camera");
  modes[1] = wxString("Brush edit");
  modes[2] = wxString("Refine");
  int w_id = currentGui->GetWidgetId(ID_MANUAL_MODE);;

  wxStaticBoxSizer *manualModeSizer = new wxStaticBoxSizer(wxHORIZONTAL, currentGui, "Mode");
  wxRadioBox *manualModeRadioBox;

  manualModeRadioBox = new wxRadioBox(currentGui, w_id, "",wxDefaultPosition, wxSize(130,-1), 3, modes, 3, wxRA_SPECIFY_ROWS);
  manualModeRadioBox->SetValidator( mafGUIValidator(currentGui, w_id, manualModeRadioBox, &m_ManualSegmentationMode) );
  manualModeSizer->Add(manualModeRadioBox);
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  // Brush Editing options
  //////////////////////////////////////////////////////////////////////////

  wxStaticBoxSizer *brushEditingSizer = new wxStaticBoxSizer(wxVERTICAL, currentGui, "Brush Options");

  // BRUSH ACTION
  m_ManualSegmentationAction = MANUAL_SEGMENTATION_SELECT;
  wxString operations[2];
  operations[MANUAL_SEGMENTATION_SELECT] = wxString("Select");
  operations[MANUAL_SEGMENTATION_ERASE] = wxString("Erase");

  wxBoxSizer *editingComboSizer = new wxBoxSizer(wxHORIZONTAL);

  w_id = currentGui->GetWidgetId((ID_MANUAL_PICKING_ACTION));

  wxStaticText *editingComboLab = new wxStaticText(currentGui, w_id, "Brush Mode", wxDefaultPosition, wxSize(60,-1));//, wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  m_ManualEditingActionComboBox = new wxComboBox  (currentGui, w_id, "", wxDefaultPosition, wxSize(130,-1), 2, operations,wxCB_READONLY);
  m_ManualEditingActionComboBox->SetValidator( mafGUIValidator(currentGui,w_id,m_ManualEditingActionComboBox, &m_ManualSegmentationAction) );
  editingComboSizer->Add( editingComboLab);//,  0, wxRIGHT, 5);
  editingComboSizer->Add( m_ManualEditingActionComboBox);//,0, wxRIGHT, 2);

  // BRUSH SHAPE
  wxString shapes[2];
  shapes[0] = wxString("circle");
  shapes[1] = wxString("square");
  w_id = currentGui->GetWidgetId(ID_MANUAL_BRUSH_SHAPE);;

  wxBoxSizer *brushShapesSizer = new wxBoxSizer(wxHORIZONTAL);
  wxStaticText *brushShapeLab = new wxStaticText(currentGui, w_id, "Shape");

  m_ManualBrushShapeRadioBox = new wxRadioBox(currentGui, w_id, "",wxDefaultPosition, wxSize(130,-1), 2, shapes, 2);
  m_ManualBrushShapeRadioBox->SetValidator( mafGUIValidator(currentGui, w_id, m_ManualBrushShapeRadioBox, &m_ManualBrushShape) );
  brushShapesSizer->Add( brushShapeLab,  0, wxRIGHT, 5);
  brushShapesSizer->Add(m_ManualBrushShapeRadioBox,0, wxRIGHT, 2);

  // BRUSH SIZE
  m_ManualBrushSize = 1;

  wxStaticText *brushSizeLab  = NULL;
  int id = currentGui->GetWidgetId(ID_MANUAL_BRUSH_SIZE);
  wxBoxSizer *brushSizeSizer = new wxBoxSizer(wxHORIZONTAL);

  brushSizeLab = new wxStaticText  (currentGui, id, "Size (unit)", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE );

  m_ManualBrushSizeText = new wxTextCtrl(currentGui, id, "", wxDefaultPosition, wxSize(40, 18));

  m_ManualBrushSizeSlider = new mafGUIFloatSlider(currentGui, id,m_ManualBrushSize,0,130, wxDefaultPosition, wxDefaultSize);

  m_ManualBrushSizeText->SetValidator(mafGUIValidator(currentGui, id, m_ManualBrushSizeText, &m_ManualBrushSize, m_ManualBrushSizeSlider, 0, 130));
  m_ManualBrushSizeSlider->SetValidator(mafGUIValidator(currentGui, id, m_ManualBrushSizeSlider, &m_ManualBrushSize, m_ManualBrushSizeText));
  brushSizeSizer->Add(brushSizeLab,  0, wxRIGHT, 5);
  brushSizeSizer->Add(m_ManualBrushSizeText, 0);
  brushSizeSizer->Add(m_ManualBrushSizeSlider,  0);

  w_id = currentGui->GetWidgetId(ID_MANUAL_CONTINUOUS_PICKING);
  wxCheckBox *manualContinuousPickingCheck = new wxCheckBox(currentGui, w_id, wxString("Continuous Picking"), wxDefaultPosition, wxSize(-1,20), 0 );
  manualContinuousPickingCheck->SetValidator( mafGUIValidator(currentGui,w_id,manualContinuousPickingCheck,&m_ManualContinuousPickingOn) );


  brushEditingSizer->Add(editingComboSizer, 0, wxALL, 1);
  brushEditingSizer->Add(brushShapesSizer, 0, wxALL, 1);
  brushEditingSizer->Add(brushSizeSizer, 0, wxALL, 1);
  brushEditingSizer->Add(manualContinuousPickingCheck, 0, wxALL, 1);
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  // Refinement Options
  //////////////////////////////////////////////////////////////////////////
  wxStaticBoxSizer *refinementSizer = new wxStaticBoxSizer(wxVERTICAL, currentGui, "Refinement Options");

  m_RefinementSegmentationAction = ID_REFINEMENT_ISLANDS_REMOVE;
  wxString refinementOperations[2];
  refinementOperations[ID_REFINEMENT_ISLANDS_REMOVE] = wxString("Remove Islands");
  refinementOperations[ID_REFINEMENT_HOLES_FILL] = wxString("Fill Holes");

  w_id = currentGui->GetWidgetId(ID_MANUAL_REFINEMENT_FILLIN);
  int w_id2 = currentGui->GetWidgetId(ID_MANUAL_REFINEMENT_REMOVE);
  mafGUIButton    *b1 = new mafGUIButton(currentGui, w_id, "Fill Holes",wxDefaultPosition, wxSize(-1, -1) );
  b1->SetValidator( mafGUIValidator(currentGui,w_id,b1) );

  mafGUIButton    *b2 = new mafGUIButton(currentGui, w_id2, "Remove Islands", wxDefaultPosition, wxSize(-1, -1) );
  b2->SetValidator( mafGUIValidator(currentGui,w_id2,b2) );

  wxBoxSizer *refinementButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
  refinementButtonsSizer->Add( b1, 0);
  refinementButtonsSizer->Add( b2, 0);

  // Size of islands/holes to be taken into consideration
  m_ManualRefinementRegionsSize = 1;
  int stepsNumber = 5;

  w_id = currentGui->GetWidgetId(ID_MANUAL_REFINEMENT_REGIONS_SIZE);

  wxSlider     *sli  = NULL;
  wxStaticText *lab  = NULL;

  int text_w   = 45*0.8;
  int slider_w = 60;

  m_ManualRefinementRegionSizeText = new wxTextCtrl (currentGui, w_id, "", wxDefaultPosition, wxSize(text_w,  18), wxSUNKEN_BORDER);
  sli = new wxSlider(currentGui, w_id,1,1,stepsNumber, wxDefaultPosition, wxSize(slider_w,18));
  sli->SetValidator(mafGUIValidator(currentGui,w_id,sli,&m_ManualRefinementRegionsSize,m_ManualRefinementRegionSizeText));
  m_ManualRefinementRegionSizeText->SetValidator(mafGUIValidator(currentGui,w_id,m_ManualRefinementRegionSizeText,&m_ManualRefinementRegionsSize,sli,1,stepsNumber)); //- if uncommented, remove also wxTE_READONLY from the text (in both places)

  wxBoxSizer *regionSizeSizer = new wxBoxSizer(wxHORIZONTAL);
  regionSizeSizer->Add(m_ManualRefinementRegionSizeText, 0);
  regionSizeSizer->Add(sli,  0);


  //refinementSizer->Add(refinementComboSizer, 0, wxALL, 1);
  refinementSizer->AddSpacer(15);
  refinementSizer->Add(regionSizeSizer, 0, wxALL, 1);
  refinementSizer->AddSpacer(25);
  refinementSizer->Add(refinementButtonsSizer, 0, wxALL, 1);
  //////////////////////////////////////////////////////////////////////////

  currentGui->Add(manualModeSizer, wxALIGN_CENTER_HORIZONTAL);
  currentGui->Add(brushEditingSizer, wxALIGN_CENTER_HORIZONTAL);
  currentGui->Add(refinementSizer, wxALIGN_CENTER_HORIZONTAL);
  currentGui->Button(ID_MANUAL_OK, mafString("Apply"));

  currentGui->TwoButtons(ID_MANUAL_REDO, ID_MANUAL_UNDO, "Redo", "Undo");


  m_SegmentationOperationsGui[MANUAL_SEGMENTATION] = currentGui;

  InitManualSegmentationGui();
  EnableManualSegmentationGui();

}

//----------------------------------------------------------------------------
void medOpSegmentation::EnableManualSegmentationGui()
//----------------------------------------------------------------------------
{
  //brush options
  // erase/select, shape, size, continuos picking

  bool enable = m_ManualSegmentationMode == MANUAL_SEGMENTATION_BRUSH && m_CurrentSlicePlane == XY;

  m_ManualEditingActionComboBox->Enable(enable);
  m_ManualBrushShapeRadioBox->Enable(enable);
  m_ManualBrushSizeText->Enable(enable);
  m_ManualBrushSizeSlider->Enable(enable);
  m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_CONTINUOUS_PICKING, enable);

  //refinement options
  // fill/remove, size, global

  enable = m_ManualSegmentationMode == MANUAL_SEGMENTATION_REFINEMENT && m_CurrentSlicePlane == XY;
  m_ManualRefinementRegionSizeText->Enable(enable);
  m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_REFINEMENT_FILLIN, enable);
  m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_REFINEMENT_REMOVE, enable);

  //m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_OK, m_ManualSegmentationMode == MANUAL_SEGMENTATION_REFINEMENT && m_CurrentSlicePlane == XY);


}
//----------------------------------------------------------------------------
void medOpSegmentation::CreateRefinementGui()
//----------------------------------------------------------------------------
{
  mafGUI *currentGui = new mafGUI(this);

  // Action: remove islands OR fill holes.
  m_RefinementSegmentationAction = ID_REFINEMENT_ISLANDS_REMOVE;
  wxString operations[2];
  operations[ID_REFINEMENT_ISLANDS_REMOVE] = wxString("Remove Islands");
  operations[ID_REFINEMENT_HOLES_FILL] = wxString("Fill Holes");

  currentGui->Combo(ID_REFINEMENT_ACTION, "Action", &m_RefinementSegmentationAction, 2, operations);

  // Size of islands/holes to be taken into consideration
  m_RefinementRegionsSize = 1;
  //currentGui->Integer(ID_REFINEMENT_REGIONS_SIZE, mafString("Size"), &m_RefinementRegionsSize, 0, MAXINT, mafString("Max size of islands/holes to be taken into consideration"));

  int stepsNumber = 5;
  int w_id = currentGui->GetWidgetId(ID_MANUAL_REFINEMENT_REGIONS_SIZE);

  int text_w   = 45*0.8;
  int slider_w = 60;

  wxTextCtrl *refinementRegionSizeText = new wxTextCtrl (currentGui, w_id, "", wxDefaultPosition, wxSize(text_w,  18), wxSUNKEN_BORDER);
  wxSlider *sli = new wxSlider(currentGui, w_id,1,1,stepsNumber, wxDefaultPosition, wxSize(slider_w,18));
  sli->SetValidator(mafGUIValidator(currentGui,w_id,sli,&m_RefinementRegionsSize,refinementRegionSizeText));
  refinementRegionSizeText->SetValidator(mafGUIValidator(currentGui,w_id,refinementRegionSizeText,&m_RefinementRegionsSize,sli,1,stepsNumber)); //- if uncommented, remove also wxTE_READONLY from the text (in both places)

  wxBoxSizer *regionSizeSizer = new wxBoxSizer(wxHORIZONTAL);
  regionSizeSizer->Add(refinementRegionSizeText, 0);
  regionSizeSizer->Add(sli,  0);

  currentGui->Add(regionSizeSizer);

  // Switch on/off the "apply on every slice" option
  m_RefinementEverySlice = 0;
  currentGui->Bool(ID_REFINEMENT_EVERY_SLICE, mafString("Global"), &m_RefinementEverySlice, 0, mafString("Apply refinement procedure on every slice"));

  m_RefinementIterative = 0;
  currentGui->Bool(ID_REFINEMENT_ITERATIVE, mafString("Iterative"), &m_RefinementIterative, 0, mafString("Switch on/off the iterative feature"));

  currentGui->TwoButtons(ID_REFINEMENT_REDO, ID_REFINEMENT_UNDO, "Redo", "Undo");

  currentGui->Button(ID_REFINEMENT_APPLY, mafString("Apply"), "");

  currentGui->Divider();

  m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION] = currentGui;

}
//----------------------------------------------------------------------------
void medOpSegmentation::InitGui()
//----------------------------------------------------------------------------
{
  int sliceMax = 1;

  if(m_CurrentSlicePlane == XY)
    sliceMax = m_VolumeDimensions[2];
  else if(m_CurrentSlicePlane == XZ)
    sliceMax = m_VolumeDimensions[1];
  else if(m_CurrentSlicePlane == YZ)
    sliceMax = m_VolumeDimensions[0];

  m_SliceSlider->SetRange(1, sliceMax);
  m_SliceText->SetValidator(mafGUIValidator(this,ID_SLICE_TEXT,m_SliceText,&m_CurrentSliceIndex,m_SliceSlider,1, sliceMax));
  m_CurrentSliceIndex = 1;

  m_SliceSlider->Update();

  InitManualSegmentationGui();

  m_GuiDialog->FitGui();
  m_GuiDialog->Update();

}

//----------------------------------------------------------------------------
void medOpSegmentation::InitManualSegmentationGui()
//----------------------------------------------------------------------------
{
  // brush size slider: min = 1; max = slice size
  if(!m_ManualBrushSizeSlider || ! m_SegmentationOperationsGui[MANUAL_SEGMENTATION])
    return;

  m_ManualBrushSizeSlider->SetMin(1);

  int maxBrushSize = 1;

  if(m_CurrentSlicePlane == XY)
  {
    maxBrushSize = std::min(m_VolumeDimensions[0], m_VolumeDimensions[1]);
  }
  else if(m_CurrentSlicePlane == XZ)
  {
    maxBrushSize = std::min(m_VolumeDimensions[0], m_VolumeDimensions[2]);
  }
  else if(m_CurrentSlicePlane == YZ)
  {
    maxBrushSize = std::min(m_VolumeDimensions[1], m_VolumeDimensions[2]);
  }

  maxBrushSize = round(maxBrushSize/2);

  m_ManualBrushSizeSlider->SetMax(maxBrushSize);
  m_ManualBrushSizeSlider->SetMin(0.5);
  m_ManualBrushSize = 0.5;
  m_ManualBrushSizeSlider->SetValue(m_ManualBrushSize);
  m_ManualBrushSizeSlider->Update();

  m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_CONTINUOUS_PICKING, (m_ManualSegmentationMode == MANUAL_SEGMENTATION_BRUSH) );

  m_GuiDialog->Update();

}
//----------------------------------------------------------------------------
void medOpSegmentation::OnChangeStep(int eventID)
//----------------------------------------------------------------------------
{
  switch(eventID)
  {
  case ID_BUTTON_NEXT:
    {
      if (m_CurrentOperation < NUMBER_OF_OPERATIONS-1)//if the current operation isn't the last operation
      {
        m_OldOperation = m_CurrentOperation;
        OnNextStep();
      }
    }
    break;
  case ID_BUTTON_PREV:
    {
      if (m_CurrentOperation > 0)//if the current operation isn't the first operation
      {
        m_OldOperation = m_CurrentOperation;

        OnPreviousStep();
      }
    }
    break;
  }
}
//------------------------------------------------------------------------
void medOpSegmentation::InitSegmentedVolume()
//------------------------------------------------------------------------
{
  mafNEW(m_SegmentatedVolume);
  m_SegmentatedVolume->SetVolumeLink(m_Volume);
  m_SegmentatedVolume->SetName("Segmented Volume");
  m_SegmentatedVolume->ReparentTo(m_Volume->GetParent());
  m_SegmentatedVolume->SetDoubleThresholdModality(true);
  m_SegmentatedVolume->Update();
}
//------------------------------------------------------------------------
void medOpSegmentation::InitThresholdVolume()
//------------------------------------------------------------------------
{
  mafNEW(m_ThresholdVolume);
  m_ThresholdVolume->DeepCopy(m_Volume);
  m_ThresholdVolume->SetName("Threshold Volume");
  m_ThresholdVolume->ReparentTo(m_Volume->GetParent());
  m_ThresholdVolume->Update();

  m_View->VmeAdd(m_ThresholdVolume);
}
//------------------------------------------------------------------------
void medOpSegmentation::OnNextStep()
//------------------------------------------------------------------------
{
  if(m_CurrentOperation == PRE_SEGMENTATION)
  {

    InitThresholdVolume();
    InitSegmentedVolume();

    InitializeInteractors();
    InitVolumeDimensions();
    InitVolumeSpacing();
    InitGui();
    UpdateSlice();
    UpdateWindowing();
    InitGui();

    m_GuiDialog->Enable(ID_PRE_SEGMENTATION,false);
    m_GuiDialog->Enable(ID_BUTTON_PREV,false);

  } else if (m_CurrentOperation == AUTOMATIC_SEGMENTATION)
  {
    m_AutomaticThresholdTextMapper->SetInput("");
    m_GuiDialog->Enable(ID_AUTO_SEGMENTATION,false);
    m_GuiDialog->Enable(ID_BUTTON_PREV,true);
  }
  else if (m_CurrentOperation == MANUAL_SEGMENTATION)
  {
    if (m_ManualModifiedWithoutApplied)
    {
      int answer = wxMessageBox(_("Current slice changes\nhave not been saved.\nDo you want to continue?"),_("Warning"),wxYES_NO|wxCENTER);
      if (answer == wxNO)
      {
        return;
      }
    }

    m_Volume->SetBehavior(m_OldBehavior);

    m_SER->GetAction("pntEditingAction")->UnBindInteractor(m_ManualPER);
    m_SER->GetAction("pntEditingAction")->UnBindDevice(m_DialogMouse);

    if(m_ManualSegmentationMode == MANUAL_SEGMENTATION_BRUSH && m_CurrentSlicePlane == XY)
    {
      wxCursor cursor = wxCursor( wxCURSOR_DEFAULT );
      m_View->GetWindow()->SetCursor(cursor);

      m_ManualPER->RemoveActor();
    }

    m_View->VmeShow(m_ManualVolumeSlice, false);

    SaveManualVolumeMask();

    m_GuiDialog->Enable(ID_MANUAL_SEGMENTATION,false);
  }
  else if (m_CurrentOperation == REFINEMENT_SEGMENTATION)
  {
    SaveRefinementVolumeMask();

    m_GuiDialog->Enable(ID_REFINEMENT,false);
  }
  else if (m_CurrentOperation == LOAD_SEGMENTATION)
  {
    m_GuiDialog->Enable(ID_LOAD_SEGMENTATION,false);
  }

  m_CurrentOperation++;

  if (m_CurrentOperation == AUTOMATIC_SEGMENTATION)
  {
    m_SnippetsLabel->SetLabel( _("CTRL + left click on the slice to select a scalar value"));
    m_Dialog->Update();
    UpdateThresholdLabel();
    m_GuiDialog->Enable(ID_AUTO_SEGMENTATION,true);
    m_GuiDialog->Enable(ID_BUTTON_NEXT,false);
  }
  else if (m_CurrentOperation == MANUAL_SEGMENTATION)
  {

    if(m_CurrentSlicePlane == XY)
    {
      InitManualVolumeSlice();
      UpdateVolumeSlice();
      m_View->SetTextureInterpolate(false);
      m_View->VmeShow(m_Volume, false);
      m_View->VmeShow(m_Volume, true);
      m_View->VmeShow(m_ManualVolumeSlice, true);
    }

    m_SER->GetAction("pntEditingAction")->BindInteractor(m_ManualPER);
    m_SER->GetAction("pntEditingAction")->BindDevice(m_DialogMouse);

    if( m_ManualSegmentationMode == MANUAL_SEGMENTATION_BRUSH)
    {
      m_Volume->SetBehavior(m_ManualPicker);

      wxCursor cursor = wxCursor( wxCURSOR_PENCIL );
      m_View->GetWindow()->SetCursor(cursor);

      m_ManualPER->EnableDrawing(true);
    }
    else
    {
      m_Volume->SetBehavior(m_AutomaticPicker);
      m_ManualPER->EnableDrawing(false);

      m_ManualPER->RemoveActor();

    }
    //////////////////////////////////////////////////////////////////////////

    m_SnippetsLabel->SetLabel( _("CTRL + left click on the slice to edit the slice"));
    m_Dialog->Update();

    m_GuiDialog->Enable(ID_MANUAL_SEGMENTATION,true);

    m_ManualModifiedWithoutApplied = false;

    m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_PICKING_MODALITY, m_CurrentSlicePlane == XY);
    m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_OK, false);
    m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_UNDO, false);
    m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_REDO, false);
    m_GuiDialog->Enable(ID_BUTTON_PREV,true);
  }
  else if (m_CurrentOperation == REFINEMENT_SEGMENTATION)
  {
    m_View->SetTextureInterpolate(true);
    m_View->VmeShow(m_Volume, false);
    m_View->VmeShow(m_Volume, true);
    m_View->VmeShow(m_ThresholdVolume, false);
    m_View->VmeShow(m_ThresholdVolume, true);

    m_SnippetsLabel->SetLabel(_(""));
    m_Dialog->Update();

    m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_APPLY, m_CurrentSlicePlane == XY);
    m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_UNDO, false);
    m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_REDO, false);
    m_GuiDialog->Enable(ID_REFINEMENT,true);
    m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_APPLY, m_CurrentSlicePlane == XY);
  }
  else if (m_CurrentOperation == LOAD_SEGMENTATION)
  {
    m_GuiDialog->Enable(ID_LOAD_SEGMENTATION,true);
    m_GuiDialog->Enable(ID_BUTTON_NEXT,false);
  }
  int oldSliceIndex = m_CurrentSliceIndex;

  if(m_OldOperation != PRE_SEGMENTATION)
    m_SegmentationOperationsRollOut[m_OldOperation]->RollOut(false);
  m_SegmentationOperationsRollOut[m_CurrentOperation]->RollOut(true);

  m_CurrentSliceIndex = oldSliceIndex;

  m_SliceSlider->SetValue(m_CurrentSliceIndex);
  m_SliceSlider->Update();


  UpdateSlice();

  m_View->CameraUpdate();

}
//------------------------------------------------------------------------
void medOpSegmentation::OnPreviousStep()
//------------------------------------------------------------------------
{
  if (m_CurrentOperation == MANUAL_SEGMENTATION)
  {
    m_Volume->SetBehavior(m_AutomaticPicker);

    m_SER->GetAction("pntEditingAction")->UnBindInteractor(m_ManualPER);
    m_SER->GetAction("pntEditingAction")->UnBindDevice(m_DialogMouse);

    if(m_ManualSegmentationMode == MANUAL_SEGMENTATION_BRUSH && m_CurrentSlicePlane == XY)
    {
      wxCursor cursor = wxCursor( wxCURSOR_DEFAULT );
      m_View->GetWindow()->SetCursor(cursor);

      m_ManualPER->RemoveActor();
    }

    m_View->VmeShow(m_ManualVolumeSlice, false);

    m_AutomaticThresholdTextMapper->SetInput("Threshold =");
    m_AutomaticScalarTextMapper->SetInput("Scalar =");

    m_View->CameraUpdate();

    m_GuiDialog->Enable(ID_MANUAL_SEGMENTATION,false);
  }
  else if (m_CurrentOperation == REFINEMENT_SEGMENTATION)
  {

    m_GuiDialog->Enable(ID_REFINEMENT,false);
  }
  else if (m_CurrentOperation == LOAD_SEGMENTATION)
  {
    m_GuiDialog->Enable(ID_LOAD_SEGMENTATION,false);
  }

  m_CurrentOperation--;

  if (m_CurrentOperation == AUTOMATIC_SEGMENTATION)
  {
    m_SnippetsLabel->SetLabel(_("CTRL + left click on the slice to select a scalar value"));
    m_Dialog->Update();

    UpdateThresholdLabel();
    m_GuiDialog->Enable(ID_AUTO_SEGMENTATION,true);
    m_GuiDialog->Enable(ID_BUTTON_PREV,false);
    m_GuiDialog->Enable(ID_BUTTON_NEXT,false);

    m_View->SetTextureInterpolate(true);

    m_View->VmeShow(m_ThresholdVolume,false);
    if(m_SegmentatedVolume->GetAutomaticOutput()->IsA("vtkStructuredPoints"))
    {
      vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast( m_SegmentatedVolume->GetAutomaticOutput() );
      m_ThresholdVolume->SetData( sp, mafVME::SafeDownCast(m_Volume)->GetTimeStamp() );
    }
    else
    {
      vtkRectilinearGrid *rg = vtkRectilinearGrid::SafeDownCast( m_SegmentatedVolume->GetAutomaticOutput() );
      m_ThresholdVolume->SetData( rg, mafVME::SafeDownCast(m_Volume)->GetTimeStamp() );
    }

    m_View->VmeShow(m_Volume, false);
    m_View->VmeShow(m_Volume, true);
    m_View->VmeShow(m_ThresholdVolume,true);

  }
  else if (m_CurrentOperation == MANUAL_SEGMENTATION)
  {
    m_SnippetsLabel->SetLabel(_("CTRL + left click on the slice to edit the slice"));
    m_Dialog->Update();

    if(m_SegmentatedVolume->GetManualOutput()->IsA("vtkStructuredPoints"))
    {
      m_ThresholdVolume->SetData( vtkStructuredPoints::SafeDownCast(m_SegmentatedVolume->GetManualOutput()),0.0 );
    }
    else
    {
      m_ThresholdVolume->SetData( vtkRectilinearGrid::SafeDownCast(m_SegmentatedVolume->GetManualOutput()), mafVME::SafeDownCast(m_Volume)->GetTimeStamp() );
    }

    m_ThresholdVolume->Update();

    if(m_CurrentSlicePlane == XY)
    {
      //m_View->VmeShow(m_ThresholdVolume, false);
      UpdateVolumeSlice();
      m_View->VmeShow(m_ManualVolumeSlice, false);
      m_View->VmeShow(m_ManualVolumeSlice, true);
    }

    m_SER->GetAction("pntEditingAction")->BindInteractor(m_ManualPER);
    m_SER->GetAction("pntEditingAction")->BindDevice(m_DialogMouse);

    if(m_CurrentSlicePlane == XY && m_ManualSegmentationMode == MANUAL_SEGMENTATION_BRUSH)
    {
      m_Volume->SetBehavior(m_ManualPicker);

      wxCursor cursor = wxCursor( wxCURSOR_PENCIL );
      m_View->GetWindow()->SetCursor(cursor);

      m_ManualPER->EnableDrawing(true);

    }
    else
    {
      m_Volume->SetBehavior(m_AutomaticPicker);

      m_ManualPER->RemoveActor();
      m_ManualPER->EnableDrawing(false);

    }

    m_View->SetTextureInterpolate(false);
    m_View->VmeShow(m_Volume, false);
    m_View->VmeShow(m_Volume, true);

    m_ManualModifiedWithoutApplied = false;

    m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_PICKING_MODALITY, m_CurrentSlicePlane == XY);
    m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_OK, false);
    m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_UNDO, false);
    m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_REDO, false);

    m_GuiDialog->Enable(ID_MANUAL_SEGMENTATION,true);
  }
  else if (m_CurrentOperation == REFINEMENT_SEGMENTATION)
  {
    m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_APPLY, m_CurrentSlicePlane == XY);

    m_View->VmeShow(m_ThresholdVolume,false);

    if(m_SegmentatedVolume->GetRefinementOutput()->IsA("vtkStructuredPoints"))
    {
      m_ThresholdVolume->SetData( vtkStructuredPoints::SafeDownCast(m_SegmentatedVolume->GetManualOutput()),0.0 );
    }
    else
    {
      m_ThresholdVolume->SetData( vtkRectilinearGrid::SafeDownCast(m_SegmentatedVolume->GetRefinementOutput()), mafVME::SafeDownCast(m_Volume)->GetTimeStamp() );
    }
    m_ThresholdVolume->Update();

    m_View->VmeShow(m_ThresholdVolume,true);

    m_SnippetsLabel->SetLabel(_(""));
    m_Dialog->Update();

    m_GuiDialog->Enable(ID_REFINEMENT,true);
    m_GuiDialog->Enable(ID_BUTTON_NEXT,true);
  }
  else if (m_CurrentOperation == LOAD_SEGMENTATION)
  {
    m_GuiDialog->Enable(ID_LOAD_SEGMENTATION,true);
  }

  int oldSliceIndex = m_CurrentSliceIndex;

  m_SegmentationOperationsRollOut[m_OldOperation]->RollOut(false);
  m_SegmentationOperationsRollOut[m_CurrentOperation]->RollOut(true);

  m_CurrentSliceIndex = oldSliceIndex;

  m_SliceSlider->SetValue(m_CurrentSliceIndex);
  m_SliceSlider->Update();

  UpdateSlice();

  m_View->CameraUpdate();

}

//----------------------------------------------------------------------------
void medOpSegmentation::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    if(e->GetSender() == m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION])
    {
      OnAutomaticSegmentationEvent(e);
      return;
    }
    if(e->GetSender() == m_SegmentationOperationsGui[MANUAL_SEGMENTATION])
    {
      OnManualSegmentationEvent(e);
      return;
    }
    if(e->GetSender() == m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION])
    {
      OnRefinementSegmentationEvent(e);
      return;
    }
    switch(e->GetId())
    {
    case ID_BUTTON_NEXT:
    case ID_BUTTON_PREV:
      {
        OnChangeStep(e->GetId());
      }
      break;
    case ID_SLICE_SLIDER:
      {
        if (m_NumSliceSliderEvents == 2)//Validator generate 2 events when the user move the slider
        {
          m_NumSliceSliderEvents = 0;

          if(m_CurrentSliceIndex<=1)
            m_CurrentSliceIndex = 1;
          else if( m_CurrentSliceIndex>m_SliceSlider->GetMax() )
            m_CurrentSliceIndex = m_SliceSlider->GetMax();

          if (m_CurrentSliceIndex != m_OldSliceIndex)
          {
            if (m_ManualModifiedWithoutApplied)
            {
              int answer = wxMessageBox(_("Current slice changes\nhave not been saved.\nDo you want to continue?"),_("Warning"),wxYES_NO|wxCENTER);
              if (answer == wxYES)
              {
                m_ManualModifiedWithoutApplied = false;
                UpdateSlice();
              }
              else
              {
                m_CurrentSliceIndex = m_OldSliceIndex;
                m_GuiDialog->Update();
              }
            }
            else
            {
              UpdateSlice();
            }
          }
        }
        else
        {
          m_NumSliceSliderEvents++;
        }

        UpdateSlice();

      }
      break;
    case ID_SLICE_NEXT:
      {
        if(m_CurrentSliceIndex<1)
          m_CurrentSliceIndex = 1;
        else if(m_CurrentSliceIndex<m_SliceSlider->GetMax())
          m_CurrentSliceIndex ++;
        else 
          m_CurrentSliceIndex = m_SliceSlider->GetMax();

        if (m_CurrentSliceIndex != m_OldSliceIndex)
        {
          if (m_ManualModifiedWithoutApplied)
          {
            int answare = wxMessageBox(_("Current slice changes\nhave not been saved.\nDo you want to continue?"),_("Warining"),wxYES_NO|wxCENTER);
            if (answare == wxYES)
            {
              m_ManualModifiedWithoutApplied = false;
              UpdateSlice();
              m_GuiDialog->Update();

              m_View->CameraUpdate();
            }
            else
            {
              m_CurrentSliceIndex = m_OldSliceIndex;
            }
          }
          else
          {
            UpdateSlice();
            m_GuiDialog->Update();

            m_View->CameraUpdate();
          }
        }

        break;
      }
    case ID_SLICE_PREV:
      {
        if(m_CurrentSliceIndex>m_SliceSlider->GetMax())
          m_CurrentSliceIndex = m_SliceSlider->GetMax();
        else if(m_CurrentSliceIndex>1)
          m_CurrentSliceIndex --;
        else
          m_CurrentSliceIndex = 1;

        if (m_CurrentSliceIndex != m_OldSliceIndex)
        {
          if (m_ManualModifiedWithoutApplied)
          {
            int answare = wxMessageBox(_("Current slice changes\nhave not been saved.\nDo you want to continue?"),_("Warining"),wxYES_NO|wxCENTER);
            if (answare == wxYES)
            {
              m_ManualModifiedWithoutApplied = false;
              UpdateSlice();
              m_GuiDialog->Update();

              m_View->CameraUpdate();
            }
            else
            {
              m_CurrentSliceIndex = m_OldSliceIndex;
            }
          }
          else
          {
            UpdateSlice();
            m_GuiDialog->Update();

            m_View->CameraUpdate();
          }
        }

        break;
      }
    case ID_SLICE_TEXT:
      {
        if (m_CurrentSliceIndex != m_OldSliceIndex)
        {
          if (m_ManualModifiedWithoutApplied)
          {
            int answare = wxMessageBox(_("Current slice changes\nhave not been saved.\nDo you want to continue?"),_("Warining"),wxYES_NO|wxCENTER);
            if (answare == wxYES)
            {
              m_ManualModifiedWithoutApplied = false;
              UpdateSlice();
              m_GuiDialog->Update();

              m_View->CameraUpdate();
            }
            else
            {
              m_CurrentSliceIndex = m_OldSliceIndex;
            }
          }
          else
          {
            UpdateSlice();
            m_GuiDialog->Update();

            m_View->CameraUpdate();
          }
        }

        break;
      }
    case ID_SLICE_PLANE:
      {
        switch(m_CurrentSlicePlane)
        {
        case XY:
          m_CameraPositionId = CAMERA_OS_Z;
          break;
        case XZ:
          m_CameraPositionId = CAMERA_OS_Y;
          break;
        case YZ:
          m_CameraPositionId = CAMERA_OS_X;
          break;
        default:
          break;
        }

        m_CurrentSliceIndex = 1;
        UpdateSlice();

        m_View->ChangeView(m_CurrentSlicePlane);
        InitGui();

        m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_PICKING_MODALITY, m_CurrentSlicePlane == XY);
        m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_OK, m_CurrentSlicePlane == XY && m_ManualModifiedWithoutApplied);
        m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_APPLY, m_CurrentSlicePlane == XY);

        if (m_CurrentOperation == AUTOMATIC_SEGMENTATION)
        {
          OnChangeThresholdType();
        }
        else if(m_CurrentOperation == MANUAL_SEGMENTATION)
        {
          if(m_CurrentSlicePlane == XY && m_ManualSegmentationMode == MANUAL_SEGMENTATION_BRUSH)
          {
            m_Volume->SetBehavior(m_ManualPicker);
            wxCursor cursor = wxCursor( wxCURSOR_PENCIL );
            m_View->GetWindow()->SetCursor(cursor);

            m_ManualPER->EnableDrawing(true);
            m_ManualPER->SetRadius(m_ManualBrushSize/2);

            InitManualVolumeSlice();

            m_View->VmeShow(m_ThresholdVolume, false);
            m_View->VmeShow(m_ManualVolumeSlice, false);
            m_View->VmeShow(m_ManualVolumeSlice, true);
          }
          else if(m_CurrentSlicePlane == XY)
          {
            InitManualVolumeSlice();

            m_View->VmeShow(m_ThresholdVolume, false);
            m_View->VmeShow(m_ManualVolumeSlice, false);
            m_View->VmeShow(m_ManualVolumeSlice, true);
          }
          else 
          {
            if(m_ManualSegmentationMode == MANUAL_SEGMENTATION_BRUSH)
            {
              m_Volume->SetBehavior(m_OldBehavior);
              m_View->GetWindow()->SetCursor(wxCursor( wxCURSOR_DEFAULT ));

              m_ManualPER->RemoveActor();
              m_ManualPER->EnableDrawing(false);
              m_View->CameraUpdate();

            }

            m_View->VmeShow(m_ManualVolumeSlice, false);
            m_View->VmeShow(m_ThresholdVolume, false);
            m_View->VmeShow(m_ThresholdVolume, true);
          }

          m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Update();
        }

        m_View->CameraUpdate();

        break;
      }
    case VME_PICKED:
      {
        //////////////////////////////////////////////////////////////////////////
        //Picking during automatic segmentation
        //////////////////////////////////////////////////////////////////////////
        if (e->GetSender() == m_AutomaticPicker && m_CurrentOperation==AUTOMATIC_SEGMENTATION)
        {
          OnAutomaticPicker(e);
          UpdateSlice();
          break;
        }
       
        if(m_CurrentOperation == MANUAL_SEGMENTATION && m_ManualSegmentationMode == MANUAL_SEGMENTATION_BRUSH)
        {
          if(m_ManualContinuousPickingOn)
          {
            if(m_ManualUndoCounter)
            {
              mafLogMessage("Picked CP counter +");

              vtkUnsignedCharArray *scalars = vtkUnsignedCharArray::New();
              scalars->DeepCopy( m_ManualVolumeSlice->GetOutput()->GetVTKData()->GetPointData()->GetScalars() );
              scalars->SetName("SCALARS");

              m_ManualUndoList.push_back( scalars );

              m_ManualUndoCounter = 0;


            }
            else
            {

              mafLogMessage("Picked CP counter 0");
              OnBrushEvent(e);

              m_ManualUndoCounter++;
            }
          }
          else
          {
            m_ManualUndoCounter = 1;

            mafLogMessage("Picked counter +");

            vtkUnsignedCharArray *scalars = vtkUnsignedCharArray::New();
            scalars->DeepCopy( m_ManualVolumeSlice->GetOutput()->GetVTKData()->GetPointData()->GetScalars() );
            scalars->SetName("SCALARS");

            m_ManualUndoList.push_back( scalars );
            OnBrushEvent(e);
          }
        }

        break;
      }
    case VME_PICKING:
      {
        if(m_CurrentOperation == MANUAL_SEGMENTATION && m_ManualContinuousPickingOn)
        {
          if(m_ManualUndoCounter)
            mafLogMessage("Picking counter +");
          else
            mafLogMessage("Picking counter 0");
          OnBrushEvent(e);
        }
      }
      break;
    case ID_OK:
      {
        m_Dialog->EndModal(wxID_OK);
        break;
      }
    case ID_CANCEL:
      {
        m_Dialog->EndModal(wxID_CANCEL);
        break;
      }
    case ID_RANGE_MODIFIED:
      {
        //threshold slider
        if (e->GetSender()==m_AutomaticThresholdSlider)
        {
          m_AutomaticThresholdSlider->GetSubRange(&m_AutomaticThreshold,&m_AutomaticUpperThreshold);
          UpdateThresholdLabel();
        }
        //Windowing
        else
       {
          double low, hi;
          m_LutSlider->GetSubRange(&low,&hi);
          m_ColorLUT->SetTableRange(low,hi);
          m_View->CameraUpdate();
          mafEventMacro(mafEvent(this,CAMERA_UPDATE));
          break;
        }
      }
    case ID_LUT_CHOOSER:
      {
        double *sr;
        sr = m_ColorLUT->GetRange();
        m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);
        m_View->CameraUpdate();
        break;
      }
    case MOUSE_MOVE:
      {
        if (e->GetSender() == m_AutomaticPER || e->GetSender() == m_ManualPER)
        {
          double value = e->GetDouble();
          mafString text = wxString::Format("Scalar = %.3f",value);
          m_AutomaticScalarTextMapper->SetInput(text.GetCStr());

          m_View->CameraUpdate();
        }
      }
      break;
    default:
      mafEventMacro(*e);
    }
  }
  else
  {
    mafEventMacro(*e);
  }
}
//------------------------------------------------------------------------
void medOpSegmentation::OnBrushEvent(mafEvent *e)
//------------------------------------------------------------------------
{
  vtkPoints *point = vtkPoints::SafeDownCast(e->GetVtkObj());
  if (point)
  {

    vtkDataSet *dataset = vtkDataSet::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_Volume)->GetOutput()->GetVTKData());
    dataset->Update();

    int id;
    id = e->GetArg();
    double datasetPoint[3];
    dataset->GetPoint(id,datasetPoint);

    if(m_ManualSegmentationAction == MANUAL_SEGMENTATION_SELECT)
    {
      SelectBrushImage(datasetPoint[0], datasetPoint[1], datasetPoint[2], true);
    }
    else if(m_ManualSegmentationAction == MANUAL_SEGMENTATION_ERASE)
    {
      SelectBrushImage(datasetPoint[0], datasetPoint[1], datasetPoint[2], false);
    }

    m_ManualModifiedWithoutApplied = true;

    m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_OK, m_CurrentSlicePlane == XY && m_ManualModifiedWithoutApplied);
    m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_UNDO, m_ManualUndoList.size()>0);

    m_View->CameraUpdate();
  }
}

//------------------------------------------------------------------------
void medOpSegmentation::SaveRefinementVolumeMask()
//------------------------------------------------------------------------
{
  vtkDataSet *dataSetRefinement = ((mafVME *)m_ThresholdVolume)->GetOutput()->GetVTKData();
  vtkDataSet *dataSetManual = m_SegmentatedVolume->GetManualOutput();
  vtkMAFSmartPointer<vtkUnsignedCharArray> newScalars;
  newScalars->SetName("SCALARS");
  newScalars->SetNumberOfTuples(dataSetRefinement->GetPointData()->GetScalars()->GetNumberOfTuples());

  long progress = 0;
  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));

  int step = ceil((double)dataSetRefinement->GetPointData()->GetScalars()->GetNumberOfTuples()/100);
  for (int i=0;i<dataSetRefinement->GetPointData()->GetScalars()->GetNumberOfTuples();i++)
  {
    if ((i%step) == 0)
    {
      progress = (i/dataSetRefinement->GetPointData()->GetScalars()->GetNumberOfTuples())*100;
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
    }
    if (dataSetRefinement->GetPointData()->GetScalars()->GetTuple1(i) == dataSetManual->GetPointData()->GetScalars()->GetTuple1(i))
    {
      newScalars->SetTuple1(i,0);
    }
    else
    {
      newScalars->SetTuple1(i,255);
    }
  }

  mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));

  if (m_RefinementVolumeMask == NULL)
  {
    mafNEW(m_RefinementVolumeMask);
    m_RefinementVolumeMask->ReparentTo(m_SegmentatedVolume);
    m_RefinementVolumeMask->SetName("Refinement mask");
    m_RefinementVolumeMask->Update();
  }

  if(dataSetRefinement->IsA("vtkStructuredPoints"))
  {
    vtkMAFSmartPointer<vtkStructuredPoints> sp;
    sp->CopyStructure(dataSetRefinement);
    sp->GetPointData()->AddArray(newScalars);
    sp->GetPointData()->SetActiveScalars("SCALARS");
    sp->SetScalarTypeToUnsignedChar();
    sp->Update();

    m_RefinementVolumeMask->SetData(sp,0.0);
    m_RefinementVolumeMask->Update();
  }
  else
  {
    vtkMAFSmartPointer<vtkRectilinearGrid> rg;
    rg->CopyStructure(dataSetRefinement);

    rg->GetPointData()->AddArray(newScalars);
    rg->GetPointData()->SetActiveScalars("SCALARS");
    rg->Update();

    m_RefinementVolumeMask->SetData(rg,0.0);
    m_RefinementVolumeMask->Update();
  }


#ifdef _DEBUG
  m_RefinementVolumeMask->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
#endif

  m_SegmentatedVolume->SetRefinementVolumeMask(m_RefinementVolumeMask);
  m_SegmentatedVolume->GetOutput()->Update();
  m_SegmentatedVolume->Update();
}
//------------------------------------------------------------------------
void medOpSegmentation::SaveManualVolumeMask()
//------------------------------------------------------------------------
{
  vtkDataSet *dataSetManual = ((mafVME *)m_ThresholdVolume)->GetOutput()->GetVTKData();
  vtkDataSet *dataSetAutomatic = m_SegmentatedVolume->GetRegionGrowingOutput();
  vtkMAFSmartPointer<vtkUnsignedCharArray> newScalars;
  newScalars->SetName("SCALARS");
  newScalars->SetNumberOfTuples(dataSetManual->GetPointData()->GetScalars()->GetNumberOfTuples());

  long progress = 0;
  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));

  int step = ceil((double)dataSetManual->GetPointData()->GetScalars()->GetNumberOfTuples()/100);
  for (int i=0;i<dataSetManual->GetPointData()->GetScalars()->GetNumberOfTuples();i++)
  {
    if ((i%step) == 0)
    {
      progress = (long)((double)(i/dataSetManual->GetPointData()->GetScalars()->GetNumberOfTuples())*100);
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
    }
    if (dataSetManual->GetPointData()->GetScalars()->GetTuple1(i) == dataSetAutomatic->GetPointData()->GetScalars()->GetTuple1(i))
    {
      newScalars->SetTuple1(i,0);
    }
    else
    {
      newScalars->SetTuple1(i,255);
    }
  }

  mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));

  if (m_ManualVolumeMask == NULL)
  {
    mafNEW(m_ManualVolumeMask);
    m_ManualVolumeMask->ReparentTo(m_SegmentatedVolume);
    m_ManualVolumeMask->SetName("Manual mask");
    m_ManualVolumeMask->Update();
  }

  if(dataSetManual->IsA("vtkStructuredPoints"))
  {
    vtkMAFSmartPointer<vtkStructuredPoints> sp;
    sp->CopyStructure(dataSetManual);
    sp->GetPointData()->AddArray(newScalars);
    sp->GetPointData()->SetActiveScalars("SCALARS");
    sp->SetScalarTypeToUnsignedChar();
    sp->Update();

    m_ManualVolumeMask->SetData(sp,0.0);
    m_ManualVolumeMask->Update();
  }
  else
  {
    vtkMAFSmartPointer<vtkRectilinearGrid> rg;
    rg->CopyStructure(dataSetManual);

    rg->GetPointData()->AddArray(newScalars);
    rg->GetPointData()->SetActiveScalars("SCALARS");
    //rg->SetScalarTypeToUnsignedChar();
    rg->Update();

    m_ManualVolumeMask->SetData(rg,0.0);
    m_ManualVolumeMask->Update();
  }


#ifndef _DEBUG
  m_ManualVolumeMask->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
#endif

  m_SegmentatedVolume->SetManualVolumeMask(m_ManualVolumeMask);
  m_SegmentatedVolume->GetOutput()->Update();
  m_SegmentatedVolume->Update();

}
//------------------------------------------------------------------------
void medOpSegmentation::OnChangeThresholdType()
//------------------------------------------------------------------------
{
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_GLOBAL_THRESHOLD,m_CurrentSlicePlane == XY);
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_THRESHOLD,m_CurrentSlicePlane == XY);
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_ADD_RANGE,m_AutomaticGlobalThreshold == RANGE && m_CurrentSlicePlane == XY);
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_REMOVE_RANGE,m_AutomaticGlobalThreshold == RANGE && m_CurrentSlicePlane == XY);
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_UPDATE_RANGE,m_AutomaticGlobalThreshold == RANGE && m_CurrentSlicePlane == XY);
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_LIST_OF_RANGE,m_AutomaticGlobalThreshold == RANGE && m_CurrentSlicePlane == XY);
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_GLOBAL_PREVIEW,m_AutomaticGlobalThreshold == GLOBAL && m_CurrentSlicePlane == XY);
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_INCREASE_MIN_RANGE_VALUE,m_AutomaticGlobalThreshold == RANGE && m_CurrentSlicePlane == XY);
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_INCREASE_MAX_RANGE_VALUE,m_AutomaticGlobalThreshold == RANGE && m_CurrentSlicePlane == XY);
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_DECREASE_MIN_RANGE_VALUE,m_AutomaticGlobalThreshold == RANGE && m_CurrentSlicePlane == XY);
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_DECREASE_MAX_RANGE_VALUE,m_AutomaticGlobalThreshold == RANGE && m_CurrentSlicePlane == XY);
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_DECREASE_MIDDLE_RANGE_VALUE,m_AutomaticGlobalThreshold == RANGE && m_CurrentSlicePlane == XY);
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_INCREASE_MIDDLE_RANGE_VALUE,m_AutomaticGlobalThreshold == RANGE && m_CurrentSlicePlane == XY);
  m_AutomaticRangeSlider->Enable(m_AutomaticGlobalThreshold == RANGE && m_CurrentSlicePlane == XY);
  if(m_CurrentOperation == AUTOMATIC_SEGMENTATION)
    //m_GuiDialog->Enable(ID_BUTTON_NEXT,(CheckNumberOfThresholds() && m_AutomaticGlobalThreshold == RANGE));
    m_GuiDialog->Enable(ID_BUTTON_NEXT,m_AutomaticGlobalThreshold == RANGE);
  UpdateThresholdLabel();
}

//------------------------------------------------------------------------
void medOpSegmentation::OnAutomaticAddRange()
//------------------------------------------------------------------------
{
  double min,max;
  m_AutomaticRangeSlider->GetSubRange(&min,&max);
  int iMin = round(min)-1;
  int iMax = round(max)-1;


  if (!AutomaticCheckRange())
  {
    return;
  }

  //Store the parameters
  AutomaticInfoRange range;
  range.m_EndSlice = iMax;
  range.m_StartSlice = iMin;
  range.m_ThresholdValue = m_AutomaticThreshold;
  range.m_UpperThresholdValue = m_AutomaticUpperThreshold;

  m_AutomaticRanges.push_back(range);

  m_AutomaticListOfRange->Append(wxString::Format("[%d,%d] low:%.3f high:%.3f",range.m_StartSlice+1,range.m_EndSlice+1,m_AutomaticThreshold,m_AutomaticUpperThreshold));

  m_GuiDialog->Enable(ID_BUTTON_NEXT,true);

  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Update();

  UpdateThresholdLabel();

  OnAutomaticPreview();
}

//----------------------------------------------------------------------------
void medOpSegmentation::SetSelectionAutomaticListOfRange(int index)
//----------------------------------------------------------------------------
{
  m_AutomaticListOfRange->SetSelection(index);
  m_AutomaticListOfRange->Update();
  wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED,m_AutomaticListOfRange->GetId());
  event.SetEventObject( m_AutomaticListOfRange );
  event.SetInt(index);
  m_AutomaticListOfRange->GetEventHandler()->ProcessEvent(event);
}


//------------------------------------------------------------------------
void medOpSegmentation::OnAutomaticRemoveRange()
//------------------------------------------------------------------------
{
  if (m_AutomaticListOfRange->GetSelection()<0)
  {
    return;
  }

  int j = 0;
  for (int i=0;i<m_AutomaticRanges.size();i++)
  {
    if (i != m_AutomaticListOfRange->GetSelection())
    {
      m_AutomaticRanges[j] = m_AutomaticRanges[i];
      j++;
    }
  }
  m_AutomaticListOfRange->Delete(m_AutomaticListOfRange->GetSelection());

  if (m_AutomaticListOfRange->GetCount()>0)
  {
    SetSelectionAutomaticListOfRange(0);
  }
  else
  {
    SetSelectionAutomaticListOfRange(-1);
  }

  m_AutomaticRanges.pop_back();

  m_GuiDialog->Enable(ID_BUTTON_NEXT, true);

  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Update();

  UpdateThresholdLabel();

  OnAutomaticPreview();
}

//------------------------------------------------------------------------
void medOpSegmentation::OnAutomaticPreview()
//------------------------------------------------------------------------
{
  wxBusyCursor wait_cursor;
  wxBusyInfo wait(_("Creating preview: Please wait"));
  //////////////////////////////////////////////////////////////////////////
  //PREVIEW
  //////////////////////////////////////////////////////////////////////////
  m_SegmentatedVolume->RemoveAllRanges();
  if (m_AutomaticGlobalThreshold == RANGE)
  {
    int result;
    m_SegmentatedVolume->SetAutomaticSegmentationThresholdModality(medVMESegmentationVolume::RANGE);
    for (int i=0;i<m_AutomaticRanges.size();i++)
    {
      result = m_SegmentatedVolume->AddRange(m_AutomaticRanges[i].m_StartSlice,m_AutomaticRanges[i].m_EndSlice,m_AutomaticRanges[i].m_ThresholdValue,m_AutomaticRanges[i].m_UpperThresholdValue);
      if (result == MAF_ERROR)
      {
        return;
      }
    }
  }
  else
  {
    m_SegmentatedVolume->SetAutomaticSegmentationThresholdModality(medVMESegmentationVolume::GLOBAL);
    m_SegmentatedVolume->SetAutomaticSegmentationGlobalThreshold(m_AutomaticThreshold,m_AutomaticUpperThreshold);
  }

  m_SegmentatedVolume->GetOutput()->Update();
  m_SegmentatedVolume->Update();

  if (vtkStructuredPoints::SafeDownCast(m_SegmentatedVolume->GetOutput()->GetVTKData()))
  {
    vtkStructuredPoints *newData = vtkStructuredPoints::SafeDownCast(m_SegmentatedVolume->GetAutomaticOutput());
    m_ThresholdVolume->SetData(newData,mafVME::SafeDownCast(m_Volume)->GetTimeStamp());
    vtkStructuredPoints *spVME = vtkStructuredPoints::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_ThresholdVolume)->GetOutput()->GetVTKData());
    spVME->Update();

  }
  else
  {
    vtkRectilinearGrid *newData = vtkRectilinearGrid::SafeDownCast(m_SegmentatedVolume->GetAutomaticOutput());
    m_ThresholdVolume->SetData(newData,mafVME::SafeDownCast(m_Volume)->GetTimeStamp());
    vtkRectilinearGrid *rgVME = vtkRectilinearGrid::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_ThresholdVolume)->GetOutput()->GetVTKData());
    rgVME->Update();
  }

  m_ThresholdVolume->Update();

  m_SegmentationColorLUT = m_ThresholdVolume->GetMaterial()->m_ColorLut;
  InitSegmentationColorLut();

  m_ThresholdVolume->GetMaterial()->m_ColorLut->SetTableRange(0,255);
  mmaVolumeMaterial *currentVolumeMaterial = ((mafVMEOutputVolume *)m_ThresholdVolume->GetOutput())->GetMaterial();
  currentVolumeMaterial->UpdateFromTables();

  m_View->VmeShow(m_ThresholdVolume,false);
  m_View->VmeShow(m_ThresholdVolume,true);

  m_View->CameraUpdate();
  //////////////////////////////////////////////////////////////////////////
}
//------------------------------------------------------------------------
void medOpSegmentation::OnAutomaticUpdateRange()
//------------------------------------------------------------------------
{
  if (m_AutomaticListOfRange->GetSelection()<0)
  {
    return;
  }

  double min,max;
  m_AutomaticRangeSlider->GetSubRange(&min,&max);
  int iMin = round(min)-1;
  int iMax = round(max)-1;

  if (AutomaticCheckRange(m_AutomaticListOfRange->GetSelection()))
  {
    double min,max;
    m_AutomaticRangeSlider->GetSubRange(&min,&max);

    m_AutomaticRanges[m_AutomaticListOfRange->GetSelection()].m_ThresholdValue = m_AutomaticThreshold;
    m_AutomaticRanges[m_AutomaticListOfRange->GetSelection()].m_UpperThresholdValue = m_AutomaticUpperThreshold;
    m_AutomaticRanges[m_AutomaticListOfRange->GetSelection()].m_StartSlice = iMin;
    m_AutomaticRanges[m_AutomaticListOfRange->GetSelection()].m_EndSlice = iMax;

    wxString line = wxString::Format("[%d,%d] low %.3f high %.3f",iMin+1,iMax+1,m_AutomaticThreshold,m_AutomaticUpperThreshold);
    m_AutomaticListOfRange->SetString(m_AutomaticListOfRange->GetSelection(),line);

    m_GuiDialog->Enable(ID_BUTTON_NEXT, true);

    m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Update();

    UpdateThresholdLabel();

    OnAutomaticPreview();
  }
}

//------------------------------------------------------------------------
void medOpSegmentation::OnAutomaticChangeRangeManually(int eventID)
//------------------------------------------------------------------------
{
  double subMin,subMax;
  m_AutomaticRangeSlider->GetSubRange(&subMin,&subMax);
  
  switch(eventID)
  {
  case ID_AUTOMATIC_INCREASE_MIN_RANGE_VALUE:
      subMin++;
    break;
  case ID_AUTOMATIC_INCREASE_MAX_RANGE_VALUE:
      subMax++;
    break;
  case ID_AUTOMATIC_DECREASE_MAX_RANGE_VALUE:
      subMax--;
    break;
  case ID_AUTOMATIC_DECREASE_MIN_RANGE_VALUE:
      subMin--;
    break;
  case ID_AUTOMATIC_INCREASE_MIDDLE_RANGE_VALUE:
      subMin++;
      subMax++;
    break;
  case ID_AUTOMATIC_DECREASE_MIDDLE_RANGE_VALUE:
      subMin--;
      subMax--;
    break;
  }

  m_AutomaticRangeSlider->SetSubRange(subMin,subMax);
}


//------------------------------------------------------------------------
void medOpSegmentation::OnAutomaticChangeThresholdManually(int eventID)
//------------------------------------------------------------------------
{
  m_AutomaticThresholdSlider->GetSubRange(&m_AutomaticThreshold,&m_AutomaticUpperThreshold);
  //Fine tuning threshold selection, get the event and update relative values
  switch(eventID)
  {
  case ID_AUTOMATIC_INCREASE_MIN_THRESHOLD:
    m_AutomaticThreshold++;
    break;
  case ID_AUTOMATIC_INCREASE_MAX_THRESHOLD:
    m_AutomaticUpperThreshold++;
    break;
  case ID_AUTOMATIC_DECREASE_MAX_THRESHOLD:
    m_AutomaticUpperThreshold--;
    break;
  case ID_AUTOMATIC_DECREASE_MIN_THRESHOLD:
    m_AutomaticThreshold--;
    break;
  case ID_AUTOMATIC_INCREASE_MIDDLE_THRESHOLD:
    m_AutomaticThreshold++;
    m_AutomaticUpperThreshold++;
    break;
  case ID_AUTOMATIC_DECREASE_MIDDLE_THRESHOLD:
    m_AutomaticThreshold--;
    m_AutomaticUpperThreshold--;
    break;
  }
  m_AutomaticThresholdSlider->SetSubRange(m_AutomaticThreshold,m_AutomaticUpperThreshold);
  
  UpdateThresholdLabel();
}

//------------------------------------------------------------------------
void medOpSegmentation::OnAutomaticSegmentationEvent(mafEvent *e)
//------------------------------------------------------------------------
{
  switch(e->GetId())
  {
  case ID_AUTOMATIC_INCREASE_MIN_RANGE_VALUE:
  case ID_AUTOMATIC_INCREASE_MAX_RANGE_VALUE:
  case ID_AUTOMATIC_INCREASE_MIDDLE_RANGE_VALUE:
  case ID_AUTOMATIC_DECREASE_MIN_RANGE_VALUE:
  case ID_AUTOMATIC_DECREASE_MAX_RANGE_VALUE:
  case ID_AUTOMATIC_DECREASE_MIDDLE_RANGE_VALUE:
    {
      OnAutomaticChangeRangeManually(e->GetId());
    }
    break;
  case ID_AUTOMATIC_INCREASE_MIN_THRESHOLD:
  case ID_AUTOMATIC_INCREASE_MAX_THRESHOLD:
  case ID_AUTOMATIC_INCREASE_MIDDLE_THRESHOLD:
  case ID_AUTOMATIC_DECREASE_MIN_THRESHOLD:
  case ID_AUTOMATIC_DECREASE_MAX_THRESHOLD:
  case ID_AUTOMATIC_DECREASE_MIDDLE_THRESHOLD:
    {
      OnAutomaticChangeThresholdManually(e->GetId()); 
    }
    break;
  case mafGUILutSlider::ID_MOUSE_D_CLICK_LEFT:
    {
      double min,max;
      m_AutomaticRangeSlider->GetSubRange(&min,&max);
      switch(e->GetArg())
      {
      case mafGUILutSlider::MAX_BUTTON:
        {
          if (m_CurrentSliceIndex<=min)
          {
            m_AutomaticRangeSlider->SetSubRange(min,min);
          }
          else
          {
            //m_AutomaticRangeSlider->DisableMouseMode();
            m_AutomaticRangeSlider->SetSubRange(min,m_CurrentSliceIndex);
            //m_AutomaticRangeSlider->ReleaseButton();
            //m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Update();
            //m_AutomaticRangeSlider->EnableMouseMode();
          }
        }
        break;
      case mafGUILutSlider::MIN_BUTTON:
        {
          if (m_CurrentSliceIndex>=max)
          {
            m_AutomaticRangeSlider->SetSubRange(max,max);
          }
          else
          {
            //m_AutomaticRangeSlider->DisableMouseMode();
            m_AutomaticRangeSlider->SetSubRange(m_CurrentSliceIndex,max);
            //m_AutomaticRangeSlider->ReleaseButton();
            //m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Update();
            //m_AutomaticRangeSlider->EnableMouseMode();
          }
        }
        break;
      case mafGUILutSlider::MIDDLE_BUTTON:
        {
          double diff = (max-min)/2;
          min = m_CurrentSliceIndex-diff;
          max = m_CurrentSliceIndex+diff;
          //m_AutomaticRangeSlider->DisableMouseMode();
          m_AutomaticRangeSlider->SetSubRange(min,max);
          //m_AutomaticRangeSlider->ReleaseButton();
          //m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Update();
          //m_AutomaticRangeSlider->EnableMouseMode();
        }
        break;
      }
    }
    break;
  case ID_AUTOMATIC_GLOBAL_PREVIEW:
    {
      OnAutomaticPreview();
      m_GuiDialog->Enable(ID_BUTTON_NEXT,true);
    }
    break;
  case ID_AUTOMATIC_THRESHOLD:
    {
      m_AutomaticThresholdSlider->GetSubRange(&m_AutomaticThreshold,&m_AutomaticUpperThreshold);
      UpdateThresholdLabel();
    }
    break;
  case ID_AUTOMATIC_GLOBAL_THRESHOLD:
    {
      OnChangeThresholdType();
      if (m_AutomaticGlobalThreshold == RANGE)
      {
        wxMessageBox(_("Tip: Double click on the sliding button to set it to the current slice value"));
      }
      m_GuiDialog->Enable(ID_BUTTON_NEXT,m_AutomaticGlobalThreshold==RANGE);
    }
    break;
  case ID_AUTOMATIC_UPDATE_RANGE:
    {
      OnAutomaticUpdateRange();
    }
    break;
  case ID_AUTOMATIC_LIST_OF_RANGE:
    {
      if (m_AutomaticListOfRange->GetSelection() != -1)//Check if a range is selected
      {
        double min,max;

        m_AutomaticThreshold = m_AutomaticRanges[m_AutomaticListOfRange->GetSelection()].m_ThresholdValue;
        m_AutomaticUpperThreshold = m_AutomaticRanges[m_AutomaticListOfRange->GetSelection()].m_UpperThresholdValue;
        min = m_AutomaticRanges[m_AutomaticListOfRange->GetSelection()].m_StartSlice+1;
        max = m_AutomaticRanges[m_AutomaticListOfRange->GetSelection()].m_EndSlice+1;

        m_AutomaticRangeSlider->SetSubRange(min,max);

        m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Update();
       
        m_AutomaticThresholdSlider->SetSubRange(m_AutomaticThreshold,m_AutomaticUpperThreshold);
      }
    }
    break;
  case ID_AUTOMATIC_ADD_RANGE:
    {
      OnAutomaticAddRange();
    }
    break;
  case ID_AUTOMATIC_REMOVE_RANGE:
    {
      OnAutomaticRemoveRange();
    }
    break;
  default:
    mafEventMacro(*e);
  }

}

//------------------------------------------------------------------------
void medOpSegmentation::OnManualSegmentationEvent(mafEvent *e)
//------------------------------------------------------------------------
{
  switch(e->GetId())
  {
  case ID_MANUAL_MODE:
    {
      EnableManualSegmentationGui();
      if(m_ManualSegmentationMode == MANUAL_SEGMENTATION_BRUSH)
      {
        m_Volume->SetBehavior(m_ManualPicker);
        //m_ManualPicker->SetLockDevice(false);
        wxCursor cursor = wxCursor( wxCURSOR_PENCIL );
        m_View->GetWindow()->SetCursor(cursor);

        m_ManualPER->EnableDrawing(true);
      }
      else 
      {
        m_Volume->SetBehavior(m_AutomaticPicker);
        //m_OldBehavior->SetLockDevice(false);
        m_View->GetWindow()->SetCursor(wxCursor( wxCURSOR_DEFAULT ));


        m_ManualPER->RemoveActor();
        m_ManualPER->EnableDrawing(false);
        m_View->CameraUpdate();

      }
      break;
    }

  case ID_MANUAL_CONTINUOUS_PICKING:
    {
      m_ManualPicker->EnableContinuousPicking(m_ManualContinuousPickingOn);
      break;
    }
  case ID_MANUAL_BRUSH_SHAPE:
    {
      m_ManualBrushShape = m_ManualBrushShapeRadioBox->GetSelection();
      if(m_ManualBrushShape == CIRCLE_BRUSH_SHAPE)
        m_ManualPER->SetBrushShape(CIRCLE_BRUSH_SHAPE);
      else 
        m_ManualPER->SetBrushShape(SQUARE_BRUSH_SHAPE);
      m_View->CameraUpdate();
      break;
    }
  case ID_MANUAL_BRUSH_SIZE:
    {
      m_ManualPER->SetRadius(m_ManualBrushSize/2);
      m_View->CameraUpdate();
      break;
    }
  case ID_MANUAL_REFINEMENT_FILLIN:
    {
      m_RefinementSegmentationAction = ID_REFINEMENT_HOLES_FILL;

      vtkUnsignedCharArray *scalars = vtkUnsignedCharArray::New();
      scalars->DeepCopy( m_ManualVolumeSlice->GetOutput()->GetVTKData()->GetPointData()->GetScalars() );
      scalars->SetName("SCALARS");

      m_ManualUndoList.push_back( scalars );

      if (!ManualVolumeSliceRefinement())
      {
        break;
      }

      //SaveRefinementVolumeMask();

      m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_UNDO, m_ManualUndoList.size()>0);
      m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_REDO, m_ManualRedoList.size()>0);

      m_ManualModifiedWithoutApplied = true;
      m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_OK, true);

      m_View->CameraUpdate();
      break;
    }
  case ID_MANUAL_REFINEMENT_REMOVE:
    {
      m_RefinementSegmentationAction = ID_REFINEMENT_ISLANDS_REMOVE;

      vtkUnsignedCharArray *scalars = vtkUnsignedCharArray::New();
      scalars->DeepCopy( m_ManualVolumeSlice->GetOutput()->GetVTKData()->GetPointData()->GetScalars() );
      scalars->SetName("SCALARS");

      m_ManualUndoList.push_back( scalars );

      if (!ManualVolumeSliceRefinement())
      {
        break;
      }

      //SaveRefinementVolumeMask();

      m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_UNDO, m_ManualUndoList.size()>0);
      m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_REDO, m_ManualRedoList.size()>0);

      m_ManualModifiedWithoutApplied = true;

      m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_OK, true);

      m_View->CameraUpdate();

      break;
    }
  case ID_MANUAL_UNDO:
    {
      int numOfChanges = m_ManualUndoList.size();
      if(numOfChanges)
      {
        vtkDataSet *dataSet = m_ManualVolumeSlice->GetOutput()->GetVTKData();

        vtkUnsignedCharArray *redoScalars = vtkUnsignedCharArray::New();
        redoScalars->DeepCopy( dataSet->GetPointData()->GetScalars() );
        redoScalars->SetName("SCALARS");
        m_ManualRedoList.push_back(redoScalars);

        vtkDataArray *undoScalars = m_ManualUndoList[numOfChanges-1];

        dataSet->GetPointData()->SetScalars(undoScalars);
        dataSet->Update();

        vtkMAFSmartPointer<vtkStructuredPoints> newDataSet;
        newDataSet->DeepCopy(dataSet);

        //m_View->VmeShow(m_ManualVolumeSlice, false);
        m_ManualVolumeSlice->SetData(newDataSet, m_Volume->GetTimeStamp());
        m_ManualVolumeSlice->Update();
        m_View->CameraUpdate();
        //m_View->VmeShow(m_ManualVolumeSlice, true);

        vtkDEL(m_ManualUndoList[numOfChanges-1]);
        m_ManualUndoList.pop_back();

        m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_UNDO, m_ManualUndoList.size()>0);
        m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_REDO, m_ManualRedoList.size()>0);

        m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_OK, m_ManualUndoList.size()>0);

      }
      break;
    }
  case ID_MANUAL_REDO:
    {
      int numOfChanges = m_ManualRedoList.size();
      if(numOfChanges)
      {
        vtkDataSet *dataSet = m_ManualVolumeSlice->GetOutput()->GetVTKData();

        vtkUnsignedCharArray *undoScalars = vtkUnsignedCharArray::New();
        undoScalars->DeepCopy( dataSet->GetPointData()->GetScalars() );
        undoScalars->SetName("SCALARS");
        m_ManualUndoList.push_back(undoScalars);

        vtkDataArray *redoScalars = m_ManualRedoList[numOfChanges-1];

        dataSet->GetPointData()->SetScalars(redoScalars);
        dataSet->Update();

        vtkMAFSmartPointer<vtkStructuredPoints> newDataSet;
        newDataSet->DeepCopy(dataSet);

        //m_View->VmeShow(m_ManualVolumeSlice, false);
        m_ManualVolumeSlice->SetData(newDataSet, m_Volume->GetTimeStamp());
        m_ManualVolumeSlice->Update();
        m_View->CameraUpdate();
        //m_View->VmeShow(m_ManualVolumeSlice, true);

        vtkDEL(m_ManualRedoList[numOfChanges-1]);
        m_ManualRedoList.pop_back();

        m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_UNDO, m_ManualUndoList.size()>0);
        m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_REDO, m_ManualRedoList.size()>0);

        m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_OK, m_ManualUndoList.size()>0);

      }
      break;
    }
  case ID_MANUAL_OK:
    {
      m_ManualModifiedWithoutApplied = false;
      ApplyVolumeSliceChanges();
      ResetManualUndoList();
      ResetManualRedoList();
      UpdateVolumeSlice();
      SaveManualVolumeMask();

      m_View->CameraUpdate();

      m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_OK, m_CurrentSlicePlane == XY && m_ManualModifiedWithoutApplied);
      m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_UNDO, m_ManualUndoList.size()>0);
      break;
    }
  default:
    mafEventMacro(*e);
  }
}

//------------------------------------------------------------------------
void medOpSegmentation::OnRefinementSegmentationEvent(mafEvent *e)
//------------------------------------------------------------------------
{
  switch(e->GetId())
  {
  case ID_REFINEMENT_ACTION:
    {
      m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_ITERATIVE, m_RefinementSegmentationAction == ID_REFINEMENT_ISLANDS_REMOVE);

    }
    break;
  case ID_REFINEMENT_REGIONS_SIZE:
    {

    }
    break;
  case ID_REFINEMENT_EVERY_SLICE:
    {

    }
    break;
  case ID_REFINEMENT_UNDO:
    {
      int numOfChanges = m_RefinementUndoList.size();
      if(numOfChanges)
      {
        vtkDataSet *dataSet = m_ThresholdVolume->GetOutput()->GetVTKData();

        vtkUnsignedCharArray *redoScalars = vtkUnsignedCharArray::New();
        redoScalars->DeepCopy( dataSet->GetPointData()->GetScalars() );
        redoScalars->SetName("SCALARS");
        m_RefinementRedoList.push_back(redoScalars);

        vtkDataArray *undoScalars = m_RefinementUndoList[numOfChanges-1];

        dataSet->GetPointData()->SetScalars(undoScalars);
        dataSet->Update();

        vtkMAFSmartPointer<vtkStructuredPoints> newDataSet;
        newDataSet->DeepCopy(dataSet);

        m_View->VmeShow(m_ThresholdVolume, false);
        m_ThresholdVolume->SetData(newDataSet, m_Volume->GetTimeStamp());
        m_View->VmeShow(m_ThresholdVolume, true);

        vtkDEL(m_RefinementUndoList[numOfChanges-1]);
        m_RefinementUndoList.pop_back();

        m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_UNDO, m_RefinementUndoList.size()>0);
        m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_REDO, m_RefinementRedoList.size()>0);
      }
      break;
    }

  case ID_REFINEMENT_REDO:
    {
      int numOfChanges = m_RefinementRedoList.size();
      if(numOfChanges)
      {
        vtkDataSet *dataSet = m_ThresholdVolume->GetOutput()->GetVTKData();

        vtkUnsignedCharArray *undoScalars = vtkUnsignedCharArray::New();
        undoScalars->DeepCopy( dataSet->GetPointData()->GetScalars() );
        undoScalars->SetName("SCALARS");
        m_RefinementUndoList.push_back(undoScalars);

        vtkDataArray *redoScalars = m_RefinementRedoList[numOfChanges-1];

        dataSet->GetPointData()->SetScalars(redoScalars);
        dataSet->Update();

        vtkMAFSmartPointer<vtkStructuredPoints> newDataSet;
        newDataSet->DeepCopy(dataSet);

        m_View->VmeShow(m_ThresholdVolume, false);
        m_ThresholdVolume->SetData(newDataSet, m_Volume->GetTimeStamp());
        m_View->VmeShow(m_ThresholdVolume, true);

        vtkDEL(m_RefinementRedoList[numOfChanges-1]);
        m_RefinementRedoList.pop_back();

        m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_UNDO, m_RefinementUndoList.size()>0);
        m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_REDO, m_RefinementRedoList.size()>0);
      }
      break;
    }
  case ID_REFINEMENT_APPLY:
    {
      vtkUnsignedCharArray *scalars = vtkUnsignedCharArray::New();
      scalars->DeepCopy( m_ThresholdVolume->GetOutput()->GetVTKData()->GetPointData()->GetScalars() );
      scalars->SetName("SCALARS");

      m_RefinementUndoList.push_back( scalars );

      if (!Refinement())
      {
        break;
      }

      SaveRefinementVolumeMask();

      m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_UNDO, m_RefinementUndoList.size()>0);
      m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_REDO, m_RefinementRedoList.size()>0);

      m_View->CameraUpdate();

    }
    break;
  default:
    mafEventMacro(*e);
  }

}

//------------------------------------------------------------------------
void medOpSegmentation::InitializeViewSlice()
//------------------------------------------------------------------------
{
  m_View->SetTextureInterpolate(true);

  // slicing the volume
  vtkDataSet *dataSet = ((mafVME *)m_Volume)->GetOutput()->GetVTKData();
  m_View->PlugVisualPipe("mafVMEVolumeGray","mafPipeVolumeSlice_BES");
  m_View->PlugVisualPipe("medVMESegmentationVolume","mafPipeVolumeSlice_BES");
  m_View->PlugVisualPipe("mafVMEImage","mafPipeImage3D");
  m_View->PlugVisualPipe("mafVMESurface","mafPipeSurfaceSlice");
 
  dataSet->GetPoint((0,0,0),m_SliceOrigin);
  m_View->InitializeSlice(m_SliceOrigin);

  m_CurrentSliceIndex = 1;

  m_View->UpdateSlicePos(0.0);
  m_View->CameraUpdate();
}

//----------------------------------------------------------------------------
void medOpSegmentation::UpdateWindowing()
//----------------------------------------------------------------------------
{
  mafVMEOutputVolume *volumeOutput = mafVMEOutputVolume::SafeDownCast(m_Volume->GetOutput());
  double sr[2],subR[2];
  volumeOutput->GetVTKData()->GetScalarRange(sr);
  mmaVolumeMaterial *currentSurfaceMaterial = volumeOutput->GetMaterial();
  currentSurfaceMaterial->m_ColorLut->GetTableRange(subR);

  volumeOutput->GetMaterial()->UpdateProp();
  m_LutWidget->SetMaterial(volumeOutput->GetMaterial());
  m_LutWidget->Enable(true);
  m_LutSlider->SetRange(sr[0],sr[1]);
  m_LutSlider->SetSubRange(subR[0],subR[1]);

}

//----------------------------------------------------------------------------
void medOpSegmentation::SelectBrushImage(double x, double y, double z, bool selection)
//----------------------------------------------------------------------------
{
  vtkDataSet *dataset = ((mafVME *)m_ManualVolumeSlice)->GetOutput()->GetVTKData();

  if(!dataset)
    return;

  double origin[3];
  dataset->GetPoint(0,origin);

  int xID = round((x - origin[0])/m_VolumeSpacing[0]);
  int yID = round((y - origin[1])/m_VolumeSpacing[1]);

  int nPointXY = m_VolumeDimensions[0]*m_VolumeDimensions[1];

  double centerOfPick[3]={x,y,z};

  double ray = m_ManualBrushSize/2; 

  unsigned char scalar = 0;
  if(selection)
    scalar = m_ManualSelectionScalarValue;

  double pointToCheckRadius[3];

  //////////////////////////////////////////////////////////////////////////
  if(m_ManualBrushShape == 0) // circle
  {

    if (m_CurrentSlicePlane == XY)
    {
      for (int i=0;i<nPointXY;i++)
      {
        dataset->GetPoint(i,pointToCheckRadius);

        //        double dist =vtkMath::Distance2BetweenPoints(centerOfPick,pointToCheckRadius);
        if (sqrt(vtkMath::Distance2BetweenPoints(centerOfPick,pointToCheckRadius))<ray)
        {
          dataset->GetPointData()->GetScalars()->SetTuple1(i, scalar);
        }
      }
    } 
    else if (m_CurrentSlicePlane == XZ)
    {
      for (int i=0;i<dataset->GetNumberOfPoints();i++)
      {
        dataset->GetPoint(i,pointToCheckRadius);

        int yIDToCheck = round((pointToCheckRadius[1] - origin[1])/m_VolumeSpacing[1]);

        if (yID == yIDToCheck)
        {
          if (sqrt(vtkMath::Distance2BetweenPoints(centerOfPick,pointToCheckRadius))<ray)
          {
            dataset->GetPointData()->GetScalars()->SetTuple1(i, scalar);
          }
        }
      }
    }
    else if (m_CurrentSlicePlane == YZ)
    {
      for (int i=0;i<dataset->GetNumberOfPoints();i++)
      {
        dataset->GetPoint(i,pointToCheckRadius);

        int xIDToCheck = round((pointToCheckRadius[0] - origin[0])/m_VolumeSpacing[0]);

        if (xID == xIDToCheck)
        {
          if (sqrt(vtkMath::Distance2BetweenPoints(centerOfPick,pointToCheckRadius))<ray)
          {
            dataset->GetPointData()->GetScalars()->SetTuple1(i, scalar);
          }
        }
      }
    }

  }
  //////////////////////////////////////////////////////////////////////////
  else // square
  {

    if (m_CurrentSlicePlane == XY)
    {

      for (int i=0;i<nPointXY;i++)
      {
        dataset->GetPoint(i,pointToCheckRadius);

        if (centerOfPick[0]-ray < pointToCheckRadius[0] && centerOfPick[0]+ray > pointToCheckRadius[0])
        {
          if (centerOfPick[1]-ray < pointToCheckRadius[1] && centerOfPick[1]+ray > pointToCheckRadius[1])
          {
            dataset->GetPointData()->GetScalars()->SetTuple1(i, scalar);
          }
        }
      }
    }
    else if (m_CurrentSlicePlane == XZ)
    {
      for (int i=0;i<dataset->GetNumberOfPoints();i++)
      {
        dataset->GetPoint(i,pointToCheckRadius);

        int yIDToCheck = round((pointToCheckRadius[1] - origin[1])/m_VolumeSpacing[1]);

        if (yID == yIDToCheck)
        {
          if (centerOfPick[0]-ray < pointToCheckRadius[0] && centerOfPick[0]+ray > pointToCheckRadius[0])
          {
            if (centerOfPick[2]-ray < pointToCheckRadius[2] && centerOfPick[2]+ray > pointToCheckRadius[2])
            {
              dataset->GetPointData()->GetScalars()->SetTuple1(i, scalar);
            }
          }
        }
      }
    }
    else if (m_CurrentSlicePlane == YZ)
    {
      for (int i=0;i<dataset->GetNumberOfPoints();i++)
      {
        dataset->GetPoint(i,pointToCheckRadius);

        int xIDToCheck = round((pointToCheckRadius[0] - origin[0])/m_VolumeSpacing[0]);

        if (xID == xIDToCheck)
        {
          if (centerOfPick[1]-ray < pointToCheckRadius[1] && centerOfPick[1]+ray > pointToCheckRadius[1])
          {
            if (centerOfPick[2]-ray < pointToCheckRadius[2] && centerOfPick[2]+ray > pointToCheckRadius[2])
            {
              dataset->GetPointData()->GetScalars()->SetTuple1(i, scalar);
            }
          }
        }
      }
    }
  }

  vtkMAFSmartPointer<vtkStructuredPoints> newImage;
  newImage->DeepCopy(dataset);
  newImage->Update();

  //m_View->VmeShow(m_ManualVolumeSlice, false);
  m_ManualVolumeSlice->SetData(newImage,mafVME::SafeDownCast(m_ThresholdVolume)->GetTimeStamp(), 2);
  
  m_ManualVolumeSlice->GetEventSource()->InvokeEvent(m_ManualVolumeSlice, VME_OUTPUT_DATA_UPDATE);
  m_ManualVolumeSlice->GetOutput()->GetVTKData()->Update();
  m_ManualVolumeSlice->GetOutput()->Update();
  m_ManualVolumeSlice->Update();

  //m_View->VmeShow(m_ManualVolumeSlice, true);

}

//------------------------------------------------------------------------
void medOpSegmentation::InitializeInteractors()
//------------------------------------------------------------------------
{
  //Create the device manager
  mafNEW(m_DeviceManager);
  m_DeviceManager->SetListener(this);
  m_DeviceManager->SetName("DialogDeviceManager");
  m_DeviceManager->Initialize();

  //Create the static event router and connect it
  mafNEW(m_SER);
  m_SER->SetName("StaticEventRouter");
  m_SER->SetListener(this);

  //Create a Mouse device
  mafPlugDevice<medDeviceButtonsPadMouseDialog>("Mouse");
  m_DialogMouse = (medDeviceButtonsPadMouseDialog *)m_DeviceManager->AddDevice("medDeviceButtonsPadMouseDialog",false); // add as persistent device
  assert(m_DialogMouse);
  m_DialogMouse->SetName("DialogMouse");

  //Define the action for pointing and manipulating
  mafAction *pntAction = m_SER->AddAction("pntAction",-10);

  //create the positional event router

  mafNEW(m_ManualPER);
  m_ManualPER->SetName("m_EditingPER");
  m_ManualPER->SetListener(this);

  assert(m_View);
  m_ManualPER->SetRenderer(m_View->GetFrontRenderer());

  mafNEW(m_ManualPicker);
  m_ManualPicker->EnableContinuousPicking(m_ManualContinuousPickingOn);

  m_ManualPicker->SetRenderer(m_View->GetFrontRenderer());
  m_ManualPicker->SetListener(this);


  m_View->GetRWI()->SetMouse(m_DialogMouse);
  m_View->SetMouse(m_DialogMouse);
  m_OldBehavior=m_Volume->GetBehavior();
  m_DialogMouse->SetView(m_View);
  m_ManualPicker->AddObserver(this);

  m_SER->AddAction("pntEditingAction");
  pntAction = m_SER->GetAction("pntEditingAction");
  m_ManualPER->AddObserver(m_ManualPicker);


  mafNEW(m_AutomaticPicker);
  m_AutomaticPicker->SetRenderer(m_View->GetFrontRenderer());
  m_AutomaticPicker->SetListener(this);
  m_AutomaticPicker->AddObserver(this);

  m_Volume->SetBehavior(m_AutomaticPicker);

  mafNEW(m_AutomaticPER);
  pntAction = m_SER->AddAction("pntActionAutomatic",-10);
  pntAction->BindDevice(m_DialogMouse);
  pntAction->BindInteractor(m_AutomaticPER);
  m_AutomaticPER->AddObserver(m_AutomaticPicker);
  m_AutomaticPER->SetRenderer(m_View->GetFrontRenderer());

}

//------------------------------------------------------------------------
bool medOpSegmentation::AutomaticCheckRange(int indexToExclude /* = -1 */)
//------------------------------------------------------------------------
{
  double min,max;
  m_AutomaticRangeSlider->GetSubRange(&min,&max);

  int minInt,maxInt;
  minInt = round(min)-1;
  maxInt = round(max)-1;

  if (minInt > maxInt)
  {
    wxMessageBox("Lower slice index is higher than upper slice index");
    return false;
  }

  //Check if any range overlaps with the actual
  for (int i=0;i<m_AutomaticRanges.size();i++)
  {
    if (indexToExclude != i)
    {
      if (m_AutomaticRanges[i].m_EndSlice == maxInt || m_AutomaticRanges[i].m_StartSlice == minInt)
      {
        wxMessageBox("Range conflict in the correct ranges list");
        return false;
      }

      if (m_AutomaticRanges[i].m_EndSlice >= minInt && m_AutomaticRanges[i].m_EndSlice <= maxInt)
      {
        wxMessageBox("Overlaps of slices range");
        return false;
      }

      if (m_AutomaticRanges[i].m_StartSlice >= minInt && m_AutomaticRanges[i].m_StartSlice <= maxInt)
      {
        wxMessageBox("Overlaps of slices range");
        return false;
      }
    }
  }

  return true;
}

//------------------------------------------------------------------------
void medOpSegmentation::UpdateSliceLabel()
//------------------------------------------------------------------------
{
  //////////////////////////////////////////////////////////////////////////
  //Update slice text actor
  //////////////////////////////////////////////////////////////////////////
  mafString text = "Slice = ";
  text<<m_CurrentSliceIndex;
  text<<" of ";
  if (m_CurrentSlicePlane == XY)
  {
    text<<m_VolumeDimensions[2];
  } 
  else if (m_CurrentSlicePlane == XZ)
  {
    text<<m_VolumeDimensions[1];
  }
  else if (m_CurrentSlicePlane == YZ)
  {
    text<<m_VolumeDimensions[0];
  }
  m_AutomaticSliceTextMapper->SetInput(text.GetCStr());
  //////////////////////////////////////////////////////////////////////////
}

//------------------------------------------------------------------------
void medOpSegmentation::UpdateThresholdLabel()
//------------------------------------------------------------------------
{
  if (m_CurrentOperation == AUTOMATIC_SEGMENTATION && (m_CameraPositionId == CAMERA_OS_Z || m_AutomaticGlobalThreshold == GLOBAL) )
  {
    if (m_AutomaticGlobalThreshold == RANGE)
    {
      //Try to find the threshold of the visualized slice
      for (int i=0;i<m_AutomaticRanges.size();i++)
      {
        if (m_AutomaticRanges[i].m_StartSlice<=m_CurrentSliceIndex-1 && m_AutomaticRanges[i].m_EndSlice>=m_CurrentSliceIndex-1)
        {
          mafString text = wxString::Format("Threshold low:%.3f high:%.3f",m_AutomaticRanges[i].m_ThresholdValue,m_AutomaticRanges[i].m_UpperThresholdValue);
          m_AutomaticThresholdTextMapper->SetInput(text.GetCStr());
          return;
        }
      }
    }
    else if(m_AutomaticGlobalThreshold == GLOBAL)
    {
      mafString text = wxString::Format("Threshold low:%.3f high:%.3f",m_AutomaticThreshold,m_AutomaticUpperThreshold);
      m_AutomaticThresholdTextMapper->SetInput(text.GetCStr());
      return;
    }
  }
  //If we are showing a vertical slice in witch can be multiple threshold and global threshold is not active
  //or if the current slice doesn't have a threshold 
  //we use an empty label
  m_AutomaticThresholdTextMapper->SetInput("");
}

//------------------------------------------------------------------------
void medOpSegmentation::OnAutomaticPicker(mafEvent *e)
//------------------------------------------------------------------------
{
  vtkPoints *point = vtkPoints::SafeDownCast(e->GetVtkObj());
  if (point)
  {
    double position[3];
    point->GetPoint(0,position);

    mafLogMessage("Point picked %.3f %.3f %.3f",position[0],position[1],position[2]);

    double scalarValue = e->GetDouble();

    mafLogMessage("Scalar value %.3f",scalarValue);

    m_AutomaticThreshold = scalarValue;

    m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Update();
  }
}

//----------------------------------------------------------------------------
bool medOpSegmentation::CheckNumberOfThresholds()
//----------------------------------------------------------------------------
{
  //////////////////////////////////////////////////////////////////////////
  //Check that all slices have a threshold
  for (int i=0;i<m_VolumeDimensions[2];i++)//only XY planes
  {
    bool found = false;
    for (int j=0;j<m_AutomaticRanges.size();j++)
    {
      if (i>=(m_AutomaticRanges[j].m_StartSlice) && i<=(m_AutomaticRanges[j].m_EndSlice))
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      mafLogMessage("Slice %d° hasn't a threshold",i+1);
      return false;
    }
  }

  return true;
  //////////////////////////////////////////////////////////////////////////
}

//----------------------------------------------------------------------------
void medOpSegmentation::InitVolumeDimensions()
//----------------------------------------------------------------------------
{
  vtkDataSet *inputDataSet = vtkDataSet::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_Volume)->GetOutput()->GetVTKData());

  if (inputDataSet)
  {
    inputDataSet->Update();

    m_Volume->GetOutput()->GetBounds(m_VolumeBounds);

    if (vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(inputDataSet))
    {
      sp->GetDimensions(m_VolumeDimensions); 
    }
    else if (vtkRectilinearGrid *rg = vtkRectilinearGrid::SafeDownCast(inputDataSet))
    {
      rg->GetDimensions(m_VolumeDimensions); 
    }

    m_VolumeParametersInitialized = true;

    if (m_SliceSlider)
    {
      if (m_CurrentSlicePlane == XY)
      {
        m_SliceSlider->SetRange(1,m_VolumeDimensions[2]);
      }
      if (m_CurrentSlicePlane == XZ)
      {
        m_SliceSlider->SetRange(1,m_VolumeDimensions[1]);
      }
      if (m_CurrentSlicePlane == YZ)
      {
        m_SliceSlider->SetRange(1,m_VolumeDimensions[0]);
      }
      m_GuiDialog->Update();
    }
    if (m_AutomaticRangeSlider)
    {
      m_AutomaticRangeSlider->SetRange(1,m_VolumeDimensions[2]);
      m_AutomaticRangeSlider->SetSubRange(1,m_VolumeDimensions[2]);

      m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Update();
    }
  }
}


//----------------------------------------------------------------------------
void medOpSegmentation::InitVolumeSpacing()
//----------------------------------------------------------------------------
{
  vtkDataSet *vme_data = ((mafVME *)m_Volume)->GetOutput()->GetVTKData();

  m_VolumeSpacing[0] = VTK_DOUBLE_MAX;
  m_VolumeSpacing[1] = VTK_DOUBLE_MAX;
  m_VolumeSpacing[2] = VTK_DOUBLE_MAX;

  //vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(vme_data);
  //sp->GetSpacing(m_VolumeSpacing);

  if (vtkImageData *image = vtkImageData::SafeDownCast(vme_data))
  {
    image->GetSpacing(m_VolumeSpacing);
  }
  else if (vtkRectilinearGrid *rgrid = vtkRectilinearGrid::SafeDownCast(vme_data))
  {
    for (int xi = 1; xi < rgrid->GetXCoordinates()->GetNumberOfTuples (); xi++)
    {
      double spcx = rgrid->GetXCoordinates()->GetTuple1(xi)-rgrid->GetXCoordinates()->GetTuple1(xi-1);
      if (m_VolumeSpacing[0] > spcx && spcx != 0.0)
        m_VolumeSpacing[0] = spcx;
    }

    for (int yi = 1; yi < rgrid->GetYCoordinates()->GetNumberOfTuples (); yi++)
    {
      double spcy = rgrid->GetYCoordinates()->GetTuple1(yi)-rgrid->GetYCoordinates()->GetTuple1(yi-1);
      if (m_VolumeSpacing[1] > spcy && spcy != 0.0)
        m_VolumeSpacing[1] = spcy;
    }

    for (int zi = 1; zi < rgrid->GetZCoordinates()->GetNumberOfTuples (); zi++)
    {
      double spcz = rgrid->GetZCoordinates()->GetTuple1(zi)-rgrid->GetZCoordinates()->GetTuple1(zi-1);
      if (m_VolumeSpacing[2] > spcz && spcz != 0.0)
        m_VolumeSpacing[2] = spcz;
    }
  }
}

//----------------------------------------------------------------------------
double medOpSegmentation::GetPosFromSliceIndexZ()
//----------------------------------------------------------------------------
{
  double pos = -1;
  vtkDataSet *inputDataSet = vtkDataSet::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_Volume)->GetOutput()->GetVTKData());

  if (inputDataSet)
  {
    inputDataSet->Update();

    double point[3];
    inputDataSet->GetPoint((m_CurrentSliceIndex-1)*m_VolumeDimensions[0]*m_VolumeDimensions[1], point);

    double bounds[6];
    inputDataSet->GetBounds(bounds);

    pos = (point[2] - bounds[4])/ (bounds[5] - bounds[4]);

  }
  return pos;

}

//----------------------------------------------------------------------------
void medOpSegmentation::UpdateSlice()
//----------------------------------------------------------------------------
{
  UpdateThresholdLabel();
  UpdateSliceLabel();

  double pos;
  if (m_CurrentSlicePlane == XY)
    pos = GetPosFromSliceIndexZ();
  else
    pos = (double)(m_CurrentSliceIndex - 1)/(m_SliceSlider->GetMax()-1) ;

  double posNew[3];
  if (m_Volume->GetOutput()->GetVTKData()->IsA("vtkStructuredPoints"))
  {
    vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(m_Volume->GetOutput()->GetVTKData());
    sp->Update();
    double spc[3],or[3];
    sp->GetSpacing(spc);
    sp->GetOrigin(or);

    if (m_CurrentSlicePlane == XY)
    {
      posNew[0] = or[0];
      posNew[1] = or[1];
      posNew[2] = (m_CurrentSliceIndex-1)*spc[2]+or[2];
    }
    if (m_CurrentSlicePlane == XZ)
    {
      posNew[0] = or[0];
      posNew[1] = (m_CurrentSliceIndex-1)*spc[1]+or[1];
      posNew[2] = or[2];
    }
    if (m_CurrentSlicePlane == YZ)
    {
      posNew[0] = (m_CurrentSliceIndex-1)*spc[0]+or[0];
      posNew[1] = or[1];
      posNew[2] = or[2];
    }
  }
  else
  {
    vtkRectilinearGrid *rg = vtkRectilinearGrid::SafeDownCast(m_Volume->GetOutput()->GetVTKData());
    rg->Update();
    double or[3];
    or[0] = rg->GetXCoordinates()->GetTuple1(0);
    or[1] = rg->GetYCoordinates()->GetTuple1(0);
    or[2] = rg->GetZCoordinates()->GetTuple1(0);

    if (m_CurrentSlicePlane == XY)
    {
      posNew[0] = or[0];
      posNew[1] = or[1];
      posNew[2] = rg->GetZCoordinates()->GetTuple1(m_CurrentSliceIndex-1);
    }
    if (m_CurrentSlicePlane == XZ)
    {
      posNew[0] = or[0];
      posNew[1] = rg->GetYCoordinates()->GetTuple1(m_CurrentSliceIndex-1);
      posNew[2] = or[2];
    }
    if (m_CurrentSlicePlane == YZ)
    {
      posNew[0] = rg->GetXCoordinates()->GetTuple1(m_CurrentSliceIndex-1);
      posNew[1] = or[1];
      posNew[2] = or[2];
    }
  }


  m_View->SetSlice( posNew );

  if(m_CurrentOperation == MANUAL_SEGMENTATION && m_CurrentSlicePlane == XY)
  {
    UpdateVolumeSlice();

    // reset undo-redo
    m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_UNDO, false);
    m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_REDO, false);

    m_ManualUndoCounter = 1;

    ResetManualRedoList();
    ResetManualUndoList();

    // reset apply
    m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_OK, false);
  }


  m_View->CameraUpdate();

  m_OldSliceIndex = m_CurrentSliceIndex;

}

//----------------------------------------------------------------------------
void medOpSegmentation::InitManualVolumeSlice()
//----------------------------------------------------------------------------
{

  if(m_ManualVolumeSlice == NULL)
  {
    mafNEW(m_ManualVolumeSlice);
    m_ManualVolumeSlice->ReparentTo(m_Volume->GetParent());
    m_ManualVolumeSlice->SetName("Manual Volume Slice");
    m_View->VmeAdd(m_ManualVolumeSlice);
  }

}

//----------------------------------------------------------------------------
void medOpSegmentation::UpdateVolumeSlice()
//----------------------------------------------------------------------------
{
  if(m_ManualVolumeSlice == NULL)
  {
    return;
  }

  vtkDataSet *inputData = NULL;

  if(!m_ThresholdVolume || !m_ThresholdVolume->GetOutput()->GetVTKData())
    return;

  inputData = m_ThresholdVolume->GetOutput()->GetVTKData();
  inputData->Update();

  vtkDataArray *inputScalars = inputData->GetPointData()->GetScalars();

  vtkMAFSmartPointer<vtkUnsignedCharArray> scalars;
  scalars->SetName("SCALARS");

  double origin[3]; 

  vtkDataSet *ds = m_Volume->GetOutput()->GetVTKData();
  if (ds->IsA("vtkStructuredPoints"))
  {
    double spc[3],or[3];
    vtkStructuredPoints::SafeDownCast(ds)->GetOrigin(or);
    origin[0] = or[0];
    origin[1] = or[1];
    origin[2] = (m_CurrentSliceIndex-1)*m_VolumeSpacing[2]+or[2];
  }
  else if (ds->IsA("vtkRectilinearGrid"))
  {
    double x = vtkRectilinearGrid::SafeDownCast(ds)->GetXCoordinates()->GetTuple1(0);
    double y = vtkRectilinearGrid::SafeDownCast(ds)->GetYCoordinates()->GetTuple1(0);
    double z = vtkRectilinearGrid::SafeDownCast(ds)->GetZCoordinates()->GetTuple1(m_CurrentSliceIndex-1);

    origin[0] = x;
    origin[1] = y;
    origin[2] = z;
  }

  


  //////////////////////////////////////////////////////////////////////////
  // spacing regolare: imagedata
  //////////////////////////////////////////////////////////////////////////
  //if(m_VolumeSpacing[2])
  if(ds->IsA("vtkStructuredPoints"))
  {
    int numberOfSlices = 2;
    vtkDataSet *imageData = m_ManualVolumeSlice->GetOutput()->GetVTKData();

    vtkMAFSmartPointer<vtkStructuredPoints> newImageData;

    int numberOfPoints;

    if(m_CurrentSlicePlane == XY)
    {
      numberOfPoints = m_VolumeDimensions[0]*m_VolumeDimensions[1];
      scalars->SetNumberOfTuples(numberOfSlices*numberOfPoints);

      for (int k=0; k<numberOfPoints; k++)
      {
        unsigned char scalarValue = inputScalars->GetTuple1( k + (m_CurrentSliceIndex-1)*numberOfPoints );
        scalars->SetTuple1(k,scalarValue);
        scalars->SetTuple1(k+numberOfPoints, scalarValue);
      }


      //newImageData->DeepCopy(imageData);
      newImageData->SetDimensions(m_VolumeDimensions[0],m_VolumeDimensions[1],numberOfSlices);
      newImageData->SetSpacing(m_VolumeSpacing[0],m_VolumeSpacing[1],1.0);
      newImageData->SetScalarTypeToUnsignedChar();
      newImageData->SetOrigin(origin[0], origin[1], origin[2]);
      newImageData->GetPointData()->AddArray(scalars);
      newImageData->GetPointData()->SetActiveScalars("SCALARS");
      newImageData->Update();
    }

    else if(m_CurrentSlicePlane == YZ)
    {
      numberOfPoints = m_VolumeDimensions[1] * m_VolumeDimensions[2];
      scalars->SetNumberOfTuples(numberOfSlices*numberOfPoints);

      double pointToCheck[3];
      int k=0;

      for(int i=0;i<m_VolumeDimensions[0]*m_VolumeDimensions[1]*m_VolumeDimensions[2];i++)
      {
        inputData->GetPoint(i, pointToCheck);
        if(origin[0] == pointToCheck[0])
        {
          unsigned char scalarValue = inputScalars->GetTuple1( i );
          scalars->SetTuple1(k,scalarValue);
          scalars->SetTuple1(k+1, scalarValue);
          //scalars->SetTuple1(k+2, scalarValue);
          k += 2;
        }
      }

      newImageData->SetDimensions(numberOfSlices,m_VolumeDimensions[1],m_VolumeDimensions[2]);
      newImageData->SetSpacing(m_VolumeSpacing[0],m_VolumeSpacing[1],m_VolumeSpacing[2]);
      newImageData->SetScalarTypeToUnsignedChar();
      newImageData->SetOrigin(origin[0], origin[1], origin[2]);
      newImageData->GetPointData()->AddArray(scalars);
      newImageData->GetPointData()->SetActiveScalars("SCALARS");
      newImageData->Update();

    }
    else if(m_CurrentSlicePlane == XZ)
    {
      numberOfPoints = m_VolumeDimensions[0]*m_VolumeDimensions[2];
      scalars->SetNumberOfTuples(numberOfSlices*numberOfPoints);

      double pointToCheck[3];

      vtkMAFSmartPointer<vtkStructuredPoints> tmpIm;

      for(int i=0;i<numberOfSlices*numberOfPoints;i++)
      {
        scalars->SetTuple1(i, 0);
      }

      tmpIm->SetDimensions(m_VolumeDimensions[0], numberOfSlices, m_VolumeDimensions[2]);
      tmpIm->SetSpacing(m_VolumeSpacing[0],m_VolumeSpacing[1],m_VolumeSpacing[2]);
      tmpIm->SetScalarTypeToUnsignedChar();
      tmpIm->SetOrigin(origin[0], origin[1], origin[2]);
      tmpIm->GetPointData()->AddArray(scalars);
      tmpIm->GetPointData()->SetActiveScalars("SCALARS");
      tmpIm->Update();

      for(int i=0;i<numberOfSlices*numberOfPoints;i++)
      {
        tmpIm->GetPoint(i, pointToCheck);

        int j = inputData->FindPoint(pointToCheck[0], origin[1], pointToCheck[2]);
        unsigned char scalarValue = inputScalars->GetTuple1(j);

        scalars->SetTuple1(i, scalarValue);
      }

      newImageData->SetDimensions(m_VolumeDimensions[0], numberOfSlices, m_VolumeDimensions[2]);
      newImageData->SetSpacing(m_VolumeSpacing[0],m_VolumeSpacing[1],m_VolumeSpacing[2]);
      newImageData->SetScalarTypeToUnsignedChar();
      newImageData->SetOrigin(origin[0], origin[1], origin[2]);
      newImageData->GetPointData()->AddArray(scalars);
      newImageData->GetPointData()->SetActiveScalars("SCALARS");
      newImageData->Update();

    }
    m_ManualVolumeSlice->SetData(newImageData,0.0);
    //lutPreset(17,m_ManualVolumeSlice->GetMaterial()->m_ColorLut);
    m_ManualColorLUT = m_ManualVolumeSlice->GetMaterial()->m_ColorLut;
    InitManualColorLut();
    m_ManualVolumeSlice->GetMaterial()->UpdateFromTables();
    m_ManualVolumeSlice->Update();

    //////////////////////////////////////////////////////////////////////////
    // comment to prevent overlay visualization in NEXT step
    //////////////////////////////////////////////////////////////////////////
    //m_View->VmeShow(m_ManualVolumeSlice, false);
    //m_View->VmeShow(m_ManualVolumeSlice, true);
    //////////////////////////////////////////////////////////////////////////

  }

  //////////////////////////////////////////////////////////////////////////
  // spacing non regolare: rectilineargrid
  //////////////////////////////////////////////////////////////////////////
  else
  {

    vtkDataSet *rgData = m_ManualVolumeSlice->GetOutput()->GetVTKData();

    vtkMAFSmartPointer<vtkRectilinearGrid> newRgData;

    vtkDataArray *x = vtkRectilinearGrid::SafeDownCast(inputData)->GetXCoordinates();
    vtkDataArray *y = vtkRectilinearGrid::SafeDownCast(inputData)->GetYCoordinates();
    vtkDataArray *z = vtkRectilinearGrid::SafeDownCast(inputData)->GetZCoordinates();

    if(m_CurrentSlicePlane == XY)
    {
      vtkDoubleArray  *newZ = vtkDoubleArray::New();
      newZ->SetNumberOfComponents(1);
      newZ->SetNumberOfTuples(2);
      newZ->SetTuple1(0, origin[2]);
      newZ->SetTuple1(1, origin[2] + 1.0);
      //newZ->SetTuple1(2, origin[2] + 2.0);

      scalars->SetNumberOfTuples(2*m_VolumeDimensions[0]*m_VolumeDimensions[1]);

      for (int k=0; k<m_VolumeDimensions[0]*m_VolumeDimensions[1]; k++)
      {
        unsigned char scalarValue = inputScalars->GetTuple1( k + (m_CurrentSliceIndex-1)*m_VolumeDimensions[0]*m_VolumeDimensions[1] );
        if(scalarValue<255)
          scalarValue=0;
        scalars->SetTuple1(k,scalarValue);
        scalars->SetTuple1(k+ (m_VolumeDimensions[0]*m_VolumeDimensions[1]), scalarValue);
      }

      newRgData->SetDimensions(m_VolumeDimensions[0],m_VolumeDimensions[1],2);
      newRgData->SetXCoordinates(x);
      newRgData->SetYCoordinates(y);
      newRgData->SetZCoordinates(newZ);
      newRgData->GetPointData()->AddArray(scalars);
      newRgData->GetPointData()->SetActiveScalars("SCALARS");
      newRgData->Update();

    }
    else if(m_CurrentSlicePlane == YZ)
    {

      vtkDoubleArray  *newX = vtkDoubleArray::New();
      newX->SetNumberOfComponents(1);
      newX->SetNumberOfTuples(2);
      newX->SetTuple1(0, origin[0]);
      newX->SetTuple1(1, origin[0] + m_VolumeSpacing[0]);
      //newX->SetTuple1(2, origin[0] + 2*m_VolumeSpacing[0]);

      scalars->SetNumberOfTuples(2*m_VolumeDimensions[1]*m_VolumeDimensions[2]);

      double pointToCheck[3];
      int k=0;

      for(int i=0;i<m_VolumeDimensions[0]*m_VolumeDimensions[1]*m_VolumeDimensions[2];i++)
      {
        //inputData->GetPoint(i, pointToCheck);
        if(origin[0] == pointToCheck[0])
        {
          unsigned char scalarValue = inputScalars->GetTuple1( i );
          scalars->SetTuple1(k,scalarValue);
          scalars->SetTuple1(k+1, scalarValue);
          //scalars->SetTuple1(k+2, scalarValue);
          k +=2;
        }
      }

      newRgData->SetDimensions(2,m_VolumeDimensions[1],m_VolumeDimensions[2]);
      newRgData->SetXCoordinates(newX);
      newRgData->SetYCoordinates(y);
      newRgData->SetZCoordinates(z);
      newRgData->GetPointData()->AddArray(scalars);
      newRgData->GetPointData()->SetActiveScalars("SCALARS");
      newRgData->Update();
    }
    else if(m_CurrentSlicePlane == XZ)
    {

    }

    m_ManualVolumeSlice->SetData(newRgData, mafVME::SafeDownCast(m_ThresholdVolume)->GetTimeStamp());
    //lutPreset(17,m_ManualVolumeSlice->GetMaterial()->m_ColorLut);
    m_ManualColorLUT = m_ManualVolumeSlice->GetMaterial()->m_ColorLut;
    InitManualColorLut();
    m_ManualVolumeSlice->GetMaterial()->UpdateFromTables();
    m_ManualVolumeSlice->Update();
    m_View->VmeShow(m_ManualVolumeSlice, false);
    m_View->VmeShow(m_ManualVolumeSlice, true);

  }

}

//----------------------------------------------------------------------------
void medOpSegmentation::ApplyVolumeSliceChanges()
//----------------------------------------------------------------------------
{
  vtkDataSet *inputDataSet = vtkDataSet::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_ThresholdVolume)->GetOutput()->GetVTKData());
  vtkDataSet *sliceDataSet = vtkDataSet::SafeDownCast(m_ManualVolumeSlice->GetOutput()->GetVTKData());

  if (inputDataSet && sliceDataSet)
  {
    inputDataSet->Update();

    long progress = 0;
    mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));

    vtkMAFSmartPointer<vtkUnsignedCharArray> newScalars;
    newScalars->SetName("SCALARS");
    newScalars->SetNumberOfTuples(m_VolumeDimensions[0]*m_VolumeDimensions[1]*m_VolumeDimensions[2]);

    //double point[3];
    //inputDataSet->GetPoint(m_CurrentSliceIndex*m_VolumeDimensions[0]*m_VolumeDimensions[1], point);
    double point[3];
    point[0] = m_View->GetSliceOrigin()[0];
    point[1] = m_View->GetSliceOrigin()[1];
    point[2] = m_View->GetSliceOrigin()[2];

    vtkDataArray *inputScalars = inputDataSet->GetPointData()->GetScalars();
    vtkDataArray *sliceScalars = sliceDataSet->GetPointData()->GetScalars();
    vtkMAFSmartPointer<vtkUnsignedCharArray> scalars;
    scalars->SetName("SCALARS");
    scalars->SetNumberOfTuples(m_VolumeDimensions[0]*m_VolumeDimensions[1]);

    if(m_CurrentSlicePlane == XY)
    {
      for (int i= 0;i<m_VolumeDimensions[2];i++)
      {
        if(i != m_CurrentSliceIndex-1)
          //if(i != zID)
        {
          for (int k=0;k<(m_VolumeDimensions[0]*m_VolumeDimensions[1]);k++)
          {
            unsigned char value = inputScalars->GetTuple1(k+i*(m_VolumeDimensions[0]*m_VolumeDimensions[1]));
            newScalars->SetTuple1(k+i*(m_VolumeDimensions[0]*m_VolumeDimensions[1]),value);
          }
        }
        else
        {
          for (int k=0;k<(m_VolumeDimensions[0]*m_VolumeDimensions[1]);k++)
          {
            double point[3];
            inputDataSet->GetPoint(k+i*(m_VolumeDimensions[0]*m_VolumeDimensions[1]), point);

            int j = sliceDataSet->FindPoint(point);

            //double value = inputScalars->GetTuple1(k+i*(m_VolumeDimensions[0]*m_VolumeDimensions[1]));
            unsigned char value = sliceScalars->GetTuple1(j);
            if(value < m_ManualSelectionScalarValue)
              value = 0;
            else value = 255;
            newScalars->SetTuple1(k+i*(m_VolumeDimensions[0]*m_VolumeDimensions[1]),value);
          }

        }

      }  
    }

    if (inputDataSet->IsA("vtkStructuredPoints"))
    {
      vtkMAFSmartPointer<vtkStructuredPoints> newSP;
      newSP->CopyStructure(vtkStructuredPoints::SafeDownCast(inputDataSet));
      newSP->Update();
      newSP->GetPointData()->AddArray(newScalars);
      newSP->GetPointData()->SetActiveScalars("SCALARS");
      newSP->SetScalarTypeToUnsignedChar();
      newSP->Update();

      m_ThresholdVolume->SetData(newSP,mafVME::SafeDownCast(m_Input)->GetTimeStamp());
      vtkStructuredPoints *spVME = vtkStructuredPoints::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_ThresholdVolume)->GetOutput()->GetVTKData());
      spVME->Update();

    }
    else
    {
      vtkMAFSmartPointer<vtkRectilinearGrid> newRG;
      newRG->CopyStructure(vtkRectilinearGrid::SafeDownCast(inputDataSet));
      newRG->Update();
      newRG->GetPointData()->AddArray(newScalars);
      newRG->GetPointData()->SetActiveScalars("SCALARS");
      newRG->Update();

      m_ThresholdVolume->SetData(newRG,mafVME::SafeDownCast(m_Input)->GetTimeStamp());
      vtkRectilinearGrid *rgVME = vtkRectilinearGrid::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_ThresholdVolume)->GetOutput()->GetVTKData());
      rgVME->Update();
    }

    m_ThresholdVolume->Update();

  }

}

//----------------------------------------------------------------------------
void medOpSegmentation::ResetManualRedoList()
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_ManualRedoList.size();i++)
  {
    vtkDEL(m_ManualRedoList[i]);
  }
  m_ManualRedoList.clear();

}

//----------------------------------------------------------------------------
void medOpSegmentation::ResetManualUndoList()
//----------------------------------------------------------------------------
{

  for (int i=0;i<m_ManualUndoList.size();i++)
  {
    vtkDEL(m_ManualUndoList[i]);
  }
  m_ManualUndoList.clear();

}

//----------------------------------------------------------------------------
void medOpSegmentation::ResetRefinementRedoList()
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_RefinementRedoList.size();i++)
  {
    vtkDEL(m_RefinementRedoList[i]);
  }
  m_RefinementRedoList.clear();

}

//----------------------------------------------------------------------------
void medOpSegmentation::ResetRefinementUndoList()
//----------------------------------------------------------------------------
{

  for (int i=0;i<m_RefinementUndoList.size();i++)
  {
    vtkDEL(m_RefinementUndoList[i]);
  }
  m_RefinementUndoList.clear();

}


//----------------------------------------------------------------------------
void medOpSegmentation::InitSegmentationColorLut()
//----------------------------------------------------------------------------
{
  if(m_SegmentationColorLUT)
  {
    unsigned char v[4*4] = 
    {
      128,128,128,0,
        255,0,0,30,
        255,0,0,30,
        255,0,0,30,
    };

    m_SegmentationColorLUT->SetNumberOfTableValues(4);
    for(int i=0; i<4; i++)
    {
      int k = i*4; 
      m_SegmentationColorLUT->SetTableValue(i, v[k]/255.0, v[k+1]/255.0, v[k+2]/255.0, v[k+3]/255.0);
    }
  }
}

//----------------------------------------------------------------------------
void medOpSegmentation::InitManualColorLut()
//----------------------------------------------------------------------------
{
  if(m_ManualColorLUT)
  {
    unsigned char v[4*4] = 
    {
      128,128,128,0,
        0,0,2550,30,
        0,0,255,30,
        0,0,255,30,
    };

    m_ManualColorLUT->SetNumberOfTableValues(4);
    for(int i=0; i<4; i++)
    {
      int k = i*4; 
      m_ManualColorLUT->SetTableValue(i, v[k]/255.0, v[k+1]/255.0, v[k+2]/255.0, v[k+3]/255.0);
    }
  }
}