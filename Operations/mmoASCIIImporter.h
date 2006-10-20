/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoASCIIImporter.h,v $
  Language:  C++
  Date:      $Date: 2006-10-20 08:33:44 $
  Version:   $Revision: 1.5 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoASCIIImporter_H__
#define __mmoASCIIImporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVMEScalar;

//----------------------------------------------------------------------------
// mmoASCIIImporter :
//----------------------------------------------------------------------------
/** */
class mmoASCIIImporter: public mafOp
{
public:
  mmoASCIIImporter(wxString label = "ASCIIImporter");
  ~mmoASCIIImporter(); 
 
  virtual void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(mmoASCIIImporter, mafOp);

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
  enum SCALAR_DATA_ORDER {
    ROW_DATA = 0,
    COLUMN_DAtA
  };

  std::vector<std::string> m_Files;
  mafString m_FileDir;

  mafVMEScalar *m_ScalarData;
  int m_ScalarOrder;

  bool m_ReadyToExecute;
};
#endif
