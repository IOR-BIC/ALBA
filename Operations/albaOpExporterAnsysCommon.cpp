/*=========================================================================

Program: ALBA
Module: albaOpExporterAnsysCommon.cpp
Authors: Nicola Vanella, Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

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

#include "albaOpExporterAnsysCommon.h"
#include "albaOpImporterAnsysCommon.h"

#include "albaDecl.h"
#include "albaGUI.h"

#include "albaSmartPointer.h"
#include "albaTagItem.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaVMEMesh.h"
#include "albaVMEMeshAnsysTextExporter.h"
#include "albaAbsMatrixPipe.h"

#include <iostream>
#include <fstream>

// vtk includes
#include "vtkALBASmartPointer.h"
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
albaCxxAbstractTypeMacro(albaOpExporterAnsysCommon); 

//----------------------------------------------------------------------------
albaOpExporterAnsysCommon::albaOpExporterAnsysCommon(const wxString &label) :
albaOpExporterFEMCommon(label)
{
  m_AnsysOutputFileNameFullPath		= "";
}
//----------------------------------------------------------------------------
albaOpExporterAnsysCommon::~albaOpExporterAnsysCommon()
{

}

//----------------------------------------------------------------------------
void albaOpExporterAnsysCommon::OnOK()
{
  albaString wildcard = GetWildcard();

  m_AnsysOutputFileNameFullPath = "";

  wxString f;
  f = albaGetSaveFile("",wildcard).char_str(); 
  if(!f.IsEmpty())
  {
    m_AnsysOutputFileNameFullPath = f;
    Write();
  }
}
//----------------------------------------------------------------------------
void albaOpExporterAnsysCommon::OpStop(int result)
{
  HideGui();
  albaEventMacro(albaEvent(this,result));        
}


//----------------------------------------------------------------------------
int albaOpExporterAnsysCommon::compareElem(const void *p1, const void *p2) 
{
  ExportElement *a, *b;
  a = (ExportElement *)p1;
  b = (ExportElement *)p2;

  double result;

	// Compare Sort Order 
	// by elementType
	//  else by matID
	//        else by elementID

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
		{
			result = a->elementID - b->elementID;
			if (result < 0)
				return -1;
			else if (result > 0)
				return 1;
			else
				assert(0); //two elements have the same element ID
		}
  }
}

//---------------------------------------------------------------------------
ExportElement *albaOpExporterAnsysCommon::CreateExportElements(albaVMEMesh * input, int rowsNumber, vtkUnstructuredGrid * inputUGrid, FILE * file)
{
  // get the ELEMENT_ID array
  vtkIntArray *elementIdArray = input->GetElementsIDArray();

  // get the Nodes Id array
  vtkIntArray *nodesIDArray = input->GetNodesIDArray();

  // get the TYPE array
  vtkIntArray *typeArray = input->GetElementsTypeArray();

  // get the REAL array
  vtkIntArray *realArray = input->GetElementsRealArray();

  ExportElement *exportVector = new ExportElement[rowsNumber];

  int currType=-1;

  for (int rowID = 0 ; rowID < rowsNumber ; rowID++)
  {
    exportVector[rowID].elementID = elementIdArray ? elementIdArray->GetValue(rowID) : rowID+1;
    exportVector[rowID].matID = GetMatIdArray() ? GetMatIdArray()[rowID] : 1;
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


