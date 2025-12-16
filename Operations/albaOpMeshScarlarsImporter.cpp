/*=========================================================================
  Program:   Bonemat
  Module:    albaOpMeshScarlarsImporter.cpp
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

#include "albaDecl.h"
#include "albaOpMeshScarlarsImporter.h"

#include "albaVMEMesh.h"
#include "albaGUI.h"

#include "vtkFloatArray.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include <unordered_map>
#include "vtkALBASmartPointer.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpMeshScarlarsImporter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpMeshScarlarsImporter::albaOpMeshScarlarsImporter(const wxString &label) :
albaOp(label)
{ 
	m_OpType = OPTYPE_IMPORTER;
	m_DefaultValues = NULL;
}
//----------------------------------------------------------------------------
albaOpMeshScarlarsImporter::~albaOpMeshScarlarsImporter()
{
	delete[] m_DefaultValues;
}

//----------------------------------------------------------------------------
albaOp* albaOpMeshScarlarsImporter::Copy()   
{
  albaOpMeshScarlarsImporter *cp = new albaOpMeshScarlarsImporter(m_Label);
  return cp;
}

//----------------------------------------------------------------------------
int albaOpMeshScarlarsImporter::ImportFile(void)
{

	if (ReadInit(m_LisScalarFile, GetTestMode(), true, "Please wait parsing Lis File...", m_Listener) == ALBA_ERROR)
	{
		albaErrorMessage("Cannot Open: %s", m_LisScalarFile.GetCStr());
		ReadFinalize();
		return ALBA_ERROR;
	}
	int lineLenght;
	bool headerReaded = false;

	char firstWord[100];
	double tmpDouble;

	albaString nodeStr = "NODE";
	albaString elementStr = "ELEM";
	
	while ((lineLenght = GetLine(true)) != 0)
	{
		//skip empty lines
		if (sscanf(m_Line, "%s", firstWord) != 1)
			continue;

		int isNumber = sscanf(firstWord, "%f", &tmpDouble);
		
		if(isNumber == 1 && !headerReaded)
		{
			albaErrorMessage("Header not found");
			ReadFinalize();
			return ALBA_ERROR;
		}
		
		if (!headerReaded && (nodeStr.Equals(firstWord) || elementStr.Equals(firstWord)))
		{
			if (ReadHeader() == ALBA_ERROR)
			{
				albaErrorMessage("Wrong Header");
				ReadFinalize();
				return ALBA_ERROR;
			}
			headerReaded = true;
			continue;
		}

		//Skip text lines and repeated headers
		if (isNumber != 1)
			continue;

		else
		{
			if (ReadLine() == ALBA_ERROR)
			{
				albaErrorMessage("Wrong Column number on line %d", m_CurrentLine);
				DeleteArrays();
				ReadFinalize();
				return ALBA_ERROR;
			}
		}
	}

	ReadFinalize();

	return ALBA_OK;
}

//----------------------------------------------------------------------------
bool albaOpMeshScarlarsImporter::InternalAccept(albaVME *node)
{ 
	return (node && albaVMEMesh::SafeDownCast(node));
}
//----------------------------------------------------------------------------
albaOpMeshScarlarsImporter::CheckIDResults albaOpMeshScarlarsImporter::IDsToIndexes()
{
	vtkUnstructuredGrid *mesh=vtkUnstructuredGrid::SafeDownCast(m_Input->GetOutput()->GetVTKData());
	vtkIntArray* meshIDs;

	if(m_ScalarScope==ELEMENT_SCALARS)
		meshIDs = vtkIntArray::SafeDownCast(mesh->GetCellData()->GetScalars("Id"));
	else //NODE_SCALARS
		meshIDs = vtkIntArray::SafeDownCast(mesh->GetPointData()->GetScalars("Id"));
	m_MeshScalarNum = meshIDs->GetNumberOfTuples();

	// Build a map: ID -> index
	std::unordered_map<int, vtkIdType> idToIndex;
	idToIndex.reserve(m_MeshScalarNum); // optimization

	for (vtkIdType i = 0; i < m_MeshScalarNum; i++)
	{
		int meshId = meshIDs->GetValue(i);
		idToIndex[meshId] = i;
	}
	
	//cycle on IDs loaded from file
	for (vtkIdType i = 0; i < m_Ids.size(); i++) 
	{
		std::unordered_map<int, vtkIdType>::iterator it = idToIndex.find(m_Ids[i]);
		if (it != idToIndex.end())
			m_Ids[i] = it->second;
		else
			return WRONG_IDS;
	}

	return (m_MeshScalarNum == m_Ids.size()) ? PERFECT_MATCH_IDS : SUBGROUP_IDS;
}

//----------------------------------------------------------------------------
void albaOpMeshScarlarsImporter::DeleteArrays()
{
	for (int i = 0; i < m_Arrays.size(); i++)
		vtkDEL(m_Arrays[i]);

	m_Arrays.clear();
}

//----------------------------------------------------------------------------
int albaOpMeshScarlarsImporter::ReadHeader()
{
	albaString nodeStr = "NODE";
	albaString elementStr = "ELEM";

	char currentWord[100];
	int readChar = 0;
	int totReadChar = 0;
	int readElements;

	sscanf(m_Line, "%s%n", currentWord, &totReadChar);

	if (nodeStr.Equals(currentWord))
		m_ScalarScope = NODE_SCALARS;
	else if (elementStr.Equals(currentWord))
		m_ScalarScope = ELEMENT_SCALARS;
	else
		return ALBA_ERROR;
	
	do 
	{
		readElements = sscanf(m_Line+totReadChar, "%s%n", currentWord, &readChar);

		if (readElements == 1)
		{
			totReadChar += readChar;
			m_ScalarNames.push_back(currentWord);

			vtkFloatArray* fArray;
			vtkNEW(fArray);
			m_Arrays.push_back(fArray);
		}
	} while (readElements==1);

	return ALBA_OK;
}

//----------------------------------------------------------------------------
int albaOpMeshScarlarsImporter::ReadLine()
{
	unsigned long int id;
	int readChar = 0;
	int totReadChar = 0;
	float value;
	int readElements;
	int currentScalar = 0;
	int arraysNum = m_Arrays.size();



	if (sscanf(m_Line, "%lu%n", &id, &totReadChar) != 1)
		return ALBA_ERROR;

	m_Ids.push_back(id);

	do
	{
		readElements = sscanf(m_Line + totReadChar, "%f%n", &value, &readChar);

		if (readElements == 1)
		{
			if (currentScalar > arraysNum)
				return ALBA_ERROR;

			totReadChar += readChar;
			m_Arrays[currentScalar]->InsertNextValue(value);
			currentScalar++;
		}
	} while (readElements == 1);

	if (currentScalar != arraysNum)
		return ALBA_ERROR;

	return ALBA_OK;
}
//----------------------------------------------------------------------------
void albaOpMeshScarlarsImporter::CreateNewScalars()
{
	vtkUnstructuredGrid* mesh = vtkUnstructuredGrid::SafeDownCast(m_Input->GetOutput()->GetVTKData());
	int idNum = m_Ids.size();


	for (int i = 0; i < m_ScalarNames.size(); i++)
	{

		vtkDataArray* existingArray = NULL;
		if (m_ScalarScope == ELEMENT_SCALARS)
			existingArray = mesh->GetCellData()->GetArray(m_ScalarNames[i]);
		else //NODE_SCALARS
			existingArray = mesh->GetPointData()->GetArray(m_ScalarNames[i]);
		
		if (existingArray != NULL)
		{
			albaString message;
			message.Printf("The mesh already has scalars named %s.\nDo you want to replace the current scalars with new ones?", m_ScalarNames[i].GetCStr());
			wxMessageDialog dialog(NULL, message.GetCStr(), "Scalar already present", wxYES_NO | wxICON_QUESTION);
			int answer = dialog.ShowModal();

			if (answer == wxID_YES) 
			{
				if (m_ScalarScope == ELEMENT_SCALARS)
					mesh->GetCellData()->RemoveArray(m_ScalarNames[i]);
				else //NODE_SCALARS
					mesh->GetPointData()->RemoveArray(m_ScalarNames[i]);
			}
			else 
			{
				continue;
			}
		}

		vtkALBASmartPointer<vtkFloatArray> newArray;

		newArray->SetName(m_ScalarNames[i]);
		newArray->SetNumberOfValues(m_MeshScalarNum);

		//fill with default value if necessary
		if (m_DefaultValues != NULL)
			std::fill_n(newArray->GetPointer(0), idNum, m_DefaultValues[i]);
		
		//setting values to the array
		for (int j = 0; j < idNum; j++)
			newArray->SetValue(m_Ids[j], m_Arrays[i]->GetValue(j));

		if (m_ScalarScope == ELEMENT_SCALARS)
			mesh->GetCellData()->AddArray(newArray);
		else //NODE_SCALARS
			mesh->GetPointData()->AddArray(newArray);
	}

	albaVMEMesh::SafeDownCast(m_Input)->SetData(mesh, 0);

	GetLogicManager()->VmeVisualModeChanged(m_Input);
}
//----------------------------------------------------------------------------
void albaOpMeshScarlarsImporter::OnEvent(albaEventBase* alba_event)
{
	if (albaEvent* e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
		case wxOK:
			CreateNewScalars();
			OpStop(OP_RUN_OK);
			break;
		case wxCANCEL:
			OpStop(OP_RUN_CANCEL);
			break;
		}
	}
}


//----------------------------------------------------------------------------
void albaOpMeshScarlarsImporter::CreateGui()
{
	// interface:
	m_Gui = new albaGUI(this);

	m_Gui->Label("");

	if (m_ScalarScope == ELEMENT_SCALARS)
		m_Gui->Label("Some Elements have no scalar assigned.");
	else
		m_Gui->Label("Some Nodes have no scalar assigned.");

	m_Gui->Label("	Please set default values.");
	m_Gui->Label("");
	
	for (int i = 0; i < m_ScalarNames.size(); i++)
		m_Gui->Float(-1, m_ScalarNames[i], m_DefaultValues + i);

	m_Gui->OkCancel();

	ShowGui();
}

//----------------------------------------------------------------------------
void albaOpMeshScarlarsImporter::OpRun()
{
	albaString wildcard = "Scalar lis files (*.lis)|*.lis|All Files (*.*)|*.*";

	m_LisScalarFile = "";

	wxString f;
	f = albaGetOpenFile("Select scalar Lis File", wildcard); 
	if(!f.IsEmpty() && wxFileExists(f))
	{
		m_LisScalarFile = f;
		
		if (ImportFile() == ALBA_ERROR)
		{
			albaEventMacro(albaEvent(this, OP_RUN_CANCEL));
			return;
		}
	
		switch (IDsToIndexes())
		{
		case PERFECT_MATCH_IDS:
			CreateNewScalars();
			albaEventMacro(albaEvent(this, OP_RUN_OK));
			return;
			break;
		case SUBGROUP_IDS:
			m_DefaultValues = new float[m_ScalarNames.size()];
			std::fill_n(m_DefaultValues, m_ScalarNames.size(), 0.0f);

			CreateGui();
			break;
		case WRONG_IDS:
			albaErrorMessage("Selected file contains IDs that are not present in the mesh.");
			albaEventMacro(albaEvent(this, OP_RUN_CANCEL));
			break;
		}
	}
}
