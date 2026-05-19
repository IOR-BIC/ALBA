/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterDicFile
 Authors: Gianluigi Crimi
 
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
#include <albaGUIBusyInfo.h>

#include "albaOpExporterDicFile.h"

#include "albaDecl.h"
#include "albaGUI.h"
#include "vtkALBASmartPointer.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMERoot.h"
#include "albaTransformBase.h"

#include "vtkDataSetWriter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkAbstractTransform.h"
#include "vtkImageCast.h"
#include "vtkImageData.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"
#include "albaVMEGroup.h"
#include "albaProgressBarHelper.h"
#include "vtkUnstructuredGrid.h"
#include "vtkTransformFilter.h"
#include "albaTagArray.h"
#include "albaVMEPointCloud.h"
#include "vtkPointData.h"
//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpExporterDicFile);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpExporterDicFile::albaOpExporterDicFile(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_File    = "";
  m_Input   = NULL;

	m_Binary        = 1;
	m_ABSMatrixFlag = 0;

	m_ForceUnsignedShortScalarOutputForStructuredPoints = false;
}
//----------------------------------------------------------------------------
albaOpExporterDicFile::~albaOpExporterDicFile()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool albaOpExporterDicFile::InternalAccept(albaVME *node)
//----------------------------------------------------------------------------
{ 
	return (node->IsALBAType(albaVMEPointCloud) && !node->GetTagArray()->IsTagPresent("albaVMERoot"));
}
//----------------------------------------------------------------------------
albaOp* albaOpExporterDicFile::Copy()   
//----------------------------------------------------------------------------
{
  albaOpExporterDicFile *cp = new albaOpExporterDicFile(m_Label);
  cp->m_File = m_File;
  return cp;
}
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum VTK_EXPORTER_ID
{
	ID_ABS_MATRIX = MINID,
  ID_CHOOSE_FILENAME,
};
//----------------------------------------------------------------------------
void albaOpExporterDicFile::OpRun()
//----------------------------------------------------------------------------
{
	vtkDataSet *inputData = m_Input->GetOutput()->GetVTKData();
	assert(inputData);

	albaString wildc = "vtk Data (*.dat)|*.dat";

	m_FileDir = albaGetLastUserFolder();
	m_File = m_FileDir + "\\" + m_Input->GetName() + ".dat";

	m_Gui = new albaGUI(this);
	m_Gui->FileSave(ID_CHOOSE_FILENAME, _("DIC file"), &m_File, wildc);
/*	m_Gui->Label("File type", true);*/
	m_Gui->Bool(ID_ABS_MATRIX, "Apply Absolute matrix", &m_ABSMatrixFlag, 1);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	m_Gui->Enable(wxOK, !m_File.IsEmpty());

	ShowGui();
}
//----------------------------------------------------------------------------
void albaOpExporterDicFile::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
    switch(e->GetId())
    {
      case wxOK:
        ExportDIC();
        OpStop(OP_RUN_OK);
      break;
      case ID_CHOOSE_FILENAME:
        m_Gui->Enable(wxOK, !m_File.IsEmpty());
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);
      break;
			case ID_ABS_MATRIX:
				break;
      default:
        albaEventMacro(*e);
      break;
    }
	}
}

//----------------------------------------------------------------------------
void albaOpExporterDicFile::ExportDIC()
//----------------------------------------------------------------------------
{					
	m_Input->GetOutput()->Update();

  vtkDataSet *vtkData = m_Input->GetOutput()->GetVTKData();
	vtkPointData *outPointData = vtkData->GetPointData();

	if(!outPointData->GetArray("Deformed Coordinates") || !outPointData->GetArray("Displacements") || !outPointData->GetArray("Displ Magnitudo") || !outPointData->GetArray("Index X") || !outPointData->GetArray("Index Y"))
	{
 		albaMessage( _("DIC Data not present!"), _("Warning"), wxOK|wxICON_WARNING);
	}
	else
	{
    SaveDICData();
	}
}
//----------------------------------------------------------------------------
void albaOpExporterDicFile::SaveDICData()
//----------------------------------------------------------------------------
{
	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar();

	vtkDataSet *inputData = m_Input->GetOutput()->GetVTKData();
	assert(inputData);

	vtkALBASmartPointer <vtkTransform> tra;
	tra->SetMatrix(m_Input->GetOutput()->GetAbsMatrix()->GetVTKMatrix());

	albaMatrix rotMRT;
	vtkALBASmartPointer <vtkTransform> rotTra;
	
	rotMRT.CopyRotation(*m_Input->GetOutput()->GetAbsMatrix());
	rotTra->SetMatrix(rotMRT.GetVTKMatrix());

	vtkDataSet *writerInput = NULL;

	vtkPointData *pointData = inputData->GetPointData();
	vtkDataArray *deformedCoords = pointData->GetArray("Deformed Coordinates");
	vtkDataArray *displacements = pointData->GetArray("Displacements");
	vtkDataArray *displMagnitudo = pointData->GetArray("Displ Magnitudo");
	vtkDataArray *indexX = pointData->GetArray("Index X");
	vtkDataArray *indexY = pointData->GetArray("Index Y");

	FILE *file = fopen(m_File.GetCStr(), "w");
	if (!file)
	{
		albaMessage(_("Unable to open file for writing!"), _("Error"), wxOK | wxICON_ERROR);
		return;
	}

	vtkIdType numPoints = inputData->GetNumberOfPoints();

	fprintf(file, "%%This file was generated by ALBA Framework\n");
	fprintf(file, "%%Index_x, Index_y, Coordinate-undef (x,y,z), Coord-def (x,y,z), Displ(x,y,z,e)\n");

	for (vtkIdType i = 0; i < numPoints; ++i)
	{
		fprintf(file, "%d ", (int)indexX->GetTuple1(i));
		fprintf(file, "%d ", (int)indexY->GetTuple1(i));

		double point[4], deformed[4], displacem[4];
		inputData->GetPoint(i, point);
		double *defTuple = deformedCoords->GetTuple3(i);
		deformed[0] = defTuple[0];
		deformed[1] = defTuple[1];
		deformed[2] = defTuple[2];
		double *displTuple = displacements->GetTuple3(i);
		displacem[0] = displTuple[0];
		displacem[1] = displTuple[1];
		displacem[2] = displTuple[2];

		point[3] = deformed[3] = displacem[3] = 1;

		if (m_ABSMatrixFlag)
		{
			tra->MultiplyPoint(point, point);
			tra->MultiplyPoint(deformed, deformed);

			rotTra->MultiplyPoint(displacem, displacem);
		}
		
		fprintf(file, "%.6f %.6f %.6f ", point[0], -point[2], point[1]);
		fprintf(file, "%.6f %.6f %.6f ", deformed[0], -deformed[2], deformed[1]);
		fprintf(file, "%.6f %.6f %.6f ", displacem[0], -displacem[2], displacem[1]);
		fprintf(file, "%.6f\n", displMagnitudo->GetTuple1(i));

		progressHelper.UpdateProgressBar(((float)i / numPoints)*100);
	}

	fclose(file);
	progressHelper.UpdateProgressBar(100);
}

//----------------------------------------------------------------------------
char **albaOpExporterDicFile::GetIcon()
{
#include "pic/MENU_IMPORT_VTK.xpm"
	return MENU_IMPORT_VTK_xpm;
}