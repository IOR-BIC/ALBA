/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpClipSurface
 Authors: Paolo Quadrani
 
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


#include "albaOpClipSurface.h"
#include "wx/busyinfo.h"

#include "albaGUI.h"
#include "albaRefSys.h"

#include "mmaMaterial.h"
#include "albaInteractorCompositorMouse.h"

#include "albaVMEGizmo.h"
#include "albaGizmoTranslate.h"
#include "albaGizmoRotate.h"
#include "albaGizmoScale.h"
#include "albaVMESurfaceParametric.h"

#include "vtkProperty.h"
#include "vtkTransform.h"
#include "vtkPolyData.h"
#include "vtkPlane.h"
#include "vtkPlaneSource.h"
#include "vtkClipPolyData.h"
#include "vtkALBAImplicitPolyData.h"
#include "vtkLinearSubdivisionFilter.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkArrowSource.h"
#include "vtkGlyph3D.h"
#include "vtkAppendPolyData.h"
#include "vtkALBAClipSurfaceBoundingBox.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpClipSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpClipSurface::albaOpClipSurface(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

  m_ClipperVME    = NULL;
	m_ClippedVME = NULL;
	m_ReverseClippedVME    = NULL;
  m_ClipperPlane  = NULL;
  m_Arrow         = NULL;
  m_Clipper       = NULL;
	m_ClipperBoundingBox = NULL;
  
  m_ImplicitPlaneGizmo  = NULL;
  m_IsaCompositor       = NULL;

	m_ResultPolyData = NULL;
	m_ClippedPolyData = NULL;
  
  m_ClipInside      = 0;
	m_UseGizmo			=	1;
	m_ClipBoundBox	= false;
  m_ClipModality  = MODE_IMPLICIT_FUNCTION;
	m_GizmoType			= GIZMO_TRANSLATE;

	m_GeometryModality = 0;
  
  m_PlaneCreated = false;

	m_PlaneHeight = 0.0;
	m_PlaneWidth	= 0.0;

	m_PlaneSource	= NULL;
	m_Gizmo				= NULL;
	m_ArrowShape	= NULL;

	m_GizmoTranslate	= NULL;
	m_GizmoRotate			= NULL;
	m_GizmoScale			= NULL;

	m_GenerateClippedOutput = true;
}

//----------------------------------------------------------------------------
albaOpClipSurface::~albaOpClipSurface()
//----------------------------------------------------------------------------
{
	//cppDEL(m_GizmoTranslate);
	//cppDEL(m_GizmoRotate);
	//cppDEL(m_GizmoScale);

	vtkDEL(m_ResultPolyData);
	vtkDEL(m_ClippedPolyData);
  albaDEL(m_IsaCompositor);
	albaDEL(m_ReverseClippedVME);
	albaDEL(m_ClippedVME);
  vtkDEL(m_ClipperPlane);
  vtkDEL(m_Clipper);
	vtkDEL(m_ClipperBoundingBox);
  vtkDEL(m_Arrow);
}
//----------------------------------------------------------------------------
albaOp* albaOpClipSurface::Copy()   
//----------------------------------------------------------------------------
{
	return new albaOpClipSurface(m_Label);
}

//----------------------------------------------------------------------------
bool albaOpClipSurface::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
	return (node != NULL && (node->IsALBAType(albaVMESurface)||node->IsALBAType(albaVMESurfaceParametric)));
}

//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum CLIP_SURFACE_ID
{
  ID_CHOOSE_SURFACE = MINID,
  ID_CLIP_BY,
	ID_GEOMETRY_MOD,
  ID_CLIP_INSIDE,
	ID_CLIP_PLANE_BBOX,
	ID_GENERATE_CLIPPED_OUTPUT,
	ID_PLANE_WIDTH,
	ID_PLANE_HEIGHT,
	ID_CHOOSE_GIZMO,
	ID_USE_GIZMO,
};

//----------------------------------------------------------------------------
void albaOpClipSurface::OpRun()   
//----------------------------------------------------------------------------
{
  vtkNEW(m_Clipper);
	vtkNEW(m_ClipperBoundingBox);

 
	vtkNEW(m_ClippedPolyData);
	vtkNEW(m_ResultPolyData);
	m_ResultPolyData->DeepCopy((vtkPolyData*)m_Input->GetOutput()->GetVTKData());

	if(!m_TestMode)
	{
		CreateGui();
		ShowGui();
	  
		ShowClipPlane(m_ClipModality != albaOpClipSurface::MODE_SURFACE);
		CreateGizmos();
		//Enable & show Gizmos
		ChangeGizmo();
	}
}
//----------------------------------------------------------------------------
void albaOpClipSurface::CreateGizmos()
//----------------------------------------------------------------------------
{
	m_ImplicitPlaneGizmo->GetOutput()->GetVTKData()->ComputeBounds();
	m_ImplicitPlaneGizmo->Modified();
	m_ImplicitPlaneGizmo->Update();

	m_GizmoTranslate = new albaGizmoTranslate(m_ImplicitPlaneGizmo, this);
	m_GizmoTranslate->SetRefSys(m_ImplicitPlaneGizmo);
	m_GizmoTranslate->Show(false);
	m_GizmoRotate = new albaGizmoRotate(m_ImplicitPlaneGizmo, this);
	m_GizmoRotate->SetRefSys(m_ImplicitPlaneGizmo);
	m_GizmoRotate->Show(false);
	m_GizmoScale = new albaGizmoScale(m_ImplicitPlaneGizmo, this);
	m_GizmoScale->SetRefSys(m_ImplicitPlaneGizmo);
	m_GizmoScale->Show(false);
}
//----------------------------------------------------------------------------
void albaOpClipSurface::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new albaGUI(this);
	m_Gui->Divider();
	wxString clip_by_choices[2] = {"surface","implicit function"};
	m_Gui->Combo(ID_CLIP_BY,_("clip by"),&m_ClipModality,2,clip_by_choices);
	m_Gui->Bool(ID_USE_GIZMO,_("use gizmo"),&m_UseGizmo,1);
	wxString gizmo_name[3] = {"translate","rotate","scale"};
	m_Gui->Combo(ID_CHOOSE_GIZMO,_("gizmo"),&m_GizmoType,3,gizmo_name);
	m_Gui->Button(ID_CHOOSE_SURFACE,_("clipper surface"));
	wxString geomModality[2] = { "Convex","Concave" };
	m_Gui->Radio(ID_GEOMETRY_MOD, "", &m_GeometryModality, 2, geomModality);
	m_Gui->Bool(ID_CLIP_INSIDE,_("Reverse clipping"),&m_ClipInside,1);
	m_Gui->Bool(ID_CLIP_PLANE_BBOX, _("Clip Plane Bound Box"), &m_ClipBoundBox, 1,"Disable to clip the entire plane");
	
	double b[6];
	m_Input->GetOutput()->GetVMEBounds(b);
	// bounding box dim
	m_PlaneWidth = b[1] - b[0];
	m_PlaneHeight = b[3] - b[2];
	//m_Gui->Double(ID_PLANE_WIDTH,_("plane w."),&m_PlaneWidth,0.0);
	//m_Gui->Double(ID_PLANE_HEIGHT,_("plane h."),&m_PlaneHeight,0.0);
	//m_Gui->Divider();
	m_Gui->Bool(ID_GENERATE_CLIPPED_OUTPUT,_("generate clipped output"),&m_GenerateClippedOutput,1);
	m_Gui->OkCancel();

	m_Gui->Enable(ID_CHOOSE_GIZMO, m_ClipModality == albaOpClipSurface::MODE_IMPLICIT_FUNCTION);
	m_Gui->Enable(ID_CLIP_PLANE_BBOX, m_ClipModality == albaOpClipSurface::MODE_IMPLICIT_FUNCTION);
	m_Gui->Enable(ID_CHOOSE_SURFACE, m_ClipModality == albaOpClipSurface::MODE_SURFACE);
	m_Gui->Enable(ID_GEOMETRY_MOD, m_ClipModality == albaOpClipSurface::MODE_SURFACE);
	m_Gui->Enable(wxOK,m_ResultPolyData != NULL);

	m_Gui->Divider();
}
//----------------------------------------------------------------------------
void albaOpClipSurface::OnEventThis(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId())
		{
		case ID_CHOOSE_SURFACE:
			{
				albaString title = "Choose m_Clipper Surface";
        e->SetPointer(&albaOpClipSurface::SurfaceAccept);
				e->SetString(&title);
				e->SetId(VME_CHOOSE);
				albaEventMacro(*e);
				m_ClipperVME = e->GetVme();
				if(m_ClipperVME == NULL)
					return;
				m_Gui->Enable(wxOK,true);
			}
			break;
		case ID_CLIP_INSIDE:
			if(m_Arrow) 
			{
				m_Arrow->SetScaleFactor(-1 * m_Arrow->GetScaleFactor());
				GetLogicManager()->CameraUpdate();
			}
			break;
		case ID_CLIP_BY:
			m_Gui->Enable(ID_CHOOSE_SURFACE, m_ClipModality == albaOpClipSurface::MODE_SURFACE);
			m_Gui->Enable(ID_GEOMETRY_MOD, m_ClipModality == albaOpClipSurface::MODE_SURFACE);
			m_Gui->Enable(ID_CLIP_PLANE_BBOX, m_ClipModality == albaOpClipSurface::MODE_IMPLICIT_FUNCTION);
			m_Gui->Enable(ID_CHOOSE_GIZMO, m_ClipModality == albaOpClipSurface::MODE_IMPLICIT_FUNCTION  && m_UseGizmo);
			m_Gui->Enable(ID_USE_GIZMO,m_ClipModality == albaOpClipSurface::MODE_IMPLICIT_FUNCTION);
			m_Gui->Enable(wxOK,m_ClipModality == albaOpClipSurface::MODE_IMPLICIT_FUNCTION);
			m_ClipInside = m_ClipModality;
			m_Gui->Update();
			ShowClipPlane(m_ClipModality != albaOpClipSurface::MODE_SURFACE);
			ChangeGizmo();
			break;
		case ID_PLANE_WIDTH:
		case ID_PLANE_HEIGHT:
			{
				if(m_PlaneCreated)
				{
					m_PlaneSource->SetPoint1(m_PlaneWidth/2,-m_PlaneHeight/2, 0);
					m_PlaneSource->SetPoint2(-m_PlaneWidth/2, m_PlaneHeight/2, 0);
					m_PlaneSource->SetOrigin(-m_PlaneWidth/2,-m_PlaneHeight/2, 0);
					m_PlaneSource->Update();
					GetLogicManager()->CameraUpdate();
				}
			}
			break;
		case ID_CHOOSE_GIZMO:
			{
				ChangeGizmo();
			}
			break;
		case ID_USE_GIZMO:
			{
				if(m_IsaCompositor && !m_UseGizmo)
					m_ImplicitPlaneGizmo->SetBehavior(m_IsaCompositor);
				else if(m_UseGizmo)
					m_ImplicitPlaneGizmo->SetBehavior(NULL);

				m_Gui->Enable(ID_CLIP_PLANE_BBOX, m_ClipModality == albaOpClipSurface::MODE_IMPLICIT_FUNCTION);
				m_Gui->Enable(ID_CHOOSE_GIZMO, m_ClipModality == albaOpClipSurface::MODE_IMPLICIT_FUNCTION  && m_UseGizmo);
				
				ChangeGizmo();
			}
			break;
		case wxOK:
			{
				int clip_result = Clip();
				if (clip_result == ALBA_ERROR)
				{
					wxMessageBox("Error while clipping surface!!", "Clipping Error");
				}
				else
					OpStop(OP_RUN_OK);

				GetLogicManager()->CameraUpdate();
			}
			break;
		case wxCANCEL:
			OpStop(OP_RUN_CANCEL);
			break;
		default:
			albaEventMacro(*e);
			break; 
		}
	}
}
//----------------------------------------------------------------------------
void albaOpClipSurface::ClipBoundingBox()
//----------------------------------------------------------------------------
{

	vtkALBASmartPointer<vtkTransformPolyDataFilter> transform_plane;
	transform_plane->SetTransform(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetVTKTransform());
	transform_plane->SetInputConnection(m_PlaneSource->GetOutputPort());
	transform_plane->Update();

	vtkALBASmartPointer<vtkTransformPolyDataFilter> transform_data_input;
	transform_data_input->SetTransform(m_Input->GetAbsMatrixPipe()->GetVTKTransform());
	transform_data_input->SetInputData(m_Input->GetOutput()->GetVTKData());
	transform_data_input->Update();

	m_ClipperBoundingBox->SetInputConnection(transform_data_input->GetOutputPort());
	m_ClipperBoundingBox->SetMask(transform_plane->GetOutput());
	m_ClipperBoundingBox->SetClipInside(m_ClipInside);
	m_ClipperBoundingBox->Update();

	m_ResultPolyData->DeepCopy(m_ClipperBoundingBox->GetOutput());
	
	if(m_GenerateClippedOutput)
	{
		m_ClipperBoundingBox->SetClipInside(m_ClipInside?0:1);
		m_ClipperBoundingBox->Update();

		m_ClippedPolyData->DeepCopy(m_ClipperBoundingBox->GetOutput());
	}

	if(!m_TestMode)
		m_Gui->Enable(wxOK,true);
}
//----------------------------------------------------------------------------
void albaOpClipSurface::OnEventGizmoTranslate(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	switch(alba_event->GetId())
	{
	case ID_TRANSFORM:
		{    
			// post multiplying matrixes coming from the gizmo to the vme
			// gizmo does not set vme pose  since they cannot scale
			PostMultiplyEventMatrix(alba_event);
		}
		break;

	default:
		{
			albaEventMacro(*alba_event);
		}
	}
}
//----------------------------------------------------------------------------
void albaOpClipSurface::OnEventGizmoRotate(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	switch(alba_event->GetId())
	{
	case ID_TRANSFORM:
		{    
			// post multiplying matrixes coming from the gizmo to the vme
			// gizmo does not set vme pose  since they cannot scale
			PostMultiplyEventMatrix(alba_event);
		}
		break;

	default:
		{
			albaEventMacro(*alba_event);
		}
	}
}
//----------------------------------------------------------------------------
void albaOpClipSurface::OnEventGizmoScale(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{ 
	switch(alba_event->GetId())
	{
	case ID_TRANSFORM:
		{ 
		GetLogicManager()->CameraUpdate();
		}
		break;

	default:
		{
			albaEventMacro(*alba_event);
		}
	}
}
//----------------------------------------------------------------------------
void albaOpClipSurface::OnEventGizmoPlane(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(alba_event->GetId())
		{
		case ID_TRANSFORM:
			{
				vtkTransform *currTr = vtkTransform::New();
				currTr->PostMultiply();
				currTr->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetVTKTransform()->GetMatrix());
				currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
				currTr->Update();

				albaMatrix newAbsMatr;
				newAbsMatr.DeepCopy(currTr->GetMatrix());
				newAbsMatr.SetTimeStamp(m_ImplicitPlaneGizmo->GetTimeStamp());

				// set the new pose to the gizmo
				m_ImplicitPlaneGizmo->SetAbsMatrix(newAbsMatr);
				UpdateISARefSys();

				GetLogicManager()->CameraUpdate();
				currTr->Delete();
			}
			break;
		default:
			{
				albaEventMacro(*alba_event);
			}
		}
	}
}
//----------------------------------------------------------------------------
void albaOpClipSurface::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (alba_event->GetSender() == this->m_Gui) // from this operation gui
	{
		OnEventThis(alba_event); 
	}
	else if(alba_event->GetSender() == this->m_GizmoTranslate)
	{
		OnEventGizmoTranslate(alba_event);
	}
	else if(alba_event->GetSender() == this->m_GizmoRotate)
	{
		OnEventGizmoRotate(alba_event);
	}
	else if(alba_event->GetSender() == this->m_GizmoScale)
	{
		OnEventGizmoScale(alba_event);
	}
	else
	{
		OnEventGizmoPlane(alba_event);
	}
}
//----------------------------------------------------------------------------
void albaOpClipSurface::ChangeGizmo()
//----------------------------------------------------------------------------
{
	if (m_ClipModality == albaOpClipSurface::MODE_IMPLICIT_FUNCTION)
	{
		m_GizmoTranslate->Show(m_GizmoType == GIZMO_TRANSLATE && m_UseGizmo);
		m_GizmoRotate->Show(m_GizmoType == GIZMO_ROTATE && m_UseGizmo);
		m_GizmoScale->Show(m_GizmoType == GIZMO_SCALE && m_UseGizmo, m_GizmoType == GIZMO_SCALE && m_UseGizmo, false, m_GizmoType == GIZMO_SCALE && m_UseGizmo);

		if (m_UseGizmo)
		{
			switch (m_GizmoType)
			{
			case GIZMO_TRANSLATE:
				m_GizmoTranslate->SetRefSys(m_ImplicitPlaneGizmo);
				break;
			case GIZMO_ROTATE:
				m_GizmoRotate->SetRefSys(m_ImplicitPlaneGizmo);
				break;
			case GIZMO_SCALE:
				m_GizmoScale->SetRefSys(m_ImplicitPlaneGizmo);
				break;
			}
		}
	}
	else
	{
		m_GizmoTranslate->Show(false);
		m_GizmoRotate->Show(false);
		m_GizmoScale->Show(false);
	}
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpClipSurface::OpStop(int result)
//----------------------------------------------------------------------------
{
  if(m_ImplicitPlaneGizmo)
  {
    m_ImplicitPlaneGizmo->SetBehavior(NULL);
    GetLogicManager()->VmeRemove(m_ImplicitPlaneGizmo);
  }
  albaDEL(m_ImplicitPlaneGizmo);
	vtkDEL(m_Gizmo);
	vtkDEL(m_ArrowShape);
	vtkDEL(m_PlaneSource);

	m_GizmoTranslate->Show(false);
	m_GizmoRotate->Show(false);
	m_GizmoScale->Show(false);
	cppDEL(m_GizmoTranslate);
	cppDEL(m_GizmoRotate);
	cppDEL(m_GizmoScale);

  HideGui();
  albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpClipSurface::OpDo()
//----------------------------------------------------------------------------
{
	albaString name;

	vtkALBASmartPointer<vtkTransformPolyDataFilter> transform_output;
	transform_output->SetTransform((vtkAbstractTransform *)m_Input->GetAbsMatrixPipe()->GetVTKTransform()->GetInverse());
	transform_output->SetInputData(m_ResultPolyData);
	transform_output->Update();

	name.Printf("Clipped %s", m_Input->GetName());
	albaNEW(m_ClippedVME);
	m_ClippedVME->SetData(transform_output->GetOutput(), m_Input->GetTimeStamp());
	m_ClippedVME->SetName(name);
	m_ClippedVME->Update();
	m_ClippedVME->ReparentTo(m_Input);
	m_ClippedVME->SetAbsMatrix(m_Input->GetAbsMatrixPipe()->GetMatrix());


	if(m_GenerateClippedOutput)
	{
		vtkALBASmartPointer<vtkTransformPolyDataFilter> transform_clipped_output;
		transform_clipped_output->SetTransform((vtkAbstractTransform *)m_Input->GetAbsMatrixPipe()->GetVTKTransform()->GetInverse());
		transform_clipped_output->SetInputData(m_ClippedPolyData);
		transform_clipped_output->Update();

		name.Printf("Reverse clipped %s", m_Input->GetName());
		albaNEW(m_ReverseClippedVME);
		m_ReverseClippedVME->SetData(transform_clipped_output->GetOutput(),m_Input->GetTimeStamp());
		m_ReverseClippedVME->SetName(name);
		m_ReverseClippedVME->Update();

		m_ReverseClippedVME->ReparentTo(m_Input);
		m_ReverseClippedVME->SetAbsMatrix(m_Input->GetAbsMatrixPipe()->GetMatrix());
	}
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpClipSurface::OpUndo()
//----------------------------------------------------------------------------
{
	if(m_GenerateClippedOutput)
	{
		m_ClippedVME->ReparentTo(NULL);
		albaDEL(m_ClippedVME);
		m_ReverseClippedVME->ReparentTo(NULL);
		albaDEL(m_ReverseClippedVME);
	}
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
int albaOpClipSurface::Clip()
//----------------------------------------------------------------------------
{
	if(!m_TestMode)
	{
    wxBusyCursor wait;
	}

  if(m_ClipModality == albaOpClipSurface::MODE_SURFACE)
	{
    if(m_ClipperVME == NULL)
      return ALBA_ERROR;

    vtkALBASmartPointer<vtkTransformPolyDataFilter> transform_data_input;
    transform_data_input->SetTransform((vtkAbstractTransform *)m_Input->GetAbsMatrixPipe()->GetVTKTransform());
    transform_data_input->SetInputData((vtkPolyData *)m_Input->GetOutput()->GetVTKData());
    transform_data_input->Update();

    // clip input surface by another surface
    // triangulate input for subdivision filter
		vtkALBASmartPointer<vtkTriangleFilter> triangles;
		triangles->SetInputConnection(transform_data_input->GetOutputPort());
    triangles->Update();
		
    m_ClipperVME->Update();
    vtkALBASmartPointer<vtkTransformPolyDataFilter> transform_data_clipper;
    transform_data_clipper->SetTransform((vtkAbstractTransform *)m_ClipperVME->GetAbsMatrixPipe()->GetVTKTransform());
    transform_data_clipper->SetInputData((vtkPolyData *)m_ClipperVME->GetOutput()->GetVTKData());
    transform_data_clipper->Update();

		vtkALBASmartPointer<vtkALBAImplicitPolyData> implicitPolyData;
		implicitPolyData->SetConcaveMode(m_GeometryModality);
		implicitPolyData->SetInput(transform_data_clipper->GetOutput());
		m_Clipper->SetInputConnection(triangles->GetOutputPort());
		m_Clipper->SetClipFunction(implicitPolyData);
	}
  else
  {
		if(m_ClipBoundBox)
		{
			ClipBoundingBox();
			return ALBA_OK;
		}
		else
		{
			vtkMatrix4x4 *mat = vtkMatrix4x4::New();
			mat->DeepCopy(m_Input->GetAbsMatrixPipe()->GetMatrixPointer()->GetVTKMatrix());
			mat->Invert();
			mat->Modified();

			// clip input surface by an implicit plane
			vtkTransform *tr = vtkTransform::New();
			tr->Concatenate(mat);
			tr->Concatenate(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetVTKTransform()->GetMatrix());
			tr->Inverse();
			tr->Update();

			m_ClipperPlane->SetTransform(tr);
			m_Clipper->SetInputData(vtkPolyData::SafeDownCast(m_Input->GetOutput()->GetVTKData()));
			m_Clipper->SetClipFunction(m_ClipperPlane);
			tr->Delete();
			mat->Delete();
		}
	}

	m_Clipper->SetGenerateClipScalars(0); // 0 outputs input data scalars, 1 outputs implicit function values
	m_Clipper->SetInsideOut(m_ClipInside);  // use 0/1 to reverse sense of clipping
	m_Clipper->SetValue(0);               // use this to control clip distance from clipper function to surface
  m_Clipper->Update();

	m_ResultPolyData->DeepCopy(m_Clipper->GetOutput());

	if(m_GenerateClippedOutput)
	{

		m_Clipper->SetInsideOut(m_ClipInside?0:1);
		m_Clipper->Update();

		m_ClippedPolyData->DeepCopy(m_Clipper->GetOutput());

	}

	if(!m_TestMode) 
		m_Gui->Enable(wxOK,true);

  return ALBA_OK;
}
//----------------------------------------------------------------------------
void albaOpClipSurface::PostMultiplyEventMatrix(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		long arg = e->GetArg();

		// handle incoming transform events
		vtkTransform *tr = vtkTransform::New();
		tr->PostMultiply();
		tr->SetMatrix(m_ImplicitPlaneGizmo->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
		tr->Concatenate(e->GetMatrix()->GetVTKMatrix());
		tr->Update();

		albaMatrix absPose;
		absPose.DeepCopy(tr->GetMatrix());
		absPose.SetTimeStamp(0.0);

		if (arg == albaInteractorGenericMouse::MOUSE_MOVE)
		{
			// move vme
			m_ImplicitPlaneGizmo->SetAbsMatrix(absPose);
			// update matrix for OpDo()
			//m_NewAbsMatrix = absPose;
		} 
		GetLogicManager()->CameraUpdate();

		// clean up
		tr->Delete();
	}
}
//----------------------------------------------------------------------------
void albaOpClipSurface::ShowClipPlane(bool show)
//----------------------------------------------------------------------------
{
  if(show)
  {
    if(m_ClipperPlane == NULL)
    {
      double b[6];
      m_Input->GetOutput()->GetVMEBounds(b);

      // bounding box dim
      double xdim = b[1] - b[0];
      double ydim = b[3] - b[2];
      double zdim = b[5] - b[4];

      // create the gizmo plane on the z = 0 plane
      vtkNEW(m_PlaneSource);
      m_PlaneSource->SetPoint1(m_PlaneWidth/2,-m_PlaneHeight/2, 0);
      m_PlaneSource->SetPoint2(-m_PlaneWidth/2, m_PlaneHeight/2, 0);
      m_PlaneSource->SetOrigin(-m_PlaneWidth/2,-m_PlaneHeight/2, 0);
      m_PlaneSource->Update();

      vtkNEW(m_ClipperPlane);
      m_ClipperPlane->SetOrigin(m_PlaneSource->GetOrigin());
      m_ClipperPlane->SetNormal(m_PlaneSource->GetNormal());

      vtkNEW(m_ArrowShape);
      m_ArrowShape->SetShaftResolution(40);
      m_ArrowShape->SetTipResolution(40);

      vtkNEW(m_Arrow);
      m_Arrow->SetInputConnection(m_PlaneSource->GetOutputPort());
      m_Arrow->SetSourceConnection(m_ArrowShape->GetOutputPort());
      m_Arrow->SetVectorModeToUseNormal();
      
      int clip_sign = m_ClipInside ? 1 : -1;
      m_Arrow->SetScaleFactor(clip_sign * abs(zdim/10.0));
      m_Arrow->Update();

      vtkNEW(m_Gizmo);
      m_Gizmo->AddInputConnection(m_PlaneSource->GetOutputPort());
      m_Gizmo->AddInputConnection(m_Arrow->GetOutputPort());
      m_Gizmo->Update();

      albaNEW(m_ImplicitPlaneGizmo);
      m_ImplicitPlaneGizmo->SetData(m_Gizmo->GetOutput());
      m_ImplicitPlaneGizmo->SetName("implicit plane gizmo");
      m_ImplicitPlaneGizmo->ReparentTo(m_Input->GetRoot());

      // position the plane
      albaSmartPointer<albaTransform> currTr;
      currTr->Translate((b[0] + b[1]) / 2, (b[2] + b[3]) / 2,(b[4] + b[5]) / 2 , POST_MULTIPLY);
      currTr->Update();

      albaMatrix mat;
      mat.DeepCopy(&currTr->GetMatrix());
      mat.SetTimeStamp(m_Input->GetTimeStamp());

      m_ImplicitPlaneGizmo->SetAbsMatrix(mat);

      GetLogicManager()->VmeShow(m_ImplicitPlaneGizmo, true);
    }
    mmaMaterial *material = m_ImplicitPlaneGizmo->GetMaterial();
    material->m_Prop->SetOpacity(0.5);
    material->m_Opacity = material->m_Prop->GetOpacity();
    
    if(!m_PlaneCreated)
    {
      material->m_Prop->SetColor(0.2,0.2,0.8);
      material->m_Prop->GetDiffuseColor(material->m_Diffuse);
      AttachInteraction();
      m_PlaneCreated = true;
    }
  }
  else
  {
    if(m_ImplicitPlaneGizmo != NULL)
    {
      mmaMaterial *material = m_ImplicitPlaneGizmo->GetMaterial();
      material->m_Prop->SetOpacity(0);
      material->m_Opacity = material->m_Prop->GetOpacity();
    }
  }

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpClipSurface::AttachInteraction()
//----------------------------------------------------------------------------
{
  albaNEW(m_IsaCompositor);

  m_IsaRotate = m_IsaCompositor->CreateBehavior(MOUSE_LEFT);
  m_IsaRotate->SetListener(this);
  m_IsaRotate->SetVME(m_ImplicitPlaneGizmo);
  m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetTypeToView();
  m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
  m_IsaRotate->GetPivotRefSys()->SetTypeToView();
  m_IsaRotate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
  m_IsaRotate->GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
  m_IsaRotate->EnableRotation(true);

  m_IsaTranslate = m_IsaCompositor->CreateBehavior(MOUSE_MIDDLE);
  m_IsaTranslate->SetListener(this);
  m_IsaTranslate->SetVME(m_ImplicitPlaneGizmo);
  m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
  m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
  m_IsaTranslate->GetPivotRefSys()->SetTypeToView();
  m_IsaTranslate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
  m_IsaTranslate->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
  m_IsaTranslate->EnableTranslation(true);

	if(!m_UseGizmo)
		m_ImplicitPlaneGizmo->SetBehavior(m_IsaCompositor);
}

//----------------------------------------------------------------------------
void albaOpClipSurface::UpdateISARefSys()
//----------------------------------------------------------------------------
{
  m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
  m_IsaRotate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
  
  m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
  m_IsaTranslate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
}

//----------------------------------------------------------------------------
bool albaOpClipSurface::SurfaceAccept(albaVME*node)
{
	return (node != NULL && (node->IsALBAType(albaVMESurface) || node->IsALBAType(albaVMESurfaceParametric)));
}

//----------------------------------------------------------------------------
void albaOpClipSurface::SetClippingModality(int mode)
//----------------------------------------------------------------------------
{
  if (mode != albaOpClipSurface::MODE_SURFACE && mode != albaOpClipSurface::MODE_IMPLICIT_FUNCTION)
  {
    return;
  }

  m_ClipModality = mode;
}
//----------------------------------------------------------------------------
void albaOpClipSurface::SetClippingSurface(albaVMESurface *surface)
//----------------------------------------------------------------------------
{
  m_ClipperVME = surface;
}
//----------------------------------------------------------------------------
void albaOpClipSurface::SetImplicitPlanePosition(albaMatrix &matrix)
//----------------------------------------------------------------------------
{
  m_ImplicitPlaneGizmo->SetAbsMatrix(matrix);
}
