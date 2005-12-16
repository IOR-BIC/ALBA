/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoSlice.cpp,v $
  Language:  C++
  Date:      $Date: 2005-12-16 18:54:16 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani, Stefano Perticoni
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


#include "mafGizmoSlice.h"
#include "mafDecl.h"
#include "mmaMaterial.h"
#include "mmgMaterialButton.h"

#include "mmiCompositorMouse.h"
#include "mmiGenericMouse.h"

#include "mafSmartPointer.h"
#include "mafVME.h"
#include "mafVMEGizmo.h"
#include "mafTransform.h"

#include "vtkMAFSmartPointer.h"
#include "vtkProperty.h"
#include "vtkPlaneSource.h"
#include "vtkOutlineFilter.h"
#include "vtkRectilinearGrid.h"
#include "vtkStructuredPoints.h"
#include "vtkPoints.h"
#include "vtkCubeSource.h"
#include "vtkAppendPolyData.h"
#include "vtkPolyData.h"
#include "vtkRenderer.h"
#include "vtkDoubleArray.h"
#include "vtkDOFMatrix.h"
#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------
mafGizmoSlice::mafGizmoSlice(mafNode* vme, mafObserver *Listener) 
//----------------------------------------------------------------------------
{
  // register the input vme
  m_vme_input = mafVME::SafeDownCast(vme);

  // default gizmo modality set to SNAP
  MovingModality = SNAP;
  m_axis      = GIZMO_SLICE_Z;
  m_snapArray = NULL;
	m_Listener  = Listener;
  m_id        = 0;

	mafNEW(m_vme_gizmo);
	m_vme_gizmo->SetName("GizmoSlice");
  m_vme_gizmo->ReparentTo(vme);

	mafNEW(m_behavior);
  MouseBH = m_behavior->CreateBehavior(MOUSE_LEFT); 
	MouseBH->SetListener(this);
  MouseBH->SetVME(m_vme_gizmo);
  MouseBH->GetTranslationConstraint()->GetRefSys()->SetTypeToLocal(m_vme_input);
  MouseBH->EnableTranslation(true);
  MouseBH->ResultMatrixConcatenationOn();
	
  vtkNEW(m_point);
	m_point->SetNumberOfPoints(1);
}
//----------------------------------------------------------------------------
mafGizmoSlice::~mafGizmoSlice()
//----------------------------------------------------------------------------
{
  vtkDEL(m_behavior);

	m_vme_gizmo->ReparentTo(NULL);
	mafDEL(m_vme_gizmo);
	vtkDEL(m_snapArray);
	vtkDEL(m_point);
}
//----------------------------------------------------------------------------
void mafGizmoSlice::SetSlice(int id, int axis, double pos)
//----------------------------------------------------------------------------
{
  //register gizmo axis
  m_axis = axis;

  // register id
  m_id = id;
	
	double b[6];
  if (vtkDataSet *vol_data = m_vme_input->GetOutput()->GetVTKData())
  {
    vol_data->Update();
	  vol_data->GetBounds(b);
	  double wx = b[1]-b[0];
	  double wy = b[3]-b[2];
	  double wz = b[5]-b[4];

    // position of the gizmo cube handle centre
    double org[3];
    org[0] = 0;
    org[1] = 0;
    org[2] = 0;

	  vtkMAFSmartPointer<vtkPlaneSource> ps;
	  ps->SetOrigin(org);

    org[0] = b[0];
    org[1] = b[2];
    org[2] = b[4];

    double interval[3][2] ={{b[0], b[1]}, {b[2], b[3]}, {b[4], b[5]}};

	  this->InitSnapArray(m_vme_input,axis);
    MouseBH->GetTranslationConstraint()->SetSnapArray(axis, m_snapArray);
    MouseBH->GetTranslationConstraint()->SetConstraintModality(axis, mmiConstraint::BOUNDS);

	  switch(axis)
	  {
		  case GIZMO_SLICE_X:
      {      
			  org[0] = pos;
			  ps->SetPoint1(0,wy,0);
			  ps->SetPoint2(0,0,wz);
        MouseBH->GetTranslationConstraint()->SetBounds(mmiConstraint::X, interval[0]);
      }
		  break;
		  case GIZMO_SLICE_Y:
      {     
			  org[1] = pos;
			  ps->SetPoint1(wx,0,0);
			  ps->SetPoint2(0,0,wz); 
        MouseBH->GetTranslationConstraint()->SetBounds(mmiConstraint::Y, interval[1]);
      } 
		  break;
		  case GIZMO_SLICE_Z:
		  default:
      {
			  org[2] = pos;
			  ps->SetPoint1(wx,0,0);
			  ps->SetPoint2(0,wy,0);
        MouseBH->GetTranslationConstraint()->SetBounds(mmiConstraint::Z, interval[2]);
      }
		  break;
	  }

    // create the gizmo outline 
	  vtkMAFSmartPointer<vtkOutlineFilter> of;
	  of->SetInput(ps->GetOutput());
	  of->Update();

    // create the gizmo handle
	  vtkMAFSmartPointer<vtkCubeSource> cs;
	  cs->SetXLength(vol_data->GetLength()/50);
	  cs->SetYLength(vol_data->GetLength()/50);
	  cs->SetZLength(vol_data->GetLength()/50);
	  cs->Update();

    // append outline and handle
	  vtkMAFSmartPointer<vtkAppendPolyData> apd;
	  apd->AddInput(cs->GetOutput());
	  apd->AddInput(of->GetOutput());
	  apd->Update();

	  m_vme_gizmo->SetData(apd->GetOutput());

    // position the gizmo 
	  mafSmartPointer<mafTransform> t;
    t->Translate(org, PRE_MULTIPLY);
	  m_vme_gizmo->SetMatrix(t->GetMatrix());   
  
    // GizmoHandleCenterMatrix holds the gizmo handle pose
    mafTransform::SetPosition(GizmoHandleCenterMatrix, org);
    
    // this matrix is keeped updated by the interactor with the gizmo handle position
    MouseBH->SetResultMatrix(&GizmoHandleCenterMatrix);

    //Default moving modality
    this->SetGizmoModalityToSnap();
  }
}
//----------------------------------------------------------------------------
void mafGizmoSlice::SetColor(double col[3])
//----------------------------------------------------------------------------
{
	memcpy(m_vme_gizmo->GetMaterial()->m_Ambient,col,sizeof(col));
  m_vme_gizmo->GetMaterial()->m_AmbientIntensity = 1;
	m_vme_gizmo->GetMaterial()->m_DiffuseIntensity = 0;
	m_vme_gizmo->GetMaterial()->m_SpecularIntensity = 0;
  m_vme_gizmo->GetMaterial()->UpdateProp();
}
//----------------------------------------------------------------------------
void mafGizmoSlice::SetListener(mafObserver *listener)
//----------------------------------------------------------------------------
{
	m_Listener = listener;
}
//----------------------------------------------------------------------------
mafVME *mafGizmoSlice::GetOutput()
//----------------------------------------------------------------------------
{
	return m_vme_gizmo;
}
//----------------------------------------------------------------------------
void mafGizmoSlice::InitSnapArray(mafVME *vol, int axis)
//----------------------------------------------------------------------------
{
  if (vtkDataSet *vol_data = vol->GetOutput()->GetVTKData())
  {
    double b[6], z;
    vol_data->Update();
    vol_data->GetBounds(b);
    if(vol_data->IsA("vtkRectilinearGrid"))
    {
	    this->m_snapArray = vtkDoubleArray::New();
	    if(axis == GIZMO_SLICE_X)
		    this->m_snapArray->DeepCopy((vtkDoubleArray *)((vtkRectilinearGrid *)vol_data)->GetXCoordinates());
	    else if(axis == GIZMO_SLICE_Y)
		    this->m_snapArray->DeepCopy((vtkDoubleArray *)((vtkRectilinearGrid *)vol_data)->GetYCoordinates());
	    else if(axis == GIZMO_SLICE_Z)
		    this->m_snapArray->DeepCopy((vtkDoubleArray *)((vtkRectilinearGrid *)vol_data)->GetZCoordinates());
    }
    if(vol_data->IsA("vtkStructuredPoints"))
    {
	    int dim[3];
	    double spc[3];
	    ((vtkStructuredPoints *)vol_data)->GetDimensions(dim);
	    ((vtkStructuredPoints *)vol_data)->GetSpacing(spc);
	    this->m_snapArray = vtkDoubleArray::New();
	    this->m_snapArray->SetNumberOfValues(dim[axis]);
	    for(int i=0;i<dim[axis];i++)
	    {
		    z = b[2*axis] + i * spc[axis];
		    this->m_snapArray->SetValue(i,z);
	    }
    }
  }
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
        tr->SetMatrix(*m_vme_gizmo->GetOutput()->GetMatrix());
        tr->Concatenate(*e->GetMatrix(), PRE_MULTIPLY);

        m_vme_gizmo->SetMatrix(tr->GetMatrix());		 

        //local position of gizmo cube handle centre
        double locPos[3];
        mafTransform::GetPosition(GizmoHandleCenterMatrix, locPos);

        // position sent as vtk point
        m_point->SetPoint(0,locPos);
        mafEventMacro(mafEvent(this,MOUSE_MOVE, m_point, m_id));
      }
      break;
      default:
        mafEventMacro(*e);
      break; 
    }
	}
}

//----------------------------------------------------------------------------
void mafGizmoSlice::SetGizmoModalityToBound() 
//----------------------------------------------------------------------------
{
  this->MovingModality = BOUND;
 
  switch(m_axis)  
  {
    case GIZMO_SLICE_X:
    {
      MouseBH->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::BOUNDS, mmiConstraint::LOCK, mmiConstraint::LOCK);   
    }    
	  break;
    case GIZMO_SLICE_Y:
    {
      MouseBH->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::BOUNDS, mmiConstraint::LOCK);    
    }
    break;   
    case GIZMO_SLICE_Z:
    {
      MouseBH->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::LOCK, mmiConstraint::BOUNDS);  
    }
	  break;
  }
}  

//----------------------------------------------------------------------------
void mafGizmoSlice::SetGizmoModalityToSnap() 
//----------------------------------------------------------------------------
{
  this->MovingModality = SNAP; 

  switch(m_axis)  
  {
    case GIZMO_SLICE_X:
    {
      MouseBH->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::SNAP_ARRAY, mmiConstraint::LOCK, mmiConstraint::LOCK);   
    }    
	  break;
    case GIZMO_SLICE_Y:
    {
      MouseBH->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::SNAP_ARRAY, mmiConstraint::LOCK);  
    }
    break;   
    case GIZMO_SLICE_Z:
    {
      MouseBH->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::LOCK, mmiConstraint::SNAP_ARRAY);  
    }
	  break;
  }
}
