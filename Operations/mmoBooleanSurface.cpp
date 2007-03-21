/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmoBooleanSurface.cpp,v $
Language:  C++
Date:      $Date: 2007-03-21 16:31:28 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi - Matteo Giacomoni
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

#include "mmoBooleanSurface.h"
#include "mafDecl.h"

#include "mmgGui.h"
#include "mafMatrix.h"
#include "mafAbsMatrixPipe.h"
#include "mafVMEGizmo.h"
#include "mafTransform.h"
#include "mmaMaterial.h"
#include "mmiCompositorMouse.h"
#include "mmiGenericMouse.h"
#include "vtkMAFSmartPointer.h"

#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkClipPolyData.h"
#include "vtkImplicitPolyData.h"
#include "vtkLinearSubdivisionFilter.h"
#include "vtkTriangleFilter.h"
#include "vtkPlaneSource.h"
#include "vtkArrowSource.h"
#include "vtkGlyph3D.h"
#include "vtkPlane.h"
#include "vtkProperty.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoBooleanSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoBooleanSurface::mmoBooleanSurface(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_InputPreserving = true;

	m_FirstOperatorVME = NULL;
	m_SecondOperatorVME = NULL;

	m_ImplicitPlaneGizmo  = NULL;

	m_ClipperPlane  = NULL;
	m_Arrow         = NULL;
	m_PlaneSource		= NULL;
	m_IsaCompositor = NULL;

	m_PlaneCreated = false;

	m_Modality = MODE_SURFACE;

	m_ClipInside = 0;
}
//----------------------------------------------------------------------------
mmoBooleanSurface::~mmoBooleanSurface()
//----------------------------------------------------------------------------
{
	for(int i=0;i<m_VTKResult.size();i++)
	{
		vtkDEL(m_VTKResult[i]);
	}
	m_VTKResult.clear();

	mafDEL(m_IsaCompositor);
	vtkDEL(m_ClipperPlane);
	vtkDEL(m_Arrow);
	vtkDEL(m_PlaneSource);
}
//----------------------------------------------------------------------------
bool mmoBooleanSurface::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	return (node && node->IsMAFType(mafVMESurface));
}
//----------------------------------------------------------------------------
mafOp *mmoBooleanSurface::Copy()   
//----------------------------------------------------------------------------
{
	return (new mmoBooleanSurface(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum FILTER_SURFACE_ID
{
	ID_CHOOSE_UNION = MINID,
	ID_CHOOSE_INTERSECTION,
	ID_CHOOSE_DIFFERENCE,
	ID_UNDO,
	ID_MODALITY,
	ID_CLIP,
	ID_CLIP_INSIDE,
};
//----------------------------------------------------------------------------
void mmoBooleanSurface::OpRun()   
//----------------------------------------------------------------------------
{  		
	m_SurfaceAccept = new mafSurfaceAccept;

	m_Gui = new mmgGui(this);

	m_FirstOperatorVME = mafVMESurface::SafeDownCast(m_Input);
	vtkPolyData *initialData;
	vtkNEW(initialData);
	initialData->DeepCopy((vtkPolyData*)m_FirstOperatorVME->GetOutput()->GetVTKData());
	m_VTKResult.push_back(initialData);

	wxString clip_by_choices[2] = {"surface","implicit function"};
	m_Gui->Combo(ID_MODALITY,"Modality",&m_Modality,2,clip_by_choices);

	m_Gui->Label("Surface:",true);
	m_Gui->Button(ID_CHOOSE_UNION,_("Union VME"));
	m_Gui->Button(ID_CHOOSE_INTERSECTION,_("Intersect VME"));
	m_Gui->Button(ID_CHOOSE_DIFFERENCE,_("Difference VME"));

	m_Gui->Label("Implicit:",true);
	m_Gui->Button(ID_CLIP,_("Clip"));
	m_Gui->Bool(ID_CLIP_INSIDE,"Clip Inside",&m_ClipInside,1);
	m_Gui->Enable(ID_CLIP,m_Modality == MODE_IMPLICIT_FUNCTION);
	m_Gui->Enable(ID_CLIP_INSIDE,m_Modality == MODE_IMPLICIT_FUNCTION);

	m_Gui->Button(ID_UNDO,_("Undo"));
	m_Gui->Enable(ID_UNDO,false);

	m_Gui->OkCancel();

	m_Gui->Enable(wxOK,false);

	ShowClipPlane(m_Modality != MODE_SURFACE);

	ShowGui();
}
//----------------------------------------------------------------------------
void mmoBooleanSurface::OpDo()
//----------------------------------------------------------------------------
{
	if(m_VTKResult.size()>1)
	{
		m_FirstOperatorVME->SetData((vtkPolyData*)m_VTKResult[m_VTKResult.size()-1],0);
		mafEventMacro(mafEvent(this,CAMERA_UPDATE));
	}
}
//----------------------------------------------------------------------------
void mmoBooleanSurface::OpUndo()
//----------------------------------------------------------------------------
{
	if(m_VTKResult.size()>1)
	{
		m_FirstOperatorVME->SetData((vtkPolyData*)m_VTKResult[0],0);
		mafEventMacro(mafEvent(this,CAMERA_UPDATE));
	}
}
//----------------------------------------------------------------------------
void mmoBooleanSurface::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
		case ID_CLIP:
			{
				Clip();
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			}
			break;
		case ID_CHOOSE_UNION:
			{
				mafString title = "Choose Union Surface";
				VmeChoose(title,e);
				Union();
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			}
			break;
		case ID_CHOOSE_INTERSECTION:
			{
				mafString title = "Choose Intersect Surface";
				VmeChoose(title,e);
				Intersection();

				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			}
			break;
		case ID_CHOOSE_DIFFERENCE:
			{
				mafString title = "Choose Difference Surface";
				VmeChoose(title,e);
				Difference();
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			}
			break;
		case ID_CLIP_INSIDE:
			if(m_Arrow) 
			{
				m_Arrow->SetScaleFactor(-1 * m_Arrow->GetScaleFactor());
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
			}
			break;
		case ID_UNDO:
			{
				Undo();
				ShowClipPlane(m_Modality != MODE_SURFACE);
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			}
			break;
		case ID_MODALITY:
			{
				m_Gui->Enable(ID_CHOOSE_UNION, m_Modality == MODE_SURFACE);
				m_Gui->Enable(ID_CHOOSE_INTERSECTION, m_Modality == MODE_SURFACE);
				m_Gui->Enable(ID_CHOOSE_DIFFERENCE, m_Modality == MODE_SURFACE);

				m_Gui->Enable(ID_CLIP,m_Modality == MODE_IMPLICIT_FUNCTION);
				m_Gui->Enable(ID_CLIP_INSIDE,m_Modality == MODE_IMPLICIT_FUNCTION);

				m_Gui->Update();
				ShowClipPlane(m_Modality != MODE_SURFACE);
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			}
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
		case wxOK:
			OpStop(OP_RUN_OK);        
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
void mmoBooleanSurface::Clip()
//----------------------------------------------------------------------------
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
	vtkMAFSmartPointer<vtkClipPolyData>clipper;
	clipper->SetInput(vtkPolyData::SafeDownCast(((mafVME *)m_Input)->GetOutput()->GetVTKData()));
	clipper->SetClipFunction(m_ClipperPlane);
	tr->Delete();
	mat->Delete();

	clipper->SetGenerateClipScalars(0); // 0 outputs input data scalars, 1 outputs implicit function values
	clipper->SetInsideOut(m_ClipInside);  // use 0/1 to reverse sense of clipping
	clipper->SetValue(0);               // use this to control clip distance from clipper function to surface
	clipper->Update();

	vtkPolyData *resultPolydata;
	vtkNEW(resultPolydata);
	resultPolydata->DeepCopy(clipper->GetOutput());
	resultPolydata->Update();

	int result=m_FirstOperatorVME->SetData(resultPolydata,0);

	if(result == MAF_ERROR)
		wxMessageBox("The result surface hasn't any points","ATTENCTION!",wxICON_EXCLAMATION);
	else
	{
		m_Gui->Enable(ID_UNDO,true);
		m_Gui->Enable(wxOK,true);
		m_VTKResult.push_back(resultPolydata);
	}

}
//----------------------------------------------------------------------------
void mmoBooleanSurface::UpdateISARefSys()
//----------------------------------------------------------------------------
{
	m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
	m_IsaRotate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());

	m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
	m_IsaTranslate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
}
//----------------------------------------------------------------------------
void mmoBooleanSurface::Undo()
//----------------------------------------------------------------------------
{
	if(m_VTKResult.size()>1)
	{
		vtkDEL(m_VTKResult[m_VTKResult.size()-1]);
		m_VTKResult.pop_back();
		m_FirstOperatorVME->SetData((vtkPolyData*)m_VTKResult[m_VTKResult.size()-1],0);
	}
	m_Gui->Enable(ID_UNDO,m_VTKResult.size()>1);
	m_Gui->Enable(wxOK,m_VTKResult.size()>1);
}
//----------------------------------------------------------------------------
void mmoBooleanSurface::Union()
//----------------------------------------------------------------------------
{
	if(m_FirstOperatorVME && m_SecondOperatorVME)
	{
		vtkPolyData *dataFirstOperator = vtkPolyData::SafeDownCast(m_FirstOperatorVME->GetOutput()->GetVTKData());
		vtkPolyData *dataSecondOperator = vtkPolyData::SafeDownCast(m_SecondOperatorVME->GetOutput()->GetVTKData());
		if(dataFirstOperator && dataSecondOperator)
		{
			vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFirstDataInput;
			transformFirstDataInput->SetTransform((vtkAbstractTransform *)((mafVME *)m_FirstOperatorVME)->GetAbsMatrixPipe()->GetVTKTransform());
			transformFirstDataInput->SetInput((vtkPolyData *)((mafVME *)m_FirstOperatorVME)->GetOutput()->GetVTKData());
			transformFirstDataInput->Update();

			vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformSecondDataInput;
			transformSecondDataInput->SetTransform((vtkAbstractTransform *)((mafVME *)m_SecondOperatorVME)->GetAbsMatrixPipe()->GetVTKTransform());
			transformSecondDataInput->SetInput((vtkPolyData *)((mafVME *)m_SecondOperatorVME)->GetOutput()->GetVTKData());
			transformSecondDataInput->Update();

			//Union
			vtkAppendPolyData *append;
			vtkNEW(append);
			append->SetInput(transformFirstDataInput->GetOutput());
			append->AddInput(transformSecondDataInput->GetOutput());
			append->Update();
			//End Union

			vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformResultDataInput;
			transformResultDataInput->SetTransform((vtkAbstractTransform *)((mafVME *)m_FirstOperatorVME)->GetAbsMatrixPipe()->GetVTKTransform()->GetInverse());
			transformResultDataInput->SetInput(append->GetOutput());
			transformResultDataInput->Update();

			vtkPolyData *resultPolydata;
			vtkNEW(resultPolydata);
			resultPolydata->DeepCopy(transformResultDataInput->GetOutput());
			m_VTKResult.push_back(resultPolydata);
		}
		else
		{
			return;
		}
		m_FirstOperatorVME->SetData((vtkPolyData*)m_VTKResult[m_VTKResult.size()-1],0);
		m_Gui->Enable(ID_UNDO,true);
		m_Gui->Enable(wxOK,true);
	}
}
//----------------------------------------------------------------------------
void mmoBooleanSurface::Intersection()
//----------------------------------------------------------------------------
{
	if(m_FirstOperatorVME && m_SecondOperatorVME)
	{
		vtkPolyData *dataFirstOperator = vtkPolyData::SafeDownCast(m_FirstOperatorVME->GetOutput()->GetVTKData());
		vtkPolyData *dataSecondOperator = vtkPolyData::SafeDownCast(m_SecondOperatorVME->GetOutput()->GetVTKData());
		if(dataFirstOperator && dataSecondOperator)
		{
			vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFirstDataInput;
			transformFirstDataInput->SetTransform((vtkAbstractTransform *)((mafVME *)m_FirstOperatorVME)->GetAbsMatrixPipe()->GetVTKTransform());
			transformFirstDataInput->SetInput((vtkPolyData *)((mafVME *)m_FirstOperatorVME)->GetOutput()->GetVTKData());
			transformFirstDataInput->Update();

			vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformSecondDataInput;
			transformSecondDataInput->SetTransform((vtkAbstractTransform *)((mafVME *)m_SecondOperatorVME)->GetAbsMatrixPipe()->GetVTKTransform());
			transformSecondDataInput->SetInput((vtkPolyData *)((mafVME *)m_SecondOperatorVME)->GetOutput()->GetVTKData());
			transformSecondDataInput->Update();

			//Intersection

			// clip input surface by another surface
			// triangulate input for subdivision filter
			vtkMAFSmartPointer<vtkTriangleFilter> triangles;
			triangles->SetInput(transformFirstDataInput->GetOutput());
			triangles->Update();

			// subdivide triangles in sphere 1 to get better clipping
			vtkMAFSmartPointer<vtkLinearSubdivisionFilter> subdivider;
			subdivider->SetInput(triangles->GetOutput());
			subdivider->SetNumberOfSubdivisions(3);   //  use  this  (0-3+)  to  see improvement in clipping
			subdivider->Update();

			vtkMAFSmartPointer<vtkImplicitPolyData> implicitPolyData;
			implicitPolyData->SetInput(transformSecondDataInput->GetOutput());

			vtkMAFSmartPointer<vtkClipPolyData>clipper;
			clipper->SetInput(subdivider->GetOutput());
			clipper->SetClipFunction(implicitPolyData);

			clipper->SetGenerateClipScalars(0); // 0 outputs input data scalars, 1 outputs implicit function values
			clipper->SetInsideOut(1);  // use 0/1 to reverse sense of clipping
			clipper->SetValue(0);               // use this to control clip distance from clipper function to surface
			clipper->Update();

			//End Intersection

			vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformResultDataInput;
			transformResultDataInput->SetTransform((vtkAbstractTransform *)((mafVME *)m_FirstOperatorVME)->GetAbsMatrixPipe()->GetVTKTransform()->GetInverse());
			transformResultDataInput->SetInput(clipper->GetOutput());
			transformResultDataInput->Update();

			vtkPolyData *resultPolydata;
			vtkNEW(resultPolydata);
			resultPolydata->DeepCopy(transformResultDataInput->GetOutput());
			resultPolydata->Update();

			int result=m_FirstOperatorVME->SetData(resultPolydata,0);

			if(result == MAF_ERROR)
				wxMessageBox("The result surface hasn't any points","ATTENCTION!",wxICON_EXCLAMATION);
			else
			{
				m_Gui->Enable(ID_UNDO,true);
				m_Gui->Enable(wxOK,true);
				m_VTKResult.push_back(resultPolydata);
			}
		}
		else
		{
			return;
		}
	}
}
//----------------------------------------------------------------------------
void mmoBooleanSurface::ShowClipPlane(bool show)
//----------------------------------------------------------------------------
{
	if(show)
	{
		if(m_ClipperPlane == NULL)
		{
			vtkNEW(m_ClipperPlane);
			vtkNEW(m_Arrow);
			vtkNEW(m_PlaneSource);
			mafNEW(m_ImplicitPlaneGizmo);
		}
			double b[6];
			((mafVME *)m_Input)->Update();
			((mafVME *)m_Input)->GetOutput()->GetVMEBounds(b);

			// bounding box dim
			double xdim = (b[1] - b[0])*3/2;
			double ydim = (b[3] - b[2])*3/2;
			double zdim = (b[5] - b[4])*3/2;

			// create the gizmo plane on the z = 0 plane
			m_PlaneSource->SetPoint1(xdim/2,-ydim/2, 0);
			m_PlaneSource->SetPoint2(-xdim/2, ydim/2, 0);
			m_PlaneSource->SetOrigin(-xdim/2,-ydim/2, 0);
			m_PlaneSource->Update();

			//vtkNEW(m_ClipperPlane);
			m_ClipperPlane->SetOrigin(m_PlaneSource->GetOrigin());
			m_ClipperPlane->SetNormal(m_PlaneSource->GetNormal());

			vtkMAFSmartPointer<vtkArrowSource> arrow_shape;
			arrow_shape->SetShaftResolution(40);
			arrow_shape->SetTipResolution(40);

			//vtkNEW(m_Arrow);
			m_Arrow->SetInput(m_PlaneSource->GetOutput());
			m_Arrow->SetSource(arrow_shape->GetOutput());
			m_Arrow->SetVectorModeToUseNormal();

			int clip_sign = m_ClipInside ? 1 : -1;
			double maxBound = (xdim >= ydim) ? (xdim >= zdim ? xdim : zdim) : (ydim >= zdim ? ydim : zdim); 
			m_Arrow->SetScaleFactor(clip_sign * abs(maxBound/10.0));
			m_Arrow->Update();

			vtkMAFSmartPointer<vtkAppendPolyData> gizmo;
			gizmo->AddInput(m_PlaneSource->GetOutput());
			gizmo->AddInput(m_Arrow->GetOutput());
			gizmo->Update();

			//mafNEW(m_ImplicitPlaneGizmo);
			m_ImplicitPlaneGizmo->SetData(gizmo->GetOutput());
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
			m_ImplicitPlaneGizmo->Modified();
			m_ImplicitPlaneGizmo->Update();

			mafEventMacro(mafEvent(this,VME_SHOW,m_ImplicitPlaneGizmo,true));
		/*}
		else
		{

		}*/
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
}
//----------------------------------------------------------------------------
void mmoBooleanSurface::AttachInteraction()
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
void mmoBooleanSurface::Difference()
//----------------------------------------------------------------------------
{
	if(m_FirstOperatorVME && m_SecondOperatorVME)
	{
		vtkPolyData *dataFirstOperator = vtkPolyData::SafeDownCast(m_FirstOperatorVME->GetOutput()->GetVTKData());
		vtkPolyData *dataSecondOperator = vtkPolyData::SafeDownCast(m_SecondOperatorVME->GetOutput()->GetVTKData());
		if(dataFirstOperator && dataSecondOperator)
		{
			vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFirstDataInput;
			transformFirstDataInput->SetTransform((vtkAbstractTransform *)((mafVME *)m_FirstOperatorVME)->GetAbsMatrixPipe()->GetVTKTransform());
			transformFirstDataInput->SetInput((vtkPolyData *)((mafVME *)m_FirstOperatorVME)->GetOutput()->GetVTKData());
			transformFirstDataInput->Update();

			vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformSecondDataInput;
			transformSecondDataInput->SetTransform((vtkAbstractTransform *)((mafVME *)m_SecondOperatorVME)->GetAbsMatrixPipe()->GetVTKTransform());
			transformSecondDataInput->SetInput((vtkPolyData *)((mafVME *)m_SecondOperatorVME)->GetOutput()->GetVTKData());
			transformSecondDataInput->Update();

			//Intersection

			// clip input surface by another surface
			// triangulate input for subdivision filter
			vtkMAFSmartPointer<vtkTriangleFilter> triangles;
			triangles->SetInput(transformFirstDataInput->GetOutput());
			triangles->Update();

			// subdivide triangles in sphere 1 to get better clipping
			vtkMAFSmartPointer<vtkLinearSubdivisionFilter> subdivider;
			subdivider->SetInput(triangles->GetOutput());
			subdivider->SetNumberOfSubdivisions(1);   //  use  this  (0-3+)  to  see improvement in clipping
			subdivider->Update();

			vtkMAFSmartPointer<vtkImplicitPolyData> implicitPolyData;
			implicitPolyData->SetInput(transformSecondDataInput->GetOutput());

			vtkClipPolyData *clipper;
			vtkNEW(clipper);
			clipper->SetInput(subdivider->GetOutput());
			clipper->SetClipFunction(implicitPolyData);

			clipper->SetGenerateClipScalars(0); // 0 outputs input data scalars, 1 outputs implicit function values
			clipper->SetInsideOut(0);  // use 0/1 to reverse sense of clipping
			clipper->SetValue(0);               // use this to control clip distance from clipper function to surface
			clipper->Update();

			//End Intersection

			vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformResultDataInput;
			transformResultDataInput->SetTransform((vtkAbstractTransform *)((mafVME *)m_FirstOperatorVME)->GetAbsMatrixPipe()->GetVTKTransform()->GetInverse());
			transformResultDataInput->SetInput(clipper->GetOutput());
			transformResultDataInput->Update();

			vtkPolyData *resultPolydata;
			vtkNEW(resultPolydata);
			resultPolydata->DeepCopy(transformResultDataInput->GetOutput());

			int result=m_FirstOperatorVME->SetData(resultPolydata,0);
			if(result == MAF_ERROR)
				wxMessageBox("The result surface hasn't any points","ATTENCTION!",wxICON_EXCLAMATION);
			else
			{
				m_Gui->Enable(ID_UNDO,true);
				m_Gui->Enable(wxOK,true);
				m_VTKResult.push_back(resultPolydata);
			}
		}
		else
		{
			return;
		}
	}
}
//----------------------------------------------------------------------------
void mmoBooleanSurface::VmeChoose(mafString title,mafEvent *e)
//----------------------------------------------------------------------------
{
	e->SetArg((long)m_SurfaceAccept);
	e->SetString(&title);
	e->SetId(VME_CHOOSE);
	mafEventMacro(*e);
	m_SecondOperatorVME = mafVMESurface::SafeDownCast(e->GetVme());
	if(m_SecondOperatorVME == NULL)
		return;
}
//----------------------------------------------------------------------------
void mmoBooleanSurface::OpStop(int result)
//----------------------------------------------------------------------------
{
	delete m_SurfaceAccept;

	if(m_ImplicitPlaneGizmo)
	{
		m_ImplicitPlaneGizmo->SetBehavior(NULL);
		mafEventMacro(mafEvent(this, VME_REMOVE, m_ImplicitPlaneGizmo));
	}
	mafDEL(m_ImplicitPlaneGizmo);

	if(result==OP_RUN_CANCEL)
	{		
		if(m_VTKResult.size()>1)
		{
			m_FirstOperatorVME->SetData((vtkPolyData*)m_VTKResult[0],0);
		}
	}

	HideGui();
	mafEventMacro(mafEvent(this,result));        
}