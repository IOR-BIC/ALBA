/*=========================================================================

 Program: MAF2
 Module: mafOpImporterLandmarkWS
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpImporterLandmarkWS_H__
#define __mafOpImporterLandmarkWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMELandmarkCloud;  
class mafEvent;
//----------------------------------------------------------------------------
// mafOpImporterLandmarkWS :
//----------------------------------------------------------------------------
/** This importer loads an ASCII file containing all landmarks trajectories,i.e. the position of any marker 
for any single frame that composes the marker trajectory. Particularly, this file is produced by means of 
Vicon Workstation© employed in Human Movement Analysis.*/

class MAF_EXPORT mafOpImporterLandmarkWS : public mafOp
{
public:
	mafOpImporterLandmarkWS(wxString label);
	~mafOpImporterLandmarkWS(); 
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafVME* node) {return true;};

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
  wxString m_FileDir;
	wxString m_File;
	mafVMELandmarkCloud  *m_VmeCloud;
};
#endif
