/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpValidateTree.h,v $
  Language:  C++
  Date:      $Date: 2008-03-14 13:21:29 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpValidateTree_H__
#define __mafOpValidateTree_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;

//----------------------------------------------------------------------------
// mafOpValidateTree :
//----------------------------------------------------------------------------
/** Class used to validate the correctness of the data tree loaded. 
The tree to be validated has to be saved, otherwise some elements could not 
have binary files associated with the VME. This class perform checks on VME id, 
binary data if exists, correctness of TagArray and links.*/
class mafOpValidateTree: public mafOp
{
public:
  mafOpValidateTree(const wxString &label = "Validate Tree");
  ~mafOpValidateTree(); 

  mafTypeMacro(mafOpValidateTree, mafOp);

  /** Return a copy of the operation.*/
  mafOp* Copy();

  /** This operation accept everything as input.*/
  bool Accept(mafNode *node) {return true;};
  
  /** Create the user interface and initialize variables.*/
  void OpRun();

protected: 
  /** iterate to all nodes and check correctness for each one.*/
  bool ValidateTree();

  /** Log errors into the log area according to the error number reported.*/
  void ErrorLog(int error_num, const char *node_name, const char *description = NULL);

  enum ERROR_IDS
  {
    INVALID_NODE = 0,
    LINK_NOT_PRESENT,
    LINK_NULL,
    EXCEPTION_ON_ITERATOR,
    ITEM_NOT_PRESENT,
    URL_EMPTY,
    BINARY_FILE_NOT_PRESENT,
    ARCHIVE_FILE_NOT_PRESENT,
  };

  mafString m_MSFPath;
};
#endif
