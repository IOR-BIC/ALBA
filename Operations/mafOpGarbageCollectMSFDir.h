/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpGarbageCollectMSFDir.h,v $
  Language:  C++
  Date:      $Date: 2012-04-19 07:42:46 $
  Version:   $Revision: 1.1.2.3 $
  Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpGarbageCollectMSFDir_H__
#define __mafOpGarbageCollectMSFDir_H__

#include "mafOp.h"
#include <set>
#include <xstring>

using namespace std;
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;

//----------------------------------------------------------------------------
// mafOpGarbageCollectMSFDir :
//----------------------------------------------------------------------------
/** 
  Class Name: mafOpGarbageCollectMSFDir

  Remove garbage files in MSF directory. These are usually left by do/undo stuff
  not synchronized  with storage. It will also clear the undo stack.

  Sample usage:

  mafOpGarbageCollectMSFDir *op = new mafOpGarbageCollectMSFDir();
  op->SetInput(anyVMETreeNode);
  int returnValue = op->GarbageCollect();

  Removed files absolute paths are written to the log area.
  
*/

class MAF_EXPORT mafOpGarbageCollectMSFDir: public mafOp
{
public:
  /** constructor */
  mafOpGarbageCollectMSFDir(const wxString &label = "Garbage Collect MSF Tree");
  
  /** destructor */
  ~mafOpGarbageCollectMSFDir(); 
  
  /**
  Remove useless garbage files in MSF dir and clear the Undo stack: return MAF_OK or MAF_ERROR*/
  int GarbageCollect();

  /** RTTI macro */
  mafTypeMacro(mafOpGarbageCollectMSFDir, mafOp);

  /** Return a copy of the operation.*/
  mafOp* Copy();

  /** This operation accept everything as input.*/
  bool Accept(mafNode *node) {return true;};

  /** Create the user interface and initialize variables.*/
  void OpRun();

protected: 
  
  int GetFilesToRemove(set<string> &filesToRemoveSet);

  /** 
  Return s1 - s2 */
  set<string> SetDifference( set<string> &s1, set<string> &s2 );
 
  set<string> GetMSFDirABSFileNamesSet();

  void PrintSet(set<string> inputSet);

  /** Log errors into the log area according to the error number reported.*/
  void ErrorLog(int error_num, const char *node_name, const char *description = NULL);
 
  mafString GetMSFDirAbsPath(mafNode *anyTreeNode);
 
  mafString GetMSFXMLFileAbsFileName(mafNode *anyTreeNode);
 
  /** files linked to vme in trees */
  set<string> m_MSFTreeBinaryFilesSet;
  set<string> m_MSFDirABSFileNamesSet;
};
#endif
