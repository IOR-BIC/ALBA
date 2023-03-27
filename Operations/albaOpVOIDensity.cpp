/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpVOIDensity
 Authors: Matteo Giacomoni & Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaOpVOIDensity.h"
#include "albaGUI.h"

#include "albaVME.h"

#include "albaVMEOutputSurface.h"
#include "albaAbsMatrixPipe.h"

#include "vtkALBASmartPointer.h"

#include "vtkFeatureEdges.h"
#include "vtkDataArray.h"
#include "vtkDoubleArray.h"
#include "vtkPlanes.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkDataSet.h"
#include "vtkALBAImplicitPolyData.h"
#include "vtkTransformPolyDataFilter.h"
#include "albaProgressBarHelper.h"
#include "albaTagArray.h"
#include "albaVMEPointCloud.h"
#include "vtkCellArray.h"
#include "vtkTransform.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpVOIDensity);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpVOIDensity::albaOpVOIDensity(const wxString &label) 
: albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

  m_Surface     = NULL;
  m_VOIScalars  = NULL;
  
  m_NumberOfScalars = 0;
  m_MeanScalar      = 0.0;
  m_MaxScalar       = 0.0;
  m_MinScalar       = 0.0;
  m_StandardDeviation = 0.0;
	m_Median = 0.0;

	UpdateStrings();

	m_EvaluateInSubRange = false;
	m_SubRange[0] = m_SubRange[1] = 0;
}
//----------------------------------------------------------------------------
albaOpVOIDensity::~albaOpVOIDensity()
//----------------------------------------------------------------------------
{
	m_Surface = NULL;
	vtkDEL(m_VOIScalars);
	m_VOICoords.clear();
}
//----------------------------------------------------------------------------
albaOp* albaOpVOIDensity::Copy()
//----------------------------------------------------------------------------
{
	return (new albaOpVOIDensity(m_Label));
}
//----------------------------------------------------------------------------
bool albaOpVOIDensity::InternalAccept(albaVME* Node)
//----------------------------------------------------------------------------
{
	return (Node != NULL && Node->IsA("albaVMEVolumeGray"));
}
//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum VOI_DENSITY_WIDGET_ID
{
	ID_CHOOSE_SURFACE = MINID,
	ID_EVALUATE_DENSITY,
	ID_ENABLE_RANGE,
	ID_RANGE_UPDATED,
  ID_NUMBER_OF_VOXEL_IN_VOI,
	ID_NUM_SCALARS,
	ID_MEAN_SCALAR,
	ID_MIN_SCALAR,
	ID_MAX_SCALAR,
	ID_STANDARD_DEVIATION,
	ID_MEDIAN,
	ID_VOXEL_LIST,
};
//----------------------------------------------------------------------------
void albaOpVOIDensity::OpRun()   
//----------------------------------------------------------------------------
{
  vtkNEW(m_VOIScalars);
	if(!this->m_TestMode)
	{
		vtkALBASmartPointer<vtkDataSet> VolumeData = m_Input->GetOutput()->GetVTKData();
		VolumeData->Update();

		VolumeData->GetPointData()->GetScalars()->GetRange(m_SubRange);

		CreateGui();

		ShowGui();
	}
}

//----------------------------------------------------------------------------
void albaOpVOIDensity::CreateGui()
{
	// setup Gui
	m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);

	m_Gui->Divider();
	m_Gui->Button(ID_CHOOSE_SURFACE, _("VOI surface"));
	m_Gui->Button(ID_EVALUATE_DENSITY, _("Evaluate"), "", _("Evaluate density inside the choosed surface"));
	m_Gui->Divider();
	m_Gui->Bool(ID_ENABLE_RANGE, "Evaluate only in subrange", &m_EvaluateInSubRange, true);
	m_Gui->VectorN(ID_RANGE_UPDATED, "Range:", m_SubRange, 2, m_SubRange[0], m_SubRange[1]);
	m_Gui->Divider(2);
	m_Gui->Label(_("Number of voxel inside the VOI"));
	m_Gui->String(ID_NUM_SCALARS, "Num=", &m_NumberOfScalarsString, "");
	m_Gui->Label(_("Voxels's scalar mean inside the VOI"));
	m_Gui->String(ID_MEAN_SCALAR, "Mean=", &m_MeanScalarString, "");
	m_Gui->String(ID_MEDIAN, "Median=", &m_MedianString, "");
	m_Gui->String(ID_MIN_SCALAR, "Min=", &m_MinScalarString, "");
	m_Gui->String(ID_MAX_SCALAR, "Max=", &m_MaxScalarString, "");
	m_Gui->String(ID_STANDARD_DEVIATION, "Std dev=", &m_StandardDeviationString, "");
	//m_VoxelList=m_Gui->ListBox(ID_VOXEL_LIST);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	m_Gui->Enable(ID_EVALUATE_DENSITY, false);
	m_Gui->Enable(ID_RANGE_UPDATED, false);
	m_Gui->Enable(wxOK, false);
	m_Gui->Update();
}

//----------------------------------------------------------------------------
void albaOpVOIDensity::OpDo()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpVOIDensity::OpUndo()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaOpVOIDensity::OpStop(int result)
{
	if (result == OP_RUN_OK)
	{
		SetDoubleTag("NumberOfScalars", m_NumberOfScalars);
		SetDoubleTag("MeanScalar", m_MeanScalar);
		SetDoubleTag("MaxScalar", m_MaxScalar);
		SetDoubleTag("MinScalar", m_MinScalar);
		SetDoubleTag("StandardDeviation", m_StandardDeviation);
		SetDoubleTag("Median:", m_Median);

		CreatePointSamplingOutput();
	}

	Superclass::OpStop(result);
}

//----------------------------------------------------------------------------
void albaOpVOIDensity::SetDoubleTag(wxString tagName, double value)
{
	albaTagItem tagItem;
	tagItem.SetName("VOI_" + tagName);
	tagItem.SetValue(value);

	if (m_Surface->GetTagArray()->IsTagPresent("VOI_" + tagName))
		m_Surface->GetTagArray()->DeleteTag("VOI_" + tagName);

	m_Surface->GetTagArray()->SetTag(tagItem);
}

//----------------------------------------------------------------------------
void albaOpVOIDensity::CreatePointSamplingOutput()
{

	albaString name = m_Surface->GetName();
	name += " sampling";

	albaVMEPointCloud *pointCloudVME;
	albaNEW(pointCloudVME);
	pointCloudVME->SetName(name);

	vtkPolyData * polydata;
	vtkNEW(polydata);

	vtkPoints *newPoints;
	vtkNEW(newPoints);

	//generate cell structure
	vtkCellArray * polys;
	vtkNEW(polys);

	vtkDoubleArray *newArray;


	vtkNEW(newArray);
	newArray->SetName("Vol Scalars");

	int nPoints = m_VOICoords.size();


	for (int i = 0; i < nPoints; i++)
	{
		newArray->InsertNextValue(m_VOIScalars->GetTuple1(i));

		newPoints->InsertNextPoint(m_VOICoords[i].GetVect());

		polys->InsertNextCell(3);
		polys->InsertCellPoint(i);
		polys->InsertCellPoint(i);
		polys->InsertCellPoint(i);
	}


	vtkPointData *outPointData = polydata->GetPointData();

	outPointData->AddArray(newArray);
	vtkDEL(newArray);


	polydata->SetPoints(newPoints);
	vtkDEL(newPoints);

	polydata->SetPolys(polys);
	vtkDEL(polys);

	polydata->Modified();
	polydata->Update();
	pointCloudVME->SetData(polydata, 0);
	vtkDEL(polydata);

	pointCloudVME->ReparentTo(m_Surface);

	albaMatrix identityM;
	pointCloudVME->SetAbsMatrix(identityM);

	albaDEL(pointCloudVME);
}

//----------------------------------------------------------------------------
double albaOpVOIDensity::GetMedian(vtkDoubleArray *valuesArray)
{
	int nTuples = valuesArray->GetNumberOfTuples();

	if (nTuples < 1)
		return 0;

	double retValue;
	double *values = new double[nTuples];

	for (int i = 0; i < nTuples; i++)
		values[i] = valuesArray->GetTuple1(i);

	qsort(values, nTuples, sizeof(double), Cmpfunc);
	
	retValue = values[nTuples / 2];
	
	delete[] values;

	return retValue;
}

//----------------------------------------------------------------------------
int albaOpVOIDensity::Cmpfunc(const void * a, const void * b)
{
	return (int)(*(double*)a - *(double*)b);
}

//----------------------------------------------------------------------------
int albaOpVOIDensity::SetSurface(albaVME *surface)
{
	m_Surface = surface;
	if (m_Surface == NULL)
		return ALBA_ERROR;
	m_Surface->Update();
	vtkALBASmartPointer<vtkFeatureEdges> FE;
	FE->SetInput((vtkPolyData *)(m_Surface->GetOutput()->GetVTKData()));
	FE->SetFeatureAngle(30);
	FE->SetBoundaryEdges(1);
	FE->SetColoring(0);
	FE->SetFeatureEdges(0);
	FE->SetManifoldEdges(0);
	FE->SetNonManifoldEdges(0);
	FE->Update();

	if (FE->GetOutput()->GetNumberOfCells() != 0)
	{
		//open polydata
		albaMessage(_("Open surface choosed!!"), _("Warning"));
		m_Surface = NULL;
		return ALBA_ERROR;
	}

	return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaOpVOIDensity::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId())
		{
			case ID_CHOOSE_SURFACE:
			{
				albaString title = _("VOI surface");
        albaEvent event(this,VME_CHOOSE,&title);
				event.SetPointer(&albaOpVOIDensity::OutputSurfaceAccept);
				albaEventMacro(event);

				if (SetSurface(event.GetVme())==ALBA_ERROR)
					return;
				
				m_Gui->Enable(ID_EVALUATE_DENSITY, true);
				m_Gui->Enable(wxOK, false);
			}
			break;
			case ID_ENABLE_RANGE:
				m_Gui->Enable(ID_RANGE_UPDATED, m_EvaluateInSubRange);
				break;
			case ID_RANGE_UPDATED:
				m_Gui->Update();
				if (m_SubRange[0] > m_SubRange[1])
					m_SubRange[1] = m_SubRange[0];
				else if (m_SubRange[1] < m_SubRange[0])
					m_SubRange[1] = m_SubRange[0];
				m_Gui->Update();
				break;
			case ID_EVALUATE_DENSITY:
				ExtractVolumeScalars();
				m_Gui->Enable(wxOK, true);
			break;
			case wxOK:
				OpStop(OP_RUN_OK);
			break;
			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
			break;
			default:
				albaEventMacro(*e);
			break; 
		}
	}
}
//----------------------------------------------------------------------------
void albaOpVOIDensity::ExtractVolumeScalars()
//----------------------------------------------------------------------------
{
	if (!this->m_TestMode)
		wxBusyCursor wait;

	double b[6];
	double Point[3], InsideScalar = 0.0, SumScalars = 0.0;
	int NumberVoxels, PointId;

	// Reset parameters
	m_NumberOfScalars = 0;
	m_MaxScalar = -99999.0;
	m_MinScalar = 99999.0;
	m_VOIScalars->Reset();
	m_VOICoords.clear();

	albaMatrix inputMeshABSMatrix = m_Surface->GetAbsMatrixPipe()->GetMatrix();
	albaMatrix inputVolumeABSMatrix = m_Input->GetAbsMatrixPipe()->GetMatrix();

	//Calculate align matrix 
	albaMatrix alignMatrix;
	inputVolumeABSMatrix.Invert();
	albaMatrix::Multiply4x4(inputVolumeABSMatrix, inputMeshABSMatrix, alignMatrix);

	vtkTransform *transform = NULL;
	transform = vtkTransform::New();
	transform->SetMatrix(alignMatrix.GetVTKMatrix());

	vtkPolyData *polydata;
	m_Surface->Update();
	polydata = (vtkPolyData *)m_Surface->GetOutput()->GetVTKData();

	vtkALBASmartPointer<vtkTransformPolyDataFilter> TransformDataFilter;
	TransformDataFilter->SetTransform(transform);
	TransformDataFilter->SetInput(polydata);
	TransformDataFilter->Update();

	vtkALBASmartPointer<vtkALBAImplicitPolyData> ImplicitSurface;
	ImplicitSurface->SetInput(TransformDataFilter->GetOutput());

	TransformDataFilter->GetOutput()->GetBounds(b);

	vtkALBASmartPointer<vtkPlanes> ImplicitBox;
	ImplicitBox->SetBounds(b);
	ImplicitBox->Modified();

	vtkALBASmartPointer<vtkDataSet> VolumeData = m_Input->GetOutput()->GetVTKData();
	VolumeData->Update();
	NumberVoxels = VolumeData->GetNumberOfPoints();

	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Evaluating Density...");

	vtkDEL(transform);

	for (int voxel = 0; voxel < NumberVoxels; voxel++)
	{
		VolumeData->GetPoint(voxel, Point);
		if (ImplicitBox->EvaluateFunction(Point) < 0)
		{
			//point is inside the bounding box of the surface: check
			//if the point is also inside the surface.
			if (ImplicitSurface->EvaluateFunction(Point) < 0)
			{
				//store the corresponding point's scalar value
				PointId = VolumeData->FindPoint(Point);
				InsideScalar = VolumeData->GetPointData()->GetTuple(PointId)[0];
				if (!m_EvaluateInSubRange || (InsideScalar >= m_SubRange[0] && InsideScalar <= m_SubRange[1]))
				{
					SumScalars += InsideScalar;
					m_MaxScalar = MAX(InsideScalar, m_MaxScalar);
					m_MinScalar = MIN(InsideScalar, m_MinScalar);
					m_NumberOfScalars++;
					m_VOIScalars->InsertNextTuple(&InsideScalar);
					albaVect3d vPos(Point);
					m_VOICoords.push_back(vPos);
				}
			}
		}
		progressHelper.UpdateProgressBar(voxel*100.0 / NumberVoxels);
	}
	if (m_NumberOfScalars == 0)
	{
		m_MeanScalar = m_MaxScalar = m_MinScalar = m_StandardDeviation = 0;
	}
	else
	{
		m_MeanScalar = SumScalars / m_NumberOfScalars;
		double Sum = 0.0;
		double s;
		for (int i = 0; i < m_NumberOfScalars; i++)
		{
			m_VOIScalars->GetTuple(i, &s);
			Sum += (s - m_MeanScalar) * (s - m_MeanScalar);
		}
		m_StandardDeviation = sqrt(Sum / m_NumberOfScalars);
	}

	m_Median = GetMedian(m_VOIScalars);

	UpdateStrings();

	if (!this->m_TestMode)
	{
		m_Gui->Update();
		albaLogMessage(wxString::Format("\nn,m,max,min,stdev,median\n%d,%.3lf,%.3lf,%.3lf,%.3lf,%3lf", m_NumberOfScalars, m_MeanScalar, m_MaxScalar, m_MinScalar, m_StandardDeviation,m_Median));
	}
}

//----------------------------------------------------------------------------
void albaOpVOIDensity::UpdateStrings()
{
	m_NumberOfScalarsString = albaString(wxString::Format("%d", m_NumberOfScalars));
	m_MeanScalarString = albaString(wxString::Format("%f", m_MeanScalar));
	m_MaxScalarString = albaString(wxString::Format("%f", m_MaxScalar));
	m_MinScalarString = albaString(wxString::Format("%f", m_MinScalar));
	m_StandardDeviationString = albaString(wxString::Format("%f", m_StandardDeviation));
	m_MedianString = albaString(wxString::Format("%f", m_Median));
}
