/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMESurfaceParametric.cpp,v $
  Language:  C++
  Date:      $Date: 2007-07-25 14:06:32 $
  Version:   $Revision: 1.8 $
  Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
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
#include "mafDataInterpolator.h"
#include "mafTagArray.h"
#include "mafMatrixVector.h"
#include "mafVMEItemVTK.h"
#include "mafEventSource.h"
#include "mafTransform.h"
#include "mmgGui.h"
#include "mmgRollOut.h"
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

#include <vector>

//------------------------------------------------------------------------------
// local defines
//------------------------------------------------------------------------------


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
  m_GeometryType = 0;

	m_RollOutCube = NULL;
	m_RollOutSphere = NULL;
	m_RollOutCylinder = NULL;
	m_RollOutCone = NULL;
  m_RollOutPlane = NULL;

	
  m_SphereRadius = 2.0;
  m_SpherePhiRes = 10.0;
  m_SphereTheRes = 10.0;
  m_ConeHeight = 5.0;
  m_ConeRadius = 2.0;
  m_ConeCapping = 0;
  m_ConeRes = 20.0;
  m_CylinderHeight = 5.0;
  m_CylinderRadius = 2.0;
  m_CylinderRes = 20.0;
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
int mafVMESurfaceParametric::DeepCopy(mafNode *a)
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
    this->m_CylinderHeight = vmeParametricSurface->m_CylinderHeight;
    this->m_CylinderRadius = vmeParametricSurface->m_CylinderRadius;
    this->m_CylinderRes = vmeParametricSurface->m_CylinderRes;
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
      this->m_CylinderHeight == ((mafVMESurfaceParametric *)vme)->m_CylinderHeight  &&
      this->m_CylinderRadius == ((mafVMESurfaceParametric *)vme)->m_CylinderRadius  &&
      this->m_CylinderRes == ((mafVMESurfaceParametric *)vme)->m_CylinderRes  &&
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
      this->m_PlanePoint2[2] == ((mafVMESurfaceParametric *)vme)->m_PlanePoint2[2] 
      )
    {
      return MAF_OK;
    }
  }
 return MAF_ERROR;
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
}
//-------------------------------------------------------------------------
mmgGui* mafVMESurfaceParametric::CreateGui()
//-------------------------------------------------------------------------
{
  mafVME::CreateGui();
  m_Gui->Divider();
  
	wxString geometry_array[5] = {_("Sphere"),_("Cone"),_("Cylinder"),_("Parallelepiped"),_("Plane")};
	m_Gui->Combo(CHANGE_PARAMETER,_("Type"),&m_GeometryType, 5, geometry_array);
	SourceGui();
  
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
      case CHANGE_PARAMETER:
        if(m_Gui)
        {
          m_RollOutSphere->RollOut(m_GeometryType == PARAMETRIC_SPHERE);
          m_RollOutCone->RollOut(m_GeometryType == PARAMETRIC_CONE);
          m_RollOutCylinder->RollOut(m_GeometryType == PARAMETRIC_CYLINDER);
          m_RollOutCube->RollOut(m_GeometryType == PARAMETRIC_CUBE);
          m_RollOutPlane->RollOut(m_GeometryType == PARAMETRIC_PLANE);
        }
				/*InternalUpdate();
				this->ForwardUpEvent(mafEvent(this,CAMERA_UPDATE));
      break;*/
      case CHANGE_VALUE_SPHERE:
      case CHANGE_VALUE_CUBE:
      case CHANGE_VALUE_CONE:
      case CHANGE_VALUE_CYLINDER:
      case CHANGE_VALUE_PLANE:
      {
	InternalUpdate();
        mafEvent ev(this,CAMERA_UPDATE);
	this->ForwardUpEvent(&ev);
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
//-------------------------------------------------------------------------
void mafVMESurfaceParametric::SourceGui()
//-------------------------------------------------------------------------
{
	if(m_Gui)
	{
		mmgGui *guiSphere = new mmgGui(this);
		mmgGui *guiCone = new mmgGui(this);
		mmgGui *guiCylinder = new mmgGui(this);
		mmgGui *guiCube = new mmgGui(this);
    mmgGui *guiPlane = new mmgGui(this);

  	guiSphere->Double(CHANGE_VALUE_SPHERE,_("Radius"), &m_SphereRadius);
    guiSphere->Double(CHANGE_VALUE_SPHERE,_("Phi res"), &m_SpherePhiRes);
    guiSphere->Double(CHANGE_VALUE_SPHERE,_("Theta res"), &m_SphereTheRes);
  
		guiCone->Double(CHANGE_VALUE_CYLINDER,_("Height"), &m_ConeHeight);
    guiCone->Double(CHANGE_VALUE_CYLINDER,_("Radius"), &m_ConeRadius);
    guiCone->Double(CHANGE_VALUE_CYLINDER,_("Resolution"), &m_ConeRes);
    guiCone->Bool(CHANGE_VALUE_CYLINDER,"Cap", &m_ConeCapping); // Open or closed cone

  	guiCylinder->Double(CHANGE_VALUE_CONE,_("Height"), &m_CylinderHeight);
    guiCylinder->Double(CHANGE_VALUE_CONE,_("Radius"), &m_CylinderRadius);
    guiCylinder->Double(CHANGE_VALUE_CONE,_("Resolution"), &m_CylinderRes);

		guiCube->Double(CHANGE_VALUE_CUBE,_("X Length"), &m_CubeXLength);
    guiCube->Double(CHANGE_VALUE_CUBE,_("Y Length"), &m_CubeYLength);
    guiCube->Double(CHANGE_VALUE_CUBE,_("Z Length"), &m_CubeZLength);

    guiPlane->Double(CHANGE_VALUE_PLANE,_("X Res"), &m_PlaneXRes);
    guiPlane->Double(CHANGE_VALUE_PLANE,_("Y Res"), &m_PlaneYRes);
    guiPlane->Vector(CHANGE_VALUE_PLANE,_("Origin"), m_PlaneOrigin);
    guiPlane->Vector(CHANGE_VALUE_PLANE,_("Point 1"), m_PlanePoint1);
    guiPlane->Vector(CHANGE_VALUE_PLANE,_("Point 2"), m_PlanePoint2);

		switch(m_GeometryType)
		{
			case PARAMETRIC_SPHERE:
				m_RollOutSphere = m_Gui->RollOut(ID_ROLLOUT_SPHERE,_("Sphere Parameters"), guiSphere);
				m_RollOutCone = m_Gui->RollOut(ID_ROLLOUT_CONE,_("Cone Parameters"), guiCone, false);
				m_RollOutCylinder = m_Gui->RollOut(ID_ROLLOUT_CYLINDER,_("Cylinder Parameters"), guiCylinder, false);
				m_RollOutCube = m_Gui->RollOut(ID_ROLLOUT_CUBE,_("Cube Parameters"), guiCube, false);
        m_RollOutPlane = m_Gui->RollOut(ID_ROLLOUT_PLANE,_("Plane Parameters"), guiPlane, false);
			break;
			case PARAMETRIC_CONE:
				m_RollOutSphere = m_Gui->RollOut(ID_ROLLOUT_SPHERE,"Sphere Parameters", guiSphere, false);
				m_RollOutCone = m_Gui->RollOut(ID_ROLLOUT_CONE,_("Cone Parameters"), guiCone);
				m_RollOutCylinder = m_Gui->RollOut(ID_ROLLOUT_CYLINDER,"Cylinder Parameters", guiCylinder, false);
				m_RollOutCube = m_Gui->RollOut(ID_ROLLOUT_CUBE,_("Cube Parameters"), guiCube, false);
        m_RollOutPlane = m_Gui->RollOut(ID_ROLLOUT_PLANE,_("Plane Parameters"), guiPlane, false);

			break;
			case PARAMETRIC_CYLINDER:
				m_RollOutSphere = m_Gui->RollOut(ID_ROLLOUT_SPHERE,_("Sphere Parameters"), guiSphere, false);
				m_RollOutCone = m_Gui->RollOut(ID_ROLLOUT_CONE,_("Cone Parameters"), guiCone, false);
				m_RollOutCylinder = m_Gui->RollOut(ID_ROLLOUT_CYLINDER,_("Cylinder Parameters"), guiCylinder);
				m_RollOutCube = m_Gui->RollOut(ID_ROLLOUT_CUBE,_("Cube Parameters"), guiCube, false);
        m_RollOutPlane = m_Gui->RollOut(ID_ROLLOUT_PLANE,_("Plane Parameters"), guiPlane, false);

			break;
			case PARAMETRIC_CUBE:
				m_RollOutSphere = m_Gui->RollOut(ID_ROLLOUT_SPHERE,_("Sphere Parameters"), guiSphere, false);
				m_RollOutCone = m_Gui->RollOut(ID_ROLLOUT_CONE,_("Cone Parameters"), guiCone, false);
				m_RollOutCylinder = m_Gui->RollOut(ID_ROLLOUT_CYLINDER,_("Cylinder Parameters"), guiCylinder, false);
				m_RollOutCube = m_Gui->RollOut(ID_ROLLOUT_CUBE,_("Cube Parameters"), guiCube);
        m_RollOutPlane = m_Gui->RollOut(ID_ROLLOUT_PLANE,_("Plane Parameters"), guiPlane, false);

			break;
      case PARAMETRIC_PLANE:
        m_RollOutSphere = m_Gui->RollOut(ID_ROLLOUT_SPHERE,_("Sphere Parameters"), guiSphere, false);
        m_RollOutCone = m_Gui->RollOut(ID_ROLLOUT_CONE,_("Cone Parameters"), guiCone, false);
        m_RollOutCylinder = m_Gui->RollOut(ID_ROLLOUT_CYLINDER,_("Cylinder Parameters"), guiCylinder, false);
        m_RollOutCube = m_Gui->RollOut(ID_ROLLOUT_CUBE,_("Cube Parameters"), guiCube, false);
        m_RollOutPlane = m_Gui->RollOut(ID_ROLLOUT_PLANE,_("Plane Parameters"), guiPlane);

        break;
		}
		
		m_Gui->FitGui();
		m_Gui->Update();
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
    m_PolyData->DeepCopy(surf->GetOutput());
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
    m_PolyData->DeepCopy(surf->GetOutput());
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
    parent->StoreDouble("CylinderHeight",m_CylinderHeight) == MAF_OK &&
    parent->StoreDouble("CylinderRadius",m_CylinderRadius) == MAF_OK &&
    parent->StoreDouble("CylinderRes",m_CylinderRes) == MAF_OK &&
    parent->StoreDouble("CubeXLength",m_CubeXLength) == MAF_OK &&
    parent->StoreDouble("CubeYLength",m_CubeYLength) == MAF_OK &&
    parent->StoreDouble("CubeZLength",m_CubeZLength) == MAF_OK &&
    parent->StoreDouble("PlaneXRes",m_PlaneXRes) == MAF_OK &&
    parent->StoreDouble("PlaneYRes",m_PlaneYRes) == MAF_OK &&
    parent->StoreVectorN("PlaneOrigin",m_PlaneOrigin,3) == MAF_OK &&
    parent->StoreVectorN("PlanePoint1",m_PlanePoint1,3) == MAF_OK &&
    parent->StoreVectorN("PlanePoint2",m_PlanePoint2,3) == MAF_OK
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
    if (
      node->RestoreMatrix("Transform",&matrix)==MAF_OK &&
      node->RestoreInteger("Geometry",m_GeometryType) == MAF_OK && 
      node->RestoreDouble("ShereRadius",m_SphereRadius) == MAF_OK && 
      node->RestoreDouble("SpherePhiRes",m_SpherePhiRes) == MAF_OK && 
      node->RestoreDouble("SphereThetaRes",m_SphereTheRes) == MAF_OK && 
      node->RestoreDouble("ConeHieght",m_ConeHeight) == MAF_OK && 
      node->RestoreDouble("ConeRadius",m_ConeRadius) == MAF_OK && 
      node->RestoreInteger("ConeCapping",m_ConeCapping) == MAF_OK && 
      node->RestoreDouble("ConeRes",m_ConeRes) == MAF_OK && 
      node->RestoreDouble("CylinderHeight",m_CylinderHeight) == MAF_OK && 
      node->RestoreDouble("CylinderRadius",m_CylinderRadius) == MAF_OK && 
      node->RestoreDouble("CylinderRes",m_CylinderRes) == MAF_OK && 
      node->RestoreDouble("CubeXLength",m_CubeXLength) == MAF_OK && 
      node->RestoreDouble("CubeYLength",m_CubeYLength) == MAF_OK && 
      node->RestoreDouble("CubeZLength",m_CubeZLength) == MAF_OK && 
      node->RestoreDouble("PlaneXRes",m_PlaneXRes) == MAF_OK && 
      node->RestoreDouble("PlaneYRes",m_PlaneYRes) == MAF_OK && 
      node->RestoreVectorN("PlaneOrigin",m_PlaneOrigin,3) == MAF_OK && 
      node->RestoreVectorN("PlanePoint1",m_PlanePoint1,3) == MAF_OK && 
      node->RestoreVectorN("PlanePoint2",m_PlanePoint2,3) == MAF_OK
    )
    {
    m_Transform->SetMatrix(matrix); 
		return MAF_OK;
    }
	}
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
char** mafVMESurfaceParametric::GetIcon() 
//-------------------------------------------------------------------------
{

  #include "mafVMEProcedural.xpm"
  return mafVMEProcedural_xpm;



}
