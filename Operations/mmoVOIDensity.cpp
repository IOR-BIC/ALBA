/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoVOIDensity.cpp,v $
  Language:  C++
  Date:      $Date: 2006-09-22 10:11:57 $
  Version:   $Revision: 1.2 $
  Authors:   Matteo Giacomoni & Paolo Quadrani
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
#include "mafEvent.h"
#include "mmgGui.h"
#include "mafOp.h"
#include "mafString.h"
#include "mmoVOIDensity.h"
#include "mafEventBase.h"
#include "mafVMESurface.h"
#include "mafAbsMatrixPipe.h"

#include "vtkMAFSmartPointer.h"
#include "mafNode.h"

#include "vtkFeatureEdges.h"
#include "vtkDataArray.h"
#include "vtkDoubleArray.h"
#include "vtkPlanes.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkDataSet.h"
#include "vtkImplicitPolyData.h"
#include "vtkTransformPolyDataFilter.h"

#define min(x0, x1) (((x0) < (x1)) ? (x0) : (x1))
#define max(x0, x1) (((x0) > (x1)) ? (x0) : (x1))

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoVOIDensity);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoVOIDensity::mmoVOIDensity(wxString label) 
: mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

  m_Surface     = NULL;
  m_VOIScalars  = NULL;
  
  m_NumberOfScalars = 0;
  m_MeanScalar      = 0.0;
  m_MaxScalar       = -99999.0;
  m_MinScalar       = 99999.0;
  m_StandardDeviation = 0.0;

	m_NumberOfScalarsString = mafString(wxString::Format("%d",m_NumberOfScalars));
  m_MeanScalarString      = mafString(wxString::Format("%f",m_MeanScalar));
  m_MaxScalarString       = mafString(wxString::Format("%f",m_MaxScalar));
  m_MinScalarString       = mafString(wxString::Format("%f",m_MinScalar));
  m_StandardDeviationString = mafString(wxString::Format("%f",m_StandardDeviation));
}
//----------------------------------------------------------------------------
mmoVOIDensity::~mmoVOIDensity( ) 
//----------------------------------------------------------------------------
{
	m_Surface = NULL;
	vtkDEL(m_VOIScalars);
}
//----------------------------------------------------------------------------
mafOp* mmoVOIDensity::Copy()
//----------------------------------------------------------------------------
{
	return (new mmoVOIDensity(m_Label));
}
//----------------------------------------------------------------------------
bool mmoVOIDensity::Accept(mafNode* Node)
//----------------------------------------------------------------------------
{
	return (Node != NULL && Node->IsA("mafVMEVolumeGray"));
}
//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum VOI_DENSITY_WIDGET_ID
{
  ID_CHOOSE_SURFACE = MINID,
  ID_EVALUATE_DENSITY,
  ID_NUMBER_OF_VOXEL_IN_VOI,
	ID_NUM_SCALARS,
	ID_MEAN_SCALAR,
	ID_MIN_SCALAR,
	ID_MAX_SCALAR,
	ID_STANDARD_DEVIATION,
	ID_VOXEL_LIST,
};
//----------------------------------------------------------------------------
void mmoVOIDensity::OpRun()   
//----------------------------------------------------------------------------
{
  vtkNEW(m_VOIScalars);
	m_VMESurfaceAccept = new mafVMESurfaceAccept;
	if(!this->m_TestMode)
	{
		// setup Gui
		m_Gui = new mmgGui(this);
		m_Gui->SetListener(this);

		m_Gui->Divider();
		m_Gui->Button(ID_CHOOSE_SURFACE,"VOI surface");
		m_Gui->Button(ID_EVALUATE_DENSITY,"Evaluate","","Evaluate density inside the choosed surface");
		m_Gui->Divider(2);
		m_Gui->Label("Number of voxel inside the VOI");
		m_Gui->String(ID_NUM_SCALARS,"n=",&m_NumberOfScalarsString,"");
		m_Gui->Label("Voxels's scalar mean inside the VOI");
		m_Gui->String(ID_MEAN_SCALAR,"m=",&m_MeanScalarString,"");
		m_Gui->String(ID_MAX_SCALAR,"max=",&m_MaxScalarString,"");
		m_Gui->String(ID_MIN_SCALAR,"min=",&m_MinScalarString,"");
		m_Gui->String(ID_STANDARD_DEVIATION,"stdev=",&m_StandardDeviationString,"");
		//m_VoxelList=m_Gui->ListBox(ID_VOXEL_LIST);
		m_Gui->Divider(2);
		m_Gui->Divider();
		m_Gui->OkCancel();
	  
		m_Gui->Enable(ID_EVALUATE_DENSITY, false);
		m_Gui->Enable(wxOK, false);
		m_Gui->Update();

		ShowGui();
	}
}
//----------------------------------------------------------------------------
void mmoVOIDensity::OpDo()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmoVOIDensity::OpUndo()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmoVOIDensity::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
			case ID_CHOOSE_SURFACE:
			{
				mafString title = "VOI surface";
				mafEvent event(this,VME_CHOOSE,&title,(long)m_VMESurfaceAccept);
				mafEventMacro(event);
				m_Surface = event.GetVme();
				if(m_Surface == NULL)
					return;
				mafVME *VME=mafVME::SafeDownCast(m_Surface);
				VME->Update();
				vtkMAFSmartPointer<vtkFeatureEdges> FE;
				FE->SetInput((vtkPolyData *)(VME->GetOutput()->GetVTKData()));
				FE->SetFeatureAngle(30);
				FE->SetBoundaryEdges(1);
				FE->SetColoring(0);
				FE->SetFeatureEdges(0);
				FE->SetManifoldEdges(0);
				FE->SetNonManifoldEdges(0);
				FE->Update();

				if(FE->GetOutput()->GetNumberOfCells() != 0)
				{
					//open polydata
					wxMessageBox("Open surface choosed!!", "Alert");
					m_Surface = NULL;
					return;
				}
				
				m_Gui->Enable(ID_EVALUATE_DENSITY, true);
				m_Gui->Enable(wxOK, true);
				VME=NULL;
			}
			break;
			case ID_EVALUATE_DENSITY:
				ExtractVolumeScalars();
			break;
			case wxOK:
				OpStop(OP_RUN_OK);
			break;
			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
			break;
			default:
				mafEventMacro(*e);
			break; 
		}
	}
}
//----------------------------------------------------------------------------
void mmoVOIDensity::OpStop(int result)
//----------------------------------------------------------------------------
{
 	cppDEL(m_VMESurfaceAccept);
	HideGui();
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mmoVOIDensity::ExtractVolumeScalars()
//----------------------------------------------------------------------------
{
	if(!this->m_TestMode)
		wxBusyCursor wait;

  double b[6];
  double Point[3], InsideScalar = 0.0, SumScalars = 0.0;
  int NumberVoxels, PointId;
  
  m_NumberOfScalars = 0;
	mafVMESurface *VME=mafVMESurface::SafeDownCast(m_Surface);
  VME->GetOutput()->GetBounds(b);
	VME->Update();

	vtkMAFSmartPointer<vtkTransformPolyDataFilter> TransformDataClipper;
  TransformDataClipper->SetTransform((vtkAbstractTransform *)VME->GetAbsMatrixPipe()->GetVTKTransform());
  TransformDataClipper->SetInput((vtkPolyData *)VME->GetOutput()->GetVTKData());
  TransformDataClipper->Update();

	vtkMAFSmartPointer<vtkImplicitPolyData> ImplicitSurface;
	ImplicitSurface->SetInput(TransformDataClipper->GetOutput());

	vtkMAFSmartPointer<vtkPlanes> ImplicitBox;
  ImplicitBox->SetBounds(b);
	ImplicitBox->Modified();

  vtkMAFSmartPointer<vtkDataSet> VolumeData = ((mafVME*)m_Input)->GetOutput()->GetVTKData();
  VolumeData->Update();
	NumberVoxels = VolumeData->GetNumberOfPoints();
  
	for (int voxel=0; voxel<NumberVoxels; voxel++)
  {
    VolumeData->GetPoint(voxel,Point);
    if(ImplicitBox->EvaluateFunction(Point) < 0)
    {
      //point is inside the bounding box of the surface: check
      //if the point is also inside the surface.
      if (ImplicitSurface->EvaluateFunction(Point) < 0)
      {
        //store the corresponding point's scalar value
        PointId = VolumeData->FindPoint(Point);
        InsideScalar = VolumeData->GetPointData()->GetTuple(PointId)[0];
        SumScalars += InsideScalar;
        m_MaxScalar = max(InsideScalar,m_MaxScalar);
        m_MinScalar = min(InsideScalar,m_MinScalar);
        m_NumberOfScalars++;
        m_VOIScalars->InsertNextTuple(&InsideScalar);
      }
    }
  }
  if(m_NumberOfScalars > 0)
  {
    m_MeanScalar = SumScalars / m_NumberOfScalars;
  }
  
  double Sum = 0.0;
	double s;
  for (int i=0;i<m_NumberOfScalars; i++)
  {
    m_VOIScalars->GetTuple(i,&s);
    Sum += (s - m_MeanScalar) * (s - m_MeanScalar);
  }
  m_StandardDeviation = sqrt(Sum/m_NumberOfScalars);

  m_NumberOfScalarsString = mafString(wxString::Format("%d",m_NumberOfScalars));
  m_MeanScalarString      = mafString(wxString::Format("%f",m_MeanScalar));
  m_MaxScalarString       = mafString(wxString::Format("%f",m_MaxScalar));
  m_MinScalarString       = mafString(wxString::Format("%f",m_MinScalar));
  m_StandardDeviationString = mafString(wxString::Format("%f",m_StandardDeviation));
	if(!this->m_TestMode)
	{
		m_Gui->Update();
		mafLogMessage(wxString::Format("\nn,m,max,min,stdev\n%d,%.3lf,%.3lf,%.3lf,%.3lf",m_NumberOfScalars,m_MeanScalar,m_MaxScalar,m_MinScalar,m_StandardDeviation));
	}
}