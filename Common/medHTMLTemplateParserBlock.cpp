/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medHTMLTemplateParserBlock.cpp,v $
Language:  C++
Date:      $Date: 2012-01-31 16:55:55 $
Version:   $Revision: 1.1.2.1 $
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
      blockType != MED_HTML_TEMPLATE_ELSE && blockType != MED_HTML_TEMPLATE_IF)
  {
    mafLogMessage("medHTMLTemplateParserBlock: Invalid Block Type");
    return;
  }

  m_BlockName=name;
  m_BlockType=blockType;

  m_DoubleFormat="%.3f";

  m_LoopsNumber=-1;  
}


//----------------------------------------------------------------------------
medHTMLTemplateParserBlock::~medHTMLTemplateParserBlock()
//----------------------------------------------------------------------------
{

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
void medHTMLTemplateParserBlock::Parse( wxString *inputTemplate,wxString *outputHTML )
//----------------------------------------------------------------------------
{

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
void medHTMLTemplateParserBlock::WriteVar( HTMLTemplateSubstitution var, wxString *outputHTML )
//----------------------------------------------------------------------------
{

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

}

//----------------------------------------------------------------------------
void medHTMLTemplateParserBlock::InheritVars( medHTMLTemplateParserBlock *father )
//----------------------------------------------------------------------------
{
  //Loops does not Inherit ArrayVars 

}

//----------------------------------------------------------------------------
int medHTMLTemplateParserBlock::ConsistenceCheck()
//----------------------------------------------------------------------------
{
  return true;
}
