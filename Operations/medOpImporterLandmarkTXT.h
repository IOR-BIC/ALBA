/*=========================================================================

 Program: MAF2
 Module: medOpImporterLandmarkTXT
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpImporterLandmarkTXT_H__
#define __medOpImporterLandmarkTXT_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMELandmarkCloud;  
class mafEvent;
//----------------------------------------------------------------------------
// medOpImporterLandmarkTXT :
//----------------------------------------------------------------------------
/** 
class name : medOpImporterLandmarkTXT
this class provides the landmark importing written in the following format:
1) Ignore the first line
2) Ignore the first n columns specified by user
3) the first element of each line is Time, then x y z x1 y1 z1 ecco
*/
class MAF_EXPORT medOpImporterLandmarkTXT : public mafOp
{
public:
  /** object constructor */
	medOpImporterLandmarkTXT(wxString label);
  /** object destructor */
	~medOpImporterLandmarkTXT();
  /** method for clone object */
	mafOp* Copy();
  /** method for catch the dispatched events */
  virtual void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node) {return true;};

	/** Builds operation's interface. */
	void OpRun();

  /** Read the file.
  the format of the file admits some speficics.
  1) Ignore the first line
  2) Ignore the first n columns specified by user
  3) the first element of each line is Time, then x y z x1 y1 z1 ecco
  */
  void Read();

  /** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_File = file_name;};

  /** Set number of column to skip */
  void SetSkipColumn(int column);

protected:
  wxString m_FileDir;
	wxString m_File;
	mafVMELandmarkCloud  *m_VmeCloud;
  int m_Start;
};
#endif
