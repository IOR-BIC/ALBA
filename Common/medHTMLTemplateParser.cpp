/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medHTMLTemplateParser.cpp,v $
Language:  C++
Date:      $Date: 2012-04-05 08:19:12 $
Version:   $Revision: 1.1.2.6 $
Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) 2002/2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/



#include "medDefines.h" 
#include "medHTMLTemplateParser.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------




medHTMLTemplateParser::medHTMLTemplateParser():
medHTMLTemplateParserBlock(MED_HTML_TEMPLATE_MAIN,wxString("Main Block"))
{
  m_Template="";
  m_Output="";
}

//----------------------------------------------------------------------------
medHTMLTemplateParser::~medHTMLTemplateParser()
//----------------------------------------------------------------------------
{
  
}

//----------------------------------------------------------------------------
void medHTMLTemplateParser::SetTemplateFromFile( wxString filename )
//----------------------------------------------------------------------------
{
  FILE *templateFile;
  int stringSize;

  char *fileString;

  templateFile=fopen(filename.c_str(),"r");
  if (templateFile!=NULL)
  {

    //we need to read the file char by char to get the count of char
    //the filesize is not correct because windows stores two char
    //for each newline and the char count results wrong.
    stringSize=0;
    char memArea[2];
    while (fread(memArea,sizeof(char),1,templateFile)>0)
      stringSize++;
  
  
    fileString=new char[stringSize+1];

    fseek(templateFile, 0L, SEEK_SET);

    fread(fileString,sizeof(char),stringSize,templateFile);

    //Adding a string terminator char at the end of the readed string
    fileString[stringSize]='\0';
  
    m_Template=fileString;
    fclose(templateFile);
    //memory releasing
    delete fileString;
  }
  else
  {
    mafLogMessage("medHTMLTemplateParser: Cannot open File: %s",filename.c_str());
  }
  
}

//----------------------------------------------------------------------------
void medHTMLTemplateParser::SetTemplateFromString( wxString templateString )
//----------------------------------------------------------------------------
{
  //simple assignement
  m_Template=templateString;
}


wxString medHTMLTemplateParser::GetOutputString()
{
  //You must call this function after parsing
  return m_Output;
}

void medHTMLTemplateParser::WriteOutputFile(wxString filename)
{
  FILE *outputFile;
  //You must call this function after parsing
  outputFile=fopen(filename.c_str(),"w");
  if (outputFile)
  {
    fwrite(m_Output.c_str(),sizeof(char),m_Output.size(),outputFile);
    fclose(outputFile);
  }
}


//----------------------------------------------------------------------------
void medHTMLTemplateParser::Parse()
//----------------------------------------------------------------------------
{

  int parsingPos=0;
  // Checks the consistence of the parsing tree
  if (ConsistenceCheck() && m_Template != "")
  {
    //pre-parsing (generates substitutions tables inside the blocks)
    PreParse(&m_Template,parsingPos);
    //use pre-parsing info to generate the output
    GenerateOutput(&m_Output);
  }
}