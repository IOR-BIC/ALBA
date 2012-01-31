/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medHTMLTemplateParserBlock.h,v $
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

#ifndef __medHTMLTemplateParserBlock_H__
#define __medHTMLTemplateParserBlock_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h" //important: mafDefines should always be included as first
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

enum HTMLBLOCK_TYPES
{
  MED_HTML_TEMPLATE_MAIN,
  MED_HTML_TEMPLATE_LOOP,
  MED_HTML_TEMPLATE_IF,
  MED_HTML_TEMPLATE_ELSE,
  MED_HTML_TEMPLATE_VARIABLE,
  MED_HTML_TEMPLATE_VARIABLE_ARRAY,
};

enum HTML_PARSED_TYPES
{
  MED_HTML_TEMPLATE_VAR,
  MED_HTML_TEMPLATE_BLOCK,
};

class wxString;

class MED_EXPORT medHTMLTemplateParserBlock
{

  
public:
  typedef struct vars{
    wxString Name;
    int Type;
    int Pos;
  } HTMLTemplateSubstitution;

public:
  typedef struct parsedItems{
    int TextPos;
    int ItemType;
    int ItemPos;    
  } HTMLTemplateParsedItems;

  /** constructor*/
	medHTMLTemplateParserBlock(int blockType, wxString name);
  /** destructor*/
	~medHTMLTemplateParserBlock(); 

  /** Add the variable specified in the name string to the list of variables */
  void AddVar(wxString name, double varValue);
  /** Add the variable specified in the name string to the list of variables */
  void AddVar(wxString name, int varValue);
  /** Add the variable specified in the name string to the list of variables */
  void AddVar(wxString name, wxString varValue);
  
  /** Add the variable specified in the name string to the list of variables */
  void PushVar(wxString name, double varValue);
  /** Add the variable specified in the name string to the list of variables */
  void PushVar(wxString name, int varValue);
  /** Add the variable specified in the name string to the list of variables */
  void PushVar(wxString name, wxString varValue);
  
  /** Add the Block specified in the list of Blocks*/
  void AddBlock(wxString name, int blockType);

  /** Template parsing */
  void Parse(wxString *inputTemplate,wxString *outputHTML);
    
protected:
  /* Return the position of the variable in the Variable Table.
     Returns -1 if variable is not found */
  int SubstitutionPos(wxString name);
    
  /* Finds a variable starting from his name */
  HTMLTemplateSubstitution GetSubstitution(wxString name);

  /* Write a specified Var to the output */
  void  WriteVar(HTMLTemplateSubstitution var, wxString *outputHTML);

  /** Generate PreParsing structures from input template 
      for this block and return the number of parsed chars */
  int PreParse(wxString *inputTemplate, int parsingPos);

  /** Clean the structures generated on PreParse phase */
  void CleanPreParsingInfo();

  /** Generate HTML output */
  void GenerateOutput(wxString *outputHTML);

  /** Inherit Variables from father Block */
  void InheritVars(medHTMLTemplateParserBlock *father);

  /** Check the consistence of the generated structure*/
  int ConsistenceCheck();
    
  //VARIABLES
  wxString m_BlockName;
  int m_BlockType;
  
  wxString m_DoubleFormat;
  int m_LoopsNumber;

  std::vector<HTMLTemplateSubstitution>	m_SubstitutionTable;

  std::vector<wxString> m_Variables;
  std::vector< std::vector<wxString> > m_VariablesArray;

  std::vector < medHTMLTemplateParserBlock * > m_SubBlocks;
  std::vector< std::vector< medHTMLTemplateParserBlock * > > m_SubBlocksArray;
};
#endif
