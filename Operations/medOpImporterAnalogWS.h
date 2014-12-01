/*=========================================================================

 Program: MAF2
 Module: medOpImporterAnalogWS
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpImporterAnalogWS_H__
#define __mafOpImporterAnalogWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"
#include <vnl\vnl_matrix.h>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class medVMEAnalog;

//----------------------------------------------------------------------------
// medOpImporterAnalogWS :
//----------------------------------------------------------------------------
/** Import analog signals in a medVMEAnalog VME*/
class MAF_EXPORT medOpImporterAnalogWS : public mafOp
{
public:
	medOpImporterAnalogWS(const wxString &label = "Analog Importer");
	~medOpImporterAnalogWS(); 
	mafOp* Copy();


	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node) {return true;};

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
  4) Put the values in a medVMEAnalog item 
  */
  void Read();

  /** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_File = file_name;};


protected:
  medVMEAnalog *m_EmgScalar;
  vnl_matrix<double> m_EmgMatrix;
  wxString m_FileDir;
	wxString m_File;
};
#endif
