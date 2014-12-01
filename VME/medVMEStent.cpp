/*=========================================================================
Program:   MAF2
Module:    $RCSfile: medVMEStent.cpp,v $
Language:  C++
Date:      $Date: 2012-10-23 10:15:31 $
Version:   $Revision: 1.1.2.8 $
Authors:   Hui Wei
==========================================================================
Copyright (c) 2013
University of Bedfordshire, UK
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmuIdFactory.h"
#include "mafDataVector.h"
#include "mafTagArray.h"
#include "mafEventSource.h"
#include "mafVMEOutputSurface.h"
#include "mafVMEOutputPolyline.h"
#include "mafDataPipeCustom.h"
#include "mafGUI.h"
#include "mafVME.h"
#include "mafNode.h"
#include "vtkMAFDeformableSimplexMeshFilter.h"
#include "vtkMAFMatrixVectorMath.h"
#include "vtkMAFPolyDataNavigator.h"
#include "vtkMAFMatrixVectorMath.h"
#include "medVMEStent.h"
#include "mafStorageElement.h"
#include "vtkMAFPolyDataNavigator.h"

#include "vtkPointData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkFloatArray.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkAppendPolyData.h"
#include "vtkIdList.h"
#include "vtkCellData.h"

#include "itkCellInterface.h"

#include <iostream>
#include <time.h>

#ifndef M_PI
#define _USE_MATH_DEFINES
#endif



//----------------------------------------------------------------------------
// define:
//----------------------------------------------------------------------------
static int numberOfCycle = 0;

//------------------------------------------------------------------------------
mafCxxTypeMacro(medVMEStent);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
medVMEStent::medVMEStent()
  : m_StentStartPosId(0), 
  m_VesselVME(NULL), m_CenterLineVME(NULL),
  m_StentParamsModified(true), m_StentCenterLineModified(true),
  m_SimplexMeshModified(true), m_StentLengthModified(true),
  m_DeployedPolydataVME(NULL), m_VesselNodeID(-1), m_CenterLineNodeID(-1),
  m_DeployedPolydataNodeID(-1)
{
  m_StentSource = new vtkMAFStentModelSource ;

  m_StentPolyData = vtkPolyData::New() ;
  m_SimplexPolyData = vtkPolyData::New() ;

  m_CenterLine = vtkPolyData::New() ;
  m_CenterLineLong = vtkPolyData::New() ;
  m_StentCenterLine = vtkPolyData::New() ;
  m_VesselSurface = vtkPolyData::New() ;

  m_StrutPairsPerCrown = 16 ;
  m_Stent_Diameter = 2.0 ;
  m_Stent_DBDiameter = 5.0 ; // Saves original DB diameter for reporting, so not lost when crimped.
  m_Crown_Length = 2.2;
  m_NumberOfCrowns = 10;
  m_Strut_Angle = 60.0;
  m_Strut_Thickness = 0.0;
  m_Id_Stent_Configuration = 1; /* 1.outofphase, 0.InPhase;  enumStCfgType */
  m_Id_Link_Connection = 2; /* 0.peak2valley;enumLinkConType {peak2valley, valley2peak, peak2peak, valley2valley} */
  m_Link_Orientation = 0;
  m_Link_Length = 1.0;
  m_Link_Alignment = 0;
  m_ComputedCrownNumber = 0;

  m_StentStartPosId = 0 ;
  m_MaxRisk = 0;
  m_FatigueBending = 0.;

  m_CenterLineDefined = false ; 
  m_VesselSurfaceDefined = false ;
  m_CenterLineVMEDefined = false ; 
  m_VesselVMEDefined = false ;
  m_DeployedPolydataStatus = DEPLOYED_PD_NONE ;

  mafVMEOutputPolyline *output = mafVMEOutputPolyline::New(); // output with no data.  Deleted by Maf.
  SetOutput(output);

  mafDataPipeCustom *pipe = mafDataPipeCustom::New(); // Deleted by MAF
  pipe->SetInput(m_StentPolyData);
  SetDataPipe(pipe);

  // deformation filter
  // NB smart itk pointer - don't delete
  m_DeformFilter = DeformFilterType::New() ; 
}



//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------
medVMEStent::~medVMEStent()
{
  delete m_StentSource ;

  m_StentPolyData->Delete() ;
  m_SimplexPolyData->Delete() ;
  m_CenterLine->Delete() ;
  m_CenterLineLong->Delete() ;
  m_StentCenterLine->Delete() ;
  m_VesselSurface->Delete() ;

  SetOutput(NULL);
}



//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
mafVMEOutputPolyline *medVMEStent::GetPolylineOutput()
{
  return (mafVMEOutputPolyline *)GetOutput();
}




//-------------------------------------------------------------------------
// Internally used to create a new instance of the GUI.
//-------------------------------------------------------------------------
mafGUI* medVMEStent::CreateGui()
{
  mafVME::CreateGui();
  if(m_Gui)
  {
    m_Gui->Label("Stent");

    //wxString stentType[3] = {"normal", "ABBott","Bard"};
    //m_Gui->Combo(CHANGE_STENT_TYPE, "Type", &m_Id_Stent_Type, 3, stentType);


    m_Gui->Double(CHANGED_STENT_PARAM,_("Diameter"), &m_Stent_Diameter, 0, 10000,-1,_("The length of the stent (mm)"));
    m_Gui->Double(CHANGED_STENT_PARAM,_("Crown Len"), &m_Crown_Length, 0, 10000,-1,_("The length of the Crown (mm)"));
    m_Gui->Integer(CHANGED_STENT_PARAM,_("Crown num"), &m_NumberOfCrowns, 0, 10000,-1,_("The number of the Crowns"));
    //m_Gui->Double(CHANGED_STENT_PARAM,_("Angle"), &m_Strut_Angle,0,360,-1,_("strut angle (deg)"));
    //m_Gui->Double(CHANGED_STENT_PARAM,_("Thickness"), &m_Strut_Thickness,0,10,-1,_("strut thickness (mm)"));

    m_Gui->Divider(2);
    wxString stentConfiguration[2] = {"in phase", "out of phase"};
    m_Gui->Combo(CHANGED_STENT_PARAM, "Config.", &m_Id_Stent_Configuration, 2, stentConfiguration);

    m_Gui->Divider(2);   
    m_Gui->Label("Link");
    wxString linkConnection[4] = {"peak-to-valley", "valley-to-peak", "peak-to-peak", "valley-to-valley"};
    m_Gui->Combo(CHANGED_STENT_PARAM, "Connection", &m_Id_Link_Connection, 4, linkConnection);
    m_Gui->Double(CHANGED_STENT_PARAM,_("Length"), &m_Link_Length, 0, 10000,-1,_("Link length(longitudinal direction (mm))"));
    wxString linkAlignment[3] = {"+0", "+1","+2"};
    m_Gui->Combo(CHANGED_STENT_PARAM, "alignment", &m_Link_Alignment, 3, linkAlignment);

    m_Gui->Divider(2);
    wxString linkOrientation[3] = {"+0", "+1","-1"};
    m_Gui->Combo(CHANGED_STENT_PARAM, "Orientation", &m_Link_Orientation, 3, linkOrientation);	  

    m_Gui->Divider(2);

    m_Gui->Button(ID_CENTERLINE, _("center line"), _(""), _("Select the centerline for creating stent"));
    m_Gui->Button(ID_CONSTRAINT_SURFACE, _("vessel surface"), _(""), _("Select the vessel surface for deploying stent"));

    m_Gui->FitGui();
    m_Gui->Update();
  }
  m_Gui->Divider();
  return m_Gui;
}



//-------------------------------------------------------------------------
// Event handler
//-------------------------------------------------------------------------
void medVMEStent::OnEvent(mafEventBase *maf_event)
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      /*case CHANGED_STENT_TYPE:
      {  
      m_StentParamsModified = true ;
      m_StentLengthModified = true ;

      InternalUpdate();
      m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
      ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
      m_Gui->Update();
      }
      break;*/
    case CHANGED_STENT_PARAM:
      {  
        m_StentParamsModified = true ;
        m_StentLengthModified = true ;
        //SetFixedParameterForSpecialStent();
        InternalUpdate();
        m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
        ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
        m_Gui->Update();
      }
      break;
    case ID_CONSTRAINT_SURFACE:
      {
        mafNode *node = FindOrSelectVesselVME(this) ;
        m_VesselVME = mafVME::SafeDownCast(node) ;
        SetVesselSurface(node);

        InternalUpdate();
        m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
        ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
        m_Gui->Update();
      }
      break ;
    case ID_CENTERLINE:
      {
        mafNode *node = FindOrSelectCenterLineVME(this) ;
        m_VesselVME = mafVME::SafeDownCast(node) ;
        SetVesselCenterLine(node);

        InternalUpdate();
        m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
        ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
        m_Gui->Update();
      }	
      break;
    default:
      mafVME::OnEvent(maf_event);
      break ;
    }//end of switch
  }//end of if
  else{
    Superclass::OnEvent(maf_event);
  }
}



//-----------------------------------------------------------------------
// Initialize.
// This method allows an external op to initialize the vme, 
// the vessel and the centerline.
//-----------------------------------------------------------------------
void medVMEStent::Initialize()
{
  mafNode *node ;

  node = FindOrSelectVesselVME(this) ;
  assert(node != NULL) ;
  m_VesselVME = mafVME::SafeDownCast(node) ;
  SetVesselSurface(node);

  node = FindOrSelectCenterLineVME(this) ;
  assert(node != NULL) ;
  m_CenterLineVME = mafVME::SafeDownCast(node) ;
  SetVesselCenterLine(node);

  InternalUpdate();
  m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
  ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
  m_Gui->Update();
}




//-------------------------------------------------------------------------
// Deep Copy
//-------------------------------------------------------------------------
int medVMEStent::DeepCopy(mafNode *a)
{ 
  // NB: this does not do a base class copy -
  // it is unnecessary and causes conflict.

  medVMEStent *vmeStent = medVMEStent::SafeDownCast(a);

  // parameters
  m_CompanyName = vmeStent->m_CompanyName ;
  m_ModelName = vmeStent->m_ModelName ;
  m_Material = vmeStent->m_Material ;
  m_DeliverySystem = vmeStent->m_DeliverySystem ;
  m_Stent_Type = vmeStent->m_Stent_Type ;
  m_Stent_Diameter = vmeStent->m_Stent_Diameter ;
  m_Stent_DBDiameter = vmeStent->m_Stent_DBDiameter ;
  m_Crown_Length = vmeStent->m_Crown_Length ;
  m_Strut_Length = vmeStent->m_Strut_Length ;
  m_Strut_Angle = vmeStent->m_Strut_Angle ;
  m_Link_Length = vmeStent->m_Link_Length;
  m_StrutPairsPerCrown = vmeStent->m_StrutPairsPerCrown ;
  m_NumberOfCrowns = vmeStent->m_NumberOfCrowns ;
  m_Link_Number = vmeStent->m_Link_Number ;
  m_Id_Link_Connection = vmeStent->m_Id_Link_Connection;
  m_Id_Stent_Configuration = vmeStent->m_Id_Stent_Configuration ;
  m_Link_Orientation = vmeStent->m_Link_Orientation;
  m_Link_Alignment = vmeStent->m_Link_Alignment;
  m_Strut_Thickness = vmeStent->m_Strut_Thickness ;
  m_Stent_DBLength = vmeStent->m_Stent_DBLength ;
  m_ComputedCrownNumber = vmeStent->m_ComputedCrownNumber ;
  m_StentParamsModified = true ;

  // vessel
  m_VesselVME = vmeStent->m_VesselVME ;
  SetVesselSurface(vmeStent->m_VesselVME) ;

  // vessel centerline
  m_StentStartPosId = vmeStent->m_StentStartPosId ;
  m_CenterLineVME = vmeStent->m_CenterLineVME ;
  SetVesselCenterLine(vmeStent->m_CenterLineVME) ;

  // stent
  m_StentPolyData->DeepCopy(vmeStent->m_StentPolyData) ;
  m_StentCenterLine->DeepCopy(vmeStent->m_StentCenterLine);
  m_StentCenterLineModified = true ;
  m_StentLength = vmeStent->m_StentLength ;
  m_StentLengthModified = vmeStent->m_StentLengthModified ;

  // simplex
  m_SimplexPolyData->DeepCopy(vmeStent->m_SimplexPolyData) ;

  // deformation filter
  m_SimplexMeshModified = true ;

  // deployed stent polydata
  m_DeployedPolydataVME = vmeStent->m_DeployedPolydataVME ;
  m_DeployedPolydataNodeID = vmeStent->m_DeployedPolydataNodeID ;
  m_DeployedPolydataStatus = vmeStent->m_DeployedPolydataStatus ;


  InternalUpdate();

  // Update resets stent, so copy polydata position again
  m_StentPolyData->DeepCopy(vmeStent->m_StentPolyData) ;

  return MAF_OK;
}




//-----------------------------------------------------------------------
// Internal store - called when saving to msf
//-----------------------------------------------------------------------
int medVMEStent::InternalStore(mafStorageElement *node)
{  
  if (Superclass::InternalStore(node)==MAF_OK)
  {
    // parameters
    if (node->StoreText("ModelName",m_ModelName) != MAF_OK) return MAF_ERROR;
    if (node->StoreText("CompanyName",m_CompanyName) != MAF_OK) return MAF_ERROR;
    if (node->StoreText("Material",m_Material) != MAF_OK) return MAF_ERROR;
    if (node->StoreDouble("DeliverySystem",m_DeliverySystem) != MAF_OK) return MAF_ERROR; 
    if (node->StoreInteger("StentType",m_Stent_Type) != MAF_OK) return MAF_ERROR; 
    if (node->StoreDouble("Diameter",m_Stent_Diameter) != MAF_OK) return MAF_ERROR; 
    if (node->StoreDouble("DBDiameter",m_Stent_DBDiameter) != MAF_OK) return MAF_ERROR;
    if (node->StoreDouble("CrownLength",m_Crown_Length) != MAF_OK) return MAF_ERROR; 
    if (node->StoreDouble("StrutLength",m_Strut_Length) != MAF_OK) return MAF_ERROR; 
    if (node->StoreDouble("StrutAngle",m_Strut_Angle) != MAF_OK) return MAF_ERROR; 
    if (node->StoreDouble("LinkLength",m_Link_Length) != MAF_OK) return MAF_ERROR; 
    if (node->StoreInteger("NumberOfStruts",m_StrutPairsPerCrown) != MAF_OK) return MAF_ERROR; 
    if (node->StoreInteger("NumberOfCrowns",m_NumberOfCrowns) != MAF_OK) return MAF_ERROR; 
    if (node->StoreInteger("NumberOfLinks",m_Link_Number) != MAF_OK) return MAF_ERROR; 
    if (node->StoreInteger("IdLinkConnection",m_Id_Link_Connection) != MAF_OK) return MAF_ERROR; 
    if (node->StoreInteger("IdStentConfig",m_Id_Stent_Configuration) != MAF_OK) return MAF_ERROR; 
    if (node->StoreInteger("LinkOrientation",m_Link_Orientation) != MAF_OK) return MAF_ERROR; 
    if (node->StoreInteger("LinkAlignment",m_Link_Alignment) != MAF_OK) return MAF_ERROR; 
    if (node->StoreDouble("StrutThickness",m_Strut_Thickness) != MAF_OK) return MAF_ERROR;  
    if (node->StoreDouble("DLength",m_Stent_DBLength) != MAF_OK) return MAF_ERROR; 
    if (node->StoreInteger("ComputedNumberOfCrowns",m_ComputedCrownNumber) != MAF_OK) return MAF_ERROR; 
	//
	if (node->StoreInteger("MaxRisk",m_MaxRisk) != MAF_OK) return MAF_ERROR;
	if (node->StoreDouble("FatigueBending",m_FatigueBending) != MAF_OK) return MAF_ERROR;
	if (node->StoreVectorN("TubeInitialStretching",m_TubeInitialStretching,m_TubeInitialStretching.size()) != MAF_OK) return MAF_ERROR;
	if (node->StoreVectorN("TubeFinalStretching",m_TubeFinalStretching,m_TubeFinalStretching.size()) != MAF_OK) return MAF_ERROR;
	if (node->StoreVectorN("TubeYoungModulus",m_TubeYoungModulus,m_TubeYoungModulus.size()) != MAF_OK) return MAF_ERROR;
	if (node->StoreVectorN("TubeDiameter",m_TubeDiameter,m_TubeDiameter.size()) != MAF_OK) return MAF_ERROR;

    // vessel
    if (node->StoreInteger("VesselNodeId", m_VesselNodeID) != MAF_OK) return MAF_ERROR; 

    // center line
    if (node->StoreInteger("CenterLineNodeId", m_CenterLineNodeID) != MAF_OK) return MAF_ERROR; 
    if (node->StoreInteger("StartPos", m_StentStartPosId) != MAF_OK) return MAF_ERROR; 

    // stent
    if (node->StoreInteger("StentLength", m_StentLength) != MAF_OK) return MAF_ERROR; 

    // deployed stent polydata
    if (node->StoreInteger("DeployedPDStatus", m_DeployedPolydataStatus) != MAF_OK) return MAF_ERROR; 
    if (node->StoreInteger("DeployedPDNodeId", m_DeployedPolydataNodeID) != MAF_OK) return MAF_ERROR; 

    return MAF_OK;
  }
  return MAF_ERROR;
}



//-----------------------------------------------------------------------
// Internal restore - called when restoring from msf
//-----------------------------------------------------------------------
int medVMEStent::InternalRestore(mafStorageElement *node)
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {	    
    // parameters
    mafString material;
    mafString modelname;
    mafString companyname;
    if (node->RestoreText("ModelName",modelname) != MAF_OK) return MAF_ERROR;
    m_ModelName = modelname;
    if (node->RestoreText("CompanyName",companyname) != MAF_OK) return MAF_ERROR;
    m_CompanyName = companyname;
    if (node->RestoreText("Material",material) != MAF_OK) return MAF_ERROR;
    m_Material = material;
    if (node->RestoreDouble("DeliverySystem",m_DeliverySystem) != MAF_OK) return MAF_ERROR; 
    if (node->RestoreInteger("StentType",m_Stent_Type) != MAF_OK) return MAF_ERROR; 
    if (node->RestoreDouble("Diameter",m_Stent_Diameter) != MAF_OK) return MAF_ERROR; 
    if (node->RestoreDouble("DBDiameter",m_Stent_DBDiameter) != MAF_OK) return MAF_ERROR;
    if (node->RestoreDouble("CrownLength",m_Crown_Length) != MAF_OK) return MAF_ERROR; 
    if (node->RestoreDouble("StrutLength",m_Strut_Length) != MAF_OK) return MAF_ERROR; 
    if (node->RestoreDouble("StrutAngle",m_Strut_Angle) != MAF_OK) return MAF_ERROR; 
    if (node->RestoreDouble("LinkLength",m_Link_Length) != MAF_OK) return MAF_ERROR; 
    if (node->RestoreInteger("NumberOfStruts",m_StrutPairsPerCrown) != MAF_OK) return MAF_ERROR; 
    if (node->RestoreInteger("NumberOfCrowns",m_NumberOfCrowns) != MAF_OK) return MAF_ERROR; 
    if (node->RestoreInteger("NumberOfLinks",m_Link_Number) != MAF_OK) return MAF_ERROR; 
    if (node->RestoreInteger("IdLinkConnection",m_Id_Link_Connection) != MAF_OK) return MAF_ERROR; 
    if (node->RestoreInteger("IdStentConfig",m_Id_Stent_Configuration) != MAF_OK) return MAF_ERROR; 
    if (node->RestoreInteger("LinkOrientation",m_Link_Orientation) != MAF_OK) return MAF_ERROR; 
    if (node->RestoreInteger("LinkAlignment",m_Link_Alignment) != MAF_OK) return MAF_ERROR; 
    if (node->RestoreDouble("StrutThickness",m_Strut_Thickness) != MAF_OK) return MAF_ERROR;  
    if (node->RestoreDouble("DLength",m_Stent_DBLength) != MAF_OK) return MAF_ERROR; 
    if (node->RestoreInteger("ComputedNumberOfCrowns",m_ComputedCrownNumber) != MAF_OK) return MAF_ERROR; 
    m_StentParamsModified = true ;


    // vessel
    if (node->RestoreInteger("VesselNodeId", m_VesselNodeID) != MAF_OK) return MAF_ERROR; 

    // vessel centerline
    if (node->RestoreInteger("StartPos", m_StentStartPosId) != MAF_OK) return MAF_ERROR; 
    if (node->RestoreInteger("CenterLineNodeId", m_CenterLineNodeID) != MAF_OK) return MAF_ERROR; 

    // stent
    if (node->RestoreInteger("StentLength", m_StentLength) != MAF_OK) return MAF_ERROR; 
    m_StentCenterLineModified = true ;
    m_StentLengthModified = true ;

    // deformation filter
    m_SimplexMeshModified = true ;

    // deployed stent polydata
    if (node->RestoreInteger("DeployedPDStatus", m_DeployedPolydataStatus) != MAF_OK) return MAF_ERROR; 
    if (node->RestoreInteger("DeployedPDNodeId", m_DeployedPolydataNodeID) != MAF_OK) return MAF_ERROR; 
    if (m_DeployedPolydataStatus == DEPLOYED_PD_OK)
      m_DeployedPolydataStatus = DEPLOYED_PD_NOT_LOADED ;

	//
	if(m_DeployedPolydataStatus == DEPLOYED_PD_NOT_LOADED)
	{
		m_TubeInitialStretching.resize(GetNumberOfUnits());
		m_TubeFinalStretching.resize(GetNumberOfUnits());
		m_TubeYoungModulus.resize(GetNumberOfUnits());
		m_TubeDiameter.resize(GetNumberOfUnits());
		if (node->RestoreInteger("MaxRisk",m_MaxRisk) != MAF_OK) return MAF_ERROR;
		if (node->RestoreDouble("FatigueBending",m_FatigueBending) != MAF_OK) return MAF_ERROR;
		if (node->RestoreVectorN("TubeInitialStretching",m_TubeInitialStretching,GetNumberOfUnits()) != MAF_OK) return MAF_ERROR;
		if (node->RestoreVectorN("TubeFinalStretching",m_TubeFinalStretching,GetNumberOfUnits()) != MAF_OK) return MAF_ERROR;
		if (node->RestoreVectorN("TubeYoungModulus",m_TubeYoungModulus,GetNumberOfUnits()) != MAF_OK) return MAF_ERROR;
		if (node->RestoreVectorN("TubeDiameter",m_TubeDiameter,GetNumberOfUnits()) != MAF_OK) return MAF_ERROR;
	}


    // NB The vme tree is not accessible from here, so the rest of the 
    // initialization with the vessel and center line vme's is 
    // completed in InternalUpdate().

    return MAF_OK;
  }
  return MAF_ERROR;
}


//-----------------------------------------------------------------------
// update stent after a change in the parameters.
//-----------------------------------------------------------------------
void medVMEStent::InternalUpdate()
{
  if (!m_VesselVMEDefined || !m_VesselSurfaceDefined){
    mafNode* node = FindOrSelectVesselVME(this) ;
    assert(node != NULL) ;
    m_VesselVME = mafVME::SafeDownCast(node) ;
    SetVesselSurface(node);
  }

  if (!m_CenterLineVMEDefined || !m_CenterLineDefined){
    mafNode* node = FindOrSelectCenterLineVME(this) ;
    assert(node != NULL) ;
    m_CenterLineVME = mafVME::SafeDownCast(node) ;
    SetVesselCenterLine(node);
  }

  if(m_StentParamsModified || m_StentCenterLineModified){	
    m_StentSource->setStentDiameter(m_Stent_Diameter);
    m_StentSource->setStentDLength(m_Stent_DBLength);//weih14 @todo
    m_StentSource->setCrownLength(m_Crown_Length);
    //m_StentSource->setCrownNumber(m_NumberOfCrowns);

    m_StentSource->setStentConfiguration((enumStCfgType)m_Id_Stent_Configuration);
    m_StentSource->setLinkConnection((enumLinkConType) m_Id_Link_Connection);
    m_StentSource->setLinkOrientation( (enumLinkOrtType)m_Link_Orientation);
    m_StentSource->setLinkNumber(m_Link_Number); // Hui suggestion to fix Maris link positions 7/2/14

    m_StentSource->setLinkLength(m_Link_Length);
    m_StentSource->setLinkAlignment(m_Link_Alignment);
    m_StentSource->setStrutsNumber(m_StrutPairsPerCrown);
    m_StentSource->setStentType(m_Stent_Type);//weih14 add

    if (m_Strut_Angle > 0.0)
      m_StentSource->setStrutAngle(m_Strut_Angle) ; // set angle and calc strut length

    int linePointNumber = m_CenterLine->GetNumberOfPoints();

    if(m_CenterLine != NULL && linePointNumber>0){
      m_StentSource->setCenterLineFromPolyData(m_StentCenterLine);
      //m_ComputedCrownNumber = m_StentSource->computeCrownNumberAfterSetCenterLine();
      //if (m_NumberOfCrowns>m_ComputedCrownNumber)
      //{
      //  m_NumberOfCrowns = m_ComputedCrownNumber;
      //}
    }
    //m_StentSource->setCrownNumber(m_NumberOfCrowns);

    //---------weih modify---------
    //m_StentSource->createStent();
    if(m_StentSource->getInphaseShort()==1){
      m_StentSource->createStentInphaseShort();
    }
    else if(m_StentSource->getStentType()==2){
      m_StentSource->createStentBardHelical();
    }
    else{
      m_StentSource->createStent();
    }
    m_NumberOfCrowns = m_StentSource->getCrownNumber();

    m_SimplexMesh = m_StentSource->GetSimplexMesh();
    m_SimplexMesh->DisconnectPipeline();

    m_Strut_Length = m_StentSource->getStrutLength();
    m_Link_Length = m_StentSource->getLinkLength();

    m_StentParamsModified = false ;
    m_StentCenterLineModified = false ;
    m_SimplexMeshModified = true ;

    PartialInitDefFilterFromStentModel() ;

    this->Modified() ;
  }

  if (m_SimplexMeshModified){
    m_StentPolyData->Initialize() ;
    m_SimplexPolyData->Initialize() ;

    UpdateStentPolydataFromSimplex() ; 
    UpdateStentPolydataFromSimplex_ViewAsSimplex() ; 
    m_SimplexMeshModified = false ;
    this->Modified() ;
  }


  if (m_DeployedPolydataStatus != DEPLOYED_PD_NONE){
    mafNode* node = FindNodeWithId(m_DeployedPolydataNodeID) ;
    assert(node != NULL) ;
    m_DeployedPolydataVME = mafVMEPolyline::SafeDownCast(node) ;
    this->SetDeployedPolydataVME(node) ;
    this->Modified() ;
  }
}



//------------------------------------------------------------------------------
// Update stent polydata from simplex 
//------------------------------------------------------------------------------
void medVMEStent::UpdateStentPolydataFromSimplex()
{
  if (m_SimplexMeshModified){
    if(m_StentSource->getInphaseShort()==1)
      UpdateStentPolydataFromSimplex_Abbott() ;
    else
      UpdateStentPolydataFromSimplex_Simple() ;
  }

  // NB Don't set m_SimplexMeshModified = false here - wait until simplex version is called.
}



/*
//------------------------------------------------------------------------------
// Update stent polydata from simplex
//------------------------------------------------------------------------------
void medVMEStent::UpdateStentPolydataFromSimplex_old()
{
  if (m_SimplexMeshModified){
    //----------------------------------------
    // copy the simplex vertices to vtkPoints
    //----------------------------------------
    vtkPoints* vpoints = vtkPoints::New();
    vpoints->SetNumberOfPoints(40000);		

    SimplexMeshType::PointsContainer::Pointer sPoints;
    sPoints = m_SimplexMesh->GetPoints();
    int pointCount =0;
    for(SimplexMeshType::PointsContainer::Iterator pointIndex = sPoints->Begin(); pointIndex != sPoints->End(); ++pointIndex)
    {
      int idx = pointIndex->Index();
      vtkFloatingPointType * pp = pointIndex->Value().GetDataPointer();
      vpoints->SetPoint(idx,pp);
      pointCount++;
    }
    vpoints->Squeeze() ;
    m_StentPolyData->SetPoints(vpoints) ;
    vpoints->Delete() ;



    int tindices[2];
    vtkCellArray *lines = vtkCellArray::New() ;
    lines->Allocate(40000) ;  

    for(StrutIterator iter = m_StentSource->GetStrutsList().begin(); iter !=m_StentSource->GetStrutsList().end(); iter++){//iter++){
      if(m_StentSource->getInphaseShort()==1){
        // Abbott stent
        int tindices2[4], tindices3[2] ;
        double endPoints[4][3], midPoints[4][3], strutLinkPoints[2][3];

        tindices2[0]=iter->startVertex;
        tindices2[1]=iter->endVertex;
        vpoints->GetPoint(tindices2[0], endPoints[0]) ;
        vpoints->GetPoint(tindices2[1], endPoints[1]) ;
        iter++;

        tindices2[2] = iter->startVertex;
        tindices2[3] = iter->endVertex;
        vpoints->GetPoint(tindices2[2], endPoints[2]) ;
        vpoints->GetPoint(tindices2[3], endPoints[3]) ;

        //------points tindices2[1] and tindices2[3] should be the  same points
        //if(tindices2[1] is link points then calculate strutLink points)

        // Do the cells for the links
        bool isLinkPoint = false;
        for(StrutIterator iter2 = m_StentSource->GetLinksList().begin(); iter2 !=m_StentSource->GetLinksList().end(); iter2++){
          tindices3[0] = iter2->startVertex;
          tindices3[1] = iter2->endVertex;
          if(tindices3[1] == tindices2[1] ){// end data of link should be the same as end of strut
            isLinkPoint = true;
            break;
          }
          //lines->InsertNextCell(2, tindices);
        }
        CalculateMidPointsFromPairOfStruts(endPoints, midPoints,isLinkPoint,strutLinkPoints) ;

        // add the 4 new midpoints
        int newPtIds[6] ;
        for(int i=0;i<6;i++){
          if(i<4){//first 4 points for mid points, last two for strutlink points
            newPtIds[i] = vpoints->InsertNextPoint(midPoints[i]);
          }else{
            if(isLinkPoint){
              newPtIds[i] = vpoints->InsertNextPoint(strutLinkPoints[i-4]);// index is 0 or 1
            }
          }
          pointCount++;
        }

        //check if this strut has a link

        //----------left arm----------
        int tindicesShort[2] ;
        tindicesShort[0]=tindices2[0];
        tindicesShort[1]=newPtIds[1] ;
        // tindicesShort[1]=newPtIds[0] ;
        lines->InsertNextCell(2,tindicesShort);

        tindicesShort[0]=newPtIds[0];              //\_
        tindicesShort[1]=newPtIds[1];
        lines->InsertNextCell(2,tindicesShort);

        if(isLinkPoint){                                  //  \_
          tindicesShort[0]=newPtIds[0];	               //   \_|
          tindicesShort[1]=newPtIds[5];
          lines->InsertNextCell(2,tindicesShort);

          tindicesShort[0]=newPtIds[5];	              //_    
          tindicesShort[1]=tindices2[1];
          lines->InsertNextCell(2,tindicesShort);

        }else{
          tindicesShort[0]=newPtIds[0];              //\_    //tindicesShort[0]=newPtIds[1];  
          tindicesShort[1]=tindices2[1];             //  \

          lines->InsertNextCell(2,tindicesShort);
        }
        //-----------right arm---------------
        tindicesShort[0]=tindices2[2];
        tindicesShort[1]=newPtIds[3] ;
        //tindicesShort[1]=newPtIds[2] ;
        lines->InsertNextCell(2,tindicesShort);

        tindicesShort[0]=newPtIds[2];
        tindicesShort[1]=newPtIds[3];
        lines->InsertNextCell(2,tindicesShort);


        if(isLinkPoint){    
          tindicesShort[0]=newPtIds[2];	               //   \_|
          tindicesShort[1]=newPtIds[4];
          lines->InsertNextCell(2,tindicesShort);

          tindicesShort[0]=newPtIds[4];	              //_    
          tindicesShort[1]=tindices2[1];
          lines->InsertNextCell(2,tindicesShort);

        }else{
          tindicesShort[0]=newPtIds[2];              //\_    //tindicesShort[0]=newPtIds[1];  
          tindicesShort[1]=tindices2[1];             //  \

          lines->InsertNextCell(2,tindicesShort);
        }

        //----------for strut link points
        if(isLinkPoint){
          tindicesShort[0]=newPtIds[4];//--
          tindicesShort[1]=newPtIds[5];
          lines->InsertNextCell(2,tindicesShort);
        }
      }//endof if inphaseShort
      else{
        // Simple stent with straight struts
        tindices[0] = iter->startVertex;
        tindices[1] = iter->endVertex;
        lines->InsertNextCell(2, tindices);
        iter++;
        tindices[0] = iter->startVertex;
        tindices[1] = iter->endVertex;
        lines->InsertNextCell(2,tindices);
      }
    }


    // Do the cells for the links
    for(StrutIterator iter = m_StentSource->GetLinksList().begin(); iter !=m_StentSource->GetLinksList().end(); iter++){
      tindices[0] = iter->startVertex;
      tindices[1] = iter->endVertex;

      //vpoints->GetPoint(tindices[0] , aVetex);//for test
      //vpoints->GetPoint(tindices[1] , bVetex);
      //if (aVetex[0]>m_Crown_Length*5 && aVetex[0]<m_Crown_Length*8 &&bVetex[0]>m_Crown_Length*5 && bVetex[0]<m_Crown_Length*8){//one middle ring
      //lines->InsertNextCell(2,tindices);
      //}

      lines->InsertNextCell(2, tindices);
    }
    lines->Squeeze() ;
    m_StentPolyData->SetLines(lines);
    lines->Delete() ;

    m_StentPolyData->Modified();
    //m_SimplexMeshModified = false ; // wait until simplex version is called before false
  }
}
*/


//------------------------------------------------------------------------------
// Update stent polydata from simplex
//------------------------------------------------------------------------------
void medVMEStent::UpdateStentPolydataFromSimplex_Simple()
{
  if (m_SimplexMeshModified){
    //----------------------------------------
    // copy the simplex vertices to vtkPoints
    //----------------------------------------
    vtkPoints* vpoints = vtkPoints::New();
    vpoints->SetNumberOfPoints(40000);		

    SimplexMeshType::PointsContainer::Pointer sPoints;
    sPoints = m_SimplexMesh->GetPoints();
    int pointCount =0;
    for(SimplexMeshType::PointsContainer::Iterator pointIndex = sPoints->Begin(); pointIndex != sPoints->End(); ++pointIndex){
      int idx = pointIndex->Index();
      vtkFloatingPointType * pp = pointIndex->Value().GetDataPointer();
      vpoints->SetPoint(idx,pp);
      pointCount++;
    }

    vpoints->Squeeze() ;


    int tindices[2];
    vtkCellArray *lines = vtkCellArray::New() ;
    lines->Allocate(40000) ;  

    // strut cells (in pairs)
    for(StrutIterator iter = m_StentSource->GetStrutsList().begin(); iter !=m_StentSource->GetStrutsList().end(); iter++){
      // Simple stent with straight struts
      tindices[0] = iter->startVertex;
      tindices[1] = iter->endVertex;
      lines->InsertNextCell(2, tindices);

      iter++;
      tindices[0] = iter->startVertex;
      tindices[1] = iter->endVertex;
      lines->InsertNextCell(2,tindices);
    }

    // link cells
    for(StrutIterator iter = m_StentSource->GetLinksList().begin(); iter !=m_StentSource->GetLinksList().end(); iter++){
      tindices[0] = iter->startVertex;
      tindices[1] = iter->endVertex;
      lines->InsertNextCell(2, tindices);
    }


    //----------------------------------------
    // Add points and cells to polydata
    //----------------------------------------
    m_StentPolyData->SetPoints(vpoints) ;
    vpoints->Delete() ;

    lines->Squeeze() ;
    m_StentPolyData->SetLines(lines);
    lines->Delete() ;


    //----------------------------------------
    // Add rank scalar to polydata
    //----------------------------------------
    int ncells = m_StentPolyData->GetNumberOfCells() ;
    vtkIntArray* rankScalar = vtkIntArray::New() ;
    rankScalar->SetName("Rank") ;
    rankScalar->SetNumberOfComponents(1) ;
    rankScalar->SetNumberOfTuples(ncells) ;

    int numStruts = m_NumberOfCrowns * 2*m_StrutPairsPerCrown ;
    int rank = 0 ;
    int cellIdStruts = 0 ;
    int cellIdLinks = numStruts ;
    for (int i = 0 ;  i < m_NumberOfCrowns ;  i++){
      for (int j = 0 ;  j < 2*m_StrutPairsPerCrown && cellIdStruts < ncells ;  j++){
        rankScalar->SetTuple1(cellIdStruts, rank) ;
        cellIdStruts++ ;
        rank++ ;
      }

      if (i == m_NumberOfCrowns-1)
        continue ; // no links after last crown

      for (int j = 0 ;  j < m_Link_Number && cellIdLinks < ncells ;  j++){
        rankScalar->SetTuple1(cellIdLinks, rank) ;
        cellIdLinks++ ;
        rank++ ;
      }
    }

    m_StentPolyData->GetCellData()->AddArray(rankScalar) ; 
    rankScalar->Delete() ;
    m_StentPolyData->Modified();
  }
}




//------------------------------------------------------------------------------
// Update stent polydata from simplex
//------------------------------------------------------------------------------
void medVMEStent::UpdateStentPolydataFromSimplex_Abbott()
{
  if (m_SimplexMeshModified){
    //----------------------------------------
    // copy the simplex vertices to vtkPoints
    //----------------------------------------
    m_SimplexMesh->Update() ;

    vtkPoints* vpoints = vtkPoints::New();
    vpoints->SetNumberOfPoints(40000);		

    SimplexMeshType::PointsContainer::Pointer sPoints;
    sPoints = m_SimplexMesh->GetPoints();
    int pointCount =0;
    for(SimplexMeshType::PointsContainer::Iterator pointIndex = sPoints->Begin(); pointIndex != sPoints->End(); ++pointIndex){
      int idx = pointIndex->Index();
      vtkFloatingPointType * pp = pointIndex->Value().GetDataPointer();
      vpoints->SetPoint(idx,pp);
      pointCount++;
    }

    vpoints->Squeeze() ;

    //----------------------------------------
    // Strut cells
    //----------------------------------------
    vtkCellArray *lines = vtkCellArray::New() ;
    lines->Allocate(40000) ;  

    for(StrutIterator iter = m_StentSource->GetStrutsList().begin(); iter !=m_StentSource->GetStrutsList().end(); iter++){
      // Get points corresponding to pair of struts
      int tindices_struts[4], tindices_link[2] ;
      double endPoints[4][3], midPoints[4][3], strutLinkPoints[2][3];

      tindices_struts[0]=iter->startVertex;
      tindices_struts[1]=iter->endVertex;
      vpoints->GetPoint(tindices_struts[0], endPoints[0]) ;
      vpoints->GetPoint(tindices_struts[1], endPoints[1]) ;
      iter++;

      tindices_struts[2] = iter->startVertex;
      tindices_struts[3] = iter->endVertex;
      vpoints->GetPoint(tindices_struts[2], endPoints[2]) ;
      vpoints->GetPoint(tindices_struts[3], endPoints[3]) ;

      // Does this strut pair have a link ?
      // Points tindices_struts[1] and tindices_struts[3] should be the  same points
      // If tindices2[1] is link points then calculate strutLink points
      bool isLinkPoint = false;
      for(StrutIterator iter2 = m_StentSource->GetLinksList().begin(); iter2 !=m_StentSource->GetLinksList().end(); iter2++){
        tindices_link[0] = iter2->startVertex;
        tindices_link[1] = iter2->endVertex;
        if (tindices_link[1] == tindices_struts[1]){ // end data of link should be the same as end of strut
          isLinkPoint = true;
          break;
        }
      }

      CalculateMidPointsFromPairOfStruts(endPoints, midPoints, isLinkPoint, strutLinkPoints) ;

      // add the 4 new midpoints
      int newPtIds[6] ;
      newPtIds[0] = vpoints->InsertNextPoint(midPoints[0]);
      newPtIds[1] = vpoints->InsertNextPoint(midPoints[1]);
      newPtIds[2] = vpoints->InsertNextPoint(midPoints[2]);
      newPtIds[3] = vpoints->InsertNextPoint(midPoints[3]);
      pointCount += 4 ;
      if (isLinkPoint){
        // add squared ends if link point
        newPtIds[4] = vpoints->InsertNextPoint(strutLinkPoints[0]);
        newPtIds[5] = vpoints->InsertNextPoint(strutLinkPoints[1]);
        pointCount += 2 ;
      }

      // Construct and insert pair of strut cells
      if (isLinkPoint){
        int t[5] ;
        t[0] = tindices_struts[0] ;
        t[1] = newPtIds[1] ;
        t[2] = newPtIds[0] ;
        t[3] = newPtIds[5] ;
        t[4] = tindices_struts[1] ;
        lines->InsertNextCell(5, t);

        t[0] = tindices_struts[2] ;
        t[1] = newPtIds[3] ;
        t[2] = newPtIds[2] ;
        t[3] = newPtIds[4] ;
        t[4] = tindices_struts[3] ;
        lines->InsertNextCell(5, t);
      }
      else{
        int t[4] ;
        t[0] = tindices_struts[0] ;
        t[1] = newPtIds[1] ;
        t[2] = newPtIds[0] ;
        t[3] = tindices_struts[1] ;
        lines->InsertNextCell(4, t);

        t[0] = tindices_struts[2] ;
        t[1] = newPtIds[3] ;
        t[2] = newPtIds[2] ;
        t[3] = tindices_struts[3] ;
        lines->InsertNextCell(4, t);
      }
    } // for struts


    //----------------------------------------
    // Link cells
    //----------------------------------------
    for(StrutIterator iter = m_StentSource->GetLinksList().begin(); iter !=m_StentSource->GetLinksList().end(); iter++){
      int tindices[2];
      tindices[0] = iter->startVertex;
      tindices[1] = iter->endVertex;
      lines->InsertNextCell(2, tindices);
    }


    //----------------------------------------
    // Add points and cells to polydata
    //----------------------------------------
    m_StentPolyData->SetPoints(vpoints) ;
    vpoints->Delete() ;

    lines->Squeeze() ;
    m_StentPolyData->SetLines(lines);
    lines->Delete() ;


    //----------------------------------------
    // Add rank scalar to polydata
    //----------------------------------------
    int ncells = m_StentPolyData->GetNumberOfCells() ;
    vtkIntArray* rankScalar = vtkIntArray::New() ;
    rankScalar->SetName("Rank") ;
    rankScalar->SetNumberOfComponents(1) ;
    rankScalar->SetNumberOfTuples(ncells) ;

    int numStruts = m_NumberOfCrowns * 2*m_StrutPairsPerCrown ;
    int rank = 0 ;
    int cellIdStruts = 0 ;
    int cellIdLinks = numStruts ;
    for (int i = 0 ;  i < m_NumberOfCrowns ;  i++){
      for (int j = 0 ;  j < 2*m_StrutPairsPerCrown && cellIdStruts < ncells ;  j++){
        rankScalar->SetTuple1(cellIdStruts, rank) ;
        cellIdStruts++ ;
        rank++ ;
      }

      if (i == 0)
        continue ; // no links between crowns 0 and 1

      for (int j = 0 ;  j < m_Link_Number && cellIdLinks < ncells ;  j++){
        rankScalar->SetTuple1(cellIdLinks, rank) ;
        cellIdLinks++ ;
        rank++ ;
      }
    }

    m_StentPolyData->GetCellData()->AddArray(rankScalar) ;
    rankScalar->Delete() ;

    m_StentPolyData->Modified();
  }
}




//------------------------------------------------------------------------------
// Calculate mid-points of pair of struts
//------------------------------------------------------------------------------
void medVMEStent::CalculateMidPointsFromPairOfStruts(const double strutEndPts[4][3], double midPts[4][3], bool isLinkPoint, double strutLinkPts[2][3]) const
{
  const double LengthFactor = 0.2 ;
  const double LengthLinkFactor = 0.2;

  vtkMAFMatrixVectorMath *math = vtkMAFMatrixVectorMath::New();
  math->SetHomogeneous(false) ;

  // calc exact midpoints m[2] of struts
  double m[2][3] ; //compute two mid points
  math->MeanVector(strutEndPts[0], strutEndPts[1], m[0]) ;
  math->MeanVector(strutEndPts[2], strutEndPts[3], m[1]) ;

  double dv[2][3];  
  //                     0 \     / 2
  // midpts[0],[1]    m[0]  \   / m[1]  midpts[2][3]  
  //                       1 \|/ 3
  math->SubtractVectors(m[1], m[0], dv[0]) ;
  math->SubtractVectors(m[0], m[1], dv[1]) ;

  math->NormalizeVector(dv[0]); 
  math->NormalizeVector(dv[1]); 

  math->CopyVector(m[0],midPts[0]);
  math->CopyVector(m[1],midPts[2]);

  math->AddMultipleOfVector(m[0],LengthFactor,dv[1],midPts[1]); 
  math->AddMultipleOfVector(m[1],LengthFactor,dv[0],midPts[3]);

  if(isLinkPoint){
    math->AddMultipleOfVector(strutEndPts[1],LengthLinkFactor,dv[0],strutLinkPts[0]);
    math->AddMultipleOfVector(strutEndPts[1],LengthLinkFactor,dv[1],strutLinkPts[1]);
  }
}



//------------------------------------------------------------------------------
// Copy simplex directly to polydata
//------------------------------------------------------------------------------
void medVMEStent::UpdateStentPolydataFromSimplex_ViewAsSimplex()
{
  if (m_SimplexMeshModified){
    vtkPoints* vpoints = vtkPoints::New();
    vpoints->SetNumberOfPoints(40000);		

    SimplexMeshType::PointsContainer::Pointer sPoints;
    sPoints = m_SimplexMesh->GetPoints();
    for(SimplexMeshType::PointsContainer::Iterator pointIndex = sPoints->Begin(); pointIndex != sPoints->End(); pointIndex++)
    {
      int idx = pointIndex->Index();
      vtkFloatingPointType * pp = pointIndex->Value().GetDataPointer();
      vpoints->SetPoint(idx,pp);
    }
    vpoints->Squeeze() ;
    m_SimplexPolyData->SetPoints(vpoints) ;
    vpoints->Delete() ;


    int tindices[10];
    vtkCellArray *cells = vtkCellArray::New() ;
    cells->Allocate(40000) ;  

    SimplexMeshType::CellsContainer::Pointer sCells;
    sCells = m_SimplexMesh->GetCells();
    for(SimplexMeshType::CellsContainer::Iterator cellIndex = sCells->Begin(); cellIndex != sCells->End() ; cellIndex++)
    {
      int numPts = cellIndex->Value()->GetNumberOfPoints() ;
      if (numPts == 2){
        int n = 0 ;
        // nb there is an unresolved problem here with cell types other than lines
        itk::CellInterface<SimplexMeshType::PixelType, SimplexMeshType::CellTraits>::PointIdConstIterator ptIdIterator ;
        for (ptIdIterator = cellIndex->Value()->PointIdsBegin() ; ptIdIterator != cellIndex->Value()->PointIdsEnd() ;  ptIdIterator++){
          tindices[n++] = *ptIdIterator ;
        }
        cells->InsertNextCell(n, tindices);
      }
    }
    cells->Squeeze() ;
    m_SimplexPolyData->SetLines(cells) ;
    cells->Delete() ;

    m_SimplexPolyData->Modified();
    m_SimplexMeshModified = false ;
  }
}





//-----------------------------------------------------------------------
// Set the vessel centerline and derive the stent centerline 
//-----------------------------------------------------------------------
void medVMEStent::SetVesselCenterLine(vtkPolyData *line){
  if (line == NULL)
    return ;

  bool needsUpdate ;
  if (m_CenterLineDefined)
    needsUpdate = (line->GetMTime() > m_CenterLine->GetMTime()) ;
  else
    needsUpdate = true ;

  if (needsUpdate){
    m_CenterLine->DeepCopy(line) ; 
    CreateLongVesselCenterLine() ;
    CreateStentCenterLine() ;
    m_CenterLineDefined = true ; 

    m_StentLengthModified = true ;
  }
}


//-------------------------------------------------------------------------
// Set the centerline
//-------------------------------------------------------------------------
void medVMEStent::SetVesselCenterLine(mafNode* node)
{
  if(node){
    mafVME *vme = mafVME::SafeDownCast(node);
    m_CenterLineNodeID = vme->GetId() ;
    m_CenterLineVMEDefined = true ;
    vtkPolyData *polyLine =vtkPolyData::SafeDownCast( vme->GetOutput()->GetVTKData());
    polyLine->Update();
    SetVesselCenterLine(polyLine);
  }
}


//------------------------------------------------------------------------------
// Set the vessel surface
//------------------------------------------------------------------------------
void medVMEStent::SetVesselSurface(vtkPolyData *surface){
  if(surface){
    m_VesselSurface->DeepCopy(surface);
    m_VesselSurfaceDefined = true;
  }
}

//-------------------------------------------------------------------------
// Set the vessel surface
//-------------------------------------------------------------------------
void medVMEStent::SetVesselSurface(mafNode* node)
{
  if(node){
    mafVME *vme = mafVME::SafeDownCast(node);
    m_VesselNodeID = vme->GetId() ;
    m_VesselVMEDefined = true ;
    vtkPolyData *polySurface = vtkPolyData::SafeDownCast(vme->GetOutput()->GetVTKData());
    polySurface->Update();
    SetVesselSurface(polySurface) ;
  }
}



//------------------------------------------------------------------------------
// Calc strut angle
//------------------------------------------------------------------------------
void medVMEStent::CalcStrutAngle()
{
  double r = m_Stent_Diameter/2.0 ;
  double alpha = 2.0*M_PI / (double)m_StrutPairsPerCrown ;
  double s = 2.0*r*sin(alpha/2.0) ;
  double t = s / (2.0 * m_Crown_Length) ;
  m_Strut_Angle = 2.0*atan(t) ;
}



//------------------------------------------------------------------------------
// Calc strut length
//------------------------------------------------------------------------------
void medVMEStent::CalcStrutLength()
{
  m_Strut_Length = 0.99 * m_Crown_Length / cos(m_Strut_Angle/2.0) ;
}



//------------------------------------------------------------------------------
// update data and return polydata                                                                    
//------------------------------------------------------------------------------
vtkPolyData* medVMEStent::GetStentPolyData()
{ 
  InternalUpdate();
  return m_StentPolyData ;
}



//------------------------------------------------------------------------------
// update data and return polydata                                                                    
//------------------------------------------------------------------------------
vtkPolyData* medVMEStent::GetSimplexPolyData()
{ 
  InternalUpdate();
  return m_SimplexPolyData ;
}


//-------------------------------------------------------------------------
// Find the tagged center line vme.
// Returns NULL if failed.
//-------------------------------------------------------------------------
mafNode* medVMEStent::FindTaggedCenterLineVME(mafNode* inputNode)
{
  // Go up one level in the tree
  mafNode* parentNode = inputNode->GetParent() ;

  // search tree for tagged item
  if (parentNode->GetTagArray()->IsTagPresent("RT3S_CENTER_LINE"))
    return parentNode ;

  for (int i = 0 ;  i < parentNode->GetNumberOfChildren() ;  i++){
    mafNode* childNode = parentNode->GetChild(i) ;
    if (childNode->GetTagArray()->IsTagPresent("RT3S_CENTER_LINE"))
      return childNode ;

    for (int j = 0 ;  j < childNode->GetNumberOfChildren() ;  j++){
      mafNode* grandChildNode = childNode->GetChild(j) ;
      if (grandChildNode->GetTagArray()->IsTagPresent("RT3S_CENTER_LINE"))
        return grandChildNode ;
    }
  }

  return NULL ;
}


//-------------------------------------------------------------------------
// Find the tagged vessel surface vme.
// Returns NULL if failed.
//-------------------------------------------------------------------------
mafNode* medVMEStent::FindTaggedVesselVME(mafNode* inputNode)
{
  // Go up one level in the tree
  mafNode* parentNode = inputNode->GetParent() ;

  // search tree for tagged item
  if (parentNode->GetTagArray()->IsTagPresent("RT3S_VESSEL"))
    return parentNode ;

  for (int i = 0 ;  i < parentNode->GetNumberOfChildren() ;  i++){
    mafNode* childNode = parentNode->GetChild(i) ;
    if (childNode->GetTagArray()->IsTagPresent("RT3S_VESSEL"))
      return childNode ;

    for (int j = 0 ;  j < childNode->GetNumberOfChildren() ;  j++){
      mafNode* grandChildNode = childNode->GetChild(j) ;
      if (grandChildNode->GetTagArray()->IsTagPresent("RT3S_VESSEL"))
        return grandChildNode ;
    }
  }

  return NULL ;
}





//-------------------------------------------------------------------------
// Find or select center line vme.
// Returns NULL if failed.
//-------------------------------------------------------------------------
mafNode* medVMEStent::FindOrSelectCenterLineVME(mafNode* inputNode)
{
  // search for id
  if (m_CenterLineNodeID != -1){
    mafNode *node = FindNodeWithId(m_CenterLineNodeID) ;
    if (node != NULL)
      return node ;
  }

  // search for tagged vme
  mafNode *node = FindTaggedCenterLineVME(inputNode) ;
  if (node != NULL)
    return node ;

  // can't find tagged vme so launch user select
  mafEvent e(this, VME_CHOOSE) ;
  mafString title = _("Select centerline vme");
  e.SetString(&title);
  ForwardUpEvent(e);
  node = e.GetVme();

  if (node != NULL){
    mafLogMessage("Adding tag to centerline vme...\n") ;
    mafVME* vme = mafVME::SafeDownCast(node) ;
    vme->GetTagArray()->SetTag("RT3S_CENTER_LINE","Polyline");
  }

  if (node == NULL){
    // can't find so invalidate flags
    m_CenterLineDefined = false ;
    m_CenterLineVMEDefined = false ;
    m_CenterLineNodeID = -1 ;
  }

  return node ;
}



//-------------------------------------------------------------------------
// Find or select the vessel surface vme.
// Returns NULL if failed.
//-------------------------------------------------------------------------
mafNode* medVMEStent::FindOrSelectVesselVME(mafNode* inputNode)
{
  // search for id
  if (m_VesselNodeID != -1){
    mafNode *node = FindNodeWithId(m_VesselNodeID) ;
    if (node != NULL)
      return node ;
  }

  // search for tagged vme
  mafNode *node = FindTaggedVesselVME(inputNode) ;
  if (node != NULL)
    return node ;

  // can't find tagged vme so launch user select
  mafEvent e(this, VME_CHOOSE) ;
  mafString title = _("Select vessel vme");
  e.SetString(&title);
  ForwardUpEvent(e);
  node = e.GetVme();

  if (node != NULL){
    mafLogMessage("Adding tag to vessel vme...\n") ;
    mafVME* vme = mafVME::SafeDownCast(node) ;
    vme->GetTagArray()->SetTag("RT3S_VESSEL","Surface");
  }

  if (node == NULL){
    // can't find so invalidate flags
    m_VesselSurfaceDefined = false ;
    m_VesselVMEDefined = false ;
    m_VesselNodeID = -1 ;
  }

  return node ;
}



//-------------------------------------------------------------------------
// Set the start position of the stent.
// The position is the id along the centerline.
//-------------------------------------------------------------------------
void medVMEStent::SetStentStartPos(int startId) 
{
  if (startId != m_StentStartPosId){
    m_StentStartPosId = startId ;
    CreateStentCenterLine() ;
    InternalUpdate() ;
  }
}




//-------------------------------------------------------------------------
// Utility method to create extrapolated polyline.
// The output line is the input plus an extrapolated section.
//-------------------------------------------------------------------------
void medVMEStent::CreateExtrapolatedLine(vtkPolyData* lineIn, vtkPolyData* lineOut, double extrapFactor)
{
  // If factor <= 1, just copy and return
  if (extrapFactor <= 1.0){
    lineOut->DeepCopy(lineIn) ;
    return ;
  }

  lineOut->Initialize() ;
  vtkCellArray *lines = vtkCellArray::New() ;
  vtkPoints *points = vtkPoints::New() ;

  int n = lineIn->GetPoints()->GetNumberOfPoints() ;
  int m = (int)(extrapFactor*(double)n + 0.5) ;

  vtkMAFMatrixVectorMath *matVecMath = vtkMAFMatrixVectorMath::New() ;
  matVecMath->SetHomogeneous(false) ;

  double x0[3], x1[3], x2[3], xnext[3] ;
  lineIn->GetPoint(n-3, x0) ;
  lineIn->GetPoint(n-2, x1) ;
  lineIn->GetPoint(n-1, x2) ;

  points->DeepCopy(lineIn->GetPoints()) ;

  for (int i = n ;  i < m ;  i++){
    double dx[3], cx[3] ;
    matVecMath->SubtractVectors(x2,x1,dx) ; // difference dx = x2-x1
    matVecMath->CopyVector(x2, cx) ; // curvature cx = x2 - 2x1 + x0
    matVecMath->AddMultipleOfVector(cx, -2.0, x1, cx) ;
    matVecMath->AddVectors(cx, x0, cx) ;

    matVecMath->CopyVector(x2, xnext) ; // xnext = x2 + dx + cx
    matVecMath->AddVectors(xnext, dx, xnext) ;
    matVecMath->AddVectors(xnext, cx, xnext) ;

    points->InsertNextPoint(xnext) ;

    matVecMath->CopyVector(x1,x0) ;
    matVecMath->CopyVector(x2,x1) ;
    matVecMath->CopyVector(xnext,x2) ;
  }

  // create the polyline cell
  int *ids = new int[m] ;
  for (int i = 0 ;  i < m ;  i++)
    ids[i] = i ;
  lines->InsertNextCell(m, ids) ;
  delete [] ids ;

  lineOut->SetPoints(points) ;
  lineOut->SetLines(lines) ;
  points->Delete() ;
  lines->Delete() ;

  matVecMath->Delete() ;
}



//-------------------------------------------------------------------------
// Utility method to create truncated polyline.
// The output line is a subset of the input from id0 to id1 inclusive.
// If id1 > n-1, the line is copied to the end.
//-------------------------------------------------------------------------
void medVMEStent::CreateTruncatedLine(vtkPolyData* lineIn, vtkPolyData* lineOut, int id0, int id1)
{
  lineOut->Initialize() ;

  int n = lineIn->GetPoints()->GetNumberOfPoints() ;
  int idlast = std::min(id1, n-1) ;
  int m = (int)(idlast - id0 + 1) ;

  if (m < 2)
    return ;

  vtkPoints *points = vtkPoints::New() ;
  for (int i = id0 ;  i <= idlast ;  i++){
    double x[3] ;
    lineIn->GetPoint(i, x) ;
    points->InsertNextPoint(x) ;
  }

  // create the polyline cell
  vtkCellArray *lines = vtkCellArray::New() ;
  int *ids = new int[m] ;
  for (int i = 0 ;  i < m ;  i++)
    ids[i] = i ;
  lines->InsertNextCell(m, ids) ;
  delete [] ids ;

  lineOut->SetPoints(points) ;
  lineOut->SetLines(lines) ;
  points->Delete() ;
  lines->Delete() ;
}



//-------------------------------------------------------------------------
// Create long vessel centerline
//-------------------------------------------------------------------------
void medVMEStent::CreateLongVesselCenterLine()
{
  CreateExtrapolatedLine(m_CenterLine, m_CenterLineLong, 2.0) ;
}



//-------------------------------------------------------------------------
// Create stent centerline from long vessel centerline and start pos
//-------------------------------------------------------------------------
void medVMEStent::CreateStentCenterLine()
{
  CreateTruncatedLine(m_CenterLineLong, m_StentCenterLine, m_StentStartPosId, 1E6) ;
  m_StentCenterLineModified = true ;
}



//-------------------------------------------------------------------------
// Get list of valid point id's, \n
// ie points which are members of cells. \n
// Needed because the stent contains unused and undefined points.
//-------------------------------------------------------------------------
void medVMEStent::GetValidPointIds(vtkPolyData* pd, vtkIdList* idList) const
{
  vtkMAFPolyDataNavigator* nav = vtkMAFPolyDataNavigator::New() ;
  vtkMAFPolyDataNavigator::IdSet idSet ; // copy to set first to efficiently remove duplicate ids
  vtkIdList* cellPtIds = vtkIdList::New() ;

  int ncells = pd->GetNumberOfCells() ;
  for (int i = 0 ;  i < ncells ;  i++){
    pd->GetCellPoints(i, cellPtIds) ;
    for (int j = 0 ;  j < cellPtIds->GetNumberOfIds() ;  j++){
      int id = cellPtIds->GetId(j) ;
      nav->AddUniqueIdToSet(idSet, id) ;  
    }
  }

  nav->CopyIdSetToList(idSet, idList) ;
  nav->Delete() ;
  cellPtIds->Delete() ;
}



//-------------------------------------------------------------------------
// Get the highest index of the valid points,
// ie points which are members of cells.
/// Needed because the stent contains unused and undefined points.
//-------------------------------------------------------------------------
int medVMEStent::GetHighestValidPointIndex(vtkPolyData *pd) const
{
  vtkIdList *ptIds = vtkIdList::New() ;
  GetValidPointIds(pd, ptIds) ;
  int idLast = -1 ;
  for (int i = 0 ;  i < ptIds->GetNumberOfIds() ;  i++){
    int id = ptIds->GetId(i) ;
    if (id > idLast)
      idLast = id ;
  }
  ptIds->Delete() ;
  return idLast ;
}



//-------------------------------------------------------------------------
// Get the approximate length of the stent.
// This is the no. of centerline vertices from end to end.
//-------------------------------------------------------------------------
int medVMEStent::CalcStentLengthVerts()
{
  if (m_StentLengthModified){
    // Get the nearest point on the centerline to the last point on the stent.
    // The stent contains undefined points(!) so we need the last valid one,
    // ie the highest pt id which is part of a cell.
    int idLast = GetHighestValidPointIndex(m_StentPolyData) ;

    double xlast[3] ;
    m_StentPolyData->GetPoint(idLast, xlast) ;
    int inear = -1 ;
    double r2near = 1.0E6 ;
    FindNearestPointOnCenterLine(xlast, m_StentCenterLine, inear, r2near) ;
    m_StentLength = inear ;
    m_StentLengthModified = false ;
  }

  return m_StentLength ;
}



//-------------------------------------------------------------------------
// Measure the current length of the stent in mm
//-------------------------------------------------------------------------
double medVMEStent::CalcStentLengthMM()
{
  vtkIdList* validPts = vtkIdList::New() ;
  GetValidPointIds(m_StentPolyData, validPts) ;

  // get points with min and max projection on center line.
  int idMin = -1 ;
  int idMax = -1 ;
  double lambdaMin = 0.0 ;
  double lambdaMax = 0.0 ;
  int n = validPts->GetNumberOfIds() ;
  for (int i = 0 ;  i < n ;  i++){
    double p[3] ;
    int idStent = validPts->GetId(i) ;
    m_StentPolyData->GetPoints()->GetPoint(idStent, p) ;

    int id ;
    double lambda, rsq ;
    FindNearestPointOnCenterLine(p, m_StentCenterLine, id, lambda, rsq) ;

    if ((i == 0) || (id < idMin) || ((id == idMin) && (lambda < lambdaMin))){
      idMin = id ;
      lambdaMin = lambda ;
    }

    if ((i == 0) || (id > idMax) || ((id == idMax) && (lambda > lambdaMax))){
      idMax = id ;
      lambdaMax = lambda ;
    }
  }

  vtkMAFMatrixVectorMath* matMath = vtkMAFMatrixVectorMath::New() ;
  matMath->SetHomogeneous(false) ;
  double xMin[3], xMax[3] ;
  CalcCoordsFromIdPosition(m_CenterLine, idMin, lambdaMin, xMin) ;
  CalcCoordsFromIdPosition(m_CenterLine, idMax, lambdaMax, xMax) ;
  double r = matMath->Distance(xMin, xMax) ;

  validPts->Delete() ;
  matMath->Delete() ;

  return r ;
}


//-------------------------------------------------------------------------
// Measure the current diameter of the stent in mm
//-------------------------------------------------------------------------
double medVMEStent::CalcStentDiameterMM()
{
  vtkIdList* validPts = vtkIdList::New() ;
  GetValidPointIds(m_StentPolyData, validPts) ;

  // get points with min and max projection on center line.
  double sumrsq = 0.0 ;
  int n = validPts->GetNumberOfIds() ;
  for (int i = 0 ;  i < n ;  i++){
    double p[3] ;
    int idStent = validPts->GetId(i) ;
    m_StentPolyData->GetPoints()->GetPoint(idStent, p) ;

    int id ;
    double lambda, rsq ;
    FindNearestPointOnCenterLine(p, m_StentCenterLine, id, lambda, rsq) ;
    sumrsq += rsq ;
  }

  double diam = 2.0*sqrt(sumrsq / (double)n) ;

  validPts->Delete() ;
  return diam ;
}



//-------------------------------------------------------------------------
// Calculate arc lengths of points along a center line.
//-------------------------------------------------------------------------
void medVMEStent::CalcArcLengthsOfPoints(vtkPolyData *pd, double* arcLengths) 
{
  int n = pd->GetPoints()->GetNumberOfPoints() ;

  vtkMAFMatrixVectorMath* matMath = vtkMAFMatrixVectorMath::New() ;
  matMath->SetHomogeneous(false) ;

  double x0[3], x1[3] ;
  arcLengths[0] = 0.0 ;
  pd->GetPoints()->GetPoint(0, x0) ;
  for (int i = 1 ;  i < n ;  i++){
    pd->GetPoints()->GetPoint(i-1, x0) ;
    pd->GetPoints()->GetPoint(i, x1) ;
    double di = matMath->Distance(x0, x1) ;
    arcLengths[i] = arcLengths[i-1]+di ;
  }

  matMath->Delete() ;
}



//-------------------------------------------------------------------------
// Calc arc length along center line given id position (id, lambda)
// where the position is a fraction lambda along the next segment id to id+1.
// This assumes that the pd is a monotonic curve of points.
//-------------------------------------------------------------------------
double medVMEStent::CalcArclengthFromIdPosition(vtkPolyData *pd, int id, double lambda) 
{
  int n = pd->GetPoints()->GetNumberOfPoints() ;
  assert((id >= 0) && (id < n)) ;

  double* arcLengths = new double[n+1] ; 
  CalcArcLengthsOfPoints(pd, arcLengths) ;
  arcLengths[n] = arcLengths[n-1] ;

  double arclen = (1.0-lambda)*arcLengths[id] + lambda*arcLengths[id+1] ;

  delete [] arcLengths ;
  return arclen ;
}



//-------------------------------------------------------------------------
// Calc id position (id, lambda) along center line given arc length
// where the position is a fraction lambda along the next segment id to id+1.
// This assumes that the pd is a monotonic curve of points.
//-------------------------------------------------------------------------
void medVMEStent::CalcIdPositionFromArcLength(vtkPolyData *pd, double arclen, int& id, double& lambda) 
{
  int n = pd->GetPoints()->GetNumberOfPoints() ;

  double* arcLengths = new double[n+1] ; 
  CalcArcLengthsOfPoints(pd, arcLengths) ;
  arcLengths[n] = arcLengths[n-1] ;

  // find first point where arc length >= arclen
  int idfound = -1 ;
  for (int i = 1 ;  i < n && idfound == -1 ;  i++){
    if (arcLengths[i] >= arclen)
      idfound = i ;
  }

  if (idfound != -1){
    if (arcLengths[idfound] == arclen){
      // return exact match
      id = idfound ;
      lambda = 0.0 ;
    }
    else{
      id = idfound-1 ;
      lambda = (arclen-arcLengths[id])/(arcLengths[id+1] - arcLengths[id]) ;
    }
  }
  else{
    // not found - return last point
    id = n-1 ;
    lambda = 0.0 ;
  }

  delete [] arcLengths ;
}




//-------------------------------------------------------------------------
// Calc coord position along center line given id position (id, lambda)
// where the position is a fraction lambda along the next segment id to id+1.
// This assumes that the pd is a monotonic curve of points.
//-------------------------------------------------------------------------
void medVMEStent::CalcCoordsFromIdPosition(vtkPolyData *pd, int id, double lambda, double* x) 
{
  int n = pd->GetPoints()->GetNumberOfPoints() ;
  assert((id >= 0) && (id < n)) ;
  assert((id < n-1) || (lambda == 0.0)) ;

  if (lambda == 0.0){
    pd->GetPoints()->GetPoint(id, x) ;
    return ;
  }
  else{
    vtkMAFMatrixVectorMath* matMath = vtkMAFMatrixVectorMath::New() ;
    matMath->SetHomogeneous(false) ;
    double x0[3], x1[3] ;
    pd->GetPoints()->GetPoint(id, x0) ;
    pd->GetPoints()->GetPoint(id+1, x1) ;
    matMath->InterpolateVectors(lambda, x0, x1, x) ;
    matMath->Delete() ;
    return ;
  }
}



//-------------------------------------------------------------------------
// Find nearest point id on center line.
//-------------------------------------------------------------------------
void medVMEStent::FindNearestPointOnCenterLine(double* p0, vtkPolyData *pd, int& id, double& distSq) 
{
  int n = pd->GetPoints()->GetNumberOfPoints() ;

  vtkMAFMatrixVectorMath* matMath = vtkMAFMatrixVectorMath::New() ;
  matMath->SetHomogeneous(false) ;

  // find nearest id point
  int imin = -1 ;
  distSq = -1.0 ;
  double x[3] ;
  for (int i = 0 ;  i < n ;  i++){
    pd->GetPoints()->GetPoint(i, x) ;
    double di2 = matMath->DistanceSquared(p0, x) ;
    if ((i == 0) || (di2 < distSq)){
      imin = i ;
      distSq = di2 ;
    }
  }

  id = imin ;
  matMath->Delete() ;
}



//-------------------------------------------------------------------------
// Find nearest point (id, lambda) on center line.
//-------------------------------------------------------------------------
void medVMEStent::FindNearestPointOnCenterLine(double* p0, vtkPolyData *pd, int& id, double& lambda, double& distSq) 
{
  int n = pd->GetPoints()->GetNumberOfPoints() ;

  vtkMAFMatrixVectorMath* matMath = vtkMAFMatrixVectorMath::New() ;
  matMath->SetHomogeneous(false) ;

  FindNearestPointOnCenterLine(p0, pd, id, distSq) ; // nearest actual point

  // search lambda to the left
  double xl[3], xm[3], xr[3], laml, lamr ;
  double distSql = -1.0 ;
  double distSqr = -1.0 ;
  pd->GetPoints()->GetPoint(id, xm) ;
  if (id > 0){
    double dp[3], dx[3] ;
    pd->GetPoints()->GetPoint(id-1, xl) ;
    matMath->SubtractVectors(p0, xl, dp) ; // dp = p0-xl
    matMath->SubtractVectors(xm, xl, dx) ; // dx = xm-xl
    double dotpp = matMath->DotProduct(dp,dp) ;
    double dotxx = matMath->DotProduct(dx,dx) ;
    double dotpx = matMath->DotProduct(dp,dx) ;
    laml = dotpx / dotxx ;
    if ((laml > 0.0) && (laml <= 1.0))
      distSql = dotpp - laml*laml*dotxx ;
  }

  // search lambda to the right
  if (id < n-1){
    double dp[3], dx[3] ;
    pd->GetPoints()->GetPoint(id+1, xr) ;
    matMath->SubtractVectors(p0, xm, dp) ; // dp = p0-xm
    matMath->SubtractVectors(xr, xm, dx) ; // dx = xr-xm
    double dotpp = matMath->DotProduct(dp,dp) ;
    double dotxx = matMath->DotProduct(dx,dx) ;
    double dotpx = matMath->DotProduct(dp,dx) ;
    lamr = dotpx / dotxx ;
    if ((lamr >= 0.0) && (lamr < 1.0))
      distSqr = dotpp - lamr*lamr*dotxx ;
  }

  // decide which side to choose
  bool choosel = (distSql != -1) ;
  bool chooser = (distSqr != -1) ;
  if (choosel && chooser){
    choosel = (distSql < distSqr) ;
    chooser = !choosel ;
  }

  if (choosel){
    id -= 1 ;
    lambda = laml ;
    distSq = distSql ;
  }
  else if (chooser){
    lambda = lamr ;
    distSq = distSqr ;
  }
  else{
    // choose neither side but keep original point
    lambda = 0.0 ;
  }

  matMath->Delete() ;
}



//-------------------------------------------------------------------------
// Partially initialize the deformation filter.
// This copies information from the stent model to the filter.
//-------------------------------------------------------------------------
void medVMEStent::PartialInitDefFilterFromStentModel()
{
  // set the input mesh
  m_DeformFilter->SetInput( m_SimplexMesh );

  // copy centerline indices of stent points to the def filter
  m_DeformFilter->SetCenterLocationIdx(m_StentSource->centerLocationIndex.begin()) ;

  // copy info about struts and links
  int tindices[2];
  vtkCellArray* strutArray = vtkCellArray::New() ;
  vtkCellArray* linkArray = vtkCellArray::New() ;

  for(StrutIterator iter = m_StentSource->GetStrutsList().begin(); iter != m_StentSource->GetStrutsList().end(); iter++){
    tindices[0] = iter->startVertex;
    tindices[1] = iter->endVertex;
    strutArray->InsertNextCell(2,tindices);
  }
  for(StrutIterator iter = m_StentSource->GetLinksList().begin(); iter != m_StentSource->GetLinksList().end(); iter++){
    tindices[0] = iter->startVertex;
    tindices[1] = iter->endVertex;
    linkArray->InsertNextCell(2,tindices);
  }

  m_DeformFilter->SetStrutLinkFromCellArray(strutArray, linkArray);

  strutArray->Delete() ;
  linkArray->Delete() ;
}



//-------------------------------------------------------------------------
// Has the simplex mesh been modified since the last call
//-------------------------------------------------------------------------
bool medVMEStent::IsSimplexMeshModified()
{
  static unsigned long mtime_old = 0 ;
  unsigned long mtime_new = m_SimplexMesh->GetMTime() ;
  if (mtime_new > mtime_old){
    mtime_old = mtime_new ;
    return true ;
  }
  else
    return false ;
}


//-------------------------------------------------------------------------
// Do one step of the deformation filter
//-------------------------------------------------------------------------
void medVMEStent::DoDeformationStep() 
{
  m_DeformFilter->Update() ;  
  m_SimplexMeshModified = true ;  
  UpdateStentPolydataFromSimplex() ;  
  UpdateStentPolydataFromSimplex_ViewAsSimplex() ;
}



//-------------------------------------------------------------------------
// Crimp the stent to a smaller diameter. \n
// The input params are for the expanded stent, \n
// so this should be the last step when the stent is created. \n
// This changes the diameter, strut angle and crown length, \n
// keeping the strut length const.
//-------------------------------------------------------------------------
void medVMEStent::CrimpStent(double crimpedDiameter)
{
  m_Stent_Diameter = crimpedDiameter ;

  double r = crimpedDiameter/2.0 ;
  double alpha = 2.0*M_PI / m_StrutPairsPerCrown ;
  double s = 2.0*r*sin(alpha/2.0) ;
  double c = sqrt(m_Strut_Length*m_Strut_Length - (s*s)/4.0) ;
  double theta = 2.0*acos(c/m_Strut_Length) ;

  m_Crown_Length = c ;
  m_Strut_Angle = theta ;
}



//-------------------------------------------------------------------------
// Search parent and siblings for node with given id
//-------------------------------------------------------------------------
mafNode* medVMEStent::FindNodeWithId(mafID id)
{
  if (id == -1)
    return NULL ;

  mafNode* parent = this->GetParent() ;
  if (parent->GetId() == id)
    return parent ;

  for (int i = 0 ;  i < parent->GetNumberOfChildren() ;  i++){
    mafNode* child = parent->GetChild(i) ;
    if (child->GetId() == id)
      return child ;

    for (int j = 0 ;  j < child->GetNumberOfChildren() ;  j++){
      mafNode* grChild = child->GetChild(j) ;
      if (grChild->GetId() == id)
        return grChild ;
    }
  }

  return NULL ;
}



//-------------------------------------------------------------------------
// Set stent polydata. \n
// Normally this is generated internally, but this allows an external op \n
// to impose a temporary position on the output polydata.
//-------------------------------------------------------------------------
void medVMEStent::SetStentPolyData(vtkPolyData* pd)
{
  int n1 = m_StentPolyData->GetPoints()->GetNumberOfPoints() ;
  int n2 = pd->GetPoints()->GetNumberOfPoints() ;
  assert(n2 == n1) ;

  for (int i = 0 ;  i < n1 ;  i++){
    double x[3] ;
    pd->GetPoint(i, x) ;
    m_StentPolyData->GetPoints()->SetPoint(i, x) ;
  }
}



//-------------------------------------------------------------------------
// Set the deployed polydata vme.
//-------------------------------------------------------------------------
void medVMEStent::SetDeployedPolydataVME(mafNode* inputNode)
{
  m_DeployedPolydataVME = mafVMEPolyline::SafeDownCast(inputNode) ;
  m_DeployedPolydataNodeID = inputNode->GetId() ;
  vtkPolyData *pd = vtkPolyData::SafeDownCast(m_DeployedPolydataVME->GetOutput()->GetVTKData()) ;
  pd->Update() ;
  SetStentPolyData(pd) ;
  m_DeployedPolydataStatus = DEPLOYED_PD_OK ;
}



//-------------------------------------------------------------------------
// Get number of units in stent. \n
// This includes partial units at the end, if the no. of crowns is odd.
//-------------------------------------------------------------------------
int medVMEStent::GetNumberOfUnits()
{
  int n ;

  switch(GetStentType()){
  case 0:
    // Maris stent is C-L-C-HL
    //                      HL-C-L-C-HL
    //                               HL-C-L-C-HL
    //                                        HL-C-L-C (even) or HL-C (odd)
    n = (m_NumberOfCrowns + 1)/2 ; // +1 to include partial unit if no. of crowns is odd.
    break ;
  case 1:
    // Abbott is C-L-PC
    //               PC-L-C-L-PC
    //                        PC-L-C-L-PC
    //                                 PC-L (even) or PC-L-C-L (odd)
    n = (m_NumberOfCrowns + 2)/2 ;
    break ;
  default:
    n = 0 ;
  }

  return n ;
}


//-------------------------------------------------------------------------
// Get length of unit
//-------------------------------------------------------------------------
double medVMEStent::GetLengthOfUnit()
{
  double len ;

  wxString model = GetStentModelName() ;
  if (model == "MARIS PLUS"){
    // Maris unit is HL-C-L-C-HL where HL is half-link
    len = 2.0*m_Crown_Length + 2.0*m_Link_Length ;
  }
  else if (model == "ABSOLUTE PRO"){
    // Abbott stent is PC-L-C-L-PC where PC is part-crown.  Crowns overlap.
    len = 2.0*m_Link_Length ;
  }
  else
    len = 0.0 ;

  return len ;
}
