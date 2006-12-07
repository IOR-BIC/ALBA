/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMESurfaceParametric.cpp,v $
  Language:  C++
  Date:      $Date: 2006-12-07 15:01:03 $
  Version:   $Revision: 1.1 $
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
mafCxxTypeMacro(mafVMESurfaceParametric);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafVMESurfaceParametric::mafVMESurfaceParametric()
//-------------------------------------------------------------------------
{
  m_GeometryType = 0;
	
	m_RollOutCube     = NULL;
	m_RollOutSphere   = NULL;
	m_RollOutCylinder = NULL;
	m_RollOutCone     = NULL;
  m_RollOutPlane    = NULL;
  m_PolyData        = NULL;

  m_SphereRadius  = 2.0;
  m_SpherePhiRes  = 10.0;
  m_SphereTheRes  = 10.0;
  m_ConeHeight    = 5.0;
  m_ConeRadius    = 2.0;
  m_ConeCapping   = 0;
  m_ConeRes       = 20.0;
  m_CylinderHeight = 5.0;
  m_CylinderRadius = 2.0;
  m_CylinderRes   = 20.0;
  m_CubeXLength   = 2.0;
  m_CubeYLength   = 2.0;
  m_CubeZLength   = 2.0;
  m_PlaneXRes     = 2.0;
  m_PlaneYRes     = 2.0;
  
  m_PlaneOrigin[0] = m_PlaneOrigin[1] = m_PlaneOrigin[2] = 0;
  m_PlanePoint1[0] = 2.0;
  m_PlanePoint1[1] = m_PlanePoint1[2] = 0.0;
  m_PlanePoint2[1] = 3.0;
  m_PlanePoint2[0] = m_PlanePoint2[2] = 0.0;

	mafNEW(m_Transform);
	mafVMEOutputSurface *output = mafVMEOutputSurface::New(); // an output with no data
	output->SetTransform(m_Transform); // force my transform in the output
	SetOutput(output);

	GetMaterial();

	vtkNEW(m_PolyData);
	// attach a data pipe which creates a bridge between VTK and MAF
	mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
	dpipe->SetDependOnAbsPose(true);
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
    //deep copy

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
    //equality
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
}
//-------------------------------------------------------------------------
mmgGui* mafVMESurfaceParametric::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = new mmgGui(this);
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
				InternalUpdate();
				this->ForwardUpEvent(mafEvent(this,CAMERA_UPDATE));
      break;
			case CHANGE_VALUE_SPHERE:
			case CHANGE_VALUE_CUBE:
			case CHANGE_VALUE_CONE:
			case CHANGE_VALUE_CYLINDER:
      case CHANGE_VALUE_PLANE:
				InternalUpdate();
				this->ForwardUpEvent(mafEvent(this,CAMERA_UPDATE));
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
		mmgGui *guiSphere   = new mmgGui(this);
		mmgGui *guiCone     = new mmgGui(this);
		mmgGui *guiCylinder = new mmgGui(this);
		mmgGui *guiCube     = new mmgGui(this);
    mmgGui *guiPlane    = new mmgGui(this);

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
    if ( parent->StoreMatrix("Transform",&m_Transform->GetMatrix()) == MAF_OK && 
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
      parent->StoreVectorN("PlanePoint2",m_PlanePoint2,3) == MAF_OK )
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
    if ( node->RestoreMatrix("Transform",&matrix)==MAF_OK &&
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
      node->RestoreVectorN("PlanePoint2",m_PlanePoint2,3) == MAF_OK )
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
  #include "mafVMESurface.xpm"
  return mafVMESurface_xpm;
}
