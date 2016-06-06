/*=========================================================================

 Program: MAF2
 Module: mafHelpManager
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafHelpManager_H__
#define __mafHelpManager_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include <set>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafWizardWaitOp;
class mafGUISettingsDialog;

/**
  Class Name: mafHelpManager.
*/
class MAF_EXPORT mafHelpManager
{
public:

  /** Default constructor */
  mafHelpManager();

  /** Default destructor */
  ~mafHelpManager();
		
	bool HasHelpEntry(mafString typeName);
	void ShowHelpEntry(mafString typeName);
	void ShowHelp();
	void SetHelpFile(mafString fileName) { m_HelpFileName = fileName; ParseHelpFile(); };
	
private:
	int ParseHelpFile();
	int GetLine(FILE *fp, char *lineBuffer);
	int ReadInit(mafString &fileName);
	void ReadFinalize();

	mafString m_HelpFileName;
	std::set<mafString> m_HelpEntry;

	FILE * m_FilePointer;
	char *m_Buffer;
	int m_BufferLeft;
	int m_BufferPointer;
	char *m_Line;
	long m_FileSize;
	long m_BytesReaded;
	
  friend class mafLogicWithManagers; // class mafHelpManager can now access data directly
  //friend class mafHelpManagerTest; // for testing 
};
#endif
