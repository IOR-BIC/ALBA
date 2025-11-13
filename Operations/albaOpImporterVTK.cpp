/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterVTK
 Authors: Paolo Quadrani
 
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

#include "albaOpImporterVTK.h"
#include <albaGUIBusyInfo.h>
#include "albaEvent.h"

#include "albaVME.h"
#include "albaVMEGeneric.h"
#include "albaVMEImage.h"
//#include "albaVMEPointSet.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMEPolyline.h"
#include "albaVMESurface.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEVolumeRGB.h"
#include "albaVMEMesh.h"

#include "albaTagArray.h"
#include "vtkALBASmartPointer.h"

#include "vtkDataSet.h"
#include "vtkDataSetReader.h"
#include "vtkPolyDataReader.h"
#include "vtkStructuredPointsReader.h"
#include "vtkStructuredGridReader.h"
#include "vtkRectilinearGridReader.h"
#include "vtkUnstructuredGridReader.h"

#include "wx\filename.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellData.h"
#include "vtkPointData.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterVTK);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpImporterVTK::albaOpImporterVTK(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_File    = "";

  //m_VmePointSet = NULL;
  m_VmeLandmarkCloud = NULL;
  m_VmePolyLine = NULL;
  m_VmeSurface  = NULL;
  m_VmeImage    = NULL;
  m_VmeGrayVol  = NULL;
  m_VmeRGBVol  = NULL;
  m_VmeMesh     = NULL;
  m_VmeGeneric  = NULL;

  m_FileDir = albaGetLastUserFolder();
}
//----------------------------------------------------------------------------
albaOpImporterVTK::~albaOpImporterVTK()
//----------------------------------------------------------------------------
{
  //albaDEL(m_VmePointSet);
  albaDEL(m_VmeLandmarkCloud);
  albaDEL(m_VmePolyLine);
  albaDEL(m_VmeSurface);
  albaDEL(m_VmeImage);
  albaDEL(m_VmeGrayVol);
  albaDEL(m_VmeRGBVol);
  albaDEL(m_VmeMesh);
  albaDEL(m_VmeGeneric);
}
//----------------------------------------------------------------------------
albaOp* albaOpImporterVTK::Copy()   
//----------------------------------------------------------------------------
{
  albaOpImporterVTK *cp = new albaOpImporterVTK(m_Label);
  cp->m_File			= m_File;
  return cp;
}
//----------------------------------------------------------------------------
void albaOpImporterVTK::OpRun()   
//----------------------------------------------------------------------------
{
	albaString wildc = "vtk Data (*.vtk)|*.vtk";
  albaString f;
  if (m_File.IsEmpty())
  {
    f = albaGetOpenFile(m_FileDir, wildc, _("Choose VTK file"));
    m_File = f;
  }

  int result = OP_RUN_CANCEL;
  if(!m_File.IsEmpty())
	{
    if (ImportVTK() == ALBA_OK)
    {
      result = OP_RUN_OK;
    }
    else
    {
      if(!this->m_TestMode)
        albaMessage(_("Unsupported file format"), _("I/O Error"), wxICON_ERROR );
    }
	}

	OpStop(result);
}
//----------------------------------------------------------------------------
int albaOpImporterVTK::ImportVTK()
//----------------------------------------------------------------------------
{
	bool success = false;
	albaGUIBusyInfo wait(_("Loading file: ..."),m_TestMode);

	vtkALBASmartPointer<vtkDataSetReader> reader;
	reader->SetFileName(m_File);

	vtkDataReader *preader = NULL;
	// workaround to avoid double reading
	switch (reader->ReadOutputType())
	{
	case VTK_POLY_DATA:
		preader = vtkPolyDataReader::New();
		break;
	case VTK_STRUCTURED_POINTS:
		preader = vtkStructuredPointsReader::New();
		break;
	case VTK_STRUCTURED_GRID:
		preader = vtkStructuredGridReader::New();
		break;
	case VTK_RECTILINEAR_GRID:
		preader = vtkRectilinearGridReader::New();
		break;
	case VTK_UNSTRUCTURED_GRID:
		preader = vtkUnstructuredGridReader::New();
		break;
	default:
		return ALBA_ERROR;
	}
	albaEventMacro(albaEvent(this, BIND_TO_PROGRESSBAR, preader));
	preader->SetFileName(m_File);
	preader->Update();

	if (preader->GetNumberOfOutputs() > 0)
	{
		wxString path, name, ext;
		wxFileName::SplitPath(m_File, &path, &name, &ext);

		vtkDataSet *data = vtkDataSet::SafeDownCast(preader->GetOutputs()[0]);
		if (data)
		{
			albaNEW(m_VmeLandmarkCloud);
			albaNEW(m_VmePolyLine);
			albaNEW(m_VmeSurface);
			albaNEW(m_VmeImage);
			albaNEW(m_VmeGrayVol);
			albaNEW(m_VmeRGBVol);
			albaNEW(m_VmeMesh);
			albaNEW(m_VmeGeneric);

			if(reader->ReadOutputType() == VTK_UNSTRUCTURED_GRID)
				CheckAndAddIDsToUnstructuredGrid(vtkUnstructuredGrid::SafeDownCast(data));
			//if (m_VmePointSet->SetDataByDetaching(data,0) == ALBA_OK)
			//{
			//  m_Output = m_VmePointSet;
			//}
			if (m_VmeLandmarkCloud->SetDataByDetaching(data, 0) == ALBA_OK)
			{
				m_VmeLandmarkCloud->CreateLMStructureFromDataVector();
				m_Output = m_VmeLandmarkCloud;
			}
			else if (m_VmePolyLine->SetDataByDetaching(data, 0) == ALBA_OK)
			{
				m_Output = m_VmePolyLine;
			}
			else if (m_VmeSurface->SetDataByDetaching(data, 0) == ALBA_OK)
			{
				m_Output = m_VmeSurface;
			}
			else if (m_VmeImage->SetDataByDetaching(data, 0) == ALBA_OK)
			{
				m_Output = m_VmeImage;
			}
			else if (m_VmeGrayVol->SetDataByDetaching(data, 0) == ALBA_OK)
			{
				m_Output = m_VmeGrayVol;
			}
			else if (m_VmeRGBVol->SetDataByDetaching(data, 0) == ALBA_OK)
			{
				m_Output = m_VmeRGBVol;
			}
			else if (m_VmeMesh->SetDataByDetaching(data, 0) == ALBA_OK)
			{
				m_Output = m_VmeMesh;
			}
			else
			{
				m_VmeGeneric->SetDataByDetaching(data, 0);
				m_Output = m_VmeGeneric;
			}

			albaTagItem tag_Nature;
			tag_Nature.SetName("VME_NATURE");
			tag_Nature.SetValue("NATURAL");
			m_Output->GetTagArray()->SetTag(tag_Nature);
			m_Output->ReparentTo(m_Input);
			m_Output->SetName(name.ToAscii());

			success = true;
		}
	}
	vtkDEL(preader);
	if (!success && !this->m_TestMode)
	{
		albaMessage(_("Error reading VTK file."), _("I/O Error"), wxICON_ERROR);
		return ALBA_ERROR;
	}
	return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaOpImporterVTK::OpStop(int result)
{
				albaEventMacro(albaEvent(this, result));
}

//----------------------------------------------------------------------------
char ** albaOpImporterVTK::GetIcon()
{
#include "pic/MENU_IMPORT_VTK.xpm"
	return MENU_IMPORT_VTK_xpm;
}

void albaOpImporterVTK::CheckAndAddIDsToUnstructuredGrid(vtkUnstructuredGrid* ug)
{
	if (!ug->GetCellData()->GetArray("Id"))
	{
		vtkALBASmartPointer<vtkIntArray> idArray;
		idArray->SetName("Id");
		idArray->SetNumberOfComponents(1);
		idArray->SetNumberOfTuples(ug->GetNumberOfCells());
		for (vtkIdType i = 0; i < ug->GetNumberOfCells(); i++)
			idArray->SetValue(i, i);
		ug->GetCellData()->AddArray(idArray);
	}

	if (!ug->GetPointData()->GetArray("Id"))
	{
		vtkALBASmartPointer<vtkIntArray> idArray;
		idArray->SetName("Id");
		idArray->SetNumberOfComponents(1);
		idArray->SetNumberOfTuples(ug->GetNumberOfPoints());
		for (vtkIdType i = 0; i < ug->GetNumberOfPoints(); i++)
			idArray->SetValue(i, i);
		ug->GetPointData()->AddArray(idArray);
	}
}
