/*=========================================================================

 Program: MAF2
 Module: mafOpGarbageCollectMSFDir
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafOpGarbageCollectMSFDir.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "mafStorage.h"
#include "mafEventIO.h"
#include "mafDataVector.h"
#include "mafVMEItem.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMERoot.h"
#include "mafVMEIterator.h"
#include "mafVMEExternalData.h"
#include "vtkDirectory.h"
#include <algorithm>
#include <iterator> 
#include "mafOpValidateTree.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpGarbageCollectMSFDir);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpGarbageCollectMSFDir::mafOpGarbageCollectMSFDir(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = false;
}
//----------------------------------------------------------------------------
mafOpGarbageCollectMSFDir::~mafOpGarbageCollectMSFDir()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpGarbageCollectMSFDir::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpGarbageCollectMSFDir(m_Label);
}
//----------------------------------------------------------------------------
void mafOpGarbageCollectMSFDir::OpRun()
//----------------------------------------------------------------------------
{
  wxString msg = "This operation will remove garbage data from your msf dir and \
clear the Undo stack. \
 \
Before continuing please make sure you have a working msf backup. \
Removed files will be written to Log Area. \
Proceed?";

  if (m_TestMode == false)
  {
    int answer = wxMessageBox(msg,_("Confirm"),wxYES_NO|wxCANCEL|wxICON_QUESTION,mafGetFrame()); // ask user if will save msf before closing
    if(answer == wxCANCEL || answer == wxNO)
    {
      mafEventMacro(mafEvent(this,OP_RUN_CANCEL));
      return;
    }
    else if(answer == wxYES)
    {
      // continue
    }
  }
 
  this->GarbageCollect();

  mafEventMacro(mafEvent(this,OP_RUN_OK));
}

int mafOpGarbageCollectMSFDir::GetFilesToRemove( set<string> &filesToRemoveSet )
{
  assert(m_Input);
  mafOpValidateTree* validateTree = new mafOpValidateTree();
  validateTree->SetInput(m_Input);

  // the input msf must be valid
  int result = validateTree->ValidateTree();
  
  if (result == mafOpValidateTree::VALIDATE_SUCCESS)
  {
    // continue
  } 
  else
  {
    wxString msg = "MSF Tree is invalid, exiting";

    if (!m_TestMode)
    {
      wxMessageBox(msg);
    }
    mafLogMessage("MSF Tree is invalid, exiting");
    return MAF_ERROR;
  }
  
  set<string> msfTreeFiles;
  result = validateTree->GetMSFTreeABSFileNamesSet(msfTreeFiles);

  if (result == MAF_OK)
  {
    set<string> msfDirFiles = this->GetMSFDirABSFileNamesSet();

    // add the msf file name:
    wxString msfXMLFileABSFileName = GetMSFXMLFileAbsFileName(m_Input);

    assert(wxFileExists(msfXMLFileABSFileName));
    msfTreeFiles.insert(msfXMLFileABSFileName.c_str());

    // add the backup file if present:
    wxString msfXMLBackupFile = msfXMLFileABSFileName.Append(".bak");
    msfXMLBackupFile.Replace("/","\\");

    if (wxFileExists(msfXMLBackupFile.c_str()))
    {
      msfTreeFiles.insert(msfXMLBackupFile.c_str());
    }

    filesToRemoveSet = SetDifference(msfDirFiles, msfTreeFiles);

    PrintSet(filesToRemoveSet);
  }
  else
  {
    return MAF_ERROR;
  }

  cppDEL(validateTree);
  return MAF_OK;
}

void mafOpGarbageCollectMSFDir::PrintSet( set<string> inputSet )
{
  set<string>::iterator iter;
  iter = inputSet.begin();

  ostringstream stringStream;
  stringStream << endl;

  while( iter != inputSet.end() ) 
  {
    stringStream << *iter << endl << endl;
    ++iter;
  }

  mafLogMessage(stringStream.str().c_str());
}

set<string> mafOpGarbageCollectMSFDir::GetMSFDirABSFileNamesSet()
{
  assert(m_Input);
  mafString msfABSPath = GetMSFDirAbsPath(m_Input);
  
  assert(msfABSPath.IsEmpty() == false);
  assert(wxDirExists(msfABSPath));

  m_MSFDirABSFileNamesSet.clear();

  vtkDirectory *directoryReader = vtkDirectory::New();
  directoryReader->Open(msfABSPath);
  for (int i=0; i < directoryReader->GetNumberOfFiles(); i++) // skipping "." and ".." dir
  {
    mafString localFileName = directoryReader->GetFile(i);

    wxString absFileName;
    absFileName << msfABSPath;
    absFileName << "/";
    absFileName << localFileName;

    absFileName.Replace("/","\\");

    if ((strcmp(localFileName,".") == 0) || \
      (strcmp(localFileName,"..") == 0)) 
    {
      continue;
    }
    else if (wxFileExists(absFileName.c_str()))
    {     
      m_MSFDirABSFileNamesSet.insert(absFileName.c_str());
    }
  }
  
  vtkDEL(directoryReader);
  return m_MSFDirABSFileNamesSet;
}

set<string> mafOpGarbageCollectMSFDir::SetDifference( set<string> &s1, set<string> &s2 )
{
  set<string> result;
  set_difference(s1.begin(), s1.end(), \
    s2.begin(), s2.end(),
    inserter(result, result.end()));

  return result;
}

mafString mafOpGarbageCollectMSFDir::GetMSFDirAbsPath(mafVME *anyTreeNode)
{
  mafVMERoot* root = mafVMERoot::SafeDownCast(anyTreeNode->GetRoot());
  assert(root);

  mafStorage *storage = root->GetStorage();
  assert(storage);

  mafString msfABSPath = ""; // empty by default
 
  if (storage != NULL)
  {  
    msfABSPath = storage->GetURL();
    msfABSPath.ExtractPathName();
  }

  wxString tmp = msfABSPath.GetCStr();
  tmp.Replace("/","\\");

  return tmp;
}

mafString mafOpGarbageCollectMSFDir::GetMSFXMLFileAbsFileName(mafVME *anyTreeNode)
{
  mafVMERoot* root = mafVMERoot::SafeDownCast(anyTreeNode->GetRoot());
  assert(root);

  mafStorage *storage = root->GetStorage();
  assert(storage);

  wxString msfXMLFileAbsFileName = ""; // empty by default

  if (storage != NULL)
  {  
    msfXMLFileAbsFileName = storage->GetURL();
  }

  msfXMLFileAbsFileName.Replace("/","\\");

  return msfXMLFileAbsFileName;
}

int mafOpGarbageCollectMSFDir::GarbageCollect()
{
  set<string> filesToRemove;
  int errorCode = GetFilesToRemove(filesToRemove);
  
  set<string>::iterator iter;
  iter = filesToRemove.begin();

  ostringstream stringStream;
  stringStream << endl;

  while( iter != filesToRemove.end() ) 
  {
    string fileToRemove = *iter;
    stringStream << "Deleting: " << fileToRemove << endl;
    
    bool result = wxRemoveFile(wxString(fileToRemove.c_str()));
    
    if (result == false)
    {
      return MAF_ERROR;  
    }
      
    stringStream << "OK" << endl;
    mafLogMessage(stringStream.str().c_str());
    
    ++iter;
  }
 
  // Clear UnDo stack if successful
  mafEventMacro(mafEvent(this, CLEAR_UNDO_STACK));

  return MAF_OK;
}