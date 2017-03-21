/*=========================================================================

 Program: MAF2
 Module: mafVMESurfaceParametric
 Authors: Roberto Mucci , Stefano Perticoni
 
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

#include "mafVMESurfaceParametric.h"
#include "mmuIdFactory.h"
#include "mafDataVector.h"
#include "mafMatrixInterpolator.h"
#include "mafDataPipeInterpolator.h"
#include "mafTagArray.h"
#include "mafMatrixVector.h"
#include "mafVMEItemVTK.h"
#include "mafTransform.h"
#include "mafGUI.h"
#include "mmaMaterial.h"
#include "mafVMEOutputSurface.h"
#include "mafDataPipeCustom.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkBitArray.h"
#include "vtkPointData.h"
#include "vtkConeSource.h"
#include "vtkCylinderSource.h"
#include "vtkPlaneSource.h"
#include "vtkCubeSource.h"
#include "vtkSphereSource.h"
#include "vtkTriangleFilter.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

#include <vector>

const bool DEBUG_MODE = true;

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
//MAF_ID_IMP(mafVMESurfaceParametric::CHANGE_PARAMETER);   // Event rised by change parameter 

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafVMESurfaceParametric);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafVMESurfaceParametric::mafVMESurfaceParametric()
//-------------------------------------------------------------------------
{
  m_GeometryType = PARAMETRIC_SPHERE;
	
  m_SphereRadius = 2.0;
  m_SpherePhiRes = 10.0;
  m_SphereTheRes = 10.0;
  m_ConeHeight = 5.0;
  m_ConeRadius = 2.0;
  m_ConeCapping = 0;
  m_ConeRes = 20.0;
  m_ConeOrientationAxis = ID_X_AXIS;
  m_CylinderHeight = 5.0;
  m_CylinderRadius = 2.0;
  m_CylinderRes = 20.0;
  m_CylinderOrientationAxis = ID_Y_AXIS;
  m_CubeXLength = 2.0;
  m_CubeYLength = 2.0;
  m_CubeZLength = 2.0;
  m_PlaneXRes = 2.0;
  m_PlaneYRes = 2.0;
  m_PlaneOrigin[0] = m_PlaneOrigin[1] = m_PlaneOrigin[2] = 0;
  m_PlanePoint1[0] = 2.0;
  m_PlanePoint1[1] = m_PlanePoint1[2] = 0.0;
  m_PlanePoint2[1] = 3.0;
  m_PlanePoint2[0] = m_PlanePoint2[2] = 0.0;
  m_EllipsoidXLenght = 1.0;
  m_EllipsoidYLenght = 2.0;
  m_EllipsoidZLenght = 3.0;
  m_EllipsoidPhiRes = 10.0;
  m_EllipsoidTheRes = 10.0;
  m_EllipsoidOrientationAxis = ID_X_AXIS;

	mafNEW(m_Transform);
	mafVMEOutputSurface *output=mafVMEOutputSurface::New(); // an output with no data
	output->SetTransform(m_Transform); // force my transform in the output
	SetOutput(output);

	GetMaterial();

	vtkNEW(m_PolyData);

  // attach a data pipe which creates a bridge between VTK and MAF
	mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
	dpipe->SetInput(m_PolyData);
	SetDataPipe(dpipe);
}

//-------------------------------------------------------------------------
mafVMESurfaceParametric::~mafVMESurfaceParametric()
//-------------------------------------------------------------------------
{
	vtkDEL(m_PolyData);
  mafDEL(m_Transform);
	SetOutput(NULL);
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMESurfaceParametric::GetMaterial()
//-------------------------------------------------------------------------
{
	mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
	if (material == NULL)
	{
		material = mmaMaterial::New();
		SetAttribute("MaterialAttributes", material);
	}
	return material;
}

//-------------------------------------------------------------------------
int mafVMESurfaceParametric::DeepCopy(mafVME *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMESurfaceParametric *vmeParametricSurface=mafVMESurfaceParametric::SafeDownCast(a);
    m_Transform->SetMatrix(vmeParametricSurface->m_Transform->GetMatrix());
    this->m_GeometryType = vmeParametricSurface->m_GeometryType;
    this->m_SphereRadius = vmeParametricSurface->m_SphereRadius;
    this->m_SpherePhiRes = vmeParametricSurface->m_SpherePhiRes;
    this->m_SphereTheRes = vmeParametricSurface->m_SphereTheRes;
    this->m_ConeHeight = vmeParametricSurface->m_ConeHeight;
    this->m_ConeRadius = vmeParametricSurface->m_ConeRadius;
    this->m_ConeCapping = vmeParametricSurface->m_ConeCapping;
    this->m_ConeRes = vmeParametricSurface->m_ConeRes;
    this->m_ConeOrientationAxis = vmeParametricSurface->m_ConeOrientationAxis;
    this->m_CylinderHeight = vmeParametricSurface->m_CylinderHeight;
    this->m_CylinderRadius = vmeParametricSurface->m_CylinderRadius;
    this->m_CylinderRes = vmeParametricSurface->m_CylinderRes;
    this->m_CylinderOrientationAxis = vmeParametricSurface->m_CylinderOrientationAxis;
    this->m_CubeXLength = vmeParametricSurface->m_CubeXLength;
    this->m_CubeYLength = vmeParametricSurface->m_CubeYLength;
    this->m_CubeZLength = vmeParametricSurface->m_CubeZLength;
    this->m_PlaneXRes = vmeParametricSurface->m_PlaneXRes;
    this->m_PlaneYRes = vmeParametricSurface->m_PlaneYRes;

    this->m_PlaneOrigin[0] = vmeParametricSurface->m_PlaneOrigin[0];
    this->m_PlaneOrigin[1] = vmeParametricSurface->m_PlaneOrigin[1];
    this->m_PlaneOrigin[2] = vmeParametricSurface->m_PlaneOrigin[2];

    this->m_PlanePoint1[0] = vmeParametricSurface->m_PlanePoint1[0];
    this->m_PlanePoint1[1] = vmeParametricSurface->m_PlanePoint1[1];
    this->m_PlanePoint1[2] = vmeParametricSurface->m_PlanePoint1[2];

    this->m_PlanePoint2[0] = vmeParametricSurface->m_PlanePoint2[0];
    this->m_PlanePoint2[1] = vmeParametricSurface->m_PlanePoint2[1];
    this->m_PlanePoint2[2] = vmeParametricSurface->m_PlanePoint2[2];

    this->m_EllipsoidXLenght = vmeParametricSurface->m_EllipsoidXLenght;
    this->m_EllipsoidYLenght = vmeParametricSurface->m_EllipsoidYLenght;
    this->m_EllipsoidZLenght = vmeParametricSurface->m_EllipsoidZLenght;
    this->m_EllipsoidPhiRes = vmeParametricSurface->m_EllipsoidPhiRes;
    this->m_EllipsoidTheRes = vmeParametricSurface->m_EllipsoidTheRes;
    this->m_EllipsoidOrientationAxis = vmeParametricSurface->m_EllipsoidOrientationAxis;

    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetInput(m_PolyData);
    }
    InternalUpdate();
    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMESurfaceParametric::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    if (     
      m_Transform->GetMatrix()==((mafVMESurfaceParametric *)vme)->m_Transform->GetMatrix() &&
      this->m_GeometryType == ((mafVMESurfaceParametric *)vme)->m_GeometryType &&
      this->m_SphereRadius == ((mafVMESurfaceParametric *)vme)->m_SphereRadius &&
      this->m_SpherePhiRes == ((mafVMESurfaceParametric *)vme)->m_SpherePhiRes &&
      this->m_SphereTheRes == ((mafVMESurfaceParametric *)vme)->m_SphereTheRes &&
      this->m_ConeHeight == ((mafVMESurfaceParametric *)vme)->m_ConeHeight  &&
      this->m_ConeRadius == ((mafVMESurfaceParametric *)vme)->m_ConeRadius  &&
      this->m_ConeCapping == ((mafVMESurfaceParametric *)vme)->m_ConeCapping  &&
      this->m_ConeRes == ((mafVMESurfaceParametric *)vme)->m_ConeRes &&
      this->m_ConeOrientationAxis == ((mafVMESurfaceParametric *)vme)->m_ConeOrientationAxis &&
      this->m_CylinderHeight == ((mafVMESurfaceParametric *)vme)->m_CylinderHeight  &&
      this->m_CylinderRadius == ((mafVMESurfaceParametric *)vme)->m_CylinderRadius  &&
      this->m_CylinderRes == ((mafVMESurfaceParametric *)vme)->m_CylinderRes  &&
      this->m_CylinderOrientationAxis == ((mafVMESurfaceParametric *)vme)->m_CylinderOrientationAxis  &&
      this->m_CubeXLength == ((mafVMESurfaceParametric *)vme)->m_CubeXLength  &&
      this->m_CubeYLength == ((mafVMESurfaceParametric *)vme)->m_CubeYLength  &&
      this->m_CubeZLength == ((mafVMESurfaceParametric *)vme)->m_CubeZLength  &&
      this->m_PlaneXRes == ((mafVMESurfaceParametric *)vme)->m_PlaneXRes  &&
      this->m_PlaneYRes == ((mafVMESurfaceParametric *)vme)->m_PlaneYRes  &&

      this->m_PlaneOrigin[0] == ((mafVMESurfaceParametric *)vme)->m_PlaneOrigin[0]  &&
      this->m_PlaneOrigin[1] == ((mafVMESurfaceParametric *)vme)->m_PlaneOrigin[1]  &&
      this->m_PlaneOrigin[2] == ((mafVMESurfaceParametric *)vme)->m_PlaneOrigin[2]  &&

      this->m_PlanePoint1[0] == ((mafVMESurfaceParametric *)vme)->m_PlanePoint1[0]  &&
      this->m_PlanePoint1[1] == ((mafVMESurfaceParametric *)vme)->m_PlanePoint1[1]  &&
      this->m_PlanePoint1[2] == ((mafVMESurfaceParametric *)vme)->m_PlanePoint1[2]  &&

      this->m_PlanePoint2[0] == ((mafVMESurfaceParametric *)vme)->m_PlanePoint2[0]  &&
      this->m_PlanePoint2[1] == ((mafVMESurfaceParametric *)vme)->m_PlanePoint2[1]  &&
      this->m_PlanePoint2[2] == ((mafVMESurfaceParametric *)vme)->m_PlanePoint2[2] &&

      this->m_EllipsoidXLenght == ((mafVMESurfaceParametric *)vme)->m_EllipsoidXLenght &&
      this->m_EllipsoidYLenght == ((mafVMESurfaceParametric *)vme)->m_EllipsoidYLenght &&
      this->m_EllipsoidZLenght == ((mafVMESurfaceParametric *)vme)->m_EllipsoidZLenght &&
      this->m_EllipsoidPhiRes == ((mafVMESurfaceParametric *)vme)->m_EllipsoidPhiRes &&
      this->m_EllipsoidTheRes == ((mafVMESurfaceParametric *)vme)->m_EllipsoidTheRes &&
      this->m_EllipsoidOrientationAxis == ((mafVMESurfaceParametric *)vme)->m_EllipsoidOrientationAxis
      )
    {
      ret = true;
    }
  }
 return ret;
}
//-------------------------------------------------------------------------
mafVMEOutputSurface *mafVMESurfaceParametric::GetSurfaceOutput()
//-------------------------------------------------------------------------
{
	return (mafVMEOutputSurface *)GetOutput();
}
//-------------------------------------------------------------------------
void mafVMESurfaceParametric::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
	m_Transform->SetMatrix(mat);
	Modified();
}
//-------------------------------------------------------------------------
bool mafVMESurfaceParametric::IsAnimated()
//-------------------------------------------------------------------------
{
	return false;
}
//-------------------------------------------------------------------------
void mafVMESurfaceParametric::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
	kframes.clear(); // no timestamps
  mafTimeStamp t = m_Transform->GetMatrix().GetTimeStamp();
  kframes.push_back(t);
}
//-------------------------------------------------------------------------
mafGUI* mafVMESurfaceParametric::CreateGui()
//-------------------------------------------------------------------------
{
  mafVME::CreateGui();
  if(m_Gui)
  {
    wxString geometryType[6] = {"Sphere", "Cone", "Cylinder", "Cube", "Plane", "Ellipsoid"};
    m_Gui->Combo(ID_GEOMETRY_TYPE, "", &m_GeometryType, 6, geometryType);
    m_Gui->Divider(2);

    CreateGuiSphere();   
    m_Gui->Divider(2);
    CreateGuiCone();
    m_Gui->Divider(2);
    CreateGuiCylinder();
    m_Gui->Divider(2);
    CreateGuiCube();
    m_Gui->Divider(2);
    CreateGuiPlane();
    m_Gui->Divider(2);
    CreateGuiEllipsoid();
   
    m_Gui->FitGui();
    
    EnableParametricSurfaceGui(m_GeometryType);
    m_Gui->Update();
  }

  m_Gui->Divider();
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMESurfaceParametric::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_GEOMETRY_TYPE:
      {  
        EnableParametricSurfaceGui(m_GeometryType);
        m_Gui->Update();
        InternalUpdate();
        m_Gui->FitGui();
      }

      case CHANGE_VALUE_SPHERE:
      case CHANGE_VALUE_CUBE:
      case CHANGE_VALUE_CONE:
      case CHANGE_VALUE_CYLINDER:
      case CHANGE_VALUE_PLANE:
      case CHANGE_VALUE_ELLIPSOID:
      {
        InternalUpdate();
        e->SetId(CAMERA_UPDATE);
        ForwardUpEvent(e);
      }
      break;
      
      default:
        mafVME::OnEvent(maf_event);
    }
  }
  
  else
  {
    Superclass::OnEvent(maf_event);
  }
}

//-----------------------------------------------------------------------
void mafVMESurfaceParametric::InternalPreUpdate()
//-----------------------------------------------------------------------
{
	InternalInitialize();
}
//-----------------------------------------------------------------------
void mafVMESurfaceParametric::InternalUpdate()
//-----------------------------------------------------------------------
{
	switch(m_GeometryType)
	{
	case PARAMETRIC_SPHERE:
		{
      vtkMAFSmartPointer<vtkSphereSource> surf;
			surf->SetRadius(m_SphereRadius);
      surf->SetPhiResolution(m_SpherePhiRes);
      surf->SetThetaResolution(m_SphereTheRes);
			surf->Update();
			m_PolyData->DeepCopy(surf->GetOutput());
			m_PolyData->Update();
		}
	break;
	case PARAMETRIC_CONE:
	{
    vtkMAFSmartPointer<vtkConeSource> surf;
    surf->SetHeight(m_ConeHeight);
    surf->SetRadius(m_ConeRadius);
    surf->SetCapping(m_ConeCapping);
    surf->SetResolution(m_ConeRes);
    surf->Update();

    vtkMAFSmartPointer<vtkTransform> t;

    switch(m_ConeOrientationAxis)
    {
    case ID_X_AXIS:
      //do nothing
      break;
    case ID_Y_AXIS:
      t->RotateZ(90);
      break;
    case ID_Z_AXIS:
      t->RotateY(-90);
      break;
    default:
      break;
    }
   
    t->Update();

    vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
    ptf->SetTransform(t);
    ptf->SetInput(surf->GetOutput());
    ptf->Update();

    m_PolyData->DeepCopy(ptf->GetOutput());
    m_PolyData->Update();
	}
	break;
	case PARAMETRIC_CYLINDER:
	{
    vtkMAFSmartPointer<vtkCylinderSource> surf;
    surf->SetHeight(m_CylinderHeight);
    surf->SetRadius(m_CylinderRadius);
    surf->SetResolution(m_CylinderRes);
    surf->Update();

    vtkMAFSmartPointer<vtkTransform> t;

    switch(m_CylinderOrientationAxis)
    {
    case ID_X_AXIS:
      t->RotateZ(90);
      break;
    case ID_Y_AXIS:
      //do nothing
      break;
    case ID_Z_AXIS:
      t->RotateX(-90);
      break;
    default:
      break;
    }

    t->Update();

    vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
    ptf->SetTransform(t);
    ptf->SetInput(surf->GetOutput());
    ptf->Update();


    m_PolyData->DeepCopy(ptf->GetOutput());
    m_PolyData->Update();

	}
	break;
	case PARAMETRIC_CUBE:
	{
    vtkMAFSmartPointer<vtkCubeSource> surf;
		surf->SetXLength(m_CubeXLength);
		surf->SetYLength(m_CubeYLength);
		surf->SetZLength(m_CubeZLength);
		surf->Update();
		m_PolyData->DeepCopy(surf->GetOutput());
		m_PolyData->Update();
	}
	break;
  
  case PARAMETRIC_PLANE:
    {
    vtkMAFSmartPointer<vtkPlaneSource> surf;
    surf->SetXResolution(m_PlaneXRes);
    surf->SetYResolution(m_PlaneYRes);
    surf->SetOrigin(m_PlaneOrigin);
    surf->SetPoint1(m_PlanePoint1);
    surf->SetPoint2(m_PlanePoint2);
    surf->Update();
    vtkMAFSmartPointer<vtkTriangleFilter> triangle;
    triangle->SetInput(surf->GetOutput());
    triangle->Update();
    m_PolyData->DeepCopy(triangle->GetOutput());
    m_PolyData->Update();
    }
    break;

  case PARAMETRIC_ELLIPSOID:
    {
      vtkMAFSmartPointer<vtkSphereSource> surf;
      surf->SetRadius(m_EllipsoidYLenght);
      surf->SetPhiResolution(m_EllipsoidPhiRes);
      surf->SetThetaResolution(m_EllipsoidTheRes);
      surf->Update();

      vtkMAFSmartPointer<vtkTransform> t;

      switch(m_EllipsoidOrientationAxis)
      {
      case ID_X_AXIS:
        //do nothing
        break;
      case ID_Y_AXIS:
        t->RotateZ(90);
        break;
      case ID_Z_AXIS:
        t->RotateY(-90);
        break;
      default:
        break;
      }

      t->Scale(m_EllipsoidXLenght/m_EllipsoidYLenght,1,m_EllipsoidZLenght/m_EllipsoidYLenght);
      t->Update();

      vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
      ptf->SetTransform(t);
      ptf->SetInput(surf->GetOutput());
      ptf->Update();

      m_PolyData->DeepCopy(ptf->GetOutput());
      m_PolyData->Update();
    }
    break;
	}
}
//-----------------------------------------------------------------------
int mafVMESurfaceParametric::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
	if (Superclass::InternalStore(parent)==MAF_OK)
	{
    if (
		parent->StoreMatrix("Transform",&m_Transform->GetMatrix()) == MAF_OK && 
		parent->StoreInteger("Geometry", 	m_GeometryType) == MAF_OK &&
    parent->StoreDouble("ShereRadius", m_SphereRadius) == MAF_OK &&
    parent->StoreDouble("SpherePhiRes",m_SpherePhiRes) == MAF_OK &&
    parent->StoreDouble("SphereThetaRes",m_SphereTheRes) == MAF_OK &&
    parent->StoreDouble("ConeHieght",m_ConeHeight) == MAF_OK &&
    parent->StoreDouble("ConeRadius",m_ConeRadius) == MAF_OK &&
    parent->StoreInteger("ConeCapping",m_ConeCapping) == MAF_OK &&
    parent->StoreDouble("ConeRes",m_ConeRes) == MAF_OK &&
    parent->StoreInteger("ConeOrientationAxis",m_ConeOrientationAxis) == MAF_OK &&
    parent->StoreDouble("CylinderHeight",m_CylinderHeight) == MAF_OK &&
    parent->StoreDouble("CylinderRadius",m_CylinderRadius) == MAF_OK &&
    parent->StoreDouble("CylinderRes",m_CylinderRes) == MAF_OK &&
    parent->StoreInteger("CylinderOrientationAxis",m_CylinderOrientationAxis) == MAF_OK &&
    parent->StoreDouble("CubeXLength",m_CubeXLength) == MAF_OK &&
    parent->StoreDouble("CubeYLength",m_CubeYLength) == MAF_OK &&
    parent->StoreDouble("CubeZLength",m_CubeZLength) == MAF_OK &&
    parent->StoreDouble("PlaneXRes",m_PlaneXRes) == MAF_OK &&
    parent->StoreDouble("PlaneYRes",m_PlaneYRes) == MAF_OK &&
    parent->StoreVectorN("PlaneOrigin",m_PlaneOrigin,3) == MAF_OK &&
    parent->StoreVectorN("PlanePoint1",m_PlanePoint1,3) == MAF_OK &&
    parent->StoreVectorN("PlanePoint2",m_PlanePoint2,3) == MAF_OK &&
    parent->StoreDouble("EllipsoidXLenght",m_EllipsoidXLenght) == MAF_OK &&
    parent->StoreDouble("EllipsoidYLenght",m_EllipsoidYLenght) == MAF_OK &&
    parent->StoreDouble("EllipsoidZLenght",m_EllipsoidZLenght) == MAF_OK &&
    parent->StoreDouble("EllipsoidTheRes",m_EllipsoidTheRes) == MAF_OK &&
    parent->StoreDouble("EllipsoidPhiRes",m_EllipsoidPhiRes) == MAF_OK &&
    parent->StoreInteger("EllipsoidOrientationAxis",m_CylinderOrientationAxis) == MAF_OK
    )
		return MAF_OK;
	}
	return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMESurfaceParametric::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
	if (Superclass::InternalRestore(node)==MAF_OK)
	{
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix); 
      node->RestoreInteger("Geometry",m_GeometryType);
      node->RestoreDouble("ShereRadius",m_SphereRadius);
      node->RestoreDouble("SpherePhiRes",m_SpherePhiRes);
      node->RestoreDouble("SphereThetaRes",m_SphereTheRes);
      node->RestoreDouble("ConeHieght",m_ConeHeight);
      node->RestoreDouble("ConeRadius",m_ConeRadius);
      node->RestoreInteger("ConeCapping",m_ConeCapping);
      node->RestoreDouble("ConeRes",m_ConeRes);
      node->RestoreInteger("ConeOrientationAxis",m_ConeOrientationAxis);
      node->RestoreDouble("CylinderHeight",m_CylinderHeight);
      node->RestoreDouble("CylinderRadius",m_CylinderRadius);
      node->RestoreDouble("CylinderRes",m_CylinderRes);
      node->RestoreInteger("CylinderOrientationAxis",m_CylinderOrientationAxis);
      node->RestoreDouble("CubeXLength",m_CubeXLength);
      node->RestoreDouble("CubeYLength",m_CubeYLength);
      node->RestoreDouble("CubeZLength",m_CubeZLength);
      node->RestoreDouble("PlaneXRes",m_PlaneXRes);
      node->RestoreDouble("PlaneYRes",m_PlaneYRes);
      node->RestoreVectorN("PlaneOrigin",m_PlaneOrigin,3);
      node->RestoreVectorN("PlanePoint1",m_PlanePoint1,3);
      node->RestoreVectorN("PlanePoint2",m_PlanePoint2,3);
      node->RestoreDouble("EllipsoidXLenght",m_EllipsoidXLenght);
      node->RestoreDouble("EllipsoidYLenght",m_EllipsoidYLenght);
      node->RestoreDouble("EllipsoidZLenght",m_EllipsoidZLenght);
      node->RestoreDouble("EllipsoidTheRes",m_EllipsoidTheRes);
      node->RestoreDouble("EllipsoidPhiRes",m_EllipsoidPhiRes);
      node->RestoreInteger("EllipsoidOrientationAxis",m_CylinderOrientationAxis);
      return MAF_OK;
    }
	}
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
void mafVMESurfaceParametric::SetGeometryType( int parametricSurfaceTypeID )
//-------------------------------------------------------------------------
{
  m_GeometryType = parametricSurfaceTypeID;
  Modified();
}
//-------------------------------------------------------------------------
void mafVMESurfaceParametric::SetSphereRadius(double radius)
//-------------------------------------------------------------------------
{
  m_SphereRadius = radius;
  Modified();
}
//-------------------------------------------------------------------------
char** mafVMESurfaceParametric::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMEProcedural.xpm"
  return mafVMEProcedural_xpm;
}

void mafVMESurfaceParametric::CreateGuiPlane()
{
  m_GuiPlane= new mafGUI(this);
  m_GuiPlane->Label("Plane");
  m_GuiPlane->Double(CHANGE_VALUE_PLANE,_("X Res"), &m_PlaneXRes);
  m_GuiPlane->Double(CHANGE_VALUE_PLANE,_("Y Res"), &m_PlaneYRes);
  m_GuiPlane->Vector(CHANGE_VALUE_PLANE,_("Origin"), m_PlaneOrigin);
  m_GuiPlane->Vector(CHANGE_VALUE_PLANE,_("Point 1"), m_PlanePoint1);
  m_GuiPlane->Vector(CHANGE_VALUE_PLANE,_("Point 2"), m_PlanePoint2);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiPlane);
}

void mafVMESurfaceParametric::CreateGuiCube()
{
  m_GuiCube = new mafGUI(this);
  m_GuiCube->Label("Cube");
  m_GuiCube->Double(CHANGE_VALUE_CUBE,_("X Length"), &m_CubeXLength);
  m_GuiCube->Double(CHANGE_VALUE_CUBE,_("Y Length"), &m_CubeYLength);
  m_GuiCube->Double(CHANGE_VALUE_CUBE,_("Z Length"), &m_CubeZLength);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiCube);
}

void mafVMESurfaceParametric::CreateGuiCylinder()
{
  m_GuiCylinder = new mafGUI(this);
  m_GuiCylinder->Label("Cylinder");
  m_GuiCylinder->Double(CHANGE_VALUE_CYLINDER,_("Height"), &m_CylinderHeight);
  m_GuiCylinder->Double(CHANGE_VALUE_CYLINDER,_("Radius"), &m_CylinderRadius);
  m_GuiCylinder->Double(CHANGE_VALUE_CYLINDER,_("Resolution"), &m_CylinderRes);
  wxString orientationArray[3] = {_("X axis"),_("Y axis"),_("Z axis")};
  m_GuiCylinder->Radio(CHANGE_VALUE_CYLINDER,"Orientation", &m_CylinderOrientationAxis, 3,orientationArray);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiCylinder);

}

void mafVMESurfaceParametric::CreateGuiCone()
{
  m_GuiCone = new mafGUI(this);
  m_GuiCone->Label("Cone");
  m_GuiCone->Double(CHANGE_VALUE_CONE,_("Height"), &m_ConeHeight);
  m_GuiCone->Double(CHANGE_VALUE_CONE,_("Radius"), &m_ConeRadius);
  m_GuiCone->Double(CHANGE_VALUE_CONE,_("Resolution"), &m_ConeRes);
  m_GuiCone->Bool(CHANGE_VALUE_CONE,"Cap", &m_ConeCapping); // Open or closed cone
  wxString orientationArray[3] = {_("X axis"),_("Y axis"),_("Z axis")};
  m_GuiCone->Radio(CHANGE_VALUE_CONE,"Orientation", &m_ConeOrientationAxis, 3,orientationArray);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiCone);
}

void mafVMESurfaceParametric::CreateGuiSphere()
{
  m_GuiSphere = new mafGUI(this);
  m_GuiSphere->Label("Sphere");
  m_GuiSphere->Double(CHANGE_VALUE_SPHERE,_("Radius"), &m_SphereRadius);
  m_GuiSphere->Double(CHANGE_VALUE_SPHERE,_("Phi res"), &m_SpherePhiRes);
  m_GuiSphere->Double(CHANGE_VALUE_SPHERE,_("Theta res"), &m_SphereTheRes);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiSphere);
}

void mafVMESurfaceParametric::CreateGuiEllipsoid()
{
  m_GuiEllipsoid = new mafGUI(this);
  m_GuiEllipsoid->Label("Ellipsoid");
  m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID,_("X Length"), &m_EllipsoidXLenght);
  m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID,_("Y Length"), &m_EllipsoidYLenght);
  m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID,_("Z Length"), &m_EllipsoidZLenght);
  m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID,_("Phi res"), &m_EllipsoidPhiRes);
  m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID,_("Theta res"), &m_EllipsoidTheRes);
  wxString orientationArray[3] = {_("X axis"),_("Y axis"),_("Z axis")};
  m_GuiEllipsoid->Radio(CHANGE_VALUE_ELLIPSOID,"Orientation", &m_EllipsoidOrientationAxis, 3,orientationArray);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiEllipsoid);
}

void mafVMESurfaceParametric::EnableGuiPlane()
{
  m_GuiPlane->Enable(CHANGE_VALUE_PLANE, true);
  m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
  m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
  m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
  m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
  m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}

void mafVMESurfaceParametric::EnableGuiCube()
{
  m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
  m_GuiCube->Enable(CHANGE_VALUE_CUBE, true);
  m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
  m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
  m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
  m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}

void mafVMESurfaceParametric::EnableGuiCylinder()
{
  m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
  m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
  m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, true);
  m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
  m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
  m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}

void mafVMESurfaceParametric::EnableGuiCone()
{
  m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
  m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
  m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
  m_GuiCone->Enable(CHANGE_VALUE_CONE, true);
  m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
  m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}

void mafVMESurfaceParametric::EnableGuiSphere()
{
  m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
  m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
  m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
  m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
  m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, true);
  m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}

void mafVMESurfaceParametric::EnableGuiEllipsoid()
{
  m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
  m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
  m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
  m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
  m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
  m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, true);
}

void mafVMESurfaceParametric::EnableParametricSurfaceGui( int surfaceTypeID )
{
  switch(surfaceTypeID)
  {
    case PARAMETRIC_SPHERE:
       EnableGuiSphere();
       
       if (DEBUG_MODE)
         {
           std::ostringstream stringStream;
           stringStream << "enabling Sphere gui" << std::endl;
           mafLogMessage(stringStream.str().c_str());
         }
    break;
  
    case PARAMETRIC_CONE:
      EnableGuiCone();
      if (DEBUG_MODE)
      {
        std::ostringstream stringStream;
        stringStream << "enabling Cone gui" << std::endl;
        mafLogMessage(stringStream.str().c_str());
      }

    break;
    
    case PARAMETRIC_CYLINDER:
      EnableGuiCylinder();
      if (DEBUG_MODE)
      {
        std::ostringstream stringStream;
        stringStream << "enabling Cylinder gui" << std::endl;
        mafLogMessage(stringStream.str().c_str());
      }

    break;
  
    case PARAMETRIC_CUBE:
      EnableGuiCube();
      if (DEBUG_MODE)
      {
        std::ostringstream stringStream;
        stringStream << "enabling Cube gui" << std::endl;
        mafLogMessage(stringStream.str().c_str());
      }

    break;

    case PARAMETRIC_PLANE:
      EnableGuiPlane();
      if (DEBUG_MODE)
      {
        std::ostringstream stringStream;
        stringStream << "enabling Plane gui" << std::endl;
        mafLogMessage(stringStream.str().c_str());
      }

    break;

    case PARAMETRIC_ELLIPSOID:
      EnableGuiEllipsoid();
      if (DEBUG_MODE)
      {
        std::ostringstream stringStream;
        stringStream << "enabling Ellipsoid gui" << std::endl;
        mafLogMessage(stringStream.str().c_str());
      }

      break;
    
    default:
      break;
  }
}