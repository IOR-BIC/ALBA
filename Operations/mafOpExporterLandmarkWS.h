/*=========================================================================

 Program: MAF2
 Module: mafOpExporterLandmarkWS
 Authors: Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpExporterLandmarkWS_H__
#define __mafOpExporterLandmarkWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafVMELandmarkCloud;

/** 
class name: mafOpExporterLandmarkWS
/** Export trajectories in a cvs file */
class MAF_EXPORT mafOpExporterLandmarkWS : public mafOp
{
public:
	mafOpExporterLandmarkWS(const wxString &label = "Trajectories Exporter");
	~mafOpExporterLandmarkWS(); 
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node);

	/** Builds operation's interface. */
	void OpRun();

  /** Write the file.
  File format:

  TRAJECTORIES
  Frequency,Hz
  PointName1,PointName2,..,PointName(n-1)
  Time, x,y,z, ...
  time0 x0,y0,z0,x0,y0,z0, .. 
  time1 x1,y1,z1,x1,y1,z1, ..
  ..
  time(m-1) x(m-1),y(m-1),z(m-1), ..

  the format of the file admits some specifics:
  1) The first line should contains the tag TRAJECTORIES.
  2) The second line contains the Frequency, which in this case is a dummy value (see below).
  3) The third line contains the points names.
  4) The fourth line contains tags.
  5) The other lines contain time and values. Time is always the first row.

  this operation does as follows:
  1) Checks for the tag TRAJECTORIES in the first line
  2) Sets Frequency to 1 --> This is done since the exporter puts time instead of sample in the file.
  3) Writes the points names
  4) Write the user tags
  5) Writes time and values 
  */
  void Write();

  /** Set the filename for the file to export */
  void SetFileName(const char *file_name){m_File = file_name;};

protected:
	wxString m_File;
	mafVMELandmarkCloud  *m_Cloud;
};
#endif
