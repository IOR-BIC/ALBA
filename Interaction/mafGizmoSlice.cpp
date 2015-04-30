/*=========================================================================

 Program: MAF2
 Module: mafGizmoSlice
 Authors: Paolo Quadrani, Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafGizmoSlice.h"
#include "mafDecl.h"
#include "mmaMaterial.h"
#include "mafGUIMaterialButton.h"
#include "mafRefSys.h"

#include "mafInteractorCompositorMouse.h"
#include "mafInteractorGenericMouse.h"

#include "mafTransform.h"
#include "mafSmartPointer.h"
#include "mafTagArray.h"
#include "mafVME.h"
#include "mafVMEGizmo.h"
#include "mafEventInteraction.h"

#include "vtkMAFSmartPointer.h"
#include "vtkProperty.h"
#include "vtkPlaneSource.h"
#include "vtkOutlineFilter.h"
#include "vtkRectilinearGrid.h"
#include "vtkStructuredPoints.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPoints.h"
#include "vtkCubeSource.h"
#include "vtkAppendPolyData.h"
#include "vtkPolyData.h"
#include "vtkRenderer.h"
#include "vtkDoubleArray.h"
#include "vtkMAFDOFMatrix.h"
#include "vtkMatrix4x4.h"


//----------------------------------------------------------------------------
mafGizmoSlice::mafGizmoSlice(mafNode* inputVme, mafObserver *Listener /* = NULL */, const char *name /* =  */, bool inverseHandle /* = false */, double centralClipfactor /* = 0 */)
//----------------------------------------------------------------------------
{
  CreateGizmoSlice(inputVme, Listener, name, inverseHandle, centralClipfactor);
}
//----------------------------------------------------------------------------
void mafGizmoSlice::CreateGizmoSlice(mafNode *imputVme, mafObserver *listener, const char *name, bool inverseHandle, double centralClipfactor)
//----------------------------------------------------------------------------
{
  m_Name = name;
  m_Listener = listener;
  
  // this is the only supported modality for the moment...
  m_Modality = G_LOCAL;

  // register the input vme
  m_InputVME = mafVME::SafeDownCast(imputVme);

  m_InverseHandle = inverseHandle;

  m_CentralClipFactor = centralClipfactor;

  mafNEW(m_GizmoHandleCenterMatrix);

  // default gizmo moving modality set to SNAP
  m_MovingModality = SNAP;
  m_Axis      = GIZMO_SLICE_Z;
  m_SnapArray = NULL;
  m_Id        = 0;

	m_CustomizedSnapArrayFlag = false;
	m_CustomizedArrayStep = 0.1;
  

  mafNEW(m_VmeGizmo);
  m_VmeGizmo->SetName(m_Name);
  m_VmeGizmo->ReparentTo(imputVme);

  mafNEW(m_GizmoBehavior);
  m_MouseBH = m_GizmoBehavior->CreateBehavior(MOUSE_LEFT);
  m_MouseBH->SetListener(this);
  m_MouseBH->SetVME(m_VmeGizmo);
  m_MouseBH->GetTranslationConstraint()->GetRefSys()->SetTypeToLocal(m_InputVME);
  m_MouseBH->EnableTranslation(true);
  m_MouseBH->ResultMatrixConcatenationOn();

  m_VmeGizmo->SetBehavior(m_GizmoBehavior);

  vtkNEW(m_Point);
  m_Point->SetNumberOfPoints(1);
}

//----------------------------------------------------------------------------
void mafGizmoSlice::SetGizmoEnableMoving(bool enable)
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
void mafGizmoSlice::DestroyGizmoSlice()
//----------------------------------------------------------------------------
{
  mafDEL(m_GizmoHandleCenterMatrix);
  m_VmeGizmo->SetBehavior(NULL);
  mafDEL(m_GizmoBehavior);

  m_VmeGizmo->ReparentTo(NULL);
  mafDEL(m_VmeGizmo);
  vtkDEL(m_SnapArray);
  vtkDEL(m_Point);
}
//----------------------------------------------------------------------------
mafGizmoSlice::~mafGizmoSlice()
//----------------------------------------------------------------------------
{
  DestroyGizmoSlice();
}
//----------------------------------------------------------------------------
void mafGizmoSlice::CreateGizmoSliceInLocalPositionOnAxis(int gizmoSliceId, int axis, double localPositionOnAxis, bool visibleCubeHandler)
//----------------------------------------------------------------------------
{
  //register gizmo axis
  m_Axis = axis;

  // register id
  m_Id = gizmoSliceId;
	
	double localBounds[6];
  if (vtkDataSet *VolumeVTKData = m_InputVME->GetOutput()->GetVTKData())
  {
    VolumeVTKData->Update();
	  VolumeVTKData->GetBounds(localBounds);
	  double wx = localBounds[1]-localBounds[0];
	  double wy = localBounds[3]-localBounds[2];
	  double wz = localBounds[5]-localBounds[4];

    // position of the gizmo cube handle centre
    double cubeHandleLocalPosition[3];
    cubeHandleLocalPosition[0] = 0;
    cubeHandleLocalPosition[1] = 0;
    cubeHandleLocalPosition[2] = 0;

	  vtkMAFSmartPointer<vtkPlaneSource> ps;
	  ps->SetOrigin(cubeHandleLocalPosition);

    
    double borderCube = VolumeVTKData->GetLength()/50;

    if(visibleCubeHandler == false)
    {
      borderCube = 0;
    }

    double inversion = 1.;
    if(false == m_InverseHandle)
    {
      cubeHandleLocalPosition[0] = localBounds[0];
      cubeHandleLocalPosition[1] = localBounds[2];
      cubeHandleLocalPosition[2] = localBounds[4];
    }
    else
    {
      cubeHandleLocalPosition[0] = localBounds[1];
      cubeHandleLocalPosition[1] = localBounds[3];
      cubeHandleLocalPosition[2] = localBounds[5];
      inversion = -1.;
    }
    

    double interval[3][2] ={{localBounds[0], localBounds[1]}, {localBounds[2], localBounds[3]}, {localBounds[4], localBounds[5]}};

	  this->InitSnapArray(m_InputVME,axis);
    m_MouseBH->GetTranslationConstraint()->SetSnapArray(axis, m_SnapArray);
    m_MouseBH->GetTranslationConstraint()->SetConstraintModality(axis, mafInteractorConstraint::BOUNDS);

	  switch(axis)
	  {
		  case GIZMO_SLICE_X:
      { 
			  cubeHandleLocalPosition[0] = localPositionOnAxis;
        cubeHandleLocalPosition[1] -= inversion * borderCube /2;
        cubeHandleLocalPosition[2] -= inversion *borderCube /2;
				if(wy<0.00001) wy=-10;
			  ps->SetPoint1(0,inversion * (wy + borderCube /2),0);
			  ps->SetPoint2(0,0,inversion *(wz + borderCube /2));
        m_MouseBH->GetTranslationConstraint()->SetBounds(mafInteractorConstraint::X, interval[0]);
      }
		  break;
		  case GIZMO_SLICE_Y:
      {     
        cubeHandleLocalPosition[0] -= inversion *borderCube /2;
			  cubeHandleLocalPosition[1] = localPositionOnAxis;
        cubeHandleLocalPosition[2] -= inversion *borderCube /2;
			  ps->SetPoint1(inversion *(wx+ borderCube /2),0,0);
			  ps->SetPoint2(0,0,inversion *(wz+ borderCube /2)); 
        m_MouseBH->GetTranslationConstraint()->SetBounds(mafInteractorConstraint::Y, interval[1]);
      } 
		  break;
		  case GIZMO_SLICE_Z:
		  default:
      {
        cubeHandleLocalPosition[0] -= inversion *borderCube /2;
        cubeHandleLocalPosition[1] -= inversion *borderCube /2;
			  cubeHandleLocalPosition[2] = localPositionOnAxis;
			  ps->SetPoint1(inversion *(wx+ borderCube /2),0,0);
				if(wy<0.00001) wy=-10;
			  ps->SetPoint2(0,inversion *(wy+ borderCube /2),0);
        m_MouseBH->GetTranslationConstraint()->SetBounds(mafInteractorConstraint::Z, interval[2]);
      }
		  break;
	  }

    vtkDataSetToPolyDataFilter *cornerFilter;

    if(m_CentralClipFactor == 0)
    {
      // create the gizmo outline 
      cornerFilter = vtkOutlineFilter::New();
      cornerFilter->SetInput(ps->GetOutput());
      cornerFilter->Update();
    }
    else
    {
      // create the gizmo outline 
      cornerFilter = vtkOutlineCornerFilter::New();
      cornerFilter->SetInput(ps->GetOutput());
      ((vtkOutlineCornerFilter*)cornerFilter)->SetCornerFactor(m_CentralClipFactor);
      cornerFilter->Update();
    }
    

    // create the gizmo handle
	  vtkMAFSmartPointer<vtkCubeSource> cs;
	  cs->SetXLength(VolumeVTKData->GetLength()/50);
	  cs->SetYLength(VolumeVTKData->GetLength()/50);
	  cs->SetZLength(VolumeVTKData->GetLength()/50);
	  cs->Update();

    // append outline and handle
	  vtkMAFSmartPointer<vtkAppendPolyData> apd;
    if(visibleCubeHandler == true)
      apd->AddInput(cs->GetOutput());
	  apd->AddInput(cornerFilter->GetOutput());
	  apd->Update();

    cornerFilter->Delete();

    m_VmeGizmo->SetData(apd->GetOutput());

    // position the gizmo 
	  mafSmartPointer<mafTransform> t;
    t->Translate(cubeHandleLocalPosition, PRE_MULTIPLY);
	  m_VmeGizmo->SetMatrix(t->GetMatrix());   
  
    // m_GizmoHandleCenterMatrix holds the gizmo handle pose
    mafTransform::SetPosition(*m_GizmoHandleCenterMatrix, cubeHandleLocalPosition);
    
    // this matrix is keeped updated by the interactor with the gizmo handle position
    m_MouseBH->SetResultMatrix(m_GizmoHandleCenterMatrix);

    //Default moving modality
    this->SetGizmoMovingModalityToBound();
  }
}
//----------------------------------------------------------------------------
void mafGizmoSlice::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_VmeGizmo->GetMaterial()->m_Diffuse[0] = col[0];
  m_VmeGizmo->GetMaterial()->m_Diffuse[1] = col[1];
  m_VmeGizmo->GetMaterial()->m_Diffuse[2] = col[2];
  m_VmeGizmo->GetMaterial()->UpdateProp();
}
//----------------------------------------------------------------------------
mafVME *mafGizmoSlice::GetOutput()
//----------------------------------------------------------------------------
{
	return m_VmeGizmo;
}
//----------------------------------------------------------------------------
void mafGizmoSlice::InitSnapArray(mafVME *vol, int axis)
//----------------------------------------------------------------------------
{
	if(true == m_CustomizedSnapArrayFlag)
	{
    //generate snaparray with step criterion
		this->m_SnapArray = vtkDoubleArray::New();
		if (vtkDataSet *vol_data = vol->GetOutput()->GetVTKData())
		{
			double b[6];
			vol_data->Update();
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

  if (vtkDataSet *vol_data = vol->GetOutput()->GetVTKData())
  {
    double b[6], z;
    vol_data->Update();
    vol_data->GetBounds(b);
    if(vol_data->IsA("vtkRectilinearGrid"))
    {
	    this->m_SnapArray = vtkDoubleArray::New();
	    if(axis == GIZMO_SLICE_X)
		    this->m_SnapArray->DeepCopy((vtkDoubleArray *)((vtkRectilinearGrid *)vol_data)->GetXCoordinates());
	    else if(axis == GIZMO_SLICE_Y)
		    this->m_SnapArray->DeepCopy((vtkDoubleArray *)((vtkRectilinearGrid *)vol_data)->GetYCoordinates());
	    else if(axis == GIZMO_SLICE_Z)
		    this->m_SnapArray->DeepCopy((vtkDoubleArray *)((vtkRectilinearGrid *)vol_data)->GetZCoordinates());
    }
    if(vol_data->IsA("vtkStructuredPoints") || vol_data->IsA("vtkImageData"))
    {
	    int dim[3];
	    double spc[3];
	    ((vtkStructuredPoints *)vol_data)->GetDimensions(dim);
	    ((vtkStructuredPoints *)vol_data)->GetSpacing(spc);
	    this->m_SnapArray = vtkDoubleArray::New();
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
		this->m_SnapArray = vtkDoubleArray::New();
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
void mafGizmoSlice::SetHandleCenterMatrix(mafMatrix *m)
//----------------------------------------------------------------------------
{
  m_GizmoHandleCenterMatrix->DeepCopy(m);
}
//----------------------------------------------------------------------------
void mafGizmoSlice::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
    switch(e->GetId())
    {
      case ID_TRANSFORM:
      {
				mafSmartPointer<mafTransform> tr;
				tr->SetMatrix(*m_VmeGizmo->GetOutput()->GetMatrix());
				tr->Concatenate(*e->GetMatrix(), PRE_MULTIPLY);

				// update the gizmo local position
				m_VmeGizmo->SetMatrix(tr->GetMatrix());		 

				//local position of gizmo cube handle centre
				double slicePlaneOrigin[3];
				mafTransform::GetPosition(*m_GizmoHandleCenterMatrix, slicePlaneOrigin);

				
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
				mafEventMacro(mafEvent(this,MOUSE_MOVE, m_Point, m_Id));


      }
      break;
      default:
				{
					mafEventMacro(*e);
				}
      break; 
    }
	}
	else if(mafEventInteraction *e = mafEventInteraction::SafeDownCast(maf_event))
	{
		if(e->GetId()==mafInteractor::BUTTON_UP)
		{
			mafSmartPointer<mafTransform> tr;
			tr->SetMatrix(*m_VmeGizmo->GetOutput()->GetMatrix());
			tr->Concatenate(*e->GetMatrix(), PRE_MULTIPLY);

			// update the gizmo local position
			m_VmeGizmo->SetMatrix(tr->GetMatrix());		 

			//local position of gizmo cube handle centre
			double slicePlaneOrigin[3];
			mafTransform::GetPosition(*m_GizmoHandleCenterMatrix, slicePlaneOrigin);

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
			mafEventMacro(mafEvent(this,MOUSE_UP, m_Point, m_Id));
		}
	}
}

//----------------------------------------------------------------------------
void mafGizmoSlice::SetGizmoMovingModalityToBound() 
//----------------------------------------------------------------------------
{
  this->m_MovingModality = BOUND;
 
  switch(m_Axis)  
  {
    case GIZMO_SLICE_X:
    {
      m_MouseBH->GetTranslationConstraint()->SetConstraintModality(mafInteractorConstraint::BOUNDS, mafInteractorConstraint::LOCK, mafInteractorConstraint::LOCK);   
    }    
	  break;
    case GIZMO_SLICE_Y:
    {
      m_MouseBH->GetTranslationConstraint()->SetConstraintModality(mafInteractorConstraint::LOCK, mafInteractorConstraint::BOUNDS, mafInteractorConstraint::LOCK);    
    }
    break;   
    case GIZMO_SLICE_Z:
    {
      m_MouseBH->GetTranslationConstraint()->SetConstraintModality(mafInteractorConstraint::LOCK, mafInteractorConstraint::LOCK, mafInteractorConstraint::BOUNDS);  
    }
	  break;
  }
}  

//----------------------------------------------------------------------------
void mafGizmoSlice::SetGizmoMovingModalityToSnap() 
//----------------------------------------------------------------------------
{
  this->m_MovingModality = SNAP; 

  switch(m_Axis)  
  {
    case GIZMO_SLICE_X:
    {
      m_MouseBH->GetTranslationConstraint()->SetConstraintModality(mafInteractorConstraint::SNAP_ARRAY, mafInteractorConstraint::LOCK, mafInteractorConstraint::LOCK);   
    }    
	  break;
    case GIZMO_SLICE_Y:
    {
      m_MouseBH->GetTranslationConstraint()->SetConstraintModality(mafInteractorConstraint::LOCK, mafInteractorConstraint::SNAP_ARRAY, mafInteractorConstraint::LOCK);  
    }
    break;   
    case GIZMO_SLICE_Z:
    {
      m_MouseBH->GetTranslationConstraint()->SetConstraintModality(mafInteractorConstraint::LOCK, mafInteractorConstraint::LOCK, mafInteractorConstraint::SNAP_ARRAY);  
    }
	  break;
  }

  SetOnSnapArray(m_Axis);
}
//----------------------------------------------------------------------------
void mafGizmoSlice::SetOnSnapArray(int axis)
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

  mafMatrix matrix;
  matrix.SetElement(axis, 3, translation);
  mafEvent event;
  event.SetId(ID_TRANSFORM);
  event.SetMatrix(&matrix);
  OnEvent(&event);
}


//----------------------------------------------------------------------------
void mafGizmoSlice::Show(bool show)
//----------------------------------------------------------------------------
{
  assert(m_VmeGizmo);

  // can not use this since it's too slow... this requires destroying and creating
  // the pipeline each time...
  // mafEventMacro(mafEvent(this,VME_SHOW,m_VmeGizmo,show));
  
  // ... instead I am using vtk opacity to speed up the render
  double opacity = show ? 1 : 0;
  m_VmeGizmo->GetMaterial()->m_Prop->SetOpacity(opacity);
}
//----------------------------------------------------------------------------
void mafGizmoSlice::SetInput( mafVME *vme )
//----------------------------------------------------------------------------
{
  if (m_VmeGizmo != NULL)
  {
    DestroyGizmoSlice();
    CreateGizmoSlice(vme, m_Listener, m_Name, m_InverseHandle, m_CentralClipFactor);
  }
}
//----------------------------------------------------------------------------
void mafGizmoSlice::SetPosition( mafMatrix *mat )
//----------------------------------------------------------------------------
{
  // update the gizmo local position
  m_VmeGizmo->SetMatrix(*mat);		 
}