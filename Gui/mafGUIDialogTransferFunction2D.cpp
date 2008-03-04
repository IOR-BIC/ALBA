/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIDialogTransferFunction2D.cpp,v $
  Language:  C++
  Date:      $Date: 2008-03-04 17:04:31 $
  Version:   $Revision: 1.3 $
  Authors:   Alexander Savenko
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

#include "mafGUIDialogTransferFunction2D.h"

#include <wx/event.h>
#include <wx/notebook.h>

#include <fstream>

#include "mafDecl.h"
#include "mmgGui.h"
#include "mmgRangeSlider.h"
#include "mmgButton.h"
#include "mmgPicButton.h"
#include "mmgFloatSlider.h"
#include "mafRWIBase.h"
#include "mafRWI.h"
#include "mmaVolumeMaterial.h"
#include "mmgValidator.h"

#include "mafTagArray.h"
#include "mafTagItem.h"
#include "mafVME.h"
#include "mafVMEOutputVolume.h"

#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkRectilinearGrid.h"
#include "vtkRenderer.h"
#include "vtkLight.h"
#include "vtkCamera.h"
#include "vtkRenderWindow.h"
#include "vtkInteractorStyle.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkTextActor.h"
#include "vtkImageMapper.h"
#include "vtkImageResample.h"
#include "vtkPolyDataMapper.h"
#include "vtkCriticalSection.h"
#include "vtkMultiThreader.h"
#include "vtkMAFImageMapToWidgetColors.h"
#include "vtkMAFAdaptiveVolumeMapper.h"
#include "vtkImageCast.h"

#define min(x0, x1) (((x0) < (x1)) ? (x0) : (x1))
#define max(x0, x1) (((x0) > (x1)) ? (x0) : (x1))
template<typename type> static inline type clip(type x, type xmin, type xmax) { if (x < xmin) return xmin; if (x > xmax) return xmax; return x; }

BEGIN_EVENT_TABLE(mafGUIDialogTransferFunction2D, wxDialog)
  EVT_IDLE(mafGUIDialogTransferFunction2D::OnIdle)
  EVT_MOTION(mafGUIDialogTransferFunction2D::OnMotion)
  EVT_SIZE(mafGUIDialogTransferFunction2D::OnSize)
  EVT_COMMAND_RANGE(MINID, MAXID, wxEVT_COMMAND_LISTBOX_SELECTED, mafGUIDialogTransferFunction2D::OnCommand)
  EVT_COMMAND_RANGE(MINID, MAXID, wxEVT_COMMAND_SLIDER_UPDATED,   mafGUIDialogTransferFunction2D::OnCommand)
  EVT_COMMAND_RANGE(MINID, MAXID, wxEVT_COMMAND_CHOICE_SELECTED,  mafGUIDialogTransferFunction2D::OnCommand)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
// constant :
//----------------------------------------------------------------------------
enum TRANSFER_FUNCTION_DIALOG_ID
{
  ID_SLICE_NUMBER = MINID,
  ID_WINDOWING_SLIDER,
  ID_TF_WIDGET_LIST,
  ID_TF_NAME,
  ID_TF_COLOR,
  ID_TF_OPACITY,
  ID_TF_DIFFUSE,
  ID_TF_VALUE,
  ID_TF_VALUE_0,
  ID_TF_VALUE_1,
  ID_TF_VALUE_2,
  ID_TF_GRADIENT,
  ID_TF_GRADIENT_0,
  ID_TF_GRADIENT_1,
  ID_TF_GRADIENT_2,
  ID_TF_GRADIENT_INTERPOLATION,
  ID_TF_ADD,
  ID_TF_REMOVE,
  ID_TF_LOAD,
  ID_TF_SAVE,
};

//----------------------------------------------------------------------------
mafGUIDialogTransferFunction2D::mafGUIDialogTransferFunction2D() :
  mmgDialog("Choose Transfer Function", wxDIALOG_MODAL | wxCAPTION | wxTHICK_FRAME )
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
mafGUIDialogTransferFunction2D::~mafGUIDialogTransferFunction2D() 
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafGUIDialogTransferFunction2D::ShowModal(mafVME *vme)
//----------------------------------------------------------------------------
{
  this->m_Vme = vme;
  mafVMEOutputVolume *out_data = mafVMEOutputVolume::SafeDownCast(m_Vme->GetOutput());
  if (out_data != NULL)
  {
    m_Material = out_data->GetMaterial();
  }
  assert(m_Material);

  this->m_VolumeProperty = vtkVolumeProperty2::New();
  if (m_Material->m_VolumeProperty2 != NULL)
    this->m_VolumeProperty->DeepCopy(m_Material->m_VolumeProperty2);
  
  if (this->m_VolumeProperty->GetTransferFunction2D() == NULL) 
	{
    this->m_TransferFunction = vtkTransferFunction2D::New();
    this->m_VolumeProperty->SetTransferFunction2D(this->m_TransferFunction);
    this->m_TransferFunction->Delete();
  }
  else 
	{
    this->m_TransferFunction = this->m_VolumeProperty->GetTransferFunction2D();
  }

  this->m_WaitActor      = NULL;
  this->m_SliceRwi       = NULL;
  this->m_SliceRenderer  = NULL;
  this->m_SliceResampler = NULL;
  this->m_SliceMapper    = NULL;
  this->m_SliceFilter    = NULL;
  this->m_SliceActor     = NULL;

  this->m_3DRwi = NULL;
  this->m_3DRenderer = NULL;
  this->m_3DMapper = NULL;
  this->m_3DVolume = NULL;

  this->m_GraphRwi = NULL;
  this->m_GraphRenderer = NULL;

  this->m_CurrentWidget = 0;
  this->m_Filename = "";

  this->m_CriticalSection = vtkCriticalSection::New();

  //initialize widget properties
  strcpy(this->m_Widget.Name, "new");

  this->m_DataReady = false;
  this->m_SlicePipeStatus = PipeNotReady;
  this->m_3DPipeStatus = PipeNotReady;

  this->m_WidgetActor = NULL;

  CreateGUI();

  vtkMultiThreader *threader = vtkMultiThreader::New();
  int threadID = threader->SpawnThread((vtkThreadFunctionType)CreatePipe, this);

	// Check for stored transfer function in volume vme
	if(m_Vme->GetTagArray()->IsTagPresent("VOLUME_TRANSFER_FUNCTION"))
		LoadTransferFunction();

  int x_pos,y_pos,w,h;
  mafGetFrame()->GetPosition(&x_pos,&y_pos);
  this->GetSize(&w,&h);
  this->SetSize(x_pos+5,y_pos+5,w,h);

  // open dialog
  this->wxDialog::ShowModal();

  ///////////////////// free memory
  this->m_CriticalSection->Lock(); // wait for thread to finish
  this->m_CriticalSection->Unlock();

  vtkDEL(this->m_VolumeProperty);
  
  m_SliceRenderer->GetActors();  //Paolo 23-06-2004 flushes the actors
  m_SliceRenderer->GetActors2D();//Paolo 23-06-2004 flushes the 2D actors
  vtkDEL(this->m_SliceRenderer);
  vtkDEL(this->m_SliceResampler);
  vtkDEL(this->m_SliceMapper);
  vtkDEL(this->m_SliceFilter);
  vtkDEL(this->m_SliceActor);
  vtkDEL(this->m_WaitActor);

  m_3DRenderer->GetVolumes();    //Paolo 23-06-2004 flushes the volumes
  vtkDEL(m_3DRenderer);
  vtkDEL(m_3DMapper);
  vtkDEL(m_3DVolume);

  m_GraphRenderer->GetActors();  //Paolo 23-06-2004 flushes the actors
  vtkDEL(m_GraphRenderer);
  vtkDEL(this->m_WidgetActor);

  m_SliceRwi->SetRenderWindow(NULL);
  m_3DRwi->SetRenderWindow(NULL);
  m_GraphRwi->SetRenderWindow(NULL);

  cppDEL(this->m_SliceRwi); 
  cppDEL(this->m_3DRwi);
  cppDEL(this->m_GraphRwi);

  vtkDEL(this->m_CriticalSection);
  threader->Delete();
}
//----------------------------------------------------------------------------
void mafGUIDialogTransferFunction2D::CreateGUI() 
//----------------------------------------------------------------------------
{
  static const wxSize controlTextSize(40, 16);

  // create two panes
  wxBoxSizer *mainSizer  = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *upPane     = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *bottomPane = new wxBoxSizer(wxHORIZONTAL);
  mainSizer->Add(upPane,     1, wxEXPAND | wxALIGN_TOP, 0);
  mainSizer->Add(bottomPane, 0, wxEXPAND | wxALIGN_BOTTOM, 0);

  wxBoxSizer *leftPane   = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *rightPane  = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *rightPaneU = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *rightPaneL = new wxBoxSizer(wxVERTICAL);
  upPane->Add(leftPane,  3, wxEXPAND | wxALIGN_LEFT, 0);
  upPane->Add(rightPane, 2, wxEXPAND | wxALIGN_RIGHT, 0);
  rightPane->Add(rightPaneU, 1, wxEXPAND | wxALIGN_TOP, 0);
  rightPane->Add(rightPaneL, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM, 0);

  //////////////////////////////////////// create left pane
  // slice preview notebook
  this->m_preview_book = new wxNotebook(this, -1, wxDefaultPosition, wxSize(360, 360));
  leftPane->Add(this->m_preview_book, 2, wxEXPAND, 0);
  // first page: slice preview
  wxPanel *previewPage = new wxPanel(this->m_preview_book);
  this->m_preview_book->AddPage(previewPage, "XY");
  wxFlexGridSizer *gridSizer = new wxFlexGridSizer(2, 2);
  gridSizer->AddGrowableCol(0);
  gridSizer->AddGrowableRow(0);
  previewPage->SetAutoLayout(TRUE);
  previewPage->SetSizer(gridSizer);
  gridSizer->Fit(previewPage);
  gridSizer->SetSizeHints(previewPage);
  // render window
	m_SliceWindow = vtkRenderWindow::New();
  this->m_SliceRenderer	= vtkRenderer::New();
  m_SliceWindow->AddRenderer(this->m_SliceRenderer);
  this->m_SliceRenderer->SetBackground(0.f,0.f,0.f);
  this->m_SliceRenderer->LightFollowCameraOff();
  this->m_SliceRenderer->GetActiveCamera()->ParallelProjectionOn();
  
  this->m_WaitActor = vtkTextActor::New();
  this->m_WaitActor->SetInput("Initializing. Please wait...");
  vtkCoordinate *c  = this->m_WaitActor->GetPositionCoordinate();
  c->SetCoordinateSystemToNormalizedViewport();
  c->SetValue(0.35, 0.5f);
  this->m_SliceRenderer->AddActor(this->m_WaitActor);

  this->m_SliceRwi = new mafRWIBase(previewPage, -1);
	this->m_SliceRwi->SetRenderWindow(m_SliceWindow);
  vtkInteractorStylePreviewImage *pstyle = vtkInteractorStylePreviewImage::New();
  pstyle->SetDialog(this);
  m_SliceWindow->GetInteractor()->SetInteractorStyle(pstyle);
  pstyle->Delete();
  m_SliceWindow->SetInteractor(NULL);
  vtkDEL(m_SliceWindow);
  this->m_SliceRwi->Show(true);
	((wxWindow *)this->m_SliceRwi)->SetSize(0, 0, 350, 320);
  gridSizer->Add(this->m_SliceRwi, 1, wxEXPAND, 0);

  // slice slider
  this->m_SliceNumber = 0;
  wxSlider *sliceSlider = new wxSlider(previewPage, ID_SLICE_NUMBER, 0, 0, 1000, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL);
  gridSizer->Add(sliceSlider, 0, wxALIGN_CENTER | wxEXPAND, 0);

  // windowing slider
  this->m_WindowingSlider = new mmgRangeSlider(previewPage, ID_WINDOWING_SLIDER, wxDefaultPosition, wxSize(-1, 30), wxNO_BORDER);
  this->m_WindowingSlider->EnableCenterWidget(false);
  gridSizer->Add(this->m_WindowingSlider, 0, wxEXPAND, 0);

  // second page: 3D preview
  previewPage = new wxPanel(this->m_preview_book);
  this->m_preview_book->AddPage(previewPage, "3D");
  wxBoxSizer *tabSizer = new wxBoxSizer(wxVERTICAL);
  previewPage->SetAutoLayout(TRUE);
  previewPage->SetSizer(tabSizer);
  tabSizer->Fit(previewPage);
  tabSizer->SetSizeHints(previewPage);
  // render window
 	m_3DWindow = vtkRenderWindow::New();
  this->m_3DRenderer	= vtkRenderer::New();
  m_3DWindow->AddRenderer(this->m_3DRenderer);
  this->m_3DRenderer->SetBackground(0.f,0.f,0.f);
  this->m_3DRwi = new mafRWIBase(previewPage, -1);
	this->m_3DRwi->SetRenderWindow(m_3DWindow);
  m_3DWindow->SetInteractor(NULL);
  vtkDEL(m_3DWindow);
  this->m_3DRwi->Show(true);
	((wxWindow *)this->m_3DRwi)->SetSize(0, 0, 350, 350);
  tabSizer->Add(this->m_3DRwi, 1, wxEXPAND, 0);

  // graph window
 	m_GraphWindow = vtkRenderWindow::New();
  this->m_GraphRenderer = vtkRenderer::New();
  m_GraphWindow->AddRenderer(this->m_GraphRenderer);
  this->m_GraphRenderer->SetBackground(0.f,0.f,0.f);
  this->m_GraphRenderer->LightFollowCameraOff();
  this->m_GraphRenderer->GetActiveCamera()->ParallelProjectionOn();
  this->m_GraphRwi = new mafRWIBase(this, -1);
	this->m_GraphRwi->SetRenderWindow(m_GraphWindow);
  vtkInteractorStyleWidget *wstyle = vtkInteractorStyleWidget::New();
  wstyle->SetDialog(this);
  m_GraphWindow->GetInteractor()->SetInteractorStyle(wstyle);
  wstyle->Delete();
  m_GraphWindow->SetInteractor(NULL);
  vtkDEL(m_GraphWindow);
  this->m_GraphRwi->Show(true);
	((wxWindow *)this->m_GraphRwi)->SetSize(0, 0, 350, 180);
  wxStaticBoxSizer *tmpSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, "Value / Gradient graph"), wxVERTICAL);
  tmpSizer->Add(this->m_GraphRwi, 1, wxEXPAND, 0);
  leftPane->Add(tmpSizer, 1, wxEXPAND, 0);

  // create right pane
  // tf selector
  this->m_WidgetList = new wxListBox(this, ID_TF_WIDGET_LIST, wxDefaultPosition, wxSize(-1, 70));
  tmpSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, "Widget: "), wxHORIZONTAL);
  rightPaneU->Add(tmpSizer, 0, wxEXPAND | wxALIGN_LEFT, 0);
  wxBoxSizer *addRemoveSizer = new wxBoxSizer(wxVERTICAL);
  tmpSizer->Add(this->m_WidgetList, 1, wxEXPAND, 0);
  tmpSizer->Add(addRemoveSizer, 0, wxALIGN_RIGHT | wxLEFT, 2);

  // add / remove buttons
  mmgButton  *b1, *b2;
  b1 = new mmgButton(this, ID_TF_ADD,    "add",wxDefaultPosition, wxSize(40, 20));
  b1->SetValidator(mmgValidator(this, ID_TF_ADD, b1));
  b2 = new mmgButton(this, ID_TF_REMOVE, "remove", wxDefaultPosition, wxSize(40, 20));
  b2->SetValidator(mmgValidator(this, ID_TF_REMOVE, b2));
  addRemoveSizer->Add(b1, 0, wxALIGN_TOP | wxALL, 6);
  addRemoveSizer->Add(b2, 0, wxALIGN_BOTTOM | wxALL, 6);

  // name
  rightPaneU->Add(new wxStaticText(this, -1, ""), 0, wxALIGN_CENTER);
  this->m_WidgetName = "";
  wxTextCtrl  *text = new wxTextCtrl(this, ID_TF_NAME, this->m_WidgetName, wxDefaultPosition, wxSize(-1, 20), wxNO_BORDER);
	text->SetValidator(mmgValidator(this, ID_TF_NAME, text, &this->m_WidgetName));
  wxBoxSizer *nameSizer = new wxBoxSizer(wxHORIZONTAL);
  nameSizer->Add(new wxStaticText(this, -1, "Name: ", wxDefaultPosition, controlTextSize), 0, wxALIGN_LEFT | wxRIGHT, 5);
  nameSizer->Add(text, wxEXPAND | wxALIGN_RIGHT, 0);
  rightPaneU->Add(nameSizer, 0, wxEXPAND | wxALIGN_LEFT | wxALL, 5);

  // colour
  this->m_WidgetColor.Set(this->m_Widget.Color[0] * 255, this->m_Widget.Color[1] * 255, this->m_Widget.Color[2] * 255);
  wxString bmp_id = "";
  bmp_id << MENU_FILE_OPEN;
  mmgPicButton *colorOpen = new mmgPicButton(this, bmp_id, ID_TF_COLOR);
	wxTextCtrl   *colorBox = new wxTextCtrl  (this, ID_TF_COLOR, "", wxDefaultPosition, wxSize(-1,16), wxTE_READONLY|wxNO_BORDER);
  colorOpen->SetValidator( mmgValidator(this, ID_TF_COLOR, colorOpen, &this->m_WidgetColor, colorBox));
  
  wxBoxSizer *colorSizer = new wxBoxSizer(wxHORIZONTAL);
  colorSizer->Add(new wxStaticText(this, -1, "Colour: ", wxDefaultPosition, controlTextSize, wxALIGN_LEFT), 0, wxALIGN_LEFT | wxRIGHT, 5);
  colorSizer->Add(colorBox, 1, /*wxEXPAND |*/ wxALIGN_LEFT, 0);
  colorSizer->Add(colorOpen, 0, wxRIGHT);
  rightPaneU->Add(colorSizer, 0, wxEXPAND | wxALIGN_LEFT | wxALL, 5);

  // Opacity
  wxBoxSizer *opacitySizer = new wxBoxSizer(wxHORIZONTAL);

	text = new wxTextCtrl  (this, ID_TF_OPACITY, ""   , wxDefaultPosition, wxSize(35, 20), 0);
  mmgFloatSlider *slider = new mmgFloatSlider(this, ID_TF_OPACITY, 0, 0, 1, wxDefaultPosition, wxSize(40, 20));
  text->SetValidator(mmgValidator(this, ID_TF_OPACITY, text, &this->m_Widget.Opacity, slider, 0.f, 1.f));
	slider->SetValidator(mmgValidator(this, ID_TF_OPACITY, slider, &this->m_Widget.Opacity, text));

  opacitySizer->Add(new wxStaticText(this, -1, "Opacity: ", wxDefaultPosition, controlTextSize), 0, wxALIGN_LEFT | wxRIGHT, 5);
  opacitySizer->Add(text, 0, wxALIGN_CENTRE, 0);
  opacitySizer->Add(slider,  1, wxEXPAND | wxLEFT, 5);
  rightPaneU->Add(opacitySizer, 0, wxEXPAND | wxALIGN_LEFT | wxALL, 5);

  // Diffuse
  wxBoxSizer *diffuseSizer = new wxBoxSizer(wxHORIZONTAL);

	text = new wxTextCtrl  (this, ID_TF_DIFFUSE, ""   , wxDefaultPosition, wxSize(35, 20), 0);
  slider = new mmgFloatSlider(this, ID_TF_DIFFUSE, 0, 0, 1, wxDefaultPosition, wxSize(40, 20));
  text->SetValidator(mmgValidator(this, ID_TF_DIFFUSE, text, &this->m_Widget.Diffuse, slider, 0.f, 1.f));
	slider->SetValidator(mmgValidator(this, ID_TF_DIFFUSE, slider, &this->m_Widget.Diffuse, text));

  diffuseSizer->Add(new wxStaticText(this, -1, "Diffuse: ", wxDefaultPosition, controlTextSize), 0, wxALIGN_LEFT | wxRIGHT, 5);
  diffuseSizer->Add(text, 0, wxALIGN_CENTRE, 0);
  diffuseSizer->Add(slider,  1, wxEXPAND | wxLEFT, 5);
  rightPaneU->Add(diffuseSizer, 0, wxEXPAND | wxALIGN_LEFT | wxALL, 5);

  // value
  wxStaticBoxSizer *valueSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, "Value range"), wxVERTICAL);
  this->m_ValueSlider = new mmgRangeSlider(this, ID_TF_VALUE, wxDefaultPosition, wxSize(-1, 30), wxNO_BORDER);
  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  this->m_Widget.Range[0][0] = this->m_Widget.Range[0][1] = this->m_Widget.Range[0][2] = 0;
  int j;
  for (j = 0; j < 3; j++) 
	{
    text = new wxTextCtrl(this, ID_TF_VALUE_0 + j, "", wxDefaultPosition, wxSize(-1, 20), 0);
    text->SetValidator(mmgValidator(this, ID_TF_VALUE_0 + j, text, this->m_Widget.Range[0] + (j == 0 ? 0 : (j == 1 ? 2 : 1)), -99999, 99999, 0));
    sizer->Add(text, 1, wxEXPAND | wxALIGN_LEFT | wxLEFT | wxRIGHT, 5);
  }
  valueSizer->Add(this->m_ValueSlider, 0, wxEXPAND | wxALIGN_LEFT | wxTOP | wxBOTTOM, 5);
  valueSizer->Add(sizer, 0, wxEXPAND | wxALIGN_LEFT, 0);
  rightPaneU->Add(valueSizer, 0, wxEXPAND | wxALIGN_LEFT | wxALL, 5);

  wxStaticBoxSizer *gradientSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, "Gradient range"), wxVERTICAL);
  this->m_GradientSlider = new mmgRangeSlider(this, ID_TF_GRADIENT, wxDefaultPosition, wxSize(-1, 30), wxNO_BORDER);
  sizer = new wxBoxSizer(wxHORIZONTAL);
  this->m_Widget.Range[1][0] = this->m_Widget.Range[1][1] = this->m_Widget.Range[1][2] = 0;
  for (j = 0; j < 3; j++) 
	{
    text = new wxTextCtrl(this, ID_TF_GRADIENT_0 + j, "", wxDefaultPosition, wxSize(-1, 20), 0);
    text->SetValidator(mmgValidator(this, ID_TF_GRADIENT_0 + j, text, this->m_Widget.Range[1] + (j == 0 ? 0 : (j == 1 ? 2 : 1)), -99999, 99999, 0));
    sizer->Add(text, 1, wxEXPAND | wxALIGN_LEFT | wxLEFT | wxRIGHT, 5);
  }
  gradientSizer->Add(this->m_GradientSlider, 0, wxEXPAND | wxALIGN_LEFT | wxTOP | wxBOTTOM, 5);
  gradientSizer->Add(sizer, 0, wxEXPAND | wxALIGN_LEFT | wxBOTTOM, 8);
  
  sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(new wxStaticText(this, -1, "interpolation: ", wxDefaultPosition, wxSize(70, 20)), 0, wxALIGN_LEFT | wxLEFT | wxRIGHT, 5);
  wxString interpolationTypeStrings[2] = { wxString("constant"), wxString("linear") };
  wxChoice *interpolationBox = new wxChoice(this, ID_TF_GRADIENT_INTERPOLATION, wxDefaultPosition, wxSize(-1, 20), 2, interpolationTypeStrings);
  interpolationBox->SetSelection(0);
  sizer->Add(interpolationBox, 1, wxEXPAND | wxALIGN_RIGHT | wxLEFT | wxRIGHT, 5);
  gradientSizer->Add(sizer, 0, wxEXPAND | wxALIGN_LEFT | wxBOTTOM, 5);

  rightPaneU->Add(gradientSizer, 0, wxEXPAND | wxALIGN_LEFT | wxALL, 5);
  
  // library
//  b1 = new mmgButton(this, ID_TF_LOAD, "load", wxDefaultPosition, wxSize(-1, 20));
//  b1->SetValidator(mmgValidator(this, ID_TF_LOAD, b1));
//  b2 = new mmgButton(this, ID_TF_SAVE, "save",wxDefaultPosition, wxSize(-1, 20));
//  b2->SetValidator(mmgValidator(this, ID_TF_SAVE, b2));
//  wxBoxSizer *ioSizer = new wxBoxSizer(wxHORIZONTAL);
//  ioSizer->Add(b1, 0, wxALIGN_CENTER,  0);
//  ioSizer->Add(b2, 0, wxALIGN_CENTER | wxLEFT,  4);
//  rightPaneL->Add(ioSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 0);
//  rightPaneL->Add(new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(-1, 20)), wxEXPAND, 0);

  // ok, cancel
  b1 = new mmgButton(this, wxOK,    "ok", wxDefaultPosition, wxSize(-1, 20));
  b1->SetValidator(mmgValidator(this, wxOK, b1));
  b2 = new mmgButton(this, wxCANCEL,    "cancel",wxDefaultPosition, wxSize(-1, 20));
  b2->SetValidator(mmgValidator(this, wxCANCEL, b2));
  
  wxBoxSizer *applyOkCancelSizer = new wxBoxSizer(wxHORIZONTAL);
  applyOkCancelSizer->Add(b1, 0, wxALIGN_CENTER | wxLEFT,  12);
  applyOkCancelSizer->Add(b2, 0, wxALIGN_CENTER | wxLEFT,  12);
  rightPaneL->Add(applyOkCancelSizer, 0, wxALIGN_BOTTOM | wxALL, 5);

  // status bar
  this->m_StatusBar = new wxStatusBar(this, -1);
  this->m_StatusBar->SetFieldsCount(2);
  this->m_StatusBar->SetMinHeight(18);
  int widths[2] = { 70, -1 };
  this->m_StatusBar->SetStatusWidths(2, widths);
  bottomPane->Add(this->m_StatusBar, 1, wxEXPAND | wxALIGN_RIGHT, 0);

  // switch auto-layout
  this->SetAutoLayout( TRUE );
  this->SetSizer(mainSizer);
  mainSizer->Fit(this);
  mainSizer->SetSizeHints(this);

  this->InitializeControls();
  this->Enable(false);
}
//----------------------------------------------------------------------------
void mafGUIDialogTransferFunction2D::InitializeControls() 
//----------------------------------------------------------------------------
{
  // get data from widget
  if (this->m_TransferFunction) 
	{
    while (this->m_CurrentWidget >= this->m_TransferFunction->GetNumberOfWidgets())
      this->m_CurrentWidget--;
  }
  else 
	{
    this->m_CurrentWidget = -1;
  }

  if (this->m_CurrentWidget >= 0 && this->m_TransferFunction) 
	{
    this->m_Widget = this->m_TransferFunction->GetWidget(this->m_CurrentWidget);
    this->m_WidgetName = this->m_TransferFunction->GetWidgetName(this->m_CurrentWidget);
    this->m_WidgetColor.Set(this->m_Widget.Color[0] * 255.f, this->m_Widget.Color[1] * 255.f, this->m_Widget.Color[2] * 255.f);
    
    TransferDataToWindow();
    this->m_ValueSlider->SetValue(0, this->m_Widget.Range[0][0]);
    this->m_ValueSlider->SetValue(1, this->m_Widget.Range[0][2]);
    this->m_ValueSlider->SetValue(2, this->m_Widget.Range[0][1]);
    this->m_GradientSlider->SetValue(0, this->m_Widget.Range[1][0]);
    this->m_GradientSlider->SetValue(1, this->m_Widget.Range[1][2]);
    this->m_GradientSlider->SetValue(2, this->m_Widget.Range[1][1]);
    ((wxChoice*)(this->FindWindow(ID_TF_GRADIENT_INTERPOLATION)))->SetSelection(this->m_Widget.GradientInterpolationOrder);
  }
  
  // enable/ disable some controls
  wxWindowList &list = this->GetChildren();
  for (wxWindowListNode *node = list.GetFirst(); node != NULL; node = node->GetNext() ) 
	{
    wxWindow *win = (wxWindow *)node->GetData();
    switch (win->GetId()) 
		{
      case ID_TF_COLOR:
      case ID_TF_OPACITY:
      case ID_TF_DIFFUSE:
      case ID_TF_VALUE:
      case ID_TF_VALUE_0:
      case ID_TF_VALUE_1:
      case ID_TF_VALUE_2:
      case ID_TF_GRADIENT:
      case ID_TF_GRADIENT_INTERPOLATION:
      case ID_TF_GRADIENT_0:
      case ID_TF_GRADIENT_1:
      case ID_TF_GRADIENT_2:
      case ID_TF_REMOVE:
        win->Enable(this->m_CurrentWidget >= 0);
      break;
      case ID_TF_SAVE:
        win->Enable(this->m_TransferFunction != NULL && this->m_TransferFunction->GetNumberOfWidgets() > 0);
      break;
      case ID_TF_LOAD:
      case ID_WINDOWING_SLIDER:
        win->Enable(this->m_DataReady && this->m_TransferFunction != NULL);
      break;
      case ID_TF_ADD:
        win->Enable(this->m_DataReady && this->m_TransferFunction != NULL && this->m_TransferFunction->GetNumberOfWidgets() < 12);
      default:
      break;
    }
  }
  if (this->m_SlicePipeStatus >= PipeReady)
    this->m_SliceFilter->SetWidgetIndex(this->m_CurrentWidget);
}
//----------------------------------------------------------------------------
void mafGUIDialogTransferFunction2D::SetWidget(const tfWidget &widget) 
//----------------------------------------------------------------------------
{
  this->m_Widget = widget;
  if (this->m_CurrentWidget >= 0 && this->m_TransferFunction) 
	{
    this->m_TransferFunction->SetWidget(this->m_CurrentWidget, widget);
    this->InitializeControls();
    this->UpdatePreview();
  }
}
//----------------------------------------------------------------------------
void mafGUIDialogTransferFunction2D::CreateWidgetList() 
//----------------------------------------------------------------------------
{
  if (this->m_TransferFunction == NULL)
    return;
  this->m_WidgetList->Clear();
  for (int i = 0; i < this->m_TransferFunction->GetNumberOfWidgets(); i++) 
	{
    char buffer[sizeof(this->m_Widget.Name) + 10];
    sprintf(buffer, "%d: %s", i, this->m_TransferFunction->GetWidgetName(i));
    this->m_WidgetList->Append(buffer);
  }

  while (this->m_CurrentWidget >= this->m_TransferFunction->GetNumberOfWidgets())
    this->m_CurrentWidget--;
  if (this->m_CurrentWidget >= 0)
    this->m_WidgetList->SetSelection(this->m_CurrentWidget);

  this->UpdateWidgets();
}
//----------------------------------------------------------------------------
void mafGUIDialogTransferFunction2D::SetControlsRange() 
//----------------------------------------------------------------------------
{
  assert(this->m_DataReady);
  
  if (this->m_TransferFunction == NULL || this->m_TransferFunction->GetNumberOfWidgets() == 0) 
	{
    this->m_Widget.Range[0][0] = this->m_DataRange[0];
    this->m_Widget.Range[0][1] = this->m_DataRange[1];
    this->m_Widget.Range[0][2] = 0.5f * (this->m_DataRange[0] + this->m_DataRange[1]);
    this->m_Widget.Range[1][0] = this->m_GradientRange[0];
    this->m_Widget.Range[1][1] = this->m_GradientRange[1];
    this->m_Widget.Range[1][2] = 0.5f * (this->m_GradientRange[0] + this->m_GradientRange[1]);
    this->m_ValueSlider->SetValue(0, this->m_Widget.Range[0][0]);
    this->m_ValueSlider->SetValue(1, this->m_Widget.Range[0][2]);
    this->m_ValueSlider->SetValue(2, this->m_Widget.Range[0][1]);
    this->m_GradientSlider->SetValue(0, this->m_Widget.Range[1][0]);
    this->m_GradientSlider->SetValue(1, this->m_Widget.Range[1][2]);
    this->m_GradientSlider->SetValue(2, this->m_Widget.Range[1][1]);
  }

  const double dx = 0.05 * (this->m_DataRange[1] - this->m_DataRange[0]);
  const double dy = 0.05 * (this->m_GradientRange[1] - this->m_GradientRange[0]);
  this->m_ValueSlider->SetRange(this->m_DataRange[0] - dx, this->m_DataRange[1] + dx);
  this->m_GradientSlider->SetRange(this->m_GradientRange[0] - dy, this->m_GradientRange[1] + dy);
  this->m_WindowingSlider->SetValue(0, this->m_SliceLevel - 0.5 * this->m_SliceWinowing);
  this->m_WindowingSlider->SetValue(1, this->m_SliceLevel);
  this->m_WindowingSlider->SetValue(2, this->m_SliceLevel + 0.5 * this->m_SliceWinowing);
  this->m_WindowingSlider->SetRange(this->m_DataRange[0] - dx, this->m_DataRange[1] + dx);

  wxSlider *slider = (wxSlider*)this->FindWindow(ID_SLICE_NUMBER);
  int extent[6];
  this->m_Vme->GetOutput()->GetVTKData()->GetWholeExtent(extent);
  slider->SetRange(0, extent[5] - extent[4]);
}

static const int WidgetTextureResolution = 256;

//----------------------------------------------------------------------------
void mafGUIDialogTransferFunction2D::UpdateWidgets() 
//----------------------------------------------------------------------------
{
  if (this->m_TransferFunction == NULL || !this->m_DataReady)
    return;
  
  // create widgets
  if (this->m_WidgetActor == NULL) 
	{
    this->m_WidgetActor = vtkWidgetActor::New();
    this->m_WidgetActor->SetTransferFunction(this->m_TransferFunction);
    
    const double dx = 0.05f * (this->m_DataRange[1] - this->m_DataRange[0]);
    const double dy = 0.05f * (this->m_GradientRange[1] - this->m_GradientRange[0]);
    this->m_WidgetActor->SetViewportRange(this->m_DataRange[0] - dx, this->m_DataRange[1] + dx, this->m_GradientRange[0] - dy, this->m_GradientRange[1] + dy);
    this->m_GraphRenderer->AddActor(this->m_WidgetActor);
  }

  this->m_WidgetActor->SetSelection(this->m_CurrentWidget);
}
//----------------------------------------------------------------------------
VTK_THREAD_RETURN_TYPE mafGUIDialogTransferFunction2D::CreatePipe(void *argDialog) 
//----------------------------------------------------------------------------
{
  mafGUIDialogTransferFunction2D *dialog = (mafGUIDialogTransferFunction2D*)(((ThreadInfoStruct *)argDialog)->UserData);

  dialog->m_CriticalSection->Lock();

  // 3d pipe
  dialog->m_3DVolume = vtkVolume::New();
  dialog->m_3DVolume->SetProperty(dialog->m_VolumeProperty);
  dialog->m_3DMapper = vtkMAFAdaptiveVolumeMapper::New();

  vtkImageCast *chardata = vtkImageCast::New();
  chardata->SetOutputScalarTypeToShort();
  vtkDataSet *data = dialog->m_Vme->GetOutput()->GetVTKData();

  // convert rect. data to image data
  vtkImageData *imageData = vtkImageData::SafeDownCast(data);
  vtkRectilinearGrid *gridData = vtkRectilinearGrid::SafeDownCast(data);

  // convert grid data to image data
  if (gridData && gridData->GetPointData() && gridData->GetPointData()->GetArray(0) &&
      gridData->GetXCoordinates()->GetNumberOfTuples() > 1 &&
      gridData->GetYCoordinates()->GetNumberOfTuples() > 1 &&
      gridData->GetZCoordinates()->GetNumberOfTuples() > 1) 
	{
    imageData = vtkImageData::New();
    imageData->ShallowCopy(gridData);
    imageData->SetDimensions(gridData->GetDimensions());
    imageData->SetNumberOfScalarComponents(gridData->GetPointData()->GetNumberOfComponents());
    double offset[3], spacing[3];
    offset[0] = gridData->GetXCoordinates()->GetTuple(0)[0];
    offset[1] = gridData->GetYCoordinates()->GetTuple(0)[0];
    offset[2] = gridData->GetZCoordinates()->GetTuple(0)[0];
    spacing[0] = gridData->GetXCoordinates()->GetTuple(1)[0] - offset[0];
    spacing[1] = gridData->GetYCoordinates()->GetTuple(1)[0] - offset[1];;
    spacing[2] = gridData->GetZCoordinates()->GetTuple(1)[0] - offset[2];;
    imageData->SetOrigin(offset);
    imageData->SetSpacing(spacing);
    // set type
    vtkDataArray *data = gridData->GetPointData()->GetArray(0);
    if (data->IsA("vtkUnsignedShortArray"))
      imageData->SetScalarType(VTK_UNSIGNED_SHORT);
    else if (data->IsA("vtkShortArray"))
      imageData->SetScalarType(VTK_SHORT);
    else if (data->IsA("vtkCharArray"))
      imageData->SetScalarType(VTK_CHAR);
    else if (data->IsA("vtkUnsignedCharArray"))
      imageData->SetScalarType(VTK_UNSIGNED_CHAR);
    else if (data->IsA("vtkFloatArray"))
      imageData->SetScalarType(VTK_FLOAT);
    else if (data->IsA("vtkDoubleArray"))
      imageData->SetScalarType(VTK_DOUBLE);
    else 
		{
      imageData->Delete();
      imageData = NULL;
    }
  }

  vtkDataArray *scalars = data->GetPointData()->GetScalars();
  if(scalars->GetDataType() != VTK_UNSIGNED_SHORT || 
    scalars->GetDataType() != VTK_SHORT ||
    scalars->GetDataType() != VTK_CHAR ||
    scalars->GetDataType() != VTK_UNSIGNED_CHAR)
  {
    chardata->SetInput(imageData);
    chardata->Update();
    dialog->m_3DMapper->SetInput(chardata->GetOutput());
  }
  else
  {
    dialog->m_3DMapper->SetInput((vtkDataSet *)data);
  }
  dialog->m_3DMapper->Update();
  dialog->m_3DVolume->SetMapper(dialog->m_3DMapper);
  dialog->m_3DRenderer->GetActiveCamera()->ParallelProjectionOff();
  dialog->m_3DRenderer->GetActiveCamera()->SetPosition(0., 0., 0.);
  dialog->m_3DRenderer->GetActiveCamera()->SetFocalPoint(0., 0., 1.);
  dialog->m_3DRenderer->ResetCamera(dialog->m_3DMapper->GetBounds());
  const int *range = dialog->m_3DMapper->GetDataRange();
  dialog->m_DataRange[0] = double(range[0]);
  dialog->m_DataRange[1] = double(range[1]);
  dialog->m_GradientRange[0] = 0.f;
  dialog->m_GradientRange[1] = dialog->m_3DMapper->GetGradientRange()[1];
  dialog->m_DataReady = true;
  dialog->m_3DPipeStatus = PipeReady;

  // create slice preview
  dialog->m_SliceActor = vtkActor2D::New();
  dialog->m_SliceActor->PickableOff();

  // filter
  dialog->m_SliceFilter = vtkMAFImageMapToWidgetColors::New();
  dialog->m_SliceFilter->SetInput(imageData);
  if (gridData && imageData)
    imageData->Delete();
  dialog->m_SliceWinowing = dialog->m_DataRange[1] - dialog->m_DataRange[0];
  dialog->m_SliceFilter->SetWindow(dialog->m_SliceWinowing);
  dialog->m_SliceLevel = 0.5f * (dialog->m_DataRange[1] + dialog->m_DataRange[0]);
  dialog->m_SliceFilter->SetLevel(dialog->m_SliceLevel);
  dialog->m_SliceFilter->SetTransferFunction(dialog->m_TransferFunction);
  dialog->m_SliceFilter->SetWidgetIndex(0);

  // resample image
  dialog->m_SliceResampler = vtkImageResample::New();
  dialog->m_SliceResampler->SetNumberOfThreads(1);
  dialog->m_SliceResampler->SetInput(dialog->m_SliceFilter->GetOutput());
  dialog->m_SliceResampler->InterpolateOn();

  // mapper
  dialog->m_SliceMapper = vtkImageMapper::New();
  dialog->m_SliceMapper->SetInput(dialog->m_SliceResampler->GetOutput());
  dialog->m_SliceMapper->SetColorWindow(255.f);
  dialog->m_SliceMapper->SetColorLevel(127.5f);
  dialog->m_SliceActor->SetMapper(dialog->m_SliceMapper);

  dialog->CreateWidgetList();
  dialog->SetControlsRange();
  dialog->InitializeControls();
  dialog->SetCursor(*wxSTANDARD_CURSOR);
  dialog->Enable();
  dialog->m_SlicePipeStatus = PipeReady;

  dialog->m_CriticalSection->Unlock();
  
  return VTK_THREAD_RETURN_VALUE;
}
//----------------------------------------------------------------------------
void mafGUIDialogTransferFunction2D::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (this->m_SlicePipeStatus < PipeReady)
    return;

  static wxString wildcard = "txt file (*.txt)|*.txt|all files (*.*)|*.*";

  double framerate = 0;
  switch(maf_event->GetId()) 
	{
    case wxOK:
      this->m_Material->m_VolumeProperty2->DeepCopy(this->m_VolumeProperty);
// 			this->m_Material->StoreToVme();
			SaveTransferFunction();
      this->EndModal(wxOK); 
    break;
    case wxCANCEL:
      this->EndModal(wxCANCEL); 
    break;
    case ID_TF_SAVE:
/*      if (this->m_TransferFunction->GetNumberOfWidgets() > 0) 
			{
				if(m_Vme->GetTagArray()->FindTag("VOLUME_TRANSFER_FUNCTION") != -1)
					m_Vme->GetTagArray()->DeleteTag("VOLUME_TRANSFER_FUNCTION");
        mafTagItem tfTAG;
				tfTAG.SetName("VOLUME_TRANSFER_FUNCTION");
				tfTAG.SetValue(this->m_TransferFunction->SaveToString());
				m_Vme->GetTagArray()->AddTag(tfTAG);
      }*/
    break;
    case ID_TF_LOAD:
/*      if(m_Vme->GetTagArray()->FindTag("VOLUME_TRANSFER_FUNCTION") == -1)
			{
				wxMessageBox("No volume transfer function stored into this volume data!!","Warning");
				return;
			}
      {
			wxString string =	m_Vme->GetTagArray()->GetTag("VOLUME_TRANSFER_FUNCTION")->GetValue();
      vtkTransferFunction2D *newf = vtkTransferFunction2D::New();
			newf->DeepCopy(this->m_TransferFunction);
			if (newf->LoadFromString(string.c_str()))
				this->m_TransferFunction->DeepCopy(newf);
			newf->Delete();
			}

      this->m_CurrentWidget = 0;
      this->CreateWidgetList();
      this->InitializeControls();
      this->UpdatePreview();*/
    break;
    case ID_SLICE_NUMBER:
      if (this->m_SliceMapper != NULL) 
			{
        wxSlider *slider = (wxSlider*)this->FindWindow(ID_SLICE_NUMBER);
        this->m_SliceNumber = clip(slider->GetValue(), 0, this->m_SliceMapper->GetWholeZMax() - 1);
        this->m_SliceMapper->SetZSlice(this->m_SliceNumber);
        this->UpdatePreview();
      }
    break;
    case ID_TF_WIDGET_LIST: // change selection
      if (this->m_WidgetList->GetSelection() < 0 && this->m_WidgetList->GetCount() > 0)
        this->m_WidgetList->SetSelection(0);
      this->m_CurrentWidget = this->m_WidgetList->GetSelection();
      this->InitializeControls();
      this->UpdatePreview();
    break;
    case ID_TF_NAME:
      if (this->m_CurrentWidget >= 0 && this->m_CurrentWidget < this->m_TransferFunction->GetNumberOfWidgets())
        this->m_TransferFunction->SetWidgetName(this->m_CurrentWidget, this->m_WidgetName);
      this->CreateWidgetList();
      strncpy(this->m_Widget.Name, this->m_WidgetName, sizeof(this->m_Widget.Name) - 1);
    break;
    case ID_TF_COLOR:
      if (this->m_CurrentWidget >= 0 && this->m_CurrentWidget < this->m_TransferFunction->GetNumberOfWidgets()) 
			{
        this->m_Widget.Color[0] = this->m_WidgetColor.Red() / 255.f;
        this->m_Widget.Color[1] = this->m_WidgetColor.Green() / 255.f;
        this->m_Widget.Color[2] = this->m_WidgetColor.Blue() / 255.f;
        this->m_TransferFunction->SetWidgetColor(this->m_CurrentWidget, this->m_Widget.Color);
        this->UpdatePreview();
      }
    break;
    case ID_TF_OPACITY:
      if (this->m_CurrentWidget >= 0 && this->m_CurrentWidget < this->m_TransferFunction->GetNumberOfWidgets() &&
          this->m_TransferFunction->SetWidgetOpacity(this->m_CurrentWidget, this->m_Widget.Opacity)) 
			{
        this->UpdatePreview();
      }
    break;
    case ID_TF_DIFFUSE:
      if (this->m_CurrentWidget >= 0 && this->m_CurrentWidget < this->m_TransferFunction->GetNumberOfWidgets() &&
          this->m_TransferFunction->SetWidgetDiffuse(this->m_CurrentWidget, this->m_Widget.Diffuse)) 
			{
        if (this->m_preview_book->GetSelection() == 1)
          this->UpdatePreview();
      }
      else 
			{
        this->InitializeControls();
      }
    break;
    case ID_TF_VALUE:
    case ID_TF_VALUE_0:
    case ID_TF_VALUE_1:
    case ID_TF_VALUE_2:
      if (maf_event->GetId() == ID_TF_VALUE) 
			{
        this->m_Widget.Range[0][0] = this->m_ValueSlider->GetValue(0);
        this->m_Widget.Range[0][1] = this->m_ValueSlider->GetValue(2);
        this->m_Widget.Range[0][2] = this->m_ValueSlider->GetValue(1);
        if (this->m_preview_book->GetSelection() == 1 && this->m_ValueSlider->IsDragging()) 
				{
          framerate = this->m_3DRenderer->GetRenderWindow()->GetDesiredUpdateRate();
          this->m_3DRenderer->GetRenderWindow()->SetDesiredUpdateRate(15.f);
        }
      }
      if (this->m_CurrentWidget >= 0 && this->m_CurrentWidget < this->m_TransferFunction->GetNumberOfWidgets() &&
          this->m_TransferFunction->SetWidgetValueRange(this->m_CurrentWidget, this->m_Widget.Range[0])) 
			{
        this->UpdatePreview();
      }
      if (framerate != 0)
        this->m_3DRenderer->GetRenderWindow()->SetDesiredUpdateRate(framerate);
      this->InitializeControls();
    break;
    case ID_TF_GRADIENT:
    case ID_TF_GRADIENT_0:
    case ID_TF_GRADIENT_1:
    case ID_TF_GRADIENT_2:
      if (maf_event->GetId() == ID_TF_GRADIENT) 
			{
        this->m_Widget.Range[1][0] = this->m_GradientSlider->GetValue(0);
        this->m_Widget.Range[1][1] = this->m_GradientSlider->GetValue(2);
        this->m_Widget.Range[1][2] = this->m_GradientSlider->GetValue(1);
      }
      if (this->m_CurrentWidget >= 0 && this->m_CurrentWidget < this->m_TransferFunction->GetNumberOfWidgets() &&
          this->m_TransferFunction->SetWidgetGradientRange(this->m_CurrentWidget, this->m_Widget.Range[1])) 
			{
        this->UpdatePreview();
      }
      this->InitializeControls();
    break;
    case ID_TF_GRADIENT_INTERPOLATION:
      if (this->m_CurrentWidget >= 0 && this->m_CurrentWidget < this->m_TransferFunction->GetNumberOfWidgets() &&
          this->m_TransferFunction->SetWidgetGradientInterpolation(this->m_CurrentWidget, ((wxChoice*)(this->FindWindow(ID_TF_GRADIENT_INTERPOLATION)))->GetSelection())) 
			{
        this->UpdatePreview();  
      }
      this->InitializeControls();
    break;
    case ID_TF_ADD:
      if (this->m_TransferFunction->AddWidget(this->m_Widget) >= 0) 
			{
        this->m_CurrentWidget = this->m_TransferFunction->GetNumberOfWidgets() - 1;
        this->CreateWidgetList();
        this->InitializeControls();
        this->UpdatePreview();
      }
    break;
    case ID_TF_REMOVE:
      if (this->m_CurrentWidget >= 0 && this->m_CurrentWidget < this->m_TransferFunction->GetNumberOfWidgets()) 
			{
        this->m_TransferFunction->RemoveWidget(this->m_CurrentWidget);
        this->m_CurrentWidget--;
        this->CreateWidgetList();
        this->InitializeControls();
        this->UpdatePreview();
      }
    break;
    case ID_WINDOWING_SLIDER:
      this->m_SliceLevel  = 0.5f * (this->m_WindowingSlider->GetValue(2) + this->m_WindowingSlider->GetValue(0));
      this->m_SliceWinowing = this->m_WindowingSlider->GetValue(2) - this->m_SliceLevel;
      if (this->m_SliceFilter) 
			{
        this->m_SliceFilter->SetWindow(this->m_SliceWinowing + 0.1f);
        this->m_SliceFilter->SetLevel(this->m_SliceLevel);
        this->UpdatePreview();
      }
    break;
    case ID_GUI_UPDATE:
    break;
    default:
      mafEventMacro(*maf_event);
    break; 
  }
}
//----------------------------------------------------------------------------
void mafGUIDialogTransferFunction2D::OnIdle(wxIdleEvent& event) 
//----------------------------------------------------------------------------
{
  if (this->m_SlicePipeStatus == PipeReady) 
	{
    this->m_SlicePipeStatus = PipePlugged;
    this->ResizePreviewWindow();
    //this->m_WaitActor->VisibilityOff();
    m_SliceRenderer->RemoveActor(this->m_WaitActor);
    this->m_SliceRenderer->AddActor(this->m_SliceActor);
    this->UpdatePreview();
  }
  if (this->m_3DPipeStatus == PipeReady) 
	{
    this->m_3DPipeStatus = PipePlugged;
    this->m_3DRenderer->AddVolume(this->m_3DVolume);
    this->UpdatePreview();
  }
}
//----------------------------------------------------------------------------
void mafGUIDialogTransferFunction2D::UpdatePreview() 
//----------------------------------------------------------------------------
{
  if (!this->m_Vme)
    return;

  const int page = this->m_preview_book->GetSelection();
  if (page == 0 && this->m_SlicePipeStatus == PipePlugged && this->m_SliceRenderer)
    this->m_SliceRenderer->GetRenderWindow()->Render();
  else if (page == 1  && this->m_3DPipeStatus == PipePlugged)
    this->m_3DRenderer->GetRenderWindow()->Render();
  this->UpdateWidgets();
  this->m_GraphRenderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void mafGUIDialogTransferFunction2D::ResizePreviewWindow() 
//----------------------------------------------------------------------------
{
  if (this->m_Vme && this->m_SlicePipeStatus >= PipeReady) 
	{
    int extent[6];
    this->m_Vme->GetOutput()->GetVTKData()->GetWholeExtent(extent);
    double zoom = min(m_SliceRwi->GetClientSize().x / double(extent[1] - extent[0]), m_SliceRwi->GetClientSize().y / double(extent[3] - extent[2]));
    this->m_SliceResampler->SetAxisMagnificationFactor(0, zoom);
    this->m_SliceResampler->SetAxisMagnificationFactor(1, zoom);

    vtkCoordinate *viewportCenter = vtkCoordinate::New();
    viewportCenter->SetCoordinateSystemToNormalizedViewport();
    viewportCenter->SetValue(0.5, 0.5);
    vtkCoordinate *c  = this->m_SliceActor->GetPositionCoordinate();
    c->SetReferenceCoordinate(viewportCenter);
    c->SetCoordinateSystemToDisplay();
    c->SetValue(-0.5f * (extent[1] - extent[0]) * zoom, -0.5f * (extent[3] - extent[2]) * zoom);
    viewportCenter->Delete();
  }
}
//----------------------------------------------------------------------------
void mafGUIDialogTransferFunction2D::LoadTransferFunction()
//----------------------------------------------------------------------------
{
	wxString string =	m_Vme->GetTagArray()->GetTag("VOLUME_TRANSFER_FUNCTION")->GetValue();
  vtkTransferFunction2D *newf = vtkTransferFunction2D::New();
	newf->DeepCopy(this->m_TransferFunction);
	if (newf->LoadFromString(string.c_str()))
		this->m_TransferFunction->DeepCopy(newf);
	newf->Delete();

  this->m_CurrentWidget = 0;
}
//----------------------------------------------------------------------------
void mafGUIDialogTransferFunction2D::SaveTransferFunction()
//----------------------------------------------------------------------------
{
	if (this->m_TransferFunction->GetNumberOfWidgets() > 0) 
	{
		if(m_Vme->GetTagArray()->IsTagPresent("VOLUME_TRANSFER_FUNCTION"))
			m_Vme->GetTagArray()->DeleteTag("VOLUME_TRANSFER_FUNCTION");
    mafTagItem tfTAG;
		tfTAG.SetName("VOLUME_TRANSFER_FUNCTION");
		tfTAG.SetValue(this->m_TransferFunction->SaveToString());
		m_Vme->GetTagArray()->SetTag(tfTAG);
  }
}
