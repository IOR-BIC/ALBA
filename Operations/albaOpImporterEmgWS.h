/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)Medical
 Module: albaOpImporterEmgWS
 Authors: Roberto Mucci - Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterEmgWS_H__
#define __albaOpImporterEmgWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMEGroup;

/** 
class name: albaOpImporterEmgWS
importer for elettromiography signals
*/
class albaOpImporterEmgWS : public albaOp
{
public:
  /** constructor */
	albaOpImporterEmgWS(wxString label);
  /** destructor */
	~albaOpImporterEmgWS(); 
  /** clone the current object */
	albaOp* Copy();


	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME* node) {return true;};

	/** Builds operation's interface. */
	void OpRun();

  /** Read the file.
  the format of the file admits some specifics.
  1) Ignore the first line
  2) Read the second line containing the Frequency
  2) Ignore 2 lines
  3) the first element of each line is Time, then n scalar values
  4) Put the values in a albaVMEScalar item 
  */
  void Read();

  /** Set the filename for the file to import */
  void SetFileName(const char *file_name) {m_File = file_name;};

protected:
  albaVMEGroup *m_Group;
	albaString m_File;
};
#endif
