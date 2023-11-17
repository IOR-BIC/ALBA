/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterASCII
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterASCII_H__
#define __albaOpImporterASCII_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVMEScalarMatrix;

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,std::string);
#endif

//----------------------------------------------------------------------------
// albaOpImporterASCII :
//----------------------------------------------------------------------------
/** This type of importer allow to import files containing only numbers 
written by rows or columns into a albaVMEScalarMatrix. Multiple files produce a time-varying albaVMEScalarMatrix.*/
class ALBA_EXPORT albaOpImporterASCII: public albaOp
{
public:
  albaOpImporterASCII(const wxString &label = "ASCIIImporter");
  ~albaOpImporterASCII(); 
 
  virtual void OnEvent(albaEventBase *alba_event);

  albaTypeMacro(albaOpImporterASCII, albaOp);

  albaOp* Copy();

	/** Builds operation's interface. */
  void OpRun();

  /** Initialize operation's variables according to the parameter's list. */
  virtual void SetParameters(void *param);

	/** Import ASCII data. */
  int ImportASCII();

  /** Add ASCII filename to the files list to be imported. 
  This is used when the operation is executed not using user interface. */
  void AddFileName(std::string &file);

  /** Set the ASCII filename to be imported. 
  This is used when the operation is executed not using user interface. */
  void SetFileName(std::string &file);

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node) { return true; };

  /** Fill the file list getting the list of files matching the specified pattern.*/
  void FillFileList(const char *file_pattern);

  enum SCALAR_DATA_ORDER {
    ROW_DATA = 0,
    COLUMN_DAtA
  };

  std::vector<wxString> m_Files; ///< List of files representing the scalar data.
  albaString m_FileDir; ///< Directory where are located the files.

  albaVMEScalarMatrix *m_ScalarData;
  int m_ScalarOrder; ///< Indicates if the scalar arrays are in rows (0) or columns (1).
  bool m_ReadyToExecute; ///< Flag used when the operation runs by passing parameter's list. If it is true in the OpRun it ends without showing the gui.
};
#endif
