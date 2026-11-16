/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterPolyline
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE. See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterPolyline_H__
#define __albaOpExporterPolyline_H__

#include "albaOp.h"

/** Exporter for polyline coordinates and point-data arrays.
The output is an ASCII text file with one point per row, in polyline order.
Each polyline is separated from the next one by an empty line. */
class ALBA_EXPORT albaOpExporterPolyline : public albaOp
{
public:
	albaOpExporterPolyline(const wxString &label = "Polyline");
	~albaOpExporterPolyline();

	/** Create a copy of this operation. */
	albaOp *Copy();

	/** Build the interface that lets the user choose the output file. */
	void OpRun();

	/** Set the name of the file to export. */
	void SetFileName(const char *file_name) { m_File = file_name; }

	/** Export the input polyline to the selected file. */
	bool ExportPolyline();

protected:
	/** Return true only for albaVMEPolyline inputs. */
	bool InternalAccept(albaVME *node);

	wxString m_File;
};

#endif
