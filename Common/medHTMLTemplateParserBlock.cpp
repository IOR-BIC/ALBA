/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medHTMLTemplateParserBlock.cpp,v $
Language:  C++
Date:      $Date: 2012-02-07 16:45:07 $
Version:   $Revision: 1.1.2.3 $
Authors:   Matteo Giacomoni
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
#include "medHTMLTemplateParserBlock.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
medHTMLTemplateParserBlock::medHTMLTemplateParserBlock(int blockType, wxString name)
//----------------------------------------------------------------------------
{
  if (blockType != MED_HTML_TEMPLATE_MAIN && blockType != MED_HTML_TEMPLATE_LOOP &&
      blockType != MED_HTML_TEMPLATE_IF)
  {
    mafLogMessage("medHTMLTemplateParserBlock: Invalid Block Type");
    return;
  }


  m_BlockName=name;
  m_BlockType=blockType;
  m_IfChars=0;

  m_DoubleFormat="%.3f";

  m_CurrentLoop=0;
  m_LoopsNumber=-1;  
}


//----------------------------------------------------------------------------
medHTMLTemplateParserBlock::~medHTMLTemplateParserBlock()
//----------------------------------------------------------------------------
{
  m_SubstitutionTable.clear();
  
  m_Variables.clear();

  for (int i=0;i<m_VariablesArray.size();i++)
    m_VariablesArray[i].clear();
  m_VariablesArray.clear();

  for (int i=0;i<m_SubBlocks.size();i++)
    delete m_SubBlocks[i];
  m_SubBlocks.clear();
  
  for (int i=0;i<m_SubBlocksArray.size();i++)
  {
    for (int j=0;j<m_SubBlocksArray[i].size();j++)
      delete m_SubBlocksArray[i][j];
    m_SubBlocksArray[i].clear();
  }
  m_SubBlocksArray.clear();
    
}


//----------------------------------------------------------------------------
void medHTMLTemplateParserBlock::AddVar( wxString name, double varValue )
//----------------------------------------------------------------------------
{
  wxString variable;
  variable.Format(m_DoubleFormat,varValue);
  AddVar(name,variable);
}

//----------------------------------------------------------------------------
void medHTMLTemplateParserBlock::AddVar( wxString name, int varValue )
//----------------------------------------------------------------------------
{
  wxString variable;
  variable.Format("%d",varValue);
  AddVar(name,variable);
}

//----------------------------------------------------------------------------
void medHTMLTemplateParserBlock::AddVar( wxString name, wxString varValue )
//----------------------------------------------------------------------------
{
  if (m_SubBlocks.size()>0 || m_SubBlocksArray.size()>0)
  {
    mafLogMessage("HTML Template ERROR: You must set all Variables before adding blocks",name.ToAscii());
  }
  else if (SubstitutionPos(name)>0)
  {
    mafLogMessage("HTML Template ERROR: Variable: \"%s\" already exists",name.ToAscii());
  }
  else
  {
    HTMLTemplateSubstitution blockVar;
    blockVar.Name=name;
    blockVar.Pos=m_Variables.size();
    blockVar.Type=MED_HTML_TEMPLATE_VARIABLE;
    m_SubstitutionTable.push_back(blockVar);
    m_Variables.push_back(varValue);
  }
}

//----------------------------------------------------------------------------
void medHTMLTemplateParserBlock::PushVar( wxString name, double varValue )
//----------------------------------------------------------------------------
{
  wxString variable;
  variable.Format(m_DoubleFormat,varValue);
  PushVar(name,variable);
}

//----------------------------------------------------------------------------
void medHTMLTemplateParserBlock::PushVar( wxString name, int varValue )
//----------------------------------------------------------------------------
{
  wxString variable;
  variable.Format("%d",varValue);
  PushVar(name,variable);
}

//----------------------------------------------------------------------------
void medHTMLTemplateParserBlock::PushVar( wxString name, wxString varValue )
//----------------------------------------------------------------------------
{
  int varPos=SubstitutionPos(name);

  //if the vars does not exists i need to create the array entry
  if (varPos<0)
  {
    std::vector<wxString> newArray;
    varPos=m_VariablesArray.size();
    m_VariablesArray.push_back(newArray);
    
    HTMLTemplateSubstitution blockVar;
    blockVar.Name=name;
    blockVar.Pos=m_Variables.size();
    blockVar.Type=MED_HTML_TEMPLATE_VARIABLE_ARRAY;
    m_SubstitutionTable.push_back(blockVar);
  }

  if (m_SubstitutionTable[varPos].Type!=MED_HTML_TEMPLATE_VARIABLE_ARRAY)
  {
    mafLogMessage("HTML Template ERROR: Array: \"%s\" has incompatible type",name.ToAscii());
  }
  else
    m_VariablesArray[varPos].push_back(varValue);
}

//----------------------------------------------------------------------------
int medHTMLTemplateParserBlock::SubstitutionPos( wxString name )
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_SubstitutionTable.size();i++)
  {
    if (name.compare(m_SubstitutionTable[i].Name)==0)
      return i;
  }
  return -1;
}


//----------------------------------------------------------------------------
void medHTMLTemplateParserBlock::AddBlock( wxString name, int blockType )
//----------------------------------------------------------------------------
{
  if (SubstitutionPos(name)>0)
  {
    mafLogMessage("HTML Template ERROR: Block: \"%s\" already exists",name.ToAscii());
  }
  else
  {
    medHTMLTemplateParserBlock *newBlock;
    newBlock=new medHTMLTemplateParserBlock(blockType, name);
    newBlock->InheritVars(this);
  }
}

//----------------------------------------------------------------------------
void medHTMLTemplateParserBlock::Parse( wxString *inputTemplate,wxString *outputHTML)
//----------------------------------------------------------------------------
{
  int parsingPos=0;
  PreParse(inputTemplate,parsingPos);
  if (ConsistenceCheck())
    GenerateOutput(outputHTML);
}

//----------------------------------------------------------------------------
medHTMLTemplateParserBlock::HTMLTemplateSubstitution medHTMLTemplateParserBlock::GetSubstitution( wxString name )
//----------------------------------------------------------------------------
{
  HTMLTemplateSubstitution retValue;
  int retPos;
  retPos=SubstitutionPos(name);
  if (retPos>=0)
  {
    retValue=m_SubstitutionTable[retPos];
  }
  else 
  {
    mafLogMessage("HTML Template ERROR: Substitution: \"%s\" not found",name.ToAscii());
  }

  return retValue;
}

//----------------------------------------------------------------------------
void medHTMLTemplateParserBlock::WriteSubstitution( HTMLTemplateParsedItems var, wxString *outputHTML )
//----------------------------------------------------------------------------
{
  HTMLTemplateSubstitution substitution;

  substitution=m_SubstitutionTable[var.SubstitutionPos];

  switch (substitution.Type)
  {
    case MED_HTML_TEMPLATE_VARIABLE:
    {
      outputHTML->Append(m_Variables[substitution.Pos]);
    }
    break;
    case MED_HTML_TEMPLATE_VARIABLE_ARRAY:
    {
      if (m_BlockType!=MED_HTML_TEMPLATE_LOOP)
      {
        mafLogMessage("HTML Template ERROR: Substitution: \"%s\" not found",substitution.Name.ToAscii());
        return;
      }
      outputHTML->Append(m_VariablesArray[substitution.Pos][m_CurrentLoop]);
    }
    break;
    case MED_HTML_TEMPLATE_LOOP:
      {
        m_SubBlocksArray[substitution.Pos][m_CurrentLoop]->GenerateOutput(outputHTML);
      }
    break;
    case MED_HTML_TEMPLATE_IF:
      {
        m_SubBlocks[substitution.Pos]->GenerateOutput(outputHTML);
      }
    break;
    default:
      {
        mafLogMessage("HTML Template ERROR: WRONG Substitution type: \"%s\" not found",substitution.Name.ToAscii());
      }
  }

}

//----------------------------------------------------------------------------
void medHTMLTemplateParserBlock::CleanPreParsingInfo()
//----------------------------------------------------------------------------
{
    
}

//----------------------------------------------------------------------------
void medHTMLTemplateParserBlock::GenerateOutput( wxString *outputHTML )
//----------------------------------------------------------------------------
{
  int parsedChars;
  int totalChars;
  int totalLoops;
  int currentSubstitution;
  int parseTo;

  m_CurrentLoop=0;
  
  if (m_BlockType==MED_HTML_TEMPLATE_LOOP)
    totalLoops=m_LoopsNumber;
  else totalLoops=1;
 
  for (int i=0;i<totalLoops;i++)
  {
    currentSubstitution=0;
    parsedChars=0;
    totalChars=m_PreParsedHTML.size();
    if (m_BlockType==MED_HTML_TEMPLATE_IF)
    {
      //The if-block is composed of both if and else chars 
      //when the variable is true we consider the chars from 0 to m_IfChars 
      //when the variable is false we consider the chars from m_ifChars to end
      //[-----TRUE-----]m_ifChars[----FALSE----]
      if (m_IfCondition) 
        totalChars=m_IfChars;
      else 
        parsedChars=m_IfChars;
    }


    do 
    {
      if (currentSubstitution==m_Substitutions.size())
      {
        //copy all unparsed chars
        parseTo=totalChars;
      }
      else 
      {
        //copy all chars form current position (parsedChars) to the next substitution
        parseTo=m_Substitutions[currentSubstitution].TextPos;
      }
      
      outputHTML->Append(m_PreParsedHTML.SubString(parsedChars,parseTo));

      if (currentSubstitution<m_Substitutions.size())
        WriteSubstitution(m_Substitutions[currentSubstitution],outputHTML);

      currentSubstitution++;
      parsedChars=parseTo;
    } 
    while (parsedChars<totalChars);
  }

  
}

//----------------------------------------------------------------------------
void medHTMLTemplateParserBlock::updateChildrenVars()
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_SubstitutionTable.size();i++)
  {
    if (m_SubstitutionTable[i].Type==MED_HTML_TEMPLATE_VARIABLE_ARRAY)
    {
      wxString newValue;
      newValue=m_VariablesArray[m_SubstitutionTable[i].Pos][m_CurrentLoop];
      for (int j=0;j<m_SubBlocks.size();j++)
      {
        m_SubBlocks[j]->UpdateVar(m_SubstitutionTable[i].Name, newValue);
      }
      for (int j=0;j<m_SubBlocks.size();j++)
      {
        m_SubBlocksArray[j][m_CurrentLoop]->UpdateVar(m_SubstitutionTable[i].Name, newValue);
      }
    }
  }
}

//----------------------------------------------------------------------------
void medHTMLTemplateParserBlock::UpdateVar( wxString name, wxString newValue )
//----------------------------------------------------------------------------
{
  int pos;
  pos=SubstitutionPos(name);
  m_Variables[pos]=newValue;
  
  for (int i=0;i<m_SubBlocks.size();i++)
    m_SubBlocks[i]->UpdateVar(name,newValue);
  
  if (m_BlockType==MED_HTML_TEMPLATE_LOOP)
    for (int i=0;i<m_SubBlocksArray.size();i++)
      for(int j=0;j<m_SubBlocksArray[i].size();j++)
        m_SubBlocksArray[i][j]->UpdateVar(name,newValue);

}


//----------------------------------------------------------------------------
void medHTMLTemplateParserBlock::InheritVars( medHTMLTemplateParserBlock *father )
//----------------------------------------------------------------------------
{
  //Bocks Inherit ArrayVars as simple vars 
  //These will be updated every loop
  int pos;

  for(int i=0;i<father->m_SubstitutionTable.size();i++)
  {
    if (father->m_SubstitutionTable[i].Type==MED_HTML_TEMPLATE_VARIABLE)
    {
      pos=father->m_SubstitutionTable[i].Pos;
      AddVar(father->m_SubstitutionTable[i].Name,father->m_Variables[pos]);
    }
    else if (father->m_SubstitutionTable[i].Type==MED_HTML_TEMPLATE_VARIABLE_ARRAY)
    {
      pos=father->m_SubstitutionTable[i].Pos;
      //we inherit variables array from the first var
      AddVar(father->m_SubstitutionTable[i].Name,father->m_VariablesArray[pos][0]);
    }
  }
  
}

//----------------------------------------------------------------------------
void medHTMLTemplateParserBlock::PreParse( wxString *inputTemplate, int &parsingPos )
//----------------------------------------------------------------------------
{
  int templateSize=inputTemplate->size();
  int continueParsing=true;
  int stdHTMLtoCopy;

  while (parsingPos<templateSize && continueParsing)
  {
    stdHTMLtoCopy=0;
    //we start parsing outside from MAFtags 
    //we search the next tag inside the input and parse it
    while (parsingPos<templateSize && inputTemplate[parsingPos+stdHTMLtoCopy]!='[')
      stdHTMLtoCopy++;

    //coping stdHtml to preParsedHTML
    if (stdHTMLtoCopy>0)
      m_PreParsedHTML.Append(inputTemplate->SubString(parsingPos,parsingPos+stdHTMLtoCopy));

    parsingPos+=stdHTMLtoCopy;

    if (inputTemplate[parsingPos]=='[')
      continueParsing=PreParseTag(inputTemplate,parsingPos);
  }
  //continueParsing is true only if we reach the end of template and not find the end tag
  if (continueParsing && m_BlockType==MED_HTML_TEMPLATE_MAIN)
    mafLogMessage("HTML Template ERROR: NO [/MAFMain] Tag");
    
}

//----------------------------------------------------------------------------
int medHTMLTemplateParserBlock::SubStringCompare( wxString *input, char *subString, int inputPos )
//----------------------------------------------------------------------------
{
    int subStringSize;

    

    wxString subWxString=subString;
    subStringSize=subWxString.size();
    if (inputPos+subStringSize>input->size())
      return false;

    wxString tmpStr=input->SubString(inputPos,inputPos+subStringSize);
    
    //Substitute this whit CmpNoCase for case independent templates
    return !(tmpStr.Cmp(subWxString));
}

#define MAF_TAG_OPENING "[MAF"
#define MAF_TAG_CLOSING "[/MAF"
#define MAF_TAG_VARIABLE "Variable"
#define MAF_TAG_LOOP "Loop"
#define MAF_TAG_IF "If"
#define MAF_TAG_ELSE "Else"

//----------------------------------------------------------------------------
int medHTMLTemplateParserBlock::PreParseTag( wxString *inputTemplate, int &parsingPos )
//----------------------------------------------------------------------------
{
  int templateSize=inputTemplate->size();
  
 
  if (SubStringCompare(inputTemplate,MAF_TAG_OPENING,parsingPos)) 
  {    
    if (SubStringCompare(inputTemplate,MAF_TAG_VARIABLE,parsingPos))
    {
      parsingPos+=Strlen("MAF_TAG_VARIABLE");

      
    }
  }
  else if (SubStringCompare(inputTemplate,MAF_TAG_CLOSING,parsingPos)) 
  {

  }
  else 
  {
    //there is a '[' char but there is not a MAF tag
    //in this case the input must be copied to pre parsed string
    //we add '[' char to m_PreParsedHTML and increase parsingPos 
    //in order to avoid a non ending loop in the caller function
    m_PreParsedHTML.Append("[");
    parsingPos++;
    return true; //continue parsing
  }



}

//----------------------------------------------------------------------------
int medHTMLTemplateParserBlock::ConsistenceCheck()
//----------------------------------------------------------------------------
{

  ///TO DO;
  return true;
}

void medHTMLTemplateParserBlock::SkipInputSpaces( wxString *inputTemplate, int &parsingPos )
{
  //add more white spaces chars here if necessary
  while ( inputTemplate[parsingPos] == ' ')
    parsingPos++;
}



