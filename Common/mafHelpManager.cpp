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

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafDecl.h"
#include "mafHelpManager.h"
#include <wx/tokenzr.h>
#include "mafLogicWithManagers.h"

//buffer size 1024*1024 
#define READ_BUFFER_SIZE 1048576 
#define READ_LINE_SIZE 524288 

//----------------------------------------------------------------------------
mafHelpManager::mafHelpManager()
//----------------------------------------------------------------------------
{
	wxString appName = wxApp::GetInstance()->GetAppName();
		
	m_HelpFileName = (mafGetApplicationDirectory()).c_str();
	m_HelpFileName += "\\Help\\";
	m_HelpFileName += appName;
	m_HelpFileName += "_help.html";

	m_HelpUrl = "file://";
	m_HelpUrl += m_HelpFileName;

	ParseHelpFile();
}

//----------------------------------------------------------------------------
mafHelpManager::~mafHelpManager()
{
	m_HelpEntry.clear();
}

//----------------------------------------------------------------------------
int mafHelpManager::ParseHelpFile()
{
	m_HelpEntry.clear();

	if (ReadInit(m_HelpFileName) == MAF_OK)
	{
		int lineLenght;

		while ((lineLenght = GetLine(m_FilePointer, m_Line)) != 0)
		{
			std::string line = m_Line;
			size_t pos = 0;
			int findPos;
			while ((findPos = line.find("<a class=\"Label\" name=\"", pos)) != std::string::npos)
			{
				pos = findPos + 23;
				size_t tagEnd = line.find("\"", pos);
				std::string tag = line.substr(pos, tagEnd - pos);
				m_HelpEntry.insert(tag.c_str());

				pos = tagEnd + 1;
			}
		}

		ReadFinalize();
	}

	return MAF_OK;
}

//----------------------------------------------------------------------------
bool mafHelpManager::HasHelpEntry(mafString typeName)
{
	return (m_HelpEntry.find(typeName) != m_HelpEntry.end());
}

//----------------------------------------------------------------------------
void mafHelpManager::ShowHelpEntry(mafString typeName)
{
	if (m_HelpEntry.find(typeName) != m_HelpEntry.end())
	{
		wxString appName = wxApp::GetInstance()->GetAppName();

		wxString url = m_HelpUrl + "#";
		url += typeName;

		mafLogicWithManagers::ShowWebSite(url);
	}
}

//----------------------------------------------------------------------------
void mafHelpManager::ShowHelp()
{
	if(wxFileExists(m_HelpFileName.GetCStr()))
		mafLogicWithManagers::ShowWebSite(m_HelpUrl.GetCStr());
}

//----------------------------------------------------------------------------
void mafHelpManager::SetHelpFile(mafString fileName)
{
	m_HelpFileName = fileName; ParseHelpFile();
	m_HelpUrl = "file://";
	m_HelpUrl += m_HelpFileName;
}

//----------------------------------------------------------------------------
int mafHelpManager::GetLine(FILE *fp, char *lineBuffer)
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

		lineBuffer[readedChars] = readValue = m_Buffer[m_BufferPointer];
		readedChars++;
		m_BufferPointer++;
		m_BufferLeft--;
	} while (readValue != '\n');

	lineBuffer[readedChars] = 0;

	//Windows translate CR/LF into CR char we need to count a char more for each newline
	m_BytesReaded += readedChars + 1;

	return readedChars;
}

//----------------------------------------------------------------------------
int mafHelpManager::ReadInit(mafString &fileName)
{
	m_FilePointer = fopen(fileName.GetCStr(), "r");

	if (m_FilePointer == NULL)
	{
		return MAF_ERROR;
	}

	// Calculate file size
	fseek(m_FilePointer, 0L, SEEK_END);
	m_FileSize = ftell(m_FilePointer);
	fseek(m_FilePointer, 0L, SEEK_SET);

	m_Buffer = new char[READ_BUFFER_SIZE];
	m_Line = new char[READ_LINE_SIZE];
	m_BytesReaded = m_BufferLeft = m_BufferPointer = 0;

	return MAF_OK;
}

//----------------------------------------------------------------------------
void mafHelpManager::ReadFinalize()
{
	delete[] m_Buffer;
	delete[] m_Line;
	fclose(m_FilePointer);
}
