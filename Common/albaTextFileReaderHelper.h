/*=========================================================================

Program: ALBA
Module: albaTextFileReaderHelper.h
Authors: Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaTextFileReaderHelper_H__
#define __albaTextFileReaderHelper_H__

#include "albaString.h"

class albaProgressBarHelper;
class albaObserver;

class ALBA_EXPORT albaTextFileReaderHelper 
{
public:
	albaTextFileReaderHelper();
	~albaTextFileReaderHelper();

	int GetLine(bool toUpper=false);
	int ReplaceInString(char *str, char from, char to);
	int ReadInit(albaString &fileName, int textMode, int showProgressBar, albaString progressBarText, albaObserver  *listener);

	void ReadFinalize();

protected:

	FILE * m_FilePointer;
	char *m_Buffer;
	int m_BufferLeft;
	int m_BufferPointer;
	char m_Line[512];
	int m_CurrentLine;
	long m_FileSize;
	long m_BytesReaded;

	albaProgressBarHelper *m_ProgressHelper;


};

#endif