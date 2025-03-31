/*=========================================================================
Program:   AlbaMaster
Module:    albaOpExtractIsosurface.cpp
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2018 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaOpExtractIsosurface.h"
#include "wx/busyinfo.h"
#include "wx\statline.h"

#include "albaDecl.h"
#include "albaGUI.h"

#include "albaGUIDialog.h"

#include "albaRWIBase.h"
#include "albaRWI.h"
#include "albaDeviceButtonsPadMouse.h"

#include "albaGUIButton.h"
#include "albaGUIValidator.h"
#include "albaGUIPicButton.h"
#include "albaGUIFloatSlider.h"

#include "albaVMEIterator.h"
#include "albaVME.h"
#include "albaVMESurface.h"
#include "albaVMEGroup.h"
#include "albaVMEOutput.h"
#include "albaInteractorExtractIsosurface.h"
#include "mmaVolumeMaterial.h"

#include "vtkLight.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "albaVMEVolumeGray.h"

#include "vtkMath.h"
#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkALBAContourVolumeMapper.h"
#include "vtkPolyDataMapper.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkInteractorStyleTrackballCamera.h" 
#include "vtkPolyData.h"
#include "vtkALBAFixedCutter.h"
#include "vtkPlane.h"
#include "vtkImageData.h"
#include "vtkTexture.h"
#include "vtkALBAVolumeSlicer.h"
#include "vtkSmartPointer.h"
#include "vtkLookupTable.h"
#include "vtkVolume.h"
#include "vtkTriangleFilter.h"
#include "vtkCleanPolyData.h"
#include "vtkPolyDataConnectivityFilter.h"

const bool DEBUG_MODE = false;

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpExtractIsosurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// widget ID's
//----------------------------------------------------------------------------
enum EXTRACT_ISOSURFACE_ID
{
	ID_ISO_SLIDER = MINID,
	ID_INCREASE_ISO,
	ID_DECREASE_ISO,
	ID_ISO,
	ID_FIT,
	ID_GRID,
	ID_SLICE,
	ID_SLICE_SLIDER,
	ID_INCREASE_SLICE,
	ID_DECREASE_SLICE,
	ID_VIEW_SLICE,
	ID_OPTIMIZE_CONTOUR,
	ID_AUTO_LOD,

	ID_MULTIPLE_CONTOURS,
	ID_NUM_OF_CONTOURS,
	ID_MIN_RANGE,
	ID_MAX_RANGE,

	ID_OK,
	ID_CANCEL,
	ID_TRIANGULATE,
	ID_CONNECTIVITY,
	ID_CLEAN,
	ID_TRILINEAR_INTERPOLATION_ON,
};

//----------------------------------------------------------------------------
albaOpExtractIsosurface::albaOpExtractIsosurface(const wxString &label) :
albaOp(label)
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;

  m_IsoSlider   = NULL;
  m_SliceSlider = NULL;
  m_OutputGroup = NULL;

  m_Dialog = NULL;
  m_Rwi = NULL;

  m_IsoValue = 0;
  m_MinDensity = 0;
  m_MaxDensity = 0;

  m_Slice     = 0;
  m_SliceMin  = 0;
  m_SliceMax  = 0;
  m_ShowSlice = 1;
  m_Autolod = 1;
  m_Optimize     = 1;
	m_Triangulate = 1;
	m_Clean				= 1;
	m_Connectivity = 0;

  m_MultiContoursFlag = 0;
  m_NumberOfContours = 1;
  m_MinRange = 0;
  m_MaxRange = 0;

  for(int i=0; i<6; i++) 
    m_BoundingBox[i]=0;

  m_Box = NULL;

  m_ContourVolumeMapper  = NULL; 
  m_OutlineFilter  = NULL;
  m_OutlineMapper  = NULL;

  m_DensityPicker  = NULL;
  m_IsoValueVector.clear();

  m_TrilinearInterpolationOn = true;
}
//----------------------------------------------------------------------------
albaOpExtractIsosurface::~albaOpExtractIsosurface()
{
  if (m_OutputGroup != NULL)
  {
    albaVMEIterator *iter = m_OutputGroup->NewIterator();
    for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    {
      if (node != NULL)
      {
        albaDEL(node);
      }
    }
    iter->Delete();
  }

  m_IsoValueVector.clear();
  vtkDEL(m_ContourVolumeMapper);
  m_Output = NULL;
}
//----------------------------------------------------------------------------
albaOp* albaOpExtractIsosurface::Copy()
{
  /** return a copy of itself, needs to put it into the undo stack */
  return new albaOpExtractIsosurface(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpExtractIsosurface::InternalAccept(albaVME* vme)
{  
  bool is3DData = false;
  double bounds[6];
  vme->GetOutput()->GetVMEBounds(bounds);
  is3DData = (bounds[0] != bounds[1] && bounds[2] != bounds[3] && bounds[4] != bounds[5]);
  return vme != NULL && vme->GetOutput()->IsA("albaVMEOutputVolume") && is3DData;
}
//----------------------------------------------------------------------------
void albaOpExtractIsosurface::OpRun()
{
  int result = OP_RUN_CANCEL;
  
  CreateOpDialog();
  CreateVolumePipeline();
  CreateSlicePipeline();
  
  int ret_dlg = m_Dialog->ShowModal();
  if( ret_dlg == wxID_OK )
  {
    result = OP_RUN_OK;
    ExtractSurface();
  }
  DeleteOpDialog();

  albaEventMacro(albaEvent(this,result));
}

//----------------------------------------------------------------------------
char ** albaOpExtractIsosurface::GetIcon()
{
#include "pic/MENU_OP_EXTACT_ISOSURFACE.xpm"
	return MENU_OP_EXTACT_ISOSURFACE_xpm;
}

//----------------------------------------------------------------------------
void albaOpExtractIsosurface::CreateOpDialog()
{
  wxBusyCursor wait;
 
  vtkDataSet *dataset = m_Input->GetOutput()->GetVTKData();
  double sr[2];
  dataset->GetScalarRange(sr);
  m_MinDensity = sr[0];
  m_MaxDensity = sr[1];
  m_MinRange = m_MinDensity;
  m_MaxRange = m_MaxDensity;

  m_IsoValue = (m_MinDensity + m_MaxDensity)*0.5;

  double b[6];
  dataset->GetBounds(b);
  m_SliceMin = b[4];
  m_SliceMax = b[5];
  m_Slice = (m_SliceMin + m_SliceMax)*0.5;
	
  //===== setup interface ====
  m_Dialog = new albaGUIDialog("Extract Isosurface", albaCLOSEWINDOW | albaRESIZABLE);

  m_PIPRen = vtkRenderer::New();

  m_Rwi = new albaRWI(m_Dialog,ONE_LAYER,false);
  m_Rwi->SetListener(this);//SIL. 16-6-2004: 
  m_Rwi->CameraSet(CAMERA_PERSPECTIVE);
  //m_Rwi->SetAxesVisibility(true);
  //m_Rwi->SetGridVisibility(false);
  
  m_Rwi->SetSize(0,0,500,500);
  m_Rwi->m_RenderWindow->AddRenderer(m_PIPRen);
  m_Rwi->m_RenderWindow->SetDesiredUpdateRate(0.0001f);
  m_Rwi->Show(true);
  m_Rwi->m_RwiBase->SetMouse(m_Mouse);
  albaNEW(m_DensityPicker);
  m_DensityPicker->SetListener(this);	
  m_Mouse->AddObserver(m_DensityPicker, MCH_INPUT);
  m_PIPRen->SetViewport(.6, .01, .99, .4);
  m_PIPRen->GetActiveCamera()->SetFocalPoint(0,0,0);
  m_PIPRen->GetActiveCamera()->SetPosition(0,0,-70);
  m_PIPRen->GetActiveCamera()->SetViewUp(0,-1,0);
  m_PIPRen->GetActiveCamera()->ParallelProjectionOn();

  wxPoint p = wxDefaultPosition;

  // iso interface
  wxStaticText *lab  = new wxStaticText(m_Dialog,-1, "Contour value: ");
  wxStaticText *foo  = new wxStaticText(m_Dialog,-1, "");
  wxTextCtrl   *text = new wxTextCtrl  (m_Dialog,ID_ISO, "",								 		p,wxSize(50, 16 ), wxNO_BORDER );
  m_IsoSlider				 = new albaGUIFloatSlider(m_Dialog,ID_ISO_SLIDER,m_IsoValue,m_MinDensity,m_MaxDensity,p,wxSize(150,20));
  m_IsoSlider->SetNumberOfSteps(sr[1]-sr[0]);
  m_StepDensity = m_IsoSlider->GetStep();
  //m_MinDensity  = m_IsoSlider->GetMin();
  //m_MaxDensity  = m_IsoSlider->GetMax();
  m_IsoValue  = m_IsoSlider->GetValue();

  // slice interface
  wxStaticText *lab_slice  = new wxStaticText(m_Dialog,   -1, "Slice position:   ");
  wxStaticText *foo_slice  = new wxStaticText(m_Dialog,   -1, "");
  wxTextCtrl   *text_slice = new wxTextCtrl  (m_Dialog,   ID_SLICE, "",							 		p,wxSize(50, 16 ), wxNO_BORDER );
  m_SliceSlider						 = new albaGUIFloatSlider(m_Dialog, ID_SLICE_SLIDER,m_Slice,m_SliceMin,m_SliceMax,p,wxSize(150,20));
  m_SliceSlider->SetNumberOfSteps(200);
  m_SliceStep = m_SliceSlider->GetStep();
  m_SliceMin  = m_SliceSlider->GetMin();
  m_SliceMax  = m_SliceSlider->GetMax();
  m_Slice     = m_SliceSlider->GetValue();

  albaGUIButton *b_incr_slice = new albaGUIButton(m_Dialog, ID_INCREASE_SLICE, ">",	p,wxSize(25, 20));
  albaGUIButton *b_decr_slice = new albaGUIButton(m_Dialog, ID_DECREASE_SLICE, "<",	p,wxSize(25, 20));
  albaGUIButton *b_incr =       new albaGUIButton(m_Dialog, ID_INCREASE_ISO,   ">",	p,wxSize(25, 20));
  albaGUIButton *b_decr =       new albaGUIButton(m_Dialog, ID_DECREASE_ISO,   "<",	p,wxSize(25, 20));

  wxCheckBox *chk_interpolation = new wxCheckBox(m_Dialog, ID_TRILINEAR_INTERPOLATION_ON,"Interpolation", p, wxSize(80,20),1);

  wxCheckBox *chk_slice = new wxCheckBox(m_Dialog, ID_VIEW_SLICE,       "Slice", p, wxSize(80,20));
  wxCheckBox *chk_opt =   new wxCheckBox(m_Dialog, ID_OPTIMIZE_CONTOUR, "Optimize", p, wxSize(80,20));
  wxCheckBox *chk_lod =   new wxCheckBox(m_Dialog, ID_AUTO_LOD,         "Auto-lod", p, wxSize(80,20));
	wxCheckBox *chk_clean =   new wxCheckBox(m_Dialog, ID_CLEAN,_("Clean"), p, wxSize(80,20));
	wxCheckBox *chk_triangulate =   new wxCheckBox(m_Dialog, ID_TRIANGULATE, _("Triangulate"), p, wxSize(80,20));
	wxCheckBox *chk_connect = new wxCheckBox(m_Dialog, ID_CONNECTIVITY, _("Connectivity"), p, wxSize(80, 20));
	//  wxCheckBox *b_grid  = new wxCheckBox(m_Dialog, ID_GRID,         "show/hide grid", p, wxSize(80,20));

  albaGUIButton  *b_fit =    new albaGUIButton(m_Dialog, ID_FIT,    "Reset camera", p,wxSize(80,20));
  albaGUIButton  *b_ok =     new albaGUIButton(m_Dialog, ID_OK,     "Ok", p, wxSize(80,20));
  albaGUIButton  *b_cancel = new albaGUIButton(m_Dialog, ID_CANCEL, "Cancel", p, wxSize(80,20));

  wxCheckBox *chk_multi = new wxCheckBox(m_Dialog, ID_MULTIPLE_CONTOURS, "Multi contours", p, wxSize(100,20));
  wxTextCtrl *text_num_of_contours = new wxTextCtrl(m_Dialog, ID_NUM_OF_CONTOURS, "Num", p,wxSize(25, 16), wxNO_BORDER);
  wxStaticText *lab_multi_contours  = new wxStaticText(m_Dialog,   -1, "Range: ");
  wxTextCtrl *text_min_range = new wxTextCtrl(m_Dialog, ID_MIN_RANGE, "", p,wxSize(50, 16));
  wxTextCtrl *text_max_range = new wxTextCtrl(m_Dialog, ID_MAX_RANGE, "", p,wxSize(50, 16));

  // iso interface validator
  text->SetValidator(albaGUIValidator(this,ID_ISO,text,&m_IsoValue,m_MinDensity,m_MaxDensity));
  m_IsoSlider->SetValidator(albaGUIValidator(this,ID_ISO_SLIDER,m_IsoSlider,&m_IsoValue,text));
  b_incr->SetValidator(albaGUIValidator(this,ID_INCREASE_ISO,b_incr));
  b_decr->SetValidator(albaGUIValidator(this,ID_DECREASE_ISO,b_decr));

  // slice interface validator
  text_slice->SetValidator(albaGUIValidator(this,ID_SLICE,text_slice,&m_Slice,m_SliceMin,m_SliceMax));
  m_SliceSlider->SetValidator(albaGUIValidator(this,ID_SLICE_SLIDER,m_SliceSlider,&m_Slice,text_slice));
  b_incr_slice->SetValidator(albaGUIValidator(this,ID_INCREASE_SLICE,b_incr_slice));
  b_decr_slice->SetValidator(albaGUIValidator(this,ID_DECREASE_SLICE,b_decr_slice));

  b_fit->SetValidator(albaGUIValidator(this,ID_FIT,b_fit));
  b_ok->SetValidator(albaGUIValidator(this,ID_OK,b_ok));
  b_cancel->SetValidator(albaGUIValidator(this,ID_CANCEL,b_cancel));
  //  b_grid->SetValidator(albaGUIValidator(this,ID_GRID,b_grid));
  chk_interpolation->SetValidator( albaGUIValidator(this, ID_TRILINEAR_INTERPOLATION_ON, chk_interpolation, &m_TrilinearInterpolationOn));
	chk_slice->SetValidator( albaGUIValidator(this, ID_VIEW_SLICE, chk_slice, &m_ShowSlice));
	chk_clean->SetValidator( albaGUIValidator(this, ID_CLEAN, chk_clean, &m_Clean));
  chk_triangulate->SetValidator( albaGUIValidator(this, ID_TRIANGULATE, chk_triangulate, &m_Triangulate));
	chk_connect->SetValidator(albaGUIValidator(this, ID_CONNECTIVITY, chk_connect, &m_Connectivity));
	chk_opt->SetValidator(albaGUIValidator(this, ID_OPTIMIZE_CONTOUR,chk_opt, &m_Optimize));
  chk_lod->SetValidator(albaGUIValidator(this, ID_AUTO_LOD, chk_lod, &m_Autolod));

  chk_multi->SetValidator(albaGUIValidator(this, ID_MULTIPLE_CONTOURS, chk_multi, &m_MultiContoursFlag));
  text_num_of_contours->SetValidator(albaGUIValidator(this, ID_NUM_OF_CONTOURS,text_num_of_contours, &m_NumberOfContours, 1, 100));
  text_min_range->SetValidator(albaGUIValidator(this, ID_MIN_RANGE,text_min_range, &m_MinRange, m_MinDensity, m_MaxDensity));
  text_max_range->SetValidator(albaGUIValidator(this, ID_MAX_RANGE,text_max_range, &m_MaxRange, m_MinDensity, m_MaxDensity));

  wxBoxSizer *h_sizer0 = NULL;
  
  wxBoxSizer *h_sizer1 = new wxBoxSizer(wxHORIZONTAL);
  h_sizer1->Add(lab,     0,wxLEFT);	
  h_sizer1->Add(text,    0,wxLEFT);	
  h_sizer1->Add(b_decr,0,wxLEFT);
  h_sizer1->Add(m_IsoSlider ,1,wxEXPAND);
  h_sizer1->Add(b_incr,0,wxLEFT);	

  wxBoxSizer *h_sizer2= new wxBoxSizer(wxHORIZONTAL);
  h_sizer2->Add(lab_slice,     0, wxLEFT);	
  h_sizer2->Add(text_slice,    0, wxLEFT);	
  h_sizer2->Add(b_decr_slice,  0, wxLEFT);
  h_sizer2->Add(m_SliceSlider, 1, wxEXPAND);
  h_sizer2->Add(b_incr_slice,  0, wxLEFT);	

  wxBoxSizer *h_sizer3 = new wxBoxSizer(wxHORIZONTAL);
//  h_sizer3->Add(foo,       1,wxEXPAND);	
  h_sizer3->Add(chk_interpolation,0,wxLEFT);
  h_sizer3->Add(chk_lod,   0,wxLEFT);
  h_sizer3->Add(chk_opt,   0,wxLEFT);
  h_sizer3->Add(chk_slice, 0,wxLEFT);
	h_sizer3->Add(chk_clean, 0,wxLEFT);
	h_sizer3->Add(chk_triangulate, 0,wxLEFT);
	h_sizer3->Add(chk_connect, 0, wxLEFT);
  //  h_sizer3->Add(b_grid,  0,wxRIGHT);
  
  wxBoxSizer *h_sizer4 = new wxBoxSizer(wxHORIZONTAL);
  h_sizer4->Add(chk_multi, 0,wxLEFT);
  h_sizer4->Add(text_num_of_contours, 0,wxLEFT);
  h_sizer4->Add(lab_multi_contours, 0,wxLEFT);
  h_sizer4->Add(text_min_range, 0,wxLEFT);
  h_sizer4->Add(text_max_range, 0,wxLEFT);
  
  wxBoxSizer *h_sizer5 = new wxBoxSizer(wxHORIZONTAL);
  h_sizer5->Add(foo,       1,wxEXPAND);	
  h_sizer5->Add(b_fit,     0,wxRIGHT);
  h_sizer5->Add(b_ok,      0,wxRIGHT);
  h_sizer5->Add(b_cancel,  0,wxRIGHT);

  wxBoxSizer *v_sizer =  new wxBoxSizer( wxVERTICAL );
  v_sizer->Add(m_Rwi->m_RwiBase, 1,wxEXPAND);
  if (h_sizer0 != NULL)
  {
	  v_sizer->Add(h_sizer0, 0,wxEXPAND | wxALL,5);
  }
  v_sizer->Add(h_sizer1,     0,wxEXPAND | wxALL,5);
  v_sizer->Add(h_sizer2,     0,wxEXPAND | wxALL,5);
  v_sizer->Add(h_sizer3,     0,wxEXPAND | wxALL,5);
  v_sizer->Add(h_sizer4,     0,wxEXPAND | wxALL,5);
	v_sizer->Add(new wxStaticLine(m_Dialog, -1/*, wxPoint(-1, -1), wxSize(panelWidth + 20, 1)*/), 0, wxEXPAND | wxALL, 5);
  v_sizer->Add(h_sizer5,     0,wxEXPAND | wxALL,5);

  m_Dialog->Add(v_sizer, 1, wxEXPAND);

  int x_pos,y_pos,w,h;
  albaGetFrame()->GetPosition(&x_pos,&y_pos);
  m_Dialog->GetSize(&w,&h);
  m_Dialog->SetSize(x_pos+5,y_pos+5,w,h);

  this->m_Rwi->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpExtractIsosurface::CreateVolumePipeline()
{
  vtkDataSet *dataset = m_Input->GetOutput()->GetVTKData();
  m_ContourVolumeMapper = vtkALBAContourVolumeMapper::New();
  m_ContourVolumeMapper->SetInput(dataset);
  m_ContourVolumeMapper->AutoLODRenderOn();
  m_ContourVolumeMapper->AutoLODCreateOn();

  double min = m_MinDensity;
  double max = m_MaxDensity;

  double range[2] = {0, 0};
  dataset->GetScalarRange(range);

  float value = 0.5f * (range[0] + range[1]);
  while (value < range[1] && m_ContourVolumeMapper->EstimateRelevantVolume(value) > 0.3f)
    value += 0.05f * (range[1] + range[0]) + 1.f;

  m_IsoValue=value;
  m_IsoValueVector.push_back(m_IsoValue);
  m_ContourVolumeMapper->SetContourValue(m_IsoValue);

  /*vtkPolyData *contour = vtkPolyData::New();
  m_ContourVolumeMapper->GetOutput(0, contour);

  m_ContourMapper = vtkPolyDataMapper::New();
  m_ContourMapper->SetInput(contour);
  m_ContourMapper->ScalarVisibilityOff();

  contour->Delete();*/

  if (!m_TestMode)
  {
    m_ContourActor = vtkVolume::New();
    m_ContourActor->SetMapper(m_ContourVolumeMapper);
    m_ContourActor->PickableOff();

    m_ContourVolumeMapper->Modified();
    m_ContourVolumeMapper->Update();
    m_Rwi->m_RenFront->AddActor(m_ContourActor);

    // bounding box actor
    m_OutlineFilter = vtkOutlineCornerFilter::New();
    m_OutlineFilter->SetInput(dataset);

    m_OutlineMapper = vtkPolyDataMapper::New();
    m_OutlineMapper->SetInput(m_OutlineFilter->GetOutput());

    m_Box = vtkActor::New();
    m_Box->SetMapper(m_OutlineMapper);
    m_Box->VisibilityOn();
    m_Box->PickableOff();
    m_Box->GetProperty()->SetColor(0,0,0.8);
    m_Box->GetProperty()->SetAmbient(1);
    m_Box->GetProperty()->SetRepresentationToWireframe();
    m_Box->GetProperty()->SetInterpolationToFlat();
    m_Rwi->m_RenFront->AddActor(m_Box);

    m_Box->GetBounds(m_BoundingBox);
    m_Rwi->SetGridPosition(m_BoundingBox[4]);
    m_Rwi->m_RenFront->ResetCamera(m_BoundingBox);
    m_Rwi->m_Camera->Dolly(1.2);
    m_Rwi->m_RenFront->ResetCameraClippingRange();
  }
}
//----------------------------------------------------------------------------
void albaOpExtractIsosurface::CreateSlicePipeline()
{
  // slicing the volume
  double srange[2],w,l, xspc = 0.33, yspc = 0.33, ext[6];

  vtkDataSet *dataset = m_Input->GetOutput()->GetVTKData();
  dataset->GetBounds(ext);
  dataset->GetScalarRange(srange);
  w = srange[1] - srange[0];
  l = (srange[1] + srange[0]) * 0.5;

  dataset->GetCenter(m_SliceOrigin);
  m_Slice = m_SliceOrigin[2];
  m_SliceXVect[0] = 1.0;
  m_SliceXVect[1] = 0.0;
  m_SliceXVect[2] = 0.0;
  m_SliceYVect[0] = 0.0;
  m_SliceYVect[1] = 1.0;
  m_SliceYVect[2] = 0.0;

  m_PolydataSlicer = vtkALBAVolumeSlicer::New();
  m_VolumeSlicer	= vtkALBAVolumeSlicer::New();
  m_VolumeSlicer->SetPlaneOrigin(m_SliceOrigin);
  m_PolydataSlicer->SetPlaneOrigin(m_VolumeSlicer->GetPlaneOrigin());
  m_VolumeSlicer->SetPlaneAxisX(m_SliceXVect);
  m_VolumeSlicer->SetPlaneAxisY(m_SliceYVect);
  m_PolydataSlicer->SetPlaneAxisX(m_SliceXVect);
  m_PolydataSlicer->SetPlaneAxisY(m_SliceYVect);
  m_VolumeSlicer->SetInput(dataset);
  m_PolydataSlicer->SetInput(dataset);
	
  m_SliceImage = vtkImageData::New();

  m_SliceImage->SetScalarType(dataset->GetPointData()->GetScalars()->GetDataType());
  m_SliceImage->SetNumberOfScalarComponents(dataset->GetPointData()->GetScalars()->GetNumberOfComponents());  
  //m_SliceImage->SetExtent(ext[0], ext[1], ext[2], ext[3], 0, 0);
  double textureRes=512;
  m_SliceImage->SetExtent(0, textureRes - 1, 0, textureRes - 1, 0, 0);
  m_SliceImage->SetSpacing(xspc, yspc, 1.f);

  m_VolumeSlicer->SetOutput(m_SliceImage);
  m_VolumeSlicer->Update();

  mmaVolumeMaterial *material = ((albaVMEVolume *)m_Input)->GetMaterial();
  double sr[2];
  ((albaVMEVolume*)m_Input)->GetOutput()->GetVTKData()->GetScalarRange(sr);
  material->m_ColorLut->SetRange(sr[0],sr[1]);
  material->UpdateFromTables();

  // if the lookup table has not yet been initialized...
  if (material->GetTableRange()[1] < material->GetTableRange()[0]) 
  {
    double scalarRange[2];

    dataset->GetScalarRange(scalarRange);

    double low = scalarRange[0];
    double high = scalarRange[1];

    // ...initialize it
    material->m_Window_LUT = high-low;
    material->m_Level_LUT  = (low+high)*.5;
		material->SetTableRange(low, high);
  }
  material->UpdateProp();

  m_SliceTexture = vtkTexture::New();
  m_SliceTexture->RepeatOff();
  m_SliceTexture->InterpolateOn();
  m_SliceTexture->SetQualityTo32Bit();
  m_SliceTexture->SetLookupTable(material->m_ColorLut);
  m_SliceTexture->MapColorScalarsThroughLookupTableOn();
  m_SliceTexture->SetInput(m_SliceImage);

  m_Polydata	= vtkPolyData::New();
  m_PolydataSlicer->SetOutput(m_Polydata);
  m_PolydataSlicer->SetTexture(m_SliceImage);
  m_PolydataSlicer->Update();

  m_SlicerMapper	= vtkPolyDataMapper::New();
  m_SlicerMapper->SetInput(m_Polydata);
  m_SlicerMapper->ScalarVisibilityOff();

  m_SlicerActor = vtkActor::New();
  m_SlicerActor->SetMapper(m_SlicerMapper);
  m_SlicerActor->SetTexture(m_SliceTexture);
  m_SlicerActor->GetProperty()->SetAmbient(1.f);
  m_SlicerActor->GetProperty()->SetDiffuse(0.f);

  m_PIPRen->AddActor(m_SlicerActor);

  vtkPolyData *contour = vtkPolyData::New();
  m_ContourVolumeMapper->GetOutput(0, contour);

  if(contour==NULL)
  {
    wxMessageBox("Operation out of memory");
    return;
  }

  m_CutterPlane = vtkPlane::New();
  m_CutterPlane->SetOrigin(m_SliceImage->GetOrigin());
  m_CutterPlane->SetNormal(0,0,1);

  m_IsosurfaceCutter = vtkALBAFixedCutter::New();
  m_IsosurfaceCutter->SetCutFunction(m_CutterPlane);  
  m_IsosurfaceCutter->SetInput(contour);
  m_IsosurfaceCutter->Update();

  contour->Delete();

  m_PolydataMapper	= vtkPolyDataMapper::New();
  m_PolydataMapper->SetInput(m_IsosurfaceCutter->GetOutput());
  m_PolydataMapper->ScalarVisibilityOff();

  m_PolydataActor = vtkActor::New();
  m_PolydataActor->SetMapper(m_PolydataMapper);
  m_PolydataActor->GetProperty()->SetColor(1,0,0);

  m_PIPRen->AddActor(m_PolydataActor);
  m_PIPRen->ResetCamera();
}
//----------------------------------------------------------------------------
void albaOpExtractIsosurface::DeleteOpDialog()
{
  m_Mouse->RemoveObserver(m_DensityPicker);

  m_Rwi->m_RenFront->RemoveActor(m_ContourActor);
  m_Rwi->m_RenFront->RemoveActor(m_Box);

  //vtkDEL(m_ContourMapper);
  vtkDEL(m_ContourActor);
  vtkDEL(m_Box);
  vtkDEL(m_OutlineFilter);
  vtkDEL(m_OutlineMapper);
  albaDEL(m_DensityPicker);

  m_PIPRen->RemoveActor(m_SlicerActor);
  m_PIPRen->RemoveActor(m_PolydataActor);
  m_Rwi->m_RenderWindow->RemoveRenderer(m_PIPRen);

  vtkDEL(m_PIPRen);
  vtkDEL(m_VolumeSlicer);
  vtkDEL(m_PolydataSlicer);
  vtkDEL(m_SliceImage);
  vtkDEL(m_SliceTexture);
  vtkDEL(m_Polydata);
  vtkDEL(m_SlicerMapper);
  vtkDEL(m_SlicerActor);
  vtkDEL(m_IsosurfaceCutter);
  vtkDEL(m_CutterPlane);
  vtkDEL(m_PolydataMapper);
  vtkDEL(m_PolydataActor);

  cppDEL(m_Rwi); 
  cppDEL(m_Dialog);
}

//----------------------------------------------------------------------------
void albaOpExtractIsosurface::OnEvent(albaEventBase *alba_event)
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {	
    case ID_OK:
      m_Dialog->EndModal(wxID_OK);
      break;
    case ID_CANCEL:
      m_Dialog->EndModal(wxID_CANCEL);
      break;
    case ID_FIT:
      m_Rwi->m_RenFront->ResetCamera(m_BoundingBox);
      m_Rwi->m_Camera->Dolly(1.2);
      m_Rwi->m_RenFront->ResetCameraClippingRange();
      m_PIPRen->ResetCamera();
      m_Rwi->m_RenderWindow->Render();
      break;
    case ID_GRID:
      //m_Rwi->SetGridVisibility(!m_Rwi->m_show_grid);
      m_Rwi->m_RenderWindow->Render();
      break;
    case ID_ISO:
      m_Dialog->TransferDataToWindow();
      m_IsoValue = m_IsoSlider->GetValue();
      m_Dialog->TransferDataToWindow();
      UpdateSurface();
      break;
    case ID_ISO_SLIDER:
      UpdateSurface();
      break;
    case ID_INCREASE_ISO:
      if(m_IsoValue<m_MaxDensity) m_IsoValue += m_StepDensity;
      m_Dialog->TransferDataToWindow();  
      UpdateSurface();
      break;
    case ID_DECREASE_ISO:
      if(m_IsoValue>m_MinDensity) m_IsoValue -= m_StepDensity;
      m_Dialog->TransferDataToWindow();  
      UpdateSurface();
      break;
    case ID_SLICE:
      m_Dialog->TransferDataToWindow();  
      m_Slice = m_SliceSlider->GetValue();
      m_Dialog->TransferDataToWindow();  
      UpdateSlice();
      break;
    case ID_SLICE_SLIDER:
      UpdateSlice();
      break;
    case ID_INCREASE_SLICE:
      if(m_Slice < m_SliceMax) 
        m_Slice += m_SliceStep;
      m_Dialog->TransferDataToWindow();  
      UpdateSlice();
      break;
    case ID_DECREASE_SLICE:
      if(m_Slice > m_SliceMin) 
        m_Slice -= m_SliceStep;
      m_Dialog->TransferDataToWindow();  
      UpdateSlice();
      break;
    case ID_VIEW_SLICE:
      if(this->m_ShowSlice)
      {
        vtkPolyData *contour = m_ContourVolumeMapper->GetOutput();
        if(contour == NULL)
        {
          wxMessageBox("Operation out of memory");
        }
        else
        {
          m_IsosurfaceCutter->SetInput(contour);
          m_IsosurfaceCutter->Update();
        }
        m_Rwi->m_RenderWindow->AddRenderer(m_PIPRen);
        contour->Delete() ; // NMcF
      }
      else
        m_Rwi->m_RenderWindow->RemoveRenderer(m_PIPRen);
      //m_Dialog->FindItem(ID_SLICE)->Enable(m_ShowSlice != 0);
      //m_Dialog->FindItem(ID_SLICE_SLIDER)->Enable(m_ShowSlice != 0);
      //m_Dialog->FindItem(ID_INCREASE_SLICE)->Enable(m_ShowSlice != 0);
      //m_Dialog->FindItem(ID_DECREASE_SLICE)->Enable(m_ShowSlice != 0);

      m_Rwi->m_RenderWindow->Render();
      break;
    case ID_AUTO_LOD:
      // toggle auto lod for polydata extraction
      if (m_ContourVolumeMapper->GetAutoLODCreate())
        m_ContourVolumeMapper->AutoLODCreateOff() ;
      else
        m_ContourVolumeMapper->AutoLODCreateOn() ;
      break ;
    case VME_PICKED:
      {
        vtkDataSet *vol = m_Input->GetOutput()->GetVTKData();
        double pos[3];
        vtkPoints *pts = NULL; 
        pts = (vtkPoints *)e->GetVtkObj();
        pts->GetPoint(0,pos);
        vol->SetUpdateExtentToWholeExtent();
        vol->Update();
        int pid = vol->FindPoint(pos);
        vtkDataArray *scalars = vol->GetPointData()->GetScalars();
        if (scalars && pid != -1)
        {
          scalars->GetTuple(pid,&m_IsoValue);
          OnEvent(&albaEvent(this,ID_ISO));
        }
        else
        {
          wxMessageBox("Invalid picked point!!", "Warning");
        }
      }
      break;
    case ID_MULTIPLE_CONTOURS:
      if (m_MultiContoursFlag == 0)
      {
        m_NumberOfContours = 1;
      }
      break;
    case ID_NUM_OF_CONTOURS:
      break;
    case ID_MIN_RANGE:
      if (m_MinRange > m_MaxRange)
      {
        m_MinRange = m_MaxRange;
        m_Dialog->TransferDataToWindow();
      }
      break;
    case ID_MAX_RANGE:
      if (m_MaxRange < m_MinRange)
      {
        m_MaxRange = m_MinRange;
        m_Dialog->TransferDataToWindow();
      }
      break;
    case ID_TRILINEAR_INTERPOLATION_ON:
      {
        m_VolumeSlicer->SetTrilinearInterpolation(m_TrilinearInterpolationOn == true);
        m_Rwi->CameraUpdate();
      }
    default:
      albaEventMacro(*e);
      break; 
    }
  }
}

//------------------------------------------------------------------------------
// Modify and extract the isosurface
// Called from various gui events in OnEvent()
void albaOpExtractIsosurface::UpdateSurface(bool use_lod)
{
  if (!m_TestMode)
  {
    m_Rwi->m_RenderWindow->SetDesiredUpdateRate(0.001f);
  }
  if (m_ContourVolumeMapper->GetContourValue() != m_IsoValue) 
  {
    m_ContourVolumeMapper->SetContourValue(m_IsoValue);
    m_IsoValueVector.clear();
    m_IsoValueVector.push_back(m_IsoValue);
    m_ContourVolumeMapper->Update();

    if (DEBUG_MODE)
    {
      albaLogMessage(m_Input->GetName());
      std::ostringstream stringStream;
      m_ContourVolumeMapper->Print(stringStream);
      albaLogMessage(stringStream.str().c_str());
    }
    
    if (!m_TestMode)
    {
      vtkPolyData *contour;/* = vtkPolyData::New();
      m_IsosurfaceCutter->SetInput(contour);
      m_IsosurfaceCutter->Update();*/
      //m_ContourMapper->SetInput(contour);
      if (m_ShowSlice)
      {
        contour = m_ContourVolumeMapper->GetOutput();
        if(contour == NULL)
        {
          m_Rwi->m_RenderWindow->Render();
          wxMessageBox("Operation out of memory");
          return;
        }
        m_IsosurfaceCutter->SetInput(contour);
        m_IsosurfaceCutter->Update();

        contour->Delete();
      }
    }
  }

  if (!m_TestMode)
  {
    m_Rwi->m_RenderWindow->Render();
  }
}

//----------------------------------------------------------------------------
void albaOpExtractIsosurface::UpdateSlice()
{
  m_SliceOrigin[2] = m_Slice;
  m_VolumeSlicer->SetPlaneOrigin(m_SliceOrigin[0], m_SliceOrigin[1], m_SliceOrigin[2]);
  m_PolydataSlicer->SetPlaneOrigin(m_VolumeSlicer->GetPlaneOrigin());
  m_VolumeSlicer->SetPlaneAxisX(m_SliceXVect);
  m_VolumeSlicer->SetPlaneAxisY(m_SliceYVect);
  m_PolydataSlicer->SetPlaneAxisX(m_SliceXVect);
  m_PolydataSlicer->SetPlaneAxisY(m_SliceYVect);

  m_VolumeSlicer->Update();
  m_PolydataSlicer->Update();

  m_CutterPlane->SetOrigin(m_VolumeSlicer->GetPlaneOrigin());
  m_IsosurfaceCutter->Update();

  this->m_PIPRen->ResetCameraClippingRange();
  m_Rwi->m_RenderWindow->SetDesiredUpdateRate(0.001f);
  m_Rwi->m_RenderWindow->Render();
}

//------------------------------------------------------------------------------
// Extract the surface for the first time and create the isosurface VME
// Called from OpRun()
void albaOpExtractIsosurface::ExtractSurface()
{
	if (!m_TestMode)
	{
		wxBusyInfo wait(_("Extracting Isosurface: please wait ..."));
	}

	m_ContourVolumeMapper->SetEnableContourAnalysis(m_Optimize != 0);

	if (m_NumberOfContours > 1)
	{
		albaNEW(m_OutputGroup);
		m_OutputGroup->SetName("Extract isosurface output");
	}

	// IMPORTANT, extract the isosurface from m_ContourVolumeMapper in this way
	// and then call surface->Delete() when the VME is created
	int divisor = m_NumberOfContours - 1;
	divisor = divisor == 0 ? 1 : divisor;
	double step = (m_MaxRange - m_MinRange) / divisor;

	for (int contour = 0; contour < m_NumberOfContours; contour++)
	{
		if (m_MultiContoursFlag != 0)
		{
			m_IsoValue = m_MinRange + step * contour;
			m_IsoValueVector.push_back(m_IsoValue);
			m_ContourVolumeMapper->SetContourValue(m_IsoValue);
			m_ContourVolumeMapper->Update();
		}

		vtkPolyData *surface;
		surface = m_ContourVolumeMapper->GetOutput();
		vtkALBASmartPointer<vtkCleanPolyData>clearFilter;
		vtkALBASmartPointer<vtkTriangleFilter>triangleFilter;

		if (m_Clean)
		{
			clearFilter->SetInput(surface);
			surface->Delete();
			clearFilter->ConvertLinesToPointsOff();
			clearFilter->ConvertPolysToLinesOff();
			clearFilter->ConvertStripsToPolysOff();
			//clearFilter->PointMergingOff();
			clearFilter->Update();
			surface = clearFilter->GetOutput();
		}

		if (m_Triangulate)
		{
			triangleFilter->SetInput(surface);
			if (!m_Clean)
				surface->Delete();
			triangleFilter->Update();
			surface = triangleFilter->GetOutput();
		}

		if (m_Connectivity)
		{
			vtkALBASmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter;
			connectivityFilter->SetInput(surface);
			connectivityFilter->Update();

			surface->DeepCopy((vtkPolyData*)(connectivityFilter->GetOutput()));
		}

		if (surface == NULL)
		{
			wxMessageBox(_("Operation out of memory"));
			return;
		}
		m_ContourVolumeMapper->Update();

		wxString name = albaString::Format("%s Isosurface %g", m_Input->GetName(), m_IsoValue);

		albaVMESurface *vme_surf;
		albaNEW(vme_surf);
		vme_surf->SetName(name.ToAscii());
		//vme_surf->SetDataByDetaching(surface,0);
		vme_surf->SetData(surface, 0);
		vme_surf->GetOutput()->Update();
		if (!m_Clean && !m_Triangulate) {
			surface->Delete();
		}

		if (m_OutputGroup != NULL)
		{
			vme_surf->ReparentTo(m_OutputGroup);
		}
		else
		{
			m_Output = vme_surf;
		}
	}

	if (m_OutputGroup != NULL)
	{
		m_Output = m_OutputGroup;
	}
}
//----------------------------------------------------------------------------
albaString albaOpExtractIsosurface::GetParameters()
{
  wxString parameter;
  for (int contour = 0; contour < m_NumberOfContours; contour++)
  {
    parameter.Append("Contour value = ");
    parameter.Append(albaString::Format("%f", m_IsoValueVector[contour]));
    parameter.Append(", ");
  }
  parameter.RemoveLast(2);

  return parameter;
}

//----------------------------------------------------------------------------
void albaOpExtractIsosurface::SetIsoValue(double isoValue)
{
  vtkDataSet *dataset = m_Input->GetOutput()->GetVTKData();
  double sr[2];
  dataset->GetScalarRange(sr);
  m_MinDensity = sr[0];
  m_MaxDensity = sr[1];

  if (isoValue >= m_MinDensity && isoValue <= m_MaxDensity)
  {
    m_IsoValue = isoValue;
  }
}
//----------------------------------------------------------------------------
void albaOpExtractIsosurface::SetTriangulate( bool triangulate )
{
  m_Triangulate = triangulate;
}
//----------------------------------------------------------------------------
void albaOpExtractIsosurface::SetClean( bool clean )
{
  m_Clean = clean;
}
//----------------------------------------------------------------------------
void albaOpExtractIsosurface::SetConnectivity(bool connectivity)
{
	m_Connectivity = connectivity;
}
