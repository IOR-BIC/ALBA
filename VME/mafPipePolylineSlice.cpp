/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipePolylineSlice.cpp,v $
  Language:  C++
  Date:      $Date: 2008-12-19 16:05:47 $
  Version:   $Revision: 1.16.2.2 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafPipePolylineSlice.h"
#include "mafSceneNode.h"
#include "mafGUI.h"
#include "mafAxes.h"

#include "mmaMaterial.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMEPolyline.h"
#include "mafVMEOutputPolyline.h"
#include "mafAbsMatrixPipe.h"
#include "mafEventSource.h"

#include "vtkMAFAssembly.h"
#include "vtkRenderer.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkPointData.h"
#include "vtkMAFFixedCutter.h"
#include "vtkPlane.h"
#include "vtkMAFToLinearTransform.h"
#include "vtkTubeFilter.h"
#include "vtkPolyData.h"
#include "vtkCardinalSpline.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkMAFSmartPointer.h"
#include "vtkMAFPolyDataToSinglePolyLine.h"
#include "vtkClipPolyData.h"
#include "vtkMAFImplicitPolyData.h"
#include "vtkCubeSource.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkAppendPolyData.h"


#include <vector>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipePolylineSlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipePolylineSlice::mafPipePolylineSlice()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineBox      = NULL;
  m_OutlineMapper   = NULL;
  m_OutlineProperty = NULL;
  m_OutlineActor    = NULL;
  m_Cutter          = NULL;
  m_ClipPolyData    = NULL;
  m_ClipPolyDataUp    = NULL;
  m_ClipPolyDataDown    = NULL;
  m_Plane	    = NULL;
  m_Tube = NULL;
//m_TubeRadial = NULL;

  m_Origin[0] = 0;
  m_Origin[1] = 0;
  m_Origin[2] = 0;

  m_Normal[0] = 0;
  m_Normal[1] = 0;
  m_Normal[2] = 0;

  m_ScalarVisibility = 0;
  m_RenderingDisplayListFlag = 0;
  m_Border=1;
  m_Radius=1.0;

  m_SplineMode = 0;
  m_SplineCoefficient = 10.0;

  m_Fill = 0;

  m_PolydataToPolylineFilter = NULL;

  m_RoiEnable = FALSE;
  m_ROI[0] = m_ROI[2] = m_ROI[4] = VTK_DOUBLE_MIN;
  m_ROI[1] = m_ROI[3] = m_ROI[5] = VTK_DOUBLE_MAX;

  m_AppendPolyData = NULL;
}
//----------------------------------------------------------------------------
void mafPipePolylineSlice::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);
  
  m_Selected = false;
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineBox      = NULL;
  m_OutlineMapper   = NULL;
  m_OutlineProperty = NULL;
  m_OutlineActor    = NULL;
  m_Axes            = NULL;
  m_PolySpline      = NULL;

  m_Vme->GetEventSource()->AddObserver(this);

  assert(m_Vme->GetOutput()->IsMAFType(mafVMEOutputPolyline));
  mafVMEOutputPolyline *polyline_output = mafVMEOutputPolyline::SafeDownCast(m_Vme->GetOutput());
  assert(polyline_output);
  polyline_output->Update();
  vtkPolyData *data = vtkPolyData::SafeDownCast(polyline_output->GetVTKData());
  data->Update();
  assert(data);
  vtkDataArray *scalars = data->GetPointData()->GetScalars();
  double sr[2] = {0,1};

  vtkNEW(m_PolydataToPolylineFilter);
  m_PolydataToPolylineFilter->SetInput(data);
  m_PolydataToPolylineFilter->Update();

	//////////////////////////////////
  vtkNEW(m_Tube);
	m_Tube->UseDefaultNormalOn();
	m_Tube->SetInput(m_PolydataToPolylineFilter->GetOutput());
	m_Tube->SetRadius(m_Radius);
	m_Tube->SetCapping(1);
	m_Tube->SetNumberOfSides(16);
	m_Tube->Update();
	
	//data = m_Tube->GetOutput();
	//////////////////////////////////

	m_Plane	= vtkPlane::New();
	m_Cutter = vtkMAFFixedCutter::New();

	m_Plane->SetOrigin(m_Origin);
	m_Plane->SetNormal(m_Normal);
  vtkNEW(m_VTKTransform);
  m_VTKTransform->SetInputMatrix(m_Vme->GetAbsMatrixPipe()->GetMatrixPointer());
	m_Plane->SetTransform(m_VTKTransform);

	m_Cutter->SetInput(m_Tube->GetOutput());
	m_Cutter->SetCutFunction(m_Plane);
	m_Cutter->Update();

  if(scalars != NULL)
  {
    m_ScalarVisibility = 1;
    scalars->GetRange(sr);
  }

  m_Mapper = vtkPolyDataMapper::New();

  vtkNEW(m_AppendPolyData);

  if(m_Fill)
    m_PolyData = RegionsCapping(m_Cutter->GetOutput());
  else
    m_PolyData = m_Cutter->GetOutput();

  vtkNEW(m_ClipPolyData);
  vtkNEW(m_ClipPolyDataUp);
  vtkNEW(m_ClipPolyDataDown);

  vtkPolyData *intermediatePolyData = NULL;
  if(m_RoiEnable)
  {
    intermediatePolyData = ExecuteROI(m_PolyData);
    m_Mapper->SetInput(intermediatePolyData);
  }
  else
  {
    m_Mapper->SetInput(m_PolyData);
  }


  m_Mapper->SetScalarVisibility(m_ScalarVisibility);
  m_Mapper->SetScalarRange(sr);
  
	if(m_Vme->IsAnimated())
  {
    m_RenderingDisplayListFlag = 1;
    m_Mapper->ImmediateModeRenderingOn();	 //avoid Display-Lists for animated items.
  }
	else
  {
    m_RenderingDisplayListFlag = 0;
    m_Mapper->ImmediateModeRenderingOff();
  }
  m_Mapper->Update();

  m_Actor = vtkActor::New();
  m_Actor->SetMapper(m_Mapper);

	m_Actor->GetProperty()->SetColor(((mafVMEOutputPolyline *)((mafVME *)m_Vme)->GetOutput())->GetMaterial()->m_Diffuse);
  m_Actor->GetProperty()->SetLineWidth (1);
  m_AssemblyFront->AddPart(m_Actor);

  // selection highlight
  m_OutlineBox = vtkOutlineCornerFilter::New();
	m_OutlineBox->SetInput(data);  

	m_OutlineMapper = vtkPolyDataMapper::New();
	m_OutlineMapper->SetInput(m_OutlineBox->GetOutput());

	m_OutlineProperty = vtkProperty::New();
	m_OutlineProperty->SetColor(1,1,1);
	m_OutlineProperty->SetAmbient(1);
	m_OutlineProperty->SetRepresentationToWireframe();
	m_OutlineProperty->SetInterpolationToFlat();

	m_OutlineActor = vtkActor::New();
	m_OutlineActor->SetMapper(m_OutlineMapper);
	m_OutlineActor->VisibilityOff();
	m_OutlineActor->PickableOff();
	m_OutlineActor->SetProperty(m_OutlineProperty);

  m_AssemblyFront->AddPart(m_OutlineActor);

  m_Axes = new mafAxes(m_RenFront, m_Vme);
  m_Axes->SetVisibility(0);

  /*
  m_axes = NULL;
	if(m_use_axes) m_axes = new mafAxes(m_ren1,m_Vme);
	if(m_use_axes) m_axes->SetVisibility(0);
	*/
  //CreateGui();
}
//----------------------------------------------------------------------------
mafPipePolylineSlice::~mafPipePolylineSlice()
//----------------------------------------------------------------------------
{
  m_Vme->GetEventSource()->RemoveObserver(this);

  m_AssemblyFront->RemovePart(m_Actor);
  m_AssemblyFront->RemovePart(m_OutlineActor);


  vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
  //vtkDEL(m_TubeRadial);
  vtkDEL(m_OutlineBox);
  vtkDEL(m_OutlineMapper);
  vtkDEL(m_OutlineProperty);
  vtkDEL(m_OutlineActor);
  vtkDEL(m_Cutter);
  vtkDEL(m_AppendPolyData);
  vtkDEL(m_ClipPolyData);
  vtkDEL(m_ClipPolyDataUp);
  vtkDEL(m_ClipPolyDataDown);
  vtkDEL(m_Plane);
  vtkDEL(m_VTKTransform);
  vtkDEL(m_Tube);
  vtkDEL(m_PolydataToPolylineFilter);
  vtkDEL(m_PolySpline);
  cppDEL(m_Axes);
	//@@@ if(m_use_axes) wxDEL(m_axes);  
}
//----------------------------------------------------------------------------
void mafPipePolylineSlice::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_Actor->GetVisibility()) 
	{
			//m_OutlineActor->SetVisibility(sel);
      //m_Axes->SetVisibility(sel);
	}
}
//----------------------------------------------------------------------------
mafGUI *mafPipePolylineSlice::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);
  m_Gui->FloatSlider(ID_BORDER_CHANGE,_("Border"),&m_Border,1.0,5.0);
  m_Gui->FloatSlider(ID_RADIUS_CHANGE,_("Radius"),&m_Radius,0.1,2.5);
  m_Gui->Bool(ID_SPLINE,_("spline"),&m_SplineMode);
  m_Gui->Bool(ID_FILL,_("Fill"),&m_Fill);
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipePolylineSlice::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
	  case ID_BORDER_CHANGE:
		  {
			  m_Actor->GetProperty()->SetLineWidth(m_Border);
			  m_Actor->Modified();
			  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
		  }
	  break;
    case ID_RADIUS_CHANGE:
      {
        SetRadius(m_Radius);
      }
      break;
    case ID_SPLINE:
      {
        UpdateProperty();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    case ID_FILL:
      {
        if(m_Fill)
          m_Mapper->SetInput(RegionsCapping(m_Cutter->GetOutput()));
        else
          m_Mapper->SetInput(m_Cutter->GetOutput());
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
      default:
        mafEventMacro(*e);
      break;
    }
  }
  else if (maf_event->GetId() == VME_OUTPUT_DATA_UPDATE && maf_event->GetSender() == m_Vme)
  {
    if(m_Vme->GetOutput() && m_Vme->GetOutput()->GetVTKData() && m_Actor)
      UpdateProperty();
  }
}
//----------------------------------------------------------------------------
void mafPipePolylineSlice::SetSlice(double *Origin)
//----------------------------------------------------------------------------
{
	m_Origin[0] = Origin[0];
	m_Origin[1] = Origin[1];
	m_Origin[2] = Origin[2];
	
	if(m_Plane && m_Cutter)
	{
		m_Plane->SetOrigin(m_Origin);
		m_Cutter->SetCutFunction(m_Plane);
		m_Cutter->Update();
        UpdateProperty();
	}
  if(m_Vme != NULL)
    m_Actor->GetProperty()->SetColor(((mafVMEOutputPolyline *)((mafVME *)m_Vme)->GetOutput())->GetMaterial()->m_Diffuse);
}
//----------------------------------------------------------------------------
void mafPipePolylineSlice::SetNormal(double *Normal)
//----------------------------------------------------------------------------
{
	m_Normal[0] = Normal[0];
	m_Normal[1] = Normal[1];
	m_Normal[2] = Normal[2];


	if(m_Plane && m_Cutter)
	{
		m_Plane->SetNormal(m_Normal);
		m_Cutter->SetCutFunction(m_Plane);
		m_Cutter->Update();
        UpdateProperty();
	}
}
//----------------------------------------------------------------------------
double mafPipePolylineSlice::GetThickness()
//----------------------------------------------------------------------------
{
	return m_Border;
}
//----------------------------------------------------------------------------
void mafPipePolylineSlice::SetThickness(double thickness)
//----------------------------------------------------------------------------
{
	m_Border=thickness;
	m_Actor->GetProperty()->SetLineWidth(m_Border);
  m_Actor->GetProperty()->SetPointSize(m_Border);

  m_Actor->GetProperty()->SetColor(((mafVMEOutputPolyline *)((mafVME *)m_Vme)->GetOutput())->GetMaterial()->m_Diffuse);
  
  m_Actor->Modified();
	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
double mafPipePolylineSlice::GetRadius()
//----------------------------------------------------------------------------
{
	return m_Radius;
}
//----------------------------------------------------------------------------
void mafPipePolylineSlice::SetRadius(double radius)
//----------------------------------------------------------------------------
{
	m_Radius=radius;
  if(m_Tube)
  {
    m_Tube->SetRadius(m_Radius);
    m_Tube->Update();
    m_Actor->GetProperty()->SetColor(((mafVMEOutputPolyline *)((mafVME *)m_Vme)->GetOutput())->GetMaterial()->m_Diffuse);
    m_Actor->Modified();
  }
	//mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafPipePolylineSlice::UpdateProperty()
//----------------------------------------------------------------------------
{
  mafVMEOutputPolyline *out_polyline = mafVMEOutputPolyline::SafeDownCast(m_Vme->GetOutput());
  if(out_polyline == NULL) return;
  out_polyline->Update();
  vtkPolyData *data = vtkPolyData::SafeDownCast(out_polyline->GetVTKData());
  if(data == NULL) return;
  data->Modified();
  data->Update();

  if(m_SplineMode)
    data = SplineProcess(data);

  data->Modified();
  data->Update();

  m_PolydataToPolylineFilter->SetInput(data);
  m_PolydataToPolylineFilter->Update();


  m_Tube->SetInput(m_PolydataToPolylineFilter->GetOutput());
  m_Tube->Update();
  m_Cutter->SetInput(m_Tube->GetOutput());
  m_Cutter->Update();
  
  if(m_Fill)
    m_PolyData = RegionsCapping(m_Cutter->GetOutput());
  else
    m_PolyData = m_Cutter->GetOutput();

  vtkPolyData *intermediatePolyData = NULL;
  if(m_RoiEnable)
  {
    intermediatePolyData = ExecuteROI(m_PolyData);
    m_Mapper->SetInput(intermediatePolyData);
  }
  else
  {
    m_Mapper->SetInput(m_PolyData);
  }

  m_Mapper->Update();

}
//----------------------------------------------------------------------------
vtkPolyData *mafPipePolylineSlice::SplineProcess(vtkPolyData *polyData)
//----------------------------------------------------------------------------
{
  //cleaned point list
  vtkMAFSmartPointer<vtkPoints> pts;
  vtkMAFSmartPointer<vtkPoints> ptsSplined;

  vtkNEW(m_PolySpline);

  pts->DeepCopy(polyData->GetPoints());

  vtkMAFSmartPointer<vtkCardinalSpline> splineX;
  vtkMAFSmartPointer<vtkCardinalSpline> splineY;
  vtkMAFSmartPointer<vtkCardinalSpline> splineZ;



  for(int i=0 ; i<pts->GetNumberOfPoints(); i++)
  {
    //mafLogMessage(wxString::Format(_("old %d : %f %f %f"), i, pts->GetPoint(i)[0],pts->GetPoint(i)[1],pts->GetPoint(i)[2] ));
    splineX->AddPoint(i, pts->GetPoint(i)[0]);
    splineY->AddPoint(i, pts->GetPoint(i)[1]);
    splineZ->AddPoint(i, pts->GetPoint(i)[2]);
  }

  for(int i=0 ; i<(pts->GetNumberOfPoints() * m_SplineCoefficient); i++)
  {		 
    double t;
    t = ( pts->GetNumberOfPoints() - 1.0 ) / ( pts->GetNumberOfPoints()*m_SplineCoefficient - 1.0 ) * i;
    ptsSplined->InsertPoint(i , splineX->Evaluate(t), splineY->Evaluate(t), splineZ->Evaluate(t));

  }


  m_PolySpline->SetPoints(ptsSplined);
  m_PolySpline->Update();

  //order
  //cell 
  vtkCellArray *cellArray;
  vtkNEW(cellArray);
  int pointId[2];

  for(int i = 0; i< m_PolySpline->GetNumberOfPoints();i++)
  {
    if (i > 0)
    {             
      pointId[0] = i - 1;
      pointId[1] = i;
      cellArray->InsertNextCell(2 , pointId);  
    }
  }

  m_PolySpline->SetLines(cellArray);
  m_PolySpline->Modified();
  m_PolySpline->Update();

  vtkDEL(cellArray);

  return m_PolySpline;
}
//----------------------------------------------------------------------------
void mafPipePolylineSlice::ShowActorOn()
//----------------------------------------------------------------------------
{
  if(m_Actor != NULL)
  {
    m_Actor->SetVisibility(true);
  }
}
//----------------------------------------------------------------------------
void mafPipePolylineSlice::ShowActorOff()
//----------------------------------------------------------------------------
{
  if(m_Actor != NULL)
  {
    m_Actor->SetVisibility(false);
  }
}
//----------------------------------------------------------------------------
void mafPipePolylineSlice::SetROI(double bounds[6])
//----------------------------------------------------------------------------
{
  m_ROI[0] = bounds[0];
  m_ROI[1] = bounds[1];
  m_ROI[2] = bounds[2];
  m_ROI[3] = bounds[3];
  m_ROI[4] = bounds[4];
  m_ROI[5] = bounds[5];
}
//----------------------------------------------------------------------------
void mafPipePolylineSlice::SetMaximumROI()
//----------------------------------------------------------------------------
{
  if(m_PolyData)
  {
    double bb[6];
    m_PolyData->GetBounds(bb);

    m_ROI[0] = bb[0];
    m_ROI[1] = bb[1];

    m_ROI[2] = bb[2];
    m_ROI[3] = bb[3];

    m_ROI[4] = bb[4];
    m_ROI[5] = bb[5];
  }
}
//----------------------------------------------------------------------------
vtkPolyData *mafPipePolylineSlice::ExecuteROI(vtkPolyData *polydata)
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkCubeSource> cube;
  cube->SetBounds(m_ROI);
  cube->Modified();
  cube->Update();

  vtkMAFSmartPointer<vtkMAFImplicitPolyData> implicitDataset;
  implicitDataset->SetInput(cube->GetOutput());

  m_ClipPolyData->SetInput(polydata);
  m_ClipPolyData->InsideOutOn();
  m_ClipPolyData->SetClipFunction(implicitDataset);
  m_ClipPolyData->Update();

  vtkMAFSmartPointer<vtkPlane> planeDown;
  planeDown->SetOrigin(m_ROI[0], m_ROI[2], m_ROI[4]);
  double normaldown[3] = {0,0,-1};
  planeDown->SetNormal(normaldown);
  planeDown->Modified();

  m_ClipPolyDataUp->SetInput(m_ClipPolyData->GetOutput());
  m_ClipPolyDataUp->SetClipFunction(planeDown);
  m_ClipPolyDataUp->InsideOutOn();
  m_ClipPolyDataUp->Update();

  vtkMAFSmartPointer<vtkPlane> planeUp;
  planeUp->SetOrigin(m_ROI[1], m_ROI[3], m_ROI[5]);
  double normalUp[3] = {0,0,1};
  planeUp->SetNormal(normalUp);
  planeUp->Modified();

  m_ClipPolyDataDown->SetInput(m_ClipPolyDataUp->GetOutput());
  m_ClipPolyDataDown->SetClipFunction(planeUp);
  m_ClipPolyDataDown->InsideOutOn();
  m_ClipPolyDataDown->Update();

  vtkPolyData *resultPolyData = m_ClipPolyDataDown->GetOutput();

  return resultPolyData;
}
//----------------------------------------------------------------------------
vtkPolyData *mafPipePolylineSlice::RegionsCapping(vtkPolyData* inputBorder)
//----------------------------------------------------------------------------
{
  m_AppendPolyData->RemoveAllInputs();
  vtkMAFSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter;
  connectivityFilter->SetInput(inputBorder);
  connectivityFilter->SetExtractionModeToSpecifiedRegions();
  connectivityFilter->Update();
  int regionNumbers = connectivityFilter->GetNumberOfExtractedRegions();

  for(int region = 0; region < regionNumbers; region++)
  {
    connectivityFilter->InitializeSpecifiedRegionList();
    connectivityFilter->AddSpecifiedRegion(region);
    connectivityFilter->Update();
    connectivityFilter->GetOutput()->Update();

    vtkMAFSmartPointer<vtkPolyData> p;
    
    //write polydata
    
    p->SetPoints(connectivityFilter->GetOutput()->GetPoints());
    p->SetLines(connectivityFilter->GetOutput()->GetLines());
    p->Update();
    /*mafString filename1 = "C:\\conn_";
    filename1 << region;
    filename1 << ".vtk";
    vtkMAFSmartPointer<vtkPolyDataWriter> pdWriter;
    pdWriter->SetInput(p);
    pdWriter->SetFileName(filename1);
    pdWriter->Update();*/
    //end write polydata

    p->DeepCopy(CappingFilter(p));

    /*mafString filename2 = "C:\\connCAPP_";
    filename2 << region;
    filename2 << ".vtk";

    pdWriter->SetInput(p);
    pdWriter->SetFileName(filename2);
    pdWriter->Update();*/
    //end write polydata
    
    m_AppendPolyData->AddInput(p);
    m_AppendPolyData->Update();
  }

  return m_AppendPolyData->GetOutput();
}
//----------------------------------------------------------------------------
vtkPolyData *mafPipePolylineSlice::CappingFilter(vtkPolyData* inputBorder)
//----------------------------------------------------------------------------
{
  int i, iCell;
  inputBorder->Update();
  // prerequisites: connected polydata with line cells that represent the edge of the hole to be capped. 
  // search average point
  double averagePoint[3] = {0.0,0.0,0.0};
  vtkMAFSmartPointer<vtkPoints>outputPoints;
  vtkMAFSmartPointer<vtkCellArray> outputCellArray;
  vtkPolyData *output;
  vtkNEW(output);
  outputPoints->DeepCopy(inputBorder->GetPoints());

  for(i = 0;i<inputBorder->GetNumberOfPoints();i++)
  {
    double currentPoint[3];
    inputBorder->GetPoint(i, currentPoint);
    averagePoint[0] += currentPoint[0];
    averagePoint[1] += currentPoint[1];
    averagePoint[2] += currentPoint[2];
  }
  // the new polydata that represents capping has input->NPoints + 1 points: the averagePoint
  double center[3];
  inputBorder->GetCenter(center);

  averagePoint[0] /= inputBorder->GetNumberOfPoints(); 
  averagePoint[1] /= inputBorder->GetNumberOfPoints();
  averagePoint[2] /= inputBorder->GetNumberOfPoints();
  outputPoints->InsertNextPoint(center);
  output->SetPoints(outputPoints);
  // create triangular cells with the new point.
  for(int i=0; i<inputBorder->GetNumberOfCells();i++)
  {
    //each line of the inputPolydata should be transformed into a triangle.
    vtkMAFSmartPointer<vtkIdList> currentCellIds;
    for (iCell = 0; iCell < inputBorder->GetCell(i)->GetNumberOfPoints(); iCell++)
    {
      currentCellIds->InsertNextId(inputBorder->GetCell(i)->GetPointIds()->GetId(iCell));
    }
    // write the last id (the averagePoint) in the current cell Id list
    currentCellIds->InsertNextId(inputBorder->GetNumberOfPoints());
    // insert the Id list in the cell array
    outputCellArray->InsertNextCell(currentCellIds);
  }
  // set the cell array to the polydata
  output->SetPolys(outputCellArray);
  output->Update();

  return output;
}
