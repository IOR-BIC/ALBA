/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoClipSurface.cpp,v $
  Language:  C++
  Date:      $Date: 2007-05-10 09:07:40 $
  Version:   $Revision: 1.10 $
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


#include "mmoClipSurface.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgGui.h"

#include "mmaMaterial.h"
#include "mmiCompositorMouse.h"
#include "mmiGenericMouse.h"

#include "mafSmartPointer.h"
#include "mafVMEGizmo.h"
#include "mafAbsMatrixPipe.h"
#include "mafTransform.h"

#include "vtkProperty.h"
#include "vtkTransform.h"
#include "vtkPolyData.h"
#include "vtkPlane.h"
#include "vtkPlaneSource.h"
#include "vtkClipPolyData.h"
#include "vtkImplicitPolyData.h"
#include "vtkLinearSubdivisionFilter.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkArrowSource.h"
#include "vtkGlyph3D.h"
#include "vtkAppendPolyData.h"


//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoClipSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoClipSurface::mmoClipSurface(const wxString &label) :
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
  
  m_ImplicitPlaneGizmo  = NULL;
  m_IsaCompositor       = NULL;

  m_OldSurface = NULL;
	m_ResultPolyData = NULL;
	m_ClippedPolyData = NULL;
  
  ClipInside      = 1;
  m_ClipModality  = MODE_IMPLICIT_FUNCTION;
  
  PlaneCreated = false;

	m_PlaneHeight = 0.0;
	m_PlaneWidth	= 0.0;

	m_PlaneSource	= NULL;
	m_Gizmo				= NULL;
	m_ArrowShape	= NULL;
}

//----------------------------------------------------------------------------
mmoClipSurface::~mmoClipSurface()
//----------------------------------------------------------------------------
{
  mafDEL(m_IsaCompositor);
	mafDEL(m_ClippedVME);
  vtkDEL(m_ClipperPlane);
  vtkDEL(m_Clipper);
  vtkDEL(m_OldSurface);
  vtkDEL(m_Arrow);
}
//----------------------------------------------------------------------------
mafOp* mmoClipSurface::Copy()   
//----------------------------------------------------------------------------
{
	return new mmoClipSurface(m_Label);
}

//----------------------------------------------------------------------------
bool mmoClipSurface::Accept(mafNode *node)
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
};

//----------------------------------------------------------------------------
void mmoClipSurface::OpRun()   
//----------------------------------------------------------------------------
{
  vtkNEW(m_Clipper);

  wxString clip_by_choices[2] = {"surface","implicit function"};

  vtkNEW(m_OldSurface);
  m_OldSurface->DeepCopy((vtkPolyData*)((mafVME *)m_Input)->GetOutput()->GetVTKData());
  
  m_SurfaceAccept = new mafSurfaceAccept;

	if(!m_TestMode)
	{
		m_Gui = new mmgGui(this);
		m_Gui->Divider();
		m_Gui->Combo(ID_CLIP_BY,_("clip by"),&m_ClipModality,2,clip_by_choices);
		m_Gui->Button(ID_CHOOSE_SURFACE,_("clipper surface"));
		m_Gui->Bool(ID_CLIP_INSIDE,_("reverse clipping"),&ClipInside,1);
		double b[6];
		((mafVME *)m_Input)->GetOutput()->GetVMEBounds(b);
		// bounding box dim
		m_PlaneWidth = b[1] - b[0];
		m_PlaneHeight = b[3] - b[2];
		m_Gui->Double(ID_PLANE_WIDTH,_("plane w."),&m_PlaneWidth,0.0);
		m_Gui->Double(ID_PLANE_HEIGHT,_("plane h."),&m_PlaneHeight,0.0);
		m_Gui->Divider();
		m_Gui->Bool(ID_GENERATE_CLIPPED_OUTPUT,_("generate clipped output"),&m_GenerateClippedOutput,1);
		m_Gui->OkCancel();
		m_Gui->Enable(ID_GENERATE_CLIPPED_OUTPUT, m_ClipModality == mmoClipSurface::MODE_IMPLICIT_FUNCTION);
		m_Gui->Enable(ID_CHOOSE_SURFACE, m_ClipModality == mmoClipSurface::MODE_SURFACE);
		m_Gui->Enable(wxOK,m_ClipModality == mmoClipSurface::MODE_IMPLICIT_FUNCTION);
	  
		m_Gui->Divider();

		ShowGui();
	  
		ShowClipPlane(m_ClipModality != mmoClipSurface::MODE_SURFACE);
	}
}
//----------------------------------------------------------------------------
void mmoClipSurface::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_CHOOSE_SURFACE:
      {
        mafString title = "Choose m_Clipper Surface";
        e->SetArg((long)m_SurfaceAccept);
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
        m_Gui->Enable(ID_CHOOSE_SURFACE, m_ClipModality == mmoClipSurface::MODE_SURFACE);
				m_Gui->Enable(ID_GENERATE_CLIPPED_OUTPUT, m_ClipModality == mmoClipSurface::MODE_IMPLICIT_FUNCTION);
        m_Gui->Enable(wxOK,m_ClipModality == mmoClipSurface::MODE_IMPLICIT_FUNCTION);
        ClipInside = m_ClipModality;
        m_Gui->Update();
        ShowClipPlane(m_ClipModality != mmoClipSurface::MODE_SURFACE);
      break;
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
			case ID_PLANE_WIDTH:
			case ID_PLANE_HEIGHT:
				{
					if(PlaneCreated)
					{
						m_PlaneSource->SetPoint1(m_PlaneWidth/2,-m_PlaneHeight/2, 0);
						m_PlaneSource->SetPoint2(-m_PlaneWidth/2, m_PlaneHeight/2, 0);
						m_PlaneSource->SetOrigin(-m_PlaneWidth/2,-m_PlaneHeight/2, 0);
						m_PlaneSource->Update();
						mafEventMacro(mafEvent(this,CAMERA_UPDATE));
					}
				}
				break;
      case wxOK:
      {
        int clip_result = Clip();
        if (clip_result == MAF_ERROR)
        {
          wxMessageBox("Error while clipping surface!!", "Clipping Error");
          OpStop(OP_RUN_CANCEL);
        }
        OpStop(OP_RUN_OK);
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
void mmoClipSurface::OpStop(int result)
//----------------------------------------------------------------------------
{
  delete m_SurfaceAccept;

  if(m_ImplicitPlaneGizmo)
  {
    m_ImplicitPlaneGizmo->SetBehavior(NULL);
    mafEventMacro(mafEvent(this, VME_REMOVE, m_ImplicitPlaneGizmo));
  }
  mafDEL(m_ImplicitPlaneGizmo);
	vtkDEL(m_Gizmo);
	vtkDEL(m_ArrowShape);
	vtkDEL(m_PlaneSource);

  HideGui();
  mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mmoClipSurface::OpDo()
//----------------------------------------------------------------------------
{
	((mafVMESurface *)m_Input)->SetData(m_ResultPolyData,((mafVME *)m_Input)->GetTimeStamp());
	if(m_GenerateClippedOutput)
	{
		mafNEW(m_ClippedVME);
		m_ClippedVME->DeepCopy(m_Input);
		m_ClippedVME->SetData(m_ClippedPolyData,((mafVME *)m_Input)->GetTimeStamp());
		m_ClippedVME->SetName("clipped");
		m_ClippedVME->Update();

		m_ClippedVME->ReparentTo(m_Input->GetParent());
	}
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mmoClipSurface::OpUndo()
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
int mmoClipSurface::Clip()
//----------------------------------------------------------------------------
{
	if(!m_TestMode)
	{
    wxBusyCursor wait;
	}

  if(m_ClipModality == mmoClipSurface::MODE_SURFACE)
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

		vtkMAFSmartPointer<vtkImplicitPolyData> implicitPolyData;
		implicitPolyData->SetInput(transform_data_clipper->GetOutput());
		int num = subdivider->GetOutput()->GetNumberOfPoints();
		m_Clipper->SetInput(subdivider->GetOutput());
		m_Clipper->SetClipFunction(implicitPolyData);
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

	vtkNEW(m_ResultPolyData);

	m_Clipper->SetGenerateClipScalars(0); // 0 outputs input data scalars, 1 outputs implicit function values
	m_Clipper->SetInsideOut(ClipInside);  // use 0/1 to reverse sense of clipping
	m_Clipper->SetValue(0);               // use this to control clip distance from clipper function to surface
  m_Clipper->Update();

	m_ResultPolyData->DeepCopy(m_Clipper->GetOutput());
	m_ResultPolyData->Update();

	if(m_GenerateClippedOutput)
	{
		vtkNEW(m_ClippedPolyData);
		m_Clipper->SetInsideOut(ClipInside?0:1);
		m_Clipper->Update();

		m_ClippedPolyData->DeepCopy(m_Clipper->GetOutput());
		m_ClippedPolyData->Update();

	}
	
  return MAF_OK;
}
//----------------------------------------------------------------------------
void mmoClipSurface::ShowClipPlane(bool show)
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
      
      int clip_sign = ClipInside ? 1 : -1;
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
    
    if(!PlaneCreated)
    {
      material->m_Prop->SetColor(0.2,0.2,0.8);
      material->m_Prop->GetDiffuseColor(material->m_Diffuse);
      AttachInteraction();
      PlaneCreated = true;
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
void mmoClipSurface::AttachInteraction()
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

  m_ImplicitPlaneGizmo->SetBehavior(m_IsaCompositor);
}

//----------------------------------------------------------------------------
void mmoClipSurface::UpdateISARefSys()
//----------------------------------------------------------------------------
{
  m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
  m_IsaRotate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
  
  m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
  m_IsaTranslate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
}
//----------------------------------------------------------------------------
void mmoClipSurface::SetClippingModality(int mode)
//----------------------------------------------------------------------------
{
  if (mode != mmoClipSurface::MODE_SURFACE && mode != mmoClipSurface::MODE_IMPLICIT_FUNCTION)
  {
    return;
  }

  m_ClipModality = mode;
}
//----------------------------------------------------------------------------
void mmoClipSurface::SetClippingSurface(mafVMESurface *surface)
//----------------------------------------------------------------------------
{
  m_ClipperVME = surface;
}
//----------------------------------------------------------------------------
void mmoClipSurface::SetImplicitPlanePosition(mafMatrix &matrix)
//----------------------------------------------------------------------------
{
  m_ImplicitPlaneGizmo->SetAbsMatrix(matrix);
}
