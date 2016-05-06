/*=========================================================================

 Program: MAF2
 Module: mafOpExporterAnalogWS
 Authors: Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpExporterAnalogWS_H__
#define __mafOpExporterAnalogWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"
#include "mafVMEAnalog.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafVMEAnalog;

/** 
class name: mafOpExporterAnalogWS
/** Export analog signals from a mafVMEAnalog VME */
class MAF_EXPORT mafOpExporterAnalogWS : public mafOp
{
public:
	mafOpExporterAnalogWS(const wxString &label = "Analog Exporter");
	~mafOpExporterAnalogWS(); 
	mafOp* Copy();


	/** Return true for the acceptable vme type. */
	bool Accept(mafVME* node);

	/** Builds operation's interface. */
	void OpRun();

  /** Read the file.
  File format:

  ANALOG
  Frequency,Hz
  Sample,Signal0,Signal1, .. ,Signal(n-1)
  Units
  time0 y00 y01 .. y0n-1 
  time1 y10 y11 .. y1n-1
  ..
  time(m-1) y(m-1)0 y(m-1)1 .. ym(n-1)

  the format of the file admits some specifics:
  1) The first line should contains the tag ANALOG.
  2) The second line contains the Frequency.
  3) The third line contains the signals names with a tag before.
  4) The fourth line contains units.
  5) The other lines contain sample and values. Sample is always the first row.

  this operation does as follows:
  1) Checks for the tag ANALOG in the first line
  2) Sets Frequency to 1 --> This is done since the exporter puts time instead of sample in the file.
  3) Writes the signal names
  4) Jump a line
  5) Writes time and values 
  */
  void Write();

  /** Set the filename for the file to export */
  void SetFileName(const char *file_name){m_File = file_name;};

protected:
  mafVMEAnalog* m_Analog;
	wxString m_File;
};
#endif
