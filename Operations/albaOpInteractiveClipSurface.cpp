/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpInteractiveClipSurface
 Authors: Paolo Quadrani, Stefano Perticoni , Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

bool DEBUG_MODE = true;

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaOpInteractiveClipSurface.h"
#include "wx/busyinfo.h"

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"

#include "mmaMaterial.h"
#include "albaInteractorCompositorMouse.h"
#include "albaInteractorGenericMouse.h"

#include "albaSmartPointer.h"
#include "albaVMEGizmo.h"
#include "albaGizmoTranslate.h"
#include "albaGizmoRotate.h"
#include "albaGizmoScale.h"
#include "albaAbsMatrixPipe.h"
#include "albaTransform.h"
#include "albaRefSys.h"
#include "albaCurvilinearAbscissaOnSkeletonHelper.h"
#include "albaVMEPolylineGraph.h"

#include "vtkProperty.h"
#include "vtkTransform.h"
#include "vtkPolyData.h"
#include "vtkPlane.h"
#include "vtkPlaneSource.h"
#include "vtkClipPolyData.h"
#include "vtkALBAImplicitPolyData.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkArrowSource.h"
#include "vtkGlyph3D.h"
#include "vtkAppendPolyData.h"
#include "vtkALBAClipSurfaceBoundingBox.h"
#include "vtkSphereSource.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpInteractiveClipSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpInteractiveClipSurface::albaOpInteractiveClipSurface(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
	m_InputPreserving = false;

  m_ClipperVME    = NULL;
	m_ClippedVME    = NULL;
  m_ClipperPlane  = NULL;
  m_Arrow         = NULL;
  m_Clipper       = NULL;
	m_ClipperBoundingBox = NULL;
  
  m_ImplicitPlaneVMEGizmo  = NULL;
  m_IsaCompositor       = NULL;

  m_OldSurface = NULL;
  
  m_ClipInside      = 1;
	m_UseGizmo			=	1;
	m_ClipBoundBox	= 1;
  m_ClipModality  = MODE_IMPLICIT_FUNCTION;
	m_GizmoType			= GIZMO_TRANSLATE;
  
  m_PlaneCreated = false;

	m_PlaneHeight = 0.0;
	m_PlaneWidth	= 0.0;

	m_PlaneSource	= NULL;
	m_AppendPolydata				= NULL;
	m_ArrowSource	= NULL;
  m_SphereSource = NULL;

	m_GizmoTranslate	= NULL;
	m_GizmoRotate			= NULL;
	m_GizmoScale			= NULL;

  m_Canundo = NULL;
  
  m_CASH = NULL;
  m_ConstrainalbaVMEPolylineGraph = NULL;
  m_ActiveBranchId = NULL;

}

//----------------------------------------------------------------------------
albaOpInteractiveClipSurface::~albaOpInteractiveClipSurface()
//----------------------------------------------------------------------------
{
	for(int i=0;i<m_ResultPolyData.size();i++)
	{
		vtkDEL(m_ResultPolyData[i]);
	}
	m_ResultPolyData.clear();

  albaDEL(m_IsaCompositor);
	albaDEL(m_ClippedVME);
  vtkDEL(m_ClipperPlane);
  vtkDEL(m_Clipper);
	vtkDEL(m_ClipperBoundingBox);
  vtkDEL(m_OldSurface);
  vtkDEL(m_Arrow);
  vtkDEL(m_SphereSource);

  cppDEL(m_CASH);
}
//----------------------------------------------------------------------------
albaOp* albaOpInteractiveClipSurface::Copy()   
//----------------------------------------------------------------------------
{
	return new albaOpInteractiveClipSurface(m_Label);
}

//----------------------------------------------------------------------------
bool albaOpInteractiveClipSurface::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
	return (node != NULL && node->IsALBAType(albaVMESurface));
}

//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum CLIP_SURFACE_ID
{
  ID_CHOOSE_SURFACE = MINID,
  ID_CLIP_BY,
  ID_CLIP_INSIDE,
	ID_GENERATE_CLIPPED_OUTPUT,
	ID_PLANE_WIDTH,
	ID_PLANE_HEIGHT,
	ID_CHOOSE_GIZMO,
	ID_USE_GIZMO,
	ID_CLIP,
	ID_UNDO,
	ID_CLIP_BOX,
  ID_CHOOSE_CONSTRAINT_VME,
};

//----------------------------------------------------------------------------
void albaOpInteractiveClipSurface::OpRun()   
//----------------------------------------------------------------------------
{
  
  vtkNEW(m_Clipper);
	vtkNEW(m_ClipperBoundingBox);
  vtkNEW(m_OldSurface);

  m_OldSurface->DeepCopy((vtkPolyData*)m_Input->GetOutput()->GetVTKData());
  
	vtkPolyData *initialData;
	vtkNEW(initialData);
	initialData->DeepCopy(m_OldSurface);
	m_ResultPolyData.push_back(initialData);

	if(!m_TestMode)
	{
    ShowClipPlane(m_ClipModality != albaOpInteractiveClipSurface::MODE_SURFACE);
		BuildTransformGizmos();
    
    assert(m_CASH == NULL);
    m_CASH = new albaCurvilinearAbscissaOnSkeletonHelper(m_ImplicitPlaneVMEGizmo, this) ;

		//Enable & show Gizmos
		ChangeGizmo();

    
    CreateGui();
    ShowGui();

		GetLogicManager()->CameraUpdate();
  }
  // Added facility for test case
  else
  {
    ShowClipPlane(m_ClipModality != albaOpInteractiveClipSurface::MODE_SURFACE);
  }
}
//----------------------------------------------------------------------------
void albaOpInteractiveClipSurface::BuildTransformGizmos()
//----------------------------------------------------------------------------
{
	m_ImplicitPlaneVMEGizmo->GetOutput()->GetVTKData()->ComputeBounds();
	m_ImplicitPlaneVMEGizmo->Modified();
	m_ImplicitPlaneVMEGizmo->Update();

	m_GizmoTranslate = new albaGizmoTranslate(m_ImplicitPlaneVMEGizmo, this);
	m_GizmoTranslate->SetRefSys(m_ImplicitPlaneVMEGizmo);
	m_GizmoTranslate->Show(false);
	m_GizmoRotate = new albaGizmoRotate(m_ImplicitPlaneVMEGizmo, this);
	m_GizmoRotate->SetRefSys(m_ImplicitPlaneVMEGizmo);
	m_GizmoRotate->Show(false);
	m_GizmoScale = new albaGizmoScale(m_ImplicitPlaneVMEGizmo, this);
	m_GizmoScale->SetRefSys(m_ImplicitPlaneVMEGizmo);
	m_GizmoScale->Show(false);
}
//----------------------------------------------------------------------------
void albaOpInteractiveClipSurface::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new albaGUI(this);
	m_Gui->Divider();
	wxString clip_by_choices[2] = {"surface","implicit function"};

	m_Gui->Combo(ID_CLIP_BY,_("clip by"),&m_ClipModality,2,clip_by_choices);
  m_Gui->Divider(2);

  m_Gui->Bool(ID_USE_GIZMO,_("use gizmo"),&m_UseGizmo,1);
	wxString gizmo_name[3] = {"translate","rotate","scale"};
	m_Gui->Combo(ID_CHOOSE_GIZMO,_("gizmo"),&m_GizmoType,3,gizmo_name);
  
  m_Gui->AddGui(m_GizmoTranslate->GetGui());
  m_Gui->AddGui(m_GizmoRotate->GetGui());
  // something strange has happened to the gizmo scale...
  // commented for the moment
  // m_Gui->AddGui(m_GizmoScale->GetGui());

  m_Gui->Divider();
  m_Gui->Divider(2);

  m_Gui->Button(ID_CHOOSE_CONSTRAINT_VME, "choose curve", "", "albaVMEPolylineGraph constrain");
  m_Gui->Divider();
  m_Gui->AddGui(m_CASH->GetGui());
  m_CASH->EnableWidgets(false);
  m_Gui->Divider();
	m_Gui->Button(ID_CHOOSE_SURFACE,_("clipper surface"));
	m_Gui->Bool(ID_CLIP_INSIDE,_("reverse clipping"),&m_ClipInside,1);
	
  m_Gui->Button(ID_CLIP,_("clip"));
	m_Gui->Button(ID_UNDO,_("undo"));
  m_Gui->OkCancel();

  GuiEnable();

	m_Gui->Divider();


}
//----------------------------------------------------------------------------
void albaOpInteractiveClipSurface::OnEventThis(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId())
		{
    case ID_CHOOSE_CONSTRAINT_VME:
      {
        int result = wxMessageBox("the current position will be reset: continue?","Warning", wxOK|wxCANCEL);
        if (result == wxOK)
        {
          // continue and change constraint...
        } 
        else
        {
          return;
        }
        albaString s(_("Choose Constrain"));
        albaEvent e(this,VME_CHOOSE, &s);
				e.SetPointer(&albaOpInteractiveClipSurface::ConstrainAccept);
        albaEventMacro(e);
        albaVME *vme = e.GetVme();
        if(vme != NULL)
        {
          OnChooseConstrainVme(vme);

          assert(true);
        }
      }
      break;
		case ID_CHOOSE_SURFACE:
			{
				albaString title = "Choose m_Clipper Surface";
				e->SetPointer(&albaOpInteractiveClipSurface::SurfaceAccept);
				e->SetString(&title);
				e->SetId(VME_CHOOSE);
				albaEventMacro(*e);
				m_ClipperVME = albaVMESurface::SafeDownCast(e->GetVme());
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
			m_Gui->Enable(ID_CHOOSE_SURFACE, m_ClipModality == albaOpInteractiveClipSurface::MODE_SURFACE);
			m_Gui->Enable(ID_CHOOSE_GIZMO, m_ClipModality == albaOpInteractiveClipSurface::MODE_IMPLICIT_FUNCTION  && m_UseGizmo);
			m_Gui->Enable(ID_GENERATE_CLIPPED_OUTPUT, m_ClipModality == albaOpInteractiveClipSurface::MODE_IMPLICIT_FUNCTION);
			m_Gui->Enable(ID_USE_GIZMO,m_ClipModality == albaOpInteractiveClipSurface::MODE_IMPLICIT_FUNCTION);
			m_Gui->Enable(wxOK,m_ClipModality == albaOpInteractiveClipSurface::MODE_IMPLICIT_FUNCTION);
			m_ClipInside = m_ClipModality;
			m_Gui->Update();
			ShowClipPlane(m_ClipModality != albaOpInteractiveClipSurface::MODE_SURFACE);
			ChangeGizmo();
			GetLogicManager()->CameraUpdate();
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
				GetLogicManager()->CameraUpdate();
			}
			break;
		case ID_USE_GIZMO:
			{
        OnUseGizmo();
			}
			break;
		case ID_CLIP:
			{
				int clip_result = Clip();
				if (clip_result == ALBA_ERROR)
				{
					wxMessageBox("Error while clipping surface!!", "Clipping Error");
				}
				GetLogicManager()->CameraUpdate();
			}
			break;
		case ID_UNDO:
			{
				Undo();
				GetLogicManager()->CameraUpdate();
			}
			break;
		case wxOK:
			{
				OpStop(OP_RUN_OK);
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
void albaOpInteractiveClipSurface::OnUseGizmo()
//----------------------------------------------------------------------------
{
	if(m_IsaCompositor && !m_UseGizmo)
	m_ImplicitPlaneVMEGizmo->SetBehavior(m_IsaCompositor);
	else if(m_IsaCompositorWithArrowGizmo && m_UseGizmo)
	m_ImplicitPlaneVMEGizmo->SetBehavior(m_IsaCompositorWithArrowGizmo);
	
	m_Gui->Enable(ID_CHOOSE_GIZMO, m_ClipModality == albaOpInteractiveClipSurface::MODE_IMPLICIT_FUNCTION  && m_UseGizmo);    				
	ChangeGizmo();
	m_Gui->Enable(ID_CHOOSE_CONSTRAINT_VME, m_UseGizmo?false:true);
	m_CASH->EnableWidgets(!m_UseGizmo && m_ConstrainalbaVMEPolylineGraph ? true : false);

	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaOpInteractiveClipSurface::ClipBoundingBox()
//----------------------------------------------------------------------------
{

	vtkALBASmartPointer<vtkTransform> rotate;
	rotate->RotateX(180);

	vtkALBASmartPointer<vtkTransformPolyDataFilter> before_transform_plane;
	vtkALBASmartPointer<vtkTransformPolyDataFilter> transform_plane;
	if(m_ClipInside==1)//if clip reverse is necessary rotate plane
	{
		before_transform_plane->SetTransform(rotate);
		before_transform_plane->SetInputConnection(m_PlaneSource->GetOutputPort());
		before_transform_plane->Update();

		transform_plane->SetInputConnection(before_transform_plane->GetOutputPort());
	}
	else
		transform_plane->SetInputConnection(m_PlaneSource->GetOutputPort());
	
	
	transform_plane->SetTransform(m_ImplicitPlaneVMEGizmo->GetAbsMatrixPipe()->GetVTKTransform());
	transform_plane->Update();

	vtkALBASmartPointer<vtkTransformPolyDataFilter> transform_data_input;
	transform_data_input->SetTransform(m_Input->GetAbsMatrixPipe()->GetVTKTransform());
	transform_data_input->SetInputData((vtkPolyData *)((albaVME *)m_Input)->GetOutput()->GetVTKData());
	transform_data_input->Update();

	m_ClipperBoundingBox->SetInputConnection(transform_data_input->GetOutputPort());
	m_ClipperBoundingBox->SetMask(transform_plane->GetOutput());
	m_ClipperBoundingBox->SetClipInside(0);
	m_ClipperBoundingBox->Update();

	vtkPolyData *newPolyData;
	vtkNEW(newPolyData);
	newPolyData->DeepCopy(m_ClipperBoundingBox->GetOutput());

	int result=((albaVMESurface*)m_Input)->SetData(newPolyData,m_Input->GetTimeStamp());

	if(result==ALBA_OK)
		m_ResultPolyData.push_back(newPolyData);

	if (!m_TestMode)
  {
    m_Gui->Enable(ID_UNDO,m_ResultPolyData.size()>1);
	  m_Gui->Enable(wxOK,m_ResultPolyData.size()>1);
  }
}
//----------------------------------------------------------------------------
void albaOpInteractiveClipSurface::OnEventGizmoTranslate(albaEventBase *alba_event)
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
void albaOpInteractiveClipSurface::OnEventGizmoRotate(albaEventBase *alba_event)
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
void albaOpInteractiveClipSurface::OnEventGizmoScale(albaEventBase *alba_event)
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
void albaOpInteractiveClipSurface::OnEventGizmoPlane(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(alba_event->GetId())
		{
		case ID_TRANSFORM:
			{
        
				if(e->GetSender()==m_IsaChangeArrowWithoutGizmo || e->GetSender()==m_IsaChangeArrowWithGizmo)
				{
					if(m_Arrow) 
					{
						if(e->GetArg()==albaInteractorGenericMouse::MOUSE_DOWN)
						{
							m_ClipInside= m_ClipInside ? 0 : 1;
							m_Gui->Update();
							m_Arrow->SetScaleFactor(-1 * m_Arrow->GetScaleFactor());
							GetLogicManager()->CameraUpdate();
						}
					}
				}
				else if(e->GetSender()==m_IsaClipWithoutGizmo || e->GetSender()==m_IsaClipWithGizmo)
				{
					if(e->GetArg()==albaInteractorGenericMouse::MOUSE_DOWN)
					{
						Clip();
						GetLogicManager()->CameraUpdate();
					}
				}
				else
				{
          m_CASH->MoveOnSkeleton(e);

          if (DEBUG_MODE)
            {
              std::ostringstream stringStream;
              stringStream  << "ABS Transform Matrix" << std::endl;
              e->GetMatrix()->Print(stringStream);
              albaLogMessage(stringStream.str().c_str());
            }

					GetLogicManager()->CameraUpdate();
				}
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
void albaOpInteractiveClipSurface::OnEvent(albaEventBase *alba_event)
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
void albaOpInteractiveClipSurface::ChangeGizmo()
//----------------------------------------------------------------------------
{
	if(m_ClipModality==albaOpInteractiveClipSurface::MODE_IMPLICIT_FUNCTION)
	{
		m_GizmoTranslate->Show(m_GizmoType==GIZMO_TRANSLATE && m_UseGizmo);
		m_GizmoRotate->Show(m_GizmoType==GIZMO_ROTATE && m_UseGizmo);
		m_GizmoScale->Show(m_GizmoType==GIZMO_SCALE && m_UseGizmo,m_GizmoType==GIZMO_SCALE && m_UseGizmo,false,m_GizmoType==GIZMO_SCALE && m_UseGizmo);
		
		if(m_UseGizmo)
		{
			switch(m_GizmoType)
			{
			case GIZMO_TRANSLATE:
				m_GizmoTranslate->SetRefSys(m_ImplicitPlaneVMEGizmo);
				break;
			case GIZMO_ROTATE:
				m_GizmoRotate->SetRefSys(m_ImplicitPlaneVMEGizmo);
				break;
			case GIZMO_SCALE:
				m_GizmoScale->SetRefSys(m_ImplicitPlaneVMEGizmo);
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
}
//----------------------------------------------------------------------------
void albaOpInteractiveClipSurface::OpStop(int result)
//----------------------------------------------------------------------------
{
  if(result == OP_RUN_CANCEL)
  {
    ((albaVMESurface *)m_Input)->SetData(m_OldSurface,m_Input->GetTimeStamp());
    ((albaVMESurface *)m_Input)->Update();
  }

  if(m_ImplicitPlaneVMEGizmo)
  {
    m_ImplicitPlaneVMEGizmo->SetBehavior(NULL);
    GetLogicManager()->VmeRemove(m_ImplicitPlaneVMEGizmo);
  }

  if(m_GizmoTranslate)
  {
	  m_GizmoTranslate->Show(false);
  }
	if (m_GizmoRotate)
	{
		m_GizmoRotate->Show(false);
	}
	if (m_GizmoScale)
	{
		m_GizmoScale->Show(false);
	}

  albaDEL(m_IsaCompositor);
  albaDEL(m_IsaCompositorWithArrowGizmo);

	cppDEL(m_GizmoTranslate);
	cppDEL(m_GizmoRotate);
	cppDEL(m_GizmoScale);

  cppDEL(m_CASH);
  albaDEL(m_ImplicitPlaneVMEGizmo);
  vtkDEL(m_AppendPolydata);
  vtkDEL(m_Arrow);
  vtkDEL(m_SphereSource);
  vtkDEL(m_ArrowSource);
  vtkDEL(m_ClipperPlane);
  vtkDEL(m_PlaneSource);

  if (!m_TestMode)
  {
    HideGui();
  }

  albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpInteractiveClipSurface::OpDo()
//----------------------------------------------------------------------------
{
	vtkALBASmartPointer<vtkTransformPolyDataFilter> transform_output;
	transform_output->SetTransform((vtkAbstractTransform *)m_Input->GetAbsMatrixPipe()->GetVTKTransform()->GetInverse());
	transform_output->SetInputData(m_ResultPolyData[m_ResultPolyData.size()-1]);
	transform_output->Update();

	((albaVMESurface *)m_Input)->SetData(transform_output->GetOutput(),m_Input->GetTimeStamp());
	
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpInteractiveClipSurface::OpUndo()
//----------------------------------------------------------------------------
{
  ((albaVMESurface *)m_Input)->SetData(m_OldSurface,m_Input->GetTimeStamp());

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
int albaOpInteractiveClipSurface::Clip()
//----------------------------------------------------------------------------
{
	if(!m_TestMode)
	{
    wxBusyCursor wait;
	}

  if(m_ClipModality == albaOpInteractiveClipSurface::MODE_SURFACE)
	{
    if(m_ClipperVME == NULL)
      return ALBA_ERROR;

    vtkALBASmartPointer<vtkTransformPolyDataFilter> transform_data_input;
    transform_data_input->SetTransform((vtkAbstractTransform *)m_Input->GetAbsMatrixPipe()->GetVTKTransform());
    transform_data_input->SetInputData((vtkPolyData *)((albaVME *)m_Input)->GetOutput()->GetVTKData());
    transform_data_input->Update();

    // clip input surface by another surface
    // triangulate input for subdivision filter
		vtkALBASmartPointer<vtkTriangleFilter> triangulatedInputData;
		triangulatedInputData->SetInputConnection(transform_data_input->GetOutputPort());
		triangulatedInputData->Update();
		
		
    m_ClipperVME->Update();
    vtkALBASmartPointer<vtkTransformPolyDataFilter> transform_data_clipper;
    transform_data_clipper->SetTransform((vtkAbstractTransform *)m_ClipperVME->GetAbsMatrixPipe()->GetVTKTransform());
    transform_data_clipper->SetInputData((vtkPolyData *)m_ClipperVME->GetOutput()->GetVTKData());
    transform_data_clipper->Update();

		vtkALBASmartPointer<vtkALBAImplicitPolyData> implicitPolyData;
		implicitPolyData->SetInput(transform_data_clipper->GetOutput());
		m_Clipper->SetInputConnection(triangulatedInputData->GetOutputPort());
		m_Clipper->SetClipFunction(implicitPolyData);
	}
  else
  {
		if(m_ClipBoundBox)
		{
			ClipBoundingBox();
 
      // Only TEST case!
      if (m_TestMode)
      {
        OpStop(OP_RUN_OK);
      }
 
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
			tr->Concatenate(m_ImplicitPlaneVMEGizmo->GetAbsMatrixPipe()->GetVTKTransform()->GetMatrix());
			tr->Inverse();
			tr->Update();

			m_ClipperPlane->SetTransform(tr);
			m_Clipper->SetInputData(vtkPolyData::SafeDownCast(m_Input->GetOutput()->GetVTKData()));
			m_Clipper->SetClipFunction(m_ClipperPlane);
			tr->Delete();
			mat->Delete();
		}
	}
	vtkPolyData *newPolyData;
	vtkNEW(newPolyData);

	m_Clipper->SetGenerateClipScalars(0); // 0 outputs input data scalars, 1 outputs implicit function values
	m_Clipper->SetInsideOut(m_ClipInside);  // use 0/1 to reverse sense of clipping
	m_Clipper->SetValue(0);               // use this to control clip distance from clipper function to surface
  m_Clipper->Update();

	newPolyData->DeepCopy(m_Clipper->GetOutput());

	int result=((albaVMESurface*)m_Input)->SetData(newPolyData,m_Input->GetTimeStamp());

	if(result==ALBA_OK)
		m_ResultPolyData.push_back(newPolyData);

	if (!m_TestMode)
  {
    m_Gui->Enable(ID_UNDO,m_ResultPolyData.size()>1);
	  m_Gui->Enable(wxOK,m_ResultPolyData.size()>1);
  }
  return ALBA_OK;
}
//----------------------------------------------------------------------------
void albaOpInteractiveClipSurface::Undo()
//----------------------------------------------------------------------------
{
	if(m_ResultPolyData.size()>1)
	{
		vtkDEL(m_ResultPolyData[m_ResultPolyData.size()-1]);
		m_ResultPolyData.pop_back();
		((albaVMESurface*)m_Input)->SetData((vtkPolyData*)m_ResultPolyData[m_ResultPolyData.size()-1],m_Input->GetTimeStamp());
	}
	m_Gui->Enable(ID_UNDO,m_ResultPolyData.size()>1);
	m_Gui->Enable(wxOK,m_ResultPolyData.size()>1);
}
//----------------------------------------------------------------------------
void albaOpInteractiveClipSurface::PostMultiplyEventMatrix(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		long arg = e->GetArg();

		// handle incoming transform events
		vtkTransform *tr = vtkTransform::New();
		tr->PostMultiply();
		tr->SetMatrix(m_ImplicitPlaneVMEGizmo->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
		tr->Concatenate(e->GetMatrix()->GetVTKMatrix());
		tr->Update();

		albaMatrix absPose;
		absPose.DeepCopy(tr->GetMatrix());
		absPose.SetTimeStamp(0.0);

		if (arg == albaInteractorGenericMouse::MOUSE_MOVE)
		{
			// move vme
			m_ImplicitPlaneVMEGizmo->SetAbsMatrix(absPose);
			// update matrix for OpDo()
			//m_NewAbsMatrix = absPose;
		} 
		GetLogicManager()->CameraUpdate();

		// clean up
		tr->Delete();
	}
}
//----------------------------------------------------------------------------
void albaOpInteractiveClipSurface::ShowClipPlane(bool show)
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

      if (DEBUG_MODE)
        {
          std::ostringstream stringStream;
          stringStream << "xdim: " << xdim  << std::endl;
          stringStream << "ydim: " << ydim  << std::endl;
          stringStream << "zdim: " << zdim  << std::endl;
          albaLogMessage(stringStream.str().c_str());
        }

      m_PlaneWidth = (b[1] - b[0]) / 4;
      m_PlaneHeight = (b[3] - b[2]) /4;

      // create the gizmo plane on the z = 0 plane
      vtkNEW(m_PlaneSource);
      m_PlaneSource->SetPoint1(m_PlaneWidth/2,-m_PlaneHeight/2, 0);
      m_PlaneSource->SetPoint2(-m_PlaneWidth/2, m_PlaneHeight/2, 0);
      m_PlaneSource->SetOrigin(-m_PlaneWidth/2,-m_PlaneHeight/2, 0);
      m_PlaneSource->Update();

      vtkNEW(m_ClipperPlane);
      m_ClipperPlane->SetOrigin(m_PlaneSource->GetOrigin());
      m_ClipperPlane->SetNormal(m_PlaneSource->GetNormal());

      vtkNEW(m_ArrowSource);
      m_ArrowSource->SetShaftResolution(40);
      m_ArrowSource->SetTipResolution(40);

      vtkNEW(m_SphereSource);
      m_SphereSource->SetRadius(m_PlaneWidth / 10);

      vtkNEW(m_Arrow);
      m_Arrow->SetInputConnection(m_PlaneSource->GetOutputPort());
      m_Arrow->SetSourceConnection(m_ArrowSource->GetOutputPort());
      m_Arrow->SetVectorModeToUseNormal();
   

      int clip_sign = m_ClipInside ? 1 : -1;
      m_Arrow->SetScaleFactor(clip_sign * abs(zdim/10.0));
      m_Arrow->Update();

      vtkNEW(m_AppendPolydata);
      m_AppendPolydata->AddInputConnection(m_PlaneSource->GetOutputPort());
      m_AppendPolydata->AddInputConnection(m_Arrow->GetOutputPort());
      m_AppendPolydata->AddInputConnection(m_SphereSource->GetOutputPort());
      m_AppendPolydata->Update();

      albaNEW(m_ImplicitPlaneVMEGizmo);
      m_ImplicitPlaneVMEGizmo->SetData(m_AppendPolydata->GetOutput());
      m_ImplicitPlaneVMEGizmo->SetName("implicit plane gizmo");
      m_ImplicitPlaneVMEGizmo->ReparentTo(m_Input->GetRoot());

      // position the plane
      albaSmartPointer<albaTransform> currTr;
      currTr->Translate((b[0] + b[1]) / 2, (b[2] + b[3]) / 2,(b[4] + b[5]) / 2 , POST_MULTIPLY);
      currTr->Update();

      albaMatrix mat;
      mat.DeepCopy(&currTr->GetMatrix());
      mat.SetTimeStamp(m_Input->GetTimeStamp());

      m_ImplicitPlaneVMEGizmo->SetAbsMatrix(mat);

      GetLogicManager()->VmeShow(m_ImplicitPlaneVMEGizmo, true);
    }
    mmaMaterial *material = m_ImplicitPlaneVMEGizmo->GetMaterial();
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
    if(m_ImplicitPlaneVMEGizmo != NULL)
    {
      mmaMaterial *material = m_ImplicitPlaneVMEGizmo->GetMaterial();
      material->m_Prop->SetOpacity(0);
      material->m_Opacity = material->m_Prop->GetOpacity();
    }
  }
}
//----------------------------------------------------------------------------
void albaOpInteractiveClipSurface::AttachInteraction()
//----------------------------------------------------------------------------
{
  albaNEW(m_IsaCompositor);

  m_IsaTranslate = m_IsaCompositor->CreateBehavior( MOUSE_LEFT );
  m_IsaTranslate->SetListener(this);
  m_IsaTranslate->SetVME(m_ImplicitPlaneVMEGizmo);
  m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
  m_IsaTranslate->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
  m_IsaTranslate->EnableTranslation(true);
  
	m_IsaChangeArrowWithoutGizmo = m_IsaCompositor->CreateBehavior(MOUSE_LEFT_SHIFT);
	m_IsaChangeArrowWithoutGizmo->SetListener(this);
	m_IsaChangeArrowWithoutGizmo->SetVME(m_ImplicitPlaneVMEGizmo);

	m_IsaClipWithoutGizmo = m_IsaCompositor->CreateBehavior(MOUSE_LEFT_CONTROL);
	m_IsaClipWithoutGizmo->SetListener(this);
	m_IsaClipWithoutGizmo->SetVME(m_ImplicitPlaneVMEGizmo);

	albaNEW(m_IsaCompositorWithArrowGizmo);

	m_IsaChangeArrowWithGizmo = m_IsaCompositorWithArrowGizmo->CreateBehavior(MOUSE_LEFT_SHIFT);
	m_IsaChangeArrowWithGizmo->SetListener(this);
	m_IsaChangeArrowWithGizmo->SetVME(m_ImplicitPlaneVMEGizmo);

	m_IsaClipWithGizmo = m_IsaCompositorWithArrowGizmo->CreateBehavior(MOUSE_LEFT_CONTROL);
	m_IsaClipWithGizmo->SetListener(this);
	m_IsaClipWithGizmo->SetVME(m_ImplicitPlaneVMEGizmo);

	if(!m_UseGizmo)
		m_ImplicitPlaneVMEGizmo->SetBehavior(m_IsaCompositor);
	else
		m_ImplicitPlaneVMEGizmo->SetBehavior(m_IsaCompositorWithArrowGizmo);
}

//----------------------------------------------------------------------------
void albaOpInteractiveClipSurface::UpdateISARefSys()
//----------------------------------------------------------------------------
{  
  m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneVMEGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
  m_IsaTranslate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneVMEGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
}
//----------------------------------------------------------------------------
void albaOpInteractiveClipSurface::SetClippingModality(int mode)
//----------------------------------------------------------------------------
{
  if (mode != albaOpInteractiveClipSurface::MODE_SURFACE && mode != albaOpInteractiveClipSurface::MODE_IMPLICIT_FUNCTION)
  {
    return;
  }

  m_ClipModality = mode;
}
//----------------------------------------------------------------------------
void albaOpInteractiveClipSurface::SetClippingSurface(albaVMESurface *surface)
//----------------------------------------------------------------------------
{
  m_ClipperVME = surface;
}
//----------------------------------------------------------------------------
void albaOpInteractiveClipSurface::SetImplicitPlanePosition(albaMatrix &matrix)
//----------------------------------------------------------------------------
{
  m_ImplicitPlaneVMEGizmo->SetAbsMatrix(matrix);
}

void albaOpInteractiveClipSurface::OnChooseConstrainVme( albaVME *vme )
{
  m_ConstrainalbaVMEPolylineGraph = vme;
  assert(m_ConstrainalbaVMEPolylineGraph);

  
  m_CASH->SetConstraintPolylineGraph(albaVMEPolylineGraph::SafeDownCast(m_ConstrainalbaVMEPolylineGraph));
  m_CASH->SetCurvilinearAbscissa(m_ActiveBranchId, 0.0);
  m_CASH->EnableWidgets(true)  ;
	GetLogicManager()->CameraUpdate();
}

void albaOpInteractiveClipSurface::GuiEnable()
{
  m_Gui->Enable(ID_GENERATE_CLIPPED_OUTPUT, m_ClipModality == albaOpInteractiveClipSurface::MODE_IMPLICIT_FUNCTION);
  m_Gui->Enable(ID_CHOOSE_GIZMO, m_ClipModality == albaOpInteractiveClipSurface::MODE_IMPLICIT_FUNCTION);
  m_Gui->Enable(ID_CHOOSE_SURFACE, m_ClipModality == albaOpInteractiveClipSurface::MODE_SURFACE);
  m_Gui->Enable(ID_CHOOSE_CONSTRAINT_VME, m_UseGizmo?false:true);
  m_Gui->Enable(wxOK,m_ClipModality == albaOpInteractiveClipSurface::MODE_IMPLICIT_FUNCTION);
}