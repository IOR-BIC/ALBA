/*=========================================================================

 Program: MAF2
 Module: mafVMESurfaceParametric
 Authors: Roberto Mucci , Stefano Perticoni, Nicola Vanella
 
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
{
	vtkDEL(m_PolyData);
  mafDEL(m_Transform);
	SetOutput(NULL);
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMESurfaceParametric::GetMaterial()
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

		this->m_TruncatedConeHeight = vmeParametricSurface->m_TruncatedConeHeight;
		this->m_TruncatedConeUpperDiameter = vmeParametricSurface->m_TruncatedConeUpperDiameter;
		this->m_TruncatedConeLowerDiameter = vmeParametricSurface->m_TruncatedConeLowerDiameter;
		this->m_TruncatedConeRes = vmeParametricSurface->m_TruncatedConeRes;
		this->m_TruncatedConeCapping = vmeParametricSurface->m_TruncatedConeCapping;
		this->m_TruncatedConeOrientationAxis = vmeParametricSurface->m_TruncatedConeOrientationAxis;

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
{
	bool ret = false;
	if (Superclass::Equals(vme))
	{
		if (
			m_Transform->GetMatrix() == ((mafVMESurfaceParametric *)vme)->m_Transform->GetMatrix() &&
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

			this->m_PlaneOrigin[0] == ((mafVMESurfaceParametric *)vme)->m_PlaneOrigin[0] &&
			this->m_PlaneOrigin[1] == ((mafVMESurfaceParametric *)vme)->m_PlaneOrigin[1] &&
			this->m_PlaneOrigin[2] == ((mafVMESurfaceParametric *)vme)->m_PlaneOrigin[2] &&

			this->m_PlanePoint1[0] == ((mafVMESurfaceParametric *)vme)->m_PlanePoint1[0] &&
			this->m_PlanePoint1[1] == ((mafVMESurfaceParametric *)vme)->m_PlanePoint1[1] &&
			this->m_PlanePoint1[2] == ((mafVMESurfaceParametric *)vme)->m_PlanePoint1[2] &&

			this->m_PlanePoint2[0] == ((mafVMESurfaceParametric *)vme)->m_PlanePoint2[0] &&
			this->m_PlanePoint2[1] == ((mafVMESurfaceParametric *)vme)->m_PlanePoint2[1] &&
			this->m_PlanePoint2[2] == ((mafVMESurfaceParametric *)vme)->m_PlanePoint2[2] &&

			this->m_EllipsoidXLenght == ((mafVMESurfaceParametric *)vme)->m_EllipsoidXLenght &&
			this->m_EllipsoidYLenght == ((mafVMESurfaceParametric *)vme)->m_EllipsoidYLenght &&
			this->m_EllipsoidZLenght == ((mafVMESurfaceParametric *)vme)->m_EllipsoidZLenght &&
			this->m_EllipsoidPhiRes == ((mafVMESurfaceParametric *)vme)->m_EllipsoidPhiRes &&
			this->m_EllipsoidTheRes == ((mafVMESurfaceParametric *)vme)->m_EllipsoidTheRes &&
			this->m_EllipsoidOrientationAxis == ((mafVMESurfaceParametric *)vme)->m_EllipsoidOrientationAxis &&

			this->m_TruncatedConeHeight == ((mafVMESurfaceParametric *)vme)->m_TruncatedConeHeight &&
			this->m_TruncatedConeUpperDiameter == ((mafVMESurfaceParametric *)vme)->m_TruncatedConeUpperDiameter &&
			this->m_TruncatedConeLowerDiameter == ((mafVMESurfaceParametric *)vme)->m_TruncatedConeLowerDiameter &&
			this->m_TruncatedConeRes == ((mafVMESurfaceParametric *)vme)->m_TruncatedConeRes &&
			this->m_TruncatedConeCapping == ((mafVMESurfaceParametric *)vme)->m_TruncatedConeCapping &&
			this->m_TruncatedConeOrientationAxis == ((mafVMESurfaceParametric *)vme)->m_TruncatedConeOrientationAxis
			)
		{
			ret = true;
		}
	}
	return ret;
}
//-------------------------------------------------------------------------
mafVMEOutputSurface *mafVMESurfaceParametric::GetSurfaceOutput()
{
	return (mafVMEOutputSurface *)GetOutput();
}
//-------------------------------------------------------------------------
void mafVMESurfaceParametric::SetMatrix(const mafMatrix &mat)
{
	m_Transform->SetMatrix(mat);
	//Update AbsMatrix output
	GetOutput()->GetAbsMatrix();
	Modified();
}
//-------------------------------------------------------------------------
bool mafVMESurfaceParametric::IsAnimated()
{
	return false;
}
//-------------------------------------------------------------------------
void mafVMESurfaceParametric::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
{
	kframes.clear(); // no timestamps
  mafTimeStamp t = m_Transform->GetMatrix().GetTimeStamp();
  kframes.push_back(t);
}

//-------------------------------------------------------------------------
void mafVMESurfaceParametric::OnEvent(mafEventBase *maf_event)
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_GEOMETRY_TYPE:
      {  
        EnableParametricSurfaceGui(m_GeometryType);
        InternalUpdate();
      }

      case CHANGE_VALUE_SPHERE:
      case CHANGE_VALUE_CUBE:
      case CHANGE_VALUE_CONE:
      case CHANGE_VALUE_CYLINDER:
      case CHANGE_VALUE_PLANE:
      case CHANGE_VALUE_ELLIPSOID:
			case CHANGE_VALUE_TRUNCATED_CONE:
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
{
	InternalInitialize();
}
//-----------------------------------------------------------------------
void mafVMESurfaceParametric::InternalUpdate()
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
	}
}
//-----------------------------------------------------------------------
int mafVMESurfaceParametric::InternalStore(mafStorageElement *parent)
{
	if (Superclass::InternalStore(parent) == MAF_OK)
	{
		if (
			parent->StoreMatrix("Transform", &m_Transform->GetMatrix()) == MAF_OK &&
			parent->StoreInteger("Geometry", m_GeometryType) == MAF_OK &&

			parent->StoreDouble("ShereRadius", m_SphereRadius) == MAF_OK &&
			parent->StoreDouble("SpherePhiRes", m_SpherePhiRes) == MAF_OK &&
			parent->StoreDouble("SphereThetaRes", m_SphereTheRes) == MAF_OK &&

			parent->StoreDouble("ConeHieght", m_ConeHeight) == MAF_OK &&
			parent->StoreDouble("ConeRadius", m_ConeRadius) == MAF_OK &&
			parent->StoreInteger("ConeCapping", m_ConeCapping) == MAF_OK &&
			parent->StoreDouble("ConeRes", m_ConeRes) == MAF_OK &&
			parent->StoreInteger("ConeOrientationAxis", m_ConeOrientationAxis) == MAF_OK &&

			parent->StoreDouble("CylinderHeight", m_CylinderHeight) == MAF_OK &&
			parent->StoreDouble("CylinderRadius", m_CylinderRadius) == MAF_OK &&
			parent->StoreDouble("CylinderRes", m_CylinderRes) == MAF_OK &&
			parent->StoreInteger("CylinderOrientationAxis", m_CylinderOrientationAxis) == MAF_OK &&

			parent->StoreDouble("CubeXLength", m_CubeXLength) == MAF_OK &&
			parent->StoreDouble("CubeYLength", m_CubeYLength) == MAF_OK &&
			parent->StoreDouble("CubeZLength", m_CubeZLength) == MAF_OK &&

			parent->StoreDouble("PlaneXRes", m_PlaneXRes) == MAF_OK &&
			parent->StoreDouble("PlaneYRes", m_PlaneYRes) == MAF_OK &&
			parent->StoreVectorN("PlaneOrigin", m_PlaneOrigin, 3) == MAF_OK &&
			parent->StoreVectorN("PlanePoint1", m_PlanePoint1, 3) == MAF_OK &&
			parent->StoreVectorN("PlanePoint2", m_PlanePoint2, 3) == MAF_OK &&

			parent->StoreDouble("EllipsoidXLenght", m_EllipsoidXLenght) == MAF_OK &&
			parent->StoreDouble("EllipsoidYLenght", m_EllipsoidYLenght) == MAF_OK &&
			parent->StoreDouble("EllipsoidZLenght", m_EllipsoidZLenght) == MAF_OK &&
			parent->StoreDouble("EllipsoidTheRes", m_EllipsoidTheRes) == MAF_OK &&
			parent->StoreDouble("EllipsoidPhiRes", m_EllipsoidPhiRes) == MAF_OK &&
			parent->StoreInteger("EllipsoidOrientationAxis", m_CylinderOrientationAxis) == MAF_OK &&

			parent->StoreInteger("TruncatedConeHeight", m_TruncatedConeHeight) == MAF_OK &&
			parent->StoreInteger("TruncatedConeUpperDiameter", m_TruncatedConeUpperDiameter) == MAF_OK &&
			parent->StoreInteger("TruncatedConeLowerDiameter", m_TruncatedConeLowerDiameter) == MAF_OK &&
			parent->StoreInteger("TruncatedConeRes", m_TruncatedConeRes) == MAF_OK &&
			parent->StoreInteger("TruncatedConeCapping", m_TruncatedConeCapping) == MAF_OK &&
			parent->StoreInteger("TruncatedConeOrientationAxis", m_TruncatedConeOrientationAxis) == MAF_OK
			)
			return MAF_OK;
	}
	return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mafVMESurfaceParametric::InternalRestore(mafStorageElement *node)
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

			node->RestoreDouble("TruncatedConeHeight", m_TruncatedConeHeight);
			node->RestoreDouble("TruncatedConeUpperDiameter", m_TruncatedConeUpperDiameter);
			node->RestoreDouble("TruncatedConeLowerDiameter", m_TruncatedConeLowerDiameter);
			node->RestoreDouble("TruncatedConeRes", m_TruncatedConeRes);
			node->RestoreInteger("TruncatedConeCapping", m_TruncatedConeCapping);
			node->RestoreInteger("TruncatedConeOrientationAxis", m_TruncatedConeOrientationAxis);

      return MAF_OK;
    }
	}
  return MAF_ERROR;
}

//-----------------------------------------------------------------------
void mafVMESurfaceParametric::CreateTruncatedCone()
{
	int RADIAL_STEPS = m_TruncatedConeRes;
	int LINEAR_STEPS = 25;

	int nPoints = RADIAL_STEPS*LINEAR_STEPS;

	vtkPoints *newPoints;
	vtkNEW(newPoints);

	newPoints->SetNumberOfPoints(nPoints);

	double baseHeight = 0;
	int currentPoint = 0;

	//Generating point structure
	for (int j = 0; j < LINEAR_STEPS; j++)
	{
		double lowDiameterRatio = ((double)(j)) / (double)(LINEAR_STEPS - 1);
		double upDiameterRatio = 1.0 - lowDiameterRatio;
		double currentRadius = ((lowDiameterRatio*m_TruncatedConeLowerDiameter + upDiameterRatio*m_TruncatedConeUpperDiameter) * 0.5);
		double currentHeight = baseHeight + (lowDiameterRatio*m_TruncatedConeHeight);

		for (int k = 0; k < RADIAL_STEPS; k++)
		{
			double pointCoord[3];
			double angle = (double)k / (double)RADIAL_STEPS*2.0*M_PI;
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

	for (int ls = 0; ls < (LINEAR_STEPS - 1); ls++)
	{
		//radial steps
		for (int rs = 0; rs < (RADIAL_STEPS - 1); rs++)
		{
			polys->InsertNextCell(3);
			polys->InsertCellPoint(rs + (ls + 1)*RADIAL_STEPS);
			polys->InsertCellPoint((rs + 1) + (ls + 1)*RADIAL_STEPS);
			polys->InsertCellPoint(rs + ls*RADIAL_STEPS);
			polys->InsertNextCell(3);
			polys->InsertCellPoint((rs + 1) + (ls + 1)*RADIAL_STEPS);
			polys->InsertCellPoint((rs + 1) + ls*RADIAL_STEPS);
			polys->InsertCellPoint(rs + ls*RADIAL_STEPS);
		}

		//connect last->first cell
		polys->InsertNextCell(3);
		polys->InsertCellPoint((RADIAL_STEPS - 1) + (ls + 1)*RADIAL_STEPS);
		polys->InsertCellPoint(+(ls + 1)*RADIAL_STEPS);
		polys->InsertCellPoint((RADIAL_STEPS - 1) + ls*RADIAL_STEPS);
		polys->InsertNextCell(3);
		polys->InsertCellPoint(+(ls + 1)*RADIAL_STEPS);
		polys->InsertCellPoint(+ls*RADIAL_STEPS);
		polys->InsertCellPoint((RADIAL_STEPS - 1) + ls*RADIAL_STEPS);
	}

	if (m_TruncatedConeCapping > 0)
	{
		//Starting Cap
		polys->InsertNextCell(RADIAL_STEPS);
		for (int i = 0; i < RADIAL_STEPS; i++)
			polys->InsertCellPoint(i);

		//Ending Cap
		polys->InsertNextCell(RADIAL_STEPS);
		for (int i = 0; i < RADIAL_STEPS; i++)
			polys->InsertCellPoint((nPoints - RADIAL_STEPS) + i);
	}

	polyData->SetPolys(polys);
	polyData->Update();

	// Transform
	vtkMAFSmartPointer<vtkTransform> t;

	switch (m_TruncatedConeOrientationAxis)
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
	ptf->SetInput(polyData);
	ptf->Update();

	vtkDEL(polys);
	vtkDEL(polyData);

	m_PolyData->DeepCopy(ptf->GetOutput());
	m_PolyData->Update();

 	m_DataPipe->Update();
}
//-----------------------------------------------------------------------
void mafVMESurfaceParametric::CreateEllipsoid()
{
	vtkMAFSmartPointer<vtkSphereSource> surf;
	surf->SetRadius(m_EllipsoidYLenght);
	surf->SetPhiResolution(m_EllipsoidPhiRes);
	surf->SetThetaResolution(m_EllipsoidTheRes);
	surf->Update();

	vtkMAFSmartPointer<vtkTransform> t;

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

	vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
	ptf->SetTransform(t);
	ptf->SetInput(surf->GetOutput());
	ptf->Update();

	m_PolyData->DeepCopy(ptf->GetOutput());
	m_PolyData->Update();
}
//-----------------------------------------------------------------------
void mafVMESurfaceParametric::CreatePlane()
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
//-----------------------------------------------------------------------
void mafVMESurfaceParametric::CreateCube()
{
	vtkMAFSmartPointer<vtkCubeSource> surf;
	surf->SetXLength(m_CubeXLength);
	surf->SetYLength(m_CubeYLength);
	surf->SetZLength(m_CubeZLength);
	surf->Update();
	m_PolyData->DeepCopy(surf->GetOutput());
	m_PolyData->Update();
}
//-----------------------------------------------------------------------
void mafVMESurfaceParametric::CreateCylinder()
{
	vtkMAFSmartPointer<vtkCylinderSource> surf;
	surf->SetHeight(m_CylinderHeight);
	surf->SetRadius(m_CylinderRadius);
	surf->SetResolution(m_CylinderRes);
	surf->Update();

	vtkMAFSmartPointer<vtkTransform> t;

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

	vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
	ptf->SetTransform(t);
	ptf->SetInput(surf->GetOutput());
	ptf->Update();

	m_PolyData->DeepCopy(ptf->GetOutput());
	m_PolyData->Update();
}
//-----------------------------------------------------------------------
void mafVMESurfaceParametric::CreateCone()
{
	vtkMAFSmartPointer<vtkConeSource> surf;
	surf->SetHeight(m_ConeHeight);
	surf->SetRadius(m_ConeRadius);
	surf->SetCapping(m_ConeCapping);
	surf->SetResolution(m_ConeRes);
	surf->Update();

	vtkMAFSmartPointer<vtkTransform> t;

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

	vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
	ptf->SetTransform(t);
	ptf->SetInput(surf->GetOutput());
	ptf->Update();

	m_PolyData->DeepCopy(ptf->GetOutput());
	m_PolyData->Update();
}
//-----------------------------------------------------------------------
void mafVMESurfaceParametric::CreateSphere()
{
	vtkMAFSmartPointer<vtkSphereSource> surf;
	surf->SetRadius(m_SphereRadius);
	surf->SetPhiResolution(m_SpherePhiRes);
	surf->SetThetaResolution(m_SphereTheRes);
	surf->Update();
	m_PolyData->DeepCopy(surf->GetOutput());
	m_PolyData->Update();
}

// SET ////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
void mafVMESurfaceParametric::SetGeometryType( int parametricSurfaceTypeID )
{
  m_GeometryType = parametricSurfaceTypeID;
  Modified();
}
//-------------------------------------------------------------------------
void mafVMESurfaceParametric::SetSphereRadius(double radius)
{
  m_SphereRadius = radius;
  Modified();
}
//----------------------------------------------------------------------------
void mafVMESurfaceParametric::SetCylinderRadius(double cylinderRadius)
{
	m_CylinderRadius = cylinderRadius;
	Modified();
}
//----------------------------------------------------------------------------
void mafVMESurfaceParametric::SetCylinderHeight(double cylinderHeight)
{
	m_CylinderHeight = cylinderHeight;
	Modified();
}

//-------------------------------------------------------------------------
char** mafVMESurfaceParametric::GetIcon() 
{
  #include "mafVMEProcedural.xpm"
  return mafVMEProcedural_xpm;
}

// CREATE GUI /////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
void mafVMESurfaceParametric::CreateGuiPlane()
{
  m_GuiPlane= new mafGUI(this);

  m_GuiPlane->Double(CHANGE_VALUE_PLANE,_("X Res"), &m_PlaneXRes);
  m_GuiPlane->Double(CHANGE_VALUE_PLANE,_("Y Res"), &m_PlaneYRes);
  m_GuiPlane->Vector(CHANGE_VALUE_PLANE,_("Origin"), m_PlaneOrigin);
  m_GuiPlane->Vector(CHANGE_VALUE_PLANE,_("Point 1"), m_PlanePoint1);
  m_GuiPlane->Vector(CHANGE_VALUE_PLANE,_("Point 2"), m_PlanePoint2);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiPlane);
}
//-------------------------------------------------------------------------
void mafVMESurfaceParametric::CreateGuiCube()
{
  m_GuiCube = new mafGUI(this);

  m_GuiCube->Double(CHANGE_VALUE_CUBE,_("X Length"), &m_CubeXLength);
  m_GuiCube->Double(CHANGE_VALUE_CUBE,_("Y Length"), &m_CubeYLength);
  m_GuiCube->Double(CHANGE_VALUE_CUBE,_("Z Length"), &m_CubeZLength);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiCube);
}
//-------------------------------------------------------------------------
void mafVMESurfaceParametric::CreateGuiCylinder()
{
  m_GuiCylinder = new mafGUI(this);

  m_GuiCylinder->Double(CHANGE_VALUE_CYLINDER,_("Height"), &m_CylinderHeight);
  m_GuiCylinder->Double(CHANGE_VALUE_CYLINDER,_("Radius"), &m_CylinderRadius);
  m_GuiCylinder->Double(CHANGE_VALUE_CYLINDER,_("Resolution"), &m_CylinderRes);
  wxString orientationArray[3] = {_("X axis"),_("Y axis"),_("Z axis")};
  m_GuiCylinder->Radio(CHANGE_VALUE_CYLINDER,"Orientation", &m_CylinderOrientationAxis, 3,orientationArray);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiCylinder);

}
//-------------------------------------------------------------------------
void mafVMESurfaceParametric::CreateGuiCone()
{
  m_GuiCone = new mafGUI(this);

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
void mafVMESurfaceParametric::CreateGuiSphere()
{
  m_GuiSphere = new mafGUI(this);

  m_GuiSphere->Double(CHANGE_VALUE_SPHERE,_("Radius"), &m_SphereRadius);
  m_GuiSphere->Double(CHANGE_VALUE_SPHERE,_("Phi res"), &m_SpherePhiRes);
  m_GuiSphere->Double(CHANGE_VALUE_SPHERE,_("Theta res"), &m_SphereTheRes);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiSphere);
}
//-------------------------------------------------------------------------
void mafVMESurfaceParametric::CreateGuiEllipsoid()
{
  m_GuiEllipsoid = new mafGUI(this);

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
void mafVMESurfaceParametric::CreateGuiTruncatedCone()
{
	m_GuiTruncatedCone = new mafGUI(this);

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
mafGUI* mafVMESurfaceParametric::CreateGui()
{
	mafVME::CreateGui();
	if (m_Gui)
	{
		wxString geometryType[7] = { "Sphere", "Cone", "Cylinder", "Cube", "Plane", "Ellipsoid", "Truncated Cone" };
		m_Gui->Combo(ID_GEOMETRY_TYPE, "", &m_GeometryType, 7, geometryType);
		m_Gui->Divider(2);
		
		CreateGuiSphere();
		CreateGuiCone();
		CreateGuiCylinder();
		CreateGuiCube();
		CreateGuiPlane();
		CreateGuiEllipsoid();
		CreateGuiTruncatedCone();

		m_Gui->Divider(2);
		
		AddLineToGUI(m_GuiSphere, 7);
		AddLineToGUI(m_GuiCone, 2);
		AddLineToGUI(m_GuiCylinder, 3);
		AddLineToGUI(m_GuiCube, 7);
		AddLineToGUI(m_GuiPlane, 5);
		AddLineToGUI(m_GuiEllipsoid, 1);
		AddLineToGUI(m_GuiTruncatedCone, 1);

		EnableParametricSurfaceGui(m_GeometryType);
	}

	m_Gui->Divider();
	return m_Gui;
}

//-------------------------------------------------------------------------
void mafVMESurfaceParametric::AddLineToGUI(mafGUI *gui, int nLines)
{
	for (int i = 0; i < nLines; i++) 
		gui->Label(" ", true);

	gui->FitGui();
	gui->Update();
}
//-------------------------------------------------------------------------
void mafVMESurfaceParametric::EnableParametricSurfaceGui( int surfaceTypeID )
{
	m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, surfaceTypeID == PARAMETRIC_SPHERE);
	m_GuiCone->Enable(CHANGE_VALUE_CONE, surfaceTypeID == PARAMETRIC_CONE);
	m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, surfaceTypeID == PARAMETRIC_CYLINDER);
	m_GuiCube->Enable(CHANGE_VALUE_CUBE, surfaceTypeID == PARAMETRIC_CUBE);
	m_GuiPlane->Enable(CHANGE_VALUE_PLANE, surfaceTypeID == PARAMETRIC_PLANE);
	m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, surfaceTypeID == PARAMETRIC_ELLIPSOID);
	m_GuiTruncatedCone->Enable(CHANGE_VALUE_TRUNCATED_CONE, surfaceTypeID == PARAMETRIC_TRUNCATED_CONE);
	
	m_GuiSphere->Show(surfaceTypeID == PARAMETRIC_SPHERE);
	m_GuiCone->Show(surfaceTypeID == PARAMETRIC_CONE);
	m_GuiCylinder->Show(surfaceTypeID == PARAMETRIC_CYLINDER);
	m_GuiCube->Show(surfaceTypeID == PARAMETRIC_CUBE);
	m_GuiPlane->Show(surfaceTypeID == PARAMETRIC_PLANE);
	m_GuiEllipsoid->Show(surfaceTypeID == PARAMETRIC_ELLIPSOID);
	m_GuiTruncatedCone->Show(surfaceTypeID == PARAMETRIC_TRUNCATED_CONE);

 	m_GuiSphere->FitGui();
	m_GuiCone->FitGui();
	m_GuiCylinder->FitGui();
	m_GuiCube->FitGui();
	m_GuiPlane->FitGui();
	m_GuiEllipsoid->FitGui();
	m_GuiTruncatedCone->FitGui();

	m_Gui->FitGui();
	m_Gui->Update();
}