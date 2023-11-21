/*=========================================================================
  Program:   ALBA (Agile Library for Biomedical Applications)
  Module:    albaOpImporterPointCloud.cpp
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

#include "albaOpImporterPointCloud.h"

#include "wx/busyinfo.h"

#include "albaDecl.h"
#include "albaGUI.h"

#include "albaVMEPointCloud.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "wx/filename.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterPointCloud);
//----------------------------------------------------------------------------

enum IMPORT_POINT_CLOUD_ID
{
	ID_FIRST = MINID,
	ID_INPUT,
	ID_SCALAR,
	ID_SCALAR1,
	ID_SCALAR2,
	ID_SCALAR3,
};

#define EPSILON 1e-3
//----------------------------------------------------------------------------
albaOpImporterPointCloud::albaOpImporterPointCloud(const wxString &label) :
albaOp(label)
{ 
	m_OpType = OPTYPE_IMPORTER;
	m_Canundo = true;

	m_FirstCoordCol=1;
	m_ScalarsNum=1;
	m_ScalarCol[0] = 4;
	m_ScalarCol[1] = 5;
	m_ScalarCol[2] = 6;
	m_ScalarNames[0] = "ID1";
	m_ScalarNames[1] = "ID2";
	m_ScalarNames[2] = "ID3";
	SetCommentLine("%");
}
//----------------------------------------------------------------------------
albaOpImporterPointCloud::~albaOpImporterPointCloud()
{
}

//----------------------------------------------------------------------------
albaOp* albaOpImporterPointCloud::Copy()   
{
  albaOpImporterPointCloud *cp = new albaOpImporterPointCloud(m_Label);
  return cp;
}

//----------------------------------------------------------------------------
void albaOpImporterPointCloud::CreateGui()
{
	m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);

	m_Gui->Label("");
	m_Gui->Label("File Characteristics:", 1);
	m_Gui->String(ID_INPUT, "Comments", &m_CommentLine, "The comment line beginning");
	m_Gui->Integer(ID_INPUT, "First Coord", &m_FirstCoordCol, 1, 100, "The Column of the first coordinate");
	m_Gui->Label("");
	m_Gui->Divider(1);

	m_Gui->Label("Scalars:", 1);
	const wxString scalarsChoice[] = { "Zero", "One", "Two", "Three"};
	m_Gui->Combo(ID_SCALAR, "", &m_ScalarsNum, 4, scalarsChoice);
	m_Gui->Divider();

	for (int i = 0; i < 3; i++)
	{
		m_Gui->String(ID_SCALAR1 + i, "Name", &(m_ScalarNames[i]), "Scalar Name");
		m_Gui->Integer(ID_SCALAR1 + i, "Column", &(m_ScalarCol[i]),1,100, "Scalar Name");
		m_Gui->Label("");
	}

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Divider(1);
	m_Gui->OkCancel();

	m_Gui->FitGui();
	m_Gui->Update();

	EnableDisableGui();
}

//----------------------------------------------------------------------------
void albaOpImporterPointCloud::EnableDisableGui()
{
	m_Gui->Enable(ID_SCALAR1, m_ScalarsNum >= 1);
	m_Gui->Enable(ID_SCALAR2, m_ScalarsNum >= 2);
	m_Gui->Enable(ID_SCALAR3, m_ScalarsNum >= 3);

	int enableok = true;

	if (m_ScalarsNum > 0 && m_ScalarCol[0] > m_FirstCoordCol && m_ScalarCol[0] < m_FirstCoordCol + 2)
		enableok = false;
	if (m_ScalarsNum > 1 && m_ScalarCol[1] > m_FirstCoordCol && m_ScalarCol[1] < m_FirstCoordCol + 2)
		enableok = false;
	if (m_ScalarsNum > 2 && m_ScalarCol[2] > m_FirstCoordCol && m_ScalarCol[2] < m_FirstCoordCol + 2)
		enableok = false;
	if (m_ScalarsNum > 1 && m_ScalarCol[0] == m_ScalarCol[1])
		enableok = false;
	if (m_ScalarsNum > 2 && (m_ScalarCol[0] == m_ScalarCol[2] || m_ScalarCol[1] == m_ScalarCol[2]))
		enableok = false;

	m_Gui->Enable(wxOK, enableok);

	m_Gui->Update();
}

//----------------------------------------------------------------------------
void albaOpImporterPointCloud::OnEvent(albaEventBase *alba_event)
{

	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
			case wxOK:
			{
				if (Import() == ALBA_OK)
					OpStop(OP_RUN_OK);
				else
					OpStop(OP_RUN_CANCEL);
			}
			break;
			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
				break;
			case ID_INPUT:
			case ID_SCALAR:
			case ID_SCALAR1:
			case ID_SCALAR2:
			case ID_SCALAR3:
				EnableDisableGui();
				break;
			default:
				albaEventMacro(*e);
				break;
		}
	}
}

//----------------------------------------------------------------------------
albaString albaOpImporterPointCloud::GetScalarName(int pos) const
{
	if (pos < 0 || pos >2)
		return "";
	else
		return m_ScalarNames[pos];
}

//----------------------------------------------------------------------------
void albaOpImporterPointCloud::SetScalarName(int pos, albaString val)
{
	if (pos < 0 || pos >2)
		return;
	else
		m_ScalarNames[pos] = val;
}

//----------------------------------------------------------------------------
void albaOpImporterPointCloud::SetNumberOfScalars(int val)
{
	if (val < 0 || val > 3)
		return;
	else 
	m_ScalarsNum = val;
}

//----------------------------------------------------------------------------
int albaOpImporterPointCloud::GetScalarColumn(int pos) const
{
	if (pos < 0 || pos > 2)
		return -1;
	else 
		return m_ScalarCol[pos];
}

//----------------------------------------------------------------------------
void albaOpImporterPointCloud::SetScalarColumn(int pos, int val)
{
	if (pos < 0 || pos > 2)
		return;
	else
		m_ScalarCol[pos] = val;
}

//----------------------------------------------------------------------------
int albaOpImporterPointCloud::Import(void)
{
	
	albaString wildcard = "Point Cloud files (*.*)|*.*|All Files (*.*)|*.*";

	if (!GetTestMode())
	{
		wxString f;
		f = albaGetOpenFile("", wildcard).ToAscii();
		if (!f.IsEmpty() && wxFileExists(f))
			SetFileName(f);
		else
			return ALBA_ERROR;
	}

	if (ReadInit(GetFileName(), GetTestMode(), true, "Please wait parsing Point Cloud File...", m_Listener) == ALBA_ERROR)
	{
		albaErrorMessage("Cannot Open: %s", GetFileName().GetCStr());
		ReadFinalize();
		return ALBA_ERROR;
	}

	
	wxString path, name, ext;

	wxFileName::SplitPath(GetFileName().GetCStr(), &path, &name, &ext);

	
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

	vtkDoubleArray *newArray[3];

	for (int i = 0; i < m_ScalarsNum; i++)
	{
		vtkNEW(newArray[i]);
		newArray[i]->SetName(m_ScalarNames[i].GetCStr());
	}

	int lineLenght = GetLine();
	
	if (lineLenght == 0)
	{
		albaErrorMessage("Wrong file: %s", GetFileName().GetCStr());
		ReadFinalize();
		return ALBA_ERROR;
	}

	double value,dist, point[3];
	int nReaded, charReaded, pos;
	int pointN = 0, columNum = m_FirstCoordCol + 2; 
	
	if (m_ScalarsNum > 0)
		columNum = MAX(columNum, m_ScalarCol[0]);
	if (m_ScalarsNum > 1)
		columNum = MAX(columNum, m_ScalarCol[1]);
	if (m_ScalarsNum > 2)
		columNum = MAX(columNum, m_ScalarCol[2]);

	do 
	{
		//Skip Comments
		if (strncmp(m_Line, GetCommentLine().GetCStr(), GetCommentLine().Length()) == 0)
				continue;

		if (IsEmptyLine(m_Line))
			continue;

		pos = 0;
		for (int i = 0; i < columNum; i++)
		{
			while (m_Line[pos] == ',' || m_Line[pos] == ';')
				pos++;

			nReaded = sscanf(m_Line+pos, "%lf %n", &value, &charReaded);
			if (nReaded != 1)
			{
				if (m_TestMode)
					albaLogMessage("Wrong column number\n\n\tFile: %s", GetFileName().GetCStr());
				else
					albaErrorMessage("Wrong column number\n\n\tFile: %s", GetFileName().GetCStr());

				for (int i = 0; i < m_ScalarsNum; i++)
					vtkDEL(newArray[i]);

				vtkDEL(newPoints);
				vtkDEL(polys);
				vtkDEL(polydata);
				albaDEL(pointCloudVME);

				ReadFinalize();
				return ALBA_ERROR;
			}

			if (i + 1 == m_FirstCoordCol)
				point[0] = value;
			else if (i + 1 == m_FirstCoordCol + 1)
				point[1] = value;
			else if (i + 1 == m_FirstCoordCol + 2)
				point[2] = value;
			else if (m_ScalarsNum > 0 && i + 1 == m_ScalarCol[0])
				newArray[0]->InsertNextValue(value);
			else if (m_ScalarsNum > 1 && i + 1 == m_ScalarCol[1])
				newArray[1]->InsertNextValue(value);
			else if (m_ScalarsNum > 2 && i + 1 == m_ScalarCol[2])
				newArray[2]->InsertNextValue(value);
			
			pos += charReaded;

		}



		newPoints->InsertNextPoint(point);

		polys->InsertNextCell(3);
		polys->InsertCellPoint(pointN);
		polys->InsertCellPoint(pointN);
		polys->InsertCellPoint(pointN);

		pointN++;
	} while ((lineLenght = GetLine()) != 0);


	vtkPointData *outPointData = polydata->GetPointData();
	
	for (int i = 0; i < m_ScalarsNum; i++)
	{
		outPointData->AddArray(newArray[i]);
		vtkDEL(newArray[i]);
	}


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
bool albaOpImporterPointCloud::IsEmptyLine(char *line)
{
	for (int i = 0;; i++)
	{
		if (line[i] == ' ' || line[i] == '\t')
			continue;
		else if (line[i] == '\n' || line[i] == '\r' || line[i] == 0)
			return true;
		else
			return false;
	}
}

//----------------------------------------------------------------------------
bool albaOpImporterPointCloud::InternalAccept(albaVME *node)
{ 
	return true;
}

//----------------------------------------------------------------------------
void albaOpImporterPointCloud::OpRun()
{
	CreateGui();
	ShowGui();
}
