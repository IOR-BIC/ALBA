/*=========================================================================

Program: MAF2
Module: mafOpExporterAnsysCommon.cpp
Authors: Nicola Vanella

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

#include "mafOpExporterAnsysCommon.h"
#include "mafOpImporterAnsysCommon.h"

#include "mafDecl.h"
#include "mafGUI.h"

#include "mafSmartPointer.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafVME.h"
#include "mafVMEMesh.h"
#include "mafVMEMeshAnsysTextExporter.h"
#include "mafAbsMatrixPipe.h"

#include <iostream>
#include <fstream>

// vtk includes
#include "vtkMAFSmartPointer.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkFieldData.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"

// vcl includes
#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <vcl_map.h>
#include <vcl_vector.h>
#include "wx/stdpaths.h"

//----------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafOpExporterAnsysCommon);

//----------------------------------------------------------------------------
mafOpExporterAnsysCommon::mafOpExporterAnsysCommon(const wxString &label) :
mafOpExporterFEMCommon(label)
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_ImporterType = 0;

  m_AnsysOutputFileNameFullPath		= "";
	wxStandardPaths std_paths;

  m_Pid = -1;
  m_ABSMatrixFlag = 1;
}
//----------------------------------------------------------------------------
mafOpExporterAnsysCommon::~mafOpExporterAnsysCommon()
{

}

//----------------------------------------------------------------------------
bool mafOpExporterAnsysCommon::Accept(mafVME *node)
{
  return (node->IsA("mafVMEMesh"));
}
//----------------------------------------------------------------------------
void mafOpExporterAnsysCommon::OpRun()   
{
	Init();
  CreateGui();
}
//----------------------------------------------------------------------------
void mafOpExporterAnsysCommon::OnEvent(mafEventBase *maf_event) 
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case wxOK:
      {
        OnOK();
        this->OpStop(OP_RUN_OK);
      }
      break;
    case wxCANCEL:
      {
        this->OpStop(OP_RUN_CANCEL);
      }
      break;
    default:
      mafEventMacro(*e);
      break;
    }	
  }
}
//----------------------------------------------------------------------------
void mafOpExporterAnsysCommon::OnOK()
{
  mafString wildcard = GetWildcard();

  m_AnsysOutputFileNameFullPath = "";

  wxString f;
  f = mafGetSaveFile("",wildcard).c_str(); 
  if(!f.IsEmpty())
  {
    m_AnsysOutputFileNameFullPath = f;
    Write();
  }
}
//----------------------------------------------------------------------------
void mafOpExporterAnsysCommon::OpStop(int result)
{
  HideGui();
  mafEventMacro(mafEvent(this,result));        
}

//----------------------------------------------------------------------------
void mafOpExporterAnsysCommon::CreateGui()
{
  Superclass::CreateGui();

	m_Gui->Divider(2);

	m_Gui->Label("Absolute matrix",true);
  m_Gui->Bool(ID_ABS_MATRIX_TO_STL,"Apply",&m_ABSMatrixFlag,0);

	m_Gui->Divider(2);
	m_Gui->Label("");

  m_Gui->OkCancel();  
  m_Gui->Divider();

	m_Gui->FitGui();
	m_Gui->Update();

  ShowGui();
}

//---------------------------------------------------------------------------
void mafOpExporterAnsysCommon::Init()
{
  mafVMEMesh *input = mafVMEMesh::SafeDownCast(m_Input);
  assert(input);

  vtkUnstructuredGrid *inputUGrid = input->GetUnstructuredGridOutput()->GetUnstructuredGridData();

  if(inputUGrid != NULL)
  {
    // Calculate Num of Elements
    m_TotalElements = inputUGrid->GetNumberOfPoints(); // Points

    m_TotalElements += inputUGrid->GetNumberOfCells(); // Elements

    vtkDataArray *materialsIDArray = NULL;
    materialsIDArray = inputUGrid->GetCellData()->GetArray("EX");

		if (materialsIDArray != NULL)
		{
			m_TotalElements += materialsIDArray->GetNumberOfTuples(); // Materials
		}
  }
}
//----------------------------------------------------------------------------
int mafOpExporterAnsysCommon::compareElem(const void *p1, const void *p2) 
{
  ExportElement *a, *b;
  a = (ExportElement *)p1;
  b = (ExportElement *)p2;

  double result;

  result = a->elementType - b->elementType;  
  if (result < 0)
    return -1;
  else if (result > 0)
    return 1;
  else
  {
    result = a->matID - b->matID;  
    if (result < 0)
      return -1;
    else if (result > 0)
      return 1;
    else
      return 0;
  }
}

//----------------------------------------------------------------------------
long mafOpExporterAnsysCommon::GetPid()   
{
  return m_Pid;
}

//---------------------------------------------------------------------------
ExportElement *mafOpExporterAnsysCommon::CreateExportElements(mafVMEMesh * input, int rowsNumber, vtkUnstructuredGrid * inputUGrid, FILE * file)
{
  // get the ELEMENT_ID array
  vtkIntArray *elementIdArray = input->GetElementsIDArray();

  // get the Nodes Id array
  vtkIntArray *nodesIDArray = input->GetNodesIDArray();

  // get the MATERIAL array
  vtkIntArray *materialArray = input->GetMaterialsIDArray();

  // get the TYPE array
  vtkIntArray *typeArray = input->GetElementsTypeArray();

  // get the REAL array
  vtkIntArray *realArray = input->GetElementsRealArray();

  ExportElement *exportVector = new ExportElement[rowsNumber];

  int currType=-1;

  for (int rowID = 0 ; rowID < rowsNumber ; rowID++)
  {
    exportVector[rowID].elementID = elementIdArray ? elementIdArray->GetValue(rowID) : rowID+1;
    exportVector[rowID].matID = materialArray ? materialArray->GetValue(rowID) : 1;
    exportVector[rowID].elementType = typeArray ? typeArray->GetValue(rowID) : 1;
    exportVector[rowID].elementReal = realArray ? realArray->GetValue(rowID) : 1;
    exportVector[rowID].cellID=rowID;
  }

  qsort(exportVector, rowsNumber, sizeof(ExportElement), compareElem);

  for (int rowID = 0 ; rowID < rowsNumber ; rowID++)
  {
    if(currType !=  exportVector[rowID].elementType)
    {
      int mode;

      vtkCell *currentCell = inputUGrid->GetCell(exportVector[rowID].cellID);
      vtkIdList *idList = currentCell->GetPointIds();
      int cellNpoints=currentCell->GetNumberOfPoints();

      switch (cellNpoints)
      {
      case 4:
        mode = 285;
        break;

      case 8: 
        mode = 45;
        break;

      case 10: 
        mode = 187;
        break;

      case 20: 
        mode = 186;
        break;

      default:
        mode = -1;
        break;
      }

      currType =  exportVector[rowID].elementType;
      fprintf(file,"ET,%d,%d\n", currType, mode);
    }
  }

  return exportVector;
}


