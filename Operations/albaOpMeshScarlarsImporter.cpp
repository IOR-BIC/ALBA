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
#include "vtkPointData.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpMeshScarlarsImporter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpMeshScarlarsImporter::albaOpMeshScarlarsImporter(const wxString &label) :
albaOp(label)
{ 
	m_OpType = OPTYPE_IMPORTER;
}
//----------------------------------------------------------------------------
albaOpMeshScarlarsImporter::~albaOpMeshScarlarsImporter()
{
}

//----------------------------------------------------------------------------
albaOp* albaOpMeshScarlarsImporter::Copy()   
{
  albaOpMeshScarlarsImporter *cp = new albaOpMeshScarlarsImporter(m_Label);
  return cp;
}

//----------------------------------------------------------------------------
int albaOpMeshScarlarsImporter::Import(void)
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

	albaString printStr = "PRINT";
	albaString nodeStr = "NODE";
	albaString elementStr = "ELEM";
	albaString statStr = "STAT";
	albaString emptyStr = "";

	while ((lineLenght = GetLine(true)) != 0)
	{
		//skip empty lines
		if (m_Line[0]=='\n')
			continue;

		if (sscanf(m_Line, "%s", firstWord) != 1)
		{
			albaErrorMessage("General Error");
			ReadFinalize();
			return ALBA_ERROR;
		}

		//Skip lines 
		if (printStr.Equals(firstWord) || statStr.Equals(firstWord) || (headerReaded && nodeStr.Equals(firstWord)) || (headerReaded && elementStr.Equals(firstWord)))
			continue;

		if (!headerReaded)
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


	return ALBA_OK;
}

//----------------------------------------------------------------------------
bool albaOpMeshScarlarsImporter::InternalAccept(albaVME *node)
{ 
	return (node && albaVMEMesh::SafeDownCast(node));
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
void albaOpMeshScarlarsImporter::OpRun()
{
	albaString wildcard = "Scalar lis files (*.lis)|*.lis|All Files (*.*)|*.*";

	int result = OP_RUN_CANCEL;
	m_LisScalarFile = "";

	wxString f;
	f = albaGetOpenFile("Select scalar Lis File", wildcard); 
	if(!f.IsEmpty() && wxFileExists(f))
	{
		m_LisScalarFile = f;
		Import();
		result = OP_RUN_OK;
	}
	albaEventMacro(albaEvent(this,result));  
}
