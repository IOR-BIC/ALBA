/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoEMGImporterWS.h,v $
  Language:  C++
  Date:      $Date: 2007-07-06 13:44:31 $
  Version:   $Revision: 1.4 $
  Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoEMGImporterWS_H__
#define __mmoEMGImporterWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include <vnl\vnl_matrix.h>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafEvent;
class medVMEEmg;

//----------------------------------------------------------------------------
// mmoEMGImporterWS :
//----------------------------------------------------------------------------
/** */
class mmoEMGImporterWS : public mafOp
{
public:
	mmoEMGImporterWS(wxString label);
	~mmoEMGImporterWS(); 
	mafOp* Copy();
  virtual void OnEvent(mafEventBase *maf_event);

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
  4) Put the values in a medVMEEmg item 
  */
  void Read();

  /** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_File = file_name;};


protected:
  medVMEEmg *m_EmgScalar;
  vnl_matrix<double> m_EmgMatrix;
  wxString m_FileDir;
	wxString m_File;
};
#endif
