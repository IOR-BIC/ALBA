/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterAnalogWS
 Authors: Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterAnalogWS_H__
#define __albaOpExporterAnalogWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"
#include "albaVMEAnalog.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class albaVMEAnalog;

/** 
class name: albaOpExporterAnalogWS
/** Export analog signals from a albaVMEAnalog VME */
class ALBA_EXPORT albaOpExporterAnalogWS : public albaOp
{
public:
	albaOpExporterAnalogWS(const wxString &label = "Analog Exporter");
	~albaOpExporterAnalogWS(); 
	albaOp* Copy();

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

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  albaVMEAnalog* m_Analog;
	wxString m_File;
};
#endif
