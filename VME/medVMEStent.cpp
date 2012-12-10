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
	vtkNEW(m_ConstrainSurfaceTmp);
	vtkNEW(m_AppendPolyData);
	

	vtkNEW(m_Centerline);
	mafNEW(m_Transform);
	
	m_NumberOfCycle = 0;
	m_Stent_Diameter = 2.0;
	m_Crown_Length = 2.2;
	m_Strut_Angle = 0.0;
	m_Strut_Thickness = 0.0;
	m_Id_Stent_Configuration = 1;/* 1.outofphase, 0.InPhase;  enumStCfgType */
	m_Id_Link_Connection = 2; /* 0.peak2valley;enumLinkConType {peak2valley, valley2peak, peak2peak, valley2valley} */
	m_Link_orientation = 0;
	m_Link_Length = 3.0;
	m_Link_Alignment = 0;
	
	m_CenterLineName = "";
	m_ConstrainSurfaceName = "";

	
	
	
	//mafVMEOutputSurface *output=mafVMEOutputSurface::New(); // an output with no data
	mafVMEOutputPolyline *output=mafVMEOutputPolyline::New(); // an output with no data
	output->SetTransform(m_Transform); // force my transform in the output
	SetOutput(output);

	GetMaterial();
	
	m_AppendPolyData->AddInput(m_PolyData);
	m_AppendPolyData->AddInput(m_CatheterPolyData);
	m_AppendPolyData->AddInput(m_ConstrainSurfaceTmp);

	m_AppendPolys = m_AppendPolyData->GetOutput();

	mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
	//dpipe->SetInput(m_PolyData);
	/** stent,catheter and surface  */
	dpipe->SetInput(m_AppendPolys);
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
//-------------------------------------------------------------------------
/*mafVMEOutputSurface *medVMEStent::GetSurfaceOutput()
	//-------------------------------------------------------------------------
{
	return (mafVMEOutputSurface *)GetOutput();
}*/

/** Material Attributes*/
//-------------------------------------------------------------------------
mmaMaterial *medVMEStent::GetMaterial()
//-------------------------------------------------------------------------
{
	mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
	if (material == NULL)
	{
		material = mmaMaterial::New();
		SetAttribute("MaterialAttributes", material);
		if (m_Output)
		{
			((mafVMEOutputPolyline *)m_Output)->SetMaterial(material);
		}
	}
	return material;

}
/** copy attributes*/
//-------------------------------------------------------------------------
int medVMEStent::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
   if (Superclass::DeepCopy(a)==MAF_OK)
  {
    medVMEStent *vmeStent =medVMEStent::SafeDownCast(a);

	m_Transform->SetMatrix(vmeStent->m_Transform->GetMatrix());

    m_Stent_Diameter = vmeStent->m_Stent_Diameter;
	m_Crown_Length = vmeStent->m_Crown_Length;
	m_Id_Stent_Configuration = vmeStent->m_Id_Stent_Configuration;
	m_Id_Link_Connection = vmeStent->m_Id_Link_Connection;
	m_Link_Length = vmeStent->m_Link_Length;
	m_Link_Alignment = vmeStent->m_Link_Alignment;
	m_Link_orientation = vmeStent->m_Link_orientation;

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

		m_Gui->Double(CHANGE_VALUE,_("Diameter"), &m_Stent_Diameter, 0, 10000,-1,_("The length of the stent (mm)"));
		m_Gui->Double(CHANGE_VALUE,_("Crown L."), &m_Crown_Length, 0, 10000,-1,_("The length of the Crown (mm)"));
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

		m_Gui->Button(ID_DEFORMATION, _("deformation"),""  , _(" stent deformation"));



		m_Gui->FitGui();

		m_Gui->Update();
	}

	m_Gui->Divider();
	return m_Gui;
}

 /*void medVMEStent::SelectVME(const char *link_name, mafNode *n){

	
  
	mafEventMacro(e);
    vme = (mafVME *)e.GetVme();

    if(vme)
    {
      vme->Update();
      //m_Surface = mafVMESurface::SafeDownCast(vme);
      //m_Surface->Update(); 
    }
	
}*/

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
		InternalUpdate();
		m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
		ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
		
		//GetPolylineOutput()->Update();
		//m_Gui->Update();
	  }
	  break;
	  case ID_CONSTRAIN_SURFACE:
	  case ID_CENTERLINE:
	  {
		 mafID button_id = e->GetId();
		 mafVME *vme;

		 mafString title = mafString("Select a VME:");
		 mafEvent e(this,VME_CHOOSE);
		 e.SetString(&title);
		 //e.SetArg((long)(&aneuOpExtractVesselParameters::SurfaceAccept)); // accept only mafVMESurface
		 ForwardUpEvent(e);
		
		 vme = (mafVME *)e.GetVme();
		 if(vme)
		{
			vme->Update();
			if (button_id == ID_CENTERLINE){
				m_CenterLineName = vme->GetName();
				if(vme->IsMAFType(medVMEPolylineGraph))
				{
					vtkPolyData *polyLine =vtkPolyData::SafeDownCast( vme->GetOutput()->GetVTKData());
					polyLine->Update();
					this->SetCenterLine(polyLine);
					this->m_CenterLineName = vme->GetName();
				}

			}else if (button_id == ID_CONSTRAIN_SURFACE){
				
				vtkPolyData *polySurface = vtkPolyData::SafeDownCast(vme->GetOutput()->GetVTKData());
				vtkIdType pointsNumber = polySurface->GetNumberOfPoints();
				this->SetConstrainSurface(polySurface);
				this->m_ConstrainSurfaceName = vme->GetName();

			}
			InternalUpdate();
			m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
			ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
			m_Gui->Update();
		}
		
	  }	
	  break;
	  case ID_DEFORMATION:
	  {
			  DoDeformation2(1);
			  m_EventSource->InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
			  ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));

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
	if(m_NumberOfCycle==0){
		if(m_Centerline)
		{	

			m_DeformFlag =0;
			m_StentPolyLine = vtkPolyData::New();
			
			//testStent.setCenterLinePolydata(m_Centerline); //waitting for code updating
			//--------do some processing between parameters--------
			//1.linkLength > crownLength
/*
			//to avoid stent being squeezed when create it
			if( (m_Id_Stent_Configuration==enumStCfgType::InPhase) && (m_Link_Length<m_Crown_Length)){//inphase
				m_Link_Length = m_Crown_Length;
			}else if (  (m_Id_Stent_Configuration==enumStCfgType::OutOfPhase) && (m_Id_Link_Connection==enumLinkConType::valley2valley) && (m_Link_Length< 2*m_Crown_Length) )//outphase
			{
				m_Link_Length = 2*m_Crown_Length;
			}
*/
			m_StentSource.setStentDiameter(m_Stent_Diameter);// 4.0,m_Stent_Diameter
			m_StentSource.setCrownLength(m_Crown_Length);

			m_StentSource.setStentConfiguration((enumStCfgType)m_Id_Stent_Configuration);//m_Id_Stent_Configuration
			m_StentSource.setLinkConnection((enumLinkConType) m_Id_Link_Connection);
			m_StentSource.setLinkOrientation( (enumLinkOrtType)m_Link_orientation);

			m_StentSource.setLinkLength(m_Link_Length);
			m_StentSource.setLinkAlignment(m_Link_Alignment);
			if(m_Centerline != NULL && m_Centerline->GetNumberOfPoints()>0){
				m_StentSource.setCenterLine(m_CenterLineStart,m_CenterLineEnd);
			}
			
			/**/			
			//testStent.setStrutAngle(m_Strut_Angle); ?? a computed value
			m_StentSource.createStent();
			m_StentRadius = m_StentSource.getRadius();

			m_SimplexMesh = m_StentSource.simplexMesh;
			m_SimplexMesh->DisconnectPipeline();
			//-------vessel----------
			//VesselMeshForTesting testVessel;
			//testVessel.createVesselMesh();
			//----------end----------

			//deformFilter->SetVesselPointsKDTree(testVessel.pointList.begin(),testVessel.pointList.end());
			

			static float vertex[3]; 
			//----------stent VTK---------
			//wh modified
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
			for(StrutIterator iter = m_StentSource.strutsList.begin(); iter !=m_StentSource.strutsList.end(); iter++){
				tindices[0] = iter->startVertex;
				tindices[1] = iter->endVertex;
				lines->InsertNextCell(2, tindices);
			}
			for(StrutIterator iter = m_StentSource.linkList.begin(); iter !=m_StentSource.linkList.end(); iter++){
				tindices[0] = iter->startVertex;
				tindices[1] = iter->endVertex;
				lines->InsertNextCell(2, tindices);
			}
			lines->Squeeze() ;
			m_StentPolyLine->SetLines(lines);
			//---------------Vessel VTK-----
			m_StentPolyLine->Modified();
			m_StentPolyLine->Update();
			//------------------output------------------
			m_PolyData->DeepCopy(m_StentPolyLine);	
			m_PolyData->Modified();
			m_PolyData->Update();
			m_AppendPolyData->Update();
			m_AppendPolys->Update();
			
		}// end of if m_Centerline
	}//end of if m_numberOfCycle
}//end of method



/** expand stent in a constrain surface */
//-------------------------------------------------------------------------
//0 for auto
//1 for step by step
void medVMEStent::DoDeformation(int type)
	//-------------------------------------------------------------------------
{

	if(m_PolyData && m_PolyData->GetNumberOfPoints()>0){//stent was created

		int steps = 1;
		int iteratorNumbers = 128;


		if(m_NumberOfCycle>iteratorNumbers){
			return;
		}
		if(type==0){
			steps = iteratorNumbers - m_NumberOfCycle;
		}

		//int iteratorTimes = 1;
		for(int i=0 ; i<steps ; i++){

			//-----------update visualization-----------
			MoveCatheter(m_NumberOfCycle);
			
/*			//-------------deform stent---------
			deformFilter->SetCurIterationNum(m_numberOfCycle);
			deformFilter->Update();

			//------------update stent visualization----------------
			static float vertex[3]; 

			vtkPoints* vpoints = vtkPoints::New();
			vpoints->SetNumberOfPoints(2000);		
			SimplexMeshType::PointsContainer::Pointer sPoints;
			sPoints = simplexMesh->GetPoints();
			for(SimplexMeshType::PointsContainer::Iterator pointIndex = sPoints->Begin(); pointIndex != sPoints->End(); ++pointIndex)
			{
				int idx = pointIndex->Index();
				vtkFloatingPointType * pp = const_cast<vtkFloatingPointType*>(pointIndex->Value().GetDataPointer());
				vpoints->SetPoint(idx,pp);
			}

			vpoints->Squeeze();
	
			stentPolyLine->SetPoints(vpoints);
			stentPolyLine->Modified();
			stentPolyLine->Update();
			vpoints->Delete() ;

			//----new try remove vessel from appendoutput----------
			//m_PolyData->SetPoints(vpoints);
			m_PolyData->DeepCopy(stentPolyLine);
			m_PolyData->Modified();
			m_PolyData->Update(); 
*/
			m_AppendPolyData->Update();
			m_AppendPolys->Update();
			m_NumberOfCycle++;


			//-------------update view------------
			mafEvent ev(this, CAMERA_UPDATE);
			ev.SetId(CAMERA_UPDATE);
			ev.SetArg(0);
			ForwardUpEvent(ev);

		}//end of for
		//m_numberOfCycle = 0;
	}
}

/** expand stent in a constrain surface */
void medVMEStent::DoDeformation2(int type){
	//DeformFilterType::Pointer deformFilter;
	if(m_DeformFlag == 0){
		deformFilter = DeformFilterType::New();	
		int cycle = 5;
		//---------------deform from here---------------

		deformFilter->SetInput( m_SimplexMesh );
		deformFilter->SetGradient( NULL);
		deformFilter->SetAlpha(0.03);
		deformFilter->SetBeta(0.01);
		deformFilter->SetIterations(4);
		deformFilter->SetRigidity(1);
		deformFilter->SetStrutLength(m_StentSource.getStrutLength());
		deformFilter->SetLinkLength(m_StentSource.getLinkLength());
		deformFilter->SetTestValue(321);
		deformFilter->SetStrutLinkIter(m_StentSource.strutsList.begin(),m_StentSource.strutsList.end(),m_StentSource.linkList.begin(),m_StentSource.linkList.end());
		if (m_ConstrainSurfaceTmp!=NULL && m_ConstrainSurfaceTmp->GetNumberOfPoints()>0)
		{
			//deformFilter->SetCenterLocationIdx(stentSource.centerLocationIndex.begin());
			deformFilter->SetCenterLocationIdxRef(m_StentSource.centerLocationIndex);
			deformFilter->SetVesselPointsKDTreeFromPolyData(m_ConstrainSurfaceTmp);
		}
		m_DeformFlag = 1;
}

	MoveCatheter(m_NumberOfCycle);
	ExpandStent(m_NumberOfCycle);
	m_NumberOfCycle++;

	/*for (int i=0;i<cycle;i++)
	{
	moveCatheter(i);
	expandStent(i);

		//-------------update view------------
		mafEvent ev(this, CAMERA_UPDATE);
		ev.SetId(CAMERA_UPDATE);
		ev.SetArg(0);
		ForwardUpEvent(ev);

	}*/
}

/** expand stent in a constrain surface */
void medVMEStent::ExpandStent(int numberOfCycle ){
	//-------------deform stent---------
	deformFilter->SetCurIterationNum(m_NumberOfCycle);
	deformFilter->Update();

	//------------update stent visualization----------------
	static float vertex[3]; 

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

	vpoints->Squeeze();

	m_StentPolyLine->SetPoints(vpoints);
	m_StentPolyLine->Modified();
	m_StentPolyLine->Update();
	vpoints->Delete() ;

	//----new try remove vessel from appendoutput----------
	//m_PolyData->SetPoints(vpoints);
	m_PolyData->DeepCopy(m_StentPolyLine);
	m_PolyData->Modified();
	m_PolyData->Update(); 
}
/** move catheter so that stent can expand */
void medVMEStent::MoveCatheter(int numberOfCycle ){
		
		static int pointsNumOnCenterLine = 209;

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
		int m = (numberOfCycle/2+100<pointsNumOnCenterLine-1)?(numberOfCycle/2+100):pointsNumOnCenterLine-1;
	    for(int i=numberOfCycle/2;i<m;i++){
		tindices[0] = i;
		tindices[1] = i+1;
		cLines->InsertNextCell(2, tindices);
	   }
	   cLines->Squeeze() ;
       m_SheathVTK->SetLines(cLines);
	  
	   m_SheathVTK->Modified();
	   m_SheathVTK->Update();
	   cLines->Delete();

	   vtkTubeFilter *sheath ;
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
}

/** give vme a constrain surface */
void medVMEStent::SetConstrainSurface(vtkPolyData *surface){
	//vtkNEW(m_ConstrainSurface);
/*
	//m_ConstrainSurface = surface;
	vtkPolyData *newSurface = CreateAConstrainSurface();
	int num = newSurface->GetNumberOfPoints();
	m_ConstrainSurfaceTmp->DeepCopy(newSurface) ;
	m_ConstrainSurfaceTmp->Modified();
	m_ConstrainSurfaceTmp->Update();
	num = m_ConstrainSurfaceTmp->GetNumberOfPoints();
*/
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
	//= vtkTubeFilter::New();
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

	//tubeFilter->Update();
	//tubeFilter->get
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

		/**------------to get start and end------------*/
		int numOfPoints = m_Centerline->GetNumberOfPoints();
		if(numOfPoints>0){
			vtkPoints *linePoints = m_Centerline->GetPoints();
			double p[3];
			vector<double> vertex;
			for(vtkIdType i = 0; i < numOfPoints; i++){
				m_Centerline->GetPoint(i,p);
				vertex.push_back(p[0]);
				vertex.push_back(p[1]);
				vertex.push_back(p[2]);
				m_StentCenterLine.push_back(vertex);
				vertex.clear();
			}
			m_CenterLineStart = m_StentCenterLine.begin();
	        m_CenterLineEnd = m_StentCenterLine.end();
		}
	}
	/**------------create catheter------------------*/
	m_SheathVTK = vtkPolyData::New();
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
	m_SheathVTK->SetPoints(centerPoints);	
	centerPoints->Delete();

	vtkCellArray *cLines = vtkCellArray::New() ;
	cLines->Allocate(300);
	for(int i=0;i<pIndex-61;i++){
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
	sheath->SetInput(m_SheathVTK);
	sheath->SetRadius(m_StentRadius +0.1);
	sheath->SetNumberOfSides(40);
	sheath->Update();
	
	
	vtkPolyData *tubePolys = TubeToPolydata(sheath);



	m_CatheterPolyData->DeepCopy(tubePolys); //important for deforming and expending
	m_CatheterPolyData->Update();
	int numTmp = m_CatheterPolyData->GetNumberOfPolys();
	int num2 = m_CatheterPolyData->GetNumberOfPoints();

	/**------------create surface for constrain---------*/

	vtkTubeFilter *constrainTube ;
	vtkNEW(constrainTube);
	//= vtkTubeFilter::New();
	constrainTube->SetInput(m_SheathVTK);
	constrainTube->SetRadius(m_StentRadius *3);
	constrainTube->SetNumberOfSides(40);
	constrainTube->Update();

	vtkPolyData *tubeSurfacePolys = TubeToPolydata(constrainTube);
	m_ConstrainSurfaceTmp->DeepCopy(tubeSurfacePolys);
	m_ConstrainSurfaceTmp->Update();           //important for deforming and expending
	num2 = m_ConstrainSurfaceTmp->GetNumberOfPoints();



}



/************************************************************************/
/* store attributes and matrix*/                                                                     
/************************************************************************/

//-----------------------------------------------------------------------
int medVMEStent::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
/*	if (Superclass::InternalStore(parent)==MAF_OK)
	{
    if (parent->StoreMatrix("Transform",&m_Transform->GetMatrix()) != MAF_OK) return MAF_ERROR;
    if (parent->StoreInteger("NumberPoints",m_NPoints) != MAF_OK) return MAF_ERROR;
    if (parent->StoreVectorN("OriginalRadius",m_OriginalRadius,m_NPoints) != MAF_OK) return MAF_ERROR;
    if (parent->StoreVectorN("Labels",m_Labels,m_NPoints) != MAF_OK) return MAF_ERROR;
    if (parent->StoreVectorN("Radius",m_Radius,m_NPoints) != MAF_OK) return MAF_ERROR;
    if (parent->StoreVectorN("MinimumRadius",m_MinRadius,m_NPoints) != MAF_OK) return MAF_ERROR;  
 
    //store planes: it needs store normal and origin for every plane
    int n = m_NPoints*3, k=0;

    double *normal = new double[n];
    double *origin = new double[n];
    //store original plane
    for(int i=0; i<n; i+=3)
    {   
      for (int j=0; j<3; j++)
      {
        normal[i+j] = m_OriginalPlanes[k]->GetNormal()[j];
        origin[i+j] = m_OriginalPlanes[k]->GetOrigin()[j];
      }
      k++;
    }
    parent->StoreVectorN("NormalOriginalPlanes",normal,n);
    parent->StoreVectorN("OriginOriginalPlanes",origin,n);
    //store new plane
    k=0;
    for(int i=0; i<n; i+=3)
    {   
      for (int j=0; j<3; j++)
      {
        normal[i+j] = m_Planes[k]->GetNormal()[j];
        origin[i+j] = m_Planes[k]->GetOrigin()[j];
      }
      k++;
    }
    parent->StoreVectorN("NormalPlanes",normal,n);
    parent->StoreVectorN("OriginPlanes",origin,n);
    delete normal;
    delete origin;

    return MAF_OK;
	}
*/
	return MAF_ERROR;
}
/************************************************************************/
/* restore attributes and matrix*/                                                                     
/************************************************************************/

//-----------------------------------------------------------------------
int medVMEStent::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
/*	if (Superclass::InternalRestore(node)==MAF_OK)
	{
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {
      
      m_Transform->SetMatrix(matrix); 
      node->RestoreInteger("NumberPoints",m_NPoints); 
     
      m_OriginalRadius = new double[m_NPoints];
      m_Radius= new double[m_NPoints];
      m_Labels= new double[m_NPoints];
      m_MinRadius= new double[m_NPoints];
      node->RestoreVectorN("OriginalRadius",m_OriginalRadius,m_NPoints);
      node->RestoreVectorN("Radius",m_Radius,m_NPoints);
      node->RestoreVectorN("Labels",m_Labels,m_NPoints);
      node->RestoreVectorN("MinimumRadius",m_MinRadius,m_NPoints);

      m_OriginalPlanes=new vtkPlane*[m_NPoints]; 
      m_Planes=new vtkPlane*[m_NPoints];

      //restore planes: it needs restore normal and origin for every plane
      int n = m_NPoints*3;
      double normalValue[3];
      double originValue[3];

      double *normal = new double[n];
      double *origin = new double[n];
      node->RestoreVectorN("NormalOriginalPlanes",normal,n);
      node->RestoreVectorN("OriginOriginalPlanes",origin,n);
      int k=0;
      for(int i=0; i<n; i+=3)
      {   
        for (int j=0; j<3; j++)
        {
          normalValue[j] = normal[i+j];
          originValue[j] = origin[i+j];
        }
        vtkNEW(m_OriginalPlanes[k]);
        m_OriginalPlanes[k]->SetNormal(normalValue);
        m_OriginalPlanes[k]->SetOrigin(originValue);
        k++;
      }
      node->RestoreVectorN("NormalPlanes",normal,n);
      node->RestoreVectorN("OriginPlanes",origin,n);   
      int l=0;
      for(int i=0; i<n; i+=3)
      {   
        for (int j=0; j<3; j++)
        {
          normalValue[j] = normal[i+j];
          originValue[j] = origin[i+j];
        }
        vtkNEW(m_Planes[l]);
        m_Planes[l]->SetNormal(normalValue);
        m_Planes[l]->SetOrigin(originValue);
        l++;
      }
      delete normal;
      delete origin;

      InitCells();
      
      //generate points
      int innerPoints=m_NPoints*m_RadialRes; 
      int outerPoints=m_NPoints*m_RadialRes;
      vtkPoints *new_points;
      vtkNEW(new_points);

      new_points->SetNumberOfPoints(innerPoints+outerPoints);

      m_PolyData->SetPoints(new_points);
      vtkDEL(new_points);
 
      return MAF_OK;
    }
	}
*/
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

