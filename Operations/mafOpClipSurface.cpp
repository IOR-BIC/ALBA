/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpClipSurface.cpp,v $
  Language:  C++
  Date:      $Date: 2009-04-22 09:42:30 $
  Version:   $Revision: 1.3.2.1 $
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


#include "mafOpClipSurface.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "mmaMaterial.h"
#include "mmiCompositorMouse.h"
#include "mmiGenericMouse.h"

#include "mafSmartPointer.h"
#include "mafVMEGizmo.h"
#include "mafGizmoTranslate.h"
#include "mafGizmoRotate.h"
#include "mafGizmoScale.h"
#include "mafAbsMatrixPipe.h"
#include "mafTransform.h"

#include "vtkProperty.h"
#include "vtkTransform.h"
#include "vtkPolyData.h"
#include "vtkPlane.h"
#include "vtkPlaneSource.h"
#include "vtkClipPolyData.h"
#include "vtkMAFImplicitPolyData.h"
#include "vtkLinearSubdivisionFilter.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkArrowSource.h"
#include "vtkGlyph3D.h"
#include "vtkAppendPolyData.h"
#include "vtkMAFClipSurfaceBoundingBox.h"


//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpClipSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpClipSurface::mafOpClipSurface(const wxString &label) :
mafOp(label)
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
  
  m_ImplicitPlaneGizmo  = NULL;
  m_IsaCompositor       = NULL;

  m_OldSurface = NULL;
	m_ResultPolyData = NULL;
	m_ClippedPolyData = NULL;
  
  m_ClipInside      = 1;
	m_UseGizmo			=	1;
	m_ClipBoundBox	= 1;
  m_ClipModality  = MODE_IMPLICIT_FUNCTION;
	m_GizmoType			= GIZMO_TRANSLATE;
  
  m_PlaneCreated = false;

	m_PlaneHeight = 0.0;
	m_PlaneWidth	= 0.0;

	m_PlaneSource	= NULL;
	m_Gizmo				= NULL;
	m_ArrowShape	= NULL;

	m_GizmoTranslate	= NULL;
	m_GizmoRotate			= NULL;
	m_GizmoScale			= NULL;
}

//----------------------------------------------------------------------------
mafOpClipSurface::~mafOpClipSurface()
//----------------------------------------------------------------------------
{
	//cppDEL(m_GizmoTranslate);
	//cppDEL(m_GizmoRotate);
	//cppDEL(m_GizmoScale);

	vtkDEL(m_ResultPolyData);
	vtkDEL(m_ClippedPolyData);
  mafDEL(m_IsaCompositor);
	mafDEL(m_ClippedVME);
  vtkDEL(m_ClipperPlane);
  vtkDEL(m_Clipper);
	vtkDEL(m_ClipperBoundingBox);
  vtkDEL(m_OldSurface);
  vtkDEL(m_Arrow);
}
//----------------------------------------------------------------------------
mafOp* mafOpClipSurface::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpClipSurface(m_Label);
}

//----------------------------------------------------------------------------
bool mafOpClipSurface::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	return (node != NULL && node->IsMAFType(mafVMESurface));
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
};

//----------------------------------------------------------------------------
void mafOpClipSurface::OpRun()   
//----------------------------------------------------------------------------
{
  vtkNEW(m_Clipper);
	vtkNEW(m_ClipperBoundingBox);
  vtkNEW(m_OldSurface);

  m_OldSurface->DeepCopy((vtkPolyData*)((mafVME *)m_Input)->GetOutput()->GetVTKData());
 
	vtkNEW(m_ClippedPolyData);
	vtkNEW(m_ResultPolyData);
	m_ResultPolyData->DeepCopy(m_OldSurface);

	if(!m_TestMode)
	{
		CreateGui();
		ShowGui();
	  
		ShowClipPlane(m_ClipModality != mafOpClipSurface::MODE_SURFACE);
		CreateGizmos();
		//Enable & show Gizmos
		ChangeGizmo();
	}
}
//----------------------------------------------------------------------------
void mafOpClipSurface::CreateGizmos()
//----------------------------------------------------------------------------
{
	m_ImplicitPlaneGizmo->GetOutput()->GetVTKData()->ComputeBounds();
	m_ImplicitPlaneGizmo->Modified();
	m_ImplicitPlaneGizmo->Update();

	m_GizmoTranslate = new mafGizmoTranslate(mafVME::SafeDownCast(m_ImplicitPlaneGizmo), this);
	m_GizmoTranslate->SetRefSys(m_ImplicitPlaneGizmo);
	m_GizmoTranslate->Show(false);
	m_GizmoRotate = new mafGizmoRotate(mafVME::SafeDownCast(m_ImplicitPlaneGizmo), this);
	m_GizmoRotate->SetRefSys(m_ImplicitPlaneGizmo);
	m_GizmoRotate->Show(false);
	m_GizmoScale = new mafGizmoScale(mafVME::SafeDownCast(m_ImplicitPlaneGizmo), this);
	m_GizmoScale->SetRefSys(m_ImplicitPlaneGizmo);
	m_GizmoScale->Show(false);
}
//----------------------------------------------------------------------------
void mafOpClipSurface::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new mafGUI(this);
	m_Gui->Divider();
	wxString clip_by_choices[2] = {"surface","implicit function"};
	m_Gui->Combo(ID_CLIP_BY,_("clip by"),&m_ClipModality,2,clip_by_choices);
	m_Gui->Bool(ID_USE_GIZMO,_("use gizmo"),&m_UseGizmo,1);
	wxString gizmo_name[3] = {"translate","rotate","scale"};
	m_Gui->Combo(ID_CHOOSE_GIZMO,_("gizmo"),&m_GizmoType,3,gizmo_name);
	m_Gui->Button(ID_CHOOSE_SURFACE,_("clipper surface"));
	m_Gui->Bool(ID_CLIP_INSIDE,_("reverse clipping"),&m_ClipInside,1);
	double b[6];
	((mafVME *)m_Input)->GetOutput()->GetVMEBounds(b);
	// bounding box dim
	m_PlaneWidth = b[1] - b[0];
	m_PlaneHeight = b[3] - b[2];
	//m_Gui->Double(ID_PLANE_WIDTH,_("plane w."),&m_PlaneWidth,0.0);
	//m_Gui->Double(ID_PLANE_HEIGHT,_("plane h."),&m_PlaneHeight,0.0);
	//m_Gui->Divider();
	m_Gui->Bool(ID_GENERATE_CLIPPED_OUTPUT,_("generate clipped output"),&m_GenerateClippedOutput,1);
	m_Gui->OkCancel();

	m_Gui->Enable(ID_GENERATE_CLIPPED_OUTPUT, m_ClipModality == mafOpClipSurface::MODE_IMPLICIT_FUNCTION);
	m_Gui->Enable(ID_CHOOSE_GIZMO, m_ClipModality == mafOpClipSurface::MODE_IMPLICIT_FUNCTION);
	m_Gui->Enable(ID_CHOOSE_SURFACE, m_ClipModality == mafOpClipSurface::MODE_SURFACE);
	m_Gui->Enable(wxOK,m_ResultPolyData != NULL);

	m_Gui->Divider();
}
//----------------------------------------------------------------------------
void mafOpClipSurface::OnEventThis(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
		case ID_CHOOSE_SURFACE:
			{
				mafString title = "Choose m_Clipper Surface";
        e->SetArg((long)&mafOpClipSurface::SurfaceAccept);
				e->SetString(&title);
				e->SetId(VME_CHOOSE);
				mafEventMacro(*e);
				m_ClipperVME = mafVMESurface::SafeDownCast(e->GetVme());
				if(m_ClipperVME == NULL)
					return;
				m_Gui->Enable(wxOK,true);
			}
			break;
		case ID_CLIP_INSIDE:
			if(m_Arrow) 
			{
				m_Arrow->SetScaleFactor(-1 * m_Arrow->GetScaleFactor());
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
			}
			break;
		case ID_CLIP_BY:
			m_Gui->Enable(ID_CHOOSE_SURFACE, m_ClipModality == mafOpClipSurface::MODE_SURFACE);
			m_Gui->Enable(ID_CHOOSE_GIZMO, m_ClipModality == mafOpClipSurface::MODE_IMPLICIT_FUNCTION  && m_UseGizmo);
			m_Gui->Enable(ID_GENERATE_CLIPPED_OUTPUT, m_ClipModality == mafOpClipSurface::MODE_IMPLICIT_FUNCTION);
			m_Gui->Enable(ID_USE_GIZMO,m_ClipModality == mafOpClipSurface::MODE_IMPLICIT_FUNCTION);
			m_Gui->Enable(wxOK,m_ClipModality == mafOpClipSurface::MODE_IMPLICIT_FUNCTION);
			m_ClipInside = m_ClipModality;
			m_Gui->Update();
			ShowClipPlane(m_ClipModality != mafOpClipSurface::MODE_SURFACE);
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
					mafEventMacro(mafEvent(this,CAMERA_UPDATE));
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

				m_Gui->Enable(ID_CHOOSE_GIZMO, m_ClipModality == mafOpClipSurface::MODE_IMPLICIT_FUNCTION  && m_UseGizmo);
				
				ChangeGizmo();
			}
			break;
		case wxOK:
			{
				int clip_result = Clip();
				if (clip_result == MAF_ERROR)
				{
					wxMessageBox("Error while clipping surface!!", "Clipping Error");
				}
				else
					OpStop(OP_RUN_OK);

				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			}
			break;
		case wxCANCEL:
			OpStop(OP_RUN_CANCEL);
			break;
		default:
			mafEventMacro(*e);
			break; 
		}
	}
}
//----------------------------------------------------------------------------
void mafOpClipSurface::ClipBoundingBox()
//----------------------------------------------------------------------------
{

	vtkMAFSmartPointer<vtkTransformPolyDataFilter> transform_plane;
	transform_plane->SetTransform(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetVTKTransform());
	transform_plane->SetInput(m_PlaneSource->GetOutput());
	transform_plane->Update();

	vtkMAFSmartPointer<vtkTransformPolyDataFilter> transform_data_input;
	transform_data_input->SetTransform(((mafVME*)m_Input)->GetAbsMatrixPipe()->GetVTKTransform());
	transform_data_input->SetInput(m_OldSurface);
	transform_data_input->Update();

	m_ClipperBoundingBox->SetInput(transform_data_input->GetOutput());
	m_ClipperBoundingBox->SetMask(transform_plane->GetOutput());
	m_ClipperBoundingBox->SetClipInside(m_ClipInside);
	m_ClipperBoundingBox->Update();

	m_ResultPolyData->DeepCopy(m_ClipperBoundingBox->GetOutput());
	m_ResultPolyData->Update();

	int result=((mafVMESurface*)m_Input)->SetData(m_ResultPolyData,((mafVME*)m_Input)->GetTimeStamp());

	if(m_GenerateClippedOutput)
	{
		m_ClipperBoundingBox->SetClipInside(m_ClipInside?0:1);
		m_ClipperBoundingBox->Update();

		m_ClippedPolyData->DeepCopy(m_ClipperBoundingBox->GetOutput());
		m_ClippedPolyData->Update();
	}

	if(!m_TestMode)
		m_Gui->Enable(wxOK,true);
}
//----------------------------------------------------------------------------
void mafOpClipSurface::OnEventGizmoTranslate(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	switch(maf_event->GetId())
	{
	case ID_TRANSFORM:
		{    
			// post multiplying matrixes coming from the gizmo to the vme
			// gizmo does not set vme pose  since they cannot scale
			PostMultiplyEventMatrix(maf_event);
		}
		break;

	default:
		{
			mafEventMacro(*maf_event);
		}
	}
}
//----------------------------------------------------------------------------
void mafOpClipSurface::OnEventGizmoRotate(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	switch(maf_event->GetId())
	{
	case ID_TRANSFORM:
		{    
			// post multiplying matrixes coming from the gizmo to the vme
			// gizmo does not set vme pose  since they cannot scale
			PostMultiplyEventMatrix(maf_event);
		}
		break;

	default:
		{
			mafEventMacro(*maf_event);
		}
	}
}
//----------------------------------------------------------------------------
void mafOpClipSurface::OnEventGizmoScale(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{ 
	switch(maf_event->GetId())
	{
	case ID_TRANSFORM:
		{ 
			mafEventMacro(mafEvent(this, CAMERA_UPDATE));
		}
		break;

	default:
		{
			mafEventMacro(*maf_event);
		}
	}
}
//----------------------------------------------------------------------------
void mafOpClipSurface::OnEventGizmoPlane(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(maf_event->GetId())
		{
		case ID_TRANSFORM:
			{
				vtkTransform *currTr = vtkTransform::New();
				currTr->PostMultiply();
				currTr->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetVTKTransform()->GetMatrix());
				currTr->Concatenate(e->GetMatrix()->GetVTKMatrix());
				currTr->Update();

				mafMatrix newAbsMatr;
				newAbsMatr.DeepCopy(currTr->GetMatrix());
				newAbsMatr.SetTimeStamp(m_ImplicitPlaneGizmo->GetTimeStamp());

				// set the new pose to the gizmo
				m_ImplicitPlaneGizmo->SetAbsMatrix(newAbsMatr);
				UpdateISARefSys();

				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
				currTr->Delete();
			}
			break;
		default:
			{
				mafEventMacro(*maf_event);
			}
		}
	}
}
//----------------------------------------------------------------------------
void mafOpClipSurface::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (maf_event->GetSender() == this->m_Gui) // from this operation gui
	{
		OnEventThis(maf_event); 
	}
	else if(maf_event->GetSender() == this->m_GizmoTranslate)
	{
		OnEventGizmoTranslate(maf_event);
	}
	else if(maf_event->GetSender() == this->m_GizmoRotate)
	{
		OnEventGizmoRotate(maf_event);
	}
	else if(maf_event->GetSender() == this->m_GizmoScale)
	{
		OnEventGizmoScale(maf_event);
	}
	else
	{
		OnEventGizmoPlane(maf_event);
	}
}
//----------------------------------------------------------------------------
void mafOpClipSurface::ChangeGizmo()
//----------------------------------------------------------------------------
{
	if(m_ClipModality==mafOpClipSurface::MODE_IMPLICIT_FUNCTION)
	{
		m_GizmoTranslate->Show(m_GizmoType==GIZMO_TRANSLATE && m_UseGizmo);
		m_GizmoRotate->Show(m_GizmoType==GIZMO_ROTATE && m_UseGizmo);
		m_GizmoScale->Show(m_GizmoType==GIZMO_SCALE && m_UseGizmo,m_GizmoType==GIZMO_SCALE && m_UseGizmo,false,m_GizmoType==GIZMO_SCALE && m_UseGizmo);
		
		if(m_UseGizmo)
		{
			switch(m_GizmoType)
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
	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpClipSurface::OpStop(int result)
//----------------------------------------------------------------------------
{
  if(m_ImplicitPlaneGizmo)
  {
    m_ImplicitPlaneGizmo->SetBehavior(NULL);
    mafEventMacro(mafEvent(this, VME_REMOVE, m_ImplicitPlaneGizmo));
  }
  mafDEL(m_ImplicitPlaneGizmo);
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
  mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mafOpClipSurface::OpDo()
//----------------------------------------------------------------------------
{
	vtkMAFSmartPointer<vtkTransformPolyDataFilter> transform_output;
	transform_output->SetTransform((vtkAbstractTransform *)((mafVME *)m_Input)->GetAbsMatrixPipe()->GetVTKTransform()->GetInverse());
	transform_output->SetInput(m_ResultPolyData);
	transform_output->Update();

	((mafVMESurface *)m_Input)->SetData(transform_output->GetOutput(),((mafVME *)m_Input)->GetTimeStamp());
	if(m_GenerateClippedOutput)
	{
		vtkMAFSmartPointer<vtkTransformPolyDataFilter> transform_clipped_output;
		transform_clipped_output->SetTransform((vtkAbstractTransform *)((mafVME *)m_Input)->GetAbsMatrixPipe()->GetVTKTransform()->GetInverse());
		transform_clipped_output->SetInput(m_ClippedPolyData);
		transform_clipped_output->Update();

		mafNEW(m_ClippedVME);
		m_ClippedVME->DeepCopy(m_Input);
		m_ClippedVME->SetData(transform_clipped_output->GetOutput(),((mafVME *)m_Input)->GetTimeStamp());
		m_ClippedVME->SetName("clipped");
		m_ClippedVME->Update();

		m_ClippedVME->ReparentTo(m_Input->GetParent());
	}
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpClipSurface::OpUndo()
//----------------------------------------------------------------------------
{
  ((mafVMESurface *)m_Input)->SetData(m_OldSurface,((mafVME *)m_Input)->GetTimeStamp());
	if(m_GenerateClippedOutput)
	{
		m_ClippedVME->ReparentTo(NULL);
		mafDEL(m_ClippedVME);
	}
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
int mafOpClipSurface::Clip()
//----------------------------------------------------------------------------
{
	if(!m_TestMode)
	{
    wxBusyCursor wait;
	}

  if(m_ClipModality == mafOpClipSurface::MODE_SURFACE)
	{
    if(m_ClipperVME == NULL)
      return MAF_ERROR;

    vtkMAFSmartPointer<vtkTransformPolyDataFilter> transform_data_input;
    transform_data_input->SetTransform((vtkAbstractTransform *)((mafVME *)m_Input)->GetAbsMatrixPipe()->GetVTKTransform());
    transform_data_input->SetInput((vtkPolyData *)((mafVME *)m_Input)->GetOutput()->GetVTKData());
    transform_data_input->Update();

    // clip input surface by another surface
    // triangulate input for subdivision filter
		vtkMAFSmartPointer<vtkTriangleFilter> triangles;
		triangles->SetInput(transform_data_input->GetOutput());
    triangles->Update();
		
		// subdivide triangles in sphere 1 to get better clipping
		vtkMAFSmartPointer<vtkLinearSubdivisionFilter> subdivider;
		subdivider->SetInput(triangles->GetOutput());
		subdivider->SetNumberOfSubdivisions(1);   //  use  this  (0-3+)  to  see improvement in clipping
    subdivider->Update();
		
    m_ClipperVME->Update();
    vtkMAFSmartPointer<vtkTransformPolyDataFilter> transform_data_clipper;
    transform_data_clipper->SetTransform((vtkAbstractTransform *)m_ClipperVME->GetAbsMatrixPipe()->GetVTKTransform());
    transform_data_clipper->SetInput((vtkPolyData *)m_ClipperVME->GetOutput()->GetVTKData());
    transform_data_clipper->Update();

		vtkMAFSmartPointer<vtkMAFImplicitPolyData> implicitPolyData;
		implicitPolyData->SetInput(transform_data_clipper->GetOutput());
		m_Clipper->SetInput(subdivider->GetOutput());
		m_Clipper->SetClipFunction(implicitPolyData);
	}
  else
  {
		if(m_ClipBoundBox)
		{
			ClipBoundingBox();
			return MAF_OK;
		}
		else
		{
			vtkMatrix4x4 *mat = vtkMatrix4x4::New();
			mat->DeepCopy(((mafVME *)m_Input)->GetAbsMatrixPipe()->GetMatrixPointer()->GetVTKMatrix());
			mat->Invert();
			mat->Modified();

			// clip input surface by an implicit plane
			vtkTransform *tr = vtkTransform::New();
			tr->Concatenate(mat);
			tr->Concatenate(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetVTKTransform()->GetMatrix());
			tr->Inverse();
			tr->Update();

			m_ClipperPlane->SetTransform(tr);
			m_Clipper->SetInput(vtkPolyData::SafeDownCast(((mafVME *)m_Input)->GetOutput()->GetVTKData()));
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
	m_ResultPolyData->Update();

	if(m_GenerateClippedOutput)
	{

		m_Clipper->SetInsideOut(m_ClipInside?0:1);
		m_Clipper->Update();

		m_ClippedPolyData->DeepCopy(m_Clipper->GetOutput());
		m_ClippedPolyData->Update();

	}

	if(!m_TestMode) 
		m_Gui->Enable(wxOK,true);

  return MAF_OK;
}
//----------------------------------------------------------------------------
void mafOpClipSurface::PostMultiplyEventMatrix(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		long arg = e->GetArg();

		// handle incoming transform events
		vtkTransform *tr = vtkTransform::New();
		tr->PostMultiply();
		tr->SetMatrix(((mafVME *)m_ImplicitPlaneGizmo)->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
		tr->Concatenate(e->GetMatrix()->GetVTKMatrix());
		tr->Update();

		mafMatrix absPose;
		absPose.DeepCopy(tr->GetMatrix());
		absPose.SetTimeStamp(0.0);

		if (arg == mmiGenericMouse::MOUSE_MOVE)
		{
			// move vme
			((mafVME *)m_ImplicitPlaneGizmo)->SetAbsMatrix(absPose);
			// update matrix for OpDo()
			//m_NewAbsMatrix = absPose;
		} 
		mafEventMacro(mafEvent(this, CAMERA_UPDATE));

		// clean up
		tr->Delete();
	}
}
//----------------------------------------------------------------------------
void mafOpClipSurface::ShowClipPlane(bool show)
//----------------------------------------------------------------------------
{
  if(show)
  {
    if(m_ClipperPlane == NULL)
    {
      double b[6];
      ((mafVME *)m_Input)->GetOutput()->GetVMEBounds(b);

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
      m_Arrow->SetInput(m_PlaneSource->GetOutput());
      m_Arrow->SetSource(m_ArrowShape->GetOutput());
      m_Arrow->SetVectorModeToUseNormal();
      
      int clip_sign = m_ClipInside ? 1 : -1;
      m_Arrow->SetScaleFactor(clip_sign * abs(zdim/10.0));
      m_Arrow->Update();

      vtkNEW(m_Gizmo);
      m_Gizmo->AddInput(m_PlaneSource->GetOutput());
      m_Gizmo->AddInput(m_Arrow->GetOutput());
      m_Gizmo->Update();

      mafNEW(m_ImplicitPlaneGizmo);
      m_ImplicitPlaneGizmo->SetData(m_Gizmo->GetOutput());
      m_ImplicitPlaneGizmo->SetName("implicit plane gizmo");
      m_ImplicitPlaneGizmo->ReparentTo(mafVME::SafeDownCast(m_Input->GetRoot()));

      // position the plane
      mafSmartPointer<mafTransform> currTr;
      currTr->Translate((b[0] + b[1]) / 2, (b[2] + b[3]) / 2,(b[4] + b[5]) / 2 , POST_MULTIPLY);
      currTr->Update();

      mafMatrix mat;
      mat.DeepCopy(&currTr->GetMatrix());
      mat.SetTimeStamp(((mafVME *)m_Input)->GetTimeStamp());

      m_ImplicitPlaneGizmo->SetAbsMatrix(mat);

      mafEventMacro(mafEvent(this,VME_SHOW,m_ImplicitPlaneGizmo,true));
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

  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpClipSurface::AttachInteraction()
//----------------------------------------------------------------------------
{
  mafNEW(m_IsaCompositor);

  m_IsaRotate = m_IsaCompositor->CreateBehavior(MOUSE_LEFT);
  m_IsaRotate->SetListener(this);
  m_IsaRotate->SetVME(m_ImplicitPlaneGizmo);
  m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetTypeToView();
  m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
  m_IsaRotate->GetPivotRefSys()->SetTypeToView();
  m_IsaRotate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
  m_IsaRotate->GetRotationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
  m_IsaRotate->EnableRotation(true);

  m_IsaTranslate = m_IsaCompositor->CreateBehavior(MOUSE_MIDDLE);
  m_IsaTranslate->SetListener(this);
  m_IsaTranslate->SetVME(m_ImplicitPlaneGizmo);
  m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
  m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
  m_IsaTranslate->GetPivotRefSys()->SetTypeToView();
  m_IsaTranslate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
  m_IsaTranslate->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
  m_IsaTranslate->EnableTranslation(true);

	if(!m_UseGizmo)
		m_ImplicitPlaneGizmo->SetBehavior(m_IsaCompositor);
}

//----------------------------------------------------------------------------
void mafOpClipSurface::UpdateISARefSys()
//----------------------------------------------------------------------------
{
  m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
  m_IsaRotate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
  
  m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
  m_IsaTranslate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
}
//----------------------------------------------------------------------------
void mafOpClipSurface::SetClippingModality(int mode)
//----------------------------------------------------------------------------
{
  if (mode != mafOpClipSurface::MODE_SURFACE && mode != mafOpClipSurface::MODE_IMPLICIT_FUNCTION)
  {
    return;
  }

  m_ClipModality = mode;
}
//----------------------------------------------------------------------------
void mafOpClipSurface::SetClippingSurface(mafVMESurface *surface)
//----------------------------------------------------------------------------
{
  m_ClipperVME = surface;
}
//----------------------------------------------------------------------------
void mafOpClipSurface::SetImplicitPlanePosition(mafMatrix &matrix)
//----------------------------------------------------------------------------
{
  m_ImplicitPlaneGizmo->SetAbsMatrix(matrix);
}
