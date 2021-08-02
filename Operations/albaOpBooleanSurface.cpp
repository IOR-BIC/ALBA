/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpBooleanSurface
 Authors: Daniele Giunchi - Matteo Giacomoni
 
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

#include "albaOpBooleanSurface.h"
#include "albaDecl.h"

#include "albaRefSys.h"
#include "albaGUI.h"
#include "albaMatrix.h"
#include "albaAbsMatrixPipe.h"
#include "albaVMEGizmo.h"
#include "albaTransform.h"
#include "mmaMaterial.h"
#include "albaInteractorCompositorMouse.h"
#include "albaInteractorGenericMouse.h"
#include "vtkALBASmartPointer.h"

#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkClipPolyData.h"
#include "vtkALBAImplicitPolyData.h"
#include "vtkLinearSubdivisionFilter.h"
#include "vtkTriangleFilter.h"
#include "vtkPlaneSource.h"
#include "vtkArrowSource.h"
#include "vtkGlyph3D.h"
#include "vtkPlane.h"
#include "vtkProperty.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpBooleanSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpBooleanSurface::albaOpBooleanSurface(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_InputPreserving = true;

	m_FirstOperatorVME = NULL;
	m_SecondOperatorVME = NULL;
  m_SecondOperatorFromParametric = NULL;
  m_ResultVME = NULL;

	m_ImplicitPlaneGizmo  = NULL;

	m_ClipperPlane  = NULL;
	m_Arrow         = NULL;
	m_PlaneSource		= NULL;
	m_IsaCompositor = NULL;

	m_PlaneCreated = false;

	m_Modality = MODE_SURFACE;

	m_ClipInside = 0;
	m_Subdivision = 1;
}
//----------------------------------------------------------------------------
albaOpBooleanSurface::~albaOpBooleanSurface()
//----------------------------------------------------------------------------
{
	for(int i=0;i<m_VTKResult.size();i++)
	{
		vtkDEL(m_VTKResult[i]);
	}
	m_VTKResult.clear();

	albaDEL(m_IsaCompositor);
	vtkDEL(m_ClipperPlane);
	vtkDEL(m_Arrow);
	vtkDEL(m_PlaneSource);

  albaDEL(m_ImplicitPlaneGizmo);

  albaDEL(m_SecondOperatorFromParametric);
  albaDEL(m_ResultVME);
}
//----------------------------------------------------------------------------
bool albaOpBooleanSurface::Accept(albaVME*node)
//----------------------------------------------------------------------------
{
	return (node && (node->IsALBAType(albaVMESurface))); //|| node->IsALBAType(albaVMESurfaceParametric)));
}
//----------------------------------------------------------------------------
albaOp *albaOpBooleanSurface::Copy()   
//----------------------------------------------------------------------------
{
	return (new albaOpBooleanSurface(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum FILTER_SURFACE_ID
{
	ID_CHOOSE_UNION = MINID,
	ID_CHOOSE_INTERSECTION,
	ID_CHOOSE_DIFFERENCE,
	ID_SUBDIVISION,
	ID_UNDO,
	ID_MODALITY,
	ID_CLIP,
	ID_CLIP_INSIDE,
};
//----------------------------------------------------------------------------
void albaOpBooleanSurface::OpRun()   
//----------------------------------------------------------------------------
{  		
	m_Gui = new albaGUI(this);

  //creation of vme result
  albaNEW(m_ResultVME);
	vtkALBASmartPointer<vtkPolyData> poly;
	poly->DeepCopy((vtkPolyData*)m_Input->GetOutput()->GetVTKData());
  m_ResultVME->SetData(poly,m_Input->GetTimeStamp());
  albaString name = _("bool_");
  name << m_Input->GetName();
  m_ResultVME->SetAbsMatrix(*m_Input->GetOutput()->GetAbsMatrix());
  m_ResultVME->SetName(name);
  m_ResultVME->Modified();
  m_ResultVME->Update();

  m_ResultVME->ReparentTo(m_Input->GetRoot());

  GetLogicManager()->VmeShow(m_ResultVME, true);

	m_FirstOperatorVME = m_ResultVME;
	vtkPolyData *initialData;
	vtkNEW(initialData);
	initialData->DeepCopy((vtkPolyData*)m_FirstOperatorVME->GetOutput()->GetVTKData());
	m_VTKResult.push_back(initialData);

	//wxString clip_by_choices[2] = {"surface","implicit function"};
	//m_Gui->Combo(ID_MODALITY,"Modality",&m_Modality,2,clip_by_choices);

	m_Gui->Label("Surface:",true);
	m_Gui->Button(ID_CHOOSE_UNION,_("Union VME"));
	m_Gui->Button(ID_CHOOSE_INTERSECTION,_("Intersect VME"));
	m_Gui->Button(ID_CHOOSE_DIFFERENCE,_("Difference VME"));
	m_Gui->Integer(ID_SUBDIVISION,_("Subdivide"), &m_Subdivision,1,10);

	/*m_Gui->Label("Implicit:",true);
	m_Gui->Button(ID_CLIP,_("Clip"));
	m_Gui->Bool(ID_CLIP_INSIDE,"Clip Inside",&m_ClipInside,1);
	m_Gui->Enable(ID_CLIP,m_Modality == MODE_IMPLICIT_FUNCTION);
	m_Gui->Enable(ID_CLIP_INSIDE,m_Modality == MODE_IMPLICIT_FUNCTION);*/

	m_Gui->Button(ID_UNDO,_("Undo"));
	m_Gui->Enable(ID_UNDO,false);

	m_Gui->OkCancel();

	m_Gui->Enable(wxOK,false);

	ShowClipPlane(m_Modality == MODE_IMPLICIT_FUNCTION);

	ShowGui();
}
//----------------------------------------------------------------------------
void albaOpBooleanSurface::OpDo()
//----------------------------------------------------------------------------
{
	m_ResultVME->ReparentTo(m_Input->GetRoot());
}
//----------------------------------------------------------------------------
void albaOpBooleanSurface::OpUndo()
//----------------------------------------------------------------------------
{
	 GetLogicManager()->VmeRemove(m_ResultVME);
}
//----------------------------------------------------------------------------
void albaOpBooleanSurface::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId())
		{
		case ID_CLIP:
			{
				Clip();
				GetLogicManager()->CameraUpdate();
			}
			break;
		case ID_CHOOSE_UNION:
			{
				albaString title = "Choose Union Surface";
				VmeChoose(title,e);
        if(m_FirstOperatorVME == m_SecondOperatorVME || m_Input == m_SecondOperatorVME)
        {
          albaMessage(_("Can't operate over the same VME"));
          return;
        }
				Union();
				GetLogicManager()->CameraUpdate();
			}
			break;
		case ID_CHOOSE_INTERSECTION:
			{
				albaString title = _("Choose Intersect Surface");
				VmeChoose(title,e);
        if(m_FirstOperatorVME == m_SecondOperatorVME || m_Input == m_SecondOperatorVME)
        {
          albaMessage(_("Can't operate over the same VME"));
          return;
        }
				Intersection();

				GetLogicManager()->CameraUpdate();
			}
			break;
		case ID_CHOOSE_DIFFERENCE:
			{
				albaString title = _("Choose Difference Surface");
				VmeChoose(title,e);
        if(m_FirstOperatorVME == m_SecondOperatorVME || m_Input == m_SecondOperatorVME)
        {
          albaMessage(_("Can't operate over the same VME"));
          return;
        }
				Difference();
				GetLogicManager()->CameraUpdate();
			}
			break;
		case ID_CLIP_INSIDE:
			if(m_Arrow) 
			{
				m_Arrow->SetScaleFactor(-1 * m_Arrow->GetScaleFactor());
				GetLogicManager()->CameraUpdate();
			}
			break;
		case ID_UNDO:
			{
				Undo();
				ShowClipPlane(m_Modality != MODE_SURFACE);
				GetLogicManager()->CameraUpdate();
			}
			break;
		case ID_MODALITY:
			{
				m_Gui->Enable(ID_CHOOSE_UNION, m_Modality == MODE_SURFACE);
				m_Gui->Enable(ID_CHOOSE_INTERSECTION, m_Modality == MODE_SURFACE);
				m_Gui->Enable(ID_CHOOSE_DIFFERENCE, m_Modality == MODE_SURFACE);
				m_Gui->Enable(ID_SUBDIVISION, m_Modality == MODE_SURFACE);

				m_Gui->Enable(ID_CLIP,m_Modality == MODE_IMPLICIT_FUNCTION);
				m_Gui->Enable(ID_CLIP_INSIDE,m_Modality == MODE_IMPLICIT_FUNCTION);

				m_Gui->Update();
				ShowClipPlane(m_Modality == MODE_IMPLICIT_FUNCTION);
				GetLogicManager()->CameraUpdate();
			}
			break;
		case ID_TRANSFORM:
			{
				vtkALBASmartPointer<vtkTransform> currTr;
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
			}
			break;
		case wxOK:
			OpStop(OP_RUN_OK);        
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
void albaOpBooleanSurface::Clip()
//----------------------------------------------------------------------------
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
	vtkALBASmartPointer<vtkClipPolyData>clipper;
	clipper->SetInput(vtkPolyData::SafeDownCast(m_FirstOperatorVME->GetOutput()->GetVTKData()));
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

	if(result == ALBA_ERROR)
	{
		albaMessage(_("The result surface hasn't any points"),_("Warning"),wxICON_EXCLAMATION);
		vtkDEL(resultPolydata);
	}
	else
	{
		m_Gui->Enable(ID_UNDO,true);
		m_Gui->Enable(wxOK,true);
		m_VTKResult.push_back(resultPolydata);
	}

}
//----------------------------------------------------------------------------
void albaOpBooleanSurface::UpdateISARefSys()
//----------------------------------------------------------------------------
{
	m_IsaRotate->GetRotationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
	m_IsaRotate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());

	m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
	m_IsaTranslate->GetPivotRefSys()->SetMatrix(m_ImplicitPlaneGizmo->GetAbsMatrixPipe()->GetMatrixPointer());
}
//----------------------------------------------------------------------------
void albaOpBooleanSurface::Undo()
//----------------------------------------------------------------------------
{
	if(m_VTKResult.size()>1)
	{
		vtkDEL(m_VTKResult[m_VTKResult.size()-1]);
		m_VTKResult.pop_back();
		m_FirstOperatorVME->SetData((vtkPolyData*)m_VTKResult[m_VTKResult.size()-1],m_Input->GetTimeStamp());
	}
	m_Gui->Enable(ID_UNDO,m_VTKResult.size()>1);
	m_Gui->Enable(wxOK,m_VTKResult.size()>1);
}
//----------------------------------------------------------------------------
void albaOpBooleanSurface::Union()
//----------------------------------------------------------------------------
{
	if(m_FirstOperatorVME && m_SecondOperatorVME)
	{
		vtkPolyData *dataFirstOperator = vtkPolyData::SafeDownCast(m_FirstOperatorVME->GetOutput()->GetVTKData());
		vtkPolyData *dataSecondOperator = vtkPolyData::SafeDownCast(m_SecondOperatorVME->GetOutput()->GetVTKData());
		if(dataFirstOperator && dataSecondOperator)
		{
			vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFirstDataInput;
			transformFirstDataInput->SetTransform((vtkAbstractTransform *)m_FirstOperatorVME->GetAbsMatrixPipe()->GetVTKTransform());
			transformFirstDataInput->SetInput((vtkPolyData *)m_FirstOperatorVME->GetOutput()->GetVTKData());
			transformFirstDataInput->Update();

			vtkALBASmartPointer<vtkTransformPolyDataFilter> transformSecondDataInput;
			transformSecondDataInput->SetTransform((vtkAbstractTransform *)m_SecondOperatorVME->GetAbsMatrixPipe()->GetVTKTransform());
			transformSecondDataInput->SetInput((vtkPolyData *)m_SecondOperatorVME->GetOutput()->GetVTKData());
			transformSecondDataInput->Update();

			//Union
			vtkALBASmartPointer<vtkAppendPolyData> append;
			append->SetInput(transformFirstDataInput->GetOutput());
			append->AddInput(transformSecondDataInput->GetOutput());
			append->Update();
			//End Union

			vtkALBASmartPointer<vtkTransformPolyDataFilter> transformResultDataInput;
			transformResultDataInput->SetTransform((vtkAbstractTransform *)m_FirstOperatorVME->GetAbsMatrixPipe()->GetVTKTransform()->GetInverse());
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
		m_FirstOperatorVME->SetData((vtkPolyData*)m_VTKResult[m_VTKResult.size()-1],m_Input->GetTimeStamp());
		if(!m_TestMode)
		{
			m_Gui->Enable(ID_UNDO,true);
			m_Gui->Enable(wxOK,true);
		}
	}
}
//----------------------------------------------------------------------------
void albaOpBooleanSurface::Intersection()
//----------------------------------------------------------------------------
{
	if(m_FirstOperatorVME && m_SecondOperatorVME)
	{
		vtkPolyData *dataFirstOperator = vtkPolyData::SafeDownCast(m_FirstOperatorVME->GetOutput()->GetVTKData());
		vtkPolyData *dataSecondOperator = vtkPolyData::SafeDownCast(m_SecondOperatorVME->GetOutput()->GetVTKData());
		
		if(dataFirstOperator && dataSecondOperator)
		{
			vtkTransformPolyDataFilter *transformFirstDataInput;
			vtkNEW(transformFirstDataInput);
			transformFirstDataInput->SetTransform((vtkAbstractTransform *)m_FirstOperatorVME->GetAbsMatrixPipe()->GetVTKTransform());
			transformFirstDataInput->SetInput((vtkPolyData *)m_FirstOperatorVME->GetOutput()->GetVTKData());
			transformFirstDataInput->Update();

			vtkTransformPolyDataFilter *transformSecondDataInput;
			vtkNEW(transformSecondDataInput);
			transformSecondDataInput->SetTransform((vtkAbstractTransform *)m_SecondOperatorVME->GetAbsMatrixPipe()->GetVTKTransform());
			transformSecondDataInput->SetInput((vtkPolyData *)m_SecondOperatorVME->GetOutput()->GetVTKData());
			transformSecondDataInput->Update();

			//Intersection

			// clip input surface by another surface
			// triangulate input for subdivision filter
			vtkTriangleFilter *triangles;
			vtkNEW(triangles);
			triangles->SetInput(transformFirstDataInput->GetOutput());
			triangles->Update();

			// subdivide triangles in sphere 1 to get better clipping
			vtkLinearSubdivisionFilter *subdivider;
			vtkNEW(subdivider);
			subdivider->SetInput(triangles->GetOutput());
			subdivider->SetNumberOfSubdivisions(m_Subdivision);   //  use  this  (0-3+)  to  see improvement in clipping
			subdivider->Update();

			vtkALBAImplicitPolyData *implicitPolyData;
			vtkNEW(implicitPolyData);
			implicitPolyData->SetInput(transformSecondDataInput->GetOutput());

			vtkClipPolyData *clipper;
			vtkNEW(clipper);
			clipper->SetInput(subdivider->GetOutput());
			clipper->SetClipFunction(implicitPolyData);

			clipper->SetGenerateClipScalars(0); // 0 outputs input data scalars, 1 outputs implicit function values
			clipper->SetInsideOut(1);  // use 0/1 to reverse sense of clipping
			clipper->SetValue(0);               // use this to control clip distance from clipper function to surface
			clipper->Update();

			//End Intersection

			vtkTransformPolyDataFilter *transformResultDataInput;
			vtkNEW(transformResultDataInput);
			transformResultDataInput->SetTransform((vtkAbstractTransform *)m_FirstOperatorVME->GetAbsMatrixPipe()->GetVTKTransform()->GetInverse());
			transformResultDataInput->SetInput(clipper->GetOutput());
			transformResultDataInput->Update();

			vtkPolyData *resultPolydata;
			vtkNEW(resultPolydata);
			resultPolydata->DeepCopy(transformResultDataInput->GetOutput());
			resultPolydata->Update();

			int result=m_FirstOperatorVME->SetData(resultPolydata,m_Input->GetTimeStamp());

			if(result == ALBA_ERROR)
			{
				albaMessage(_("The result surface hasn't any points"),_("Warning"),wxICON_EXCLAMATION);
				albaDEL(resultPolydata);
			}
			else
			{
				if(!m_TestMode)
				{
					m_Gui->Enable(ID_UNDO,true);
					m_Gui->Enable(wxOK,true);
				}
				m_VTKResult.push_back(resultPolydata);
			}
			vtkDEL(transformResultDataInput);
			vtkDEL(clipper);
			vtkDEL(implicitPolyData);
			vtkDEL(subdivider);
			vtkDEL(triangles);
			vtkDEL(transformSecondDataInput);
			vtkDEL(transformFirstDataInput);
		}
		else
		{
			return;
		}
	}
}
//----------------------------------------------------------------------------
void albaOpBooleanSurface::ShowClipPlane(bool show)
//----------------------------------------------------------------------------
{
	if(show)
	{
		if(m_ClipperPlane == NULL)
		{
			vtkNEW(m_ClipperPlane);
			vtkNEW(m_Arrow);
			vtkNEW(m_PlaneSource);
			albaNEW(m_ImplicitPlaneGizmo);
		}
			double b[6];
			m_Input->Update();
			m_Input->GetOutput()->GetVMEBounds(b);

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

			vtkALBASmartPointer<vtkArrowSource> arrow_shape;
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

			vtkALBASmartPointer<vtkAppendPolyData> gizmo;
			gizmo->AddInput(m_PlaneSource->GetOutput());
			gizmo->AddInput(m_Arrow->GetOutput());
			gizmo->Update();

			//albaNEW(m_ImplicitPlaneGizmo);
			m_ImplicitPlaneGizmo->SetData(gizmo->GetOutput());
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
			m_ImplicitPlaneGizmo->Modified();
			m_ImplicitPlaneGizmo->Update();

			GetLogicManager()->VmeShow(m_ImplicitPlaneGizmo, true);
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
void albaOpBooleanSurface::AttachInteraction()
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

	m_ImplicitPlaneGizmo->SetBehavior(m_IsaCompositor);
}
//----------------------------------------------------------------------------
void albaOpBooleanSurface::Difference()
//----------------------------------------------------------------------------
{
	if(m_FirstOperatorVME && m_SecondOperatorVME)
	{
		vtkPolyData *dataFirstOperator = vtkPolyData::SafeDownCast(m_FirstOperatorVME->GetOutput()->GetVTKData());
		vtkPolyData *dataSecondOperator = vtkPolyData::SafeDownCast(m_SecondOperatorVME->GetOutput()->GetVTKData());
		if(dataFirstOperator && dataSecondOperator)
		{
			vtkTransformPolyDataFilter *transformFirstDataInput;
			vtkNEW(transformFirstDataInput);
			transformFirstDataInput->SetTransform((vtkAbstractTransform *)m_FirstOperatorVME->GetAbsMatrixPipe()->GetVTKTransform());
			transformFirstDataInput->SetInput((vtkPolyData *)m_FirstOperatorVME->GetOutput()->GetVTKData());
			transformFirstDataInput->Update();

			vtkTransformPolyDataFilter *transformSecondDataInput;
			vtkNEW(transformSecondDataInput);
			transformSecondDataInput->SetTransform((vtkAbstractTransform *)m_SecondOperatorVME->GetAbsMatrixPipe()->GetVTKTransform());
			transformSecondDataInput->SetInput((vtkPolyData *)m_SecondOperatorVME->GetOutput()->GetVTKData());
			transformSecondDataInput->Update();

			//Intersection

			// clip input surface by another surface
			// triangulate input for subdivision filter
			vtkTriangleFilter *triangles;
			vtkNEW(triangles);
			triangles->SetInput(transformFirstDataInput->GetOutput());
			triangles->Update();

			// subdivide triangles in sphere 1 to get better clipping
			vtkLinearSubdivisionFilter *subdivider;
			vtkNEW(subdivider);
			subdivider->SetInput(triangles->GetOutput());
			subdivider->SetNumberOfSubdivisions(m_Subdivision);   //  use  this  (0-3+)  to  see improvement in clipping
			subdivider->Update();

			vtkALBAImplicitPolyData *implicitPolyData;
			vtkNEW(implicitPolyData);
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

			vtkTransformPolyDataFilter *transformResultDataInput;
			vtkNEW(transformResultDataInput);
			transformResultDataInput->SetTransform((vtkAbstractTransform *)m_FirstOperatorVME->GetAbsMatrixPipe()->GetVTKTransform()->GetInverse());
			transformResultDataInput->SetInput(clipper->GetOutput());
			transformResultDataInput->Update();

			vtkPolyData *resultPolydata;
			vtkNEW(resultPolydata);
			resultPolydata->DeepCopy(transformResultDataInput->GetOutput());

			int result=m_FirstOperatorVME->SetData(resultPolydata,m_Input->GetTimeStamp());
			if(result == ALBA_ERROR)
			{
				if (!m_TestMode)
					albaMessage(_("The result surface hasn't any points"), _("Warning"), wxICON_EXCLAMATION);
				else
					albaLogMessage("Warning: The result surface hasn't any points");
				albaDEL(resultPolydata);
			}
			else
			{
				if(!m_TestMode)
				{
					m_Gui->Enable(ID_UNDO,true);
					m_Gui->Enable(wxOK,true);
				}
				m_VTKResult.push_back(resultPolydata);
			}
			vtkDEL(transformResultDataInput);
			vtkDEL(clipper);
			vtkDEL(implicitPolyData);
			vtkDEL(subdivider);
			vtkDEL(triangles);
			vtkDEL(transformSecondDataInput);
			vtkDEL(transformFirstDataInput);
		}
		else
		{
			return;
		}
	}
}
//----------------------------------------------------------------------------
void albaOpBooleanSurface::VmeChoose(albaString title,albaEvent *e)
//----------------------------------------------------------------------------
{
	e->SetPointer(&albaOpBooleanSurface::SurfaceAccept);
	e->SetString(&title);
	e->SetId(VME_CHOOSE);
	albaEventMacro(*e);
	m_SecondOperatorVME = albaVMESurface::SafeDownCast(e->GetVme());
	if(m_SecondOperatorVME == NULL && albaVMESurfaceParametric::SafeDownCast(e->GetVme()) != NULL)
  {
    vtkALBASmartPointer<vtkTransformPolyDataFilter> transformSecondDataInput;
    transformSecondDataInput->SetTransform((vtkAbstractTransform *)e->GetVme()->GetAbsMatrixPipe()->GetVTKTransform());
    transformSecondDataInput->SetInput((vtkPolyData *)e->GetVme()->GetOutput()->GetVTKData());
    transformSecondDataInput->Update();

    albaNEW(m_SecondOperatorFromParametric);
    m_SecondOperatorFromParametric->SetData(transformSecondDataInput->GetOutput(), m_Input->GetTimeStamp());
    m_SecondOperatorFromParametric->Update();

    m_SecondOperatorVME = m_SecondOperatorFromParametric;
  }
}
//----------------------------------------------------------------------------
void albaOpBooleanSurface::OpStop(int result)
//----------------------------------------------------------------------------
{
	if(m_ImplicitPlaneGizmo)
	{
		m_ImplicitPlaneGizmo->SetBehavior(NULL);
		GetLogicManager()->VmeRemove(m_ImplicitPlaneGizmo);
	}
	albaDEL(m_ImplicitPlaneGizmo);

  if(result == OP_RUN_CANCEL)
	{
	  GetLogicManager()->VmeRemove(m_ResultVME);
	}

	HideGui();
	albaEventMacro(albaEvent(this,result));        
}
