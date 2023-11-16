/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMESurfaceParametric
 Authors: Roberto Mucci , Stefano Perticoni, Nicola Vanella
 
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

#include "albaVMESurfaceParametric.h"
#include "mmuIdFactory.h"
#include "albaDataVector.h"
#include "albaMatrixInterpolator.h"
#include "albaDataPipeInterpolator.h"
#include "albaTagArray.h"
#include "albaMatrixVector.h"
#include "albaVMEItemVTK.h"
#include "albaTransform.h"
#include "albaGUI.h"
#include "mmaMaterial.h"
#include "albaVMEOutputSurface.h"
#include "albaDataPipeCustom.h"

#include "vtkALBASmartPointer.h"
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
#include "vtkMath.h"
#include "vtkPolyLine.h"
#include "vtkLinearExtrusionFilter.h"
#include "albaOpManager.h"

const bool DEBUG_MODE = true;

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
//ALBA_ID_IMP(albaVMESurfaceParametric::CHANGE_PARAMETER);   // Event rised by change parameter 

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaVMESurfaceParametric);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaVMESurfaceParametric::albaVMESurfaceParametric()
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

	m_TruncatedConeHeight = 5.0;
	m_TruncatedConeUpperDiameter = 4.0;
	m_TruncatedConeLowerDiameter = 3.0;
	m_TruncatedConeRes = 20.0;
	m_TruncatedConeCapping = 0;
	m_TruncatedConeOrientationAxis = ID_X_AXIS;

	m_EllipticCylinderHeight = 5.0;
	m_EllipticCylinderR1 = 5.0;
	m_EllipticCylinderR2 = 3.0;
	m_EllipticCylinderRes = 20.0;
	m_EllipticCylinderCapping = 0;
	m_EllipticCylinderOrientationAxis = ID_X_AXIS;

	albaNEW(m_Transform);
	albaVMEOutputSurface *output=albaVMEOutputSurface::New(); // an output with no data
	output->SetTransform(m_Transform); // force my transform in the output
	SetOutput(output);

	GetMaterial();

	vtkNEW(m_PolyData);

  // attach a data pipe which creates a bridge between VTK and ALBA
	albaDataPipeCustom *dpipe = albaDataPipeCustom::New();
	dpipe->SetInput(m_PolyData);
	SetDataPipe(dpipe);
}

//-------------------------------------------------------------------------
albaVMESurfaceParametric::~albaVMESurfaceParametric()
{
	vtkDEL(m_PolyData);
  albaDEL(m_Transform);
	SetOutput(NULL);
}

//-------------------------------------------------------------------------
mmaMaterial *albaVMESurfaceParametric::GetMaterial()
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
int albaVMESurfaceParametric::DeepCopy(albaVME *a)
{ 
  if (Superclass::DeepCopy(a)==ALBA_OK)
  {
    albaVMESurfaceParametric *vmeParametricSurface=albaVMESurfaceParametric::SafeDownCast(a);
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

		this->m_TruncatedConeHeight = vmeParametricSurface->m_TruncatedConeHeight;
		this->m_TruncatedConeUpperDiameter = vmeParametricSurface->m_TruncatedConeUpperDiameter;
		this->m_TruncatedConeLowerDiameter = vmeParametricSurface->m_TruncatedConeLowerDiameter;
		this->m_TruncatedConeRes = vmeParametricSurface->m_TruncatedConeRes;
		this->m_TruncatedConeCapping = vmeParametricSurface->m_TruncatedConeCapping;
		this->m_TruncatedConeOrientationAxis = vmeParametricSurface->m_TruncatedConeOrientationAxis;

		this->m_EllipticCylinderHeight = vmeParametricSurface->m_EllipticCylinderHeight;
		this->m_EllipticCylinderR1 = vmeParametricSurface->m_EllipticCylinderR1;
		this->m_EllipticCylinderR2 = vmeParametricSurface->m_EllipticCylinderR2;
		this->m_EllipticCylinderRes = vmeParametricSurface->m_EllipticCylinderRes;
		this->m_EllipticCylinderCapping = vmeParametricSurface->m_EllipticCylinderCapping;
		this->m_EllipticCylinderOrientationAxis = vmeParametricSurface->m_EllipticCylinderOrientationAxis;

    albaDataPipeCustom *dpipe = albaDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetInput(m_PolyData);
    }
    InternalUpdate();
    return ALBA_OK;
  }  
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
bool albaVMESurfaceParametric::Equals(albaVME *vme)
{
	bool ret = false;
	if (Superclass::Equals(vme))
	{
		if (
			m_Transform->GetMatrix() == ((albaVMESurfaceParametric *)vme)->m_Transform->GetMatrix() &&
			this->m_GeometryType == ((albaVMESurfaceParametric *)vme)->m_GeometryType &&

			this->m_SphereRadius == ((albaVMESurfaceParametric *)vme)->m_SphereRadius &&
			this->m_SpherePhiRes == ((albaVMESurfaceParametric *)vme)->m_SpherePhiRes &&
			this->m_SphereTheRes == ((albaVMESurfaceParametric *)vme)->m_SphereTheRes &&

			this->m_ConeHeight == ((albaVMESurfaceParametric *)vme)->m_ConeHeight  &&
			this->m_ConeRadius == ((albaVMESurfaceParametric *)vme)->m_ConeRadius  &&
			this->m_ConeCapping == ((albaVMESurfaceParametric *)vme)->m_ConeCapping  &&
			this->m_ConeRes == ((albaVMESurfaceParametric *)vme)->m_ConeRes &&
			this->m_ConeOrientationAxis == ((albaVMESurfaceParametric *)vme)->m_ConeOrientationAxis &&

			this->m_CylinderHeight == ((albaVMESurfaceParametric *)vme)->m_CylinderHeight  &&
			this->m_CylinderRadius == ((albaVMESurfaceParametric *)vme)->m_CylinderRadius  &&
			this->m_CylinderRes == ((albaVMESurfaceParametric *)vme)->m_CylinderRes  &&
			this->m_CylinderOrientationAxis == ((albaVMESurfaceParametric *)vme)->m_CylinderOrientationAxis  &&

			this->m_CubeXLength == ((albaVMESurfaceParametric *)vme)->m_CubeXLength  &&
			this->m_CubeYLength == ((albaVMESurfaceParametric *)vme)->m_CubeYLength  &&
			this->m_CubeZLength == ((albaVMESurfaceParametric *)vme)->m_CubeZLength  &&

			this->m_PlaneXRes == ((albaVMESurfaceParametric *)vme)->m_PlaneXRes  &&
			this->m_PlaneYRes == ((albaVMESurfaceParametric *)vme)->m_PlaneYRes  &&

			this->m_PlaneOrigin[0] == ((albaVMESurfaceParametric *)vme)->m_PlaneOrigin[0] &&
			this->m_PlaneOrigin[1] == ((albaVMESurfaceParametric *)vme)->m_PlaneOrigin[1] &&
			this->m_PlaneOrigin[2] == ((albaVMESurfaceParametric *)vme)->m_PlaneOrigin[2] &&

			this->m_PlanePoint1[0] == ((albaVMESurfaceParametric *)vme)->m_PlanePoint1[0] &&
			this->m_PlanePoint1[1] == ((albaVMESurfaceParametric *)vme)->m_PlanePoint1[1] &&
			this->m_PlanePoint1[2] == ((albaVMESurfaceParametric *)vme)->m_PlanePoint1[2] &&

			this->m_PlanePoint2[0] == ((albaVMESurfaceParametric *)vme)->m_PlanePoint2[0] &&
			this->m_PlanePoint2[1] == ((albaVMESurfaceParametric *)vme)->m_PlanePoint2[1] &&
			this->m_PlanePoint2[2] == ((albaVMESurfaceParametric *)vme)->m_PlanePoint2[2] &&

			this->m_EllipsoidXLenght == ((albaVMESurfaceParametric *)vme)->m_EllipsoidXLenght &&
			this->m_EllipsoidYLenght == ((albaVMESurfaceParametric *)vme)->m_EllipsoidYLenght &&
			this->m_EllipsoidZLenght == ((albaVMESurfaceParametric *)vme)->m_EllipsoidZLenght &&
			this->m_EllipsoidPhiRes == ((albaVMESurfaceParametric *)vme)->m_EllipsoidPhiRes &&
			this->m_EllipsoidTheRes == ((albaVMESurfaceParametric *)vme)->m_EllipsoidTheRes &&
			this->m_EllipsoidOrientationAxis == ((albaVMESurfaceParametric *)vme)->m_EllipsoidOrientationAxis &&

			this->m_TruncatedConeHeight == ((albaVMESurfaceParametric *)vme)->m_TruncatedConeHeight &&
			this->m_TruncatedConeUpperDiameter == ((albaVMESurfaceParametric *)vme)->m_TruncatedConeUpperDiameter &&
			this->m_TruncatedConeLowerDiameter == ((albaVMESurfaceParametric *)vme)->m_TruncatedConeLowerDiameter &&
			this->m_TruncatedConeRes == ((albaVMESurfaceParametric *)vme)->m_TruncatedConeRes &&
			this->m_TruncatedConeCapping == ((albaVMESurfaceParametric *)vme)->m_TruncatedConeCapping &&
			this->m_TruncatedConeOrientationAxis == ((albaVMESurfaceParametric *)vme)->m_TruncatedConeOrientationAxis &&

			this->m_EllipticCylinderHeight == ((albaVMESurfaceParametric *)vme)->m_EllipticCylinderHeight &&
			this->m_EllipticCylinderR1 == ((albaVMESurfaceParametric *)vme)->m_EllipticCylinderR1 &&
			this->m_EllipticCylinderR2 == ((albaVMESurfaceParametric *)vme)->m_EllipticCylinderR2 &&
			this->m_EllipticCylinderRes == ((albaVMESurfaceParametric *)vme)->m_EllipticCylinderRes &&
			this->m_EllipticCylinderCapping == ((albaVMESurfaceParametric *)vme)->m_EllipticCylinderCapping &&
			this->m_EllipticCylinderOrientationAxis == ((albaVMESurfaceParametric *)vme)->m_EllipticCylinderOrientationAxis
			)
		{
			ret = true;
		}
	}
	return ret;
}
//-------------------------------------------------------------------------
albaVMEOutputSurface *albaVMESurfaceParametric::GetSurfaceOutput()
{
	return (albaVMEOutputSurface *)GetOutput();
}
//-------------------------------------------------------------------------
void albaVMESurfaceParametric::SetMatrix(const albaMatrix &mat)
{
	m_Transform->SetMatrix(mat);
	//Update AbsMatrix output
	GetOutput()->GetAbsMatrix();
	Modified();
}
//-------------------------------------------------------------------------
bool albaVMESurfaceParametric::IsAnimated()
{
	return false;
}
//-------------------------------------------------------------------------
void albaVMESurfaceParametric::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
{
	kframes.clear(); // no timestamps
  albaTimeStamp t = m_Transform->GetMatrix().GetTimeStamp();
  kframes.push_back(t);
}

//-------------------------------------------------------------------------
void albaVMESurfaceParametric::OnEvent(albaEventBase *alba_event)
{
  // events to be sent up or down in the tree are simply forwarded
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case ID_GEOMETRY_TYPE:
      {  
				SetGeometryType(m_GeometryType);
        InternalUpdate();
      }

      case CHANGE_VALUE_SPHERE:
      case CHANGE_VALUE_CUBE:
      case CHANGE_VALUE_CONE:
      case CHANGE_VALUE_CYLINDER:
      case CHANGE_VALUE_PLANE:
      case CHANGE_VALUE_ELLIPSOID:
			case CHANGE_VALUE_TRUNCATED_CONE:
			case CHANGE_VALUE_ELLIPTIC_CYLINDER:
      {
        InternalUpdate();
        e->SetId(CAMERA_UPDATE);
        ForwardUpEvent(e);
      }
      break;
      
      default:
        albaVME::OnEvent(alba_event);
    }
  }
  
  else
  {
    Superclass::OnEvent(alba_event);
  }
}

//-----------------------------------------------------------------------
void albaVMESurfaceParametric::InternalPreUpdate()
{
	InternalInitialize();
}
//-----------------------------------------------------------------------
void albaVMESurfaceParametric::InternalUpdate()
{
	switch(m_GeometryType)
	{
	case PARAMETRIC_SPHERE:
		{
		CreateSphere();
		}
	break;
	case PARAMETRIC_CONE:
	{
		CreateCone();
	}
	break;
	case PARAMETRIC_CYLINDER:
	{
		CreateCylinder();
	}
	break;
	case PARAMETRIC_CUBE:
	{
		CreateCube();
	}
	break;
  
  case PARAMETRIC_PLANE:
    {
		CreatePlane();
    }
    break;

  case PARAMETRIC_ELLIPSOID:
    {
		CreateEllipsoid();
    }
    break;

	case PARAMETRIC_TRUNCATED_CONE:
	{
		CreateTruncatedCone();
	}
	break;

	case PARAMETRIC_ELLIPTIC_CYLINDER:
	{
		CreateEllipticCylinder();
	}
	break;
	}
}
//-----------------------------------------------------------------------
int albaVMESurfaceParametric::InternalStore(albaStorageElement *parent)
{
	if (Superclass::InternalStore(parent) == ALBA_OK)
	{
		if (
			parent->StoreMatrix("Transform", &m_Transform->GetMatrix()) == ALBA_OK &&
			parent->StoreInteger("Geometry", m_GeometryType) == ALBA_OK &&

			parent->StoreDouble("ShereRadius", m_SphereRadius) == ALBA_OK &&
			parent->StoreDouble("SpherePhiRes", m_SpherePhiRes) == ALBA_OK &&
			parent->StoreDouble("SphereThetaRes", m_SphereTheRes) == ALBA_OK &&

			parent->StoreDouble("ConeHieght", m_ConeHeight) == ALBA_OK &&
			parent->StoreDouble("ConeRadius", m_ConeRadius) == ALBA_OK &&
			parent->StoreInteger("ConeCapping", m_ConeCapping) == ALBA_OK &&
			parent->StoreDouble("ConeRes", m_ConeRes) == ALBA_OK &&
			parent->StoreInteger("ConeOrientationAxis", m_ConeOrientationAxis) == ALBA_OK &&

			parent->StoreDouble("CylinderHeight", m_CylinderHeight) == ALBA_OK &&
			parent->StoreDouble("CylinderRadius", m_CylinderRadius) == ALBA_OK &&
			parent->StoreDouble("CylinderRes", m_CylinderRes) == ALBA_OK &&
			parent->StoreInteger("CylinderOrientationAxis", m_CylinderOrientationAxis) == ALBA_OK &&

			parent->StoreDouble("CubeXLength", m_CubeXLength) == ALBA_OK &&
			parent->StoreDouble("CubeYLength", m_CubeYLength) == ALBA_OK &&
			parent->StoreDouble("CubeZLength", m_CubeZLength) == ALBA_OK &&

			parent->StoreDouble("PlaneXRes", m_PlaneXRes) == ALBA_OK &&
			parent->StoreDouble("PlaneYRes", m_PlaneYRes) == ALBA_OK &&
			parent->StoreVectorN("PlaneOrigin", m_PlaneOrigin, 3) == ALBA_OK &&
			parent->StoreVectorN("PlanePoint1", m_PlanePoint1, 3) == ALBA_OK &&
			parent->StoreVectorN("PlanePoint2", m_PlanePoint2, 3) == ALBA_OK &&

			parent->StoreDouble("EllipsoidXLenght", m_EllipsoidXLenght) == ALBA_OK &&
			parent->StoreDouble("EllipsoidYLenght", m_EllipsoidYLenght) == ALBA_OK &&
			parent->StoreDouble("EllipsoidZLenght", m_EllipsoidZLenght) == ALBA_OK &&
			parent->StoreDouble("EllipsoidTheRes", m_EllipsoidTheRes) == ALBA_OK &&
			parent->StoreDouble("EllipsoidPhiRes", m_EllipsoidPhiRes) == ALBA_OK &&
			parent->StoreInteger("EllipsoidOrientationAxis", m_CylinderOrientationAxis) == ALBA_OK &&

			parent->StoreInteger("TruncatedConeHeight", m_TruncatedConeHeight) == ALBA_OK &&
			parent->StoreInteger("TruncatedConeUpperDiameter", m_TruncatedConeUpperDiameter) == ALBA_OK &&
			parent->StoreInteger("TruncatedConeLowerDiameter", m_TruncatedConeLowerDiameter) == ALBA_OK &&
			parent->StoreInteger("TruncatedConeRes", m_TruncatedConeRes) == ALBA_OK &&
			parent->StoreInteger("TruncatedConeCapping", m_TruncatedConeCapping) == ALBA_OK &&
			parent->StoreInteger("TruncatedConeOrientationAxis", m_TruncatedConeOrientationAxis) == ALBA_OK &&

			parent->StoreDouble("EllipticCylinderHeight", m_EllipticCylinderHeight) == ALBA_OK &&
			parent->StoreDouble("EllipticCylinderR1", m_EllipticCylinderR1) == ALBA_OK &&
			parent->StoreDouble("EllipticCylinderR2", m_EllipticCylinderR2) == ALBA_OK &&
			parent->StoreInteger("EllipticCylinderRes", m_EllipticCylinderRes) == ALBA_OK &&
			parent->StoreInteger("EllipticCylinderCapping", m_EllipticCylinderCapping) == ALBA_OK &&
			parent->StoreInteger("EllipticCylinderOrientationAxis", m_EllipticCylinderOrientationAxis) == ALBA_OK
			)
			return ALBA_OK;
	}
	return ALBA_ERROR;
}
//-----------------------------------------------------------------------
int albaVMESurfaceParametric::InternalRestore(albaStorageElement *node)
{
	if (Superclass::InternalRestore(node)==ALBA_OK)
	{
    albaMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==ALBA_OK)
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

			node->RestoreDouble("TruncatedConeHeight", m_TruncatedConeHeight);
			node->RestoreDouble("TruncatedConeUpperDiameter", m_TruncatedConeUpperDiameter);
			node->RestoreDouble("TruncatedConeLowerDiameter", m_TruncatedConeLowerDiameter);
			node->RestoreDouble("TruncatedConeRes", m_TruncatedConeRes);
			node->RestoreInteger("TruncatedConeCapping", m_TruncatedConeCapping);
			node->RestoreInteger("TruncatedConeOrientationAxis", m_TruncatedConeOrientationAxis);

			node->RestoreDouble("EllipticCylinderHeight", m_EllipticCylinderHeight);
			node->RestoreDouble("EllipticCylinderR1", m_EllipticCylinderR1);
			node->RestoreDouble("EllipticCylinderR2", m_EllipticCylinderR2);
			node->RestoreDouble("EllipticCylinderRes", m_EllipticCylinderRes);
			node->RestoreInteger("EllipticCylinderCapping", m_EllipticCylinderCapping);
			node->RestoreInteger("EllipticCylinderOrientationAxis", m_EllipticCylinderOrientationAxis);

      return ALBA_OK;
    }
	}
  return ALBA_ERROR;
}

//-----------------------------------------------------------------------
void albaVMESurfaceParametric::CreateTruncatedCone()
{
	int nPoints = m_TruncatedConeRes *2;

	vtkPoints *newPoints;
	vtkNEW(newPoints);

	newPoints->SetNumberOfPoints(nPoints);

	double baseHeight = 0;
	int currentPoint = 0;

	//Generating point structure
	for (int j = 0; j < 2; j++)
	{
		double lowDiameterRatio = ((double)(j)) / (double)(2 - 1);
		double upDiameterRatio = 1.0 - lowDiameterRatio;
		double currentRadius = ((lowDiameterRatio*m_TruncatedConeLowerDiameter + upDiameterRatio*m_TruncatedConeUpperDiameter) * 0.5);
		double currentHeight = baseHeight + (lowDiameterRatio*m_TruncatedConeHeight);

		for (int k = 0; k < m_TruncatedConeRes; k++)
		{
			double pointCoord[3];
			double angle = (double)k / (double)m_TruncatedConeRes*2.0*M_PI;
			pointCoord[0] = sin(angle)*currentRadius;
			pointCoord[1] = cos(angle)*currentRadius;
			pointCoord[2] = currentHeight;

			newPoints->SetPoint(currentPoint, pointCoord);
			currentPoint++;
		}
	}

	vtkPolyData *polyData;
	vtkNEW(polyData);

	polyData->SetPoints(newPoints);
	vtkDEL(newPoints);

	//generate cell structure
	vtkCellArray * polys;
	vtkNEW(polys);

	for (int ls = 0; ls < (2 - 1); ls++)
	{
		//radial steps
		for (int rs = 0; rs < (m_TruncatedConeRes - 1); rs++)
		{
			polys->InsertNextCell(3);
			polys->InsertCellPoint(rs + (ls + 1)*m_TruncatedConeRes);
			polys->InsertCellPoint((rs + 1) + (ls + 1)*m_TruncatedConeRes);
			polys->InsertCellPoint(rs + ls*m_TruncatedConeRes);
			polys->InsertNextCell(3);
			polys->InsertCellPoint((rs + 1) + (ls + 1)*m_TruncatedConeRes);
			polys->InsertCellPoint((rs + 1) + ls*m_TruncatedConeRes);
			polys->InsertCellPoint(rs + ls*m_TruncatedConeRes);
		}

		//connect last->first cell
		polys->InsertNextCell(3);
		polys->InsertCellPoint((m_TruncatedConeRes - 1) + (ls + 1)*m_TruncatedConeRes);
		polys->InsertCellPoint(+(ls + 1)*m_TruncatedConeRes);
		polys->InsertCellPoint((m_TruncatedConeRes - 1) + ls*m_TruncatedConeRes);
		polys->InsertNextCell(3);
		polys->InsertCellPoint(+(ls + 1)*m_TruncatedConeRes);
		polys->InsertCellPoint(+ls*m_TruncatedConeRes);
		polys->InsertCellPoint((m_TruncatedConeRes - 1) + ls*m_TruncatedConeRes);
	}

	if (m_TruncatedConeCapping > 0)
	{
		//Starting Cap
		polys->InsertNextCell(m_TruncatedConeRes);
		for (int i = 0; i < m_TruncatedConeRes; i++)
			polys->InsertCellPoint(i);

		//Ending Cap
		polys->InsertNextCell(m_TruncatedConeRes);
		for (int i = 0; i < m_TruncatedConeRes; i++)
			polys->InsertCellPoint((nPoints - m_TruncatedConeRes) + i);
	}

	polyData->SetPolys(polys);
	polyData->Update();

	// Transform
	vtkALBASmartPointer<vtkTransform> t;

	switch (m_TruncatedConeOrientationAxis)
	{
	case ID_X_AXIS:
		//do nothing
		break;
	case ID_Y_AXIS:
		t->RotateX(90);
		break;
	case ID_Z_AXIS:
		t->RotateY(-90);
		break;
	default:
		break;
	}

	t->Update();
	
	vtkALBASmartPointer<vtkTransformPolyDataFilter> ptf;
	ptf->SetTransform(t);
	ptf->SetInput(polyData);
	ptf->Update();

	vtkDEL(polys);
	vtkDEL(polyData);

	m_PolyData->DeepCopy(ptf->GetOutput());
	m_PolyData->Update();

 	m_DataPipe->Update();
}
//-----------------------------------------------------------------------
void albaVMESurfaceParametric::CreateEllipsoid()
{
	vtkALBASmartPointer<vtkSphereSource> surf;
	surf->SetRadius(m_EllipsoidYLenght);
	surf->SetPhiResolution(m_EllipsoidPhiRes);
	surf->SetThetaResolution(m_EllipsoidTheRes);
	surf->Update();

	vtkALBASmartPointer<vtkTransform> t;

	switch (m_EllipsoidOrientationAxis)
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

	t->Scale(m_EllipsoidXLenght / m_EllipsoidYLenght, 1, m_EllipsoidZLenght / m_EllipsoidYLenght);
	t->Update();

	vtkALBASmartPointer<vtkTransformPolyDataFilter> ptf;
	ptf->SetTransform(t);
	ptf->SetInput(surf->GetOutput());
	ptf->Update();

	m_PolyData->DeepCopy(ptf->GetOutput());
	m_PolyData->Update();
}
//-----------------------------------------------------------------------
void albaVMESurfaceParametric::CreatePlane()
{
	vtkALBASmartPointer<vtkPlaneSource> surf;
	surf->SetXResolution(m_PlaneXRes);
	surf->SetYResolution(m_PlaneYRes);
	surf->SetOrigin(m_PlaneOrigin);
	surf->SetPoint1(m_PlanePoint1);
	surf->SetPoint2(m_PlanePoint2);
	surf->Update();
	vtkALBASmartPointer<vtkTriangleFilter> triangle;
	triangle->SetInput(surf->GetOutput());
	triangle->Update();
	m_PolyData->DeepCopy(triangle->GetOutput());
	m_PolyData->Update();
}
//-----------------------------------------------------------------------
void albaVMESurfaceParametric::CreateCube()
{
	vtkALBASmartPointer<vtkCubeSource> surf;
	surf->SetXLength(m_CubeXLength);
	surf->SetZLength(m_CubeYLength);
	surf->SetYLength(m_CubeZLength);
	surf->Update();
	m_PolyData->DeepCopy(surf->GetOutput());
	m_PolyData->Update();
}
//-----------------------------------------------------------------------
void albaVMESurfaceParametric::CreateCylinder()
{
	vtkALBASmartPointer<vtkCylinderSource> surf;
	surf->SetHeight(m_CylinderHeight);
	surf->SetRadius(m_CylinderRadius);
	surf->SetResolution(m_CylinderRes);
	surf->Update();

	vtkALBASmartPointer<vtkTransform> t;

	switch (m_CylinderOrientationAxis)
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

	vtkALBASmartPointer<vtkTransformPolyDataFilter> ptf;
	ptf->SetTransform(t);
	ptf->SetInput(surf->GetOutput());
	ptf->Update();

	m_PolyData->DeepCopy(ptf->GetOutput());
	m_PolyData->Update();
}
//-----------------------------------------------------------------------
void albaVMESurfaceParametric::CreateCone()
{
	vtkALBASmartPointer<vtkConeSource> surf;
	surf->SetHeight(m_ConeHeight);
	surf->SetRadius(m_ConeRadius);
	surf->SetCapping(m_ConeCapping);
	surf->SetResolution(m_ConeRes);
	surf->Update();

	vtkALBASmartPointer<vtkTransform> t;

	switch (m_ConeOrientationAxis)
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

	vtkALBASmartPointer<vtkTransformPolyDataFilter> ptf;
	ptf->SetTransform(t);
	ptf->SetInput(surf->GetOutput());
	ptf->Update();

	m_PolyData->DeepCopy(ptf->GetOutput());
	m_PolyData->Update();
}
//-----------------------------------------------------------------------
void albaVMESurfaceParametric::CreateSphere()
{
	vtkALBASmartPointer<vtkSphereSource> surf;
	surf->SetRadius(m_SphereRadius);
	surf->SetPhiResolution(m_SpherePhiRes);
	surf->SetThetaResolution(m_SphereTheRes);
	surf->Update();
	m_PolyData->DeepCopy(surf->GetOutput());
	m_PolyData->Update();
}
//-------------------------------------------------------------------------
void albaVMESurfaceParametric::CreateEllipticCylinder()
{
	double angle = 0;
	int id = 0;
	int centerX = 0, centerY = 0;
	int nPoints = m_EllipticCylinderRes * 2;

	vtkPoints *newPoints;
	vtkNEW(newPoints);

	newPoints->SetNumberOfPoints(nPoints);
	
	for (int i=0; i<m_EllipticCylinderRes; i++)
	{
		angle = 2 * vtkMath::Pi() * (i/m_EllipticCylinderRes);
		newPoints->SetPoint(id, m_EllipticCylinderR1 * cos(angle) + centerX, m_EllipticCylinderR2 * sin(angle) + centerY, -m_EllipticCylinderHeight / 2.0);
		id++;
	}

	for (int i = 0; i < m_EllipticCylinderRes; i++)
	{
		angle = 2 * vtkMath::Pi() * (i / m_EllipticCylinderRes);
		newPoints->SetPoint(id, m_EllipticCylinderR1 * cos(angle) + centerX, m_EllipticCylinderR2 * sin(angle) + centerY, +m_EllipticCylinderHeight / 2.0);
		id++;
	}

	vtkPolyData *polyData;
	vtkNEW(polyData);

	polyData->SetPoints(newPoints);
	vtkDEL(newPoints);

	//generate cell structure
	vtkCellArray * polys;
	vtkNEW(polys);

	for (int ls = 0; ls < (2 - 1); ls++)
	{
		//radial steps
		for (int rs = 0; rs < (m_EllipticCylinderRes - 1); rs++)
		{
			polys->InsertNextCell(3);
			polys->InsertCellPoint(rs + ls*m_EllipticCylinderRes);
			polys->InsertCellPoint((rs + 1) + (ls + 1)*m_EllipticCylinderRes);
			polys->InsertCellPoint(rs + (ls + 1)*m_EllipticCylinderRes);
			polys->InsertNextCell(3);
			polys->InsertCellPoint(rs + ls*m_EllipticCylinderRes);
			polys->InsertCellPoint((rs + 1) + ls*m_EllipticCylinderRes);
			polys->InsertCellPoint((rs + 1) + (ls + 1)*m_EllipticCylinderRes);
		}

		//connect last->first cell
		polys->InsertNextCell(3);
		polys->InsertCellPoint((m_EllipticCylinderRes - 1) + ls*m_EllipticCylinderRes);
		polys->InsertCellPoint(+(ls + 1)*m_EllipticCylinderRes);
		polys->InsertCellPoint((m_EllipticCylinderRes - 1) + (ls + 1)*m_EllipticCylinderRes);
		polys->InsertNextCell(3);
		polys->InsertCellPoint((m_EllipticCylinderRes - 1) + ls*m_EllipticCylinderRes);
		polys->InsertCellPoint(+ls*m_EllipticCylinderRes);
		polys->InsertCellPoint(+(ls + 1)*m_EllipticCylinderRes);
	}

	if (m_EllipticCylinderCapping > 0)
	{
		//Starting Cap
		polys->InsertNextCell(m_EllipticCylinderRes);
		for (int i = m_EllipticCylinderRes-1; i >=0 ; i--)
			polys->InsertCellPoint(i);

		//Ending Cap
		polys->InsertNextCell(m_EllipticCylinderRes);
		for (int i = 0; i < m_EllipticCylinderRes; i++)
			polys->InsertCellPoint(m_EllipticCylinderRes + i);
	}

	polyData->SetPolys(polys);
	polyData->Update();

	// Transform
	vtkALBASmartPointer<vtkTransform> t;

	switch (m_EllipticCylinderOrientationAxis)
	{
	case ID_X_AXIS:
		//do nothing
		break;
	case ID_Y_AXIS:
		t->RotateX(90);
		break;
	case ID_Z_AXIS:
		t->RotateY(-90);
		break;
	default:
		break;
	}

	t->Update();

	vtkALBASmartPointer<vtkTransformPolyDataFilter> ptf;
	ptf->SetTransform(t);
	ptf->SetInput(polyData);
	ptf->Update();

	vtkDEL(polys);
	vtkDEL(polyData);

	m_PolyData->DeepCopy(ptf->GetOutput());
	m_PolyData->Update();

	m_DataPipe->Update();
}

// SET ////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
void albaVMESurfaceParametric::SetGeometryType( int parametricSurfaceTypeID )
{
  m_GeometryType = parametricSurfaceTypeID;
	EnableParametricSurfaceGui(m_GeometryType);

	
	albaOpManager * opManager = GetLogicManager()->GetOpManager();
	if(opManager)
		opManager->EnableOp();

  Modified();
}
//-------------------------------------------------------------------------
void albaVMESurfaceParametric::SetSphereRadius(double radius)
{
  m_SphereRadius = radius;
  Modified();
}
//----------------------------------------------------------------------------
void albaVMESurfaceParametric::SetCylinderRadius(double cylinderRadius)
{
	m_CylinderRadius = cylinderRadius;
	Modified();
}
//----------------------------------------------------------------------------
void albaVMESurfaceParametric::SetCylinderHeight(double cylinderHeight)
{
	m_CylinderHeight = cylinderHeight;
	Modified();
}

//----------------------------------------------------------------------------
void albaVMESurfaceParametric::SetEllipticCylinderR1(double val)
{
	m_EllipticCylinderR1 = val;
	Modified();
}

//----------------------------------------------------------------------------
void albaVMESurfaceParametric::SetEllipticCylinderHeight(double val)
{
	m_EllipticCylinderHeight = val;
	Modified();
}

//----------------------------------------------------------------------------
void albaVMESurfaceParametric::SetEllipticCylinderR2(double val)
{
	m_EllipticCylinderR2 = val;
	Modified();
}

//----------------------------------------------------------------------------
void albaVMESurfaceParametric::SetEllipticCylinderCapping(int val)
{
	m_EllipticCylinderCapping = val;
	Modified();
}

//----------------------------------------------------------------------------
void albaVMESurfaceParametric::SetCylinderRes(double val)
{
	m_CylinderRes = val;
	Modified();
}

//----------------------------------------------------------------------------
void albaVMESurfaceParametric::SetEllipticCylinderRes(double val)
{
	m_EllipticCylinderRes = val;
	Modified();
}

//-------------------------------------------------------------------------
char** albaVMESurfaceParametric::GetIcon() 
{
  #include "albaVMEProcedural.xpm"
  return albaVMEProcedural_xpm;
}

// CREATE GUI /////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
void albaVMESurfaceParametric::CreateGuiPlane()
{
  m_GuiPlane= new albaGUI(this);

  m_GuiPlane->Double(CHANGE_VALUE_PLANE,_("X Res"), &m_PlaneXRes);
  m_GuiPlane->Double(CHANGE_VALUE_PLANE,_("Y Res"), &m_PlaneYRes);
  m_GuiPlane->Vector(CHANGE_VALUE_PLANE,_("Origin"), m_PlaneOrigin);
  m_GuiPlane->Vector(CHANGE_VALUE_PLANE,_("Point 1"), m_PlanePoint1);
  m_GuiPlane->Vector(CHANGE_VALUE_PLANE,_("Point 2"), m_PlanePoint2);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiPlane);
}
//-------------------------------------------------------------------------
void albaVMESurfaceParametric::CreateGuiCube()
{
  m_GuiCube = new albaGUI(this);

  m_GuiCube->Double(CHANGE_VALUE_CUBE,_("X Length"), &m_CubeXLength);
  m_GuiCube->Double(CHANGE_VALUE_CUBE,_("Y Length"), &m_CubeYLength);
  m_GuiCube->Double(CHANGE_VALUE_CUBE,_("Z Length"), &m_CubeZLength);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiCube);
}
//-------------------------------------------------------------------------
void albaVMESurfaceParametric::CreateGuiCylinder()
{
  m_GuiCylinder = new albaGUI(this);

  m_GuiCylinder->Double(CHANGE_VALUE_CYLINDER,_("Height"), &m_CylinderHeight);
  m_GuiCylinder->Double(CHANGE_VALUE_CYLINDER,_("Radius"), &m_CylinderRadius);
  m_GuiCylinder->Double(CHANGE_VALUE_CYLINDER,_("Resolution"), &m_CylinderRes);
  wxString orientationArray[3] = {_("X axis"),_("Y axis"),_("Z axis")};
  m_GuiCylinder->Radio(CHANGE_VALUE_CYLINDER,"Orientation", &m_CylinderOrientationAxis, 3,orientationArray);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiCylinder);

}
//-------------------------------------------------------------------------
void albaVMESurfaceParametric::CreateGuiCone()
{
  m_GuiCone = new albaGUI(this);

  m_GuiCone->Double(CHANGE_VALUE_CONE,_("Height"), &m_ConeHeight);
  m_GuiCone->Double(CHANGE_VALUE_CONE,_("Radius"), &m_ConeRadius);
  m_GuiCone->Double(CHANGE_VALUE_CONE,_("Resolution"), &m_ConeRes);
  m_GuiCone->Bool(CHANGE_VALUE_CONE,"Cap", &m_ConeCapping); // Open or closed cone
  wxString orientationArray[3] = {_("X axis"),_("Y axis"),_("Z axis")};
  m_GuiCone->Radio(CHANGE_VALUE_CONE,"Orientation", &m_ConeOrientationAxis, 3,orientationArray);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiCone);
}
//-------------------------------------------------------------------------
void albaVMESurfaceParametric::CreateGuiSphere()
{
  m_GuiSphere = new albaGUI(this);

  m_GuiSphere->Double(CHANGE_VALUE_SPHERE,_("Radius"), &m_SphereRadius);
  m_GuiSphere->Double(CHANGE_VALUE_SPHERE,_("Phi res"), &m_SpherePhiRes);
  m_GuiSphere->Double(CHANGE_VALUE_SPHERE,_("Theta res"), &m_SphereTheRes);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiSphere);
}
//-------------------------------------------------------------------------
void albaVMESurfaceParametric::CreateGuiEllipsoid()
{
  m_GuiEllipsoid = new albaGUI(this);

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
//-------------------------------------------------------------------------
void albaVMESurfaceParametric::CreateGuiTruncatedCone()
{
	m_GuiTruncatedCone = new albaGUI(this);

	m_GuiTruncatedCone->Double(CHANGE_VALUE_TRUNCATED_CONE, _("Height"), &m_TruncatedConeHeight);
	m_GuiTruncatedCone->Double(CHANGE_VALUE_TRUNCATED_CONE, _("Up Radius"), &m_TruncatedConeUpperDiameter);
	m_GuiTruncatedCone->Double(CHANGE_VALUE_TRUNCATED_CONE, _("Dw Radius"), &m_TruncatedConeLowerDiameter);
	m_GuiTruncatedCone->Double(CHANGE_VALUE_TRUNCATED_CONE, _("Resolution"), &m_TruncatedConeRes);
	m_GuiTruncatedCone->Bool(CHANGE_VALUE_TRUNCATED_CONE, "Cap", &m_TruncatedConeCapping); // Open or closed cone
	wxString orientationArray[3] = { _("X axis"),_("Y axis"),_("Z axis") };
	m_GuiTruncatedCone->Radio(CHANGE_VALUE_TRUNCATED_CONE, "Orientation", &m_TruncatedConeOrientationAxis, 3, orientationArray);
	assert(m_Gui);
	m_Gui->AddGui(m_GuiTruncatedCone);
}
//-------------------------------------------------------------------------
void albaVMESurfaceParametric::CreateGuiEllipticCylinder()
{
	m_GuiEllipticCylinder = new albaGUI(this);

	m_GuiEllipticCylinder->Double(CHANGE_VALUE_ELLIPTIC_CYLINDER, _("Height"), &m_EllipticCylinderHeight);
	m_GuiEllipticCylinder->Double(CHANGE_VALUE_ELLIPTIC_CYLINDER, _("R1"), &m_EllipticCylinderR1);
	m_GuiEllipticCylinder->Double(CHANGE_VALUE_ELLIPTIC_CYLINDER, _("R2"), &m_EllipticCylinderR2);
	m_GuiEllipticCylinder->Double(CHANGE_VALUE_ELLIPTIC_CYLINDER, _("Resolution"), &m_EllipticCylinderRes);
	m_GuiEllipticCylinder->Bool(CHANGE_VALUE_ELLIPTIC_CYLINDER, "Cap", &m_EllipticCylinderCapping); // Open or closed cylinder
	wxString orientationArray[3] = { _("X axis"),_("Y axis"),_("Z axis") };
	m_GuiEllipticCylinder->Radio(CHANGE_VALUE_ELLIPTIC_CYLINDER, "Orientation", &m_EllipticCylinderOrientationAxis, 3, orientationArray);
	
	assert(m_Gui);
	m_Gui->AddGui(m_GuiEllipticCylinder);
}

//-------------------------------------------------------------------------
albaGUI* albaVMESurfaceParametric::CreateGui()
{
	albaVME::CreateGui();
	if (m_Gui)
	{
		wxString geometryType[8] = { "Sphere", "Cone", "Cylinder", "Cube", "Plane", "Ellipsoid", "Truncated Cone", "Elliptic Cylinder", };
		m_Gui->Combo(ID_GEOMETRY_TYPE, "", &m_GeometryType, 8, geometryType);
		m_Gui->Divider();
		
		CreateGuiSphere();
		CreateGuiCone();
		CreateGuiCylinder();
		CreateGuiCube();
		CreateGuiPlane();
		CreateGuiEllipsoid();
		CreateGuiTruncatedCone();
		CreateGuiEllipticCylinder();

		AddLineToGUI(m_GuiSphere, 7);
		AddLineToGUI(m_GuiCone, 2);
		AddLineToGUI(m_GuiCylinder, 3);
		AddLineToGUI(m_GuiCube, 7);
		AddLineToGUI(m_GuiPlane, 5);
		AddLineToGUI(m_GuiEllipsoid, 1);
		AddLineToGUI(m_GuiTruncatedCone, 1);
		AddLineToGUI(m_GuiEllipticCylinder, 1);

		EnableParametricSurfaceGui(m_GeometryType);
	}

	m_Gui->Divider();
	return m_Gui;
}

//-------------------------------------------------------------------------
void albaVMESurfaceParametric::AddLineToGUI(albaGUI *gui, int nLines)
{
	for (int i = 0; i < nLines; i++) 
		gui->Label(" ", true);

	gui->FitGui();
	gui->Update();
}
//-------------------------------------------------------------------------
void albaVMESurfaceParametric::EnableParametricSurfaceGui( int surfaceTypeID )
{
	if (m_Gui)
	{
		m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, surfaceTypeID == PARAMETRIC_SPHERE);
		m_GuiCone->Enable(CHANGE_VALUE_CONE, surfaceTypeID == PARAMETRIC_CONE);
		m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, surfaceTypeID == PARAMETRIC_CYLINDER);
		m_GuiCube->Enable(CHANGE_VALUE_CUBE, surfaceTypeID == PARAMETRIC_CUBE);
		m_GuiPlane->Enable(CHANGE_VALUE_PLANE, surfaceTypeID == PARAMETRIC_PLANE);
		m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, surfaceTypeID == PARAMETRIC_ELLIPSOID);
		m_GuiTruncatedCone->Enable(CHANGE_VALUE_TRUNCATED_CONE, surfaceTypeID == PARAMETRIC_TRUNCATED_CONE);
		m_GuiEllipticCylinder->Enable(CHANGE_VALUE_ELLIPTIC_CYLINDER, surfaceTypeID == PARAMETRIC_ELLIPTIC_CYLINDER);

		m_GuiSphere->Show(surfaceTypeID == PARAMETRIC_SPHERE);
		m_GuiCone->Show(surfaceTypeID == PARAMETRIC_CONE);
		m_GuiCylinder->Show(surfaceTypeID == PARAMETRIC_CYLINDER);
		m_GuiCube->Show(surfaceTypeID == PARAMETRIC_CUBE);
		m_GuiPlane->Show(surfaceTypeID == PARAMETRIC_PLANE);
		m_GuiEllipsoid->Show(surfaceTypeID == PARAMETRIC_ELLIPSOID);
		m_GuiTruncatedCone->Show(surfaceTypeID == PARAMETRIC_TRUNCATED_CONE);
		m_GuiEllipticCylinder->Show(surfaceTypeID == PARAMETRIC_ELLIPTIC_CYLINDER);

		m_GuiSphere->FitGui();
		m_GuiCone->FitGui();
		m_GuiCylinder->FitGui();
		m_GuiCube->FitGui();
		m_GuiPlane->FitGui();
		m_GuiEllipsoid->FitGui();
		m_GuiTruncatedCone->FitGui();
		m_GuiEllipticCylinder->FitGui();

		m_Gui->FitGui();
		m_Gui->Update();
	}
}