/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medHTMLTemplateParserBlock.h,v $
Language:  C++
Date:      $Date: 2012-04-06 08:17:58 $
Version:   $Revision: 1.1.2.8 $
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

#ifndef __medHTMLTemplateParserBlock_H__
#define __medHTMLTemplateParserBlock_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medCommonDefines.h" //important: mafDefines should always be included as first
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

enum HTMLBLOCK_TYPES
{
  MED_HTML_TEMPLATE_MAIN,
  MED_HTML_TEMPLATE_LOOP,
  MED_HTML_TEMPLATE_IF,
};

enum HTML_SUBSTITUTION_TYPES
{
MED_HTML_SUBSTITUTION_VARIABLE,
MED_HTML_SUBSTITUTION_VARIABLE_ARRAY,
MED_HTML_SUBSTITUTION_BLOCK,
MED_HTML_SUBSTITUTION_BLOCK_ARRAY,
MED_HTML_SUBSTITUTION_FORWARD_UP,
};


class wxString;

/**
 class name: medHTMLTemplateParserBlock
    sub-class for Template parsing used by medHTMLTemplateParser
*/
class MED_COMMON_EXPORT medHTMLTemplateParserBlock
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
    int SubsTablePos;    
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
  
  /** Push the variable specified in the name string to the list of variables */
  void PushVar(wxString name, double varValue);
  /** Push the variable specified in the name string to the list of variables */
  void PushVar(wxString name, int varValue);
  /** Push the variable specified in the name string to the list of variables */
  void PushVar(wxString name, wxString varValue);
  
  /** Return the string of the Variable 
      used for added vars*/
  wxString GetVar(wxString name);

  /** Return the string of the Variable
      Used for variable-arrays, if pos=-1 last pushed variable will be returned
      used for pushed vars*/
  wxString GetNthVar(wxString name, int arrayPos=-1);


  /** Add the Block specified in the list of Blocks*/
  void AddBlock(wxString name, int blockType);

  /** Push the Block specified in the list of Blocks*/
  void PushBlock(wxString name, int blockType);

  /** Return the pointer to the block specified in name 
     if the block is part of an block-array last pushed block will be returned */
  medHTMLTemplateParserBlock *GetBlock(wxString name);

  /** Return the pointer to the block specified in name and pos.
     Used for block-arrays, if pos=-1 last pushed block will be returned*/
  medHTMLTemplateParserBlock *GetNthBlock(wxString name,int pos=-1);

  /** Set the verity value for the "if" blocks*/
  void SetIfCondition(int condition);

  /** Set the verity value for the "if" blocks*/
  int GetIfCondition();

  /** Set the number of loops for the "if" blocks*/
  void SetNLoops(int nloops);

  /** Set the number of loops for the "if" blocks*/
  int GetNLoops();

protected:
  /** Return the position of the variable in the Variable Table.
     Returns -1 if variable is not found */
  int SubstitutionPos(wxString *name);
    
  /** Finds a variable starting from his name */
  HTMLTemplateSubstitution GetSubstitution(wxString name);

  /** Write a specified Var to the output */
  void  WriteSubstitution(HTMLTemplateSubstitution var, wxString *outputHTML);

  /** Generate PreParsing structures from input template 
      for this block and return the number of parsed chars */
  void PreParse(wxString *inputTemplate, int &parsingPos);

  /** Parse the input starting from a tag */
  int PreParseTag(wxString *inputTemplate, int &parsingPos);

  /** Utility function that skip spaces on input template */
  void SkipInputSpaces(wxString *inputTemplate, int &parsingPos);

  /** AddSubstitution to m_Subistitution if necessary search the correspondent variable
      to the block root, returns the position of the added substitution*/
  int AddSubstitution(wxString *tagName, int SubstitutionType); 

  /** This function is called when a substitution is not found in local scope in order to search
      it upward to the blocks root.
      Returns the father substitution pos and generates a path to the substitution if is necessary */
  int AddForward(wxString *tagName, int substitutionType);

  /** Return true if input contains subString starting from inputPos */
  int SubStringCompare(wxString *input, char *subString, int inputPos);

  /** Clean the structures generated on PreParse phase */
  void CleanPreParsingInfo();
  
  /** Generate HTML output */
  void GenerateOutput(wxString *outputHTML);

  /** Reads the name of the tag from the input template and save it to varName.
      This function skips spaces and the closing char ']'*/
  void ReadTagName(wxString *inputTemplate, int &parsingPos, wxString &tagName);

  /** Check the consistence of the generated structure*/
  int ConsistenceCheck();

  /** Sets the father for the block */
  void SetFather(medHTMLTemplateParserBlock *father);
    
  //VARIABLES
  medHTMLTemplateParserBlock *m_Father;

  wxString m_BlockName;
  int m_BlockType; 
  
  int m_IfChars;
  bool m_IfCondition;
  
  wxString m_DoubleFormat;
  int m_CurrentLoop;
  int m_LoopsNumber;

  std::vector<HTMLTemplateSubstitution>	m_SubstitutionTable;

  std::vector < wxString > m_Variables;
  std::vector < std::vector<wxString> > m_VariablesArray;

  std::vector < medHTMLTemplateParserBlock * > m_SubBlocks;
  std::vector < std::vector< medHTMLTemplateParserBlock * > > m_SubBlocksArray;

  wxString m_PreParsedHTML;
  std::vector < HTMLTemplateParsedItems > m_Substitutions;

};
#endif
