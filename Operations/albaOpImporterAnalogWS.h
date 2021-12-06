/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterAnalogWS
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterAnalogWS_H__
#define __albaOpImporterAnalogWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"
#include <vnl\vnl_matrix.h>

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_VNL_MATRIX(ALBA_EXPORT, double);
#endif

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class albaVMEAnalog;

//----------------------------------------------------------------------------
// albaOpImporterAnalogWS :
//----------------------------------------------------------------------------
/** Import analog signals in a albaVMEAnalog VME*/
class ALBA_EXPORT albaOpImporterAnalogWS : public albaOp
{
public:
	albaOpImporterAnalogWS(const wxString &label = "Analog Importer");
	~albaOpImporterAnalogWS(); 
	albaOp* Copy();

	/** Builds operation's interface. */
	void OpRun();

  /** Read the file.
  the format of the file admits some specifics.
  1) The first line contains the tag ANALOG
  2) The second line contains the Frequency
  3) The third line contains the Signal names
  4) The fourth line contains the units
  5) The first element of each line is the sample, then n scalar values

  the operation does as follow:
  1) Ignore the first line
  2) Read the second line containing the Frequency
  2) Ignore 2 lines
  3) the first element of each line is sample, then n scalar values
  4) Put the values in a albaVMEAnalog item 
  */
  void Read();

  /** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_File = file_name;};
	
protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node) { return true; };

  albaVMEAnalog *m_EmgScalar;
  vnl_matrix<double> m_EmgMatrix;
  wxString m_FileDir;
	wxString m_File;
};
#endif
