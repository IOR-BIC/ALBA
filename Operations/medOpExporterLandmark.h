/*=========================================================================

 Program: MAF2Medical
 Module: medOpExporterLandmark
 Authors: Paolo Quadrani, Daniele Giunchi, Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpExporterLandmark_H__
#define __medOpExporterLandmark_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"
#include "mafVMELandmarkCloud.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// medOpExporterLandmark :
//----------------------------------------------------------------------------
/** Exporter for the landmark coordinates: the data are exported in ASCII format. 
Each raw represents a landmark and contains the (x,y,z) coordinate.
The exporter can be performed on a single Landmark Cloud or on a tree containing many of them.
In the latter case the operation exports all the LCs in the input VME sub-tree. */
class MAF_EXPORT medOpExporterLandmark: public mafOp
{
public:
	medOpExporterLandmark(const wxString &label = "Landmark Exporter");
	~medOpExporterLandmark(); 
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Build the interface of the operation, i.e the dialog that let choose the name of the output file. */
	void OpRun();

  /** Look for landmarks in the sub-tree */
  int FindLandmarkClouds(mafNode* node);  

  /** Export landmarks contained into a mafVMELandmarkCloud.*/
  void ExportLandmark(mafVMELandmarkCloud* cloud = NULL);

  /** Set the filename for the file to export */
  void SetFileName(const char *file_name) {m_File = file_name;};

  /** Set the filename for the directory where to export */
  void SetDirName(const char *file_name) {m_FileDir = file_name;};

protected:
	wxString	m_File;
	wxString	m_FileDir;

  std::vector<mafVMELandmarkCloud*> m_LC_vector;
  std::vector<mafString> m_LC_names;
};
#endif
