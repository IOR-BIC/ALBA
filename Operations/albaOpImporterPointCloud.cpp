/*=========================================================================
  Program:   Bonemat
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

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterPointCloud);
//----------------------------------------------------------------------------

enum BONEMAT_ID
{
	ID_INPUT = MINID,
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
	m_CommentLine = "%";
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

	m_Gui->Label("");
	m_Gui->Label("File Characteristics:", 1);
	m_Gui->String(ID_INPUT, "Comments", &m_CommentLine, "The comment line beginning");
	m_Gui->Integer(ID_INPUT, "First Coord", &m_FirstCoordCol, 1, 100, "The Column of the first coordinate");
	m_Gui->Label("");

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

	m_Gui->OkCancel();

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
		}
	}
}

//----------------------------------------------------------------------------
int albaOpImporterPointCloud::Import(void)
{
	
	m_FileName = "";
	albaString wildcard = "Point Cloud files (*.*)|*.*|All Files (*.*)|*.*";


	wxString f;
	f = albaGetOpenFile("", wildcard).c_str();
	if (!f.IsEmpty() && wxFileExists(f))
		m_FileName = f;
	else
		return ALBA_ERROR;

	if (ReadInit(m_FileName, GetTestMode(), true, "Please wait parsing DIC File...", m_Listener) == ALBA_ERROR)
	{
		albaErrorMessage("Cannot Open: %s", m_FileName.GetCStr());
		ReadFinalize();
		return ALBA_ERROR;
	}

	
	wxString path, name, ext;

	wxSplitPath(m_FileName, &path, &name, &ext);

	
	albaVMEPointCloud *polydataVME;
	albaNEW(polydataVME);
	polydataVME->SetName(name);

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
		albaErrorMessage("Wrong file: %s", m_FileName.GetCStr());
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
		if (strncmp(m_Line, m_CommentLine.GetCStr(), m_CommentLine.Length()) == 0)
				continue;

		pos = 0;
		for (int i = 0; i < columNum; i++)
		{
			nReaded = sscanf(m_Line+pos, "%lf %n", &value, &charReaded);
			if (nReaded != 1)
			{
				albaErrorMessage("Wrong column number\n\n\tFile: %s", m_FileName.GetCStr());
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
	polydataVME->SetData(polydata,0);
	vtkDEL(polydata);

	polydataVME->ReparentTo(m_Input);
	albaDEL(polydataVME);

	ReadFinalize();

	return ALBA_OK;
}

//----------------------------------------------------------------------------
bool albaOpImporterPointCloud::Accept(albaVME *node)
{ 
	return true;
}

//----------------------------------------------------------------------------
void albaOpImporterPointCloud::OpRun()
{
	CreateGui();

	ShowGui();
}
