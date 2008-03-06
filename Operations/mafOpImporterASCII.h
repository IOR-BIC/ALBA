/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpImporterASCII.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpImporterASCII_H__
#define __mafOpImporterASCII_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVMEScalarMatrix;

//----------------------------------------------------------------------------
// mafOpImporterASCII :
//----------------------------------------------------------------------------
/** This type of importer allow to import files containing only numbers 
written by rows or columns into a mafVMEScalarMatrix. Multiple files produce a time-varying mafVMEScalarMatrix.*/
class mafOpImporterASCII: public mafOp
{
public:
  mafOpImporterASCII(const wxString &label = "ASCIIImporter");
  ~mafOpImporterASCII(); 
 
  virtual void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(mafOpImporterASCII, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode* node) {return true;};

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
  /** Fill the file list getting the list of files matching the specified pattern.*/
  void FillFileList(const char *file_pattern);

  enum SCALAR_DATA_ORDER {
    ROW_DATA = 0,
    COLUMN_DAtA
  };

  std::vector<std::string> m_Files; ///< List of files representing the scalar data.
  mafString m_FileDir; ///< Directory where are located the files.

  mafVMEScalarMatrix *m_ScalarData;
  int m_ScalarOrder; ///< Indicates if the scalar arrays are in rows (0) or columns (1).
  bool m_ReadyToExecute; ///< Flag used when the operation runs by passing parameter's list. If it is true in the OpRun it ends without showing the gui.
};
#endif
