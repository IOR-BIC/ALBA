/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterLandmarkWS
 Authors: Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterLandmarkWS_H__
#define __albaOpExporterLandmarkWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class albaVMELandmarkCloud;

/** 
class name: albaOpExporterLandmarkWS
/** Export trajectories in a cvs file */
class ALBA_EXPORT albaOpExporterLandmarkWS : public albaOp
{
public:
	albaOpExporterLandmarkWS(const wxString &label = "Trajectories Exporter");
	~albaOpExporterLandmarkWS(); 
	albaOp* Copy();

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

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	wxString m_File;
	albaVMELandmarkCloud  *m_Cloud;
};
#endif
