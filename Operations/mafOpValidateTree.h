/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpValidateTree.h,v $
  Language:  C++
  Date:      $Date: 2012-04-19 07:42:47 $
  Version:   $Revision: 1.3.2.5 $
  Authors:   Paolo Quadrani , Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpValidateTree_H__
#define __mafOpValidateTree_H__

#include "mafOp.h"
#include <set>
#include <xstring>

using namespace std;
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;

//----------------------------------------------------------------------------
// mafOpValidateTree :
//----------------------------------------------------------------------------
/** 
  Class Name: mafOpValidateTree
Class used to validate the correctness of the data tree loaded. 
The tree to be validated has to be saved, otherwise some elements could not 
have binary files associated with the VME. This class perform checks on VME id, 
binary data if exists, correctness of TagArray and links.

Sample usage:

  mafOpValidateTree *op = new mafOpValidateTree();
  op->SetInput(anyVMETreeNode);
  int returnValue = op->ValidateTree();
  
Check returnValue against VALIDATE_RETURN_VALUES
*/
class MAF_EXPORT mafOpValidateTree: public mafOp
{
public:
  /** constructor */
  mafOpValidateTree(const wxString &label = "Validate Tree");
  /** destructor */
  ~mafOpValidateTree(); 
  
  /** Iterate on all tree nodes and check the consistency for each one: 
  return a VALIDATE_RETURN_VALUES id*/
  int ValidateTree();

  /** 
  If the tree is valid return MAF_OK and fills fileNamesSet with the absolute file names of 
  data belonging to vme's in vme tree. These are all the files that need to be copied in order
  to replicate the entire msf. Otherwise it returns MAF_ERROR and an empty set*/
  int GetMSFTreeABSFileNamesSet(set<string> &fileNamesSet);;
 
  /** values returned by ValidateTree() */
  enum VALIDATE_RETURN_VALUES
  {
    VALIDATE_ERROR = 0,
    VALIDATE_SUCCESS,
    VALIDATE_WARNING,
  };

  /** RTTI macro */
  mafTypeMacro(mafOpValidateTree, mafOp);

  /** Return a copy of the operation.*/
  mafOp* Copy();

  /** This operation accept everything as input.*/
  bool Accept(mafNode *node) {return true;};

  /** Create the user interface and initialize variables.*/
  void OpRun();

protected: 

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

  mafString m_MSFPath;

  // extract
  /** files linked to vme in trees */
  set<string> m_MSFTreeAbsFileNamesSet;

};
#endif
