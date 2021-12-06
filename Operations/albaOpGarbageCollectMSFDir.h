/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpGarbageCollectMSFDir
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpGarbageCollectMSFDir_H__
#define __albaOpGarbageCollectMSFDir_H__

#include "albaOp.h"
#include <set>
#include <xstring>

using namespace std;
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;

//----------------------------------------------------------------------------
// albaOpGarbageCollectMSFDir :
//----------------------------------------------------------------------------
/** 
  Class Name: albaOpGarbageCollectMSFDir

  Remove garbage files in MSF directory. These are usually left by do/undo stuff
  not synchronized  with storage. It will also clear the undo stack.

  Sample usage:

  albaOpGarbageCollectMSFDir *op = new albaOpGarbageCollectMSFDir();
  op->SetInput(anyVMETreeNode);
  int returnValue = op->GarbageCollect();

  Removed files absolute paths are written to the log area.
  
*/

class ALBA_EXPORT albaOpGarbageCollectMSFDir: public albaOp
{
public:
  /** constructor */
  albaOpGarbageCollectMSFDir(const wxString &label = "Garbage Collect MSF Tree");
  
  /** destructor */
  ~albaOpGarbageCollectMSFDir(); 
  
  /**
  Remove useless garbage files in MSF dir and clear the Undo stack: return ALBA_OK or ALBA_ERROR*/
  int GarbageCollect();

  /** RTTI macro */
  albaTypeMacro(albaOpGarbageCollectMSFDir, albaOp);

  /** Return a copy of the operation.*/
  albaOp* Copy();

  /** Create the user interface and initialize variables.*/
  void OpRun();

protected: 
  
	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node) { return true; };

  int GetFilesToRemove(set<string> &filesToRemoveSet);

  /** 
  Return s1 - s2 */
  set<string> SetDifference( set<string> &s1, set<string> &s2 );
 
  set<string> GetMSFDirABSFileNamesSet();

  void PrintSet(set<string> inputSet);

  /** Log errors into the log area according to the error number reported.*/
  void ErrorLog(int error_num, const char *node_name, const char *description = NULL);
 
  albaString GetMSFDirAbsPath(albaVME *anyTreeNode);
 
  albaString GetMSFXMLFileAbsFileName(albaVME *anyTreeNode);
 
  /** files linked to vme in trees */
  set<string> m_MSFTreeBinaryFilesSet;
  set<string> m_MSFDirABSFileNamesSet;
};
#endif
