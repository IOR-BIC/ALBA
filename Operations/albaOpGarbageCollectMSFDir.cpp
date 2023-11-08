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

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaOpGarbageCollectMSFDir.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"

#include "albaVMEStorage.h"
#include "albaEventIO.h"
#include "albaDataVector.h"
#include "albaVMEItem.h"
#include "albaVMEGenericAbstract.h"
#include "albaVMERoot.h"
#include "albaVMEIterator.h"
#include "albaVMEExternalData.h"
#include "vtkDirectory.h"
#include <algorithm>
#include <iterator> 
#include "albaOpValidateTree.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpGarbageCollectMSFDir);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpGarbageCollectMSFDir::albaOpGarbageCollectMSFDir(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = false;
}
//----------------------------------------------------------------------------
albaOpGarbageCollectMSFDir::~albaOpGarbageCollectMSFDir()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
albaOp* albaOpGarbageCollectMSFDir::Copy()   
//----------------------------------------------------------------------------
{
	return new albaOpGarbageCollectMSFDir(m_Label);
}
//----------------------------------------------------------------------------
void albaOpGarbageCollectMSFDir::OpRun()
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
    int answer = wxMessageBox(msg,_("Confirm"),wxYES_NO|wxCANCEL|wxICON_QUESTION,albaGetFrame()); // ask user if will save msf before closing
    if(answer == wxCANCEL || answer == wxNO)
    {
      albaEventMacro(albaEvent(this,OP_RUN_CANCEL));
      return;
    }
    else if(answer == wxYES)
    {
      // continue
    }
  }
 
  this->GarbageCollect();

  albaEventMacro(albaEvent(this,OP_RUN_OK));
}

int albaOpGarbageCollectMSFDir::GetFilesToRemove( set<wxString> &filesToRemoveSet )
{
  assert(m_Input);
  albaOpValidateTree* validateTree = new albaOpValidateTree();
  validateTree->SetInput(m_Input);

  // the input msf must be valid
  int result = validateTree->ValidateTree();
  
  if (result == albaOpValidateTree::VALIDATE_SUCCESS)
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
    albaLogMessage("MSF Tree is invalid, exiting");
    return ALBA_ERROR;
  }
  
  set<wxString> msfTreeFiles;
  result = validateTree->GetMSFTreeABSFileNamesSet(msfTreeFiles);

  if (result == ALBA_OK)
  {
    set<wxString> msfDirFiles = this->GetMSFDirABSFileNamesSet();

    // add the msf file name:
    wxString msfXMLFileABSFileName = GetMSFXMLFileAbsFileName(m_Input);

    assert(wxFileExists(msfXMLFileABSFileName));
    msfTreeFiles.insert(msfXMLFileABSFileName.ToAscii());

    // add the backup file if present:
    wxString msfXMLBackupFile = msfXMLFileABSFileName.Append(".bak");
    msfXMLBackupFile.Replace("/","\\");

    if (wxFileExists(msfXMLBackupFile.ToAscii()))
    {
      msfTreeFiles.insert(msfXMLBackupFile.ToAscii());
    }

    filesToRemoveSet = SetDifference(msfDirFiles, msfTreeFiles);

    PrintSet(filesToRemoveSet);
  }
  else
  {
    return ALBA_ERROR;
  }

  cppDEL(validateTree);
  return ALBA_OK;
}

void albaOpGarbageCollectMSFDir::PrintSet( set<wxString> inputSet )
{
  set<wxString>::iterator iter;
  iter = inputSet.begin();

  ostringstream stringStream;
  stringStream << endl;

  while( iter != inputSet.end() ) 
  {
    stringStream << *iter << endl << endl;
    ++iter;
  }

  albaLogMessage(stringStream.str().c_str());
}

set<wxString> albaOpGarbageCollectMSFDir::GetMSFDirABSFileNamesSet()
{
  assert(m_Input);
  albaString msfABSPath = GetMSFDirAbsPath(m_Input);
  
  assert(msfABSPath.IsEmpty() == false);
  assert(wxDirExists(msfABSPath.GetCStr()));

  m_MSFDirABSFileNamesSet.clear();

  vtkDirectory *directoryReader = vtkDirectory::New();
  directoryReader->Open(msfABSPath);
  for (int i=0; i < directoryReader->GetNumberOfFiles(); i++) // skipping "." and ".." dir
  {
    albaString localFileName = directoryReader->GetFile(i);

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
    else if (wxFileExists(absFileName.ToAscii()))
    {     
      m_MSFDirABSFileNamesSet.insert(absFileName.ToAscii());
    }
  }
  
  vtkDEL(directoryReader);
  return m_MSFDirABSFileNamesSet;
}

set<wxString> albaOpGarbageCollectMSFDir::SetDifference( set<wxString> &s1, set<wxString> &s2 )
{
  set<wxString> result;
  set_difference(s1.begin(), s1.end(), \
    s2.begin(), s2.end(),
    inserter(result, result.end()));

  return result;
}

albaString albaOpGarbageCollectMSFDir::GetMSFDirAbsPath(albaVME *anyTreeNode)
{
  albaVMERoot* root = albaVMERoot::SafeDownCast(anyTreeNode->GetRoot());
  assert(root);

  albaVMEStorage *storage = root->GetStorage();
  assert(storage);

  albaString msfABSPath = ""; // empty by default
 
  if (storage != NULL)
  {  
    msfABSPath = storage->GetURL();
    msfABSPath.ExtractPathName();
  }

  wxString tmp = msfABSPath.GetCStr();
  tmp.Replace("/","\\");

  return tmp;
}

albaString albaOpGarbageCollectMSFDir::GetMSFXMLFileAbsFileName(albaVME *anyTreeNode)
{
  albaVMERoot* root = albaVMERoot::SafeDownCast(anyTreeNode->GetRoot());
  assert(root);

	albaVMEStorage *storage = root->GetStorage();
  assert(storage);

  wxString msfXMLFileAbsFileName = ""; // empty by default

  if (storage != NULL)
  {  
    msfXMLFileAbsFileName = storage->GetURL();
  }

  msfXMLFileAbsFileName.Replace("/","\\");

  return msfXMLFileAbsFileName;
}

int albaOpGarbageCollectMSFDir::GarbageCollect()
{
  set<wxString> filesToRemove;
  int errorCode = GetFilesToRemove(filesToRemove);
  
  set<wxString>::iterator iter;
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
      return ALBA_ERROR;  
    }
      
    stringStream << "OK" << endl;
    albaLogMessage(stringStream.str().c_str());
    
    ++iter;
  }
 
  // Clear UnDo stack if successful
  albaEventMacro(albaEvent(this, CLEAR_UNDO_STACK));

  return ALBA_OK;
}