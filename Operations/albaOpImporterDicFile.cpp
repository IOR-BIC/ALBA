/*=========================================================================
  Program:   Bonemat
  Module:    albaOpLoadScalarFromDicFile.cpp
  Language:  C++
  Date:      $Date: 2010-11-23 16:50:26 $
  Version:   $Revision: 1.1.1.1.2.4 $
  Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaOpImporterDicFile.h"

#include "albaGUIBusyInfo.h"

#include "albaDecl.h"
#include "albaGUI.h"

#include "albaVMEPointCloud.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "wx/filename.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterDicFile);
//----------------------------------------------------------------------------

#define EPSILON 1e-3
//----------------------------------------------------------------------------
albaOpImporterDicFile::albaOpImporterDicFile(const wxString &label) :
albaOpImporterFile(label)
{
	SetWildc("Digital Image Correlation files (*.dat)|*.dat|All Files (*.*)|*.*");
	m_OpType = OPTYPE_IMPORTER;
	m_Canundo = true;
}
//----------------------------------------------------------------------------
albaOpImporterDicFile::~albaOpImporterDicFile()
{
}

//----------------------------------------------------------------------------
albaOp* albaOpImporterDicFile::Copy()   
{
  albaOpImporterDicFile *cp = new albaOpImporterDicFile(m_Label);
  return cp;
}

//----------------------------------------------------------------------------
int albaOpImporterDicFile::ImportFile()
{
	
	if (ReadInit(m_FileName, GetTestMode(), true, "Please wait parsing DIC File...", m_Listener) == ALBA_ERROR)
	{
		albaErrorMessage("Cannot Open: %s", m_FileName.GetCStr());
		ReadFinalize();
		return ALBA_ERROR;
	}

	wxString path, name, ext;

	wxFileName::SplitPath(m_FileName.GetCStr(), &path, &name, &ext);

	
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
	newArray->SetName("DIC");



	//Skip comments
	int lineLenght = 0;
	do 
	{
		lineLenght = GetLine();
	} while (lineLenght > 0 && m_Line[0] == '%');
	
	if (lineLenght == 0)
	{
		albaErrorMessage("Wrong file: %s", m_FileName.GetCStr());
		ReadFinalize();
		return ALBA_ERROR;
	}

	double values[12],dist;

	int pointN = 0;

	do 
	{
		int nReaded = sscanf(m_Line,  "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5], &values[6], &values[7], &values[8], &values[9], &values[10], &values[11]);

		if (nReaded != 12)
		{
			albaErrorMessage("Wrong file: %s", m_FileName.GetCStr());
			ReadFinalize();
			return ALBA_ERROR;
		}

		newPoints->InsertNextPoint(&values[2]);

		polys->InsertNextCell(3);
		polys->InsertCellPoint(pointN);
		polys->InsertCellPoint(pointN);
		polys->InsertCellPoint(pointN);

		newArray->InsertNextValue(values[11]);

		pointN++;
	} while ((lineLenght = GetLine()) != 0);


	vtkPointData *outPointData = polydata->GetPointData();
	outPointData->AddArray(newArray);
	vtkDEL(newArray);

	polydata->SetPoints(newPoints);
	vtkDEL(newPoints);

	polydata->SetPolys(polys);
	vtkDEL(polys);

	polydata->Modified();
	polydata->Update();
	pointCloudVME->SetData(polydata,0);
	vtkDEL(polydata);

	pointCloudVME->ReparentTo(m_Input);

	m_Output = pointCloudVME;

	albaDEL(pointCloudVME);

	ReadFinalize();

	return ALBA_OK;
}

//----------------------------------------------------------------------------
bool albaOpImporterDicFile::InternalAccept(albaVME *node)
{ 
	return true;
}

//----------------------------------------------------------------------------
void albaOpImporterDicFile::OpRun()
{

	int result = OP_RUN_CANCEL;
	m_FileName = "";

	wxString f;
	f = albaGetOpenFile("", m_Wildc).ToAscii(); 
	if(!f.IsEmpty() && wxFileExists(f))
	{
		m_FileName = f;
		if(ImportFile()==ALBA_OK)
			result = OP_RUN_OK;
	}
	albaEventMacro(albaEvent(this,result));  
}
