/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpImporterEmgWS.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 12:03:55 $
  Version:   $Revision: 1.2 $
  Authors:   Roberto Mucci - Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpImporterEmgWS_H__
#define __medOpImporterEmgWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEGroup;

//----------------------------------------------------------------------------
// medOpImporterEmgWS :
//----------------------------------------------------------------------------
/** */
class medOpImporterEmgWS : public mafOp
{
public:
	medOpImporterEmgWS(wxString label);
	~medOpImporterEmgWS(); 
	mafOp* Copy();


	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node) {return true;};

	/** Builds operation's interface. */
	void OpRun();

  /** Read the file.
  the format of the file admits some specifics.
  1) Ignore the first line
  2) Read the second line containing the Frequency
  2) Ignore 2 lines
  3) the first element of each line is Time, then n scalar values
  4) Put the values in a mafVMEScalar item 
  */
  void Read();

  /** Set the filename for the file to import */
  void SetFileName(const char *file_name) {m_File = file_name;};

protected:
  mafVMEGroup *m_Group;
	mafString m_File;
};
#endif
