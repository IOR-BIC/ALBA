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

#include "medVMEStent.h"


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

//------new include
#include <iostream>
#include "itkSimplexMesh.h"
#include "itkDefaultDynamicMeshTraits.h"
#include "DeformableSimplexMeshFilter001.h"

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkProperty.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"

//--------typedef----------
typedef itk::DefaultDynamicMeshTraits<double, 3, 3, double, double> MeshTraits;
typedef itk::SimplexMesh<double,3,MeshTraits>           SimplexMeshType;
typedef SimplexMeshType::NeighborListType               NeighborsListType;
typedef itk::SimplexMeshGeometry::IndexArray            IndexArray;
typedef SimplexMeshType::PointType                      PointType;
typedef StentModelSource::Strut                         Strut;
typedef StentModelSource::StentConfigurationType        enumStCfgType;
typedef StentModelSource::LinkConnectionType            enumLinkConType;
typedef StentModelSource::LinkOrientationType           enumLinkOrtType;


typedef itk::DeformableSimplexMeshFilter001<SimplexMeshType,SimplexMeshType> DeformFilterType;
typedef vector<Strut>::const_iterator    StrutIterator;

//-------------define-----------------
SimplexMeshType::Pointer simplexMesh;
DeformFilterType::Pointer deformFilter;
static int numberOfCycle = 0;

//------------------------------------------------------------------------------
mafCxxTypeMacro(medVMEStent);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
medVMEStent::medVMEStent()
//-------------------------------------------------------------------------
{
	
	vtkNEW(m_PolyData);
	vtkNEW(m_Centerline);
	mafNEW(m_Transform);
	
	m_numberOfCycle = 0;
	m_Stent_Diameter = 2.0;
	m_Crown_Length = 2.2;
	m_Strut_Angle = 0.0;
	m_Strut_Thickness = 0.0;
	m_Id_Stent_Configuration = 1;//1.outofphase//0.InPhase;//enumStCfgType
	m_Id_Link_Connection = 2;//0.peak2valley;//enumLinkConType //{peak2valley, valley2peak, peak2peak, valley2valley}
	m_Link_orientation = 0;
	m_Link_Length = 3.0;
	m_Link_Alignment = 0;
	
	
	
	//mafVMEOutputSurface *output=mafVMEOutputSurface::New(); // an output with no data
	mafVMEOutputPolyline *output=mafVMEOutputPolyline::New(); // an output with no data
	output->SetTransform(m_Transform); // force my transform in the output
	SetOutput(output);

	GetMaterial();
	
	mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
	dpipe->SetInput(m_PolyData);
	SetDataPipe(dpipe);
}

//-------------------------------------------------------------------------
medVMEStent::~medVMEStent()
//-------------------------------------------------------------------------
{
	vtkDEL(m_PolyData);
	mafDEL(m_Transform);
	SetOutput(NULL);
}

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
	  dpipe->SetInput(m_PolyData);
    }

    return MAF_OK;
  }  
  return MAF_ERROR;

}




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
  
//-------------------------------------------------------------------------
void medVMEStent::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
	m_Transform->SetMatrix(mat);
	Modified();
}
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
		m_Gui->Label("deform");
		m_Gui->OkCancel();

		m_Gui->FitGui();

		m_Gui->Update();
	}

	m_Gui->Divider();
	return m_Gui;
}


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
	  case wxOK:
	  {
			  DoDeformation(0);
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
void medVMEStent::InternalPreUpdate()
//-----------------------------------------------------------------------
{
	InternalInitialize();

}
//-----------------------------------------------------------------------
void medVMEStent::InternalUpdate()
	//-----------------------------------------------------------------------
{
	 
	//-------------------------------------
	if(m_numberOfCycle==0){
		if(m_Centerline)
		{	
			stentPolyLine = vtkPolyData::New();
			deformFilter = DeformFilterType::New();	

			StentModelSource stentSource;
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
			stentSource.setStentDiameter(m_Stent_Diameter);// 4.0,m_Stent_Diameter
			stentSource.setCrownLength(m_Crown_Length);

			stentSource.setStentConfiguration((enumStCfgType)m_Id_Stent_Configuration);//m_Id_Stent_Configuration
			stentSource.setLinkConnection((enumLinkConType) m_Id_Link_Connection);
			stentSource.setLinkOrientation( (enumLinkOrtType)m_Link_orientation);

			stentSource.setLinkLength(m_Link_Length);
			stentSource.setLinkAlignment(m_Link_Alignment);

			/**/			
			//testStent.setStrutAngle(m_Strut_Angle); ?? a computed value
			stentSource.createStent();
			simplexMesh = stentSource.simplexMesh;

			//-------vessel----------
			//VesselMeshForTesting testVessel;
			//testVessel.createVesselMesh();
			//----------end----------
			//---------------deform from here---------------
			simplexMesh->DisconnectPipeline();
			deformFilter->SetInput( simplexMesh );
			deformFilter->SetGradient( NULL);
			deformFilter->SetAlpha(0.03);
			deformFilter->SetBeta(0.01);
			deformFilter->SetIterations(4);
			deformFilter->SetRigidity(1);
			deformFilter->SetStrutLength(stentSource.getStrutLength());
			deformFilter->SetLinkLength(stentSource.getLinkLength());
			deformFilter->SetStrutLinkIter(stentSource.strutsList.begin(),stentSource.strutsList.end(),stentSource.linkList.begin(),stentSource.linkList.end());
			//deformFilter->SetVesselPointsKDTree(testVessel.pointList.begin(),testVessel.pointList.end());

			static float vertex[3]; 
			//----------stent VTK---------
			//wh modified
			// Create the vtkPoints object and set the number of points
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
			vpoints->Squeeze() ;
			stentPolyLine->SetPoints(vpoints) ;
			vpoints->Delete() ;

			int tindices[2];
			vtkCellArray *lines = vtkCellArray::New() ;
			lines->Allocate(2000) ;   
			for(StrutIterator iter = stentSource.strutsList.begin(); iter !=stentSource.strutsList.end(); iter++){
				tindices[0] = iter->startVertex;
				tindices[1] = iter->endVertex;
				lines->InsertNextCell(2, tindices);
			}
			for(StrutIterator iter = stentSource.linkList.begin(); iter !=stentSource.linkList.end(); iter++){
				tindices[0] = iter->startVertex;
				tindices[1] = iter->endVertex;
				lines->InsertNextCell(2, tindices);
			}
			lines->Squeeze() ;
			stentPolyLine->SetLines(lines);
			//---------------Vessel VTK-----
			stentPolyLine->Modified();
			stentPolyLine->Update();
			//------------------output------------------
			m_PolyData->DeepCopy(stentPolyLine);	
			m_PolyData->Modified();
			m_PolyData->Update();
			
		}// end of if m_Centerline
	}//end of if m_numberOfCycle
}//end of method

//-------------------------------------------------------------------------
//0 for auto
//1 for step by step
void medVMEStent::DoDeformation(int type)
	//-------------------------------------------------------------------------
{

	if(stentPolyLine){
	
	
		int steps = 1;
		int iteratorNumbers = 128;


		if(m_numberOfCycle>iteratorNumbers){
			return;
		}
		if(type==0){
			steps = iteratorNumbers - m_numberOfCycle;
		}

		//int iteratorTimes = 1;
		for(int i=0 ; i<steps ; i++){

			deformFilter->SetCurIterationNum(m_numberOfCycle);
			deformFilter->Update();

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

			//}

			m_numberOfCycle++;
			//----new try remove vessel from appendoutput----------
			//m_PolyData->SetPoints(vpoints);
			m_PolyData->DeepCopy(stentPolyLine);
			m_PolyData->Modified();
			m_PolyData->Update(); 

			mafEvent ev(this, CAMERA_UPDATE);
			ev.SetId(CAMERA_UPDATE);
			ev.SetArg(0);
			//this->OnEvent(&ev);
			ForwardUpEvent(ev);
		}//end of for
		m_numberOfCycle = 0;
	}
}

void medVMEStent::SetCenterLine(vtkPolyData *line){
	if(line){
		this->m_Centerline = line;
	}
}




















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

//-------------------------------------------------------------------------
char** medVMEStent::GetIcon() 
//-------------------------------------------------------------------------
{
 
  #include "mafVMEVolumeLarge.xpm"
  return mafVMEVolumeLarge_xpm;
}

