/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterPointCloud
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
//----------------------------------------------------------------------------
#include "albaOpExporterPointCloud.h"

#include "albaDecl.h"
#include "albaGUIBusyInfo.h"
#include "albaVMEPointCloud.h"

#include "vtkDataArray.h"
#include "vtkDataSet.h"
#include "vtkPointData.h"

#include <vector>

//----------------------------------------------------------------------------
albaOpExporterPointCloud::albaOpExporterPointCloud(const wxString &label) :	albaOp(label)
{
	m_OpType = OPTYPE_EXPORTER;
	m_Canundo = true;
	m_File = "";
}

//----------------------------------------------------------------------------
albaOpExporterPointCloud::~albaOpExporterPointCloud()
{
}

//----------------------------------------------------------------------------
albaOp *albaOpExporterPointCloud::Copy()
{
	albaOpExporterPointCloud *copy = new albaOpExporterPointCloud(m_Label);
	copy->m_File = m_File;
	return copy;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool albaOpExporterPointCloud::InternalAccept(albaVME *node)
//----------------------------------------------------------------------------
{
	return node != NULL && node->IsALBAType(albaVMEPointCloud);
}

//----------------------------------------------------------------------------
void albaOpExporterPointCloud::OpRun()
{
	int result = OP_RUN_CANCEL;

	// Ask the user to choose the output file when one has not already been set.
	if (m_File.IsEmpty())
	{
		wxString proposed = albaGetLastUserFolder();
		proposed += wxString("\\") + m_Input->GetName();
		proposed += ".txt";

		wxString wildcard = "Text file (*.txt)|*.txt";
		m_File = albaGetSaveFile(proposed, wildcard).ToAscii();
	}

	// Export the point cloud and report whether the operation succeeded.
	if (!m_File.IsEmpty() && ExportPointCloud())
		result = OP_RUN_OK;

	albaEventMacro(albaEvent(this, result));
}

//----------------------------------------------------------------------------
bool albaOpExporterPointCloud::ExportPointCloud()
{
	albaGUIBusyInfo wait(_("Saving point cloud: Please wait"), m_TestMode);

	albaVMEPointCloud *pointCloud = (albaVMEPointCloud *)m_Input;
	vtkDataSet *data = pointCloud->GetOutput()->GetVTKData();

	// The output data set contains the point coordinates and point-data arrays.
	if (data == NULL)
		return false;

	FILE *outFile = albaTryOpenFile(m_File.ToAscii(), "w");
	if (outFile == NULL)
	{
		albaErrorMessage("Cannot open file: %s", m_File.ToAscii());
		return false;
	}

	// Collect numeric point-data arrays for writing their names and values.
	vtkPointData *pointData = data->GetPointData();
	std::vector<vtkDataArray *> arrays;

	if (pointData != NULL)
	{
		for (int i = 0; i < pointData->GetNumberOfArrays(); i++)
		{
			vtkDataArray *array = pointData->GetArray(i);
			if (array != NULL)
				arrays.push_back(array);
		}
	}

	// Write the column names; array values follow the three coordinate columns.
	fprintf(outFile, "#\tX\tY\tZ");
	for (int i = 0; i < arrays.size(); i++)
		fprintf(outFile, "\t%s", arrays[i]->GetName() ? arrays[i]->GetName() : "");
	fprintf(outFile, "\n");

	// Write one row per point, using the first component of each array.
	for (vtkIdType i = 0; i < data->GetNumberOfPoints(); i++)
	{
		double point[3];
		data->GetPoint(i, point);

		fprintf(outFile, "%lf\t%lf\t%lf", point[0], point[1], point[2]);
		for (int j = 0; j < arrays.size(); j++)
			fprintf(outFile, "\t%lf", arrays[j]->GetTuple1(i));
		fprintf(outFile, "\n");
	}

	fclose(outFile);
	return true;
}