/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterPointCloud
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE. See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterPointCloud_H__
#define __albaOpExporterPointCloud_H__

#include "albaOp.h"

/** Exporter for point cloud coordinates and point-data arrays.
The output is an ASCII text file with one point per row. */
class ALBA_EXPORT albaOpExporterPointCloud : public albaOp
{
public:
	albaOpExporterPointCloud(const wxString &label = "Point Cloud");
	~albaOpExporterPointCloud();

	/** Create a copy of this operation. */
	albaOp *Copy();

	/** Build the interface that lets the user choose the output file. */
	void OpRun();

	/** Set the name of the file to export. */
	void SetFileName(const char *file_name) { m_File = file_name; }

	/** Export the input point cloud to the selected file. */
	bool ExportPointCloud();

protected:
	/** Return true only for albaVMEPointCloud inputs. */
	bool InternalAccept(albaVME *node);

	wxString m_File;
};

#endif