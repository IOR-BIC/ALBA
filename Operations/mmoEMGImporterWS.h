/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)Medical
 Module: mmoEMGImporterWS
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mmoEMGImporterWS_H__
#define __mmoEMGImporterWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include <vnl\vnl_matrix.h>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class medVMEEmg;

//----------------------------------------------------------------------------
// mmoEMGImporterWS :
//----------------------------------------------------------------------------
/** */
class mmoEMGImporterWS : public albaOp
{
public:
	mmoEMGImporterWS(const wxString &label = "EMGImporterWS");
	~mmoEMGImporterWS(); 
	albaOp* Copy();


	/** Return true for the acceptable vme type. */
	bool Accept(albaNode* node) {return true;};

	/** Builds operation's interface. */
	void OpRun();

  /** Read the file.
  the format of the file admits some specifics.
  1) Ignore the first line
  2) Read the second line containing the Frequency
  2) Ignore 2 lines
  3) the first element of each line is Time, then n scalar values
  4) Put the values in a medVMEEmg item 
  */
  void Read();

  /** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_File = file_name;};


protected:
  medVMEEmg *m_EmgScalar;
  wxString m_FileDir;
	wxString m_File;
};
#endif
