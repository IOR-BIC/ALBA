/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterPolyline
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
#include "albaOpExporterPolyline.h"

#include "albaDecl.h"
#include "albaGUIBusyInfo.h"
#include "albaVMEPolyline.h"

#include "vtkCellArray.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"

#include <vector>

//----------------------------------------------------------------------------
albaOpExporterPolyline::albaOpExporterPolyline(const wxString &label) :	albaOp(label)
{
	m_OpType = OPTYPE_EXPORTER;
	m_Canundo = true;
	m_File = "";
}

//----------------------------------------------------------------------------
albaOpExporterPolyline::~albaOpExporterPolyline()
{
}

//----------------------------------------------------------------------------
albaOp *albaOpExporterPolyline::Copy()
{
	albaOpExporterPolyline *copy = new albaOpExporterPolyline(m_Label);
	copy->m_File = m_File;
	return copy;
}

//----------------------------------------------------------------------------
bool albaOpExporterPolyline::InternalAccept(albaVME *node)
{
	return node != NULL && node->IsALBAType(albaVMEPolyline);
}

//----------------------------------------------------------------------------
void albaOpExporterPolyline::OpRun()
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

	// Export the polyline and report whether the operation succeeded.
	if (!m_File.IsEmpty() && ExportPolyline())
		result = OP_RUN_OK;

	albaEventMacro(albaEvent(this, result));
}

//----------------------------------------------------------------------------
bool albaOpExporterPolyline::ExportPolyline()
{
	albaGUIBusyInfo wait(_("Saving polyline: Please wait"), m_TestMode);

	albaVMEPolyline *polyline = (albaVMEPolyline *)m_Input;
	polyline->Update();
	vtkPolyData *data = vtkPolyData::SafeDownCast(polyline->GetOutput()->GetVTKData());

	// The output data set contains the point coordinates, the line cells and point-data arrays.
	if (data == NULL || data->GetLines() == NULL)
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

	// Write the points of each polyline in order, separating polylines with an empty line.
	vtkCellArray *lines = data->GetLines();
	vtkIdType npts;
	const vtkIdType *pts;
	bool first = true;

	lines->InitTraversal();
	while (lines->GetNextCell(npts, pts))
	{
		if (!first)
			fprintf(outFile, "\n");
		first = false;

		for (vtkIdType i = 0; i < npts; i++)
		{
			double point[3];
			data->GetPoint(pts[i], point);

			fprintf(outFile, "%lf\t%lf\t%lf", point[0], point[1], point[2]);
			for (int j = 0; j < arrays.size(); j++)
				fprintf(outFile, "\t%lf", arrays[j]->GetTuple1(pts[i]));
			fprintf(outFile, "\n");
		}
	}

	fclose(outFile);
	return true;
}
