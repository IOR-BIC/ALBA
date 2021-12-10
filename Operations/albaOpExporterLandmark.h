/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterLandmark
 Authors: Paolo Quadrani, Daniele Giunchi, Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterLandmark_H__
#define __albaOpExporterLandmark_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"
#include "albaVMELandmarkCloud.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// albaOpExporterLandmark :
//----------------------------------------------------------------------------
/** Exporter for the landmark coordinates: the data are exported in ASCII format. 
Each raw represents a landmark and contains the (x,y,z) coordinate.
The exporter can be performed on a single Landmark Cloud or on a tree containing many of them.
In the latter case the operation exports all the LCs in the input VME sub-tree. */
class ALBA_EXPORT albaOpExporterLandmark: public albaOp
{
public:
	albaOpExporterLandmark(const wxString &label = "Landmark Exporter");
	~albaOpExporterLandmark(); 
	albaOp* Copy();

	/** Build the interface of the operation, i.e the dialog that let choose the name of the output file. */
	void OpRun();

  /** Look for landmarks in the sub-tree */
  int FindLandmarkClouds(albaVME* node);  

  /** Export landmarks contained into a albaVMELandmarkCloud.*/
  void ExportLandmark(albaVMELandmarkCloud* cloud = NULL);

  /** Set the filename for the file to export */
  void SetFileName(const char *file_name) {m_File = file_name;};

  /** Set the filename for the directory where to export */
  void SetDirName(const char *file_name) {m_FileDir = file_name;};

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	wxString	m_File;
	wxString	m_FileDir;

  std::vector<albaVMELandmarkCloud*> m_LC_vector;
  std::vector<albaString> m_LC_names;
};
#endif
