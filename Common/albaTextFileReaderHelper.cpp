
/*=========================================================================

Program: ALBA
Module: albaTextFileReaderHelper.cpp
Authors: Gianluigi Crimi	

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
#include "albaTextFileReaderHelper.h"
#include "albaProgressBarHelper.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


//buffer size 1024*1024 
#define READ_BUFFER_SIZE 1048576 


//----------------------------------------------------------------------------
albaTextFileReaderHelper::albaTextFileReaderHelper()
{
	m_ProgressHelper = 0;
	m_Buffer = NULL;
	m_FilePointer = NULL;
}

//----------------------------------------------------------------------------
albaTextFileReaderHelper::~albaTextFileReaderHelper()
{
	ReadFinalize();
}


//----------------------------------------------------------------------------
int albaTextFileReaderHelper::GetLine(bool toUpper)
{
	char readValue;
	int readedChars = 0;

	do
	{
		if (m_BufferLeft == 0)
		{
			m_BufferLeft = fread(m_Buffer, sizeof(char), READ_BUFFER_SIZE, m_FilePointer);
			m_BufferPointer = 0;
			//Breaks if EOF is reached
			if (m_BufferLeft == 0)
				break;
		}

		if(toUpper)
			m_Line[readedChars] = readValue = (toupper(m_Buffer[m_BufferPointer]));
		else
			m_Line[readedChars] = readValue = m_Buffer[m_BufferPointer];
	
		readedChars++;
		m_BufferPointer++;
		m_BufferLeft--;
	} while (readValue != '\n');

	m_Line[readedChars] = 0;

	//Windows translate CR/LF into CR char we need to count a char more for each newline
	m_BytesReaded += readedChars + 1;
	m_CurrentLine++;

	if(m_ProgressHelper)
		m_ProgressHelper->UpdateProgressBar(((double)m_BytesReaded) * 100 / m_FileSize);

	return readedChars;
}
//----------------------------------------------------------------------------
int albaTextFileReaderHelper::ReplaceInString(char *str, char from, char to)
{
	int count = 0;

	for (int i = 0; str[i] != '\0'; i++)
	{
		if (str[i] == from)
		{
			str[i] = to;
			count++;
		}
	}

	return count;
}
//----------------------------------------------------------------------------
int albaTextFileReaderHelper::ReadInit(albaString &fileName, int textMode, int showProgressBar, albaString progressBarText, albaObserver *listener)
{
	m_FilePointer = fopen(fileName.GetCStr(), "r");

	m_CurrentLine = 0;

	if (m_FilePointer == NULL)
	{
		if (textMode == false)
			albaMessage(_("Error parsing input files! File not found."), _("Error"));
		else
			printf("Error parsing input files! File not found.\n");
		return ALBA_ERROR;
	}

	if (showProgressBar)
	{
		m_ProgressHelper = new albaProgressBarHelper(listener);
		m_ProgressHelper->SetTextMode(textMode);
		m_ProgressHelper->InitProgressBar(progressBarText.GetCStr());
	}

	// Calculate file size
	fseek(m_FilePointer, 0L, SEEK_END);
	m_FileSize = ftell(m_FilePointer);
	fseek(m_FilePointer, 0L, SEEK_SET);

	m_Buffer = new char[READ_BUFFER_SIZE];
	m_BytesReaded = m_BufferLeft = m_BufferPointer = 0;

	return ALBA_OK;
}
//----------------------------------------------------------------------------
void albaTextFileReaderHelper::ReadFinalize()
{
	cppDEL(m_ProgressHelper);
	if (m_Buffer)
	{
		delete[] m_Buffer;
		m_Buffer = NULL;
	}
	if (m_FilePointer)
	{
		fclose(m_FilePointer);
		m_FilePointer = NULL;
	}
}
