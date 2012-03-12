/*=========================================================================
Program:   LHP
Module:    $RCSfile: medOpSegmentation.cpp,v $
Language:  C++
Date:      $Date: 2012-03-12 14:37:48 $
Version:   $Revision: 1.1.2.54 $
Authors:   Eleonora Mambrini - Matteo Giacomoni, Gianluigi Crimi, Alberto Losi
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
#include "mafPipeVolumeSlice_BES.h"

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
#include "vtkVolumeProperty.h"

#include "vtkRenderer.h"
#include "vtkTextMapper.h"
#include "vtkActor2D.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"
#include "vtkPointData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkConnectivityFilter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkMEDImageFillHolesRemoveIslands.h"

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
#define min(a,b)(((a) < (b)) ? (a) : (b))
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

static int m_CurrentVolumeDimensions[3] = {0,0,0};          //<Used to load only volume with the specified dimensions
static double m_CurrentVolumeSpacing[3] = {0,0,0};          //<Used to load only volume with the specified spacing
static double m_CurrentVolumeBounds[6] = {0,0,0,0,0,0};           //<Used to load only volume with the specified bounds
static bool m_CurrentVolumeParametersInitialized = false;

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
  m_CurrentSlicePlane = XY;
  m_OldSliceIndex = m_CurrentSliceIndex;
  m_OldSlicePlane = m_CurrentSlicePlane;


  m_NumSliceSliderEvents = 0;
  m_SliceSlider = NULL;


  m_CurrentOperation = 0;

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

  m_ManualBrushShapeRadioBox = NULL;
  m_ManualBrushShape = CIRCLE_BRUSH_SHAPE;
  m_ManualBrushSize = 1;
  m_ManualRefinementRegionsSize = 1;

  m_ManualBrushSizeText = NULL;
  m_ManualBrushSizeSlider = NULL;

  m_ManualRefinementComboBox = NULL;
  m_ManualRefinementRegionSizeText = NULL;

  m_SegmentationPicker = NULL;

  m_ManualVolumeMask = NULL;
  m_ManualVolumeSlice = NULL;

  m_ManualPER = NULL;
  
  m_ManualUndoList.clear();
  m_ManualRedoList.clear();

  m_PickingStarted = false;
  //////////////////////////////////////////////////////////////////////////
  //Automatic initializations
  //////////////////////////////////////////////////////////////////////////
  m_AutomaticThreshold = 0.0;
  m_AutomaticUpperThreshold = 0.0;
  m_AutomaticMouseThreshold = 0.0;
  m_AutomaticRanges.clear();
  m_AutomaticListOfRange = NULL;
  m_AutomaticThresholdTextActor = NULL;
  m_AutomaticThresholdTextMapper = NULL;
  m_AutomaticSliceTextActor = NULL;
  m_AutomaticSliceTextMapper = NULL;
  m_AutomaticScalarTextActor = NULL;
  m_AutomaticScalarTextMapper = NULL;
  m_AutomaticRangeSlider = NULL;
  m_AutomaticPER = NULL;
  m_AutomaticGlobalThreshold = GLOBAL;

  //////////////////////////////////////////////////////////////////////////
  //Refinement initializations
  //////////////////////////////////////////////////////////////////////////
  m_RefinementVolumeMask = NULL;

  m_RefinementUndoList.clear();
  m_RefinementRedoList.clear();
  //////////////////////////////////////////////////////////////////////////

  m_TrilinearInterpolationOn = FALSE;

  m_InitialFocalPoint[0]=9999;
  m_InitialFocalPoint[1]=9999;
  m_InitialFocalPoint[2]=9999;
  m_InitialScaleFactor = -1;

  m_MajorityThreshold = 9;

  m_RealDrawnImage = NULL;
  m_CurrentBrushMoveEventCount = 0;

  m_ThresholdVolumeSlice = NULL;
  m_EmptyVolumeSlice = NULL;

  m_LoadedVolume = NULL;
  m_LastMouseMovePointID = 0;

  m_LoadedVolumeName = "[Select input volume]";

  m_OldAutomaticThreshold = MAXINT;
  m_OldAutomaticUpperThreshold = MAXINT;
}
//----------------------------------------------------------------------------
medOpSegmentation::~medOpSegmentation()
//----------------------------------------------------------------------------
{

  RemoveVMEs();

  mafDEL(m_OutputVolume);
  mafDEL(m_SegmentatedVolume);

  Superclass;
}
//----------------------------------------------------------------------------
bool medOpSegmentation::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsA("mafVMEVolumeGray") );
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
  InitVolumeSpacing();
  // Set static variable for load segmentation step
  if(m_VolumeParametersInitialized)
  {
    for(int i = 0; i < 6; i++)
    {
      m_CurrentVolumeBounds[i] =  m_VolumeBounds[i];
    }
    for(int i = 0; i < 3; i++)
    {
      m_CurrentVolumeDimensions[i] =  m_VolumeDimensions[i];
    }
    for(int i = 0; i < 3; i++)
    {
      m_CurrentVolumeSpacing[i] =  m_VolumeSpacing[i];
    }
    m_CurrentVolumeParametersInitialized = true;
  }

  // interface:
  CreateOpDialog();
  InitializeViewSlice();
  InitGui();

  m_OldVolumeParent = m_Volume->GetParent();
  m_Volume->SetParent(m_Volume->GetRoot());

  m_View->VmeAdd(m_Volume);
  m_View->VmeShow(m_Volume, true);
  //m_View->VmeCreatePipe(m_Volume);
  SetTrilinearInterpolation(m_Volume);
  m_View->UpdateSlicePos(0.0);
  m_View->CameraReset();
  m_View->CameraUpdate();
  
  OnNextStep();
  GetCameraAttribute(m_InitialFocalPoint, &m_InitialScaleFactor);

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
  if (!m_OutputVolume)
    mafNEW(m_OutputVolume);

  switch (m_CurrentOperation)
  {
    case  AUTOMATIC_SEGMENTATION:
    {
      m_OutputVolume->DeepCopy(m_ThresholdVolume);
    }
    break;
    case  MANUAL_SEGMENTATION:
    {
      m_OutputVolume->DeepCopy(m_ManualVolumeMask);
    }
    break;
    case  REFINEMENT_SEGMENTATION:
    case  LOAD_SEGMENTATION:
    {
      m_OutputVolume->DeepCopy(m_LoadedVolume);
    }
    break;
  }
  m_OutputVolume->ReparentTo(m_Volume->GetParent());
  m_OutputVolume->SetName(_("Segmentation Output"));
  lutPreset(4,m_OutputVolume->GetMaterial()->m_ColorLut);
  m_OutputVolume->ReparentTo(m_Volume->GetParent());
  RemoveVMEs();
  mafOp::OpDo();
}

//----------------------------------------------------------------------------
void medOpSegmentation::OpUndo()
//----------------------------------------------------------------------------
{
  if (m_OutputVolume)
  {
    m_OutputVolume->ReparentTo(NULL);
    mafDEL(m_OutputVolume);
  }
  if (m_SegmentatedVolume)
  {
    m_SegmentatedVolume->ReparentTo(NULL);
    mafDEL(m_SegmentatedVolume);
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medOpSegmentation::RemoveVMEs()
//----------------------------------------------------------------------------
{

  if(m_ManualVolumeSlice)
  {
    m_ManualVolumeSlice->ReparentTo(NULL);
    mafDEL(m_ManualVolumeSlice);
  }

  if(m_ThresholdVolume)
  {
    m_ThresholdVolume->ReparentTo(NULL);
    mafDEL(m_ThresholdVolume);
  }
  
  if(m_ManualVolumeMask)
  {
    m_ManualVolumeMask->ReparentTo(NULL);
    mafDEL(m_ManualVolumeMask);
  }

  if(m_RefinementVolumeMask)
  {
    m_RefinementVolumeMask->ReparentTo(NULL);
    mafDEL(m_RefinementVolumeMask);
  }

  if(m_ThresholdVolumeSlice)
  {
    m_ThresholdVolumeSlice->ReparentTo(NULL);
    mafDEL(m_ThresholdVolumeSlice);
  }
  if(m_EmptyVolumeSlice)
  {
    m_EmptyVolumeSlice->ReparentTo(NULL);
    mafDEL(m_EmptyVolumeSlice);
  }
}
//----------------------------------------------------------------------------
void medOpSegmentation::OpStop(int result)
//----------------------------------------------------------------------------
{
  //remove vme now on cancel on ok vme will be removed by opdo method
  if (result == OP_RUN_CANCEL)
  {
    RemoveVMEs();
    if (m_SegmentatedVolume)
    {
      m_SegmentatedVolume->ReparentTo(NULL);
      mafDEL(m_SegmentatedVolume);
    }
  }

  ResetManualUndoList();
  ResetManualRedoList();
  ResetRefinementUndoList();
  ResetRefinementRedoList();

  mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void medOpSegmentation::CreateOpDialog()
//----------------------------------------------------------------------------
{
  wxBusyCursor wait;
  wxPoint defPos = wxDefaultPosition;
  wxSize defSize = wxDefaultSize;

  m_Dialog = new mafGUIDialog(m_Label, mafCLOSEWINDOW);  
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

  m_LutWidget = new medGUILutHistogramSwatch(m_GuiDialog,m_GuiDialog->GetWidgetId(ID_LUT_CHOOSER), "LUT", m_Volume->GetOutput()->GetVTKData(), m_Volume->GetMaterial(),wxSize(135,18) );
  m_LutWidget->SetEditable(true);

  /////////////////////////////////////////////////////
  m_GuiDialog->Divider();
  m_GuiDialog->Divider();
  m_GuiDialog->Divider(0);
  m_GuiDialog->Bool(ID_ENABLE_TRILINEAR_INTERPOLATION,"Interpolation",&m_TrilinearInterpolationOn,1,"Enable/Disable tri-linear interpolation on slices");

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

//   m_LoadSegmentationButton = new mafGUIButton(m_Dialog,ID_LOAD_SEGMENTATION,_("Load"),defPos);
//   m_LoadSegmentationButton->SetListener(this);
//   m_LoadSegmentationButton->SetValidator(mafGUIValidator(this,ID_LOAD_SEGMENTATION,m_LoadSegmentationButton));

  hSz2->Add(m_OkButton,0,wxEXPAND | wxALL);
  hSz2->Add(m_CancelButton,0,wxEXPAND | wxALL);
  m_Dialog->Add(hSz1);
  m_Dialog->Add(hSz3);
  m_Dialog->Add(hSz2);

  m_GuiDialog->Divider();

  CreateSliceNavigationGui();

  CreateLoadSegmentationGui();
  CreateAutoSegmentationGui();
  CreateManualSegmentationGui();
  CreateRefinementGui();
  

  
  m_SegmentationOperationsRollOut[LOAD_SEGMENTATION]        = m_GuiDialog->RollOut(ID_LOAD_SEGMENTATION, "Load Segmentation", m_SegmentationOperationsGui[LOAD_SEGMENTATION], false);
  m_SegmentationOperationsRollOut[AUTOMATIC_SEGMENTATION]   = m_GuiDialog->RollOut(ID_AUTO_SEGMENTATION, "Thresholding", m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION], false);
  m_SegmentationOperationsRollOut[MANUAL_SEGMENTATION]      = m_GuiDialog->RollOut(ID_MANUAL_SEGMENTATION, "Manual Segmentation", m_SegmentationOperationsGui[MANUAL_SEGMENTATION], false);
  m_SegmentationOperationsRollOut[REFINEMENT_SEGMENTATION]  = m_GuiDialog->RollOut(ID_REFINEMENT, "Segmentation Refinement", m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION], false);

  m_GuiDialog->Enable(ID_LOAD_SEGMENTATION,false);
  m_GuiDialog->Enable(ID_AUTO_SEGMENTATION, false);
  m_GuiDialog->Enable(ID_MANUAL_SEGMENTATION,false);
  m_GuiDialog->Enable(ID_REFINEMENT,false);

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
  mafDEL(m_SegmentationPicker);
  
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
  if(m_ThresholdVolumeSlice)
  {
    m_View->VmeShow(m_ThresholdVolumeSlice,false);
    m_View->VmeRemove(m_ThresholdVolumeSlice);
  }
  if(m_EmptyVolumeSlice)
  {
    m_View->VmeShow(m_EmptyVolumeSlice,false);
    m_View->VmeRemove(m_EmptyVolumeSlice);
  }
  if(m_LoadedVolume)
  {
    m_View->VmeShow(m_LoadedVolume,false);
    m_View->VmeRemove(m_LoadedVolume);
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

  mafDEL(m_SegmentationPicker);

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
bool medOpSegmentation::Refinement()
//----------------------------------------------------------------------------
{
  
//   if ( !(( m_CurrentSlicePlane == XY && (m_RefinementRegionsSize>m_VolumeDimensions[0] || m_RefinementRegionsSize>m_VolumeDimensions[1]) ) ||
//          ( m_CurrentSlicePlane == YZ && (m_RefinementRegionsSize>m_VolumeDimensions[1] || m_RefinementRegionsSize>m_VolumeDimensions[2]) ) ||
//          ( m_CurrentSlicePlane == XZ && (m_RefinementRegionsSize>m_VolumeDimensions[0] || m_RefinementRegionsSize>m_VolumeDimensions[2]) )) )
//   {
//     wxMessageBox("Region size is bigger than slice dimension - Choose another value.");
//     return false;
//   }

  // threshold values empirically assigned

//   if( !(( m_RefinementRegionsSize > 5) || ( m_RefinementRegionsSize>=3 && (m_RefinementIterative || m_RefinementEverySlice) ) ))
//     if (wxMessageBox( "Region size is too big, it could take a long time  - Continue?", "Warning", wxYES_NO, NULL) == wxNO)
//       return false;
  
  wxBusyCursor wait_cursor;
  wxBusyInfo wait(_("Wait! The algorithm could take long time!"));

  vtkDataSet *inputDataSet = vtkDataSet::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_RefinementVolumeMask)->GetOutput()->GetVTKData());

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
        if(ApplyRefinementFilter2(im, filteredImage) && filteredImage)
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
          if(ApplyRefinementFilter2(im, filteredImage) && filteredImage)
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

      m_RefinementVolumeMask->SetData(newSP,mafVME::SafeDownCast(m_Volume)->GetTimeStamp());
      vtkStructuredPoints *spVME = vtkStructuredPoints::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_RefinementVolumeMask)->GetOutput()->GetVTKData());
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

      m_RefinementVolumeMask->SetData(newRG,mafVME::SafeDownCast(m_Volume)->GetTimeStamp());
      vtkRectilinearGrid *rgVME = vtkRectilinearGrid::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_RefinementVolumeMask)->GetOutput()->GetVTKData());
      rgVME->Update();
    }

    m_RefinementVolumeMask->Update();

    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));


    m_View->VmeShow(m_RefinementVolumeMask,false);
    m_View->VmeShow(m_RefinementVolumeMask,true);
    SetTrilinearInterpolation(m_RefinementVolumeMask);

    m_View->CameraUpdate();
  }

  return true;
}
//----------------------------------------------------------------------------
bool medOpSegmentation::ApplyRefinementFilter2(vtkStructuredPoints *inputImage, vtkStructuredPoints *outputImage)
//----------------------------------------------------------------------------
{
  vtkMEDImageFillHolesRemoveIslands *filter = vtkMEDImageFillHolesRemoveIslands::New();
  filter->SetInput(inputImage);
  filter->SetEdgeSize(m_RefinementRegionsSize);
  if(m_RefinementSegmentationAction == ID_REFINEMENT_HOLES_FILL)
  {
    filter->SetAlgorithmToFillHoles();
  }
  else if(m_RefinementSegmentationAction == ID_REFINEMENT_ISLANDS_REMOVE)
  {
    filter->SetAlgorithmToRemoveIslands();
  }
  filter->Update();
  outputImage->DeepCopy(filter->GetOutput());
  outputImage->Update();
  filter->Delete();
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
    holeFillingFilter->SetMajorityThreshold( m_MajorityThreshold ); 

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
void medOpSegmentation::CreateAutoSegmentationGui()
//----------------------------------------------------------------------------
{
  mafGUI *currentGui = new mafGUI(this);

  currentGui->Label(_("Threshold"),true);
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
  m_AutomaticThreshold=sr[1];
  m_AutomaticUpperThreshold=sr[1];
  m_AutomaticThresholdSlider = new mafGUILutSlider(currentGui,-1,wxPoint(0,0),wxSize(300,24));
  m_AutomaticThresholdSlider->SetListener(this);
  m_AutomaticThresholdSlider->SetText(1,"Threshold");  
  m_AutomaticThresholdSlider->SetRange(sr[0],sr[1]);
  m_AutomaticThresholdSlider->SetSubRange(sr[1],sr[1]);

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
  currentGui->Label("");
  currentGui->Label(_("Threshold type:"),true);
  currentGui->Radio(ID_AUTOMATIC_GLOBAL_THRESHOLD,"",&m_AutomaticGlobalThreshold,2,choices);

  currentGui->Label("");
  currentGui->Label("Global range:",true);
//   currentGui->Button(ID_AUTOMATIC_GLOBAL_PREVIEW,_("preview"));
//   currentGui->Enable(ID_AUTOMATIC_GLOBAL_PREVIEW,m_AutomaticGlobalThreshold==GLOBAL);
  
  
  //Slides Range
  //[ + ] [ + ] [ + ]
  //[min][range][max]
  //[ - ] [ - ] [ - ] 

  m_AutomaticRangeSlider = new mafGUILutSlider(currentGui,-1,wxPoint(0,0),wxSize(300,24));
  currentGui->Label("");
  currentGui->Label(_("Slice range settings:"),true);
  m_AutomaticRangeSlider->SetListener(this);
  m_AutomaticRangeSlider->SetText(1,"Z Axis");  
  m_AutomaticRangeSlider->SetRange(1,m_VolumeDimensions[2]);
  m_AutomaticRangeSlider->SetSubRange(1,m_VolumeDimensions[2]);

//  currentGui->Label("");
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
  //End

  m_AutomaticListOfRange = currentGui->ListBox(ID_AUTOMATIC_LIST_OF_RANGE,"");
  currentGui->TwoButtons(ID_AUTOMATIC_ADD_RANGE,ID_AUTOMATIC_REMOVE_RANGE,("Add"),_("Remove"));
  currentGui->Button(ID_AUTOMATIC_UPDATE_RANGE,("Update"));
  currentGui->Label("");

  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION] = currentGui;
}

//----------------------------------------------------------------------------
void medOpSegmentation::CreateLoadSegmentationGui()
//----------------------------------------------------------------------------
{
  mafGUI *currentGui = new mafGUI(this);

  currentGui->Label(&m_LoadedVolumeName);
  currentGui->TwoButtons(ID_LOAD_SEGMENTATION,ID_RESET_LOADED,"Load","Reset");
  m_SegmentationOperationsGui[LOAD_SEGMENTATION] = currentGui;

}
//----------------------------------------------------------------------------
void medOpSegmentation::CreateManualSegmentationGui()
//----------------------------------------------------------------------------
{

  mafGUI *currentGui = new mafGUI(this);

  //////////////////////////////////////////////////////////////////////////
  // Brush Editing options
  //////////////////////////////////////////////////////////////////////////

  wxStaticBoxSizer *brushEditingSizer = new wxStaticBoxSizer(wxVERTICAL, currentGui, "Brush Options");
  
  // BRUSH SHAPE
  wxString shapes[2];
  shapes[0] = wxString("circle");
  shapes[1] = wxString("square");
  int w_id = currentGui->GetWidgetId(ID_MANUAL_BRUSH_SHAPE);;

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

  m_ManualBrushSizeSlider = new mafGUIFloatSlider(currentGui, id,m_ManualBrushSize,1,int(min(m_VolumeDimensions[0],m_VolumeDimensions[1])/2), wxDefaultPosition, wxDefaultSize);

  m_ManualBrushSizeText->SetValidator(mafGUIValidator(currentGui, id, m_ManualBrushSizeText, &m_ManualBrushSize, m_ManualBrushSizeSlider, 1, int(min(m_VolumeDimensions[0],m_VolumeDimensions[1])/2)));
  m_ManualBrushSizeSlider->SetValidator(mafGUIValidator(currentGui, id, m_ManualBrushSizeSlider, &m_ManualBrushSize, m_ManualBrushSizeText));
  
  m_ManualBrushSize=1;
  m_ManualBrushSizeSlider->SetValue(m_ManualBrushSize);
  m_ManualBrushSizeText->SetValue("1");
  m_ManualBrushSizeSlider->SetNumberOfSteps(int(min(m_VolumeDimensions[0],m_VolumeDimensions[1])/4)-2);
  m_ManualBrushSizeSlider->SetRange(1,int(min(m_VolumeDimensions[0],m_VolumeDimensions[1])/4),1);
  m_ManualBrushSizeSlider->SetMax(int(min(m_VolumeDimensions[0],m_VolumeDimensions[1])/4));
  m_ManualBrushSizeSlider->SetMin(1);
  m_ManualBrushSizeSlider->Update();
  m_ManualBrushSizeText->Update();

  brushSizeSizer->Add(brushSizeLab,  0, wxRIGHT, 5);
  brushSizeSizer->Add(m_ManualBrushSizeText, 0);
  brushSizeSizer->Add(m_ManualBrushSizeSlider, 0);

  brushEditingSizer->Add(brushShapesSizer, 0, wxALL, 1);
  brushEditingSizer->Add(brushSizeSizer, 0, wxALL, 1);
 
  currentGui->Add(brushEditingSizer, wxALIGN_CENTER_HORIZONTAL);
  
  currentGui->TwoButtons(ID_MANUAL_UNDO,ID_MANUAL_REDO,"Undo","Redo");


  m_SegmentationOperationsGui[MANUAL_SEGMENTATION] = currentGui;

  EnableManualSegmentationGui();

}

//----------------------------------------------------------------------------
void medOpSegmentation::EnableManualSegmentationGui()
//----------------------------------------------------------------------------
{
  //brush options
  // erase/select, shape, size, continuous picking
  m_ManualBrushShapeRadioBox->Enable(true);
  m_ManualBrushSizeText->Enable(true);
  m_ManualBrushSizeSlider->Enable(true);

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

  wxTextCtrl *refinementRegionSizeText = new wxTextCtrl (currentGui, w_id, "", wxDefaultPosition, wxSize(text_w,  18), wxSUNKEN_BORDER,wxDefaultValidator,"Size:");
  
  wxSlider *sli = new wxSlider(currentGui, w_id,1,1,stepsNumber, wxDefaultPosition, wxSize(slider_w,18));
  sli->SetValidator(mafGUIValidator(currentGui,w_id,sli,&m_RefinementRegionsSize,refinementRegionSizeText));
  refinementRegionSizeText->SetValidator(mafGUIValidator(currentGui,w_id,refinementRegionSizeText,&m_RefinementRegionsSize,sli,1,stepsNumber)); //- if uncommented, remove also wxTE_READONLY from the text (in both places)
  wxStaticText *sizeText = new wxStaticText(currentGui,w_id,"Size: ");
  wxBoxSizer *regionSizeSizer = new wxBoxSizer(wxHORIZONTAL);
  regionSizeSizer->Add(sizeText,0);
  regionSizeSizer->Add(refinementRegionSizeText, 0);
  regionSizeSizer->Add(sli,  0);

  currentGui->Add(regionSizeSizer);

  // Switch on/off the "apply on every slice" option
  m_RefinementEverySlice = 0;
  currentGui->Bool(ID_REFINEMENT_EVERY_SLICE, mafString("Global"), &m_RefinementEverySlice, 0, mafString("Apply refinement procedure on every slice"));

  m_RefinementIterative = 0;
  //currentGui->Bool(ID_REFINEMENT_ITERATIVE, mafString("Iterative"), &m_RefinementIterative, 0, mafString("Switch on/off the iterative feature"));

  currentGui->TwoButtons(ID_REFINEMENT_UNDO, ID_REFINEMENT_REDO, "Undo", "Redo");

  currentGui->Button(ID_REFINEMENT_APPLY, mafString("Apply"), "");

  //currentGui->Integer(-1,"m. thr.",&m_MajorityThreshold,0);

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


  m_GuiDialog->FitGui();
  m_GuiDialog->Update();

}


//------------------------------------------------------------------------
void medOpSegmentation::InitSegmentedVolume()
//------------------------------------------------------------------------
{
  mafNEW(m_SegmentatedVolume);
  m_SegmentatedVolume->SetVolumeLink(m_Volume);
  m_SegmentatedVolume->SetName("Segmented Volume");
  m_SegmentatedVolume->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
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
  //m_View->VmeCreatePipe(m_ThresholdVolume);
  m_View->CameraUpdate();
}

//------------------------------------------------------------------------
void medOpSegmentation::InitManualVolumeMask()
//------------------------------------------------------------------------
{
  if (!m_ManualVolumeMask)
    mafNEW(m_ManualVolumeMask);
  if(m_LoadedVolume)
  {
    m_ManualVolumeMask->DeepCopy(m_LoadedVolume);
  }
  else
  {
    m_ManualVolumeMask->DeepCopy(m_ThresholdVolume);
  }
  
  m_ManualVolumeMask->SetName("Manual Volume Mask");
  m_ManualVolumeMask->ReparentTo(m_Volume->GetParent());
  m_ManualVolumeMask->Update();
}

//------------------------------------------------------------------------
void medOpSegmentation::InitRefinementVolumeMask()
//------------------------------------------------------------------------
{
  if (m_RefinementVolumeMask)
  {
    m_View->VmeRemove(m_RefinementVolumeMask);
    mafDEL(m_RefinementVolumeMask);
  }

  mafNEW(m_RefinementVolumeMask);

  m_RefinementVolumeMask->DeepCopy(m_ManualVolumeMask);
  m_RefinementVolumeMask->SetName("Refinement Volume Mask");
  
  m_RefinementVolumeMask->ReparentTo(m_Volume->GetParent());
  vtkLookupTable *lut = m_RefinementVolumeMask->GetMaterial()->m_ColorLut;
  InitMaskColorLut(lut);
  lut->SetTableRange(0,255);
  m_RefinementVolumeMask->GetMaterial()->UpdateFromTables();
  m_ManualVolumeSlice->Update();
  m_RefinementVolumeMask->Update();
  m_View->VmeAdd(m_RefinementVolumeMask);
  //m_View->VmeCreatePipe(m_RefinementVolumeMask);
}

//------------------------------------------------------------------------
void medOpSegmentation::OnAutomaticStep()
//------------------------------------------------------------------------
{
  //gui stuff
  
  m_SnippetsLabel->SetLabel( _("Left click + Ctrl to select lower threshold.  Left click + Alt to select upper threshold"));
  m_Dialog->Update();
  UpdateThresholdLabel();
  m_GuiDialog->Enable(ID_AUTO_SEGMENTATION,true);
  m_GuiDialog->Enable(ID_BUTTON_PREV,true);
  if(m_LoadedVolume == NULL)
  {
    m_GuiDialog->Enable(ID_BUTTON_NEXT,true);
    m_OldAutomaticThreshold = MAXINT;
    m_OldAutomaticUpperThreshold = MAXINT;

    InitEmptyVolumeSlice();
    UpdateThresholdRealTimePreview();
    SetTrilinearInterpolation(m_ThresholdVolumeSlice);
    UpdateSlice();
    m_View->ChangeView(m_CurrentSlicePlane);
    m_View->CameraUpdate();
  }
  else
  {
    m_View->VmeShow(m_ThresholdVolume,false);

    m_GuiDialog->Enable(ID_BUTTON_NEXT,true);
   
    if (vtkStructuredPoints::SafeDownCast(m_LoadedVolume->GetOutput()->GetVTKData()))
    {
      vtkStructuredPoints *newData = vtkStructuredPoints::SafeDownCast(m_LoadedVolume->GetOutput()->GetVTKData());
      m_ThresholdVolume->SetData(newData,mafVME::SafeDownCast(m_Volume)->GetTimeStamp());
      vtkStructuredPoints *spVME = vtkStructuredPoints::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_ThresholdVolume)->GetOutput()->GetVTKData());
      spVME->Update();

    }
    else
    {
      vtkRectilinearGrid *newData = vtkRectilinearGrid::SafeDownCast(m_LoadedVolume->GetOutput()->GetVTKData());
      m_ThresholdVolume->SetData(newData,mafVME::SafeDownCast(m_Volume)->GetTimeStamp());
      vtkRectilinearGrid *rgVME = vtkRectilinearGrid::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_ThresholdVolume)->GetOutput()->GetVTKData());
      rgVME->Update();
    }

    m_ThresholdVolume->Update();

    m_SegmentationColorLUT = m_ThresholdVolume->GetMaterial()->m_ColorLut;
    InitMaskColorLut(m_SegmentationColorLUT);

    m_ThresholdVolume->GetMaterial()->m_ColorLut->SetTableRange(0,255);
    mmaVolumeMaterial *currentVolumeMaterial = ((mafVMEOutputVolume *)m_ThresholdVolume->GetOutput())->GetMaterial();
    currentVolumeMaterial->UpdateFromTables();

    
    //m_View->VmeShow(m_ThresholdVolume,true);
    SetTrilinearInterpolation(m_ThresholdVolume);

    m_View->CameraUpdate();

    //m_CurrentOperation = AUTOMATIC_SEGMENTATION;
    //OnNextStep();
  }

}

//------------------------------------------------------------------------
void medOpSegmentation::OnManualStep()
//------------------------------------------------------------------------
{
  //gui stuff
  //set brush cursor - enable drawing
  // brush size slider: min = 1; max = slice size
  m_SnippetsLabel->SetLabel( _("Left click + Ctrl Draw.  Left click + Alt Erase"));

//   int maxBrushSize;
//   maxBrushSize = min(m_VolumeDimensions[0], m_VolumeDimensions[1]);
//   maxBrushSize = min(maxBrushSize, m_VolumeDimensions[2]);
//   maxBrushSize = round(maxBrushSize/2.0);
// 
  m_ManualBrushSize=1;
  m_ManualBrushSizeSlider->SetValue(m_ManualBrushSize);
  m_ManualBrushSizeText->SetValue("1");
  m_ManualBrushSizeSlider->SetNumberOfSteps(int(min(m_VolumeDimensions[0],m_VolumeDimensions[1])/4)-2);
  m_ManualBrushSizeSlider->SetRange(1,int(min(m_VolumeDimensions[0],m_VolumeDimensions[1])/4),1);
  m_ManualBrushSizeSlider->SetMax(int(min(m_VolumeDimensions[0],m_VolumeDimensions[1])/4));
  m_ManualBrushSizeSlider->SetMin(1);
  m_ManualBrushSizeSlider->Update();
  m_ManualBrushSizeText->Update();
  
  m_ManualPER->SetTargetVolumeSpacing(max(m_VolumeSpacing[0] , m_VolumeSpacing[1]));
  m_ManualPER->SetRadius(double(m_ManualBrushSize)/2.0);
  m_View->CameraUpdate();

  
  m_SER->GetAction("pntActionAutomatic")->UnBindDevice(m_DialogMouse);
  m_SER->GetAction("pntActionAutomatic")->UnBindInteractor(m_AutomaticPER);
  m_SER->GetAction("pntEditingAction")->BindInteractor(m_ManualPER);
  m_SER->GetAction("pntEditingAction")->BindDevice(m_DialogMouse);
  
  
  m_AutomaticScalarTextMapper->SetInput("");


  wxCursor cursor = wxCursor( wxCURSOR_PENCIL );
  m_View->GetWindow()->SetCursor(cursor);
  m_ManualPER->EnableDrawing(true);

  m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_PICKING_MODALITY, m_CurrentSlicePlane);
  m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_UNDO, false);
  m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_REDO, false);
  m_GuiDialog->Enable(ID_MANUAL_SEGMENTATION,true);
  m_GuiDialog->Update();
  //logic stuff
  UpdateVolumeSlice();
  m_View->VmeShow(m_ManualVolumeSlice, true);
  SetTrilinearInterpolation(m_ManualVolumeSlice);

  m_GuiDialog->Enable(ID_BUTTON_NEXT,true);
  m_CurrentBrushMoveEventCount = 0;


  UndoBrushPreview();
  ApplyVolumeSliceChanges();

  UpdateSlice();

  CreateRealDrawnImage();

  m_View->ChangeView(m_CurrentSlicePlane);
  m_View->CameraUpdate();
}
//------------------------------------------------------------------------
void medOpSegmentation::OnAutomaticStepExit()
//------------------------------------------------------------------------
{
  if(m_ThresholdVolumeSlice)
  {
    m_View->VmeShow(m_ThresholdVolumeSlice,false);
  }
  if(m_EmptyVolumeSlice)
  {
    m_EmptyVolumeSlice->ReparentTo(NULL);
    mafDEL(m_EmptyVolumeSlice);
  }

  m_GuiDialog->Enable(ID_AUTO_SEGMENTATION,false);
}
//------------------------------------------------------------------------
void medOpSegmentation::OnManualStepExit()
//------------------------------------------------------------------------
{
  UndoBrushPreview();
  if(m_RealDrawnImage)
  {
    m_RealDrawnImage->Delete();
    m_RealDrawnImage = NULL;
  }

  //Gui stuff
  //set default cursor - remove draw actor  
  wxCursor cursor = wxCursor( wxCURSOR_DEFAULT );
  m_View->GetWindow()->SetCursor(cursor);
  m_ManualPER->RemoveActor();
  //logic stuff
  m_SER->GetAction("pntEditingAction")->UnBindInteractor(m_ManualPER);
  m_SER->GetAction("pntEditingAction")->UnBindDevice(m_DialogMouse);
  m_SER->GetAction("pntActionAutomatic")->BindDevice(m_DialogMouse);
  m_SER->GetAction("pntActionAutomatic")->BindInteractor(m_AutomaticPER);
  //apply residual changes
  ApplyVolumeSliceChanges(); 
  
  m_View->VmeShow(m_ManualVolumeSlice, false);
  m_GuiDialog->Enable(ID_MANUAL_SEGMENTATION,false);

  m_SnippetsLabel->SetLabel( _(""));
}

//------------------------------------------------------------------------
void medOpSegmentation::OnRefinementStep()
//------------------------------------------------------------------------
{
  //gui stuff
  m_Dialog->Update();
  m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_UNDO, false);
  m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_REDO, false);
  m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_APPLY, true);
  m_GuiDialog->Enable(ID_BUTTON_NEXT,false);
  m_GuiDialog->Enable(ID_REFINEMENT,true);
  
  //logic stuff
  InitRefinementVolumeMask();
  m_View->VmeShow(m_RefinementVolumeMask,true);
  SetTrilinearInterpolation(m_RefinementVolumeMask);

  UpdateSlice();
  m_View->ChangeView(m_CurrentSlicePlane);
  m_View->CameraUpdate();
}

//------------------------------------------------------------------------
void medOpSegmentation::OnLoadStep()
//------------------------------------------------------------------------
{
  m_GuiDialog->Enable(ID_LOAD_SEGMENTATION,true);
  m_GuiDialog->Enable(ID_BUTTON_NEXT,true);
  m_GuiDialog->Enable(ID_BUTTON_PREV,false);

  UpdateSlice();
  m_View->ChangeView(m_CurrentSlicePlane);
  m_View->CameraUpdate();
}

//------------------------------------------------------------------------
void medOpSegmentation::OnNextStep()
//------------------------------------------------------------------------
{
  switch (m_CurrentOperation)
  {
    case PRE_SEGMENTATION:
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

      //next step -> LOAD Segmentation
      OnLoadStep();
    } 
    break;
    case LOAD_SEGMENTATION:
      {
        m_GuiDialog->Enable(ID_LOAD_SEGMENTATION,false);
        if(m_LoadedVolume)
        {
          InitManualVolumeMask();
          InitManualVolumeSlice();
          m_View->VmeShow(m_LoadedVolume,false);
          UpdateSlice();
          m_GuiDialog->Enable(ID_BUTTON_PREV,true);
          //next step -> MANUAL_SEGMENTATION
          OnManualStep();
          m_SegmentationOperationsRollOut[m_CurrentOperation]->RollOut(false);
          m_CurrentOperation = AUTOMATIC_SEGMENTATION; // trick
        }
        else
        {
          mafNEW(m_EmptyVolumeSlice);
          InitEmptyVolumeSlice();
          mafNEW(m_ThresholdVolumeSlice);
          InitThresholdVolumeSlice();
          //next step -> AUTOMATIC_SEGMENTATION 
          OnAutomaticStep();
        }
      }
      break;
    case AUTOMATIC_SEGMENTATION:
    {
      OnAutomaticPreview();
      OnAutomaticStepExit();
      m_AutomaticThresholdTextMapper->SetInput("");
      m_GuiDialog->Enable(ID_AUTO_SEGMENTATION,false);
      InitManualVolumeMask();
      InitManualVolumeSlice();
      m_View->VmeShow(m_ThresholdVolume,false);
      UpdateSlice();
      m_GuiDialog->Enable(ID_BUTTON_PREV,true);
      //next step -> MANUAL_SEGMENTATION
      OnManualStep();
    }
    break;
    case  MANUAL_SEGMENTATION:
    {
      OnManualStepExit();
      //next step -> REFINEMENT_SEGMENTATION
      OnRefinementStep();
    }
    break;
    case REFINEMENT_SEGMENTATION:
    {
      SaveRefinementVolumeMask();
      m_GuiDialog->Enable(ID_REFINEMENT,false);
      m_View->VmeShow(m_RefinementVolumeMask,false);

      //next step -> none
      //OnLoadSegmentationStep();
    }
    break;
    default:
    {
      mafLogMessage("Invalid Operation");
      return;
    }
  }
  m_CurrentOperation++;

  int oldSliceIndex = m_CurrentSliceIndex;

  if( (m_CurrentOperation-1) != PRE_SEGMENTATION)
    m_SegmentationOperationsRollOut[m_CurrentOperation-1]->RollOut(false);
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
  int answer = wxMessageBox(_("Current changes will be lost if you go on previous Step \nDo you want to continue?"),_("Warning"),wxYES_NO|wxCENTER);
  if (answer == wxNO)
    return;
  
  switch (m_CurrentOperation)
  {
    case MANUAL_SEGMENTATION:
    {
      OnManualStepExit();
      
      if(m_LoadedVolume)
      {
        m_SegmentationOperationsRollOut[m_CurrentOperation]->RollOut(false);
        m_CurrentOperation = AUTOMATIC_SEGMENTATION;
        //prev step -> LOAD_SEGMENTATION
        m_View->VmeShow(m_LoadedVolume,true);
        SetTrilinearInterpolation(m_LoadedVolume);
        OnLoadStep();
        m_View->CameraUpdate();
      }
      else
      {
        //prev step -> AUTOMATIC_SEGMENTATION
        OnAutomaticStepExit();
        mafNEW(m_EmptyVolumeSlice);
        InitEmptyVolumeSlice();
        OnAutomaticStep();
        //m_View->VmeShow(m_ThresholdVolume,false);
      }
    }
    break;
    case REFINEMENT_SEGMENTATION:
    {
      m_GuiDialog->Enable(ID_REFINEMENT,false);
      m_View->VmeShow(m_RefinementVolumeMask,false);
      OnManualStep();
    }
    break;
    case AUTOMATIC_SEGMENTATION:
      {
        OnAutomaticStepExit();
        //prev step -> AUTOMATIC_SEGMENTATION
        OnLoadStep();
//         m_View->VmeShow(m_ThresholdVolume,true);
//         SetTrilinearInterpolation(m_ThresholdVolume)
      }
      break;
    default:
    {
      mafLogMessage("Invalid Operation");
      return;
    }
  }
    

  m_CurrentOperation--;

  int oldSliceIndex = m_CurrentSliceIndex;

  m_SegmentationOperationsRollOut[m_CurrentOperation+1]->RollOut(false);
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
    if(e->GetSender() == m_SegmentationOperationsGui[LOAD_SEGMENTATION])
      OnLoadSegmentationEvent(e);
    if(e->GetSender() == m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION])
      OnAutomaticSegmentationEvent(e);
    else if(e->GetSender() == m_SegmentationOperationsGui[MANUAL_SEGMENTATION])
      OnManualSegmentationEvent(e);
    else if(e->GetSender() == m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION])
      OnRefinementSegmentationEvent(e);
    else if (e->GetSender() == m_AutomaticPER && e->GetId()== MOUSE_MOVE)
    {
      m_AutomaticMouseThreshold = e->GetDouble();
      mafString text = wxString::Format("Scalar = %.3f",m_AutomaticMouseThreshold);
      m_AutomaticScalarTextMapper->SetInput(text.GetCStr());
      m_View->CameraUpdate();
    }
    else if (e->GetSender() == m_ManualPER && e->GetId()== MOUSE_MOVE)
    {
      UndoBrushPreview();
      if(e->GetDouble() > m_CurrentBrushMoveEventCount)
      {
        m_CurrentBrushMoveEventCount = e->GetDouble();
        int oldAction = m_ManualSegmentationAction;
        m_ManualSegmentationAction = MANUAL_SEGMENTATION_SELECT;
        m_LastMouseMovePointID = e->GetArg();
        OnBrushEvent(e);
        m_ManualSegmentationAction = oldAction;
      }

      m_View->CameraUpdate();
    }
    else if (e->GetSender() == m_SegmentationPicker && e->GetId()== medInteractorSegmentationPicker::VME_ALT_PICKED)
    {
      //Picking during automatic segmentation
      if (m_CurrentOperation==AUTOMATIC_SEGMENTATION)
      {
        m_AutomaticUpperThreshold = m_AutomaticMouseThreshold;
        m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Update();
        m_AutomaticThreshold=min(m_AutomaticUpperThreshold,m_AutomaticThreshold);
        m_AutomaticThresholdSlider->SetSubRange(m_AutomaticThreshold,m_AutomaticUpperThreshold);
        UpdateThresholdLabel();
      }
      //Picking during manual segmentation
      else if(m_CurrentOperation == MANUAL_SEGMENTATION)
        StartDraw(e, true);
    }
    //SWITCH
    else switch(e->GetId()) 
    {
    case ID_BUTTON_NEXT:
      {
        if (m_CurrentOperation < NUMBER_OF_OPERATIONS-1)//if the current operation isn't the last operation
          OnNextStep();
      }
      break;
    case ID_BUTTON_PREV:
      {
        if (m_CurrentOperation > 0)//if the current operation isn't the first operation
          OnPreviousStep();
      }
      break;
    case ID_SLICE_SLIDER:
      {
        //if (m_NumSliceSliderEvents == 2)//Validator generate 2 events when the user move the slider REMOVED: GEnerate problems on slice update!
        {
          m_NumSliceSliderEvents = 0;
          if (m_CurrentSliceIndex != m_OldSliceIndex && m_CurrentOperation==MANUAL_SEGMENTATION)
          {
            UndoBrushPreview();
            ApplyVolumeSliceChanges();
          }
          else if (m_CurrentOperation==AUTOMATIC_SEGMENTATION)
          {
            InitEmptyVolumeSlice();
            UpdateThresholdRealTimePreview();
          }
          UpdateSlice();
          if(m_CurrentOperation == MANUAL_SEGMENTATION)
          {
            CreateRealDrawnImage();
          }
        }
//         else
//         {
//           m_NumSliceSliderEvents++;
//         }
      }
      break;
    case ID_SLICE_NEXT:
      {
        if(m_CurrentSliceIndex<m_SliceSlider->GetMax())
          m_CurrentSliceIndex++;
        if (m_CurrentSliceIndex != m_OldSliceIndex && m_CurrentOperation==MANUAL_SEGMENTATION)
        {
          UndoBrushPreview();
          ApplyVolumeSliceChanges();
        }
        else if (m_CurrentOperation==AUTOMATIC_SEGMENTATION)
        {
          InitEmptyVolumeSlice();
          UpdateThresholdRealTimePreview();
        }
        UpdateSlice();
        if(m_CurrentOperation == MANUAL_SEGMENTATION)
        {
          CreateRealDrawnImage();
        }
        break;
      }
    case ID_SLICE_PREV:
      {
        if(m_CurrentSliceIndex>1)
          m_CurrentSliceIndex--;
        if (m_CurrentOperation==MANUAL_SEGMENTATION)
        {
          UndoBrushPreview();
          ApplyVolumeSliceChanges();
        }
        else if (m_CurrentSliceIndex != m_OldSliceIndex && m_CurrentOperation==AUTOMATIC_SEGMENTATION)
        {
          InitEmptyVolumeSlice();
          UpdateThresholdRealTimePreview();
        }
        UpdateSlice();
        if(m_CurrentOperation == MANUAL_SEGMENTATION)
        {
          CreateRealDrawnImage();
        }
        break;
      }
    case ID_SLICE_TEXT:
      {
        if (m_CurrentSliceIndex != m_OldSliceIndex && m_CurrentOperation==MANUAL_SEGMENTATION)
        {
          UndoBrushPreview();
          ApplyVolumeSliceChanges();
        }
        else if (m_CurrentOperation==AUTOMATIC_SEGMENTATION)
        {
          InitEmptyVolumeSlice();
          UpdateThresholdRealTimePreview();
        }
        UpdateSlice();
        if(m_CurrentOperation == MANUAL_SEGMENTATION)
        {
          CreateRealDrawnImage();
        }
        break;
      }
    case ID_SLICE_PLANE:
      {
        m_CurrentSliceIndex = 1;
        if (m_CurrentOperation == MANUAL_SEGMENTATION)
        {
          UndoBrushPreview();
          ApplyVolumeSliceChanges();
        }
        else if (m_CurrentOperation==AUTOMATIC_SEGMENTATION)
        {
          InitEmptyVolumeSlice();
          UpdateThresholdRealTimePreview();
        }
        UpdateSlice();
        if(m_CurrentOperation == MANUAL_SEGMENTATION)
        {
          CreateRealDrawnImage();
        }
        m_View->ChangeView(m_CurrentSlicePlane);
        InitGui();
        if (m_CurrentOperation == AUTOMATIC_SEGMENTATION)
          OnChangeThresholdType();
        else if(m_CurrentOperation == MANUAL_SEGMENTATION)
          m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Update();
        m_View->CameraUpdate();
        break;
      }
    case VME_PICKED:
      {
        //Picking during automatic segmentation
        if (m_CurrentOperation==AUTOMATIC_SEGMENTATION)
        {
          m_AutomaticThreshold = m_AutomaticMouseThreshold;
          m_AutomaticUpperThreshold=max(m_AutomaticUpperThreshold,m_AutomaticThreshold);
          m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Update();
          m_AutomaticThresholdSlider->SetSubRange(m_AutomaticThreshold,m_AutomaticUpperThreshold);
          UpdateThresholdLabel();
        }
        //Picking during manual segmentation
        if(m_CurrentOperation == MANUAL_SEGMENTATION)
          StartDraw(e, false);
        m_CurrentBrushMoveEventCount = 0;
        break;
      }
    case VME_PICKING:
      {
        if(m_CurrentOperation == MANUAL_SEGMENTATION)
        {          
          OnBrushEvent(e);
          m_ManualVolumeSlice->GetOutput()->GetVTKData()->Update();
          m_ManualVolumeSlice->Update();
          CreateRealDrawnImage();
          m_PickingStarted=false;
        }
        break;
      }
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
          UpdateThresholdRealTimePreview();
          UpdateSlice();
          m_View->CameraUpdate();
        }
        //Windowing
        else if(e->GetSender() == m_LutSlider)
        {
          double low, hi;
          m_LutSlider->GetSubRange(&low,&hi);
          m_ColorLUT->SetTableRange(low,hi);
          m_View->CameraUpdate();
          mafEventMacro(mafEvent(this,CAMERA_UPDATE));
        }
        else if(e->GetSender() == m_AutomaticRangeSlider)
        {
          double low, hi;
          m_AutomaticRangeSlider->GetSubRange(&low,&hi);

          if(m_CurrentSliceIndex < low)
          {
            m_CurrentSliceIndex = low;
          }
          if(m_CurrentSliceIndex > hi)
          {
            m_CurrentSliceIndex = hi;
          }

          InitEmptyVolumeSlice();
          UpdateThresholdRealTimePreview();
          UpdateSlice();
          m_View->CameraUpdate();
        }
        break;
      }
    case ID_LUT_CHOOSER:
      {
        double *sr;
        sr = m_ColorLUT->GetRange();
        m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);
        m_View->CameraUpdate();
        break;
      }
    case ID_ENABLE_TRILINEAR_INTERPOLATION:
      {
        SetTrilinearInterpolation(m_Volume);
        SetTrilinearInterpolation(m_ManualVolumeMask);
        SetTrilinearInterpolation(m_ManualVolumeSlice);
        SetTrilinearInterpolation(m_ThresholdVolume);
        SetTrilinearInterpolation(m_ThresholdVolumeSlice);
        SetTrilinearInterpolation(m_EmptyVolumeSlice);
        SetTrilinearInterpolation(m_RefinementVolumeMask);
        SetTrilinearInterpolation(m_LoadedVolume);
        m_View->CameraUpdate();
      }break;
    default:
      break;
    }
  }
}

//------------------------------------------------------------------------
void medOpSegmentation::StartDraw(mafEvent *e, bool erase)
//------------------------------------------------------------------------
{
  if (erase) m_ManualSegmentationAction = MANUAL_SEGMENTATION_ERASE;
  else m_ManualSegmentationAction = MANUAL_SEGMENTATION_SELECT;
  //Picking starts here I need to save an undo stack
  if(!m_PickingStarted)
  {
    UndoRedoState urs;
    urs.dataArray = vtkUnsignedCharArray::New();
    urs.dataArray->DeepCopy( m_ManualVolumeSlice->GetOutput()->GetVTKData()->GetPointData()->GetScalars() );
    urs.dataArray->SetName("SCALARS");
    urs.plane=m_CurrentSlicePlane;
    urs.slice=m_CurrentSliceIndex;
    m_ManualUndoList.push_back( urs );

    m_PickingStarted = true;

    //On edit a new branch of redo-list starts, i need to clear the redo stack
    ResetManualRedoList();
    m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_REDO, false);
  }
  else
  {
    OnBrushEvent(e);
    m_PickingStarted=false;
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

    SelectBrushImage(datasetPoint[0], datasetPoint[1], datasetPoint[2], m_ManualSegmentationAction == MANUAL_SEGMENTATION_SELECT);
    

    m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_UNDO, m_ManualUndoList.size()>0);

    m_View->CameraUpdate();
  }
}

//------------------------------------------------------------------------
void medOpSegmentation::SaveRefinementVolumeMask()
//------------------------------------------------------------------------
{
  m_SegmentatedVolume->SetRefinementVolumeMask(m_RefinementVolumeMask);
  m_SegmentatedVolume->GetOutput()->Update();
  m_SegmentatedVolume->Update();
}


//------------------------------------------------------------------------
void medOpSegmentation::OnChangeThresholdType()
//------------------------------------------------------------------------
{
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_ADD_RANGE,m_AutomaticGlobalThreshold == RANGE );
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_REMOVE_RANGE,m_AutomaticGlobalThreshold == RANGE );
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_UPDATE_RANGE,m_AutomaticGlobalThreshold == RANGE );
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_LIST_OF_RANGE,m_AutomaticGlobalThreshold == RANGE );
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_INCREASE_MIN_RANGE_VALUE,m_AutomaticGlobalThreshold == RANGE );
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_INCREASE_MAX_RANGE_VALUE,m_AutomaticGlobalThreshold == RANGE );
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_DECREASE_MIN_RANGE_VALUE,m_AutomaticGlobalThreshold == RANGE );
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_DECREASE_MAX_RANGE_VALUE,m_AutomaticGlobalThreshold == RANGE );
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_DECREASE_MIDDLE_RANGE_VALUE,m_AutomaticGlobalThreshold == RANGE );
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_INCREASE_MIDDLE_RANGE_VALUE,m_AutomaticGlobalThreshold == RANGE );
  m_AutomaticRangeSlider->Enable(m_AutomaticGlobalThreshold == RANGE);
  m_SegmentationOperationsGui[AUTOMATIC_SEGMENTATION]->Enable(ID_AUTOMATIC_GLOBAL_PREVIEW,m_AutomaticGlobalThreshold==GLOBAL);


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
  UpdateThresholdRealTimePreview();

  m_View->CameraUpdate();
  //OnAutomaticPreview();
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
  UpdateThresholdRealTimePreview();
  m_View->CameraUpdate();
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
  InitMaskColorLut(m_SegmentationColorLUT);

  m_ThresholdVolume->GetMaterial()->m_ColorLut->SetTableRange(0,255);
  mmaVolumeMaterial *currentVolumeMaterial = ((mafVMEOutputVolume *)m_ThresholdVolume->GetOutput())->GetMaterial();
  currentVolumeMaterial->UpdateFromTables();

  m_View->VmeShow(m_ThresholdVolume,false);
  m_View->VmeShow(m_ThresholdVolume,true);
  SetTrilinearInterpolation(m_ThresholdVolume);

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

    //OnAutomaticPreview();
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
  UpdateThresholdRealTimePreview();
  m_View->CameraUpdate();
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
  UpdateThresholdRealTimePreview();
  m_View->CameraUpdate();
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
            m_AutomaticRangeSlider->SetSubRange(min,min);
          else
            m_AutomaticRangeSlider->SetSubRange(min,m_CurrentSliceIndex);
        }
        break;
      case mafGUILutSlider::MIN_BUTTON:
        {
          if (m_CurrentSliceIndex>=max)
            m_AutomaticRangeSlider->SetSubRange(max,max);
          else
            m_AutomaticRangeSlider->SetSubRange(m_CurrentSliceIndex,max);
        }
        break;
      case mafGUILutSlider::MIDDLE_BUTTON:
        {
          double diff = (max-min)/2;
          min = m_CurrentSliceIndex-diff;
          max = m_CurrentSliceIndex+diff;
          m_AutomaticRangeSlider->SetSubRange(min,max);
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
      if (m_AutomaticRanges.size()>0)
        OnAutomaticPreview();
      m_GuiDialog->Enable(ID_BUTTON_NEXT,m_AutomaticGlobalThreshold==RANGE && m_AutomaticRanges.size()>0);
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
void medOpSegmentation::ReloadUndoRedoState(vtkDataSet *dataSet,UndoRedoState state)
//------------------------------------------------------------------------
{
  if (state.plane!=m_CurrentSlicePlane || state.slice!=m_CurrentSliceIndex)
  {

    m_CurrentSlicePlane=state.plane;
    m_CurrentSliceIndex=state.slice;
    //m_View->ChangeView(m_CurrentSlicePlane);
    UpdateSlice();
    InitGui();
    m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Update();
  }

  double focalPoint[3];
  double scaleFactor;
  GetCameraAttribute(focalPoint, &scaleFactor);
  double bounds[4];
  GetVisualizedBounds(focalPoint, scaleFactor, bounds);

  vtkImageData* undoRedoData = vtkImageData::New();
  undoRedoData->DeepCopy(dataSet);
  undoRedoData->Update();

  for(int i = 0; i < undoRedoData->GetPointData()->GetScalars()->GetNumberOfTuples(); i++)
  {
    undoRedoData->GetPointData()->GetScalars()->SetTuple1(i,(unsigned char)abs(state.dataArray->GetTuple1(i) - dataSet->GetPointData()->GetScalars()->GetTuple1(i)));
  }
  undoRedoData->Update();

  dataSet->GetPointData()->SetScalars(state.dataArray);
  //Show changes
  m_View->VmeShow(m_ManualVolumeSlice, false);
  m_ManualVolumeSlice->Update();
  m_View->VmeShow(m_ManualVolumeSlice, true);
  
  SetTrilinearInterpolation(m_ManualVolumeSlice);

  if(ResetZoom(undoRedoData,bounds))
  {
    m_View->ChangeView(m_CurrentSlicePlane);
  }

  m_View->CameraUpdate();

  undoRedoData->Delete();
}

//------------------------------------------------------------------------
void medOpSegmentation::OnManualSegmentationEvent(mafEvent *e)
//------------------------------------------------------------------------
{
  switch(e->GetId())
  {

  case ID_MANUAL_BRUSH_SHAPE:
    {
      m_ManualBrushShape = m_ManualBrushShapeRadioBox->GetSelection();
      if(m_ManualBrushShape == CIRCLE_BRUSH_SHAPE)
        m_ManualPER->SetBrushShape(CIRCLE_BRUSH_SHAPE);
      else 
        m_ManualPER->SetBrushShape(SQUARE_BRUSH_SHAPE);
      UndoBrushPreview();
      int oldAction = m_ManualSegmentationAction;
      m_ManualSegmentationAction = MANUAL_SEGMENTATION_SELECT;
      mafEvent dummyEvent;
      vtkPoints *dummyPoints = vtkPoints::New();
      dummyEvent.SetVtkObj(dummyPoints);
      dummyEvent.SetArg(m_LastMouseMovePointID);
      OnBrushEvent(&dummyEvent);
      dummyPoints->Delete();
      m_View->CameraUpdate();
      m_ManualSegmentationAction = oldAction;
      break;
    }
  case ID_MANUAL_BRUSH_SIZE:
    {
      m_ManualBrushSize = int(m_ManualBrushSize);
      m_ManualPER->SetRadius(double(m_ManualBrushSize)/2);
      UndoBrushPreview();
      int oldAction = m_ManualSegmentationAction;
      m_ManualSegmentationAction = MANUAL_SEGMENTATION_SELECT;
      mafEvent dummyEvent;
      vtkPoints *dummyPoints = vtkPoints::New();
      dummyEvent.SetVtkObj(dummyPoints);
      dummyEvent.SetArg(m_LastMouseMovePointID);
      OnBrushEvent(&dummyEvent);
      dummyPoints->Delete();
      m_View->CameraUpdate();
      m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Update();
      m_ManualSegmentationAction = oldAction;
      break;
    }
  case ID_MANUAL_UNDO:
    {
      int numOfChanges = m_ManualUndoList.size();
      if(numOfChanges)
      {
        vtkDataSet *dataSet = m_ManualVolumeSlice->GetOutput()->GetVTKData();

        //if i changed slice/plane from last edit the redo information
        //are in the plane-slice of last edit (where i saved last undo info).
        if (m_CurrentSlicePlane!=m_ManualUndoList[numOfChanges-1].plane ||
            m_CurrentSliceIndex != m_ManualUndoList[numOfChanges-1].slice)
        {
          m_CurrentSlicePlane=m_ManualUndoList[numOfChanges-1].plane;
          m_CurrentSliceIndex=m_ManualUndoList[numOfChanges-1].slice;
          UpdateSlice();
        }
        
        //Add current state to Redo-list
        UndoRedoState urs;
        urs.dataArray = vtkUnsignedCharArray::New();
        urs.dataArray->DeepCopy( dataSet->GetPointData()->GetScalars() );
        urs.dataArray->SetName("SCALARS");
        urs.plane=m_CurrentSlicePlane;
        urs.slice=m_CurrentSliceIndex;
        m_ManualRedoList.push_back(urs);

        //Update current slice with Undo-data
        ReloadUndoRedoState(dataSet,m_ManualUndoList[numOfChanges-1]);
         
        //Remove item from undo list
        vtkDEL(m_ManualUndoList[numOfChanges-1].dataArray);
        m_ManualUndoList.pop_back();

        //Enable-disable buttons
        m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_UNDO, m_ManualUndoList.size()>0);
        m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_REDO, true);

        ApplyVolumeSliceChanges();
      }
      break;
    }
  case ID_MANUAL_REDO:
    {
      int numOfChanges = m_ManualRedoList.size();
      if(numOfChanges)
      {
        vtkDataSet *dataSet = m_ManualVolumeSlice->GetOutput()->GetVTKData();

        //if i changed slice/plane from last edit the redo information
        //are in the plane-slice of last edit (where i saved last undo info).
        if (m_CurrentSlicePlane!=m_ManualRedoList[numOfChanges-1].plane ||
          m_CurrentSliceIndex != m_ManualRedoList[numOfChanges-1].slice)
        {
          m_CurrentSlicePlane=m_ManualRedoList[numOfChanges-1].plane;
          m_CurrentSliceIndex=m_ManualRedoList[numOfChanges-1].slice;
          UpdateSlice();
        }


        //Add current state to Undo-list
        UndoRedoState urs;
        urs.dataArray = vtkUnsignedCharArray::New();
        urs.dataArray->DeepCopy( dataSet->GetPointData()->GetScalars() );
        urs.dataArray->SetName("SCALARS");
        urs.plane=m_CurrentSlicePlane;
        urs.slice=m_CurrentSliceIndex;
        m_ManualUndoList.push_back(urs);

        //Update current slice with Redo-data
        ReloadUndoRedoState(dataSet,m_ManualRedoList[numOfChanges-1]);
       
        //remove item from undo list
        vtkDEL(m_ManualRedoList[numOfChanges-1].dataArray);
        m_ManualRedoList.pop_back();

        //Enable-disable buttons
        m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_UNDO, true);
        m_SegmentationOperationsGui[MANUAL_SEGMENTATION]->Enable(ID_MANUAL_REDO, m_ManualRedoList.size()>0);

        //UndoBrushPreview();
        ApplyVolumeSliceChanges();
      }
      break;
    }
   default:
    mafEventMacro(*e);
  }
}
//------------------------------------------------------------------------
void medOpSegmentation::OnLoadSegmentationEvent(mafEvent *e)
//------------------------------------------------------------------------
{
  switch(e->GetId())
  {
  case ID_LOAD_SEGMENTATION:
    {
      mafString title = mafString("Select a segmentation:");
      mafEvent e(this,VME_CHOOSE);
      e.SetString(&title);
      e.SetArg((long)(&medOpSegmentation::SegmentedVolumeAccept));
      mafEventMacro(e);
      mafVME *vme = (mafVME *)e.GetVme();
      m_LoadedVolume = mafVMEVolumeGray::SafeDownCast(vme);

      if(m_LoadedVolume)
      {
        m_SegmentationColorLUT = m_LoadedVolume->GetMaterial()->m_ColorLut;
        InitMaskColorLut(m_SegmentationColorLUT);
        m_LoadedVolume->GetMaterial()->UpdateFromTables();
        m_LoadedVolume->Update();

        m_LoadedVolumeName = m_LoadedVolume->GetName();
        m_SegmentationOperationsGui[LOAD_SEGMENTATION]->Update();

        // add vme parents to the view

        mafVME* parent = m_LoadedVolume;
        std::vector<mafVME*> parents;
        do 
        {
          parents.push_back(parent);
          parent = parent->GetParent();
        }
        while(!parent->IsA("mafVMERoot"));

        for(int p = 0; p < parents.size(); p++)
        {
          m_View->VmeAdd(parents.at(parents.size() - (p + 1)));
        }
        m_View->VmeShow(m_LoadedVolume,true);
        SetTrilinearInterpolation(m_LoadedVolume);

        UpdateSlice();
        m_View->ChangeView(m_CurrentSlicePlane);
        m_View->CameraUpdate();
      }
    }
    break;
  case ID_RESET_LOADED:
    {
      if(m_LoadedVolume)
      {
        m_View->VmeShow(m_LoadedVolume,false);
      }
      //m_View->VmeRemove(m_LoadedVolume);
      m_View->CameraUpdate();
      m_LoadedVolume = NULL;
      m_LoadedVolumeName = "[Select input volume]";
      m_SegmentationOperationsGui[LOAD_SEGMENTATION]->Update();
    }
    break;
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
        vtkDataSet *dataSet = m_RefinementVolumeMask->GetOutput()->GetVTKData();

        vtkUnsignedCharArray *redoScalars = vtkUnsignedCharArray::New();
        redoScalars->DeepCopy( dataSet->GetPointData()->GetScalars() );
        redoScalars->SetName("SCALARS");
        m_RefinementRedoList.push_back(redoScalars);

        vtkDataArray *undoScalars = m_RefinementUndoList[numOfChanges-1];

        dataSet->GetPointData()->SetScalars(undoScalars);
        dataSet->Update();

        vtkMAFSmartPointer<vtkStructuredPoints> newDataSet;
        newDataSet->DeepCopy(dataSet);

        m_View->VmeShow(m_RefinementVolumeMask, false);
        m_RefinementVolumeMask->SetData(newDataSet, m_Volume->GetTimeStamp());
        
        m_View->VmeShow(m_RefinementVolumeMask, true);
        SetTrilinearInterpolation(m_RefinementVolumeMask);
        

        vtkDEL(m_RefinementUndoList[numOfChanges-1]);
        m_RefinementUndoList.pop_back();

        m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_UNDO, m_RefinementUndoList.size()>0);
        m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_REDO, m_RefinementRedoList.size()>0);

        m_View->CameraUpdate();
      }
      break;
    }

  case ID_REFINEMENT_REDO:
    {
      int numOfChanges = m_RefinementRedoList.size();
      if(numOfChanges)
      {
        vtkDataSet *dataSet = m_RefinementVolumeMask->GetOutput()->GetVTKData();

        vtkUnsignedCharArray *undoScalars = vtkUnsignedCharArray::New();
        undoScalars->DeepCopy( dataSet->GetPointData()->GetScalars() );
        undoScalars->SetName("SCALARS");
        m_RefinementUndoList.push_back(undoScalars);

        vtkDataArray *redoScalars = m_RefinementRedoList[numOfChanges-1];

        dataSet->GetPointData()->SetScalars(redoScalars);
        dataSet->Update();

        vtkMAFSmartPointer<vtkStructuredPoints> newDataSet;
        newDataSet->DeepCopy(dataSet);

        m_View->VmeShow(m_RefinementVolumeMask, false);
        m_RefinementVolumeMask->SetData(newDataSet, m_Volume->GetTimeStamp());
        m_View->VmeShow(m_RefinementVolumeMask, true);
        SetTrilinearInterpolation(m_RefinementVolumeMask);

        vtkDEL(m_RefinementRedoList[numOfChanges-1]);
        m_RefinementRedoList.pop_back();

        m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_UNDO, m_RefinementUndoList.size()>0);
        m_SegmentationOperationsGui[REFINEMENT_SEGMENTATION]->Enable(ID_REFINEMENT_REDO, m_RefinementRedoList.size()>0);

        m_View->CameraUpdate();
      }
      break;
    }
  case ID_REFINEMENT_APPLY:
    {
      vtkUnsignedCharArray *scalars = vtkUnsignedCharArray::New();
      scalars->DeepCopy( m_RefinementVolumeMask->GetOutput()->GetVTKData()->GetPointData()->GetScalars() );
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

  // if spacing are different get the target spacing

  double diffSpacingXY = abs(m_VolumeSpacing[0] - m_VolumeSpacing[1]);
  double diffSpacingXZ = abs(m_VolumeSpacing[0] - m_VolumeSpacing[2]);
  double diffSpacingYZ = abs(m_VolumeSpacing[1] - m_VolumeSpacing[2]);

  double minDiffSpacing = min(min(diffSpacingXY,diffSpacingXZ),diffSpacingYZ);

  double targetSpacing = max(max(m_VolumeSpacing[0],m_VolumeSpacing[1]),m_VolumeSpacing[2]);


  vtkDataSet *dataset = ((mafVME *)m_ManualVolumeSlice)->GetOutput()->GetVTKData();
  if(!dataset)
    return;
  
  double center[3]={x,y,z};

  int abscissa = 0;
  int ordinate = 1;
  int unused = 2;

  double factors[2] = {1,m_VolumeDimensions[0]};
  switch (m_CurrentSlicePlane)
  {
// 
//     case XY:
//       {
// 
//       }
//       break;
    case XZ:
      {
        abscissa = 0;
        ordinate = 2;
        unused = 1;
        // y dimension = 2
        factors[0] = 1;
        factors[1] = m_VolumeDimensions[0] * 2;
      }
      break;
    case YZ:
      {
        abscissa = 1;
        ordinate = 2;
        unused = 0;
        // x dimension = 2
        factors[0] = 2;
        factors[1] = m_VolumeDimensions[1] * 2;
      }
      break;
  }

  double volumeDimensions[3];

  volumeDimensions[abscissa] =  m_VolumeDimensions[abscissa];
  volumeDimensions[ordinate] = m_VolumeDimensions[ordinate];
  volumeDimensions[unused] = 2;
   
  double numberOfPoints = m_VolumeDimensions[abscissa] * m_VolumeDimensions[ordinate] * volumeDimensions[unused];

  unsigned char scalar = 0;
  if(selection)
  {
    scalar = 255;
  }

  double min_distance = MAXDOUBLE;
  int nearestIndex = -1;
  //get the nearest dataset point
  for (int i=0;i<numberOfPoints;i++)
  {
    double xyz[3];
    dataset->GetPoint(i,xyz);
    //double distance = vtkMath::Distance2BetweenPoints(xyz,center);
    if(center[0] >= xyz[0] && center[0] < xyz[0] + m_VolumeSpacing[0] \
        && center[1] >= xyz[1] && center[1] < xyz[1] + m_VolumeSpacing[1] \
        && center[2] >= xyz[2] && center[2] < xyz[2] + m_VolumeSpacing[2] \
        /*&& nearestIndex == -1*/)
    {
      xyz[0] = xyz[0] +  m_VolumeSpacing[0];
      xyz[1] = xyz[1] +  m_VolumeSpacing[1];
      xyz[2] = xyz[2] +  m_VolumeSpacing[2];
      double distance2 = vtkMath::Distance2BetweenPoints(xyz,center);
      if(distance2 < min_distance)
      {
        nearestIndex = i;
        //min_distance = distance2;
      /*break;*/
      }
    }
  }

  assert(nearestIndex != -1);

  int nearestDummyIndex = int(m_ManualBrushSize / 2) * factors[0] + int(m_ManualBrushSize / 2) * factors[1];//int(m_ManualBrushSize / 2) + int(m_ManualBrushSize / 2) * volumeDimensions[0] + int(m_ManualBrushSize / 2) * volumeDimensions[1];
  std::vector<int> dummyIndices;
  if(m_ManualBrushShape == 0) // circle
  {
    double radius = (double((m_ManualBrushSize + 1) / 2.))* targetSpacing;
    double radius2 = pow(radius,2);
    double dummyCenter[3];
    dummyCenter[abscissa] = radius;
    dummyCenter[ordinate] = radius;
    dummyCenter[unused] = 0;
    
    for(int i = 0; i < int(m_ManualBrushSize); i++)
    {
      for(int j = 0; j < int(m_ManualBrushSize); j++) // remove pixel that are not inside the circle
      {
        // get the center of the pixel
        double dummyPixel[3]; // = {i * m_VolumeSpacing[abscissa] + m_VolumeSpacing[abscissa] / 2., j * m_VolumeSpacing[ordinate] + m_VolumeSpacing[ordinate] / 2.,0};
        dummyPixel[abscissa] = i * targetSpacing;
        dummyPixel[ordinate] = j * targetSpacing;
        dummyPixel[unused] = dummyCenter[unused];
        double index = (i * factors[0] + j * factors[1] - nearestDummyIndex);

        if(vtkMath::Distance2BetweenPoints(dummyPixel,dummyCenter) < radius2 || index == 0)
        {
          dummyIndices.push_back(index);
        }
        else
        {
          dummyIndices.push_back(-(numberOfPoints + 1));
        }
      }
    }
  }
  else // square
  {
    for(int i = 0; i < int(m_ManualBrushSize); i++)
    {
      for(int j = 0; j < int(m_ManualBrushSize); j++) // all pixel in the square are on
      {
        dummyIndices.push_back((i * factors[0] + j * factors[1] - nearestDummyIndex));
      }
    }
  }
//   for(int i = 0; i < dummyIndices.size(); i++)
//   {
//     int curIndex = dummyIndices.at(i) + nearestIndex;
//     if(curIndex >= 0 && curIndex < numberOfPoints)
//     {
//       dataset->GetPointData()->GetScalars()->SetTuple1(curIndex, scalar);
//     }
//   }

  int initialLine = int(double(nearestIndex) / factors[1]) - int(m_ManualBrushSize/2);
  for(int i = 0; i < int(m_ManualBrushSize); i++)
  {
    for(int j = 0; j < int(m_ManualBrushSize); j++) // all pixel in the square are on
    {
      int curIndex = dummyIndices.at(i + j * m_ManualBrushSize) + int(nearestIndex);
      int realLine = int(double(curIndex) / factors[1]);
      //mafLogMessage("----> %d  %d || %d  %d || %d || %d  %d",realLine,initialLine + j,initialLine,j, curIndex,i,j);
      if(curIndex >= 0 && curIndex < numberOfPoints && realLine == initialLine + i)
      {
        dataset->GetPointData()->GetScalars()->SetTuple1(curIndex, scalar);
      }
    }
  }

  dataset->GetPointData()->Update();
  dataset->Update();
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
/**
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

  double spacing = max(m_VolumeSpacing[0] , m_VolumeSpacing[1]);

  double ray = ((m_ManualBrushSize/2.) * spacing);

  unsigned char scalar = 0;
  if(selection)
    scalar=255;
  double pointToCheckRadius[3];

  //////////////////////////////////////////////////////////////////////////
  if(m_ManualBrushShape == 0) // circle
  {

    if (m_CurrentSlicePlane == XY)
    {
      for (int i=0;i<nPointXY;i++)
      {
        dataset->GetPoint(i,pointToCheckRadius);

		if (sqrt(vtkMath::Distance2BetweenPoints(centerOfPick,pointToCheckRadius))<=ray)
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

        if (yID == yIDToCheck && sqrt(vtkMath::Distance2BetweenPoints(centerOfPick,pointToCheckRadius))<=ray)
            dataset->GetPointData()->GetScalars()->SetTuple1(i, scalar);
      }
    }
    else if (m_CurrentSlicePlane == YZ)
    {
      for (int i=0;i<dataset->GetNumberOfPoints();i++)
      {
        dataset->GetPoint(i,pointToCheckRadius);

        int xIDToCheck = round((pointToCheckRadius[0] - origin[0])/m_VolumeSpacing[0]);

        if (xID == xIDToCheck && sqrt(vtkMath::Distance2BetweenPoints(centerOfPick,pointToCheckRadius))<=ray)
            dataset->GetPointData()->GetScalars()->SetTuple1(i, scalar);
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

        if (centerOfPick[0]-ray <= pointToCheckRadius[0] && centerOfPick[0]+ray >= pointToCheckRadius[0] &&
            centerOfPick[1]-ray <= pointToCheckRadius[1] && centerOfPick[1]+ray >= pointToCheckRadius[1])
          dataset->GetPointData()->GetScalars()->SetTuple1(i, scalar);
      }
    }
    else if (m_CurrentSlicePlane == XZ)
    {
      for (int i=0;i<dataset->GetNumberOfPoints();i++)
      {
        dataset->GetPoint(i,pointToCheckRadius);

        int yIDToCheck = round((pointToCheckRadius[1] - origin[1])/m_VolumeSpacing[1]);

        if (yID == yIDToCheck &&
            centerOfPick[0]-ray <= pointToCheckRadius[0] && centerOfPick[0]+ray >= pointToCheckRadius[0] &&
            centerOfPick[2]-ray <= pointToCheckRadius[2] && centerOfPick[2]+ray >= pointToCheckRadius[2])
          dataset->GetPointData()->GetScalars()->SetTuple1(i, scalar);
      }
    }
    else if (m_CurrentSlicePlane == YZ)
    {
      for (int i=0;i<dataset->GetNumberOfPoints();i++)
      {
        dataset->GetPoint(i,pointToCheckRadius);

        int xIDToCheck = round((pointToCheckRadius[0] - origin[0])/m_VolumeSpacing[0]);

        if (xID == xIDToCheck &&
            centerOfPick[1]-ray <= pointToCheckRadius[1] && centerOfPick[1]+ray >= pointToCheckRadius[1] &&
            centerOfPick[2]-ray <= pointToCheckRadius[2] && centerOfPick[2]+ray >= pointToCheckRadius[2])
          dataset->GetPointData()->GetScalars()->SetTuple1(i, scalar);
      }
    }
  }

  dataset->GetPointData()->Update();
  dataset->Update();
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
*/
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

  mafNEW(m_SegmentationPicker);

  m_SegmentationPicker->SetRenderer(m_View->GetFrontRenderer());
  m_SegmentationPicker->SetListener(this);

  m_View->GetRWI()->SetMouse(m_DialogMouse);
  m_View->SetMouse(m_DialogMouse);
  m_OldBehavior=m_Volume->GetBehavior();
  m_DialogMouse->SetView(m_View);

  m_Volume->SetBehavior(m_SegmentationPicker);

  m_SER->AddAction("pntEditingAction");
  pntAction = m_SER->GetAction("pntEditingAction");
  m_ManualPER->AddObserver(m_SegmentationPicker);

//   m_View->GetRWI()->SetMouse(m_DialogMouse);
//   m_View->SetMouse(m_DialogMouse);
//   m_OldBehavior=m_Volume->GetBehavior();
//   m_DialogMouse->SetView(m_View);
  
  m_SER->AddAction("pntEditingAction");
  pntAction = m_SER->GetAction("pntEditingAction");


  
  mafNEW(m_AutomaticPER);
  pntAction = m_SER->AddAction("pntActionAutomatic",-10);
  pntAction->BindDevice(m_DialogMouse);
  pntAction->BindInteractor(m_AutomaticPER);
  m_AutomaticPER->SetListener(this);
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
    text<<m_VolumeDimensions[2];
  else if (m_CurrentSlicePlane == XZ)
    text<<m_VolumeDimensions[1];
  else if (m_CurrentSlicePlane == YZ)
    text<<m_VolumeDimensions[0];
  
  m_AutomaticSliceTextMapper->SetInput(text.GetCStr());
  //////////////////////////////////////////////////////////////////////////
}

//------------------------------------------------------------------------
void medOpSegmentation::UpdateThresholdLabel()
//------------------------------------------------------------------------
{
  if (m_CurrentOperation == AUTOMATIC_SEGMENTATION && (m_CurrentSlicePlane == XY || m_AutomaticGlobalThreshold == GLOBAL) )
  {
    if (m_AutomaticGlobalThreshold == RANGE)
    {
      //Try to find the threshold of the visualized slice
      for (int i=0;i<m_AutomaticRanges.size();i++)
        if (m_AutomaticRanges[i].m_StartSlice<=m_CurrentSliceIndex-1 && m_AutomaticRanges[i].m_EndSlice>=m_CurrentSliceIndex-1)
        {
          mafString text = wxString::Format("Threshold low:%.3f high:%.3f",m_AutomaticRanges[i].m_ThresholdValue,m_AutomaticRanges[i].m_UpperThresholdValue);
          m_AutomaticThresholdTextMapper->SetInput(text.GetCStr());
          return;
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


//----------------------------------------------------------------------------
bool medOpSegmentation::CheckNumberOfThresholds()
//----------------------------------------------------------------------------
{
  //////////////////////////////////////////////////////////////////////////
  //Check that all slices have a threshold
  for (int i=0;i<m_VolumeDimensions[2];i++)//only XY planes
    for (int j=0;j<m_AutomaticRanges.size();j++)
      if (i>=(m_AutomaticRanges[j].m_StartSlice) && i<=(m_AutomaticRanges[j].m_EndSlice))
      {
        mafLogMessage("Slice %d° hasn't a threshold",i+1);
        return false;
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
      sp->GetDimensions(m_VolumeDimensions); 
    else if (vtkRectilinearGrid *rg = vtkRectilinearGrid::SafeDownCast(inputDataSet))
      rg->GetDimensions(m_VolumeDimensions); 

    m_VolumeParametersInitialized = true;

    if (m_SliceSlider)
    {
      if (m_CurrentSlicePlane == XY)
        m_SliceSlider->SetRange(1,m_VolumeDimensions[2]);
      else if (m_CurrentSlicePlane == XZ)
        m_SliceSlider->SetRange(1,m_VolumeDimensions[1]);
      else if (m_CurrentSlicePlane == YZ)
        m_SliceSlider->SetRange(1,m_VolumeDimensions[0]);
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
void medOpSegmentation::GetSliceOrigin(double *origin)
//----------------------------------------------------------------------------
{
  if (m_Volume->GetOutput()->GetVTKData()->IsA("vtkStructuredPoints"))
  {
    vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(m_Volume->GetOutput()->GetVTKData());
    sp->Update();
    double spc[3];
    sp->GetSpacing(spc);
    sp->GetOrigin(origin);

    if (m_CurrentSlicePlane == XY)
      origin[2] = (m_CurrentSliceIndex-1)*spc[2]+origin[2];
    else if (m_CurrentSlicePlane == XZ)
      origin[1] = (m_CurrentSliceIndex-1)*spc[1]+origin[1];
    else if (m_CurrentSlicePlane == YZ)
      origin[0] = (m_CurrentSliceIndex-1)*spc[0]+origin[0];

  }
  else
  {
    vtkRectilinearGrid *rg = vtkRectilinearGrid::SafeDownCast(m_Volume->GetOutput()->GetVTKData());
    rg->Update();
    origin[0] = rg->GetXCoordinates()->GetTuple1(0);
    origin[1] = rg->GetYCoordinates()->GetTuple1(0);
    origin[2] = rg->GetZCoordinates()->GetTuple1(0);

    if (m_CurrentSlicePlane == XY)
      origin[2] = rg->GetZCoordinates()->GetTuple1(m_CurrentSliceIndex-1);
    else if (m_CurrentSlicePlane == XZ)
      origin[1] = rg->GetYCoordinates()->GetTuple1(m_CurrentSliceIndex-1);
    if (m_CurrentSlicePlane == YZ)
      origin[0] = rg->GetXCoordinates()->GetTuple1(m_CurrentSliceIndex-1);
  }
}

//----------------------------------------------------------------------------
void medOpSegmentation::UpdateSlice()
//----------------------------------------------------------------------------
{
  m_OldSliceIndex = m_CurrentSliceIndex;
  m_OldSlicePlane = m_CurrentSlicePlane;

  UpdateThresholdLabel();
  UpdateSliceLabel();

  
  double posNew[3];
  
  GetSliceOrigin(posNew);

  m_View->SetSlice( posNew );

  if(m_CurrentOperation == MANUAL_SEGMENTATION)
    UpdateVolumeSlice();

  m_View->CameraUpdate();
  m_GuiDialog->Update();
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
    //m_View->VmeCreatePipe(m_ManualVolumeSlice);
    InitDataVolumeSlice<vtkUnsignedCharArray>(m_ManualVolumeSlice);

    m_View->CameraUpdate();
  }

}

//----------------------------------------------------------------------------
void medOpSegmentation::InitEmptyVolumeSlice()
//----------------------------------------------------------------------------
{
  if(m_EmptyVolumeSlice != NULL)
  {
    m_EmptyVolumeSlice->ReparentTo(m_Volume->GetParent());
    m_EmptyVolumeSlice->SetName("Threshold Volume Slice");
    //m_View->VmeAdd(m_EmptyVolumeSlice);
    //m_View->VmeCreatePipe(m_EmptyVolumeSlice);
    //m_View->CameraUpdate();

    InitDataVolumeSlice<vtkDoubleArray>(m_EmptyVolumeSlice);
  }
}

//----------------------------------------------------------------------------
template <class ImageType>
void medOpSegmentation::InitDataVolumeSlice(mafVMEVolumeGray *slice)
//----------------------------------------------------------------------------
{
  if(!slice || !m_Volume || !m_Volume->GetOutput()->GetVTKData())
    return;

//   m_OldSliceIndex = m_CurrentSliceIndex;
//   m_OldSlicePlane = m_CurrentSlicePlane;


  vtkDataSet *inputData = NULL;

  inputData = m_Volume->GetOutput()->GetVTKData();
  inputData->Update();

  vtkDataArray *inputScalars = inputData->GetPointData()->GetScalars();

  vtkMAFSmartPointer<ImageType> scalars;
  scalars->SetName("SCALARS");

  double origin[3]; 

  vtkDataSet *ds = m_Volume->GetOutput()->GetVTKData();

  GetSliceOrigin(origin);

  // Single slice volumes crashes on vmeshow
  int numberOfSlices = 2;
  int numberOfPoints;

  ///////////////////////////////////////////////////////////////////
  //Setting up Scalars this is the same both for structured point
  //and for rectilinear grid
  if(m_CurrentSlicePlane == XY)
  {
    numberOfPoints = m_VolumeDimensions[0]*m_VolumeDimensions[1];
    scalars->SetNumberOfTuples(numberOfSlices*numberOfPoints);
    int z=(m_CurrentSliceIndex-1);
    for (int x=0; x<m_VolumeDimensions[0]; x++)
      for (int y=0; y<m_VolumeDimensions[1]; y++)
      { 
        int volPos=x + y*m_VolumeDimensions[0] + z*m_VolumeDimensions[0]*m_VolumeDimensions[1];
        int slicePos = x + y*m_VolumeDimensions[0] + 0;
        scalars->SetTuple1(slicePos, inputScalars->GetTuple1(volPos) );
      }
  }

  else if(m_CurrentSlicePlane == YZ)
  {
    numberOfPoints = m_VolumeDimensions[1] * m_VolumeDimensions[2];
    scalars->SetNumberOfTuples(numberOfSlices*numberOfPoints);
    int x=(m_CurrentSliceIndex-1);
    for (int y=0; y<m_VolumeDimensions[1]; y++)
      for (int z=0; z<m_VolumeDimensions[2]; z++)
      { 
        int volPos=x + y*m_VolumeDimensions[0] + z*m_VolumeDimensions[0]*m_VolumeDimensions[1];
        int slicePos=0 + y*numberOfSlices + z*numberOfSlices*m_VolumeDimensions[1];
        scalars->SetTuple1(slicePos, inputScalars->GetTuple1(volPos) );
      }
  }

  else if(m_CurrentSlicePlane == XZ)
  {
    numberOfPoints = m_VolumeDimensions[0] * m_VolumeDimensions[2];
    scalars->SetNumberOfTuples(numberOfSlices*numberOfPoints);
    int y=(m_CurrentSliceIndex-1);
    for (int x=0; x<m_VolumeDimensions[0]; x++)
      for (int z=0; z<m_VolumeDimensions[2]; z++)
      { 
        int volPos=x + y*m_VolumeDimensions[0] + z*m_VolumeDimensions[0]*m_VolumeDimensions[1];
        int slicePos=x + 0 + z*m_VolumeDimensions[0]*numberOfSlices;
        scalars->SetTuple1(slicePos, inputScalars->GetTuple1(volPos) );
      }
  }


  //////////////////////////////////////////////////////////////////////////
  // spacing regolare: imagedata
  //////////////////////////////////////////////////////////////////////////
  if(ds->IsA("vtkStructuredPoints"))
  {
    vtkDataSet *imageData = slice->GetOutput()->GetVTKData();
    vtkMAFSmartPointer<vtkStructuredPoints> newImageData;

    if(m_CurrentSlicePlane == XY)
      newImageData->SetDimensions(m_VolumeDimensions[0],m_VolumeDimensions[1],numberOfSlices);
    else if(m_CurrentSlicePlane == YZ)
      newImageData->SetDimensions(numberOfSlices,m_VolumeDimensions[1],m_VolumeDimensions[2]);
    else if(m_CurrentSlicePlane == XZ)
      newImageData->SetDimensions(m_VolumeDimensions[0],numberOfSlices,m_VolumeDimensions[2]);

    newImageData->SetSpacing(m_VolumeSpacing[0],m_VolumeSpacing[1],m_VolumeSpacing[2]);
    newImageData->SetScalarTypeToUnsignedChar();
    newImageData->SetOrigin(origin[0], origin[1], origin[2]);
    newImageData->GetPointData()->AddArray(scalars);
    newImageData->GetPointData()->SetActiveScalars("SCALARS");
    newImageData->Update();
    slice->SetData(newImageData,0.0);
  }

  //////////////////////////////////////////////////////////////////////////
  // spacing non regolare: rectilineargrid
  //////////////////////////////////////////////////////////////////////////
  else
  {

    vtkDataSet *rgData = slice->GetOutput()->GetVTKData();

    vtkMAFSmartPointer<vtkRectilinearGrid> newRgData;

    vtkDataArray *x = vtkRectilinearGrid::SafeDownCast(inputData)->GetXCoordinates();
    vtkDataArray *y = vtkRectilinearGrid::SafeDownCast(inputData)->GetYCoordinates();
    vtkDataArray *z = vtkRectilinearGrid::SafeDownCast(inputData)->GetZCoordinates();


    if(m_CurrentSlicePlane == XY)
    {
      z = vtkDoubleArray::New();
      z->SetNumberOfComponents(1);
      z->SetNumberOfTuples(2);
      z->SetTuple1(0, origin[2]);
      z->SetTuple1(1, origin[2] + 1.0);
      newRgData->SetDimensions(m_VolumeDimensions[0],m_VolumeDimensions[1],numberOfSlices);
    }
    else if(m_CurrentSlicePlane == YZ)
    {
      x = vtkDoubleArray::New();
      x->SetNumberOfComponents(1);
      x->SetNumberOfTuples(2);
      x->SetTuple1(0, origin[0]);
      x->SetTuple1(1, origin[0] + 1.0);
      newRgData->SetDimensions(numberOfSlices,m_VolumeDimensions[1],m_VolumeDimensions[2]);
    }
    else if(m_CurrentSlicePlane == XZ)
    {
      y = vtkDoubleArray::New();
      y->SetNumberOfComponents(1);
      y->SetNumberOfTuples(2);
      y->SetTuple1(0, origin[1]);
      y->SetTuple1(1, origin[1] + 1.0);
      newRgData->SetDimensions(m_VolumeDimensions[0],numberOfSlices,m_VolumeDimensions[2]);
    }
    newRgData->SetXCoordinates(x);
    newRgData->SetYCoordinates(y);
    newRgData->SetZCoordinates(z);
    newRgData->GetPointData()->AddArray(scalars);
    newRgData->GetPointData()->SetActiveScalars("SCALARS");
    newRgData->Update();
    slice->SetData(newRgData, 0.0);
    if (m_CurrentSlicePlane == YZ) vtkDEL(x);
    else if (m_CurrentSlicePlane == XZ) vtkDEL(y);
    else if (m_CurrentSlicePlane == XY) vtkDEL(z);
  }
}

//----------------------------------------------------------------------------
void medOpSegmentation::InitThresholdVolumeSlice()
//----------------------------------------------------------------------------
{
  if(m_ThresholdVolumeSlice != NULL)
  {
    m_ThresholdVolumeSlice->ReparentTo(m_Volume->GetParent());
    m_ThresholdVolumeSlice->SetName("Threshold Volume Slice");

    InitDataVolumeSlice<vtkUnsignedCharArray>(m_ThresholdVolumeSlice);

    m_SegmentationColorLUT = m_ThresholdVolumeSlice->GetMaterial()->m_ColorLut;
    InitMaskColorLut(m_SegmentationColorLUT);
    m_ThresholdVolumeSlice->GetMaterial()->UpdateFromTables();
    m_ThresholdVolumeSlice->Update();

    m_View->VmeAdd(m_ThresholdVolumeSlice);
    m_View->VmeShow(m_ThresholdVolumeSlice,false);
    m_OldSliceIndex = -1;
    UpdateThresholdRealTimePreview();
    SetTrilinearInterpolation(m_ThresholdVolumeSlice);
    m_View->CameraUpdate();
  }
}

//----------------------------------------------------------------------------
void medOpSegmentation::UpdateVolumeSlice()
//----------------------------------------------------------------------------
{
  if(!m_ManualVolumeSlice || !m_ManualVolumeMask || !m_ManualVolumeMask->GetOutput()->GetVTKData())
    return;

  m_OldSliceIndex = m_CurrentSliceIndex;
  m_OldSlicePlane = m_CurrentSlicePlane;

  
  vtkDataSet *inputData = NULL;
  
  inputData = m_ManualVolumeMask->GetOutput()->GetVTKData();
  inputData->Update();

  vtkDataArray *inputScalars = inputData->GetPointData()->GetScalars();

  vtkMAFSmartPointer<vtkUnsignedCharArray> scalars;
  scalars->SetName("SCALARS");

  double origin[3]; 

  vtkDataSet *ds = m_Volume->GetOutput()->GetVTKData();
  
  GetSliceOrigin(origin);
  
  // Single slice volumes crashes on vmeshow
  int numberOfSlices = 2;
  int numberOfPoints;

  ///////////////////////////////////////////////////////////////////
  //Setting up Scalars this is the same both for structured point
  //and for rectilinear grid
  if(m_CurrentSlicePlane == XY)
  {
    numberOfPoints = m_VolumeDimensions[0]*m_VolumeDimensions[1];
    scalars->SetNumberOfTuples(numberOfSlices*numberOfPoints);
    int z=(m_CurrentSliceIndex-1);
    for (int x=0; x<m_VolumeDimensions[0]; x++)
      for (int y=0; y<m_VolumeDimensions[1]; y++)
      { 
        int volPos=x + y*m_VolumeDimensions[0] + z*m_VolumeDimensions[0]*m_VolumeDimensions[1];
        int slicePos = x + y*m_VolumeDimensions[0] + 0;
        scalars->SetTuple1(slicePos, inputScalars->GetTuple1(volPos) );
      }
  }

  else if(m_CurrentSlicePlane == YZ)
  {
    numberOfPoints = m_VolumeDimensions[1] * m_VolumeDimensions[2];
    scalars->SetNumberOfTuples(numberOfSlices*numberOfPoints);
    int x=(m_CurrentSliceIndex-1);
    for (int y=0; y<m_VolumeDimensions[1]; y++)
      for (int z=0; z<m_VolumeDimensions[2]; z++)
      { 
        int volPos=x + y*m_VolumeDimensions[0] + z*m_VolumeDimensions[0]*m_VolumeDimensions[1];
        int slicePos=0 + y*numberOfSlices + z*numberOfSlices*m_VolumeDimensions[1];
        scalars->SetTuple1(slicePos, inputScalars->GetTuple1(volPos) );
      }
  }

  else if(m_CurrentSlicePlane == XZ)
  {
    numberOfPoints = m_VolumeDimensions[0] * m_VolumeDimensions[2];
    scalars->SetNumberOfTuples(numberOfSlices*numberOfPoints);
    int y=(m_CurrentSliceIndex-1);
    for (int x=0; x<m_VolumeDimensions[0]; x++)
      for (int z=0; z<m_VolumeDimensions[2]; z++)
      { 
        int volPos=x + y*m_VolumeDimensions[0] + z*m_VolumeDimensions[0]*m_VolumeDimensions[1];
        int slicePos=x + 0 + z*m_VolumeDimensions[0]*numberOfSlices;
        scalars->SetTuple1(slicePos, inputScalars->GetTuple1(volPos) );
      }
  }


  //////////////////////////////////////////////////////////////////////////
  // spacing regolare: imagedata
  //////////////////////////////////////////////////////////////////////////
  if(ds->IsA("vtkStructuredPoints"))
  {
    vtkDataSet *imageData = m_ManualVolumeSlice->GetOutput()->GetVTKData();
    vtkMAFSmartPointer<vtkStructuredPoints> newImageData;
  
    if(m_CurrentSlicePlane == XY)
       newImageData->SetDimensions(m_VolumeDimensions[0],m_VolumeDimensions[1],numberOfSlices);
    else if(m_CurrentSlicePlane == YZ)
      newImageData->SetDimensions(numberOfSlices,m_VolumeDimensions[1],m_VolumeDimensions[2]);
    else if(m_CurrentSlicePlane == XZ)
      newImageData->SetDimensions(m_VolumeDimensions[0],numberOfSlices,m_VolumeDimensions[2]);

    newImageData->SetSpacing(m_VolumeSpacing[0],m_VolumeSpacing[1],m_VolumeSpacing[2]);
    newImageData->SetScalarTypeToUnsignedChar();
    newImageData->SetOrigin(origin[0], origin[1], origin[2]);
    newImageData->GetPointData()->AddArray(scalars);
    newImageData->GetPointData()->SetActiveScalars("SCALARS");
    newImageData->Update();
    m_ManualVolumeSlice->SetData(newImageData,0.0);
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
      z = vtkDoubleArray::New();
      z->SetNumberOfComponents(1);
      z->SetNumberOfTuples(2);
      z->SetTuple1(0, origin[2]);
      z->SetTuple1(1, origin[2] + 1.0);
      newRgData->SetDimensions(m_VolumeDimensions[0],m_VolumeDimensions[1],numberOfSlices);
    }
    else if(m_CurrentSlicePlane == YZ)
    {
      x = vtkDoubleArray::New();
      x->SetNumberOfComponents(1);
      x->SetNumberOfTuples(2);
      x->SetTuple1(0, origin[0]);
      x->SetTuple1(1, origin[0] + 1.0);
      newRgData->SetDimensions(numberOfSlices,m_VolumeDimensions[1],m_VolumeDimensions[2]);
    }
    else if(m_CurrentSlicePlane == XZ)
    {
      y = vtkDoubleArray::New();
      y->SetNumberOfComponents(1);
      y->SetNumberOfTuples(2);
      y->SetTuple1(0, origin[1]);
      y->SetTuple1(1, origin[1] + 1.0);
      newRgData->SetDimensions(m_VolumeDimensions[0],numberOfSlices,m_VolumeDimensions[2]);
    }
    newRgData->SetXCoordinates(x);
    newRgData->SetYCoordinates(y);
    newRgData->SetZCoordinates(z);
    newRgData->GetPointData()->AddArray(scalars);
    newRgData->GetPointData()->SetActiveScalars("SCALARS");
    newRgData->Update();
    m_ManualVolumeSlice->SetData(newRgData, 0.0);
    if (m_CurrentSlicePlane == YZ) vtkDEL(x);
    else if (m_CurrentSlicePlane == XZ) vtkDEL(y);
    else if (m_CurrentSlicePlane == XY) vtkDEL(z);
  }
  m_ManualColorLUT = m_ManualVolumeSlice->GetMaterial()->m_ColorLut;
  InitMaskColorLut(m_ManualColorLUT);
  m_ManualVolumeSlice->GetMaterial()->UpdateFromTables();
  m_ManualVolumeSlice->Update();

}

//----------------------------------------------------------------------------
void medOpSegmentation::ApplyVolumeSliceChanges()
//----------------------------------------------------------------------------
{
  vtkDataSet *inputDataSet = vtkDataSet::SafeDownCast(m_ManualVolumeMask->GetOutput()->GetVTKData());
  vtkDataSet *sliceDataSet = vtkDataSet::SafeDownCast(m_ManualVolumeSlice->GetOutput()->GetVTKData());
  
  if (inputDataSet && sliceDataSet)
  {
    vtkDataArray *inputScalars = inputDataSet->GetPointData()->GetScalars();
    vtkDataArray *scalars = sliceDataSet->GetPointData()->GetScalars();

    int numberOfPoints;
    int numberOfSlices=2;
    
    if(m_OldSlicePlane == XY)
    {
      numberOfPoints = m_VolumeDimensions[0]*m_VolumeDimensions[1];
      int z=(m_OldSliceIndex-1);
      for (int x=0; x<m_VolumeDimensions[0]; x++)
        for (int y=0; y<m_VolumeDimensions[1]; y++)
        { 
          int volPos=x + y*m_VolumeDimensions[0] + z*m_VolumeDimensions[0]*m_VolumeDimensions[1];
          int slicePos = x + y*m_VolumeDimensions[0] + 0;
          inputScalars->SetTuple1(volPos, scalars->GetTuple1(slicePos) );
        }
    }

    else if(m_OldSlicePlane == YZ)
    {
      numberOfPoints = m_VolumeDimensions[1] * m_VolumeDimensions[2];
      int x=(m_OldSliceIndex-1);
      for (int y=0; y<m_VolumeDimensions[1]; y++)
        for (int z=0; z<m_VolumeDimensions[2]; z++)
        { 
          int volPos=x + y*m_VolumeDimensions[0] + z*m_VolumeDimensions[0]*m_VolumeDimensions[1];
          int slicePos=0 + y*numberOfSlices + z*numberOfSlices*m_VolumeDimensions[1];
          inputScalars->SetTuple1(volPos, scalars->GetTuple1(slicePos) );
        }
    }

    else if(m_OldSlicePlane == XZ)
    {
      numberOfPoints = m_VolumeDimensions[0] * m_VolumeDimensions[2];
      int y=(m_OldSliceIndex-1);
      for (int x=0; x<m_VolumeDimensions[0]; x++)
        for (int z=0; z<m_VolumeDimensions[2]; z++)
        { 
          int volPos=x + y*m_VolumeDimensions[0] + z*m_VolumeDimensions[0]*m_VolumeDimensions[1];
          int slicePos=x + 0 + z*m_VolumeDimensions[0]*numberOfSlices;
          inputScalars->SetTuple1(volPos, scalars->GetTuple1(slicePos) );
        }
    }
    
    m_ManualVolumeMask->Update();
  }

}

//----------------------------------------------------------------------------
void medOpSegmentation::ResetManualRedoList()
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_ManualRedoList.size();i++)
    vtkDEL(m_ManualRedoList[i].dataArray);
  m_ManualRedoList.clear();
}

//----------------------------------------------------------------------------
void medOpSegmentation::ResetManualUndoList()
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_ManualUndoList.size();i++)
    vtkDEL(m_ManualUndoList[i].dataArray);
  m_ManualUndoList.clear();
}

//----------------------------------------------------------------------------
void medOpSegmentation::ResetRefinementRedoList()
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_RefinementRedoList.size();i++)
    vtkDEL(m_RefinementRedoList[i]);
  m_RefinementRedoList.clear();
}

//----------------------------------------------------------------------------
void medOpSegmentation::ResetRefinementUndoList()
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_RefinementUndoList.size();i++)
    vtkDEL(m_RefinementUndoList[i]);
  m_RefinementUndoList.clear();
}


//----------------------------------------------------------------------------
void medOpSegmentation::InitMaskColorLut(vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
  if(lut)
  {
    lut->SetNumberOfTableValues(2);
    lut->SetTableValue(0, 0.0, 0.0, 0.0, 0.0);
    lut->SetTableValue(1, 0.9, 0.1, 0.1, 0.4);
  }
}

//----------------------------------------------------------------------------
void medOpSegmentation::SetTrilinearInterpolation(mafVMEVolumeGray *volume)
//----------------------------------------------------------------------------
{
  if(volume)
  {
    mafPipeVolumeSlice_BES *pipe = mafPipeVolumeSlice_BES::SafeDownCast(m_View->GetNodePipe(volume));
    if(pipe)
    {
      pipe->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
    }
  }
}

//----------------------------------------------------------------------------
void medOpSegmentation::GetCameraAttribute(double *focalPoint, double *scaleFactor)
//----------------------------------------------------------------------------
{
  *scaleFactor = m_View->m_Rwi->m_RenFront->GetActiveCamera()->GetParallelScale();
  m_View->m_Rwi->m_RenFront->GetActiveCamera()->GetFocalPoint(focalPoint);
}
//----------------------------------------------------------------------------
void medOpSegmentation::GetVisualizedBounds(double focalPoint[3], double scaleFactor, double bounds[4])
//----------------------------------------------------------------------------
{
  double visualizedRatio = scaleFactor / m_InitialScaleFactor;
  double xTranslation = focalPoint[0] - m_InitialFocalPoint[0];
  double yTranslation = focalPoint[1] - m_InitialFocalPoint[1];

  double b[6];
  m_Volume->GetOutput()->GetVTKData()->GetBounds(b);
  
  double xSize = b[1] - b[0];
  double ySize = b[3] - b[2];

  double size = max(xSize,ySize);

  double newSize = size * visualizedRatio;

  bounds[0] = focalPoint[0] - newSize/2;
  bounds[1] = focalPoint[0] + newSize/2;

  bounds[2] = focalPoint[1] - newSize/2;
  bounds[3] = focalPoint[1] + newSize/2;

}
//----------------------------------------------------------------------------
bool medOpSegmentation::ResetZoom(vtkDataSet* dataset, double visbleBounds[4])
//----------------------------------------------------------------------------
{
  for(int i = 0; i < dataset->GetNumberOfPoints(); i++)
  {
    if((unsigned char)(dataset->GetPointData()->GetScalars()->GetTuple1(i)) == 255)
    {
      double x[3];
      dataset->GetPoint(i,x);
      if(x[0] > visbleBounds[0] && x[0] < visbleBounds[1] && x[1] > visbleBounds[2] && x[1] < visbleBounds[3])
      {
        return false;
      }
    }
  }  
  return true;
}

//----------------------------------------------------------------------------
void medOpSegmentation::CreateRealDrawnImage()
//----------------------------------------------------------------------------
{
  m_ManualVolumeSlice->GetOutput()->GetVTKData()->Update();
  if(!m_RealDrawnImage)
  {
    m_RealDrawnImage = vtkUnsignedCharArray::New();
  }
  m_RealDrawnImage->DeepCopy( m_ManualVolumeSlice->GetOutput()->GetVTKData()->GetPointData()->GetScalars() );
  m_RealDrawnImage->SetName("SCALARS");
}

//----------------------------------------------------------------------------
void medOpSegmentation::UndoBrushPreview()
//----------------------------------------------------------------------------
{
  if(m_RealDrawnImage)
  {
    vtkImageData* dataSet = (vtkImageData*) m_ManualVolumeSlice->GetOutput()->GetVTKData();
    dataSet->GetPointData()->SetScalars(m_RealDrawnImage);
    dataSet->Update();
    m_ManualVolumeSlice->SetData(dataSet,mafVME::SafeDownCast(m_Volume)->GetTimeStamp());
    m_ManualVolumeSlice->Update();
    ApplyVolumeSliceChanges();
  }
}
//----------------------------------------------------------------------------
void medOpSegmentation::UpdateThresholdRealTimePreview()
//----------------------------------------------------------------------------
{
  if(m_OldAutomaticThreshold == m_AutomaticThreshold && m_OldAutomaticUpperThreshold == m_AutomaticUpperThreshold && m_OldSliceIndex == m_CurrentSliceIndex && m_OldSlicePlane != m_CurrentSlicePlane)
  {
    return;
  }
  m_OldSlicePlane = m_CurrentSlicePlane;
  m_OldAutomaticThreshold = m_AutomaticThreshold;
  m_OldAutomaticUpperThreshold = m_AutomaticUpperThreshold;
  
  medVMESegmentationVolume *tVol;
  mafNEW(tVol);

  tVol->SetVolumeLink(m_EmptyVolumeSlice);
  tVol->SetName("Threshold Volume");
  tVol->ReparentTo(tVol->GetParent());
  tVol->SetDoubleThresholdModality(true);
  tVol->Update();


  tVol->RemoveAllRanges();
  if (m_AutomaticGlobalThreshold == RANGE)
  {
    int result;
    tVol->SetAutomaticSegmentationThresholdModality(medVMESegmentationVolume::RANGE);
    for (int i=0;i<m_AutomaticRanges.size();i++)
    {
      if(m_CurrentSliceIndex >= m_AutomaticRanges[i].m_StartSlice && m_CurrentSliceIndex <= m_AutomaticRanges[i].m_EndSlice)
        result = tVol->AddRange(0,1,m_AutomaticRanges[i].m_ThresholdValue,m_AutomaticRanges[i].m_UpperThresholdValue);
      
//       if (result == MAF_ERROR)
//       {
//         return;
//       }
    }
    if(tVol->GetNumberOfRanges() == 0)
      result = tVol->AddRange(0,1,m_AutomaticThreshold,m_AutomaticUpperThreshold);
  }
  else
  {
    tVol->SetAutomaticSegmentationThresholdModality(medVMESegmentationVolume::GLOBAL);
    tVol->SetAutomaticSegmentationGlobalThreshold(m_AutomaticThreshold,m_AutomaticUpperThreshold);
  }

  tVol->GetOutput()->Update();
  tVol->Update();

  if (vtkStructuredPoints::SafeDownCast(tVol->GetOutput()->GetVTKData()))
  {
    vtkStructuredPoints *newData = vtkStructuredPoints::SafeDownCast(tVol->GetAutomaticOutput());
    m_ThresholdVolumeSlice->SetData(newData,mafVME::SafeDownCast(m_Volume)->GetTimeStamp());
    vtkStructuredPoints *spVME = vtkStructuredPoints::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_ThresholdVolumeSlice)->GetOutput()->GetVTKData());
    spVME->Update();

  }
  else if(vtkRectilinearGrid::SafeDownCast(tVol->GetOutput()->GetVTKData()))
  {
    vtkRectilinearGrid *newData = vtkRectilinearGrid::SafeDownCast(tVol->GetAutomaticOutput());
    m_ThresholdVolumeSlice->SetData(newData,mafVME::SafeDownCast(m_Volume)->GetTimeStamp());
    vtkRectilinearGrid *rgVME = vtkRectilinearGrid::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_ThresholdVolumeSlice)->GetOutput()->GetVTKData());
    rgVME->Update();
  }

  m_ThresholdVolumeSlice->Update();

  m_SegmentationColorLUT = m_ThresholdVolumeSlice->GetMaterial()->m_ColorLut;
  InitMaskColorLut(m_SegmentationColorLUT);

  m_ThresholdVolumeSlice->GetMaterial()->m_ColorLut->SetTableRange(0,255);
  mmaVolumeMaterial *currentVolumeMaterial = ((mafVMEOutputVolume *)m_ThresholdVolumeSlice->GetOutput())->GetMaterial();
  currentVolumeMaterial->UpdateFromTables();
  
  m_ThresholdVolumeSlice->Modified();
  m_ThresholdVolumeSlice->Update();
  m_View->VmeShow(m_ThresholdVolumeSlice,true);
  /*SetTrilinearInterpolation(m_ThresholdVolumeSlice);*/
  
  mafDEL(tVol);
}

//----------------------------------------------------------------------------
bool medOpSegmentation::SegmentedVolumeAccept(mafNode* node)
//----------------------------------------------------------------------------
{
  if(node != NULL  \
    && node->IsMAFType(mafVMEVolumeGray) \
    && m_CurrentVolumeParametersInitialized \
    && mafVMEVolumeGray::SafeDownCast(node)->GetOutput() != NULL \
    && mafVMEVolumeGray::SafeDownCast(node)->GetOutput()->GetVTKData() != NULL \
//     && vtkDataSet::SafeDownCast(mafVMEVolumeGray::SafeDownCast(node)->GetOutput()->GetVTKData())->GetPointData()->GetScalars() != NULL
    )
  {
    mafVMEVolumeGray *volumeToCheck = mafVMEVolumeGray::SafeDownCast(node);
    volumeToCheck->Update();
    /* loaded volume must have the same bounds of the input volume */
    double b[6];
    volumeToCheck->GetOutput()->GetBounds(b);
    for(int i = 0; i < 6; i++)
    {
      if(b[i] != m_CurrentVolumeBounds[i])
      {
        return false;
      }
    }
    /* scalar range should be 0 - 255 */
    double sr[2];
    volumeToCheck->GetOutput()->Update();
    volumeToCheck->GetOutput()->GetVTKData()->Update();
    if(vtkDataSet::SafeDownCast(volumeToCheck->GetOutput()->GetVTKData())->GetPointData() != NULL && vtkDataSet::SafeDownCast(volumeToCheck->GetOutput()->GetVTKData())->GetPointData()->GetScalars() != NULL)
    {
      vtkDataSet::SafeDownCast(volumeToCheck->GetOutput()->GetVTKData())->GetPointData()->GetScalars()->GetRange(sr);
      if(sr[0] < 0 || sr[1] > 255) /* scalar range could be 0-0 empty masc or 255-255 full mask */
      {
        return false;
      }
    }
    return true;
  }

  return false;
}