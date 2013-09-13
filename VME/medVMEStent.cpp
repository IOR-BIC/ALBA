/*=========================================================================
Program:   MAF2Medical
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
#include "vtkMEDDeformableSimplexMeshFilter.h"
#include "vtkMEDMatrixVectorMath.h"
#include "vtkMEDPolyDataNavigator.h"
#include "vtkMEDMatrixVectorMath.h"
#include "medVMEStent.h"

#include "vtkPointData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkFloatArray.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkAppendPolyData.h"

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
  m_SimplexMeshModified(true), m_StentLengthModified(true)
{
  m_StentSource = new vtkMEDStentModelSource ;

  m_StentPolyData = vtkPolyData::New() ;
  m_SimplexPolyData = vtkPolyData::New() ;

  m_CenterLine = vtkPolyData::New() ;
  m_CenterLineLong = vtkPolyData::New() ;
  m_StentCenterLine = vtkPolyData::New() ;
  m_VesselSurface = vtkPolyData::New() ;

  m_Struts_Number = 16 ;
  m_Stent_Diameter = 2.0 ;
  m_Crown_Length = 2.2;
  m_Crown_Number = 10;
  m_Strut_Angle = 60.0;
  m_Strut_Thickness = 0.0;
  m_Id_Stent_Configuration = 1; /* 1.outofphase, 0.InPhase;  enumStCfgType */
  m_Id_Link_Connection = 2; /* 0.peak2valley;enumLinkConType {peak2valley, valley2peak, peak2peak, valley2valley} */
  m_Link_orientation = 0;
  m_Link_Length = 1.0;
  m_Link_Alignment = 0;
  m_ComputedCrownNumber = 0;

  m_StentStartPosId = 0 ;

  m_CenterLineSetFlag = 0; 
  m_VesselSurfaceSetFlag = 0;

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
// Deep Copy
//-------------------------------------------------------------------------
int medVMEStent::DeepCopy(mafNode *a)
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    medVMEStent *vmeStent = medVMEStent::SafeDownCast(a);

    m_Stent_Diameter = vmeStent->m_Stent_Diameter ;
    m_Crown_Length = vmeStent->m_Crown_Length ;
    m_Crown_Number = vmeStent->m_Crown_Number ;
    m_Strut_Angle = vmeStent->m_Strut_Angle ;
    m_Strut_Thickness = vmeStent->m_Strut_Thickness ;
    m_Strut_Length = vmeStent->m_Strut_Length ;
    m_Link_Length = vmeStent->m_Link_Length;
    m_Link_Alignment = vmeStent->m_Link_Alignment;
    m_Link_orientation = vmeStent->m_Link_orientation;
    m_Id_Link_Connection = vmeStent->m_Id_Link_Connection;
    m_Id_Stent_Configuration = vmeStent->m_Id_Stent_Configuration ;

    m_CenterLine->DeepCopy(vmeStent->m_CenterLine);
    m_VesselSurface->DeepCopy(vmeStent->m_VesselSurface);

    m_StentStartPosId = vmeStent->m_StentStartPosId ;

    m_CenterLineSetFlag = vmeStent->m_CenterLineSetFlag ;
    m_VesselSurfaceSetFlag = vmeStent->m_VesselSurfaceSetFlag ;

    m_StentParamsModified = true ;
    m_StentCenterLineModified = true ;
    m_SimplexMeshModified = true ;
    InternalUpdate();

    return MAF_OK;
  }  
  return MAF_ERROR;
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
    m_Gui->Integer(CHANGED_STENT_PARAM,_("Crown num"), &m_Crown_Number, 0, 10000,-1,_("The number of the Crowns"));
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
    m_Gui->Combo(CHANGED_STENT_PARAM, "Orientation", &m_Link_orientation, 3, linkOrientation);	  

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

  m_VesselSurfaceSetFlag = 0 ;
  node = FindOrSelectVesselVME(this) ;
  m_VesselVME = mafVME::SafeDownCast(node) ;
  SetVesselSurface(node);

  m_CenterLineSetFlag = 0 ;
  node = FindOrSelectCenterLineVME(this) ;
  m_CenterLineVME = mafVME::SafeDownCast(node) ;
  SetVesselCenterLine(node);

  InternalUpdate();
  m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
  ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
  m_Gui->Update();
}



//-----------------------------------------------------------------------
// update stent after a change in the parameters.
//-----------------------------------------------------------------------
void medVMEStent::InternalUpdate()
{
  if(m_StentParamsModified || m_StentCenterLineModified){	
    m_StentSource->setStentDiameter(m_Stent_Diameter);
    m_StentSource->setCrownLength(m_Crown_Length);
    m_StentSource->setCrownNumber(m_Crown_Number);

    m_StentSource->setStentConfiguration((enumStCfgType)m_Id_Stent_Configuration);
    m_StentSource->setLinkConnection((enumLinkConType) m_Id_Link_Connection);
    m_StentSource->setLinkOrientation( (enumLinkOrtType)m_Link_orientation);

    m_StentSource->setLinkLength(m_Link_Length);
    m_StentSource->setLinkAlignment(m_Link_Alignment);
    m_StentSource->setStrutsNumber(m_Struts_Number);

    if (m_Strut_Angle > 0.0)
      m_StentSource->setStrutAngle(m_Strut_Angle) ; // set angle and calc strut length

    int linePointNumber = m_CenterLine->GetNumberOfPoints();

    if(m_CenterLine != NULL && linePointNumber>0){
      m_StentSource->setCenterLineFromPolyData(m_StentCenterLine);
      //m_ComputedCrownNumber = m_StentSource->computeCrownNumberAfterSetCenterLine();
      //if (m_Crown_Number>m_ComputedCrownNumber)
      //{
      //  m_Crown_Number = m_ComputedCrownNumber;
      //}
    }
    m_StentSource->setCrownNumber(m_Crown_Number);

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

  //m_SimplexMeshModified = IsSimplexMeshModified() ;
  if (m_SimplexMeshModified){
    m_StentPolyData->Initialize() ;
    m_SimplexPolyData->Initialize() ;
    UpdateStentPolydataFromSimplex() ; 
    UpdateStentPolydataFromSimplex_ViewAsSimplex() ; 
    m_SimplexMeshModified = false ;
    this->Modified() ;
  }
}



//------------------------------------------------------------------------------
// Give ABBoTT and BARD stent fixed parameter
//------------------------------------------------------------------------------
/*void medVMEStent::SetFixedParameterForSpecialStent(){
if(m_Id_Stent_Type==1){//ABBOTT
m_Stent->SetStentDiameter(8);

m_Stent->SetStentCrownLength(3.8);
m_Stent->SetStentConfiguration(0);//0 in phase, 1 out of phase
m_Stent->SetLinkConnection(0);//0.peak2valley;enumLinkConType {peak2valley, valley2peak, peak2peak, valley2valley} 
m_Stent->SetLinkLength(2.2); //6 longer than 3.8 or 2.2 shorter than 3.8

m_Stent->SetLinkAlignment(0);//0,1,2
m_Stent->SetLinkOrientation(0);//0,1,-1

m_Stent->SetStrutsNumber(6);
m_Stent->SetLinkNumber(3);
}else if(m_Id_Stent_Type==2){//BARD

}
}*/



//------------------------------------------------------------------------------
// Update stent polydata from simplex
//------------------------------------------------------------------------------
void medVMEStent::UpdateStentPolydataFromSimplex()
{
  if (m_SimplexMeshModified){
    vtkPoints* vpoints = vtkPoints::New();
    vpoints->SetNumberOfPoints(2000);		

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
    lines->Allocate(20000) ;  

    for(StrutIterator iter = m_StentSource->GetStrutsList().begin(); iter !=m_StentSource->GetStrutsList().end(); iter++){//iter++){
      if(m_StentSource->getInphaseShort()==1){
        // Abbott stent
        int tindices2[4] ;
        double endPoints[4][3], midPoints[4][3];

        tindices2[0]=iter->startVertex;
        tindices2[1]=iter->endVertex;
        vpoints->GetPoint(tindices2[0], endPoints[0]) ;
        vpoints->GetPoint(tindices2[1], endPoints[1]) ;

        iter++;
        tindices2[2] = iter->startVertex;
        tindices2[3] = iter->endVertex;
        vpoints->GetPoint(tindices2[2], endPoints[2]) ;
        vpoints->GetPoint(tindices2[3], endPoints[3]) ;

        CalculateMidPointsFromPairOfStruts(endPoints, midPoints) ;

        // add the 4 new midpoints
        int newPtIds[4] ;
        for(int i=0;i<4;i++){
          newPtIds[i] = vpoints->InsertNextPoint(midPoints[i]);
          pointCount++;
        }

        //----------left arm------------
        int tindicesShort[2] ;
        tindicesShort[0]=tindices2[0];
        tindicesShort[1]=newPtIds[0] ;
        lines->InsertNextCell(2,tindicesShort);

        tindicesShort[0]=newPtIds[0];
        tindicesShort[1]=newPtIds[1];
        lines->InsertNextCell(2,tindicesShort);

        tindicesShort[0]=newPtIds[1];
        tindicesShort[1]=tindices2[1];
        lines->InsertNextCell(2,tindicesShort);

        //-----------right arm---------------
        tindicesShort[0]=tindices2[2];
        tindicesShort[1]=newPtIds[2] ;
        lines->InsertNextCell(2,tindicesShort);

        tindicesShort[0]=newPtIds[2];
        tindicesShort[1]=newPtIds[3];
        lines->InsertNextCell(2,tindicesShort);

        tindicesShort[0]=newPtIds[3];
        tindicesShort[1]=tindices2[3];
        lines->InsertNextCell(2,tindicesShort);
      }
      else{
        // Simple stent with straight struts
        tindices[0] = iter->startVertex;
        tindices[1] = iter->endVertex;
        lines->InsertNextCell(2, tindices);
        iter++;
      }
    }


    // Do the cells for the links
    for(StrutIterator iter = m_StentSource->GetLinksList().begin(); iter !=m_StentSource->GetLinksList().end(); iter++){
      tindices[0] = iter->startVertex;
      tindices[1] = iter->endVertex;
      lines->InsertNextCell(2, tindices);
    }
    lines->Squeeze() ;
    m_StentPolyData->SetLines(lines);
    lines->Delete() ;

    m_StentPolyData->Modified();
    //m_SimplexMeshModified = false ; // wait until simplex version is called before false
  }
}



//------------------------------------------------------------------------------
// Calculate extra mid-points on struts of Abbott stent polydata.
// Input is a pair of adjacent struts - output is four midpoints.
//------------------------------------------------------------------------------
void medVMEStent::CalculateMidPointsFromPairOfStruts(const double strutEndPts[4][3], double midPts[4][3]) const
{
  const double LengthFactor = 0.01 ;

  vtkMEDMatrixVectorMath *math = vtkMEDMatrixVectorMath::New() ;
  math->SetHomogeneous(false) ;

  // calc exact midpoints m[2] of struts
  double m[2][3] ;
  for (int i = 0 ;  i < 2 ;  i++)
    math->MeanVector(strutEndPts[2*i], strutEndPts[2*i+1], m[i]) ;

  // calc directions v[2] and magnitudes r[2] of struts
  double v[2][3], r[2], rsq[2] ; 
  for (int i = 0 ;  i < 2 ;  i++){
    math->SubtractVectors(strutEndPts[2*i+1], strutEndPts[2*i], v[i]) ;
    r[i] = math->MagnitudeOfVector(v[i]) ;
    rsq[i] = r[i]*r[i] ;
  }

  // calc vector directions dm[2] from each midpoint to opposite midpoint
  double dm[2][3] ;
  math->SubtractVectors(m[1], m[0], dm[0]) ;
  math->SubtractVectors(m[0], m[1], dm[1]) ;

  // calc dot products of m and v
  double dprod[2] ;
  for (int i = 0 ;  i < 2 ;  i++)
    dprod[i] = math->DotProduct(dm[i], v[i]) ;
 
  // Calc small displacement vectors a[2][3] from midpoints.
  // Each displacement vector must be normal to strut and coplanar with strut and opposite midpoint.
  // Thus a = dprod*v - rsq*dm
  double a[2][3] ; // displacement vectors
  for (int i = 0 ;  i < 2 ;  i++){
    math->MultiplyVectorByScalar(dprod[i], v[i], a[i]) ;
    math->SubtractMultipleOfVector(a[i], rsq[i], dm[i], a[i]) ;
    math->NormalizeVector(a[i]) ;
  }

  // Add displacements to midpoints to calc output midpoints
  for (int i = 0 ;  i < 2 ;  i++){
    math->SubtractMultipleOfVector(m[i], LengthFactor*r[i], a[i], midPts[2*i]) ;
    math->AddMultipleOfVector(m[i], LengthFactor*r[i], a[i], midPts[2*i+1]) ;
  }

  math->Delete() ;
}




//------------------------------------------------------------------------------
// Copy simplex directly to polydata
//------------------------------------------------------------------------------
void medVMEStent::UpdateStentPolydataFromSimplex_ViewAsSimplex()
{
  if (m_SimplexMeshModified){
    vtkPoints* vpoints = vtkPoints::New();
    vpoints->SetNumberOfPoints(2000);		

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
    cells->Allocate(2000) ;  

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
  if(line){
    m_CenterLine->DeepCopy(line) ; 
    CreateLongVesselCenterLine() ;
    CreateStentCenterLine() ;
    m_CenterLineSetFlag = 1; 

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
    m_VesselSurfaceSetFlag = 1;
  }
}

//-------------------------------------------------------------------------
// Set the vessel surface
//-------------------------------------------------------------------------
void medVMEStent::SetVesselSurface(mafNode* node)
{
  if(node){
    mafVME *vme = mafVME::SafeDownCast(node);
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
  double alpha = 2.0*M_PI / (double)m_Struts_Number ;
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
// Find the tagged vesselt surface vme.
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
  // search for tagged vme
  mafNode *node = FindTaggedCenterLineVME(inputNode) ;

  if (node != NULL)
    mafLogMessage("Found centerline ok\n") ;
  else{
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
  }

  return node ;
}



//-------------------------------------------------------------------------
// Find or select the vessel surface vme.
// Returns NULL if failed.
//-------------------------------------------------------------------------
mafNode* medVMEStent::FindOrSelectVesselVME(mafNode* inputNode)
{
  // search for tagged vme
  mafNode *node = FindTaggedVesselVME(inputNode) ;

  if (node != NULL)
    mafLogMessage("Found vessel ok\n") ;
  else{
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

  vtkMEDMatrixVectorMath *matVecMath = vtkMEDMatrixVectorMath::New() ;
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
// Get the highest index of the valid points,
// ie points which are members of cells.
/// Needed because the stent contains unused and undefined points.
//-------------------------------------------------------------------------
int medVMEStent::GetHighestValidPointIndex(vtkPolyData *pd) const
{
  vtkIdList *ptIds = vtkIdList::New() ;
  int ncells = pd->GetNumberOfCells() ;
  int idLast = -1 ;
  for (int i = 0 ;  i < ncells ;  i++){
    pd->GetCellPoints(i, ptIds) ;
    for (int j = 0 ;  j < ptIds->GetNumberOfIds() ;  j++){
      int id = ptIds->GetId(j) ;
      if (id > idLast)
        idLast = id ;
    }
  }
  ptIds->Delete() ;
  return idLast ;
}



//-------------------------------------------------------------------------
// Get the approximate length of the stent.
// This is the no. of centerline vertices from end to end.
//-------------------------------------------------------------------------
int medVMEStent::GetStentLength()
{
  if (m_StentLengthModified){
    vtkMEDMatrixVectorMath *matVecMath = vtkMEDMatrixVectorMath::New() ;
    matVecMath->SetHomogeneous(false) ;

    // Get the nearest point on the centerline to the last point on the stent.
    // The stent contains undefined points(!) so we need the last valid one,
    // ie the highest pt id which is part of a cell.
    int idLast = GetHighestValidPointIndex(m_StentPolyData) ;

    double xlast[3] ;
    m_StentPolyData->GetPoint(idLast, xlast) ;

    int n = m_StentCenterLine->GetPoints()->GetNumberOfPoints() ;
    int inear = -1 ;
    double r2near = 1.0E6 ;
    for (int i = 0 ;  i < n ;  i++){
      double x[3] ;
      m_StentCenterLine->GetPoint(i, x) ;
      double r2 = matVecMath->DistanceSquared(x, xlast) ;
      if ((i == 0) || (r2 < r2near)){
        r2near = r2 ;
        inear = i ;
      }
    }

    m_StentLength = inear ;
    m_StentLengthModified = false ;
    matVecMath->Delete() ;
  }

  return m_StentLength ;
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
/// Crimp the stent to a smaller diameter. \n
/// The input params are for the expanded stent, \n
/// so this should be the last step when the stent is created. \n
/// This changes the diameter, strut angle and crown length, \n
/// keeping the strut length const.
//-------------------------------------------------------------------------
void medVMEStent::CrimpStent(double crimpedDiameter)
{
  m_Stent_Diameter = crimpedDiameter ;
  
  double r = crimpedDiameter/2.0 ;
  double alpha = 2.0*M_PI / m_Struts_Number ;
  double s = 2.0*r*sin(alpha/2.0) ;
  double c = sqrt(m_Strut_Length*m_Strut_Length - (s*s)/4.0) ;
  double theta = 2.0*acos(c/m_Strut_Length) ;

  m_Crown_Length = c ;
  m_Strut_Angle = theta ;
}
