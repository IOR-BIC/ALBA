/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardBlockFileExistCheck
 Authors: Simone Bnà
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaWizardBlockFileExistCheck_H__
#define __albaWizardBlockFileExistCheck_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaWizardBlock.h"
#include <vector>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: albaWizardTypeFileExistCheck.
  Class for create a file exist check block
*/
class ALBA_EXPORT albaWizardBlockFileExistCheck : public albaWizardBlock
{
public:

  /** Default constructor */
  albaWizardBlockFileExistCheck(const char *name);

  /** Default destructor */
  ~albaWizardBlockFileExistCheck();
    
  /** Set The title of the window showed when the type check was not  */
  void SetWindowTitle(const char *Title);

  /** Get The title of the window showed when the type check was not  */
  wxString GetWindowTitle(){return m_Title;};

  /** Set the path for VME selection */
  void VmeSelect(const char *path);

  /** Get the path for VME selection */
  wxString GetVmeSelect(){return m_VmeSelect;};

  /** Set The title of the selection window */
  void SetDescription(const char *description);

  /** Get The title of the selection window */
  wxString GetDescription(){return m_Description;};

  /** Add a new accepted type.
      Uses the string of VME type.
      */
  void AddAcceptedType(const char *label);

  /** Returns the list of accepted types. */
  std::vector < wxString > *GetAcceptedTypeList(){return &m_AcceptedVmes;};

  /** Set name of the Block called after wrong check.
      By default is set to "END" and the wizard will closed on wrong check. */     
  void SetWrongTypeNextBlock(const char *block);

  /** Get name of the Block called after wrong check. */     
  wxString GetWrongTypeNextBlock(){return m_WrongTypeNextBlock;};

  /** Return the name of the Block witch will be executed after this */
  wxString GetNextBlock();

  /** Enable/disable the visualization of error message box*/
  void EnableErrorMessage(bool enabled) {m_ErrorMessageEnabled=enabled;};
  
  /** Return true if error message show is enabled*/
  bool IsErrorMessageEnabled() {return m_ErrorMessageEnabled;};

protected:
  /** Starts the execution of the block */
  virtual void ExcutionBegin();

private:

  wxString m_Title;
  wxString m_Description;

  wxString m_VmeSelect;

  int m_ErrorMessageEnabled;
  
  std::vector <wxString> m_AcceptedVmes;
  int m_TestPassed;
  
  wxString m_WrongTypeNextBlock;
};
#endif