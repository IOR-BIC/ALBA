/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpExporterAnalogWS.h,v $
  Language:  C++
  Date:      $Date: 2010-09-08 11:42:39 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Simone Brazzale
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpExporterAnalogWS_H__
#define __medOpExporterAnalogWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "medVMEAnalog.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class medVMEAnalog;

//----------------------------------------------------------------------------
// medOpExporterAnalogWS :
//----------------------------------------------------------------------------
/** Import analog signals in a medVMEAnalog VME*/
class medOpExporterAnalogWS : public mafOp
{
public:
	medOpExporterAnalogWS(const wxString &label = "Analog Exporter");
	~medOpExporterAnalogWS(); 
	mafOp* Copy();


	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node);

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
  3) Reads the signal names
  4) Jump a line
  5) Read time and values 
  */
  void Write();

  /** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_File = file_name;};

protected:
  medVMEAnalog* m_Analog;
	wxString m_File;
};
#endif
