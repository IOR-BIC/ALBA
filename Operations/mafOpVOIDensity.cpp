/*=========================================================================

 Program: MAF2
 Module: mafOpVOIDensity
 Authors: Matteo Giacomoni & Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpVOIDensity.h"
#include "mafGUI.h"

#include "mafVME.h"

#include "mafVMEOutputSurface.h"
#include "mafAbsMatrixPipe.h"

#include "vtkMAFSmartPointer.h"

#include "vtkFeatureEdges.h"
#include "vtkDataArray.h"
#include "vtkDoubleArray.h"
#include "vtkPlanes.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkDataSet.h"
#include "vtkMAFImplicitPolyData.h"
#include "vtkTransformPolyDataFilter.h"


//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpVOIDensity);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpVOIDensity::mafOpVOIDensity(const wxString &label) 
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
mafOpVOIDensity::~mafOpVOIDensity()
//----------------------------------------------------------------------------
{
	m_Surface = NULL;
	vtkDEL(m_VOIScalars);
}
//----------------------------------------------------------------------------
mafOp* mafOpVOIDensity::Copy()
//----------------------------------------------------------------------------
{
	return (new mafOpVOIDensity(m_Label));
}
//----------------------------------------------------------------------------
bool mafOpVOIDensity::Accept(mafVME* Node)
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
void mafOpVOIDensity::OpRun()   
//----------------------------------------------------------------------------
{
  vtkNEW(m_VOIScalars);
	if(!this->m_TestMode)
	{
		// setup Gui
		m_Gui = new mafGUI(this);
		m_Gui->SetListener(this);

		m_Gui->Divider();
		m_Gui->Button(ID_CHOOSE_SURFACE,_("VOI surface"));
		m_Gui->Button(ID_EVALUATE_DENSITY, _("Evaluate"), "", _("Evaluate density inside the choosed surface"));
		m_Gui->Divider(2);
		m_Gui->Label(_("Number of voxel inside the VOI"));
		m_Gui->String(ID_NUM_SCALARS,"n=",&m_NumberOfScalarsString,"");
		m_Gui->Label(_("Voxels's scalar mean inside the VOI"));
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

		m_Gui->Divider();

		ShowGui();
	}
}
//----------------------------------------------------------------------------
void mafOpVOIDensity::OpDo()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpVOIDensity::OpUndo()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpVOIDensity::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
			case ID_CHOOSE_SURFACE:
			{
				mafString title = _("VOI surface");
        mafEvent event(this,VME_CHOOSE,&title,(long)&mafOpVOIDensity::OutputSurfaceAccept);
				mafEventMacro(event);
				m_Surface = event.GetVme();
				if(m_Surface == NULL)
					return;
				m_Surface->Update();
				vtkMAFSmartPointer<vtkFeatureEdges> FE;
				FE->SetInputData((vtkPolyData *)(m_Surface->GetOutput()->GetVTKData()));
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
					mafMessage(_("Open surface choosed!!"), _("Warning"));
					m_Surface = NULL;
					return;
				}
				
				m_Gui->Enable(ID_EVALUATE_DENSITY, true);
				m_Gui->Enable(wxOK, true);
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
void mafOpVOIDensity::ExtractVolumeScalars()
//----------------------------------------------------------------------------
{
	if(!this->m_TestMode)
		wxBusyCursor wait;

  double b[6];
  double Point[3], InsideScalar = 0.0, SumScalars = 0.0;
  int NumberVoxels, PointId;
  
  m_NumberOfScalars = 0;
	vtkAbstractTransform *transform;
	vtkPolyData *polydata;
	m_Surface->GetOutput()->GetBounds(b);
	m_Surface->Update();
	transform=(vtkAbstractTransform*)m_Surface->GetAbsMatrixPipe()->GetVTKTransform();
	polydata=(vtkPolyData *)m_Surface->GetOutput()->GetVTKData();

	vtkMAFSmartPointer<vtkTransformPolyDataFilter> TransformDataClipper;
  TransformDataClipper->SetTransform(transform);
  TransformDataClipper->SetInputData(polydata);
  TransformDataClipper->Update();

	vtkMAFSmartPointer<vtkMAFImplicitPolyData> ImplicitSurface;
	ImplicitSurface->SetInput(TransformDataClipper->GetOutput());

	vtkMAFSmartPointer<vtkPlanes> ImplicitBox;
  ImplicitBox->SetBounds(b);
	ImplicitBox->Modified();

  vtkMAFSmartPointer<vtkDataSet> VolumeData = m_Input->GetOutput()->GetVTKData();
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
        InsideScalar = VolumeData->GetPointData()->GetScalars()->GetTuple(PointId)[0];
        SumScalars += InsideScalar;
        m_MaxScalar = MAX(InsideScalar,m_MaxScalar);
        m_MinScalar = MIN(InsideScalar,m_MinScalar);
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
