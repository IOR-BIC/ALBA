/*=========================================================================

 Program: MAF2Medical
 Module: medHTMLTemplateParser
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/



#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medHTMLTemplateParser.h"


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