/*=========================================================================

 Program: MAF2
 Module: mafHTMLTemplateParserBlock
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafHTMLTemplateParserBlock_H__
#define __mafHTMLTemplateParserBlock_H__

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
  maf_HTML_TEMPLATE_MAIN,
  maf_HTML_TEMPLATE_LOOP,
  maf_HTML_TEMPLATE_IF,
};

enum HTML_SUBSTITUTION_TYPES
{
maf_HTML_SUBSTITUTION_VARIABLE,
maf_HTML_SUBSTITUTION_VARIABLE_ARRAY,
maf_HTML_SUBSTITUTION_BLOCK,
maf_HTML_SUBSTITUTION_BLOCK_ARRAY,
maf_HTML_SUBSTITUTION_FORWARD_UP,
};

class mafView;
class wxString;

/**
 class name: mafHTMLTemplateParserBlock
    sub-class for Template parsing used by mafHTMLTemplateParser
*/
class MAF_EXPORT mafHTMLTemplateParserBlock
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
	mafHTMLTemplateParserBlock(int blockType, wxString name);
  /** destructor*/
	~mafHTMLTemplateParserBlock(); 

  /** Add the variable specified in the name string to the list of variables */
  void AddVar(wxString name, double varValue);
  /** Add the variable specified in the name string to the list of variables */
  void AddVar(wxString name, int varValue);
  /** Add the variable specified in the name string to the list of variables */
  void AddVar(wxString name, wxString varValue);
	/** Add the variable specified in the name string to the list of variables */
	void AddImageVar(wxString name, mafView *view, wxString imagePath = "");

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
  mafHTMLTemplateParserBlock *GetBlock(wxString name);

  /** Return the pointer to the block specified in name and pos.
     Used for block-arrays, if pos=-1 last pushed block will be returned*/
  mafHTMLTemplateParserBlock *GetNthBlock(wxString name,int pos=-1);

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
  void SetFather(mafHTMLTemplateParserBlock *father);
    
	wxString CalculateImageRTF(mafView *view, wxString imagePath);

  //VARIABLES
  mafHTMLTemplateParserBlock *m_Father;

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

  std::vector < mafHTMLTemplateParserBlock * > m_SubBlocks;
  std::vector < std::vector< mafHTMLTemplateParserBlock * > > m_SubBlocksArray;

  wxString m_PreParsedHTML;
  std::vector < HTMLTemplateParsedItems > m_Substitutions;

};
#endif
