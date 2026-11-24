/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaHelpManager
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaHelpManager_H__
#define __albaHelpManager_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaAbsHelpManager.h"
#include <set>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class albaWizardWaitOp;
class albaGUISettingsDialog;

/**
  Class Name: albaHelpManager.
*/
class ALBA_EXPORT albaHelpManager : public albaAbsHelpManager
{
public:

  /** Default constructor */
  albaHelpManager();

  /** Default destructor */
  ~albaHelpManager();
		
	/** Return true if the help has a page for the given type name */
	virtual bool HasHelpEntry(albaString typeName) override;

	/** Open the help page of the given type name */
	virtual void ShowHelpEntry(albaString typeName) override;

	void ShowHelp();
	void SetHelpFile(albaString fileName);;
	
private:
	int ParseHelpFile();
	int GetLine(FILE *fp, char *lineBuffer);
	int ReadInit(albaString &fileName);
	void ReadFinalize();

	albaString m_HelpFileName;
	albaString m_HelpUrl;
	std::set<albaString> m_HelpEntry;

	FILE * m_FilePointer;
	char *m_Buffer;
	int m_BufferLeft;
	int m_BufferPointer;
	char *m_Line;
	long m_FileSize;
	long m_BytesReaded;
	
  friend class albaLogicWithManagers; // class albaHelpManager can now access data directly
  //friend class albaHelpManagerTest; // for testing 
};
#endif
