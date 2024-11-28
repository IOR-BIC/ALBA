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
#include "albaTagItem.h"
#include "albaVMEPointCloud.h"
#include "albaVMEVolumeGray.h"
#include "vtkCellArray.h"
#include "vtkTransform.h"
#include "vtkImageData.h"
#include "vtkCleanPolyData.h"
#include "vtkGenericCell.h"
#include "vtkTriangle.h"


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

	m_CreateSegOutput = false;
	m_CreatePointCloudOutput = true;

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
	m_VOIIds.clear();
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
	ID_ENABLE_RANGE,
	ID_RANGE_UPDATED,
  ID_NUMBER_OF_VOXEL_IN_VOI,
	ID_NUM_SCALARS,
	ID_MEAN_SCALAR,
	ID_MIN_SCALAR,
	ID_MAX_SCALAR,
	ID_STANDARD_DEVIATION,
	ID_MEDIAN,
	ID_SURFACE_AREA,
	ID_VOXEL_LIST,
	ID_EXPORT_REPORT,
	ID_CREATE_SEGMENTATION_OUTPUT,
	ID_CREATE_CLOUD_POINT_OUTPUT,
};
//----------------------------------------------------------------------------
void albaOpVOIDensity::OpRun()
//----------------------------------------------------------------------------
{
	vtkNEW(m_VOIScalars);

	vtkALBASmartPointer<vtkDataSet> volumeData = m_Input->GetOutput()->GetVTKData();
	volumeData->Update();

	volumeData->GetPointData()->GetScalars()->GetRange(m_SubRange);

	m_ImagedataVol = vtkImageData::SafeDownCast(volumeData) ? true : false;

	if (!this->m_TestMode)
	{
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
	m_Gui->Divider();
	m_Gui->Bool(ID_ENABLE_RANGE, "Evaluate only in subrange", &m_EvaluateInSubRange, true);
	m_Gui->VectorN(ID_RANGE_UPDATED, "Range:", m_SubRange, 2, m_SubRange[0], m_SubRange[1]);
	m_Gui->Divider(2);
	m_Gui->Label(_("Number of voxel inside the VOI"));
	m_Gui->String(ID_NUM_SCALARS, "Num=", &m_NumberOfScalarsString, "");
	m_Gui->Label(_("Voxels's scalar mean inside the VOI"));
	m_Gui->String(ID_MEAN_SCALAR, "Mean=", &m_MeanScalarString, "");
	m_Gui->String(ID_MEDIAN, "Median=", &m_MedianString, "");
	m_Gui->String(ID_SURFACE_AREA, "Area=", &m_SurfaceAreaString, "");
	m_Gui->String(ID_MIN_SCALAR, "Min=", &m_MinScalarString, "");
	m_Gui->String(ID_MAX_SCALAR, "Max=", &m_MaxScalarString, "");
	m_Gui->String(ID_STANDARD_DEVIATION, "Std dev=", &m_StandardDeviationString, "");
	//m_VoxelList=m_Gui->ListBox(ID_VOXEL_LIST);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Divider(1);
	m_Gui->Label("");
	m_Gui->Bool(ID_CREATE_SEGMENTATION_OUTPUT, "Create Segmentation Output", &m_CreateSegOutput, true);
	m_Gui->Bool(ID_CREATE_CLOUD_POINT_OUTPUT, "Create Cloud Point Output", &m_CreatePointCloudOutput, true);
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->Label("");
	m_Gui->Button(ID_EXPORT_REPORT, "Export Report");
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	m_Gui->Enable(ID_RANGE_UPDATED, false);
	m_Gui->Enable(ID_EXPORT_REPORT, false);
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
		SetDoubleTag("Area:", m_SurfaceArea);

		if(m_CreatePointCloudOutput)
			CreatePointSamplingOutput();
		
		if (m_CreateSegOutput)
			CreateSegmentationOutput();
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
void albaOpVOIDensity::CalculateSurfaceArea()
{
	vtkCleanPolyData *cleaner = vtkCleanPolyData::New();
	vtkTriangleFilter *triangulator = vtkTriangleFilter::New();

	m_SurfaceArea = 0;

	vtkPolyData *surface = vtkPolyData::SafeDownCast(m_Surface->GetOutput()->GetVTKData());
	if (surface == NULL)
		return;

	cleaner->SetInput(surface);
	cleaner->ConvertPolysToLinesOff();
	cleaner->GetOutput()->Update();
	triangulator->SetInput(cleaner->GetOutput());
	triangulator->Update();
	vtkPolyData *triSurface=triangulator->GetOutput();

	int TrianglesNum = triSurface->GetNumberOfCells();
	for (int i = 0; i < TrianglesNum; i++)
	{
		if (triSurface->GetCellType(i) == VTK_TRIANGLE)
		{
			vtkGenericCell *triangle = vtkGenericCell::New();
			triSurface->GetCell(i, triangle);

			vtkPoints *points = triangle->GetPoints();
			double a[3], b[3], c[3];

			points->GetPoint(0, a);
			points->GetPoint(1, b);
			points->GetPoint(2, c);

			double area = vtkTriangle::TriangleArea(a, b, c);

			//area is used to estimate the cell size;
			m_SurfaceArea += area;
		}
		else
			albaErrorMacro("Wrong CELL data Type!");
	}
}

//----------------------------------------------------------------------------
void albaOpVOIDensity::CreateSegmentationOutput()
{
	vtkDataSet * vtkData = m_Input->GetOutput()->GetVTKData();

	albaString name = m_Surface->GetName();
	name += " segmentation";

	albaVMEVolumeGray *segmentationVME;
	albaNEW(segmentationVME);
	segmentationVME->DeepCopy(m_Input);
	segmentationVME->SetName(name);


	vtkUnsignedCharArray *segScalars;
	vtkNEW(segScalars);
	vtkIdType scalarNum = vtkData->GetNumberOfPoints();
	segScalars->SetNumberOfTuples(scalarNum);
	
	for (int i = 0; i < scalarNum; i++)
		segScalars->SetTuple1(i, 0);

	int nPoints = m_VOICoords.size();
	for (int i = 0; i < nPoints; i++)
		segScalars->SetTuple1(m_VOIIds[i],255);

	vtkImageData *segImDa;
	vtkNEW(segImDa);

	segImDa->DeepCopy(vtkData);
	segImDa->GetPointData()->SetScalars(segScalars);

	segmentationVME->SetData(segImDa, segmentationVME->GetTimeStamp());
	
	segmentationVME->ReparentTo(m_Surface);
	segmentationVME->SetAbsMatrix(*m_Input->GetOutput()->GetAbsMatrix(),m_Input->GetTimeStamp());

	albaDEL(segmentationVME);
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
	if (surface == NULL)
		return ALBA_ERROR;

	m_Surface = surface;
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
		albaString openStr;
		openStr.Printf("%s is an Open Surface",surface->GetName());
		albaMessage(openStr.GetCStr(), _("Warning"));
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

				if (SetSurface(event.GetVme()) != ALBA_ERROR)
				{
					EvaluateSurface();
					m_Gui->Enable(ID_EXPORT_REPORT, true);
					m_Gui->Enable(wxOK, true);
				}
			}
			break;
			case ID_ENABLE_RANGE:
				m_Gui->Enable(ID_RANGE_UPDATED, m_EvaluateInSubRange);
				break;
			case ID_RANGE_UPDATED:
				SortSubRange();

				break;
			case ID_EXPORT_REPORT:
				WriteReport();
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
void albaOpVOIDensity::SortSubRange()
{
	if (m_SubRange[0] > m_SubRange[1])
		m_SubRange[1] = m_SubRange[0];
	else if (m_SubRange[1] < m_SubRange[0])
		m_SubRange[1] = m_SubRange[0];
	if(m_Gui)
		m_Gui->Update();
}

//----------------------------------------------------------------------------
void albaOpVOIDensity::EvaluateSurface()
//----------------------------------------------------------------------------
{
	if (!this->m_TestMode)
		wxBusyCursor wait;

	double b[6];
	double point[3], insideScalar = 0.0, sumScalars = 0.0;
	int numberVoxels, pointId;

	// Reset parameters
	m_NumberOfScalars = 0;
	m_MaxScalar = -99999.0;
	m_MinScalar = 99999.0;
	m_VOIScalars->Reset();
	m_VOICoords.clear();
	m_VOIIds.clear();

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
	numberVoxels = VolumeData->GetNumberOfPoints();

	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Evaluating Density...");

	vtkDEL(transform);

	for (vtkIdType voxel = 0; voxel < numberVoxels; voxel++)
	{
		VolumeData->GetPoint(voxel, point);
		if (ImplicitBox->EvaluateFunction(point) < 0)
		{
			//point is inside the bounding box of the surface: check
			//if the point is also inside the surface.
			if (ImplicitSurface->EvaluateFunction(point) < 0)
			{
				//store the corresponding point's scalar value
				pointId = VolumeData->FindPoint(point);
				insideScalar = VolumeData->GetPointData()->GetTuple(pointId)[0];
				if (!m_EvaluateInSubRange || (insideScalar >= m_SubRange[0] && insideScalar <= m_SubRange[1]))
				{
					sumScalars += insideScalar;
					m_MaxScalar = MAX(insideScalar, m_MaxScalar);
					m_MinScalar = MIN(insideScalar, m_MinScalar);
					m_NumberOfScalars++;
					m_VOIScalars->InsertNextTuple(&insideScalar);
					albaVect3d vPos(point);
					m_VOICoords.push_back(vPos);
					m_VOIIds.push_back(voxel);
				}
			}
		}
		progressHelper.UpdateProgressBar(voxel*100.0 / numberVoxels);
	}
	if (m_NumberOfScalars == 0)
	{
		m_MeanScalar = m_MaxScalar = m_MinScalar = m_StandardDeviation = 0;
	}
	else
	{
		m_MeanScalar = sumScalars / m_NumberOfScalars;
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

	CalculateSurfaceArea();

	UpdateStrings();

	if (!this->m_TestMode)
	{
		m_Gui->Update();
		albaLogMessage(albaString::Format("\nn,m,max,min,stdev,median\n%d,%.3lf,%.3lf,%.3lf,%.3lf,%3lf", m_NumberOfScalars, m_MeanScalar, m_MaxScalar, m_MinScalar, m_StandardDeviation,m_Median));
	}
}

//----------------------------------------------------------------------------
void albaOpVOIDensity::UpdateStrings()
{
	m_NumberOfScalarsString = albaString::Format("%d", m_NumberOfScalars);
	m_MeanScalarString = albaString::Format("%f", m_MeanScalar);
	m_MaxScalarString = albaString::Format("%f", m_MaxScalar);
	m_MinScalarString = albaString::Format("%f", m_MinScalar);
	m_StandardDeviationString = albaString::Format("%f", m_StandardDeviation);
	m_MedianString = albaString::Format("%f", m_Median);
	m_SurfaceAreaString = albaString::Format("%f", m_SurfaceArea);
}


//----------------------------------------------------------------------------
void albaOpVOIDensity::WriteReport()
{
	albaString fileNameFullPath = albaGetDocumentsDirectory();
	fileNameFullPath.Append("\\NewReport.csv");

	albaString wildc = "Report file (*.csv)|*.csv";
	albaString newFileName = albaGetSaveFile(fileNameFullPath.GetCStr(), wildc, "Save Report", 0, false);

	//////////////////////////////////////////////////////////////////////////
	if (newFileName == "") return;

	// Calculate Date-Time Report
	time_t rawtime;
	struct tm * timeinfo;  time(&rawtime);
	timeinfo = localtime(&rawtime);

	//////////////////////////////////////////////////////////////////////////
	CreateCSVFile(newFileName);

	//////////////////////////////////////////////////////////////////////////
	// Open Report File
	wxString url = "file:///";
	url = url + newFileName.GetCStr();
	url.Replace("\\", "/");
	albaLogMessage("Opening %f", url.ToAscii());
	wxString command = "rundll32.exe url.dll,FileProtocolHandler ";
	command = command + url;
	wxExecute(command);
}
//----------------------------------------------------------------------------
void albaOpVOIDensity::CreateCSVFile(albaString file)
{
	bool firstAcces = !wxFileExists(file.GetCStr());

	FILE * pFile;
	pFile = fopen(file, "a+");

	GetTags();

	if (pFile != NULL)
	{

		if (firstAcces) // Header
		{
			// Patient Info
			fprintf(pFile, "PZName;PZCode;PZBirthdate;PZCenter;PZExamDate;SurfaceName;");

			// Main scores 
			fprintf(pFile, "NumberOfScalars;MeanScalar;MaxScalar;MinScalar;StandardDeviation;Median;SurfaceArea");
		}

		

		// Patient Info
		fprintf(pFile, "\n%s;%s;%s;%s;%s;%s;", m_PatientName.GetCStr(), m_PatientCode.GetCStr(), m_PatientBirthdate.GetCStr(), m_PatientCenter.GetCStr(), m_PatientExamDate.GetCStr(), m_Surface->GetName());

		// Main scores 
		fprintf(pFile, "%s;%s;%s;%s;%s;%s;%s", m_NumberOfScalarsString.GetCStr(), m_MeanScalarString.GetCStr(), m_MaxScalarString.GetCStr(), m_MinScalarString.GetCStr(), m_StandardDeviationString.GetCStr(), m_MedianString.GetCStr(), m_SurfaceAreaString.GetCStr());

		
		fclose(pFile);
	}
	else
	{
		albaErrorMessage("Cannot open CSV file, it can be opened by an another application.\nPlease close the application or select another file.");
	}
}

//----------------------------------------------------------------------------
void albaOpVOIDensity::SetSubRange(double subRangeA, double subRangeB)
{
	m_SubRange[0] = subRangeA; m_SubRange[1] = subRangeB; SortSubRange();
}

void albaOpVOIDensity::GetTags()
{
	// Patient Name 
	albaTagArray * tagArray = m_Input->GetTagArray();
	albaTagItem *tag = tagArray->GetTag("PatientsName");

	if (tag)
	{
		albaString tmp;
		tag->GetValueAsSingleString(tmp);
		wxString tmp2 = tmp.GetCStr();

		tmp2.Replace("(\"", "");
		tmp2.Replace("\")", "");
		tmp2.Replace("\"", "");
		tmp2.Replace("^", " ");
		tmp2.Replace("_", " ");

		m_PatientName = tmp2;
	}

	// Patient Code
	tag = tagArray->GetTag("PatientID");

	if (tag)
	{
		albaString tmp;
		tag->GetValueAsSingleString(tmp);
		wxString tmp2 = tmp.GetCStr();

		tmp2.Replace("(\"", "");
		tmp2.Replace("\")", "");
		tmp2.Replace("\"", "");
		tmp2.Replace("^", " ");
		tmp2.Replace("_", " ");

		m_PatientCode = tmp2;
	}

	// Patients BirthDate
	tag = tagArray->GetTag("PatientsBirthDate");

	if (tag)
	{
		albaString tmp;
		tag->GetValueAsSingleString(tmp);
		wxString tmp2 = tmp.GetCStr();

		tmp2.Replace("(\"", "");
		tmp2.Replace("\")", "");
		tmp2.Replace("\"", "");
		tmp2.Replace("^", " ");
		tmp2.Replace("_", " ");

		m_PatientBirthdate = "" + tmp2.SubString(6, 7) + "-" + tmp2.SubString(4, 5) + "-" + tmp2.SubString(0, 3);
	}

	// Center
	tag = tagArray->GetTag("InstitutionName");

	if (tag)
	{
		albaString tmp;
		tag->GetValueAsSingleString(tmp);
		wxString tmp2 = tmp.GetCStr();

		tmp2.Replace("(\"", "");
		tmp2.Replace("\")", "");
		tmp2.Replace("\"", "");
		tmp2.Replace("^", " ");
		tmp2.Replace("_", " ");

		m_PatientCenter = tmp2;
	}

	// Exam date
	tag = tagArray->GetTag("AcquisitionDate");

	if (tag)
	{
		albaString tmp;
		tag->GetValueAsSingleString(tmp);
		wxString tmp2 = tmp.GetCStr();

		tmp2.Replace("(\"", "");
		tmp2.Replace("\")", "");
		tmp2.Replace("\"", "");
		tmp2.Replace("^", " ");
		tmp2.Replace("_", " ");

		m_PatientExamDate = "" + tmp2.SubString(6, 7) + "-" + tmp2.SubString(4, 5) + "-" + tmp2.SubString(0, 3);
	}
}
