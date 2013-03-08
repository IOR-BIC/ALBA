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

#include "vtkMEDDeformableSimplexMeshFilter.h"


#include "mmuIdFactory.h"
#include "mafDataVector.h"
#include "mafMatrixInterpolator.h"
#include "mafDataPipeInterpolator.h"
#include "mafTagArray.h"
#include "mafMatrixVector.h"
#include "mafVMEItemVTK.h"
#include "mafEventSource.h"
#include "mafTransform.h"
#include "mafGUI.h"
#include "mmaMaterial.h"
#include "mafVMEOutputSurface.h"
#include "mafVMEOutputPolyline.h"
#include "mafDataPipeCustom.h"
#include "medVMEPolylineGraph.h"

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


#include <vtkSmartPointer.h>

/**------new include */
#include <iostream>
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
#include "mafVMEStorage.h"
#include <time.h>



/**----------------------------------------------------------------------------*/
// define:
/**----------------------------------------------------------------------------*/

typedef itk::DeformableSimplexMeshFilterImpl<SimplexMeshType,SimplexMeshType> DeformFilterType;
DeformFilterType::Pointer deformFilter;
static int numberOfCycle = 0;

//------------------------------------------------------------------------------
mafCxxTypeMacro(medVMEStent);
//------------------------------------------------------------------------------
/**  construct method*/
//------------------------------------------------------------------------------
medVMEStent::medVMEStent()
//-------------------------------------------------------------------------
{
	
	vtkNEW(m_PolyData);
	vtkNEW(m_CatheterPolyData);
	vtkNEW(m_TestVesselPolyData);
	//vtkNEW(m_ConstrainSurfaceTmp);
	
	vtkNEW(m_AppendPolyData);
	

	vtkNEW(m_Centerline);
	vtkNEW(m_ConstrainSurface);

	mafNEW(m_Transform);
	
	
	m_Stent_Diameter = 2.0;
	m_Crown_Length = 2.2;
	m_Crown_Number = 10;
	m_Strut_Angle = 0.0;
	m_Strut_Thickness = 0.0;
	m_Id_Stent_Configuration = 1;/* 1.outofphase, 0.InPhase;  enumStCfgType */
	m_Id_Link_Connection = 2; /* 0.peak2valley;enumLinkConType {peak2valley, valley2peak, peak2peak, valley2valley} */
	m_Link_orientation = 0;
	m_Link_Length = 3.0;
	m_Link_Alignment = 0;
	
	m_CenterLineName = "";
	m_ConstrainSurfaceName = "";
	m_CenterLineSetFlag = 0; 
	m_ConstrainSurfaceSetFlag = 0;
	m_ComputedCrownNumber = 0;
	
	m_DeformFlag =0;
	m_ShowCatheter = false;
	m_numberOfCycle = 0;
	
	//mafVMEOutputSurface *output=mafVMEOutputSurface::New(); // an output with no data
	mafVMEOutputPolyline *output=mafVMEOutputPolyline::New(); // an output with no data
	output->SetTransform(m_Transform); // force my transform in the output
	SetOutput(output);

	GetMaterial();
	
	m_AppendPolyData->AddInput(m_PolyData);
	//m_AppendPolyData->AddInput(m_CatheterPolyData); //temp remove add when centerline was set
	//m_AppendPolyData->AddInput(m_ConstrainSurfaceTmp);

	m_AppendPolys = m_AppendPolyData->GetOutput();

	mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
	
	/** stent,catheter and surface  */
	dpipe->SetInput(m_AppendPolys);
	//dpipe->SetInput(m_PolyData); //temp
	SetDataPipe(dpipe);
}
/**  destruction method*/
//-------------------------------------------------------------------------
medVMEStent::~medVMEStent()
//-------------------------------------------------------------------------
{
	vtkDEL(m_PolyData);
	vtkDEL(m_AppendPolyData);
	vtkDEL(m_AppendPolys);
	mafDEL(m_Transform);
	//--remove stent cache-
	vector<vtkPoints*>::iterator pointsIter = m_ItPointsContainer.begin();
	
	for (; pointsIter!=m_ItPointsContainer.end(); ++pointsIter )
	{
		vtkPoints* current = *pointsIter;
		vtkDEL( current);  
	}
	m_ItPointsContainer.clear();
	
	SetOutput(NULL);
}

/**  construct method*/
//-------------------------------------------------------------------------
mafVMEOutputPolyline *medVMEStent::GetPolylineOutput()
	//-------------------------------------------------------------------------
{
	return (mafVMEOutputPolyline *)GetOutput();
	//return (mafVMEOutputPolyline *)m_PolyData;
}

//-----------------------------------------------------------------------
void medVMEStent::SetTimeStamp(mafTimeStamp t)
	//-----------------------------------------------------------------------
{
	t = t < 0 ? 0 : t;
	bool update_vtk_data = t != m_CurrentTime;
	if (update_vtk_data)
	{
		Superclass::SetTimeStamp(t);

		DisplayStentExpendByStep(t);

	}
}

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

	GetMaterial();
	
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
	m_StentRadius = vmeStent->m_StentRadius;
	/**----------- center line and constrain surface-----------*/
	m_CenterLineName = vmeStent->m_CenterLineName;
	m_ConstrainSurfaceName = vmeStent->m_ConstrainSurfaceName;

	vtkNEW(m_Centerline);
	vtkNEW(m_ConstrainSurface);

	m_Centerline = vtkPolyData::New();
	m_Centerline->DeepCopy(vmeStent->GetCenterLine());


	/**-------- compute and update---------------*/

	InternalUpdate();
	mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
	if (dpipe)
    {
	  //dpipe->SetInput(m_PolyData);
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
	return true;
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

		m_Gui->Double(CHANGE_VALUE,_("Diameter"), &m_Stent_Diameter, 0, 10000,-1,_("The length of the stent (mm)"));
		m_Gui->Double(CHANGE_VALUE,_("Crown Len"), &m_Crown_Length, 0, 10000,-1,_("The length of the Crown (mm)"));
		m_Gui->Integer(CHANGE_VALUE,_("Crown num"), &m_Crown_Number, 0, 10000,-1,_("The number of the Crowns"));
		//m_Gui->Double(CHANGE_VALUE,_("Angle"), &m_Strut_Angle,0,360,-1,_("strut angle (deg)"));
		//m_Gui->Double(CHANGE_VALUE,_("Thickness"), &m_Strut_Thickness,0,10,-1,_("strut thickness (mm)"));

		m_Gui->Divider(2);
		wxString stentConfiguration[2] = {"in phase", "out of phase"};
		m_Gui->Combo(CHANGE_VALUE, "Config.", &m_Id_Stent_Configuration, 2, stentConfiguration);

		m_Gui->Divider(2);   
		m_Gui->Label("Link");
		wxString linkConnection[4] = {"peak-to-valley", "valley-to-peak", "peak-to-peak", "valley-to-valley"};
		m_Gui->Combo(CHANGE_VALUE, "Connection", &m_Id_Link_Connection, 4, linkConnection);
		m_Gui->Double(CHANGE_VALUE,_("Length"), &m_Link_Length, 0, 10000,-1,_("Link length(longitudinal direction (mm))"));
		wxString linkAlignment[3] = {"+0", "+1","+2"};
		m_Gui->Combo(CHANGE_VALUE, "alignment", &m_Link_Alignment, 3, linkAlignment);

		m_Gui->Divider(2);
		wxString linkOrientation[3] = {"+0", "+1","-1"};
		m_Gui->Combo(CHANGE_VALUE, "Orientation", &m_Link_orientation, 3, linkOrientation);	  

		m_Gui->Divider(2);

		m_Gui->Button(ID_CENTERLINE, &m_CenterLineName,  _("center line"), _("Select the centerline for creating stent"));//Centerline
		//m_Gui->Label("Constrain Surface");
		//m_Gui->Button(ID_CONSTRAIN_SURFACE, &m_ConstrainSurfaceName, _(".."), _("Select the constrain surface for deploying stent"));
		m_Gui->Button(ID_CONSTRAIN_SURFACE, &m_ConstrainSurfaceName,  _("constrain surface"), _("Select the constrain surface for deploying stent"));

		m_Gui->Divider(2);

		//change Vessel View
		m_Gui->Bool(CHANGE_VIEW,_("catheter"),&m_ShowCatheter,0,"Show catheter");
		m_Gui->Enable(CHANGE_VIEW,m_CenterLineSetFlag);
		

		m_Gui->Button(ID_DEFORMATION, _("deformation"),""  , _(" stent deformation"));
		m_Gui->Enable(ID_DEFORMATION,(m_CenterLineSetFlag)&&(m_ConstrainSurfaceSetFlag));


		m_Gui->FitGui();
		m_Gui->Update();
	}
	m_Gui->Divider();
	return m_Gui;
}

 

  /** Precess events coming from other objects */ 
//-------------------------------------------------------------------------
void medVMEStent::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
		case CHANGE_VALUE:
		  {  
			/*if centerline was set compute maximum crownNumber*/
			 /* if (this->m_Centerline )
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
	 
		  case ID_CONSTRAIN_SURFACE:
		  case ID_CENTERLINE:
		  {
		
			 mafVME *vme;
			 mafNode *node;
			 mafID button_id = e->GetId(); 
			 mafString title = mafString("Select a VME:");
			 mafEvent e(this,VME_CHOOSE);
			 e.SetString(&title);
			 //e.SetArg((long)(&aneuOpExtractVesselParameters::SurfaceAccept)); // accept only mafVMESurface
			 ForwardUpEvent(e);
			 node = e.GetVme();
			 vme = (mafVME *)node;
			 if(vme)
			{
				vme->Update();
				if (button_id == ID_CENTERLINE){
					m_CenterLineName = vme->GetName();
					if(vme->IsMAFType(medVMEPolylineGraph))
					{
						SetAndKeepCenterLine(node);

					}

				}else if (button_id == ID_CONSTRAIN_SURFACE){
				
					SetAndKeepConstrainSurface(node);


				}
				InternalUpdate();
				m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
				ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
				m_Gui->Enable(ID_DEFORMATION,(m_CenterLineSetFlag)&&(m_ConstrainSurfaceSetFlag));
				m_Gui->Enable(CHANGE_VIEW,m_CenterLineSetFlag);
				m_Gui->Update();
			}
		
		  }	
		  break;
		  case CHANGE_VIEW:{
			  DisplayCatherter(); //display or remove catherter from output
			  m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
			  ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
		  }	  
		  break;
		  case ID_DEFORMATION:
		  {
				  //DoDeformation2(1);
				  DoDeformation3(0);
				  m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
				  ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));

		  }
		  break;
		  default:
			mafVME::OnEvent(maf_event);
    }//end of switch
  }//end of if
  else{
    Superclass::OnEvent(maf_event);
  }
/*else if (maf_event->GetChannel()==MCH_DOWN)
  {
		//mafTimeStamp t = m_CurrentTime;	 
		moveCatheter(m_CurrentTime);
	    m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
	    ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE)); 
  }*/  
  
}
/** called to prepare the update of the output */
//-----------------------------------------------------------------------
void medVMEStent::InternalPreUpdate()
//-----------------------------------------------------------------------
{
	InternalInitialize();

}
/** update the output data structure */
//-----------------------------------------------------------------------
void medVMEStent::InternalUpdate()
	//-----------------------------------------------------------------------
{
	//-------------------------------------
	if(m_DeformFlag==0 && m_numberOfCycle ==0){//
		if(m_Centerline)
		{	
			
			m_StentPolyLine = vtkPolyData::New();
			
			//testStent.setCenterLinePolydata(m_Centerline); //waitting for code updating

			vtkMEDStentModelSource currentStentSource;

			currentStentSource.setStentDiameter(m_Stent_Diameter);// 4.0,m_Stent_Diameter
			currentStentSource.setCrownLength(m_Crown_Length);
			currentStentSource.setCrownNumber(m_Crown_Number);
			
			currentStentSource.setStentConfiguration((enumStCfgType)m_Id_Stent_Configuration);//m_Id_Stent_Configuration
			currentStentSource.setLinkConnection((enumLinkConType) m_Id_Link_Connection);
			currentStentSource.setLinkOrientation( (enumLinkOrtType)m_Link_orientation);

			currentStentSource.setLinkLength(m_Link_Length);
			currentStentSource.setLinkAlignment(m_Link_Alignment);
			//currentStentSource.setTestValue();
			int linePointNumber = m_Centerline->GetNumberOfPoints();
			if(m_Centerline != NULL && linePointNumber>0){
				currentStentSource.setCenterLineFromPolyData(m_Centerline);
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
			m_StentRadius = currentStentSource.getRadius();

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
			m_StentPolyLine->SetPoints(vpoints) ;
			vpoints->Delete() ;

			int tindices[2];
			vtkCellArray *lines = vtkCellArray::New() ;
			lines->Allocate(2000) ;  

			m_StrutArray = vtkCellArray::New();
			m_LinkArray = vtkCellArray::New();
			
			 
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
			m_StentPolyLine->SetLines(lines);
			//---------------Vessel VTK-----
			m_StentPolyLine->Modified();
			m_StentPolyLine->Update();

			//----------------output------------
			m_PolyData->DeepCopy(m_StentPolyLine);	
			m_PolyData->Modified();
			m_PolyData->Update();


			m_AppendPolyData->Update();
			m_AppendPolys->Update();
			
		}// end of if m_Centerline
	}//end of if m_numberOfCycle
}//end of method




void medVMEStent::DoDeformation3(int type){
	int steps = 1;
	int iteratorNumbers =300;//20*m_Crown_Number;



	if(type==0){
		steps = iteratorNumbers ;//-m_numberOfCycle ;
	}
	if (m_DeformFlag ==0)
	{
		deformFilter = DeformFilterType::New();	
		//---------------deform settings from here---------------
		deformFilter->SetInput( m_SimplexMesh );
		deformFilter->SetGradient( NULL);
		deformFilter->SetAlpha(0.4);//0.03 (0.01-0.3)//my version 0.3
		deformFilter->SetBeta(0.02);//0.01 (0.01-1) //my version 0.05 //b0.3 does not match a0.3, in a mass
		deformFilter->SetIterations(2); //will effect speed
		deformFilter->SetRigidity(1); //(1-8 smoother)
		deformFilter->SetStrutLength(this->GetStrutLength());//(m_StentSource.getStrutLength());
		deformFilter->SetLinkLength(this->GetLinkLength());//(m_StentSource.getLinkLength());
		deformFilter->SetStrutLinkFromCellArray(m_StrutArray,m_LinkArray);
		if (m_ConstrainSurface!=NULL && m_ConstrainSurface->GetNumberOfPoints()>0)
		{
			
			deformFilter->SetCenterLocationIdx(m_centerLocation.begin());
			deformFilter->SetVesselPointsKDTreeFromPolyData(m_ConstrainSurface);
		}
		PreComputeStentPointsBySteps(steps);
		//DisplayStentExpend(steps);


		m_DeformFlag = 1;
	}
	
}

void medVMEStent::DisplayStentExpend( int steps )
{
	vector<vtkPoints*>::iterator pointsIter = m_ItPointsContainer.begin();
	vtkPoints* currentPoints;
	for (int i=0; pointsIter!=m_ItPointsContainer.end(); ++pointsIter,i++ )
	{
		currentPoints = *pointsIter;
		ResetStentPoints(currentPoints);
		if(m_ShowCatheter){
			moveCatheter(i);
		}
		
	}
}
void medVMEStent::DisplayCatherter(){

	if(m_ShowCatheter){//add
		createCatheter(m_Centerline);
		m_AppendPolyData->AddInput(m_CatheterPolyData);
	}else{//remove
		m_AppendPolyData->RemoveInput(m_CatheterPolyData);
	}
	

}
void medVMEStent::ResetStentPoints( vtkPoints* currentPoints )
{
	//---set output---------
	m_StentPolyLine->SetPoints(currentPoints);
	m_StentPolyLine->Modified();
	m_StentPolyLine->Update();
	//vpoints->Delete() ; ????

	//----new try remove vessel from appendoutput----------
	m_PolyData->DeepCopy(m_StentPolyLine);
	m_PolyData->Update(); 
	//----------
	m_numberOfCycle++;
	//UpdateViewAfterDeformation();
}
void medVMEStent::UpdateViewAfterDeformation(){

	m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);//must update data
	ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));//must updata camera


}
/*void medVMEStent::DisplayStentExpendByStep(mafTimeStamp t){
	
	if(floor(t)<m_ItPointsContainer.size()){
		if(m_ShowCatheter){
			moveCatheter(t*2);
		}

		vtkPoints* oneStepPoints = m_ItPointsContainer[t];
		ResetStentPoints(oneStepPoints);
		UpdateViewAfterDeformation();
	}

}*/

//expend stent and move catheter
void medVMEStent::DisplayStentExpendByStep(mafTimeStamp t){


		if(floor(t)<m_ItPointsContainer.size()){// if stent needs expend
			vtkPoints* oneStepPoints = m_ItPointsContainer[t];
			ResetStentPoints(oneStepPoints);
		}

		//at lease catheter needs update, moveCatheter its self will deside if update needed
		if(m_ShowCatheter){
			moveCatheter(t*2);
		}
		UpdateViewAfterDeformation();

}


/************************************************************************/
/* be careful, when should ,recompute points.                                                                     */
/************************************************************************/
void medVMEStent::PreComputeStentPointsBySteps( int steps){
	//-------------timer--------
	time_t t1,t2;
	(void) time(&t1);
	//-------------timer--------


	m_Gui->Enable(ID_DEFORMATION,false);
	m_Gui->Update();
	for (int i=0;i<steps;i++)
	{
		//-------one step deformation--------
		deformFilter->SetCurIterationNum(i);
		deformFilter->Update();
		//----------get point set from mesh-----------
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
		m_ItPointsContainer.push_back(vpoints);


		//numberOfCycle++;
	}
	//-------------timer--------
	(void) time(&t2);
	//-------------timer--------
	int diff =(int) t2 -t1;
	m_Gui->Enable(ID_DEFORMATION,true);
	m_Gui->Update();

}



/** expand stent in a constrain surface 
* for this method, only need to change position of every points, since lines was set.
*/
void medVMEStent::expandStent(int numberOfCycle ){
	//-------------deform stent---------
	deformFilter->SetCurIterationNum(numberOfCycle);
	deformFilter->Update();

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
	
	m_StentPolyLine->SetPoints(vpoints);
	m_StentPolyLine->Modified();
	m_StentPolyLine->Update();
	vpoints->Delete() ;

	//----new try remove vessel from appendoutput----------
	m_PolyData->DeepCopy(m_StentPolyLine);
	m_PolyData->Update(); 
	
}
void medVMEStent::createTestVesselPolydata(vtkPolyData  *centerLine){
	int test1 = centerLine->GetNumberOfPoints();
	int test2 = centerLine->GetNumberOfLines();
	vtkTubeFilter *sheath ;
	vtkNEW(sheath);
	//= vtkTubeFilter::New();
	sheath->SetInput(centerLine);//(m_Centerline);//(m_SheathVTK);
	sheath->SetRadius(m_StentRadius +5);
	sheath->SetNumberOfSides(20);
	sheath->Update();


	vtkPolyData *tubePolys = TubeToPolydata(sheath);

	m_TestVesselPolyData->DeepCopy(tubePolys); //important for deforming and expending
	//m_CatheterPolyData->DeepCopy(m_Centerline);//just for test
	m_TestVesselPolyData->Update();
	//m_AppendPolyData->AddInput(m_TestVesselPolyData);//to show vessel
	int numTmp = m_TestVesselPolyData->GetNumberOfPolys();
}
void medVMEStent::createCatheter(vtkPolyData  *centerLine){
	int test1 = centerLine->GetNumberOfPoints();
	int test2 = centerLine->GetNumberOfLines();
	vtkTubeFilter *sheath ;
	vtkNEW(sheath);
	//= vtkTubeFilter::New();
	sheath->SetInput(centerLine);//(m_Centerline);//(m_SheathVTK);
	sheath->SetRadius(m_StentRadius +0.1);
	sheath->SetNumberOfSides(40);
	sheath->Update();


	vtkPolyData *tubePolys = TubeToPolydata(sheath);

	m_CatheterPolyData->DeepCopy(tubePolys); //important for deforming and expending
	//m_CatheterPolyData->DeepCopy(m_Centerline);//just for test
	m_CatheterPolyData->Update();
	int numTmp = m_CatheterPolyData->GetNumberOfPolys();


}
/** move catheter so that stent can expand */
void medVMEStent::moveCatheter(mafTimeStamp currentIter ){
		
		vtkPolyData *copyOfCenterLine = vtkNEW(copyOfCenterLine);
		copyOfCenterLine->DeepCopy(m_Centerline);
	    
		static int pointsNumOnCenterLine = copyOfCenterLine->GetNumberOfPoints();

		vtkCellArray *cLines = vtkCellArray::New() ;
	    cLines->Allocate(300);
		int tindices[2];
		/*--------
		* setting the speed for catheter's pulling away
		* we need the centerline been densely and evenly sampled
		* here we set the speed as pulling away to next center point every x iterations, 
		* x = 2 for now
		*
		*it is simulated by changing the line cells of the VTKPolyData. 
        *Only Connect the subset of central vertices that are covered by the catheters with line cells.
		*
		*
		*please set this corresponding with the parameters used in the DeformaleSimplexMeshFilter001
		*-----------*/
		int speed = 4;
		int m = (currentIter/speed+100<pointsNumOnCenterLine-1)?(currentIter/speed+100):pointsNumOnCenterLine-1;
	    for(int i=currentIter/speed;i<m;i++){
			tindices[0] = i;
			tindices[1] = i+1;
			cLines->InsertNextCell(2, tindices);
	   }
	   cLines->Squeeze() ;
       copyOfCenterLine->SetLines(cLines);
	 
	   // m_SheathVTK->SetPoints(m_Centerline->getpo)
	  
	   copyOfCenterLine->Modified();
	   copyOfCenterLine->Update();
	   cLines->Delete();

	   createCatheter(copyOfCenterLine);

	/*   vtkTubeFilter *sheath ;
	   vtkNEW(sheath);
	   //= vtkTubeFilter::New();
	   sheath->SetInput(m_SheathVTK);
	   sheath->SetRadius(m_StentRadius +0.1);
	   sheath->SetNumberOfSides(40);
	   sheath->Update();


	   vtkPolyData *tubePolys = TubeToPolydata(sheath);

	   m_CatheterPolyData->DeepCopy(tubePolys); //important for deforming and expending
	   m_CatheterPolyData->Update();
	   int numTmp = m_CatheterPolyData->GetNumberOfPolys();
	   */
}




/*for Catheter moving*/
void medVMEStent::SetCenterLocationIdxRef(vector<int> const&ve){
	//m_centerLocationIdx = ve.begin();
	m_centerLocation = ve;
}

void medVMEStent::SetCenterLocationIdx(vector<int>::const_iterator centerLocationIndex){
	m_centerLocationIdx = centerLocationIndex;
}

/** give vme a constrain surface */
void medVMEStent::SetConstrainSurface(vtkPolyData *surface){
	if(surface){

		m_ConstrainSurface->DeepCopy(surface);
		m_ConstrainSurface->Update();
		int num = surface->GetNumberOfPoints();
		num = m_ConstrainSurface->GetNumberOfPoints();
		if(num>0){
			m_ConstrainSurfaceSetFlag = 1;
		}
	}
	//vtkNEW(m_ConstrainSurface);

}
/** create one using the center line */
vtkPolyData* medVMEStent::CreateAConstrainSurface(){
	vtkPolyData *rtnSurface; 
	vtkNEW(rtnSurface);
	
	vtkPolyData *surfacePoly = vtkPolyData::New();
	static float cVertex[3]; //vertex 
	int tindices[2];//edges
	vtkPoints *centerPoints = vtkPoints::New();
	centerPoints->Allocate(300);
	int pIndex = 0;
	for(vector<vector<double>>::const_iterator iter = m_CenterLineStart;
		iter!=m_CenterLineEnd; iter++){
			cVertex[0] = (*iter)[0];
			cVertex[1] = (*iter)[1];
			cVertex[2] = (*iter)[2];
			centerPoints->InsertPoint(pIndex,cVertex);
			pIndex++;
	}
	centerPoints->Squeeze();
	surfacePoly->SetPoints(centerPoints);	
	centerPoints->Delete();

	vtkCellArray *cLines = vtkCellArray::New() ;
	cLines->Allocate(300);
	for(int i=0;i<pIndex;i++){
		tindices[0] = i;
		tindices[1] = i+1;
		cLines->InsertNextCell(2, tindices);
	}
	cLines->Squeeze() ;
	surfacePoly->SetLines(cLines);
	cLines->Delete();
	surfacePoly->Update();

	vtkTubeFilter *surfaceTube ;
	vtkNEW(surfaceTube);
	surfaceTube->SetInput(surfacePoly);
	surfaceTube->SetRadius(m_StentRadius * 2);
	surfaceTube->SetNumberOfSides(40);
	surfaceTube->Update();


	//-------------create tube------------------
	// Create a tube (cylinder) around the line
	vtkPolyData *tubeStrips = surfaceTube->GetOutput();
	vtkCellArray *strips = tubeStrips->GetStrips();  
	vtkIdType number = strips->GetNumberOfCells();
	int iStrips = tubeStrips->GetNumberOfStrips();

	//------------to polydata----------------
	vtkTriangleFilter *tubeTriangles = vtkTriangleFilter::New();
	tubeTriangles->SetInput(surfaceTube->GetOutput());
	tubeTriangles->Update();

	vtkPolyData *tubePolys = tubeTriangles->GetOutput();
	tubePolys = tubeTriangles->GetOutput();
	tubePolys->BuildCells();
	tubePolys->Update();
	int num1 = tubePolys->GetNumberOfPoints();


	rtnSurface->DeepCopy(tubePolys);

	return rtnSurface;
}

/** convert centerline from polydata into vector
 * create catheter for the center line
 */
void medVMEStent::SetCenterLine(vtkPolyData *line){

	/**------------convert centerline----------*/
	if(line){
		this->m_Centerline = line; //set centerlint
		int test1 = line->GetNumberOfPoints();
		int test2 = line->GetNumberOfLines();
		m_CenterLineSetFlag = 1;//set flag
		createCatheter(m_Centerline);//create first // if(m_ShowCatheter)		
		
	/**------------create catheter------------------*/
/*
		int pointsNumber = this->m_Centerline ->GetNumberOfPoints();
		int tindices[2];//edges
		m_SheathVTK = vtkPolyData::New();
		m_SheathVTK->SetPoints(this->m_Centerline->GetPoints());
		vtkCellArray *cLines = vtkCellArray::New() ;
		cLines->Allocate(pointsNumber);
		for(int i=0;i<pointsNumber-1;i++){
			tindices[0] = i;
			tindices[1] = i+1;
			cLines->InsertNextCell(2, tindices);
		}		

	cLines->Squeeze() ;
	m_SheathVTK->SetLines(cLines);
	cLines->Delete();
	m_SheathVTK->Update();

	vtkTubeFilter *sheath ;
	vtkNEW(sheath);
	//= vtkTubeFilter::New();
	sheath->SetInput(line);//(m_SheathVTK);
	sheath->SetRadius(m_StentRadius +0.1);
	sheath->SetNumberOfSides(40);
	sheath->Update();
	
	
	vtkPolyData *tubePolys = TubeToPolydata(sheath);

	m_CatheterPolyData->DeepCopy(tubePolys); //important for deforming and expending
	m_CatheterPolyData->Update();
	int numTmp = m_CatheterPolyData->GetNumberOfPolys();
	int num2 = m_CatheterPolyData->GetNumberOfPoints();
*/	}//end of if line
	

}

/************************************************************************/
/* store attributes and matrix*/                                                                     
/************************************************************************/

//-----------------------------------------------------------------------
int medVMEStent::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
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
	 if (parent->StoreDouble("StentRadius",m_StentRadius) != MAF_OK) return MAF_ERROR;
	/**----------- center line and constrain surface-----------*/
	 if (parent->StoreText("CenterLineName",m_CenterLineName) != MAF_OK) return MAF_ERROR;
	 //if (parent->StoreText("ConstrainSurfaceName",m_ConstrainSurfaceName) != MAF_OK) return MAF_ERROR;
	 /*------save centerline vme and surface vme ------*/
	 parent->StoreInteger("VmeLinkedListNumberOfElement", m_VmeLinkedList.size());

	 //---------get line point serial------------
	 int numOfPoints = m_Centerline->GetNumberOfPoints();
	 if(numOfPoints>0){
		 vtkPoints *linePoints = m_Centerline->GetPoints();
		 double p[3];
		 vector<double> vertex;
		 for(vtkIdType i = 0; i < numOfPoints; i++){
			 m_Centerline->GetPoint(i,p);
			 
			 //------for store-----
			 m_StentCenterLineSerial.push_back(p[0]);
			 m_StentCenterLineSerial.push_back(p[1]);
			 m_StentCenterLineSerial.push_back(p[2]);
		 }
	 }
	 //parent->StoreVectorN("VmeLinkedList",m_VmeLinkedList, m_VmeLinkedList.size());
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
	  node->RestoreDouble("StentRadius",m_StentRadius);

	  /**----------- center line name and constrain surface name-----------*/
	  node->RestoreText("CenterLineName",m_CenterLineName);
	  node->RestoreText("ConstrainSurfaceName",m_ConstrainSurfaceName);
	  /*---------centerline VME and surface VME -------------*/
	  m_VmeLinkedList.resize(2); //2objects: centerline and surface
	  node->RestoreVectorN("VmeLinkedList",m_VmeLinkedList, m_VmeLinkedList.size());
	 
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
		  aLine->Update();
		  int test1 = aLine->GetNumberOfPoints();
		  int test2 = aLine->GetNumberOfPolys();
		  
		  //m_Centerline = aLine;
		  this->SetCenterLine(aLine);
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
/*-----------the -----------*/
void medVMEStent::SetAndKeepCenterLine( mafNode * node )
{
	if(node){
		mafVME *vme = mafVME::SafeDownCast(node);
		vtkPolyData *polyLine =vtkPolyData::SafeDownCast( vme->GetOutput()->GetVTKData());
		polyLine->Update();
		vtkPolyData *copyLine = vtkNEW(copyLine);
		copyLine->DeepCopy(polyLine);
		this->SetCenterLine(copyLine);
		SetLink("CenterLineVME", node);
		this->m_CenterLineName = vme->GetName();

		//*------------------keep vme id in m_VmeLinkedList -----------------
		m_VmeLinkedList.insert(m_VmeLinkedList.begin(),vme->GetId());
	}
}

void medVMEStent::SetAndKeepConstrainSurface( mafNode * node )
{
	if(node){
		mafVME *vme = mafVME::SafeDownCast(node);
		vtkPolyData *polySurface = vtkPolyData::SafeDownCast(vme->GetOutput()->GetVTKData());
		polySurface->Update();
		vtkIdType pointsNumber = polySurface->GetNumberOfPoints();
		//this->SetConstrainSurface(polySurface);
		//temp
		this->createTestVesselPolydata(m_Centerline);
		this->SetConstrainSurface(m_TestVesselPolyData);
		//temp finish
		
		SetLink("ConstrainSurfaceVME", node);
		this->m_ConstrainSurfaceName = vme->GetName();
		//*------------------keep vme id in m_VmeLinkedList -----------------
		m_VmeLinkedList.insert(m_VmeLinkedList.begin()+1,vme->GetId());
	}
}
//-------------------------------------------------------------------------
mmaMaterial *medVMEStent::GetMaterial()
	//-------------------------------------------------------------------------
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








