/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterLandmarkWS
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterLandmarkWS_H__
#define __albaOpImporterLandmarkWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMELandmarkCloud;  
class albaEvent;
//----------------------------------------------------------------------------
// albaOpImporterLandmarkWS :
//----------------------------------------------------------------------------
/** This importer loads an ASCII file containing all landmarks trajectories,i.e. the position of any marker 
for any single frame that composes the marker trajectory. Particularly, this file is produced by means of 
Vicon Workstation© employed in Human Movement Analysis.*/

class ALBA_EXPORT albaOpImporterLandmarkWS : public albaOp
{
public:
	albaOpImporterLandmarkWS(wxString label);
	~albaOpImporterLandmarkWS(); 
	albaOp* Copy();

	/** Builds operation's interface. */
	void OpRun();

  /** Read the file.
  the format of the file requires some spec
  1) Check "TRAJECTORIES" as string of the first line
  2) Read the frequency 
  3) Read the names of the AL
  2) Ignore the fourth line
  3) the first element of each line is Time, then x y z x1 y1 z1 etc
  */
  void Read();

  /** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_File = file_name;};

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node) { return true; };

  wxString m_FileDir;
	wxString m_File;
	albaVMELandmarkCloud  *m_VmeCloud;
};
#endif
