/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpValidateTree
 Authors: Paolo Quadrani , Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpValidateTree_H__
#define __albaOpValidateTree_H__

#include "albaOp.h"
#include <set>
#include <xstring>

using namespace std;
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;

//----------------------------------------------------------------------------
// albaOpValidateTree :
//----------------------------------------------------------------------------
/** 
  Class Name: albaOpValidateTree
Class used to validate the correctness of the data tree loaded. 
The tree to be validated has to be saved, otherwise some elements could not 
have binary files associated with the VME. This class perform checks on VME id, 
binary data if exists, correctness of TagArray and links.

Sample usage:

  albaOpValidateTree *op = new albaOpValidateTree();
  op->SetInput(anyVMETreeNode);
  int returnValue = op->ValidateTree();
  
Check returnValue against VALIDATE_RETURN_VALUES
*/
class ALBA_EXPORT albaOpValidateTree: public albaOp
{
public:
  /** constructor */
  albaOpValidateTree(const wxString &label = "Validate Tree");
  /** destructor */
  ~albaOpValidateTree(); 
  
  /** Iterate on all tree nodes and check the consistency for each one: 
  return a VALIDATE_RETURN_VALUES id*/
  int ValidateTree();

  /** 
  If the tree is valid return ALBA_OK and fills fileNamesSet with the absolute file names of 
  data belonging to vme's in vme tree. These are all the files that need to be copied in order
  to replicate the entire msf. Otherwise it returns ALBA_ERROR and an empty set*/
  int GetMSFTreeABSFileNamesSet(set<wxString> &fileNamesSet);;
 
  /** values returned by ValidateTree() */
  enum VALIDATE_RETURN_VALUES
  {
    VALIDATE_ERROR = 0,
    VALIDATE_SUCCESS,
    VALIDATE_WARNING,
  };

  /** RTTI macro */
  albaTypeMacro(albaOpValidateTree, albaOp);

  /** Return a copy of the operation.*/
  albaOp* Copy();
	
  /** Create the user interface and initialize variables.*/
  void OpRun();

protected: 

	/** This operation accept everything as input.*/
	bool InternalAccept(albaVME*node) {return true;};

  /** Log errors into the log area according to the error number reported.*/
  void ErrorLog(int error_num, const char *node_name, const char *description = NULL);
 
  /** validate report ids*/
  enum VALIDATE_REPORT_IDS
  {
    INVALID_NODE = 0,
    LINK_NOT_PRESENT,
    LINK_NULL,
    EXCEPTION_ON_ITERATOR,
    ITEM_NOT_PRESENT,
    MAX_ITEM_ID_PATCHED,
    URL_EMPTY,
    BINARY_FILE_NOT_PRESENT,
    ARCHIVE_FILE_NOT_PRESENT,
  };

  albaString m_MSFPath;

  // extract
  /** files linked to vme in trees */
  set<wxString> m_MSFTreeAbsFileNamesSet;

};
#endif
