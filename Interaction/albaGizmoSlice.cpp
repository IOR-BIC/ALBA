/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoSlice
 Authors: Paolo Quadrani, Stefano Perticoni
 
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


#include "albaGizmoSlice.h"
#include "albaDecl.h"
#include "mmaMaterial.h"
#include "albaGUIMaterialButton.h"
#include "albaRefSys.h"

#include "albaInteractorCompositorMouse.h"
#include "albaInteractorGenericMouse.h"

#include "albaTransform.h"
#include "albaSmartPointer.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaVMEGizmo.h"
#include "albaEventInteraction.h"

#include "vtkALBASmartPointer.h"
#include "vtkProperty.h"
#include "vtkPlaneSource.h"
#include "vtkOutlineFilter.h"
#include "vtkRectilinearGrid.h"
#include "vtkImageData.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPoints.h"
#include "vtkCubeSource.h"
#include "vtkAppendPolyData.h"
#include "vtkPolyData.h"
#include "vtkRenderer.h"
#include "vtkDoubleArray.h"
#include "vtkALBADOFMatrix.h"
#include "vtkMatrix4x4.h"
#include "albaVMEOutputSurface.h"


//----------------------------------------------------------------------------
albaGizmoSlice::albaGizmoSlice(albaVME* inputVme, albaObserver *Listener /* = NULL */, const char *name /* =  */, bool inverseHandle /* = false */, double centralClipfactor /* = 0 */)
//----------------------------------------------------------------------------
{
	m_Bounds[0] = m_Bounds[2] = m_Bounds[4] = -10;
	m_Bounds[1] = m_Bounds[3] = m_Bounds[5] = 10;
  CreateGizmoSlice(inputVme, Listener, name, inverseHandle, centralClipfactor);
}
//----------------------------------------------------------------------------
void albaGizmoSlice::CreateGizmoSlice(albaVME *imputVme, albaObserver *listener, const char *name, bool inverseHandle, double centralClipfactor)
//----------------------------------------------------------------------------
{
  m_Name = name;
  m_Listener = listener;
  
  // this is the only supported modality for the moment...
  m_Modality = G_LOCAL;

  // register the input vme
  m_InputVME = imputVme;

  m_InverseHandle = inverseHandle;

  m_CentralClipFactor = centralClipfactor;

  albaNEW(m_GizmoHandleCenterMatrix);

  // default gizmo moving modality set to SNAP
  m_MovingModality = SNAP;
  m_Axis      = GIZMO_SLICE_Z;
  m_SnapArray = NULL;
  m_Id        = 0;

	m_CustomizedSnapArrayFlag = false;
	m_CustomizedArrayStep = 0.1;
  

  albaNEW(m_VmeGizmo);
  m_VmeGizmo->SetName(m_Name);
  m_VmeGizmo->ReparentTo(m_InputVME);

  albaNEW(m_GizmoBehavior);
  m_MouseBH = m_GizmoBehavior->CreateBehavior(MOUSE_LEFT);
  m_MouseBH->SetListener(this);
  m_MouseBH->SetVME(m_VmeGizmo);
	if(m_InputVME)
		m_MouseBH->GetTranslationConstraint()->GetRefSys()->SetTypeToLocal(m_InputVME);
	else
		m_MouseBH->GetTranslationConstraint()->GetRefSys()->SetTypeToGlobal();

  m_MouseBH->EnableTranslation(true);
  m_MouseBH->ResultMatrixConcatenationOn();

  m_VmeGizmo->SetBehavior(m_GizmoBehavior);

  vtkNEW(m_Point);
  m_Point->SetNumberOfPoints(1);
}

//----------------------------------------------------------------------------
void albaGizmoSlice::SetGizmoEnableMoving(bool enable)
//----------------------------------------------------------------------------
{
  if(true == enable)
  {
    m_VmeGizmo->SetBehavior(m_GizmoBehavior);
  }
  else
  {
    m_VmeGizmo->SetBehavior(NULL);
  }
}

//----------------------------------------------------------------------------
void albaGizmoSlice::DestroyGizmoSlice()
//----------------------------------------------------------------------------
{
  albaDEL(m_GizmoHandleCenterMatrix);
  m_VmeGizmo->SetBehavior(NULL);
  albaDEL(m_GizmoBehavior);

  m_VmeGizmo->ReparentTo(NULL);
  albaDEL(m_VmeGizmo);
  vtkDEL(m_SnapArray);
  vtkDEL(m_Point);
}
//----------------------------------------------------------------------------
albaGizmoSlice::~albaGizmoSlice()
//----------------------------------------------------------------------------
{
  DestroyGizmoSlice();
}
//----------------------------------------------------------------------------
void albaGizmoSlice::UpdateGizmoSliceInLocalPositionOnAxis(int gizmoSliceId, int axis, double localPositionOnAxis, bool visibleCubeHandler)
//----------------------------------------------------------------------------
{
	//register gizmo axis
	m_Axis = axis;

	// register id
	m_Id = gizmoSliceId;
	vtkDataSet *VolumeVTKData = m_InputVME ? m_InputVME->GetOutput()->GetVTKData() : NULL;

	if (VolumeVTKData)
		VolumeVTKData->GetBounds(m_Bounds);

	double wx = m_Bounds[1] - m_Bounds[0];
	double wy = m_Bounds[3] - m_Bounds[2];
	double wz = m_Bounds[5] - m_Bounds[4];

	// position of the gizmo cube handle centre
	double cubeHandleLocalPosition[3];
	cubeHandleLocalPosition[0] = 0;
	cubeHandleLocalPosition[1] = 0;
	cubeHandleLocalPosition[2] = 0;

	vtkALBASmartPointer<vtkPlaneSource> ps;
	ps->SetOrigin(cubeHandleLocalPosition);

	double borderCube=GetBorderCube();


	if (visibleCubeHandler == false)
	{
		borderCube = 0;
	}

	double inversion = 1.;
	if (false == m_InverseHandle)
	{
		cubeHandleLocalPosition[0] = m_Bounds[0];
		cubeHandleLocalPosition[1] = m_Bounds[2];
		cubeHandleLocalPosition[2] = m_Bounds[4];
	}
	else
	{
		cubeHandleLocalPosition[0] = m_Bounds[1];
		cubeHandleLocalPosition[1] = m_Bounds[3];
		cubeHandleLocalPosition[2] = m_Bounds[5];
		inversion = -1.;
	}


	double interval[3][2] = { {m_Bounds[0], m_Bounds[1]}, {m_Bounds[2], m_Bounds[3]}, {m_Bounds[4], m_Bounds[5]} }; 

	InitSnapArray(m_InputVME, axis);
	m_MouseBH->GetTranslationConstraint()->SetSnapArray(axis, m_SnapArray);
	m_MouseBH->GetTranslationConstraint()->SetConstraintModality(axis, albaInteractorConstraint::BOUNDS);

	switch (axis)
	{
		case GIZMO_SLICE_X:
		{
			cubeHandleLocalPosition[0] = localPositionOnAxis;
			cubeHandleLocalPosition[1] -= inversion * borderCube / 2;
			cubeHandleLocalPosition[2] -= inversion *borderCube / 2;
			if (wy < 0.00001) wy = -10;
			ps->SetPoint1(0, inversion * (wy + borderCube / 2), 0);
			ps->SetPoint2(0, 0, inversion *(wz + borderCube / 2));
			m_MouseBH->GetTranslationConstraint()->SetBounds(albaInteractorConstraint::X, interval[0]);
		}
		break;
		case GIZMO_SLICE_Y:
		{
			cubeHandleLocalPosition[0] -= inversion *borderCube / 2;
			cubeHandleLocalPosition[1] = localPositionOnAxis;
			cubeHandleLocalPosition[2] -= inversion *borderCube / 2;
			ps->SetPoint1(inversion *(wx + borderCube / 2), 0, 0);
			ps->SetPoint2(0, 0, inversion *(wz + borderCube / 2));
			m_MouseBH->GetTranslationConstraint()->SetBounds(albaInteractorConstraint::Y, interval[1]);
		}
		break;
		case GIZMO_SLICE_Z:
		default:
		{
			cubeHandleLocalPosition[0] -= inversion *borderCube / 2;
			cubeHandleLocalPosition[1] -= inversion *borderCube / 2;
			cubeHandleLocalPosition[2] = localPositionOnAxis;
			ps->SetPoint1(inversion *(wx + borderCube / 2), 0, 0);
			if (wy < 0.00001) wy = -10;
			ps->SetPoint2(0, inversion *(wy + borderCube / 2), 0);
			m_MouseBH->GetTranslationConstraint()->SetBounds(albaInteractorConstraint::Z, interval[2]);
		}
		break;
	}

    vtkPolyDataAlgorithm *cornerFilter;

	if (m_CentralClipFactor == 0)
	{
		// create the gizmo outline 
		cornerFilter = vtkOutlineFilter::New();
      cornerFilter->SetInputConnection(ps->GetOutputPort());
		cornerFilter->Update();
	}
	else
	{
		// create the gizmo outline 
		cornerFilter = vtkOutlineCornerFilter::New();
      cornerFilter->SetInputConnection(ps->GetOutputPort());
		((vtkOutlineCornerFilter*)cornerFilter)->SetCornerFactor(m_CentralClipFactor);
		cornerFilter->Update();
	}


	// create the gizmo handle
	vtkALBASmartPointer<vtkCubeSource> cs;
	cs->SetXLength(borderCube);
	cs->SetYLength(borderCube);
	cs->SetZLength(borderCube);
	cs->Update();

	// append outline and handle
	vtkALBASmartPointer<vtkAppendPolyData> apd;
    if(visibleCubeHandler == true)
      apd->AddInputConnection(cs->GetOutputPort());
	  apd->AddInputConnection(cornerFilter->GetOutputPort());
	apd->Update();

	m_VmeGizmo->SetData(apd->GetOutput());
	m_VmeGizmo->Modified();
	m_VmeGizmo->Update();
	m_VmeGizmo->GetSurfaceOutput()->Update();
	m_VmeGizmo->GetSurfaceOutput()->GetVTKData()->Modified();

	cornerFilter->Delete();

	// position the gizmo 
	albaSmartPointer<albaTransform> t;
	t->Translate(cubeHandleLocalPosition, PRE_MULTIPLY);
	m_VmeGizmo->SetMatrix(t->GetMatrix());

	// m_GizmoHandleCenterMatrix holds the gizmo handle pose
	albaTransform::SetPosition(*m_GizmoHandleCenterMatrix, cubeHandleLocalPosition);

	// this matrix is keeped updated by the interactor with the gizmo handle position
	m_MouseBH->SetResultMatrix(m_GizmoHandleCenterMatrix);

	//Default moving modality
	this->SetGizmoMovingModalityToBound();
}

//----------------------------------------------------------------------------
double albaGizmoSlice::GetBorderCube()
{
	double dims[3];

	dims[0] = m_Bounds[3] - m_Bounds[0];
	dims[1] = m_Bounds[4] - m_Bounds[1];
	dims[2] = m_Bounds[5] - m_Bounds[3];

	return sqrt(dims[0] * dims[0] + dims[1] * dims[1] + dims[2] * dims[2]) / 50.0;
}


//----------------------------------------------------------------------------
void albaGizmoSlice::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_VmeGizmo->GetMaterial()->m_Diffuse[0] = col[0];
  m_VmeGizmo->GetMaterial()->m_Diffuse[1] = col[1];
  m_VmeGizmo->GetMaterial()->m_Diffuse[2] = col[2];
  m_VmeGizmo->GetMaterial()->UpdateProp();
}
//----------------------------------------------------------------------------
albaVME *albaGizmoSlice::GetOutput()
//----------------------------------------------------------------------------
{
	return m_VmeGizmo;
}
//----------------------------------------------------------------------------
void albaGizmoSlice::InitSnapArray(albaVME *vol, int axis)
//----------------------------------------------------------------------------
{
	if(m_SnapArray == NULL)
		m_SnapArray = vtkDoubleArray::New();
	m_SnapArray->Reset();

	if (vol == NULL)
		return;

	if(m_CustomizedSnapArrayFlag)
	{
    //generate snaparray with step criterion
		this->m_SnapArray->Reset();
		if (vtkDataSet *vol_data = vol->GetOutput()->GetVTKData())
		{
			double b[6];
			vol_data->GetBounds(b);
			
			if(axis == GIZMO_SLICE_X)
			{
				this->m_SnapArray->InsertNextTuple1(b[0]);
				double currentBound = b[0];
				while(currentBound < b[1])
				{
          currentBound += m_CustomizedArrayStep;
          this->m_SnapArray->InsertNextTuple1(currentBound);
				}
			}
			else if(axis == GIZMO_SLICE_Y)
			{
				this->m_SnapArray->InsertNextTuple1(b[2]);
				double currentBound = b[2];
				while(currentBound < b[3])
				{
					currentBound += m_CustomizedArrayStep;
					this->m_SnapArray->InsertNextTuple1(currentBound);
				}
			}
			else if(axis == GIZMO_SLICE_Z)
			{
				this->m_SnapArray->InsertNextTuple1(b[4]);
				double currentBound = b[4];
				while(currentBound < b[5])
				{
					currentBound += m_CustomizedArrayStep;
					this->m_SnapArray->InsertNextTuple1(currentBound);
				}
			}
			
		}
		return;
	}

	vtkDataSet *vol_data = vol ? vol->GetOutput()->GetVTKData(): NULL;
  if (vol_data)
  {
    double b[6], z;
    vol_data->GetBounds(b);
    if(vol_data->IsA("vtkRectilinearGrid"))
    {
			this->m_SnapArray->Reset();
			if (axis == GIZMO_SLICE_X)
		    this->m_SnapArray->DeepCopy((vtkDoubleArray *)((vtkRectilinearGrid *)vol_data)->GetXCoordinates());
	    else if(axis == GIZMO_SLICE_Y)
		    this->m_SnapArray->DeepCopy((vtkDoubleArray *)((vtkRectilinearGrid *)vol_data)->GetYCoordinates());
	    else if(axis == GIZMO_SLICE_Z)
		    this->m_SnapArray->DeepCopy((vtkDoubleArray *)((vtkRectilinearGrid *)vol_data)->GetZCoordinates());
    }
    if(vol_data->IsA("vtkImageData") || vol_data->IsA("vtkImageData"))
    {
	    int dim[3];
	    double spc[3];
	    ((vtkImageData *)vol_data)->GetDimensions(dim);
	    ((vtkImageData *)vol_data)->GetSpacing(spc);
			this->m_SnapArray->Reset();
			this->m_SnapArray->SetNumberOfValues(dim[axis]);
	    for(int i=0;i<dim[axis];i++)
	    {
		    z = b[2*axis] + i * spc[axis];
		    this->m_SnapArray->SetValue(i,z);
	    }
    }
	if(vol_data->IsA("vtkPolyData"))
	{
		double b[6];
		vol_data->GetBounds(b);
		this->m_SnapArray->Reset();
		this->m_SnapArray->SetNumberOfValues(2);
		if(axis == GIZMO_SLICE_X)
		{
			this->m_SnapArray->SetValue(0,b[0]);
			this->m_SnapArray->SetValue(1,b[1]);
		}
		else if(axis == GIZMO_SLICE_Y)
		{
			this->m_SnapArray->SetValue(0,b[2]);
			this->m_SnapArray->SetValue(1,b[3]);
		}
		else if(axis == GIZMO_SLICE_Z)
		{
			this->m_SnapArray->SetValue(0,b[4]);
			this->m_SnapArray->SetValue(1,b[5]);
		}
	}
  }
}
//----------------------------------------------------------------------------
void albaGizmoSlice::SetHandleCenterMatrix(albaMatrix *m)
//----------------------------------------------------------------------------
{
  m_GizmoHandleCenterMatrix->DeepCopy(m);
}

//----------------------------------------------------------------------------
void albaGizmoSlice::SetBounds(double *val)
{
	for (int i = 0; i < 6; i++) 
		m_Bounds[i] = val[i];
}

//----------------------------------------------------------------------------
void albaGizmoSlice::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
    switch(e->GetId())
    {
      case ID_TRANSFORM:
      {
				albaSmartPointer<albaTransform> tr;
				tr->SetMatrix(*m_VmeGizmo->GetOutput()->GetMatrix());
				tr->Concatenate(*e->GetMatrix(), PRE_MULTIPLY);

				// update the gizmo local position
				m_VmeGizmo->SetMatrix(tr->GetMatrix());		 

				//local position of gizmo cube handle centre
				double slicePlaneOrigin[3];
				GetPosition(slicePlaneOrigin);


				
				// position sent as vtk point
        if(m_Axis == GIZMO_SLICE_X)
        {
          slicePlaneOrigin[1] = 0.;
          slicePlaneOrigin[2] = 0.;
        }
        else if(m_Axis == GIZMO_SLICE_Y)
        {
          slicePlaneOrigin[0] = 0.;
          slicePlaneOrigin[2] = 0.;
        }
        else if(m_Axis == GIZMO_SLICE_Z)
        {
          slicePlaneOrigin[0] = 0.;
          slicePlaneOrigin[1] = 0.;
        }

				
				m_Point->SetPoint(0,slicePlaneOrigin);
				albaEventMacro(albaEvent(this,MOUSE_MOVE, m_Point, m_Id));


      }
      break;
      default:
				{
					albaEventMacro(*e);
				}
      break; 
    }
	}
	else if(albaEventInteraction *e = albaEventInteraction::SafeDownCast(alba_event))
	{
		if(e->GetId()==albaInteractor::BUTTON_UP)
		{
			albaSmartPointer<albaTransform> tr;
			tr->SetMatrix(*m_VmeGizmo->GetOutput()->GetMatrix());
			tr->Concatenate(*e->GetMatrix(), PRE_MULTIPLY);

			// update the gizmo local position
			m_VmeGizmo->SetMatrix(tr->GetMatrix());		 

			//local position of gizmo cube handle centre
			double slicePlaneOrigin[3];
			albaTransform::GetPosition(*m_GizmoHandleCenterMatrix, slicePlaneOrigin);

			// position sent as vtk point
      if(m_Axis == GIZMO_SLICE_X)
		  {
			  slicePlaneOrigin[1] = 0.;
        slicePlaneOrigin[2] = 0.;
		  }
		  else if(m_Axis == GIZMO_SLICE_Y)
		  {
        slicePlaneOrigin[0] = 0.;
        slicePlaneOrigin[2] = 0.;
		  }
		  else if(m_Axis == GIZMO_SLICE_Z)
		  {
        slicePlaneOrigin[0] = 0.;
        slicePlaneOrigin[1] = 0.;
		  }

			m_Point->SetPoint(0,slicePlaneOrigin);
			albaEventMacro(albaEvent(this,MOUSE_UP, m_Point, m_Id));
		}
	}
}

//----------------------------------------------------------------------------
void albaGizmoSlice::SetGizmoMovingModalityToBound() 
//----------------------------------------------------------------------------
{
  this->m_MovingModality = BOUND;
 
  switch(m_Axis)  
  {
    case GIZMO_SLICE_X:
    {
      m_MouseBH->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::BOUNDS, albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK);   
    }    
	  break;
    case GIZMO_SLICE_Y:
    {
      m_MouseBH->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::BOUNDS, albaInteractorConstraint::LOCK);    
    }
    break;   
    case GIZMO_SLICE_Z:
    {
      m_MouseBH->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK, albaInteractorConstraint::BOUNDS);  
    }
	  break;
  }
}  

//----------------------------------------------------------------------------
void albaGizmoSlice::SetGizmoMovingModalityToSnap() 
//----------------------------------------------------------------------------
{
  this->m_MovingModality = SNAP; 

  switch(m_Axis)  
  {
    case GIZMO_SLICE_X:
    {
      m_MouseBH->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::SNAP_ARRAY, albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK);   
    }    
	  break;
    case GIZMO_SLICE_Y:
    {
      m_MouseBH->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::SNAP_ARRAY, albaInteractorConstraint::LOCK);  
    }
    break;   
    case GIZMO_SLICE_Z:
    {
      m_MouseBH->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK, albaInteractorConstraint::SNAP_ARRAY);  
    }
	  break;
  }

  SetOnSnapArray(m_Axis);
}
//----------------------------------------------------------------------------
void albaGizmoSlice::SetOnSnapArray(int axis)
//----------------------------------------------------------------------------
{
  vtkDoubleArray *snapArray = m_MouseBH->GetTranslationConstraint()->GetSnapArray(axis);
  if(NULL == snapArray) return;

  double translation = 0.;
  double pos[3], rot[3];
  this->m_VmeGizmo->GetOutput()->GetPose(pos,rot, m_VmeGizmo->GetTimeStamp());
  double minimumDistance = VTK_DOUBLE_MAX;
  double slice;
  double effectiveTranslation;
  double currentDifference;
  for(int i=0; i<snapArray->GetSize(); i++)
  {
    slice = snapArray->GetTuple1(i);
    effectiveTranslation = slice - pos[axis];
    currentDifference = fabs(effectiveTranslation);
    if(currentDifference < minimumDistance)
    {
      minimumDistance = currentDifference;
      translation = effectiveTranslation;
    }
    
  }
  m_GizmoHandleCenterMatrix->SetElement(axis, 3, m_GizmoHandleCenterMatrix->GetElement(axis, 3) + translation);

  albaMatrix matrix;
  matrix.SetElement(axis, 3, translation);
  albaEvent event;
  event.SetId(ID_TRANSFORM);
  event.SetMatrix(&matrix);
  OnEvent(&event);
}


//----------------------------------------------------------------------------
void albaGizmoSlice::Show(bool show)
//----------------------------------------------------------------------------
{
  assert(m_VmeGizmo);

  // can not use this since it's too slow... this requires destroying and creating
  // the pipeline each time...
  
  // ... instead I am using vtk opacity to speed up the render
  double opacity = show ? 1 : 0;
  m_VmeGizmo->GetMaterial()->m_Prop->SetOpacity(opacity);
}
//----------------------------------------------------------------------------
void albaGizmoSlice::SetInput( albaVME *vme )
//----------------------------------------------------------------------------
{
	m_InputVME = vme;

	m_VmeGizmo->ReparentTo(m_InputVME);
	if (m_InputVME)
		m_MouseBH->GetTranslationConstraint()->GetRefSys()->SetTypeToLocal(m_InputVME);
	else
		m_MouseBH->GetTranslationConstraint()->GetRefSys()->SetTypeToGlobal();
}
//----------------------------------------------------------------------------
void albaGizmoSlice::SetPosition( albaMatrix *mat )
//----------------------------------------------------------------------------
{
  // update the gizmo local position
  m_VmeGizmo->SetMatrix(*mat);		 
}

//----------------------------------------------------------------------------
void albaGizmoSlice::GetPosition(double * slicePlaneOrigin)
{
	albaTransform::GetPosition(*m_GizmoHandleCenterMatrix, slicePlaneOrigin);
}
