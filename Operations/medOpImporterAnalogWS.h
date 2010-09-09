/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpImporterAnalogWS.h,v $
  Language:  C++
  Date:      $Date: 2010-09-09 15:19:21 $
  Version:   $Revision: 1.1.2.3 $
  Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpImporterAnalogWS_H__
#define __mafOpImporterAnalogWS_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
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
class medOpImporterAnalogWS : public mafOp
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
