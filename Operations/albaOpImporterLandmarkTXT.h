/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterLandmarkTXT
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterLandmarkTXT_H__
#define __albaOpImporterLandmarkTXT_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMELandmarkCloud;  
class albaEvent;
//----------------------------------------------------------------------------
// albaOpImporterLandmarkTXT :
//----------------------------------------------------------------------------
/** 
class name : albaOpImporterLandmarkTXT
this class provides the landmark importing written in the following format:
1) Ignore the first line
2) Ignore the first n columns specified by user
3) the first element of each line is Time, then x y z x1 y1 z1 ecco
*/
class ALBA_EXPORT albaOpImporterLandmarkTXT : public albaOp
{
public:
  /** object constructor */
	albaOpImporterLandmarkTXT(wxString label);
  /** object destructor */
	~albaOpImporterLandmarkTXT();
  /** method for clone object */
	albaOp* Copy();
  /** method for catch the dispatched events */
  virtual void OnEvent(albaEventBase *alba_event);

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

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node) { return true; };

  wxString m_FileDir;
	wxString m_File;
	albaVMELandmarkCloud  *m_VmeCloud;
  int m_Start;
};
#endif
