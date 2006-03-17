/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoSlice.cpp,v $
  Language:  C++
  Date:      $Date: 2006-03-17 11:18:27 $
  Version:   $Revision: 1.3 $
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

#include "mafTransform.h"
#include "mafSmartPointer.h"
#include "mafTagArray.h"
#include "mafVME.h"
#include "mafVMEGizmo.h"

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
  m_VmeInput = mafVME::SafeDownCast(vme);

  mafNEW(m_GizmoHandleCenterMatrix);

  // default gizmo modality set to SNAP
  m_MovingModality = SNAP;
  m_axis      = GIZMO_SLICE_Z;
  m_SnapArray = NULL;
	m_Listener  = Listener;
  m_id        = 0;

  mafNEW(m_VmeGizmo);
	m_VmeGizmo->SetName("GizmoSlice");
//  m_VmeGizmo->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
  m_VmeGizmo->ReparentTo(vme);

	mafNEW(m_GizmoBehavior);
  m_MouseBH = m_GizmoBehavior->CreateBehavior(MOUSE_LEFT); 
	m_MouseBH->SetListener(this);
  m_MouseBH->SetVME(m_VmeGizmo);
  m_MouseBH->GetTranslationConstraint()->GetRefSys()->SetTypeToLocal(m_VmeInput);
  m_MouseBH->EnableTranslation(true);
  m_MouseBH->ResultMatrixConcatenationOn();

  m_VmeGizmo->SetBehavior(m_GizmoBehavior);
	
  vtkNEW(m_Point);
	m_Point->SetNumberOfPoints(1);
}
//----------------------------------------------------------------------------
mafGizmoSlice::~mafGizmoSlice()
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
void mafGizmoSlice::SetSlice(int id, int axis, double pos)
//----------------------------------------------------------------------------
{
  //register gizmo axis
  m_axis = axis;

  // register id
  m_id = id;
	
	double b[6];
  if (vtkDataSet *vol_data = m_VmeInput->GetOutput()->GetVTKData())
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

	  this->InitSnapArray(m_VmeInput,axis);
    m_MouseBH->GetTranslationConstraint()->SetSnapArray(axis, m_SnapArray);
    m_MouseBH->GetTranslationConstraint()->SetConstraintModality(axis, mmiConstraint::BOUNDS);

	  switch(axis)
	  {
		  case GIZMO_SLICE_X:
      {      
			  org[0] = pos;
			  ps->SetPoint1(0,wy,0);
			  ps->SetPoint2(0,0,wz);
        m_MouseBH->GetTranslationConstraint()->SetBounds(mmiConstraint::X, interval[0]);
      }
		  break;
		  case GIZMO_SLICE_Y:
      {     
			  org[1] = pos;
			  ps->SetPoint1(wx,0,0);
			  ps->SetPoint2(0,0,wz); 
        m_MouseBH->GetTranslationConstraint()->SetBounds(mmiConstraint::Y, interval[1]);
      } 
		  break;
		  case GIZMO_SLICE_Z:
		  default:
      {
			  org[2] = pos;
			  ps->SetPoint1(wx,0,0);
			  ps->SetPoint2(0,wy,0);
        m_MouseBH->GetTranslationConstraint()->SetBounds(mmiConstraint::Z, interval[2]);
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

    m_VmeGizmo->SetData(apd->GetOutput());

    // position the gizmo 
	  mafSmartPointer<mafTransform> t;
    t->Translate(org, PRE_MULTIPLY);
	  m_VmeGizmo->SetMatrix(t->GetMatrix());   
  
    // m_GizmoHandleCenterMatrix holds the gizmo handle pose
    mafTransform::SetPosition(*m_GizmoHandleCenterMatrix, org);
    
    // this matrix is keeped updated by the interactor with the gizmo handle position
    m_MouseBH->SetResultMatrix(m_GizmoHandleCenterMatrix);

    //Default moving modality
    this->SetGizmoModalityToSnap();
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
void mafGizmoSlice::SetListener(mafObserver *listener)
//----------------------------------------------------------------------------
{
	m_Listener = listener;
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
    if(vol_data->IsA("vtkStructuredPoints"))
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
        tr->SetMatrix(*m_VmeGizmo->GetOutput()->GetMatrix());
        tr->Concatenate(*e->GetMatrix(), PRE_MULTIPLY);

        m_VmeGizmo->SetMatrix(tr->GetMatrix());		 

        //local position of gizmo cube handle centre
        double locPos[3];
        mafTransform::GetPosition(*m_GizmoHandleCenterMatrix, locPos);

        // position sent as vtk point
        m_Point->SetPoint(0,locPos);
        mafEventMacro(mafEvent(this,MOUSE_MOVE, m_Point, m_id));
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
  this->m_MovingModality = BOUND;
 
  switch(m_axis)  
  {
    case GIZMO_SLICE_X:
    {
      m_MouseBH->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::BOUNDS, mmiConstraint::LOCK, mmiConstraint::LOCK);   
    }    
	  break;
    case GIZMO_SLICE_Y:
    {
      m_MouseBH->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::BOUNDS, mmiConstraint::LOCK);    
    }
    break;   
    case GIZMO_SLICE_Z:
    {
      m_MouseBH->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::LOCK, mmiConstraint::BOUNDS);  
    }
	  break;
  }
}  

//----------------------------------------------------------------------------
void mafGizmoSlice::SetGizmoModalityToSnap() 
//----------------------------------------------------------------------------
{
  this->m_MovingModality = SNAP; 

  switch(m_axis)  
  {
    case GIZMO_SLICE_X:
    {
      m_MouseBH->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::SNAP_ARRAY, mmiConstraint::LOCK, mmiConstraint::LOCK);   
    }    
	  break;
    case GIZMO_SLICE_Y:
    {
      m_MouseBH->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::SNAP_ARRAY, mmiConstraint::LOCK);  
    }
    break;   
    case GIZMO_SLICE_Z:
    {
      m_MouseBH->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::LOCK, mmiConstraint::SNAP_ARRAY);  
    }
	  break;
  }
}
