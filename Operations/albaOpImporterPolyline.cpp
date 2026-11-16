/*=========================================================================
  Program:   ALBA (Agile Library for Biomedical Applications)
  Module:    albaOpImporterPolyline.cpp
  Language:  C++
  Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) BIC-IOR 2026 (https://github.com/IOR-BIC)

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

#include "albaOpImporterPolyline.h"

#include "albaGUIBusyInfo.h"

#include "albaDecl.h"
#include "albaGUI.h"

#include "albaVMEPolyline.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkIdList.h"
#include "wx/filename.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterPolyline);
//----------------------------------------------------------------------------

enum IMPORT_POLYLINE_ID
{
	ID_FIRST = MINID,
	ID_INPUT,
	ID_SCALAR,
	ID_SCALAR1,
	ID_SCALAR2,
	ID_SCALAR3,
};

//----------------------------------------------------------------------------
albaOpImporterPolyline::albaOpImporterPolyline(const wxString &label) :
albaOp(label)
{
	m_OpType = OPTYPE_IMPORTER;
	m_Canundo = true;

	m_FirstCoordCol=1;
	m_ScalarsNum=0;
	m_ScalarCol[0] = 4;
	m_ScalarCol[1] = 5;
	m_ScalarCol[2] = 6;
	m_ScalarNames[0] = "ID1";
	m_ScalarNames[1] = "ID2";
	m_ScalarNames[2] = "ID3";
	SetCommentLine("#");
}
//----------------------------------------------------------------------------
albaOpImporterPolyline::~albaOpImporterPolyline()
{
}

//----------------------------------------------------------------------------
albaOp* albaOpImporterPolyline::Copy()
{
  albaOpImporterPolyline *cp = new albaOpImporterPolyline(m_Label);
  return cp;
}

//----------------------------------------------------------------------------
void albaOpImporterPolyline::CreateGui()
{
	m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);

	m_Gui->Label("");
	m_Gui->Label("File Characteristics:", 1);
	m_Gui->String(ID_INPUT, "Comments", &m_CommentLine, "The comment line beginning");
	m_Gui->Integer(ID_INPUT, "First Coord", &m_FirstCoordCol, 1, 100, "The Column of the first coordinate");
	m_Gui->Label("Empty lines separate polylines");
	m_Gui->Label("");
	m_Gui->Divider(1);

	m_Gui->Label("Scalars:", 1);
	const wxString scalarsChoice[] = { "Zero", "One", "Two", "Three"};
	m_Gui->Combo(ID_SCALAR, "", &m_ScalarsNum, 4, scalarsChoice);
	m_Gui->Divider();

	for (int i = 0; i < 3; i++)
	{
		m_Gui->String(ID_SCALAR1 + i, "Name", &(m_ScalarNames[i]), "Scalar Name");
		m_Gui->Integer(ID_SCALAR1 + i, "Column", &(m_ScalarCol[i]),1,100, "Scalar Column");
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
void albaOpImporterPolyline::EnableDisableGui()
{
	m_Gui->Enable(ID_SCALAR1, m_ScalarsNum >= 1);
	m_Gui->Enable(ID_SCALAR2, m_ScalarsNum >= 2);
	m_Gui->Enable(ID_SCALAR3, m_ScalarsNum >= 3);

	int enableok = true;

	// A scalar column cannot overlap the three coordinate columns or another scalar column
	for (int i = 0; i < m_ScalarsNum; i++)
	{
		if (m_ScalarCol[i] >= m_FirstCoordCol && m_ScalarCol[i] <= m_FirstCoordCol + 2)
			enableok = false;
		for (int j = 0; j < i; j++)
			if (m_ScalarCol[i] == m_ScalarCol[j])
				enableok = false;
	}

	m_Gui->Enable(wxOK, enableok);

	m_Gui->Update();
}

//----------------------------------------------------------------------------
void albaOpImporterPolyline::OnEvent(albaEventBase *alba_event)
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
albaString albaOpImporterPolyline::GetScalarName(int pos) const
{
	if (pos < 0 || pos > 2)
		return "";
	else
		return m_ScalarNames[pos];
}

//----------------------------------------------------------------------------
void albaOpImporterPolyline::SetScalarName(int pos, albaString val)
{
	if (pos >= 0 && pos <= 2)
		m_ScalarNames[pos] = val;
}

//----------------------------------------------------------------------------
void albaOpImporterPolyline::SetNumberOfScalars(int val)
{
	if (val >= 0 && val <= 3)
		m_ScalarsNum = val;
}

//----------------------------------------------------------------------------
int albaOpImporterPolyline::GetScalarColumn(int pos) const
{
	if (pos < 0 || pos > 2)
		return -1;
	else
		return m_ScalarCol[pos];
}

//----------------------------------------------------------------------------
void albaOpImporterPolyline::SetScalarColumn(int pos, int val)
{
	if (pos >= 0 && pos <= 2)
		m_ScalarCol[pos] = val;
}

//----------------------------------------------------------------------------
int albaOpImporterPolyline::Import(void)
{
	albaString wildcard = "Polyline files (*.txt)|*.txt|All Files (*.*)|*.*";

	if (!GetTestMode())
	{
		wxString f;
		f = albaGetOpenFile("", wildcard).ToAscii();
		if (!f.IsEmpty() && wxFileExists(f))
			SetFileName(f);
		else
			return ALBA_ERROR;
	}

	if (ReadInit(GetFileName(), GetTestMode(), true, "Please wait parsing Polyline File...", m_Listener) == ALBA_ERROR)
	{
		albaErrorMessage("Cannot Open: %s", GetFileName().GetCStr());
		ReadFinalize();
		return ALBA_ERROR;
	}

	int lineLenght = GetLine();
	if (lineLenght == 0)
	{
		albaErrorMessage("Wrong file: %s", GetFileName().GetCStr());
		ReadFinalize();
		return ALBA_ERROR;
	}

	wxString path, name, ext;
	wxFileName::SplitPath(GetFileName().GetCStr(), &path, &name, &ext);

	vtkPoints *newPoints;
	vtkNEW(newPoints);

	vtkCellArray *lines;
	vtkNEW(lines);

	// Point ids of the polyline being read
	vtkIdList *currentLine;
	vtkNEW(currentLine);

	vtkDoubleArray *newArray[3];
	for (int i = 0; i < m_ScalarsNum; i++)
	{
		vtkNEW(newArray[i]);
		newArray[i]->SetName(m_ScalarNames[i].GetCStr());
	}

	double value, point[3];
	int nReaded, charReaded, pos;
	int pointN = 0, columNum = m_FirstCoordCol + 2;
	bool readError = false;

	for (int i = 0; i < m_ScalarsNum; i++)
		columNum = MAX(columNum, m_ScalarCol[i]);

	do
	{
		//Skip Comments
		if (strncmp(m_Line, GetCommentLine().GetCStr(), GetCommentLine().Length()) == 0)
			continue;

		// An empty line closes the current polyline
		if (IsEmptyLine(m_Line))
		{
			if (currentLine->GetNumberOfIds() > 1)
				lines->InsertNextCell(currentLine);
			currentLine->Reset();
			continue;
		}

		pos = 0;
		for (int i = 0; i < columNum; i++)
		{
			while (m_Line[pos] == ',' || m_Line[pos] == ';')
				pos++;

			nReaded = sscanf(m_Line+pos, "%lf %n", &value, &charReaded);
			if (nReaded != 1)
			{
				readError = true;
				break;
			}

			if (i + 1 == m_FirstCoordCol)
				point[0] = value;
			else if (i + 1 == m_FirstCoordCol + 1)
				point[1] = value;
			else if (i + 1 == m_FirstCoordCol + 2)
				point[2] = value;
			else
			{
				for (int j = 0; j < m_ScalarsNum; j++)
					if (i + 1 == m_ScalarCol[j])
						newArray[j]->InsertNextValue(value);
			}

			pos += charReaded;
		}

		if (readError)
			break;

		newPoints->InsertNextPoint(point);
		currentLine->InsertNextId(pointN);
		pointN++;
	} while ((lineLenght = GetLine()) != 0);

	// Close the last polyline
	if (!readError && currentLine->GetNumberOfIds() > 1)
		lines->InsertNextCell(currentLine);
	vtkDEL(currentLine);

	if (readError || lines->GetNumberOfCells() == 0)
	{
		if (m_TestMode)
			albaLogMessage(readError ? "Wrong column number\n\n\tFile: %s" : "No polyline found\n\n\tFile: %s", GetFileName().GetCStr());
		else
			albaErrorMessage(readError ? "Wrong column number\n\n\tFile: %s" : "No polyline found\n\n\tFile: %s", GetFileName().GetCStr());

		for (int i = 0; i < m_ScalarsNum; i++)
			vtkDEL(newArray[i]);
		vtkDEL(newPoints);
		vtkDEL(lines);

		ReadFinalize();
		return ALBA_ERROR;
	}

	vtkPolyData *polydata;
	vtkNEW(polydata);

	vtkPointData *outPointData = polydata->GetPointData();
	for (int i = 0; i < m_ScalarsNum; i++)
	{
		outPointData->AddArray(newArray[i]);
		vtkDEL(newArray[i]);
	}

	polydata->SetPoints(newPoints);
	vtkDEL(newPoints);

	polydata->SetLines(lines);
	vtkDEL(lines);

	polydata->Modified();

	albaVMEPolyline *polylineVME;
	albaNEW(polylineVME);
	polylineVME->SetName(name);
	int result = polylineVME->SetData(polydata, 0);
	vtkDEL(polydata);

	if (result == ALBA_OK)
	{
		polylineVME->ReparentTo(m_Input);
		m_Output = polylineVME;
	}

	albaDEL(polylineVME);

	ReadFinalize();

	return result;
}

//----------------------------------------------------------------------------
bool albaOpImporterPolyline::IsEmptyLine(char *line)
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
bool albaOpImporterPolyline::InternalAccept(albaVME *node)
{
	return true;
}

//----------------------------------------------------------------------------
void albaOpImporterPolyline::OpRun()
{
	CreateGui();
	ShowGui();
}
