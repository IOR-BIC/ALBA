/*=========================================================================
Program:   MAF2Medical
Module:    $RCSfile: medVMEStent.cpp,v $
Language:  C++
Date:      $Date: 2012-10-23 10:15:31 $
Version:   $Revision: 1.1.2.8 $
Authors:   Hui Wei
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

/**----------------------------------------------------------------------------*/
// Include:
/**----------------------------------------------------------------------------*/
#include "medVMEStent.h"

#include "itkDeformableSimplexMesh3DFilter.h"
#include "itkMesh.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkCovariantVector.h"
#include "itkSimplexMesh.h"
#include "itkDefaultDynamicMeshTraits.h"

#include "mmuIdFactory.h"
#include "mafDataVector.h"
#include "mafMatrixInterpolator.h"
#include "mafDataPipeInterpolator.h"
#include "mafTagArray.h"
#include "mafMatrixVector.h"
#include "mafVMEItemVTK.h"
#include "mafEventSource.h"
#include "mafTransform.h"
#include "mmaMaterial.h"
#include "mafVMEOutputSurface.h"
#include "mafVMEOutputPolyline.h"
#include "mafDataPipeCustom.h"
#include "mafVMEStorage.h"
#include "vtkMAFSmartPointer.h"
#include "mafGUI.h"
#include "mafGuiDialog.h"
#include "mafGUIValidator.h"
#include "mafGUIFloatSlider.h"
#include "mafGUIButton.h"
#include "mafRWI.h"
#include "mafRWIBase.h"

#include "medVMEPolylineGraph.h"
#include "vtkMEDDeformableSimplexMeshFilter.h"
#include "medVMEStentDeploymentVisualPipe.h"

#include "vtkBitArray.h"
#include "vtkPointData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include <vtkSmartPointer.h>

#include "vtkFloatArray.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkProperty.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkAppendPolyData.h"
#include "vtkTubeFilter.h"
#include "vtkTriangleFilter.h"

#include <iostream>
#include <time.h>

#include "vtkMEDPolyDataNavigator.h"
#include <fstream>



//----------------------------------------------------------------------------
// define:
//----------------------------------------------------------------------------

//typedef itk::DeformableSimplexMeshFilterImpl<SimplexMeshType,SimplexMeshType> DeformFilterType;
//DeformFilterType::Pointer m_DeformFilter;
static int numberOfCycle = 0;

//------------------------------------------------------------------------------
mafCxxTypeMacro(medVMEStent);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// construct method
//------------------------------------------------------------------------------
medVMEStent::medVMEStent()
  : m_Alpha_Default(0.2), m_Beta_Default(0.02), m_Gamma_Default(0.05),
  m_Rigidity_Default(1.0), m_Damping_Default(0.65), m_Epsilon_Default(0.3),
  m_IterationsPerStep_Default(1), m_NumberOfSteps_Default(50),
  m_CatheterSpeed_Default(0.5), m_CatheterPauseOn_Default(0), 
  m_CatheterPausePosition_Default(10.0), m_CatheterPauseDuration_Default(30),
  m_DeploymentMode(DEPLOYMENT_MODE_DEFORM),
  m_DefParamsDlg(NULL), m_DeployCtrlDlg(NULL),
  m_DeploymentVisualPipe(NULL),
  m_StentStartPosId(30), m_StentTubeRadius(0.0)
{
  m_StentPolyData = vtkPolyData::New() ;
  m_TestVesselPolyData = vtkPolyData::New() ;

  m_AppendPolyData = vtkAppendPolyData::New() ;
  m_AppendPolyData->AddInput(m_StentPolyData);
  m_AppendPolys = m_AppendPolyData->GetOutput();

  m_VesselCenterLine = vtkPolyData::New() ;
  m_StentCenterLine = vtkPolyData::New() ;
  m_ConstraintSurface = vtkPolyData::New() ;

  mafNEW(m_Transform);

  m_Stent_Diameter = 2.0 ;
  m_Stent_Radius = m_Stent_Diameter/2.0 ;
  m_Crown_Length = 2.2;
  m_Crown_Number = 10;
  m_Strut_Angle = 0.0;
  m_Strut_Thickness = 0.0;
  m_Id_Stent_Configuration = 1;/* 1.outofphase, 0.InPhase;  enumStCfgType */
  m_Id_Link_Connection = 2; /* 0.peak2valley;enumLinkConType {peak2valley, valley2peak, peak2peak, valley2valley} */
  m_Link_orientation = 0;
  m_Link_Length = 3.0;
  m_Link_Alignment = 0;

  m_VesselCenterLineName = "";
  m_ConstraintSurfaceName = "";
  m_VesselCenterLineSetFlag = 0; 
  m_ConstraintSurfaceSetFlag = 0;
  m_ComputedCrownNumber = 0;

  m_DeformFlag =0;
  m_ShowCatheter = false;
  m_numberOfCycle = 0;

  //mafVMEOutputSurface *output=mafVMEOutputSurface::New(); // an output with no data
  mafVMEOutputPolyline *output=mafVMEOutputPolyline::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  GetMaterial();


  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();

  /** stent,catheter and surface  */
  dpipe->SetInput(m_AppendPolys);
  //dpipe->SetInput(m_StentPolyData); //temp
  SetDataPipe(dpipe);

  SetDefParamsToDefaults() ;


  // catheter visual pipe
  m_CatheterCenterLine = vtkPolyData::New() ;

  m_CatheterTubeFilter = vtkTubeFilter::New() ;
  m_CatheterTubeFilter->SetInput(m_CatheterCenterLine) ;
  m_CatheterTubeFilter->SetRadius(m_Stent_Radius + 0.1) ;
  m_CatheterTubeFilter->SetNumberOfSides(12) ;

  m_CatheterPolyData = m_CatheterTubeFilter->GetOutput() ;


  // deformation filter
  // NB smart itk pointer - don't delete
  m_DeformFilter = DeformFilterType::New() ; 

  m_StrutArray = vtkCellArray::New() ;
  m_LinkArray = vtkCellArray::New() ;
}



//-------------------------------------------------------------------------
//  destruction method
//-------------------------------------------------------------------------
medVMEStent::~medVMEStent()
{
  m_StentPolyData->Delete() ;
  m_TestVesselPolyData->Delete() ;
  m_AppendPolyData->Delete() ;
  m_VesselCenterLine->Delete() ;
  m_StentCenterLine->Delete() ;
  m_ConstraintSurface->Delete() ;

  mafDEL(m_Transform);

  //--remove stent points cache-
  for (int i = 0 ;  i < (int)m_ItPointsContainer.size() ;  i++)
    m_ItPointsContainer[i]->Delete() ;
  m_ItPointsContainer.clear();

  m_CatheterCenterLine->Delete() ;
  m_CatheterTubeFilter->Delete() ;

  if (m_DeploymentVisualPipe != NULL)
    delete m_DeploymentVisualPipe ;

  m_StrutArray->Delete() ;
  m_LinkArray->Delete() ;

  SetOutput(NULL);
}



//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
mafVMEOutputPolyline *medVMEStent::GetPolylineOutput()
{
  return (mafVMEOutputPolyline *)GetOutput();
  //return (mafVMEOutputPolyline *)m_StentPolyData;
}


/*
//-----------------------------------------------------------------------
void medVMEStent::SetTimeStamp(mafTimeStamp t)
//-----------------------------------------------------------------------
{
t = t < 0 ? 0 : t;
bool update_vtk_data = t != m_CurrentTime;
if (update_vtk_data)
{
Superclass::SetTimeStamp(t);

DisplayStentExpandByStep(t);

}
}
*/

//-------------------------------------------------------------------------
/*mafVMEOutputSurface *medVMEStent::GetSurfaceOutput()
//-------------------------------------------------------------------------
{
return (mafVMEOutputSurface *)GetOutput();
}*/


/** copy attributes*/
//-------------------------------------------------------------------------
int medVMEStent::DeepCopy(mafNode *a)
  //-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    medVMEStent *vmeStent =medVMEStent::SafeDownCast(a);
    m_Transform->SetMatrix(vmeStent->m_Transform->GetMatrix());

    //GetMaterial();

    //copy a attributes to this object
    /** basic stent  */
    m_Stent_Diameter = vmeStent->GetStentDiameter();
    m_Crown_Length = vmeStent->GetStentCrownLength();
    m_Strut_Thickness = vmeStent->GetStrutThickness();
    m_Id_Stent_Configuration = vmeStent->GetStentConfiguration();
    /**  stent link  */
    m_Id_Link_Connection = vmeStent->GetLinkConnection();
    m_Link_Length = vmeStent->m_Link_Length;
    m_Link_Alignment = vmeStent->m_Link_Alignment;
    m_Link_orientation = vmeStent->m_Link_orientation;
    m_Stent_Radius = vmeStent->m_Stent_Radius;
    /**----------- center line and constraint surface-----------*/
    m_VesselCenterLineName = vmeStent->m_VesselCenterLineName;
    m_ConstraintSurfaceName = vmeStent->m_ConstraintSurfaceName;

    m_VesselCenterLine->DeepCopy(vmeStent->m_VesselCenterLine);
    m_ConstraintSurface->DeepCopy(vmeStent->m_ConstraintSurface);


    /**-------- compute and update---------------*/

    InternalUpdate();
    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      //dpipe->SetInput(m_StentPolyData);
      dpipe->SetInput(m_AppendPolyData->GetOutput());
    }
    return MAF_OK;
  }  
  return MAF_ERROR;

}



/** two vmes*/
//-------------------------------------------------------------------------
bool medVMEStent::Equals(mafVME *vme)
  //-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    if (true     
      //m_Transform->GetMatrix()==((mafVMEStent *)vme)->m_Transform->GetMatrix() &&
      // this->m_GeometryType == ((mafVMEStent *)vme)->m_GeometryType &&
      //parameters equal judge  
      )
    {
      ret = true;
    }
  }
  return ret;

}  
/** transform matrix*/
//-------------------------------------------------------------------------
void medVMEStent::SetMatrix(const mafMatrix &mat)
  //-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}
/** return always false since  the vme is not an animated VME (position 
is the same for all timestamps). */
//-------------------------------------------------------------------------
bool medVMEStent::IsAnimated()
  //-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
void medVMEStent::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
  //-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
  mafTimeStamp t = m_Transform->GetMatrix().GetTimeStamp();
  kframes.push_back(t);
}


/** Internally used to create a new instance of the GUI.*/
//-------------------------------------------------------------------------
mafGUI* medVMEStent::CreateGui()
  //-------------------------------------------------------------------------
{
  mafVME::CreateGui();
  if(m_Gui)
  {
    m_Gui->Label("Stent");

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

    m_Gui->Button(ID_CENTERLINE, &m_VesselCenterLineName,  _("center line"), _("Select the centerline for creating stent"));//Centerline
    //m_Gui->Label("Constraint Surface");
    //m_Gui->Button(ID_CONSTRAINT_SURFACE, &m_ConstraintSurfaceName, _(".."), _("Select the constraint surface for deploying stent"));
    m_Gui->Button(ID_CONSTRAINT_SURFACE, &m_ConstraintSurfaceName,  _("constraint surface"), _("Select the constraint surface for deploying stent"));

    m_Gui->Divider(2);

    //change Vessel View
    m_Gui->Bool(DISPLAY_CATHETER,_("catheter"),&m_ShowCatheter,0,"Show catheter");
    m_Gui->Enable(DISPLAY_CATHETER,m_VesselCenterLineSetFlag);

    m_Gui->Button(ID_DEFORMATION_PARAMETERS, _("deform params"),""  , _(" deformation parameters"));
    m_Gui->Enable(ID_DEFORMATION_PARAMETERS, true);

    m_Gui->Button(ID_DEFORMATION, _("deformation"),""  , _(" stent deformation"));
    m_Gui->Enable(ID_DEFORMATION,(m_VesselCenterLineSetFlag)&&(m_ConstraintSurfaceSetFlag));


    m_Gui->FitGui();
    m_Gui->Update();
  }
  m_Gui->Divider();
  return m_Gui;
}



//-------------------------------------------------------------------------
// Precess events coming from other objects
//-------------------------------------------------------------------------
void medVMEStent::OnEvent(mafEventBase *maf_event)
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case CHANGED_STENT_PARAM:
      {  
        /*if centerline was set compute maximum crownNumber*/
        /* if (this->m_VesselCenterLine )
        {
        if( m_Crown_Number >m_ComputedCrownNumber){
        m_Crown_Number = m_ComputedCrownNumber;
        }
        }*/
        InternalUpdate();
        m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
        ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
        m_Gui->Update();
        //GetPolylineOutput()->Update();
        //m_Gui->Update();
      }
      break;
    case ID_CONSTRAINT_SURFACE:
      {
        // search for tagged vme
        mafNode *node = FindTaggedVesselVME(this) ;

        if (node != NULL)
          mafLogMessage("Found vessel ok\n") ;
        else{
          // can't find tagged vme so launch user select
          mafString title = mafString("Select Vessel Vme:");
          mafEvent e(this,VME_CHOOSE);
          e.SetString(&title);
          ForwardUpEvent(e);
          node = e.GetVme();
        }

        if (node != NULL){
          mafVME *vme = (mafVME*)node ;
          vme->Update() ;

          SetAndKeepConstraintSurface(node);

          InternalUpdate();
          m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
          ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
          m_Gui->Enable(ID_DEFORMATION,(m_VesselCenterLineSetFlag)&&(m_ConstraintSurfaceSetFlag));
          m_Gui->Enable(DISPLAY_CATHETER,m_VesselCenterLineSetFlag);
          m_Gui->Update();

          vme->GetTagArray()->SetTag("RT3S_VESSEL","Surface");
        }
      }
      break ;
    case ID_CENTERLINE:
      {
        // search for tagged vme
        mafNode *node = FindTaggedCenterLineVME(this) ;

        if (node != NULL)
          mafLogMessage("Found centerline ok\n") ;
        else{
          // can't find tagged vme so launch user select
          mafString title = mafString("Select Centerline Vme:");
          mafEvent e(this,VME_CHOOSE);
          e.SetString(&title);
          ForwardUpEvent(e);
          node = e.GetVme();
        }

        if (node != NULL){
          mafVME *vme = (mafVME*)node ;
          vme->Update() ;

          m_VesselCenterLineName = vme->GetName();
          if(vme->IsMAFType(medVMEPolylineGraph))
            SetAndKeepCenterLine(node);

          InternalUpdate();
          m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
          ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
          m_Gui->Enable(ID_DEFORMATION,(m_VesselCenterLineSetFlag)&&(m_ConstraintSurfaceSetFlag));
          m_Gui->Enable(DISPLAY_CATHETER,m_VesselCenterLineSetFlag);
          m_Gui->Update();

          vme->GetTagArray()->SetTag("RT3S_CENTER_LINE","Polyline");
        }
      }	
      break;
    case DISPLAY_CATHETER:
      {
        ToggleDisplayCatheter(); //display or remove catheter from output
        m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
        ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
      }	  
      break;
    case ID_DEFORMATION:
      {
        DoDeformation3(0);
        m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
        ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));

        CreateDeployCtrlDialog() ; // launch deployment slider
      }
      break;
    case ID_DEFORMATION_PARAMETERS:
      {
        SaveDefParams() ;

        // Display dialog for deformation parameters
        CreateDefParamsDialog() ;  
        DeleteDefParamsDialog() ;
      }      
      break;
    case ID_DEF_PARAMS_ALPHA:
    case ID_DEF_PARAMS_BETA:
    case ID_DEF_PARAMS_GAMMA:
    case ID_DEF_PARAMS_RIGIDITY:
    case ID_DEF_PARAMS_DAMPING:
    case ID_DEF_PARAMS_EPSILON:
    case ID_DEF_PARAMS_ITS_PER_STEP:
    case ID_DEF_PARAMS_CATHETER_SPEED:
    case ID_DEF_PARAMS_PAUSE_POSITION:
    case ID_DEF_PARAMS_PAUSE_DURATION:
      m_DefParamsDlg->TransferDataFromWindow() ;
      break ;
    case ID_DEF_PARAMS_STEPS:
      m_DefParamsDlg->TransferDataFromWindow() ;
      if (m_DeployCtrlDlg != NULL)
        UpdateDeploymentSliderRange() ;
      break ;
    case ID_DEF_PARAMS_RESTORE:
      SetDefParamsToDefaults() ;
      m_DefParamsDlg->TransferDataToWindow() ;
      break ;
    case ID_DEF_PARAMS_UNDO:
      SetDefParamsToSaved() ;
      m_DefParamsDlg->TransferDataToWindow() ;
      break ;
    case ID_DEF_PARAMS_OK:
      m_DefParamsDlg->EndModal(wxID_OK);
      break ;
    case ID_DEF_PARAMS_CANCEL:
      SetDefParamsToSaved() ;
      m_DefParamsDlg->EndModal(wxID_CANCEL);
      break ;
    case ID_DEPLOY_CTRL_TXT:
    case ID_DEPLOY_CTRL_SLIDER:
      {
        m_DeployCtrlDlg->TransferDataFromWindow() ;
        ClampSliderPos() ;
        m_DeployCtrlDlg->TransferDataToWindow() ;
        DisplayStentExpandByStep((int)m_SliderPos);
        m_DeploymentVisualPipe->Render() ;
      }
      break ;
    case ID_DEPLOY_CTRL_DEC1:
      {
        m_SliderPos -= 1.0 ;
        ClampSliderPos() ;
        m_DeployCtrlDlg->TransferDataToWindow() ;
        DisplayStentExpandByStep((int)m_SliderPos);
        m_DeploymentVisualPipe->Render() ;
      }
      break ;
    case ID_DEPLOY_CTRL_INC1:
      {
        m_SliderPos += 1.0 ;
        ClampSliderPos() ;
        m_DeployCtrlDlg->TransferDataToWindow() ;
        DisplayStentExpandByStep((int)m_SliderPos);
        m_DeploymentVisualPipe->Render() ;
      }
      break ;
    case ID_DEPLOY_SHOW_VESSEL_CHKBOX:
      m_DeploymentVisualPipe->ShowVessel(m_DeployDlg_ShowVessel) ;
      m_DeploymentVisualPipe->Render() ;
      break ;
    case ID_DEPLOY_SHOW_CENTERLINE_CHKBOX:
      m_DeploymentVisualPipe->ShowCenterLine(m_DeployDlg_ShowCenterLine) ;
      m_DeploymentVisualPipe->Render() ;
      break ;
    case ID_DEPLOY_SHOW_CATHETER_CHKBOX:
      m_DeploymentVisualPipe->ShowCatheter(m_DeployDlg_ShowCatheter) ;
      m_DeploymentVisualPipe->Render() ;
      break ;
    case ID_DEPLOY_SHOW_STENT_CHKBOX:
      m_DeploymentVisualPipe->ShowStent(m_DeployDlg_ShowStent) ;
      m_DeploymentVisualPipe->Render() ;
      break ;
    case ID_DEPLOY_CTRL_OK:
      m_DeployCtrlDlg->EndModal(wxID_OK);
      break ;
    case ID_DEPLOY_CTRL_CANCEL:
      m_DeployCtrlDlg->EndModal(wxID_CANCEL);
      break ;
    default:
      mafVME::OnEvent(maf_event);
      break ;
    }//end of switch
  }//end of if
  else{
    Superclass::OnEvent(maf_event);
  }
  /*else if (maf_event->GetChannel()==MCH_DOWN)
  {
  //mafTimeStamp t = m_CurrentTime;	 
  MoveCatheter(m_CurrentTime);
  m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
  ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE)); 
  }*/  

}





//-----------------------------------------------------------------------
// called to prepare the update of the output
//-----------------------------------------------------------------------
void medVMEStent::InternalPreUpdate()
{
  InternalInitialize();

}



//-----------------------------------------------------------------------
// Create polydata stent from current parameters and update
//-----------------------------------------------------------------------
void medVMEStent::InternalUpdate()
{
  if(m_DeformFlag==0 && m_numberOfCycle ==0){
    if(m_VesselCenterLine){	
      m_StentPolyData->Initialize() ;

      //testStent.setCenterLinePolydata(m_VesselCenterLine); //waiting for code updating

      vtkMEDStentModelSource currentStentSource;

      currentStentSource.setStentDiameter(m_Stent_Diameter);
      currentStentSource.setCrownLength(m_Crown_Length);
      currentStentSource.setCrownNumber(m_Crown_Number);

      currentStentSource.setStentConfiguration((enumStCfgType)m_Id_Stent_Configuration);
      currentStentSource.setLinkConnection((enumLinkConType) m_Id_Link_Connection);
      currentStentSource.setLinkOrientation( (enumLinkOrtType)m_Link_orientation);

      currentStentSource.setLinkLength(m_Link_Length);
      currentStentSource.setLinkAlignment(m_Link_Alignment);
      //currentStentSource.setTestValue();
      int linePointNumber = m_VesselCenterLine->GetNumberOfPoints();
      if(m_VesselCenterLine != NULL && linePointNumber>0){
        // set center line of stent and catheter, offset by stent start pos
        m_DeformFilter->GetCatheterCalculator()->CreateTruncatedCenterLine(m_VesselCenterLine, m_StentCenterLine, m_StentStartPosId, 1E6) ;
        m_DeformFilter->GetCatheterCalculator()->SetCenterLine(m_StentCenterLine) ;
        currentStentSource.setCenterLineFromPolyData(m_StentCenterLine); 
        m_ComputedCrownNumber = currentStentSource.computeCrownNumberAfterSetCenterLine();
        if (m_Crown_Number>m_ComputedCrownNumber)
        {
          m_Crown_Number = m_ComputedCrownNumber;
        }
      }
      currentStentSource.setCrownNumber(m_Crown_Number);

      /**/			
      //testStent.setStrutAngle(m_Strut_Angle); ?? a computed value
      currentStentSource.createStent();
      m_Stent_Radius = currentStentSource.getRadius();

      m_SimplexMesh = currentStentSource.simplexMesh;
      m_SimplexMesh->DisconnectPipeline();

      /*--------------------------   output some parameters for deformation ---------------------*/
      m_StrutLength = currentStentSource.getStrutLength();
      m_Link_Length = currentStentSource.getLinkLength();
      //this->SetCenterLocationIdx(currentStentSource.centerLocationIndex.begin());
      this->SetCenterLocationIdxRef(currentStentSource.centerLocationIndex);
      //m_StentSource = currentStentSource;
      /*--------------------------  output finished ----------------------------------------------*/

      static float vertex[3]; 
      //----------stent VTK---------
      // Create the vtkPoints object and set the number of points
      vtkPoints* vpoints = vtkPoints::New();
      vpoints->SetNumberOfPoints(2000);		
      SimplexMeshType::PointsContainer::Pointer sPoints;
      sPoints = m_SimplexMesh->GetPoints();
      for(SimplexMeshType::PointsContainer::Iterator pointIndex = sPoints->Begin(); pointIndex != sPoints->End(); ++pointIndex)
      {
        int idx = pointIndex->Index();
        vtkFloatingPointType * pp = const_cast<vtkFloatingPointType*>(pointIndex->Value().GetDataPointer());
        vpoints->SetPoint(idx,pp);
      }
      vpoints->Squeeze() ;
      m_StentPolyData->SetPoints(vpoints) ;
      vpoints->Delete() ;

      int tindices[2];
      vtkCellArray *lines = vtkCellArray::New() ;
      lines->Allocate(2000) ;  

      m_StrutArray->Initialize() ;
      m_LinkArray->Initialize() ;
      m_StrutArray->Allocate(2000);
      m_LinkArray->Allocate(2000);

      for(StrutIterator iter = currentStentSource.strutsList.begin(); iter !=currentStentSource.strutsList.end(); iter++){
        tindices[0] = iter->startVertex;
        tindices[1] = iter->endVertex;
        lines->InsertNextCell(2, tindices);
        m_StrutArray->InsertNextCell(2,tindices);
      }
      for(StrutIterator iter = currentStentSource.linkList.begin(); iter !=currentStentSource.linkList.end(); iter++){
        tindices[0] = iter->startVertex;
        tindices[1] = iter->endVertex;
        lines->InsertNextCell(2, tindices);
        m_LinkArray->InsertNextCell(2,tindices);
      }
      lines->Squeeze() ;
      m_StentPolyData->SetLines(lines);
      lines->Delete() ;

      m_StentPolyData->Modified();
      m_StentPolyData->Update();
 
      m_AppendPolyData->Update();
      m_AppendPolys->Update();

    }// end of if m_VesselCenterLine
  }//end of if m_numberOfCycle
}//end of method




//------------------------------------------------------------------------------
// Set deformation parameters and run filter
//------------------------------------------------------------------------------
void medVMEStent::DoDeformation3(int type)
{
  if (m_DeformFlag ==0)
  {
    // set the parameters of the deformation filter
    m_DeformFilter->SetInput( m_SimplexMesh );
    m_DeformFilter->SetGradient( NULL);
    m_DeformFilter->SetAlpha(m_Alpha);//0.03 (0.01-0.3)//my version 0.3
    m_DeformFilter->SetBeta(m_Beta);//0.01 (0.01-1) //my version 0.05 //b0.3 does not match a0.3, in a mass
    m_DeformFilter->SetGamma(m_Gamma) ;
    m_DeformFilter->SetRigidity(m_Rigidity); //(1-8 smoother)
    m_DeformFilter->SetDamping(m_Damping) ;
    m_DeformFilter->SetEpsilon(m_Epsilon) ;
    m_DeformFilter->SetIterations(m_IterationsPerStep); //will effect speed
    m_DeformFilter->SetStrutLength(this->GetStrutLength());//(m_StentSource.getStrutLength());
    m_DeformFilter->SetLinkLength(this->GetLinkLength());//(m_StentSource.getLinkLength());
    m_DeformFilter->SetStrutLinkFromCellArray(m_StrutArray,m_LinkArray);
    if (m_ConstraintSurface!=NULL && m_ConstraintSurface->GetNumberOfPoints()>0)
    {			
      m_DeformFilter->SetCenterLocationIdx(m_centerLocation.begin());
      m_DeformFilter->SetVesselPointsKDTreeFromPolyData(m_ConstraintSurface);
    }

    // Set the speed of the catheter withdrawal and insert pause if requested
    m_DeformFilter->GetCatheterCalculator()->SetStartPos(0.0) ;
    m_DeformFilter->GetCatheterCalculator()->SetSpeed(m_CatheterSpeed) ;
    m_DeformFilter->GetCatheterCalculator()->SetCenterLine(m_StentCenterLine) ;
    if (m_CatheterPauseOn == 1)
      m_DeformFilter->GetCatheterCalculator()->SetPauseAtPosition(m_CatheterPausePosition, m_CatheterPauseDuration) ;
    else
      m_DeformFilter->GetCatheterCalculator()->SetPauseToNone() ;

    // set tolerance equal to tube radius
    m_DeformFilter->SetMinDistanceToVessel(m_StentTubeRadius) ;

    // Compute the deformation
    PreComputeStentPointsBySteps(m_NumberOfSteps); //run filter for specified no. of steps

    m_DeformFlag = 1;
  }

  // dump deformation history
  fstream thing ;
  thing.open("thing.csv", thing.out) ;
  m_DeformFilter->GetDeformationHistory()->PrintSelf(thing) ;
  thing.close() ;
}



void medVMEStent::DisplayStentExpand( int steps )
{
  vector<vtkPoints*>::iterator pointsIter = m_ItPointsContainer.begin();
  vtkPoints* currentPoints;
  for (int i=0; pointsIter!=m_ItPointsContainer.end(); ++pointsIter,i++ )
  {
    currentPoints = *pointsIter;
    ResetStentPoints(currentPoints);
    if(m_ShowCatheter){
      MoveCatheter(i);
    }

  }
}



//------------------------------------------------------------------------------
// Add or remove catheter display
//------------------------------------------------------------------------------
void medVMEStent::ToggleDisplayCatheter(){

  if(m_ShowCatheter)
    m_AppendPolyData->AddInput(m_CatheterPolyData);
  else
    m_AppendPolyData->RemoveInput(m_CatheterPolyData);
}



//------------------------------------------------------------------------------
// Set stent points to current and update
//------------------------------------------------------------------------------
void medVMEStent::ResetStentPoints( vtkPoints* currentPoints )
{
  m_StentPolyData->SetPoints(currentPoints);
  m_StentPolyData->Modified();
  m_StentPolyData->Update();

  m_numberOfCycle++;
}



void medVMEStent::UpdateViewAfterDeformation(){

  m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);//must update data
  ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));//must updata camera


}



/*void medVMEStent::DisplayStentExpandByStep(mafTimeStamp t){

if(floor(t)<m_ItPointsContainer.size()){
if(m_ShowCatheter){
MoveCatheter(t*2);
}

vtkPoints* oneStepPoints = m_ItPointsContainer[t];
ResetStentPoints(oneStepPoints);
UpdateViewAfterDeformation();
}

}*/



//-----------------------------------------------------------------------------
// Display stent and catheter for given step
//-----------------------------------------------------------------------------
void medVMEStent::DisplayStentExpandByStep(int step)
{
  if(step < m_ItPointsContainer.size()){// if stent needs expend
    vtkPoints* oneStepPoints = m_ItPointsContainer[step];
    ResetStentPoints(oneStepPoints);
  }

  if(m_ShowCatheter){
    MoveCatheter(step);
  }

  UpdateViewAfterDeformation();
}



//-----------------------------------------------------------------------------
/// Run filter for no. of steps.
/// Store stent position at each step for later display.
//-----------------------------------------------------------------------------
void medVMEStent::PreComputeStentPointsBySteps( int steps){
  //-------------timer--------
  time_t t1,t2;
  (void) time(&t1);
  //-------------timer--------


  m_Gui->Enable(ID_DEFORMATION,false);
  m_Gui->Update();

  for (int i=0; i<steps; i++)
  {
    // one step deformation
    m_DeformFilter->SetCurrentStepNum(i); // used to synchronize expansion with catheter
    m_DeformFilter->Update();

    // get point set from mesh and save
    vtkPoints* vpoints = vtkPoints::New(); // n.b. deleted in destructor with m_ItPointsContainer
    for (int j = 0 ;  j < m_SimplexMesh->GetNumberOfPoints() ;  j++){
      PointType pt ;
      m_SimplexMesh->GetPoint(j,&pt) ;
      double *x = pt.GetDataPointer() ;
      vpoints->InsertNextPoint(x) ;
    }

    // Save current point set for later display
    m_ItPointsContainer.push_back(vpoints);
  }

  //-------------timer--------
  (void) time(&t2);
  int diff =(int)(t2-t1);
  //-------------timer--------

  m_Gui->Enable(ID_DEFORMATION,true);
  m_Gui->Update();

}



//----------------------------------------------------------------------------
// expand stent in a constraint surface 
// for this method, only need to change position of every points, since lines was set.
//----------------------------------------------------------------------------
void medVMEStent::expandStent(int numberOfCycle )
{
  //-------------deform stent---------
  m_DeformFilter->SetCurrentStepNum(numberOfCycle);
  m_DeformFilter->Update();

  //------------update stent visualization----------------
  static float vertex[3]; 

  vtkPoints* vpoints = vtkPoints::New();
  vpoints->SetNumberOfPoints(2000);		
  SimplexMeshType::PointsContainer::Pointer sPoints;
  sPoints = m_SimplexMesh->GetPoints();
  int tmpIdx = 0;
  for(SimplexMeshType::PointsContainer::Iterator pointIndex = sPoints->Begin(); pointIndex != sPoints->End(); ++pointIndex)
  {
    int idx = pointIndex->Index();
    vtkFloatingPointType * pp = const_cast<vtkFloatingPointType*>(pointIndex->Value().GetDataPointer());


    vpoints->SetPoint(idx,pp);
    tmpIdx++;
  }
  vpoints->Squeeze();
  //---------test---------
  int numPoints = vpoints->GetNumberOfPoints();
  double x1Coord[3],x2Coord[3];
  vpoints->GetPoint(0,x1Coord);
  vpoints->GetPoint(tmpIdx-1,x2Coord);
  //---------test end-----

  m_StentPolyData->SetPoints(vpoints);
  m_StentPolyData->Modified();
  m_StentPolyData->Update();
  vpoints->Delete() ;
}



void medVMEStent::createTestVesselPolydata(vtkPolyData  *centerLine){
  int test1 = centerLine->GetNumberOfPoints();
  int test2 = centerLine->GetNumberOfLines();
  vtkTubeFilter *sheath ;
  vtkNEW(sheath);
  //= vtkTubeFilter::New();
  sheath->SetInput(centerLine);//(m_VesselCenterLine);//(m_SheathVTK);
  sheath->SetRadius(m_Stent_Radius +5);
  sheath->SetNumberOfSides(20);
  sheath->Update();

  vtkPolyData *tubePolys = TubeToPolydata(sheath);

  m_TestVesselPolyData->DeepCopy(tubePolys); //important for deforming and expending
  //m_CatheterPolyData->DeepCopy(m_VesselCenterLine);//just for test
  m_TestVesselPolyData->Update();
  //m_AppendPolyData->AddInput(m_TestVesselPolyData);//to show vessel
  int numTmp = m_TestVesselPolyData->GetNumberOfPolys();
}



//----------------------------------------------------------------------------
// Create the catheter in the start position
//----------------------------------------------------------------------------
void medVMEStent::CreateCatheter(vtkPolyData  *centerLine){
  m_DeformFilter->GetCatheterCalculator()->SetCenterLine(centerLine) ;
  m_DeformFilter->GetCatheterCalculator()->CreateTruncatedCenterLine(m_CatheterCenterLine, 0.0) ;

  m_CatheterTubeFilter->SetRadius(m_Stent_Radius) ; // make sure radius is up to date
  m_CatheterPolyData->Update() ;
}



//-----------------------------------------------------------------------------
// move catheter so that stent can expand
//-----------------------------------------------------------------------------
void medVMEStent::MoveCatheter(int step)
{
  m_DeformFilter->GetCatheterCalculator()->CreateTruncatedCenterLine(m_CatheterCenterLine, step) ;
  m_CatheterPolyData->Update() ;
}




/*for Catheter moving*/
void medVMEStent::SetCenterLocationIdxRef(vector<int> const&ve){
  //m_centerLocationIdx = ve.begin();
  m_centerLocation = ve;
}



void medVMEStent::SetCenterLocationIdx(vector<int>::const_iterator centerLocationIndex){
  m_centerLocationIdx = centerLocationIndex;
}



/** give vme a constraint surface */
void medVMEStent::SetConstraintSurface(vtkPolyData *surface){
  if(surface){

    m_ConstraintSurface->DeepCopy(surface);
    m_ConstraintSurface->Update();
    int num = surface->GetNumberOfPoints();
    num = m_ConstraintSurface->GetNumberOfPoints();
    if(num>0){
      m_ConstraintSurfaceSetFlag = 1;
    }
  }
  //vtkNEW(m_ConstraintSurface);
}




//-----------------------------------------------------------------------
// Set the center line and create the catheter
//-----------------------------------------------------------------------
void medVMEStent::SetCenterLine(vtkPolyData *line){
  if(line){
    m_VesselCenterLine->DeepCopy(line) ; //set centerline
    m_VesselCenterLineSetFlag = 1; //set flag

    CreateCatheter(m_VesselCenterLine);		
  }
}




//-----------------------------------------------------------------------
// Store attributes and matrix
//-----------------------------------------------------------------------
int medVMEStent::InternalStore(mafStorageElement *parent)
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    if (parent->StoreMatrix("Transform",&m_Transform->GetMatrix()) != MAF_OK) return MAF_ERROR;

    if (parent->StoreDouble("StentDiameter",m_Stent_Diameter) != MAF_OK) return MAF_ERROR;
    if (parent->StoreDouble("CrownLength",m_Crown_Length) != MAF_OK) return MAF_ERROR;
    if (parent->StoreDouble("CrownNumber",m_Crown_Number) != MAF_OK) return MAF_ERROR;
    if (parent->StoreDouble("StrutThickness",m_Strut_Thickness) != MAF_OK) return MAF_ERROR;
    if (parent->StoreInteger("IdStentConfiguration",m_Id_Stent_Configuration) != MAF_OK) return MAF_ERROR;
    /**  stent link  */
    if (parent->StoreInteger("IdLinkConnection",m_Id_Link_Connection) != MAF_OK) return MAF_ERROR;
    if (parent->StoreDouble("LinkLength",m_Link_Length) != MAF_OK) return MAF_ERROR;
    if (parent->StoreInteger("LinkAlignment",m_Link_Alignment) != MAF_OK) return MAF_ERROR;
    if (parent->StoreInteger("LinkOrientation",m_Link_orientation) != MAF_OK) return MAF_ERROR;
    if (parent->StoreDouble("StentRadius",m_Stent_Radius) != MAF_OK) return MAF_ERROR;
    /**----------- center line and constraint surface-----------*/
    if (parent->StoreText("CenterLineName",m_VesselCenterLineName) != MAF_OK) return MAF_ERROR;
    //if (parent->StoreText("ConstraintSurfaceName",m_ConstraintSurfaceName) != MAF_OK) return MAF_ERROR;
    /*------save centerline vme and surface vme ------*/
    parent->StoreInteger("VmeLinkedListNumberOfElement", m_VmeIdList.size());

    //---------get line point serial------------
    int numOfPoints = m_VesselCenterLine->GetNumberOfPoints();
    if(numOfPoints>0){
      vtkPoints *linePoints = m_VesselCenterLine->GetPoints();
      double p[3];
      vector<double> vertex;
      for(vtkIdType i = 0; i < numOfPoints; i++){
        m_VesselCenterLine->GetPoint(i,p);

        //------for store-----
        m_StentCenterLineSerial.push_back(p[0]);
        m_StentCenterLineSerial.push_back(p[1]);
        m_StentCenterLineSerial.push_back(p[2]);
      }
    }
    //parent->StoreVectorN("VmeLinkedList",m_VmeIdList, m_VmeIdList.size());
    parent->StoreInteger("CenterLineVertex3xNumber",m_StentCenterLineSerial.size());
    parent->StoreVectorN("CenterLineVertex",m_StentCenterLineSerial,m_StentCenterLineSerial.size());

    return MAF_OK;
  }

  return MAF_ERROR;
}



/************************************************************************/
/* restore attributes and matrix*/                                                                     
/************************************************************************/

//-----------------------------------------------------------------------
int medVMEStent::InternalRestore(mafStorageElement *node)
  //-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {

      m_Transform->SetMatrix(matrix); 

      node->RestoreDouble("StentDiameter",m_Stent_Diameter);
      node->RestoreDouble("CrownLength",m_Crown_Length);
      node->RestoreInteger("CrownNumber",m_Crown_Number);
      // node->RestoreDouble("CrownNumber",m_Crown_Number);

      node->RestoreDouble("StrutThickness",m_Strut_Thickness);
      node->RestoreInteger("IdStentConfiguration",m_Id_Stent_Configuration);
      /**  stent link  */
      node->RestoreInteger("IdLinkConnection",m_Id_Link_Connection);
      node->RestoreDouble("LinkLength",m_Link_Length);
      node->RestoreInteger("LinkAlignment",m_Link_Alignment);
      node->RestoreInteger("LinkOrientation",m_Link_orientation);
      node->RestoreDouble("StentRadius",m_Stent_Radius);

      /**----------- center line name and constraint surface name-----------*/
      node->RestoreText("CenterLineName",m_VesselCenterLineName);
      node->RestoreText("ConstraintSurfaceName",m_ConstraintSurfaceName);
      /*---------centerline VME and surface VME -------------*/
      m_VmeIdList.resize(2); //2objects: centerline and surface
      node->RestoreVectorN("VmeLinkedList",m_VmeIdList, m_VmeIdList.size());

      //----------------------------
      int centerLine3xNumber ;
      node->RestoreInteger("CenterLineVertex3xNumber",centerLine3xNumber);
      if(centerLine3xNumber >0){
        m_StentCenterLineSerial.resize(centerLine3xNumber);
        node->RestoreVectorN("CenterLineVertex",m_StentCenterLineSerial,centerLine3xNumber);
        //--------recover centerline with lines---------------
        vtkPolyData *aLine = vtkPolyData::New();
        vtkPoints *centerPoints = vtkPoints::New();
        centerPoints->Allocate(300);
        static double cVertex[3]; //vertex 
        int k=0;
        for(int i=0; i<centerLine3xNumber; i+=3)
        {   
          vector<double> vertex;
          for (int j=0; j<3; j++)
          {
            cVertex[j]= m_StentCenterLineSerial[i+j];
          }
          centerPoints->InsertPoint(k,cVertex);
          k++;
        }
        centerPoints->Squeeze();
        int tindices[2];
        vtkCellArray *cLines = vtkCellArray::New() ;
        int pointNumber = centerLine3xNumber/3;
        cLines->Allocate(pointNumber);
        for(int i=0;i<pointNumber-1;i++){
          tindices[0] = i;
          tindices[1] = i+1;
          cLines->InsertNextCell(2, tindices);
        }		

        cLines->Squeeze() ;
        // m_SheathVTK->SetLines(cLines);
        aLine->SetPoints(centerPoints);
        aLine->SetLines(cLines);
        centerPoints->Delete() ;
        cLines->Delete() ;
        aLine->Update();
        int test1 = aLine->GetNumberOfPoints();
        int test2 = aLine->GetNumberOfPolys();

        //m_VesselCenterLine = aLine;
        this->SetCenterLine(aLine);
        aLine->Delete() ;
      }
      /*----------now recompute -----------*/
      InternalUpdate();
      /*--finished--*/

      return MAF_OK;
    }
  }

  return MAF_ERROR;
}



/************************************************************************/
/* update data and return append polydata */                                                                     
/************************************************************************/

vtkPolyData* medVMEStent::GetPolyData()
{ 
  InternalUpdate();
  return  m_AppendPolys;
}



/************************************************************************/
/* icon file */                                                                     
/************************************************************************/
//-------------------------------------------------------------------------
char** medVMEStent::GetIcon() 
  //-------------------------------------------------------------------------
{

#include "mafVMEVolumeLarge.xpm"
  return mafVMEVolumeLarge_xpm;
}



/************************************************************************/
/* convert tube strips into triangle polys */                                                                     
/************************************************************************/
vtkPolyData* medVMEStent::TubeToPolydata( vtkTubeFilter * sheath )
{
  /**-------------create tube------------------*/
  // Create a tube (cylinder) around the line
  vtkPolyData *tubeStrips = sheath->GetOutput();
  vtkCellArray *strips = tubeStrips->GetStrips();  
  vtkIdType number = strips->GetNumberOfCells();
  int iStrips = tubeStrips->GetNumberOfStrips();

  //tubeFilter->Update();
  //tubeFilter->get
  /**------------to polydata----------------*/
  vtkTriangleFilter *tubeTriangles = vtkTriangleFilter::New();
  tubeTriangles->SetInput(sheath->GetOutput());
  tubeTriangles->Update();

  vtkPolyData *tubePolys = tubeTriangles->GetOutput();
  tubePolys = tubeTriangles->GetOutput();
  tubePolys->BuildCells();
  tubePolys->Update();
  int num1 = tubePolys->GetNumberOfPoints();
  return tubePolys;
}




//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void medVMEStent::SetAndKeepCenterLine( mafNode * node )
{
  if(node){
    mafVME *vme = mafVME::SafeDownCast(node);
    vtkPolyData *polyLine =vtkPolyData::SafeDownCast( vme->GetOutput()->GetVTKData());
    polyLine->Update();
    this->SetCenterLine(polyLine);
    SetLink("CenterLineVME", node);  // mafNode method - link to another node.
    this->m_VesselCenterLineName = vme->GetName();

    // Save the vme id
    m_VmeIdList.insert(m_VmeIdList.begin(),vme->GetId());
  }
}



//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void medVMEStent::SetAndKeepConstraintSurface( mafNode * node )
{
  if(node){
    mafVME *vme = mafVME::SafeDownCast(node);
    vtkPolyData *polySurface = vtkPolyData::SafeDownCast(vme->GetOutput()->GetVTKData());
    polySurface->Update();
    vtkIdType pointsNumber = polySurface->GetNumberOfPoints();
    //this->SetConstraintSurface(polySurface);
    //temp
    this->createTestVesselPolydata(m_VesselCenterLine);
    this->SetConstraintSurface(m_TestVesselPolyData);
    //temp finish

    SetLink("ConstraintSurfaceVME", node);
    this->m_ConstraintSurfaceName = vme->GetName();

    // Save the vme id
    m_VmeIdList.insert(m_VmeIdList.begin()+1,vme->GetId());
  }
}



//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
mmaMaterial* medVMEStent::GetMaterial()
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();

    material->m_Prop->SetOpacity(0.4);
    material->m_Opacity = material->m_Prop->GetOpacity();
    material->m_Prop->SetDiffuse(0.85);
    material->m_DiffuseIntensity=material->m_Prop->GetDiffuse();
    material->m_Prop->SetSpecularPower(75);
    material->m_SpecularPower = material->m_Prop->GetSpecularPower();
    material->m_Prop->SetSpecular(0.4);
    material->m_SpecularIntensity = material->m_Prop->GetSpecular();
    material->m_Prop->SetLineWidth(3.0);

    SetAttribute("MaterialAttributes", material);
  }
  return material;
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
// Find the tagged constraintt surface vme.
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
// Create deformation parameters dialog
//-------------------------------------------------------------------------
void medVMEStent::CreateDefParamsDialog()
{
  // create the dialog
  m_DefParamsDlg = new mafGUIDialog("Deformation Parameters", mafCLOSEWINDOW | mafRESIZABLE); 

  // vertical stacker for the rows of widgets
  wxBoxSizer *vs1 = new wxBoxSizer(wxVERTICAL);


  // deformation heading
  wxStaticText* deformationHeading = new wxStaticText(m_DefParamsDlg, wxID_ANY, "Deformation params", wxPoint(0,0), wxSize(150,20));
  vs1->Add(deformationHeading, 0, wxALIGN_LEFT | wxALL, 2);

  // alpha
  wxStaticText* labelAlpha = new wxStaticText(m_DefParamsDlg, wxID_ANY, "Alpha", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl* txtCtrlAlpha = new wxTextCtrl(m_DefParamsDlg , ID_DEF_PARAMS_ALPHA, "", wxPoint(0,0), wxSize(150,20), wxNO_BORDER);
  txtCtrlAlpha->SetValidator(mafGUIValidator(this, ID_DEF_PARAMS_ALPHA, txtCtrlAlpha, &m_Alpha)) ;

  wxBoxSizer *alphaBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  alphaBoxSizer->Add(labelAlpha,0);
  alphaBoxSizer->Add(txtCtrlAlpha,0);
  vs1->Add(alphaBoxSizer,0,wxALIGN_CENTER | wxALL, 2);

  // beta
  wxStaticText* labelBeta = new wxStaticText(m_DefParamsDlg, wxID_ANY, "Beta", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl* txtCtrlBeta = new wxTextCtrl(m_DefParamsDlg , ID_DEF_PARAMS_BETA, "", wxPoint(0,0), wxSize(150,20), wxNO_BORDER);
  txtCtrlBeta->SetValidator(mafGUIValidator(this, ID_DEF_PARAMS_BETA, txtCtrlBeta, &m_Beta)) ;

  wxBoxSizer *betaBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  betaBoxSizer->Add(labelBeta,0);
  betaBoxSizer->Add(txtCtrlBeta,0);
  vs1->Add(betaBoxSizer,0,wxALIGN_CENTER | wxALL, 2);

  // gamma
  wxStaticText* labelGamma = new wxStaticText(m_DefParamsDlg, wxID_ANY, "Gamma", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl* txtCtrlGamma = new wxTextCtrl(m_DefParamsDlg , ID_DEF_PARAMS_GAMMA, "", wxPoint(0,0), wxSize(150,20), wxNO_BORDER);
  txtCtrlGamma->SetValidator(mafGUIValidator(this, ID_DEF_PARAMS_GAMMA, txtCtrlGamma, &m_Gamma)) ;

  wxBoxSizer *gammaBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  gammaBoxSizer->Add(labelGamma,0);
  gammaBoxSizer->Add(txtCtrlGamma,0);
  vs1->Add(gammaBoxSizer,0,wxALIGN_CENTER | wxALL, 2);

  // rigidity
  wxStaticText* labelRigidity = new wxStaticText(m_DefParamsDlg, wxID_ANY, "Rigidity", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl* txtCtrlRigidity = new wxTextCtrl(m_DefParamsDlg , ID_DEF_PARAMS_RIGIDITY, "", wxPoint(0,0), wxSize(150,20), wxNO_BORDER);
  txtCtrlRigidity->SetValidator(mafGUIValidator(this, ID_DEF_PARAMS_RIGIDITY, txtCtrlRigidity, &m_Rigidity)) ;

  wxBoxSizer *rigidityBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  rigidityBoxSizer->Add(labelRigidity,0);
  rigidityBoxSizer->Add(txtCtrlRigidity,0);
  vs1->Add(rigidityBoxSizer,0,wxALIGN_CENTER | wxALL, 2);


  // damping
  wxStaticText* labelDamping = new wxStaticText(m_DefParamsDlg, wxID_ANY, "Damping", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl* txtCtrlDamping = new wxTextCtrl(m_DefParamsDlg , ID_DEF_PARAMS_DAMPING, "", wxPoint(0,0), wxSize(150,20), wxNO_BORDER);
  txtCtrlDamping->SetValidator(mafGUIValidator(this, ID_DEF_PARAMS_DAMPING, txtCtrlDamping, &m_Damping)) ;

  wxBoxSizer *dampingBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  dampingBoxSizer->Add(labelDamping,0);
  dampingBoxSizer->Add(txtCtrlDamping,0);
  vs1->Add(dampingBoxSizer,0,wxALIGN_CENTER | wxALL, 2);


  // epsilon
  wxStaticText* labelEpsilon = new wxStaticText(m_DefParamsDlg, wxID_ANY, "Epsilon", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl* txtCtrlEpsilon = new wxTextCtrl(m_DefParamsDlg , ID_DEF_PARAMS_EPSILON, "", wxPoint(0,0), wxSize(150,20), wxNO_BORDER);
  txtCtrlEpsilon->SetValidator(mafGUIValidator(this, ID_DEF_PARAMS_EPSILON, txtCtrlEpsilon, &m_Epsilon)) ;

  wxBoxSizer *epsilonBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  epsilonBoxSizer->Add(labelEpsilon,0);
  epsilonBoxSizer->Add(txtCtrlEpsilon,0);
  vs1->Add(epsilonBoxSizer,0,wxALIGN_CENTER | wxALL, 2);


  // itsPerStep
  wxStaticText* labelItsPerStep = new wxStaticText(m_DefParamsDlg, wxID_ANY, "Its per step", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl* txtCtrlItsPerStep = new wxTextCtrl(m_DefParamsDlg , ID_DEF_PARAMS_ITS_PER_STEP, "", wxPoint(0,0), wxSize(150,20), wxNO_BORDER);
  txtCtrlItsPerStep->SetValidator(mafGUIValidator(this, ID_DEF_PARAMS_ITS_PER_STEP, txtCtrlItsPerStep, &m_IterationsPerStep)) ;

  wxBoxSizer *itsPerStepBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  itsPerStepBoxSizer->Add(labelItsPerStep,0);
  itsPerStepBoxSizer->Add(txtCtrlItsPerStep,0);
  vs1->Add(itsPerStepBoxSizer, 0, wxALIGN_CENTER | wxALL, 2);


  // steps
  wxStaticText* labelSteps = new wxStaticText(m_DefParamsDlg, wxID_ANY, "Steps", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl* txtCtrlSteps = new wxTextCtrl(m_DefParamsDlg , ID_DEF_PARAMS_STEPS, "", wxPoint(0,0), wxSize(150,20), wxNO_BORDER);
  txtCtrlSteps->SetValidator(mafGUIValidator(this, ID_DEF_PARAMS_STEPS, txtCtrlSteps, &m_NumberOfSteps)) ;

  wxBoxSizer *stepsBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  stepsBoxSizer->Add(labelSteps,0);
  stepsBoxSizer->Add(txtCtrlSteps,0);
  vs1->Add(stepsBoxSizer, 0, wxALIGN_CENTER | wxALL, 2);


  // catheter heading
  wxStaticText* catheterHeading = new wxStaticText(m_DefParamsDlg, wxID_ANY, "Catheter params", wxPoint(0,0), wxSize(150,20));
  vs1->Add(catheterHeading, 0, wxALIGN_LEFT | wxALL, 2);


  // catheter pause
  m_PauseChkBox = new wxCheckBox(m_DefParamsDlg, ID_DEF_PARAMS_PAUSE_CHKBOX, "Pause", wxPoint(0,0), wxSize(100,20));
  m_PauseChkBox->SetValidator(mafGUIValidator(this, ID_DEF_PARAMS_PAUSE_CHKBOX, m_PauseChkBox, &m_CatheterPauseOn));
  m_PauseChkBox->Enable(true) ;
  m_PauseChkBox->SetValue(false) ;
  vs1->Add(m_PauseChkBox, 0, wxALIGN_LEFT | wxALL, 2);


  // catheter speed
  wxStaticText* labelSpeed = new wxStaticText(m_DefParamsDlg, wxID_ANY, "Speed", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl* txtCtrlSpeed = new wxTextCtrl(m_DefParamsDlg , ID_DEF_PARAMS_CATHETER_SPEED, "", wxPoint(0,0), wxSize(150,20), wxNO_BORDER);
  txtCtrlSpeed->SetValidator(mafGUIValidator(this, ID_DEF_PARAMS_CATHETER_SPEED, txtCtrlSpeed, &m_CatheterSpeed)) ;

  wxBoxSizer *speedBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  speedBoxSizer->Add(labelSpeed,0);
  speedBoxSizer->Add(txtCtrlSpeed,0);
  vs1->Add(speedBoxSizer, 0, wxALIGN_CENTER | wxALL, 2);


  // catheter pause position
  wxStaticText* labelPausePos = new wxStaticText(m_DefParamsDlg, wxID_ANY, "Pause Position", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl* txtCtrlPausePos = new wxTextCtrl(m_DefParamsDlg , ID_DEF_PARAMS_PAUSE_POSITION, "", wxPoint(0,0), wxSize(150,20), wxNO_BORDER);
  txtCtrlPausePos->SetValidator(mafGUIValidator(this, ID_DEF_PARAMS_PAUSE_POSITION, txtCtrlPausePos, &m_CatheterPausePosition)) ;

  wxBoxSizer *pausePosBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  pausePosBoxSizer->Add(labelPausePos,0);
  pausePosBoxSizer->Add(txtCtrlPausePos,0);
  vs1->Add(pausePosBoxSizer, 0, wxALIGN_CENTER | wxALL, 2);


  // catheter pause duration
  wxStaticText* labelPauseDur = new wxStaticText(m_DefParamsDlg, wxID_ANY, "Pause Duration", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl* txtCtrlPauseDur = new wxTextCtrl(m_DefParamsDlg , ID_DEF_PARAMS_PAUSE_DURATION, "", wxPoint(0,0), wxSize(150,20), wxNO_BORDER);
  txtCtrlPauseDur->SetValidator(mafGUIValidator(this, ID_DEF_PARAMS_PAUSE_DURATION, txtCtrlPauseDur, &m_CatheterPauseDuration)) ;

  wxBoxSizer *pauseDurBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  pauseDurBoxSizer->Add(labelPauseDur,0);
  pauseDurBoxSizer->Add(txtCtrlPauseDur,0);
  vs1->Add(pauseDurBoxSizer, 0, wxALIGN_CENTER | wxALL, 2);


  // restore and undo buttons
  mafGUIButton* buttonRestore = new mafGUIButton(m_DefParamsDlg, ID_DEF_PARAMS_RESTORE, "Restore", wxPoint(0,0), wxSize(50,20));
  buttonRestore->SetListener(this);

  mafGUIButton* buttonUndo = new mafGUIButton(m_DefParamsDlg, ID_DEF_PARAMS_UNDO, "Undo", wxPoint(0,0), wxSize(50,20));
  buttonUndo->SetListener(this);

  wxBoxSizer *restoreBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  restoreBoxSizer->Add(buttonRestore,0);
  restoreBoxSizer->Add(buttonUndo,0);
  vs1->Add(restoreBoxSizer,0,wxALIGN_CENTER | wxALL, 2);


  // ok/cancel buttons
  mafGUIButton* buttonOk = new mafGUIButton(m_DefParamsDlg, ID_DEF_PARAMS_OK, "Ok", wxPoint(0,0), wxSize(50,20));
  buttonOk->SetListener(this);

  mafGUIButton *buttonCancel = new mafGUIButton(m_DefParamsDlg, ID_DEF_PARAMS_CANCEL, "Cancel", wxPoint(0,0), wxSize(50,20));
  buttonCancel->SetListener(this);

  wxBoxSizer *okCancelBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  okCancelBoxSizer->Add(buttonOk,0);
  okCancelBoxSizer->Add(buttonCancel,0);
  vs1->Add(okCancelBoxSizer,0,wxALIGN_CENTER | wxALL, 2);


  // put the vertical sizer into the dialog and display
  vs1->Fit(m_DefParamsDlg);	  // fit the window to the min size of the sizer
  m_DefParamsDlg->Add(vs1) ;  // plug the sizer into the dialog
  m_DefParamsDlg->ShowModal();
}



//-------------------------------------------------------------------------
// Delete deformation parameters dialog
//-------------------------------------------------------------------------
void medVMEStent::DeleteDefParamsDialog()
{
  delete m_DefParamsDlg ;
  m_DefParamsDlg = NULL ;
}



//-------------------------------------------------------------------------
// Restore deformation parameters to defaults
//-------------------------------------------------------------------------
void medVMEStent::SetDefParamsToDefaults()
{
  m_Alpha = m_Alpha_Default ;
  m_Beta = m_Beta_Default ;
  m_Gamma = m_Gamma_Default ;
  m_Rigidity = m_Rigidity_Default ;
  m_Damping = m_Damping_Default ;
  m_Epsilon = m_Epsilon_Default ;
  m_IterationsPerStep = m_IterationsPerStep_Default ;
  m_NumberOfSteps = m_NumberOfSteps_Default ;
  m_CatheterSpeed = m_CatheterSpeed_Default ;
  m_CatheterPauseOn = m_CatheterPauseOn_Default ;
  m_CatheterPausePosition = m_CatheterPausePosition_Default ;
  m_CatheterPauseDuration = m_CatheterPauseDuration_Default ;

  SaveDefParams() ;
}


//-------------------------------------------------------------------------
// Restore deformation parameters to saved values
//-------------------------------------------------------------------------
void medVMEStent::SetDefParamsToSaved()
{
  m_Alpha = m_Alpha_Saved ;
  m_Beta = m_Beta_Saved ;
  m_Gamma = m_Gamma_Saved ;
  m_Rigidity = m_Rigidity_Saved ;
  m_Damping = m_Damping_Saved ;
  m_Epsilon = m_Epsilon_Saved ;
  m_IterationsPerStep = m_IterationsPerStep_Saved ;
  m_NumberOfSteps = m_NumberOfSteps_Saved ;
  m_CatheterSpeed = m_CatheterSpeed_Saved ;
  m_CatheterPauseOn = m_CatheterPauseOn_Saved ;
  m_CatheterPausePosition = m_CatheterPausePosition_Saved ;
  m_CatheterPauseDuration = m_CatheterPauseDuration_Saved ;
}


//-------------------------------------------------------------------------
// Save current deformation parameters (in case of undo or cancel)
//-------------------------------------------------------------------------
void medVMEStent::SaveDefParams()
{
  m_Alpha_Saved = m_Alpha ;
  m_Beta_Saved = m_Beta ;
  m_Gamma_Saved = m_Gamma ;
  m_Rigidity_Saved = m_Rigidity ;
  m_Damping_Saved = m_Damping ;
  m_Epsilon_Saved = m_Epsilon ;
  m_IterationsPerStep_Saved = m_IterationsPerStep ;
  m_NumberOfSteps_Saved = m_NumberOfSteps ;
  m_CatheterSpeed_Saved = m_CatheterSpeed ;
  m_CatheterPauseOn_Saved = m_CatheterPauseOn ;
  m_CatheterPausePosition_Saved = m_CatheterPausePosition ;
  m_CatheterPauseDuration_Saved = m_CatheterPauseDuration ;
}




//-------------------------------------------------------------------------
// Create deployment control dialog
//-------------------------------------------------------------------------
void medVMEStent::CreateDeployCtrlDialog()
{
  // create the dialog
  m_DeployCtrlDlg = new mafGUIDialog("Deployment Control", mafCLOSEWINDOW | mafRESIZABLE); 
  m_DeployCtrlDlg->SetSize(800,50) ;
  m_DeployCtrlDlg->SetPosition(wxPoint(200,200)) ;

  // vertical stacker for the rows of widgets
  wxBoxSizer *vs1 = new wxBoxSizer(wxVERTICAL);


  // set up render window
  m_DeployRwi = new mafRWI(m_DeployCtrlDlg,ONE_LAYER,false);
  m_DeployRwi->SetListener(this);
  m_DeployRwi->CameraSet(CAMERA_PERSPECTIVE);
  m_DeployRwi->m_RenderWindow->SetDesiredUpdateRate(0.0001f);
  m_DeployRwi->SetSize(0,0,800,500);
  m_DeployRwi->m_RenFront->SetBackground(0.7, 0.7, 0.7) ;
  m_DeployRwi->Show(true);

  // Create visual pipe
  m_DeploymentVisualPipe = new medVMEStentDeploymentVisualPipe(m_DeployRwi->m_RenFront) ;
  m_DeploymentVisualPipe->SetCatheter(m_CatheterPolyData) ;
  m_DeploymentVisualPipe->SetCenterLine(m_VesselCenterLine) ;
  m_DeploymentVisualPipe->SetVessel(m_TestVesselPolyData) ;
  m_DeploymentVisualPipe->SetStent(m_StentPolyData) ;
  m_DeploymentVisualPipe->SetStentTubeRadius(m_StentTubeRadius) ;
  m_DeployRwi->m_RenFront->ResetCamera() ;
  m_DeploymentVisualPipe->ResetCameraPosition() ;
  m_DeploymentVisualPipe->Render() ;


  // set vtk interactor style so that vtk widgets can work
  vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New() ;
  m_DeployRwi->m_RenFront->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style) ;
  style->Delete() ;

  vs1->Add(m_DeployRwi->m_RwiBase, 1, wxEXPAND | wxALL, 5) ; 


  // slider and text ctrl
  wxTextCtrl* txtCtrlPos = new wxTextCtrl(m_DeployCtrlDlg , ID_DEPLOY_CTRL_TXT, "", wxPoint(0,0), wxSize(80,20), wxNO_BORDER);
  txtCtrlPos->SetValidator(mafGUIValidator(this, ID_DEPLOY_CTRL_TXT, txtCtrlPos, &m_SliderPos)) ;

  m_SliderPos = 0.0 ;
  m_DeploymentSlider = new mafGUIFloatSlider(m_DeployCtrlDlg, ID_DEPLOY_CTRL_SLIDER, 0, 0, m_NumberOfSteps) ;
  m_DeploymentSlider->SetValidator(mafGUIValidator(this, ID_DEPLOY_CTRL_SLIDER, m_DeploymentSlider, &m_SliderPos, txtCtrlPos)) ;
  m_DeploymentSlider->SetSize(600,20) ;
  m_DeploymentSlider->SetMinSize(wxSize(600,20)) ;
  m_DeploymentSlider->SetNumberOfSteps(m_NumberOfSteps) ;

  mafGUIButton* buttonDec1 = new mafGUIButton(m_DeployCtrlDlg, ID_DEPLOY_CTRL_DEC1, "<", wxPoint(0,0), wxSize(50,20));
  buttonDec1->SetListener(this);

  mafGUIButton* buttonInc1 = new mafGUIButton(m_DeployCtrlDlg, ID_DEPLOY_CTRL_INC1, ">", wxPoint(0,0), wxSize(50,20));
  buttonInc1->SetListener(this);

  wxBoxSizer *sliderBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  sliderBoxSizer->Add(m_DeploymentSlider, 0, wxEXPAND | wxALL, 5);
  sliderBoxSizer->Add(txtCtrlPos,0);
  sliderBoxSizer->Add(buttonDec1,0);
  sliderBoxSizer->Add(buttonInc1,0);

  vs1->Add(sliderBoxSizer,0,wxALIGN_LEFT | wxALL, 2);



  // Show Vessel
  m_ShowVesselChkBox = new wxCheckBox(m_DeployCtrlDlg, ID_DEPLOY_SHOW_VESSEL_CHKBOX, "Show Vessel", wxPoint(0,0), wxSize(100,20));
  m_ShowVesselChkBox->SetValidator(mafGUIValidator(this, ID_DEPLOY_SHOW_VESSEL_CHKBOX, m_ShowVesselChkBox, &m_DeployDlg_ShowVessel));
  m_ShowVesselChkBox->Enable(true) ;
  m_ShowVesselChkBox->SetValue(false) ;

  // Show CenterLine
  m_ShowCenterLineChkBox = new wxCheckBox(m_DeployCtrlDlg, ID_DEPLOY_SHOW_CENTERLINE_CHKBOX, "Show CenterLine", wxPoint(0,0), wxSize(100,20));
  m_ShowCenterLineChkBox->SetValidator(mafGUIValidator(this, ID_DEPLOY_SHOW_CENTERLINE_CHKBOX, m_ShowCenterLineChkBox, &m_DeployDlg_ShowCenterLine));
  m_ShowCenterLineChkBox->Enable(true) ;
  m_ShowCenterLineChkBox->SetValue(false) ;

  // Show Catheter
  m_ShowCatheterChkBox = new wxCheckBox(m_DeployCtrlDlg, ID_DEPLOY_SHOW_CATHETER_CHKBOX, "Show Catheter", wxPoint(0,0), wxSize(100,20));
  m_ShowCatheterChkBox->SetValidator(mafGUIValidator(this, ID_DEPLOY_SHOW_CATHETER_CHKBOX, m_ShowCatheterChkBox, &m_DeployDlg_ShowCatheter));
  m_ShowCatheterChkBox->Enable(true) ;
  m_ShowCatheterChkBox->SetValue(false) ;

  // Show Stent
  m_ShowStentChkBox = new wxCheckBox(m_DeployCtrlDlg, ID_DEPLOY_SHOW_STENT_CHKBOX, "Show Stent", wxPoint(0,0), wxSize(100,20));
  m_ShowStentChkBox->SetValidator(mafGUIValidator(this, ID_DEPLOY_SHOW_STENT_CHKBOX, m_ShowStentChkBox, &m_DeployDlg_ShowStent));
  m_ShowStentChkBox->Enable(true) ;
  m_ShowStentChkBox->SetValue(false) ;

  wxBoxSizer *showComponentsBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  showComponentsBoxSizer->Add(m_ShowVesselChkBox,0);
  showComponentsBoxSizer->Add(m_ShowCenterLineChkBox,0);
  showComponentsBoxSizer->Add(m_ShowCatheterChkBox,0);
  showComponentsBoxSizer->Add(m_ShowStentChkBox,0);

  vs1->Add(showComponentsBoxSizer, 0, wxALIGN_LEFT | wxALL, 2);



  // ok/cancel buttons
  mafGUIButton* buttonOk = new mafGUIButton(m_DeployCtrlDlg, ID_DEPLOY_CTRL_OK, "Ok", wxPoint(0,0), wxSize(50,20));
  buttonOk->SetListener(this);

  mafGUIButton *buttonCancel = new mafGUIButton(m_DeployCtrlDlg, ID_DEPLOY_CTRL_CANCEL, "Cancel", wxPoint(0,0), wxSize(50,20));
  buttonCancel->SetListener(this);

  wxBoxSizer *okCancelBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  okCancelBoxSizer->Add(buttonOk,0);
  okCancelBoxSizer->Add(buttonCancel,0);
  vs1->Add(okCancelBoxSizer, 0, wxALIGN_LEFT | wxALL, 2);


  // put the vertical sizer into the dialog and display
  vs1->Fit(m_DeployCtrlDlg);	  // fit the window to the min size of the sizer
  m_DeployCtrlDlg->Add(vs1) ;  // plug the sizer into the dialog
  m_DeployCtrlDlg->ShowModal();
}



//-------------------------------------------------------------------------
// Delete deployment control dialog
//-------------------------------------------------------------------------
void medVMEStent::DeleteDeployCtrlDialog()
{
  delete m_DeployCtrlDlg ;
  m_DeployCtrlDlg = NULL ;

  delete m_DeploymentVisualPipe ;
  m_DeploymentVisualPipe = NULL ;
}



//-------------------------------------------------------------------------
// Update range of deployment slider
//-------------------------------------------------------------------------
void medVMEStent::UpdateDeploymentSliderRange()
{
  m_SliderPos = 0.0 ;
  m_DeploymentSlider->SetRange(0.0, m_NumberOfSteps) ;
  m_DeployCtrlDlg->TransferDataToWindow() ;
}


//-------------------------------------------------------------------------
// clamp position to range of slider
//-------------------------------------------------------------------------
void medVMEStent::ClampSliderPos()
{
  // round to integer if in deformation mode
  if (m_DeploymentMode == DEPLOYMENT_MODE_DEFORM){
    m_SliderPos = (int)(m_SliderPos + 0.5) ;
  }

  if (m_SliderPos < m_DeploymentSlider->GetMin())
    m_SliderPos = m_DeploymentSlider->GetMin() ;
  if (m_SliderPos > m_DeploymentSlider->GetMax())
    m_SliderPos = m_DeploymentSlider->GetMax() ;
}
