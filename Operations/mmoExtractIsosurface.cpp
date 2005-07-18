/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoExtractIsosurface.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-18 10:12:58 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani     Silvano Imboden
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

#include "mmoExtractIsosurface.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mmgGui.h"

#include "mmgDialog.h"

#include "mafRWIBase.h"
#include "mafRWI.h"
#include "mmdMouse.h"

#include "mmgButton.h"
#include "mmgValidator.h"
#include "mmgPicButton.h"
#include "mmgFloatSlider.h"

#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMEOutput.h"
#include "mmiPicker.h"

#include "vtkLight.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "mafVMEVolumeGray.h"

#include "vtkMath.h"
#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkVolume.h"
#include "vtkContourVolumeMapper.h"
#include "vtkPolyDataMapper.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkInteractorStyleTrackballCamera.h" 
#include "vtkPolyData.h"
#include "vtkFixedCutter.h"
#include "vtkPlane.h"
#include "vtkImageData.h"
#include "vtkTexture.h"
#include "vtkVolumeSlicer.h"

//----------------------------------------------------------------------------
mmoExtractIsosurface::mmoExtractIsosurface(wxString label) :
mafOp(label), m_IsosurfaceVme(NULL)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;
  
  m_IsoSlider   = NULL;
  m_SliceSlider = NULL;
  
  m_Dialog = NULL;
	m_Rwi = NULL;
	
  m_IsoValue = 0;
	m_min = 0;
	m_max = 0;
	
  m_Slice     = 0;
	m_SliceMin  = 0;
	m_SliceMax  = 0;
  m_ShowSlice = 1;
  m_Clean     = 1;
  
  for(int i=0; i<6; i++) m_bbox[i]=0;

  v_volume = NULL;
  v_box    = NULL;

  v_contour_mapper  = NULL; 
  v_outline_filter  = NULL;
  v_outline_mapper  = NULL;

  m_DensityPicker   = NULL;
}
//----------------------------------------------------------------------------
mmoExtractIsosurface::~mmoExtractIsosurface()
//----------------------------------------------------------------------------
{
  vtkDEL(m_IsosurfaceVme);
}
//----------------------------------------------------------------------------
mafOp* mmoExtractIsosurface::Copy()
//----------------------------------------------------------------------------
{
  /** return a copy of itself, needs to put it into the undo stack */
  return new mmoExtractIsosurface(m_Label);
}
//----------------------------------------------------------------------------
bool mmoExtractIsosurface::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  return vme != NULL && vme->IsMAFType(mafVMEVolumeGray);
}
//----------------------------------------------------------------------------
void mmoExtractIsosurface::OpRun()
//----------------------------------------------------------------------------
{
  int result = OP_RUN_CANCEL;

  CreateOpDialog();
  int ret_dlg = m_Dialog->ShowModal();
  if( ret_dlg == wxID_OK )
  {
		 result = OP_RUN_OK;
  	 ExtractSurface(m_Clean != 0);
  }
  DeleteOpDialog();

	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mmoExtractIsosurface::OpDo()
//----------------------------------------------------------------------------
{
  m_IsosurfaceVme->ReparentTo(m_Input);
	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mmoExtractIsosurface::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_IsosurfaceVme);
  mafEventMacro(mafEvent(this,VME_REMOVE,m_IsosurfaceVme));
	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
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
  ID_OK,
  ID_CANCEL,
};
//----------------------------------------------------------------------------
void mmoExtractIsosurface::CreateOpDialog()
//----------------------------------------------------------------------------
{
  vtkDataSet *dataset = ((mafVME *)m_Input)->GetOutput()->GetVTKData();
  double sr[2];
	dataset->GetScalarRange(sr);
  m_min = sr[0];
  m_max = sr[1];
  m_IsoValue = (m_min + m_max)*0.5;

  double b[6];
  dataset->GetBounds(b);
  m_SliceMin = b[4];
  m_SliceMax = b[5];
  m_Slice = (m_SliceMin + m_SliceMax)*0.5;
	
	
  //===== setup interface ====
  m_Dialog = new mmgDialog("Extract Isosurface", mafCLOSEWINDOW | mafRESIZABLE);

  m_PIPRen = vtkRenderer::New();

  m_Rwi = new mafRWI(m_Dialog,ONE_LAYER,true);
  m_Rwi->SetListener(this);//SIL. 16-6-2004: 
	m_Rwi->CameraSet(CAMERA_PERSPECTIVE);
	//m_Rwi->SetAxesVisibility(true);
  //m_Rwi->SetGridVisibility(false);
	m_Rwi->SetSize(0,0,500,500);
	m_Rwi->Show(true);
  m_Rwi->m_RwiBase->SetMouse(m_Mouse);
  mafNEW(m_DensityPicker);
  m_DensityPicker->SetListener(this);
  m_Mouse->AddObserver(m_DensityPicker, MCH_INPUT);
  m_Rwi->m_RenderWindow->AddRenderer(m_PIPRen);
  m_PIPRen->SetViewport(.6, .01, .99, .4);
  m_PIPRen->GetActiveCamera()->SetFocalPoint(0,0,0);
  m_PIPRen->GetActiveCamera()->SetPosition(0,0,-70);
  m_PIPRen->GetActiveCamera()->SetViewUp(0,-1,0);
  m_PIPRen->GetActiveCamera()->ParallelProjectionOn();

  wxPoint p = wxDefaultPosition;
  // iso interface
  wxStaticText *lab  = new wxStaticText(m_Dialog,-1, "contur value: ");
	wxStaticText *foo  = new wxStaticText(m_Dialog,-1, "");
	wxTextCtrl   *text = new wxTextCtrl  (m_Dialog,ID_ISO, "",								 		p,wxSize(50, 16 ), wxNO_BORDER );
  m_IsoSlider				 = new mmgFloatSlider(m_Dialog,ID_ISO_SLIDER,m_IsoValue,m_min,m_max,p,wxSize(150,20));
	m_IsoSlider->SetNumberOfSteps(200);
	m_step = m_IsoSlider->GetStep();
  //m_min  = m_IsoSlider->GetMin();
  //m_max  = m_IsoSlider->GetMax();
  m_IsoValue  = m_IsoSlider->GetValue();

  // slice interface
  wxStaticText *lab_slice  = new wxStaticText(m_Dialog,-1, "slice position: ");
	wxStaticText *foo_slice  = new wxStaticText(m_Dialog,-1, "");
	wxTextCtrl   *text_slice = new wxTextCtrl  (m_Dialog,ID_SLICE, "",							 		p,wxSize(50, 16 ), wxNO_BORDER );
  m_SliceSlider						 = new mmgFloatSlider(m_Dialog,ID_SLICE_SLIDER,m_Slice,m_SliceMin,m_SliceMax,p,wxSize(150,20));
	m_SliceSlider->SetNumberOfSteps(200);
	m_SliceStep = m_SliceSlider->GetStep();
  m_SliceMin  = m_SliceSlider->GetMin();
  m_SliceMax  = m_SliceSlider->GetMax();
  m_Slice     = m_SliceSlider->GetValue();

	mmgButton *b_incr_slice  = new mmgButton(m_Dialog,ID_INCREASE_SLICE, ">",		 p,wxSize(25, 20));
  mmgButton *b_decr_slice  = new mmgButton(m_Dialog,ID_DECREASE_SLICE, "<",		 p,wxSize(25, 20));
	mmgButton *b_incr  = new mmgButton(m_Dialog,ID_INCREASE_ISO, ">",				 		 p,wxSize(25, 20));
  mmgButton *b_decr  = new mmgButton(m_Dialog,ID_DECREASE_ISO, "<",				 		 p,wxSize(25, 20));
  
  wxCheckBox *chk     = new wxCheckBox(m_Dialog,ID_VIEW_SLICE,  "slice",         p,wxSize(80,20));
  wxCheckBox *chk_opt = new wxCheckBox(m_Dialog,ID_OPTIMIZE_CONTOUR, "optimize", p,wxSize(80,20));
  mmgButton  *b_fit   = new mmgButton(m_Dialog,ID_FIT,          "reset camera",  p,wxSize(80,20));
//  wxCheckBox *b_grid  = new wxCheckBox(m_Dialog,ID_GRID,         "show/hide grid",p,wxSize(80,20));
  mmgButton  *b_ok      = new mmgButton(m_Dialog,ID_OK,         "ok",p,wxSize(80,20));
  mmgButton  *b_cancel  = new mmgButton(m_Dialog,ID_CANCEL,     "cancel",p,wxSize(80,20));
	
	// iso interface validator
  text->SetValidator(mmgValidator(this,ID_ISO,text,&m_IsoValue,m_min,m_max));
	m_IsoSlider->SetValidator(mmgValidator(this,ID_ISO_SLIDER,m_IsoSlider,&m_IsoValue,text));
  b_incr->SetValidator(mmgValidator(this,ID_INCREASE_ISO,b_incr));
  b_decr->SetValidator(mmgValidator(this,ID_DECREASE_ISO,b_decr));
  
	// slice interface validator
  text_slice->SetValidator(mmgValidator(this,ID_SLICE,text_slice,&m_Slice,m_SliceMin,m_SliceMax));
	m_SliceSlider->SetValidator(mmgValidator(this,ID_SLICE_SLIDER,m_SliceSlider,&m_Slice,text_slice));
  b_incr_slice->SetValidator(mmgValidator(this,ID_INCREASE_SLICE,b_incr_slice));
  b_decr_slice->SetValidator(mmgValidator(this,ID_DECREASE_SLICE,b_decr_slice));
  
  b_fit->SetValidator(mmgValidator(this,ID_FIT,b_fit));
  b_ok->SetValidator(mmgValidator(this,ID_OK,b_ok));
  b_cancel->SetValidator(mmgValidator(this,ID_CANCEL,b_cancel));
//  b_grid->SetValidator(mmgValidator(this,ID_GRID,b_grid));
  chk->SetValidator( mmgValidator(this,ID_VIEW_SLICE,chk,&m_ShowSlice));
  chk_opt->SetValidator(mmgValidator(this,ID_OPTIMIZE_CONTOUR,chk_opt,&m_Clean));

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
  h_sizer3->Add(foo,     1,wxEXPAND);	
  h_sizer3->Add(chk_opt, 0,wxRIGHT);
  h_sizer3->Add(chk,     0,wxRIGHT);
//  h_sizer3->Add(b_grid,  0,wxRIGHT);
  h_sizer3->Add(b_fit,   0,wxRIGHT);
  h_sizer3->Add(b_ok,   0,wxRIGHT);
  h_sizer3->Add(b_cancel, 0,wxRIGHT);

  wxBoxSizer *v_sizer =  new wxBoxSizer( wxVERTICAL );
  v_sizer->Add(m_Rwi->m_RwiBase, 1,wxEXPAND);
  v_sizer->Add(h_sizer1,     0,wxEXPAND | wxALL,5);
  v_sizer->Add(h_sizer2,     0,wxEXPAND | wxALL,5);
  v_sizer->Add(h_sizer3,     0,wxEXPAND | wxALL,5);

  m_Dialog->Add(v_sizer, 1, wxEXPAND);

  int x_pos,y_pos,w,h;
  mafGetFrame()->GetPosition(&x_pos,&y_pos);
  m_Dialog->GetSize(&w,&h);
  m_Dialog->SetSize(x_pos+5,y_pos+5,w,h);

  CreateVolumePipeline();
  CreateSlicePipeline();
}
//----------------------------------------------------------------------------
void mmoExtractIsosurface::CreateVolumePipeline()
//----------------------------------------------------------------------------
{
  mafVMEVolumeGray *vol_vme = mafVMEVolumeGray::SafeDownCast(m_Input);
  vtkDataSet *dataset = vol_vme->GetVolumeOutput()->GetVTKData();
	v_contour_mapper = vtkContourVolumeMapper::New();
	v_contour_mapper->SetInput(dataset);

//  v_volume = vtkVolume::New();
//	v_volume->SetMapper(v_contour_mapper);
//	m_Rwi->m_RenFront->AddActor(v_volume);

	double min = m_min;
	double max = m_max;

//	while (m_IsoValue < max && v_contour_mapper->EstimateRelevantVolume(m_IsoValue) > 0.3f)
//		m_IsoValue += 0.05f * (min + max) + 1.f;
	v_contour_mapper->SetContourValue(m_IsoValue);

  vtkPolyData *contour = vtkPolyData::New();
  v_contour_mapper->GetOutput(0, contour);

  m_contour_volume_mapper = vtkPolyDataMapper::New();
  m_contour_volume_mapper->SetInput(contour);
  m_contour_volume_mapper->ScalarVisibilityOff();

  contour->Delete();

  m_contour_volume_actor = vtkActor::New();
  m_contour_volume_actor->SetMapper(m_contour_volume_mapper);

  m_Rwi->m_RenFront->AddActor(m_contour_volume_actor);

  // bounding box actor
	v_outline_filter = vtkOutlineCornerFilter::New();
	v_outline_filter->SetInput(dataset);

	v_outline_mapper = vtkPolyDataMapper::New();
	v_outline_mapper->SetInput(v_outline_filter->GetOutput());

	v_box = vtkActor::New();
	v_box->SetMapper(v_outline_mapper);
	v_box->VisibilityOn();
	v_box->PickableOff();
	v_box->GetProperty()->SetColor(0,0,0.8);
	v_box->GetProperty()->SetAmbient(1);
	v_box->GetProperty()->SetRepresentationToWireframe();
	v_box->GetProperty()->SetInterpolationToFlat();
	m_Rwi->m_RenFront->AddActor(v_box);

	v_box->GetBounds(m_bbox);
	m_Rwi->SetGridPosition(m_bbox[4]);
	m_Rwi->m_RenFront->ResetCamera(m_bbox);
	m_Rwi->m_Camera->Dolly(1.2);
	m_Rwi->m_RenFront->ResetCameraClippingRange();

}
//----------------------------------------------------------------------------
void mmoExtractIsosurface::CreateSlicePipeline()
//----------------------------------------------------------------------------
{
  // slicing the volume
 	double srange[2],w,l, xspc = 0.33, yspc = 0.33, ext[6];

  vtkDataSet *dataset = ((mafVME *)m_Input)->GetOutput()->GetVTKData();
  dataset->GetBounds(ext);
	dataset->GetScalarRange(srange);
	w = srange[1] - srange[0];
	l = (srange[1] + srange[0]) * 0.5;

  dataset->GetCenter(m_origin);
  m_Slice = m_origin[2];
  m_xVect[0] = 1.0;
  m_xVect[1] = 0.0;
  m_xVect[2] = 0.0;
  m_yVect[0] = 0.0;
  m_yVect[1] = 1.0;
  m_yVect[2] = 0.0;

	m_pslicer = vtkVolumeSlicer::New();
	m_slicer	= vtkVolumeSlicer::New();
	m_slicer->SetPlaneOrigin(m_origin);
	m_pslicer->SetPlaneOrigin(m_slicer->GetPlaneOrigin());
	m_slicer->SetPlaneAxisX(m_xVect);
	m_slicer->SetPlaneAxisY(m_yVect);
	m_pslicer->SetPlaneAxisX(m_xVect);
	m_pslicer->SetPlaneAxisY(m_yVect);
	m_slicer->SetInput(dataset);
	m_pslicer->SetInput(dataset);

	m_image_slice = vtkImageData::New();
  m_image_slice->SetScalarTypeToUnsignedChar();
	m_image_slice->SetNumberOfScalarComponents(3);
  m_image_slice->SetExtent(ext[0], ext[1], ext[2], ext[3], 0, 0);
	m_image_slice->SetSpacing(xspc, yspc, 1.f);

  m_slicer->SetOutput(m_image_slice);
	m_slicer->SetWindow(w);
	m_slicer->SetLevel(l);
	m_slicer->Update();

  m_texture = vtkTexture::New();
	m_texture->RepeatOff();
	m_texture->InterpolateOn();
	m_texture->SetQualityTo32Bit();
	m_texture->SetInput(m_image_slice);

  m_polydata	= vtkPolyData::New();
	m_pslicer->SetOutput(m_polydata);
	m_pslicer->SetTexture(m_image_slice);
	m_pslicer->Update();
	
  m_smapper	= vtkPolyDataMapper::New();
	m_smapper->SetInput(m_polydata);
	m_smapper->ScalarVisibilityOff();

	m_actorSlice = vtkActor::New();
	m_actorSlice->SetMapper(m_smapper);
	m_actorSlice->SetTexture(m_texture);
	m_actorSlice->GetProperty()->SetAmbient(1.f);
	m_actorSlice->GetProperty()->SetDiffuse(0.f);

  m_PIPRen->AddActor(m_actorSlice);

  vtkPolyData *contour = vtkPolyData::New();
  v_contour_mapper->GetOutput(0, contour);

  m_plane = vtkPlane::New();
  m_plane->SetOrigin(m_image_slice->GetOrigin());
  m_plane->SetNormal(0,0,1);
  
  m_cutter = vtkFixedCutter::New();
  m_cutter->SetCutFunction(m_plane);
  m_cutter->SetInput(contour);
  m_cutter->Update();

  contour->Delete();

  m_mapper	= vtkPolyDataMapper::New();
  m_mapper->SetInput(m_cutter->GetOutput());
  m_mapper->ScalarVisibilityOff();

  m_actor = vtkActor::New();
  m_actor->SetMapper(m_mapper);
	m_actor->GetProperty()->SetColor(1,0,0);
  
  m_PIPRen->AddActor(m_actor);
  m_PIPRen->ResetCamera();
}
//----------------------------------------------------------------------------
void mmoExtractIsosurface::DeleteOpDialog()
//----------------------------------------------------------------------------
{
	m_Rwi->m_RenFront->GetVolumes();
	m_Rwi->m_RenFront->RemoveActor(v_volume);
  m_Rwi->m_RenFront->RemoveActor(v_box);

  //vtkDEL(v_volume);
  vtkDEL(m_contour_volume_mapper);
  vtkDEL(m_contour_volume_actor);
  vtkDEL(v_box);
  vtkDEL(v_contour_mapper);
  vtkDEL(v_outline_filter);
  vtkDEL(v_outline_mapper);
  mafDEL(m_DensityPicker);

  m_PIPRen->RemoveActor(m_actorSlice);
  m_PIPRen->RemoveActor(m_actor);

  vtkDEL(m_PIPRen);
  vtkDEL(m_slicer);
  vtkDEL(m_pslicer);
  vtkDEL(m_image_slice);
  vtkDEL(m_texture);
  vtkDEL(m_polydata);
  vtkDEL(m_smapper);
  vtkDEL(m_actorSlice);
  vtkDEL(m_cutter);
  vtkDEL(m_plane);
  vtkDEL(m_mapper);
  vtkDEL(m_actor);

  cppDEL(m_Rwi); 
  cppDEL(m_Dialog);
}
//----------------------------------------------------------------------------
void mmoExtractIsosurface::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
        m_Rwi->m_RenFront->ResetCamera(m_bbox);
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
        UpdateSurface(true);
      break;
      case ID_INCREASE_ISO:
        if(m_IsoValue<m_max) m_IsoValue += m_step;
        m_Dialog->TransferDataToWindow();  
        UpdateSurface();
      break;
      case ID_DECREASE_ISO:
        if(m_IsoValue>m_min) m_IsoValue -= m_step;
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
        if(m_Slice<m_SliceMax) m_Slice += m_SliceStep;
        m_Dialog->TransferDataToWindow();  
        UpdateSlice();
      break;
      case ID_DECREASE_SLICE:
        if(m_Slice>m_SliceMin) m_Slice -= m_SliceStep;
        m_Dialog->TransferDataToWindow();  
        UpdateSlice();
      break;
      case ID_VIEW_SLICE:
        if(this->m_ShowSlice)
          m_Rwi->m_RenderWindow->AddRenderer(m_PIPRen);
        else
          m_Rwi->m_RenderWindow->RemoveRenderer(m_PIPRen);
        m_Dialog->FindItem(ID_SLICE)->Enable(m_ShowSlice != 0);
        m_Dialog->FindItem(ID_SLICE_SLIDER)->Enable(m_ShowSlice != 0);
        m_Dialog->FindItem(ID_INCREASE_SLICE)->Enable(m_ShowSlice != 0);
        m_Dialog->FindItem(ID_DECREASE_SLICE)->Enable(m_ShowSlice != 0);

        m_Rwi->m_RenderWindow->Render();
      break;
      case VME_PICKED:
      {
        vtkDataSet *vol = ((mafVME *)m_Input)->GetOutput()->GetVTKData();
        double pos[3];
        vtkPoints *pts = NULL; 
        pts = (vtkPoints *)e->GetVtkObj();
        pts->GetPoint(0,pos);
        int pid = vol->FindPoint(pos);
        vtkDataArray *scalars = vol->GetPointData()->GetScalars();
        scalars->GetTuple(pid,&m_IsoValue);
        OnEvent(&mafEvent(this,ID_ISO));
      }
      break;
      default:
        mafEventMacro(*e);
      break; 
    }
  }
}
//----------------------------------------------------------------------------
void mmoExtractIsosurface::UpdateSurface(bool use_lod)
//----------------------------------------------------------------------------
{
  m_Rwi->m_RenderWindow->SetDesiredUpdateRate(0.001f);
  if (v_contour_mapper->GetContourValue() != m_IsoValue) 
	{
    v_contour_mapper->SetContourValue(m_IsoValue);
    v_contour_mapper->Update();
    vtkPolyData *contour = vtkPolyData::New();
    v_contour_mapper->GetOutput(0, contour);
    m_contour_volume_mapper->SetInput(contour);
    if (m_ShowSlice)
    {
      m_cutter->SetInput(contour);
      m_cutter->Update();
    }
    contour->Delete();
    if(use_lod) m_Rwi->m_RenderWindow->SetDesiredUpdateRate(15.0f);
  }

  m_Rwi->m_RenderWindow->Render();
  m_Rwi->m_RenderWindow->SetDesiredUpdateRate(15.f);
}
//----------------------------------------------------------------------------
void mmoExtractIsosurface::UpdateSlice()
//----------------------------------------------------------------------------
{
  m_origin[2] = m_Slice;
  m_slicer->SetPlaneOrigin(m_origin[0], m_origin[1], m_origin[2]);
  m_pslicer->SetPlaneOrigin(m_slicer->GetPlaneOrigin());
  m_slicer->SetPlaneAxisX(m_xVect);
  m_slicer->SetPlaneAxisY(m_yVect);
  m_pslicer->SetPlaneAxisX(m_xVect);
  m_pslicer->SetPlaneAxisY(m_yVect);

  m_slicer->Update();
  m_pslicer->Update();

  m_plane->SetOrigin(m_slicer->GetPlaneOrigin());
  m_cutter->Update();

  this->m_PIPRen->ResetCameraClippingRange();
  m_Rwi->m_RenderWindow->Render();
  m_Rwi->m_RenderWindow->SetDesiredUpdateRate(15.f);
}
//----------------------------------------------------------------------------
void mmoExtractIsosurface::ExtractSurface(bool clean) 
//----------------------------------------------------------------------------
{
  wxBusyInfo wait("Extracting Isosurface: please wait ...");
  
	v_contour_mapper->SetEnableContourAnalysis(clean);
  
	// IMPORTANT, extract the isosurface from v_contour_mapper in this way
	// and then call surface->Delete() when the VME is created
	vtkPolyData *surface = vtkPolyData::New();
	v_contour_mapper->GetOutput(0, surface);
  v_contour_mapper->Update();

  wxString name = wxString::Format( "%s Isosurface %g", m_Input->GetName(),m_IsoValue );

  mafNEW(m_IsosurfaceVme);
  m_IsosurfaceVme->SetName(name.c_str());
  m_IsosurfaceVme->SetDataByDetaching(surface,0);

  surface->Delete(); 
}
