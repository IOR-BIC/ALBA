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


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafHTMLTemplateParserBlock.h"
#include "mafView.h"
#include "mafRWIBase.h"
#include "mafDecl.h"

//----------------------------------------------------------------------------
mafHTMLTemplateParserBlock::mafHTMLTemplateParserBlock(int blockType, wxString name)
//----------------------------------------------------------------------------
{
  //Checking Block Type
  if (blockType != maf_HTML_TEMPLATE_MAIN && blockType != maf_HTML_TEMPLATE_LOOP &&
      blockType != maf_HTML_TEMPLATE_IF)
  {
    mafLogMessage("mafHTMLTemplateParserBlock: Invalid Block Type");
    return;
  }


  m_BlockName=name;
  m_BlockType=blockType;
  m_IfChars=0;

  m_Father=NULL;

  m_DoubleFormat="%.3f";

  m_CurrentLoop=0;
  m_LoopsNumber=-1;  
}


//----------------------------------------------------------------------------
mafHTMLTemplateParserBlock::~mafHTMLTemplateParserBlock()
//----------------------------------------------------------------------------
{
  m_SubstitutionTable.clear();
  
  //Cleaning Variables
  m_Variables.clear();

  //Deleting Cleaning variables inside each sub-array
  for (int i=0;i<m_VariablesArray.size();i++)
    m_VariablesArray[i].clear();
  //Cleaning the array
  m_VariablesArray.clear();

  //Deleting sub-blocks
  for (int i=0;i<m_SubBlocks.size();i++)
    delete m_SubBlocks[i];
  m_SubBlocks.clear();
  
  for (int i=0;i<m_SubBlocksArray.size();i++)
  {
    //Deleting Sub-Array blocks and cleaning array
    for (int j=0;j<m_SubBlocksArray[i].size();j++)
      delete m_SubBlocksArray[i][j];
    m_SubBlocksArray[i].clear();
  }
  m_SubBlocksArray.clear();
    
}


//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlock::AddVar( wxString name, double varValue )
//----------------------------------------------------------------------------
{
  //Formatting the double var using m_Doubleformat ("%.3f") 
  AddVar(name,wxString::Format(m_DoubleFormat,varValue));
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlock::AddVar( wxString name, int varValue )
//----------------------------------------------------------------------------
{
  AddVar(name,wxString::Format("%d",varValue));
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlock::AddVar( wxString name, wxString varValue )
//----------------------------------------------------------------------------
{
  //if Variable already exists we dont add it again
  if (SubstitutionPos(&name)>0)
    mafLogMessage("HTML Template ERROR: Variable: \"%s\" already exists",name.ToAscii());
  else
  {
    //Adding the Variable to the array and the relative substitution to the substitution table
    HTMLTemplateSubstitution subst;
    subst.Name=name;
    subst.Pos=m_Variables.size();
    subst.Type=maf_HTML_SUBSTITUTION_VARIABLE;
    m_SubstitutionTable.push_back(subst);
    m_Variables.push_back(varValue);
  }
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlock::AddImageVar(wxString name, mafView *view, wxString imagePath)
//----------------------------------------------------------------------------
{
	if (view)
		AddVar(name, CalculateImageRTF(view, imagePath)); //RTF image generation

	//TODO AddImageVar for HTML Template
}


//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlock::PushVar( wxString name, double varValue )
//----------------------------------------------------------------------------
{
  PushVar(name,wxString::Format(m_DoubleFormat,varValue));
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlock::PushVar( wxString name, int varValue )
//----------------------------------------------------------------------------
{
  PushVar(name,wxString::Format("%d",varValue));
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlock::PushVar( wxString name, wxString varValue )
//----------------------------------------------------------------------------
{
  int varPos=SubstitutionPos(&name);
  int blockPos;

  //We can push Variables only to the loops block
  if (m_BlockType!=maf_HTML_TEMPLATE_LOOP)
  {
    mafLogMessage("HTML Template ERROR: You can push variables only on Loop blocks, use AddVar instead");
    return;
  }
  //We need to set the loop number before pushing anything to the loop
  else if (m_LoopsNumber<0)
  {
    mafLogMessage("HTML Template ERROR: You must set the loops number before push any variable");
    return;
  }

  //if the vars does not exists i need to create the array entry
  if (varPos<0)
  {
    std::vector<wxString> newArray;
    m_VariablesArray.push_back(newArray);
    
    HTMLTemplateSubstitution subst;
    subst.Name=name;
    subst.Pos=m_VariablesArray.size()-1;
    subst.Type=maf_HTML_SUBSTITUTION_VARIABLE_ARRAY;
    
    //if a new entry is created in the substitution table the position is
    //equal at the size before the push 
    varPos=m_SubstitutionTable.size();
    m_SubstitutionTable.push_back(subst);
  }

  blockPos=m_SubstitutionTable[varPos].Pos;

  //checking substitusion type before push
  if (m_SubstitutionTable[varPos].Type!=maf_HTML_SUBSTITUTION_VARIABLE_ARRAY)
  {
    mafLogMessage("HTML Template ERROR: Array: \"%s\" has incompatible type",name.ToAscii());
    return;
  }
  //checking if there are free slots for pushing vars
  else if (m_VariablesArray[blockPos].size()>m_LoopsNumber)
  {
    mafLogMessage("HTML Template ERROR: to many push for: '%s'",name.ToAscii());
    return;
  }
  
  m_VariablesArray[blockPos].push_back(varValue);
}



//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlock::AddBlock( wxString name, int blockType )
//----------------------------------------------------------------------------
{
  //checking if block already exist
  if (SubstitutionPos(&name)>0)
  {
    mafLogMessage("HTML Template ERROR: Block: \"%s\" already exists",name.ToAscii());
  }
  else
  {
    //adding the new block in block array and the relative substitution to the table
    mafHTMLTemplateParserBlock *newBlock;
    newBlock=new mafHTMLTemplateParserBlock(blockType, name);
    newBlock->SetFather(this);

    HTMLTemplateSubstitution subst;
    subst.Name=name;
    subst.Pos=m_SubBlocks.size();
    subst.Type=maf_HTML_SUBSTITUTION_BLOCK;
    m_SubstitutionTable.push_back(subst);
    m_SubBlocks.push_back(newBlock);
  }
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlock::PushBlock( wxString name, int blockType )
//----------------------------------------------------------------------------
{
  int varPos=SubstitutionPos(&name);
  int blockPos;

  //blocks can be pushed only on loop blocks
  if (m_BlockType!=maf_HTML_TEMPLATE_LOOP)
  {
    mafLogMessage("HTML Template ERROR: You can push sub-blocks only on Loop blocks, , use AddBLock instead");
    return;
  }
  //we need to set the loop number before any push
  else if (m_LoopsNumber<0)
  {
    mafLogMessage("HTML Template ERROR: You must set the loops number before push any sub-block");
    return;
  }
  //if the block does not exists i need to create the array entry
  if (varPos<0)
  {
    //creating the new array for this block and
    //adding the substitution in the substitution table
    std::vector<mafHTMLTemplateParserBlock *> newArray;
    m_SubBlocksArray.push_back(newArray);
    
    HTMLTemplateSubstitution subst;
    subst.Name=name;
    subst.Pos=m_SubBlocksArray.size()-1;
    subst.Type=maf_HTML_SUBSTITUTION_BLOCK_ARRAY;

    varPos=m_SubstitutionTable.size();
    m_SubstitutionTable.push_back(subst);
  }
  //if the block already exist there are already the correspondent array
  //and the relative substitution so we don't need to add it

  blockPos=m_SubstitutionTable[varPos].Pos;

  //checking if the existent substitution refers to a block array to
  if (m_SubstitutionTable[varPos].Type!=maf_HTML_SUBSTITUTION_BLOCK_ARRAY)
  {
    mafLogMessage("HTML Template ERROR: Array: \"%s\" has incompatible type",name.ToAscii());
    return;
  }
  //checking if there are free slots for pushing blocks
  else if (m_SubBlocksArray[blockPos].size()>m_LoopsNumber)
  {
    mafLogMessage("HTML Template ERROR: to many push for: '%s'",name.ToAscii());
    return;
  }
  else
  {
    mafHTMLTemplateParserBlock *newBlock;
    newBlock=new mafHTMLTemplateParserBlock(blockType, name);
    newBlock->SetFather(this);
    m_SubBlocksArray[blockPos].push_back(newBlock);
  }

}


//----------------------------------------------------------------------------
int mafHTMLTemplateParserBlock::SubstitutionPos( wxString *name )
//----------------------------------------------------------------------------
{
  //searching in the substitutions table for a block name
  for (int i=0;i<m_SubstitutionTable.size();i++)
  {
    if (name->compare(m_SubstitutionTable[i].Name)==0)
      return i;
  }
  return -1;
}




//----------------------------------------------------------------------------
mafHTMLTemplateParserBlock::HTMLTemplateSubstitution mafHTMLTemplateParserBlock::GetSubstitution( wxString name )
//----------------------------------------------------------------------------
{
  HTMLTemplateSubstitution retValue;
  int retPos;
  //get the position of the substitution
  retPos=SubstitutionPos(&name);
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
void mafHTMLTemplateParserBlock::WriteSubstitution( HTMLTemplateSubstitution substitution, wxString *outputHTML )
//----------------------------------------------------------------------------
{
   switch (substitution.Type)
  {
    //write the matching output to outputHTML depending on sustitution type
    case maf_HTML_SUBSTITUTION_VARIABLE:
    {
      outputHTML->Append(m_Variables[substitution.Pos]);
    }
    break;
    case maf_HTML_SUBSTITUTION_VARIABLE_ARRAY:
    {
      if (m_BlockType!=maf_HTML_TEMPLATE_LOOP)
      {
        mafLogMessage("HTML Template ERROR: Substitution: \"%s\" not found",substitution.Name.ToAscii());
        return;
      }
      outputHTML->Append(m_VariablesArray[substitution.Pos][m_CurrentLoop]);
    }
    break;
    //if the substitution match a block we write is output to the outputHTML starting 
    //from this point
    case maf_HTML_SUBSTITUTION_BLOCK:
    {
      m_SubBlocks[substitution.Pos]->GenerateOutput(outputHTML);
    }
    break;
    case maf_HTML_SUBSTITUTION_BLOCK_ARRAY:
    {
      m_SubBlocksArray[substitution.Pos][m_CurrentLoop]->GenerateOutput(outputHTML);
    }
    break;
    //if a variable is not in local scope there is  a forward up substitution
    //so we need to call this method in the father block to write the substitution
    case maf_HTML_SUBSTITUTION_FORWARD_UP:
    {
      int fatherPos;
      HTMLTemplateSubstitution fatherSubstituion;
      fatherPos=substitution.Pos;
      fatherSubstituion=m_Father->m_SubstitutionTable[fatherPos];
      m_Father->WriteSubstitution(fatherSubstituion,outputHTML);
    }
    break;
    default:
    {
      mafLogMessage("HTML Template ERROR: WRONG Substitution type: \"%s\" not found",substitution.Name.ToAscii());
    }
  }

}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlock::CleanPreParsingInfo()
//----------------------------------------------------------------------------
{
    
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlock::GenerateOutput( wxString *outputHTML )
//----------------------------------------------------------------------------
{
  int parsedChars;
  int totalChars;
  int totalLoops;
  int currentSubstitution;
  int parseTo;

  m_CurrentLoop=0;
  
  //for non loops block we need to generate the output just one time
  if (m_BlockType==maf_HTML_TEMPLATE_LOOP)
    totalLoops=m_LoopsNumber;
  else totalLoops=1;
 
  for (int i=0;i<totalLoops;i++)
  {
    m_CurrentLoop=i;
    currentSubstitution=0;
    parsedChars=0;
    totalChars=m_PreParsedHTML.size();
    if (m_BlockType==maf_HTML_TEMPLATE_IF)
    {
      //The if-block is composed of both if and else chars 
      //when the variable is true we consider the chars from 0 to m_IfChars 
      //when the variable is false we consider the chars from m_ifChars to end
      //[-----TRUE-----]m_ifChars[----FALSE----]
      if (m_IfCondition) 
        totalChars=m_IfChars;
      else 
      {
        parsedChars=m_IfChars;
        //jumping substitution of the true part
        while (currentSubstitution<m_Substitutions.size() && m_Substitutions[currentSubstitution].TextPos< parsedChars)
          currentSubstitution++;
      }
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
      

      //coping the simple html unparsed part to the output
      outputHTML->Append(m_PreParsedHTML.SubString(parsedChars,parseTo-1));

      //if there are substitution to write we write it 
      //at the end of the block we don't write any substitution
      if (currentSubstitution<m_Substitutions.size())
      {
        int pos;
        HTMLTemplateSubstitution substitution;
        pos=m_Substitutions[currentSubstitution].SubsTablePos;
        substitution=m_SubstitutionTable[pos];
        WriteSubstitution(substitution,outputHTML);
      }

      currentSubstitution++;
      parsedChars=parseTo;
    } 
    while (parsedChars<totalChars);
  }

  
}



//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlock::PreParse( wxString *inputTemplate, int &parsingPos )
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
    while (parsingPos+stdHTMLtoCopy<templateSize && inputTemplate->GetChar(parsingPos+stdHTMLtoCopy)!='[')
      stdHTMLtoCopy++;

    if (inputTemplate->GetChar(parsingPos)=='[')
      stdHTMLtoCopy--;

    //coping stdHtml to preParsedHTML
    if (stdHTMLtoCopy>0)
      m_PreParsedHTML.Append(inputTemplate->SubString(parsingPos,parsingPos+stdHTMLtoCopy-1));

    parsingPos+=stdHTMLtoCopy;

    if (inputTemplate->GetChar(parsingPos)=='[')
      continueParsing=PreParseTag(inputTemplate,parsingPos);
  }
  //continueParsing is true only if we reach the end of template and not find the end tag
  if (!continueParsing && m_BlockType==maf_HTML_TEMPLATE_MAIN)
    mafLogMessage("HTML Template ERROR: Wrong Closing Tag");
    
}

//----------------------------------------------------------------------------
int mafHTMLTemplateParserBlock::SubStringCompare( wxString *input, char *subString, int inputPos )
//----------------------------------------------------------------------------
{
    int subStringSize;

    

    wxString subWxString=subString;
    subStringSize=subWxString.size();
    if (inputPos+subStringSize>input->size())
      return false;

    //split the input string from current pos (inputpos) to the size of the sub string
    wxString tmpStr=input->SubString(inputPos,inputPos+subStringSize-1);
    
    //Substitute this whit CmpNoCase for case independent templates
    return !(tmpStr.Cmp(subWxString));
}

#define maf_HMTL_TAG_OPENING "[MAF"
#define maf_HTML_TAG_CLOSING "[/MAF"
#define maf_HTML_TAG_VARIABLE "Variable"
#define maf_HTML_TAG_LOOP "Loop"
#define maf_HTML_TAG_IF "If"
#define maf_HTML_TAG_ELSE "Else"

//----------------------------------------------------------------------------
int mafHTMLTemplateParserBlock::PreParseTag( wxString *inputTemplate, int &parsingPos )
//----------------------------------------------------------------------------
{
  int templateSize=inputTemplate->size();
  wxString tagName;

  // OPENING TAG
  if (SubStringCompare(inputTemplate,maf_HMTL_TAG_OPENING,parsingPos)) 
  { 
    int substitutionType;
    
    int subPos;

    parsingPos+=Strlen(maf_HMTL_TAG_OPENING);
    //cheking the opening tag type
    if (SubStringCompare(inputTemplate,maf_HTML_TAG_VARIABLE,parsingPos))
    {
      substitutionType=maf_HTML_SUBSTITUTION_VARIABLE;
      parsingPos+=Strlen(maf_HTML_TAG_VARIABLE);
    }
    else if (SubStringCompare(inputTemplate,maf_HTML_TAG_LOOP,parsingPos))
    {
      substitutionType=maf_HTML_SUBSTITUTION_BLOCK;
      parsingPos+=Strlen(maf_HTML_TAG_LOOP);
    }
    else if (SubStringCompare(inputTemplate,maf_HTML_TAG_IF,parsingPos))
    {
      substitutionType=maf_HTML_SUBSTITUTION_BLOCK;
      parsingPos+=Strlen(maf_HTML_TAG_IF);
    }
    else if (SubStringCompare(inputTemplate,maf_HTML_TAG_ELSE,parsingPos))
    {
      //if I find a else tag I update local variables and continue parsing inside this block
      if (m_BlockType!=maf_HTML_TEMPLATE_IF)
        mafLogMessage("mafHTMLTemplateParserBlock: [MAFElse] found inside a non [MAFIf] block ");
      parsingPos+=Strlen(maf_HTML_TAG_ELSE);

      ReadTagName(inputTemplate,parsingPos,tagName);

      int comp=tagName.Cmp(m_BlockName);

      if (comp!=0)
        mafLogMessage(" ELSE mafHTMLTemplateParserBlock: warning wrong closing tag");
      else m_IfChars=m_PreParsedHTML.size();

      //return because Else tag has not a tag name
      return true;
    }
    else 
      mafLogMessage("mafHTMLTemplateParserBlock: Invalid TAG Type");

    ReadTagName(inputTemplate,parsingPos,tagName);
    subPos=AddSubstitution(&tagName,substitutionType);
    
   

    //Parsing sub tags
    if (subPos>=0 && (substitutionType==maf_HTML_TEMPLATE_IF || substitutionType==maf_HTML_TEMPLATE_LOOP))
    {
      int tablePos=m_Substitutions[subPos].SubsTablePos;
      int blockPos=m_SubstitutionTable[tablePos].Pos;
      
      if (m_SubstitutionTable[tablePos].Type==maf_HTML_SUBSTITUTION_BLOCK)
        m_SubBlocks[blockPos]->PreParse(inputTemplate,parsingPos);
      else if (m_SubstitutionTable[tablePos].Type==maf_HTML_SUBSTITUTION_BLOCK_ARRAY)
      { 
        int actualParsingPos=parsingPos;
        for (int i=0;i<m_LoopsNumber;i++)
        {
          //Every sub block must be parsed, each loop parsing pos will be increased so we need to 
          //move the parsing pos back in order to parse the same text to each loop
          parsingPos=actualParsingPos;
          int x=m_SubBlocksArray[blockPos].size();
          mafHTMLTemplateParserBlock *blockPointer=m_SubBlocksArray[blockPos][i];
          blockPointer->PreParse(inputTemplate,parsingPos);
        }
      }
      else if (m_SubstitutionTable[tablePos].Type=maf_HTML_SUBSTITUTION_FORWARD_UP)
      {
          mafLogMessage("Tag Name already used");
          return false;
      }
    }
    
  }
  //CLOSING TAG
  else if (SubStringCompare(inputTemplate,maf_HTML_TAG_CLOSING,parsingPos)) 
  {
    parsingPos+=Strlen(maf_HTML_TAG_CLOSING);

    if (SubStringCompare(inputTemplate,maf_HTML_TAG_LOOP,parsingPos))
    {
      parsingPos+=Strlen(maf_HTML_TAG_LOOP);
      if (m_BlockType!=maf_HTML_TEMPLATE_LOOP)
        mafLogMessage("mafHTMLTemplateParserBlock: Invalid closing TAG Type");
    }
    else if (SubStringCompare(inputTemplate,maf_HTML_TAG_IF,parsingPos))
    {
      parsingPos+=Strlen(maf_HTML_TAG_IF);
      if (m_BlockType!=maf_HTML_TEMPLATE_IF)
        mafLogMessage("mafHTMLTemplateParserBlock: Invalid closing TAG Type");
      //if there is not an ELSE TAG the if char are all chars now
      if (m_IfChars==0)
        m_IfChars=m_PreParsedHTML.size();
    }
    else 
     mafLogMessage("mafHTMLTemplateParserBlock: Invalid TAG Type");
   
    ReadTagName(inputTemplate,parsingPos,tagName);


    int comp=tagName.Cmp(m_BlockName);

    if (comp!=0)
      mafLogMessage("mafHTMLTemplateParserBlock: warning wrong closing tag");

    return comp;
  } 
  else 
  {
    //there is a '[' char but there is not a MAF tag
    //in this case the input must be copied to pre parsed string
    //we add '[' char to m_PreParsedHTML and increase parsingPos 
    //in order to avoid a non ending loop in the caller function
    m_PreParsedHTML.Append("[");
    parsingPos++;
  }

  //continue parsing
  return true;
}

//----------------------------------------------------------------------------
int mafHTMLTemplateParserBlock::ConsistenceCheck()
//----------------------------------------------------------------------------
{

  if (m_BlockType==maf_HTML_TEMPLATE_LOOP)
  {
    //m_LoopsNumber must be set for loop blocks
    if (m_LoopsNumber<0)
    {
       mafLogMessage("mafHTMLTemplateParserBlock: loops number is not set for:%s",m_BlockName.c_str());
       return false;
    }
    for (int i=0;i<m_VariablesArray.size();i++)
    {
      //Each variable array must contain exactly m_LoopsNumber Elements
      if (m_VariablesArray[i].size()!=m_LoopsNumber)
      {
        mafLogMessage("mafHTMLTemplateParserBlock: wrong variable number for:%s Block",m_BlockName.c_str());
        return false;
      }
    }
    for (int i=0;i<m_SubBlocksArray.size();i++)
    {
      //Each Sub-Blocks array must contain exactly m_LoopsNumber Elements
      if (m_SubBlocksArray[i].size()!=m_LoopsNumber)
      {
        mafLogMessage("mafHTMLTemplateParserBlock: wrong Sub-Blocks number for:%s Block",m_BlockName.c_str());
        return false;
      }
    }
  }

  //Each Sub-Block must be consistent
  for (int i=0;i<m_SubBlocks.size();i++)
    if (m_SubBlocks[i]->ConsistenceCheck()==false)
      return false;

  //Also Each Sub-Block in each Array must be consistent 
  for (int i=0;i<m_SubBlocksArray.size();i++)
    for (int j=0;j<m_SubBlocksArray[i].size();j++)
      if (m_SubBlocksArray[i][j]->ConsistenceCheck()==false)
        return false;

  return true;
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlock::SkipInputSpaces( wxString *inputTemplate, int &parsingPos )
//----------------------------------------------------------------------------
{
  //add more white spaces chars here if necessary
  while ( inputTemplate->GetChar(parsingPos) == ' ')
    parsingPos++;
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlock::ReadTagName( wxString *inputTemplate, int &parsingPos, wxString &tagName )
//----------------------------------------------------------------------------
{
  int nameStart;
  //skip spaces before name 
  SkipInputSpaces(inputTemplate,parsingPos);
  nameStart=parsingPos;

  //reads textual char (stopping on spaces or tag end)
  while ( inputTemplate->GetChar(parsingPos) != ' ' &&  inputTemplate->GetChar(parsingPos) != ']' )
    parsingPos++;


  tagName=inputTemplate->SubString(nameStart,parsingPos-1);

  //skip other spaces
  SkipInputSpaces(inputTemplate,parsingPos);

  //read closing char ']'
  if (inputTemplate->GetChar(parsingPos)!=']')
    mafLogMessage("mafHTMLTemplateParserBlock: Expected tag end:']'");
  else parsingPos++; 
}

//----------------------------------------------------------------------------
int mafHTMLTemplateParserBlock::AddSubstitution(wxString *tagName, int SubstitutionType)
//----------------------------------------------------------------------------
{
  int pos;
  HTMLTemplateParsedItems substitution;
  
  //Searching in substitution table
  pos=SubstitutionPos(tagName);

  if (pos>=0) 
    substitution.SubsTablePos=pos;    
  else 
    //if a substitution was not found locally we search to the root
    substitution.SubsTablePos=AddForward(tagName,SubstitutionType);
  
  //if a substitution was not found in local or global scope 
  //we return -1 and not add it to local m_substitutions
  if (substitution.SubsTablePos<0)
    return -1;
  else 
  {
    substitution.TextPos=m_PreParsedHTML.size();
    m_Substitutions.push_back(substitution);

    return m_Substitutions.size()-1;
  }
}

//----------------------------------------------------------------------------
int mafHTMLTemplateParserBlock::AddForward( wxString *tagName, int SubstitutionType )
//----------------------------------------------------------------------------
{
  int pos;

  pos=SubstitutionPos(tagName);
  // if the substitution is found in the local scope we don't need to add a new up forward 
  // we simply return the tablepos of the substitution and the soon will add this info to his scope
  if (pos>=0)
  {
    if (m_SubstitutionTable[pos].Type==maf_HTML_SUBSTITUTION_BLOCK || m_SubstitutionTable[pos].Type==maf_HTML_SUBSTITUTION_BLOCK_ARRAY)
    {
      mafLogMessage("mafHTMLTemplateParserBlock Warning: Tag '%s' was not found as Block",tagName->c_str());
      return -1;
    }
    return pos;
  }
  // if this is the root (maf_HTML_TEMPLATE_MAIN) and the substitution was not found the substitution 
  // does not exist and we return -1
  else if (m_BlockType == maf_HTML_TEMPLATE_MAIN)
  {
    mafLogMessage("mafHTMLTemplateParserBlock: Warning Tag '%s' was not found",tagName->c_str());
    return -1;
  }
  else 
  {
    HTMLTemplateSubstitution subst;
    if (m_Father==NULL)
    {
      mafLogMessage("mafHTMLTemplateParserBlock: Father is not set");
      return -1;
    }

    subst.Name=*tagName;
    //the position of this substitution is the position in the father table
    subst.Pos=m_Father->AddForward(tagName,SubstitutionType);
    if (subst.Pos<0)
      return -1;
    subst.Type=maf_HTML_SUBSTITUTION_FORWARD_UP;
    m_SubstitutionTable.push_back(subst);
    return m_SubstitutionTable.size()-1;
  }
  
}


//----------------------------------------------------------------------------
mafHTMLTemplateParserBlock * mafHTMLTemplateParserBlock::GetBlock( wxString name )
//----------------------------------------------------------------------------
{

  //searching in m_SubBlocks
  for (int i=0;i<m_SubBlocks.size();i++)
  {
    if (name.compare(m_SubBlocks[i]->m_BlockName)==0)
      return m_SubBlocks[i];
  }

  //if blocks is not present in main block and this is a loop we search in m_SubBlocksArray too
  if (m_BlockType==maf_HTML_TEMPLATE_LOOP)
    return GetNthBlock(name);

  //block not found
  mafLogMessage("mafHTMLTemplateParserBlock: Block: '%s' not found!",name.c_str());
  return NULL;

}

//----------------------------------------------------------------------------
mafHTMLTemplateParserBlock * mafHTMLTemplateParserBlock::GetNthBlock( wxString name,int pos/*=-1*/ )
//----------------------------------------------------------------------------
{
  //searching in m_SubBlocks
  int elements;
  for (int i=0;i<m_SubBlocksArray.size();i++)
  {
    elements = m_SubBlocksArray[i].size()-1;
    if (name.compare(m_SubBlocksArray[i][0]->m_BlockName)==0)
    {
      //if pos == -1 we return the last element of the array.
      if (pos>0)
      {
        if (pos>elements)
        {
           mafLogMessage("mafHTMLTemplateParserBlock: Block: '%s' wrong pos!",name.c_str());
           return NULL;
        }
        return m_SubBlocksArray[i][pos];
      }
      else 
      { 
        return m_SubBlocksArray[i][elements];
      }
    }
  }

  //block not found
  mafLogMessage("mafHTMLTemplateParserBlock: Block: '%s' not found!",name.c_str());
  return NULL;

}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlock::SetIfCondition( int condition )
//----------------------------------------------------------------------------
{
  //checking block type
  if (m_BlockType!=maf_HTML_TEMPLATE_IF)
  {
    mafLogMessage("HTML Template ERROR: You can set the value of if condition only on 'if' blocks");
    return;
  }
  m_IfCondition=(bool)condition;
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlock::SetNLoops( int nloops )
//----------------------------------------------------------------------------
{
  //checking block type
  if (m_BlockType!=maf_HTML_TEMPLATE_LOOP)
  {
    mafLogMessage("HTML Template ERROR: You can set the loops number only on 'loops' blocks");
    return;
  }
  //checking nloops is an acceptable value
  else if (nloops<0)
  {
    mafLogMessage("HTML Template ERROR: nloops must be at the least zero");
    return;
  }
  m_LoopsNumber=nloops;
}

//----------------------------------------------------------------------------
void mafHTMLTemplateParserBlock::SetFather( mafHTMLTemplateParserBlock *father )
//----------------------------------------------------------------------------
{

  //if block type is maf_HTML_TEMPLATE_MAIN this is the root of the tree
  //so we cannot set a father
  if (m_BlockType==maf_HTML_TEMPLATE_MAIN)
  {
    mafLogMessage("HTML Template ERROR: The main Block does not have a father");
  }
  else 
    m_Father=father;

}

wxString mafHTMLTemplateParserBlock::GetVar( wxString name )
{

  //getting substitution pos
  int pos = SubstitutionPos(&name);
  
  //if pos < 0 the substitution was not found
  if (pos<0)
  {
    mafLogMessage("mafHTMLTemplateParserBlock: Variable: '%s' not found!",name.c_str());
    return "Error";
  }

  //we need to check the substitution type
  else if (m_SubstitutionTable[pos].Type!=maf_HTML_SUBSTITUTION_VARIABLE)
  {
    //block not found
    mafLogMessage("mafHTMLTemplateParserBlock: Variable: '%s' wrong type!",name.c_str());
    return "Error";
  }
  //Return the variable
  else 
  {
    return m_Variables[ m_SubstitutionTable[pos].Pos ];
  }

}

wxString mafHTMLTemplateParserBlock::GetNthVar( wxString name, int arrayPos/*=-1*/ )
{

  
  //getting substitution pos
  int pos = SubstitutionPos(&name);

  //if pos < 0 the substitution was not found
  if (pos<0)
  {
    mafLogMessage("mafHTMLTemplateParserBlock: Variable Array: '%s' not found!",name.c_str());
    return "Error";
  }

  //we need to check the substitution type
  else if (m_SubstitutionTable[pos].Type!=maf_HTML_SUBSTITUTION_VARIABLE_ARRAY)
  {
    //block not found
    mafLogMessage("mafHTMLTemplateParserBlock: Variable Array: '%s' wrong type!",name.c_str());
    return "Error";
  }

  //Return the variable
  else 
  {
    
    int arraySize= m_VariablesArray[ m_SubstitutionTable[pos].Pos ].size();

    if (arrayPos>arraySize-1)
    {
      //block not found
      mafLogMessage("mafHTMLTemplateParserBlock: Variable Array: '%s', there are few element in the array",name.c_str());
      return "Error";
    }
    
    //if array pos is -1 last element will be returned
    if (arrayPos==-1)
    {
      arrayPos=arraySize-1;
    }

    return m_VariablesArray[ m_SubstitutionTable[pos].Pos ][arrayPos];
  }

}

int mafHTMLTemplateParserBlock::GetIfCondition()
{
  //checking block type
  if (m_BlockType!=maf_HTML_TEMPLATE_IF)
  {
    mafLogMessage("HTML Template ERROR: You can get the value of if condition only on 'if' blocks");
    return false;
  }
  return m_IfCondition;
}

int mafHTMLTemplateParserBlock::GetNLoops()
{
  //checking block type
  if (m_BlockType!=maf_HTML_TEMPLATE_LOOP)
  {
    mafLogMessage("HTML Template ERROR: You can get the loops number only on 'loops' blocks");
    return -1;
  }
  
  return m_LoopsNumber;
}

//----------------------------------------------------------------------------
wxString mafHTMLTemplateParserBlock::CalculateImageRTF(mafView *view, wxString imagePath)
{
	int widthGoal = 8640;
	int heightGoal = 4680;

	if (imagePath == "")
	{
		// Write Image
		wxString logPath = mafGetAppDataDirectory().c_str();
		imagePath = logPath + "\\imm.jpg";

		view->CameraUpdate();
		view->GetRWI()->Update();
		view->GetRWI()->SaveImage(imagePath);

		int width = view->GetRWI()->m_Width;
		int height = view->GetRWI()->m_Height;

		int newWidthGoal = (heightGoal / height) * width;

		if (newWidthGoal > widthGoal)
		{
			heightGoal = (widthGoal / width) * height;
		}
		else
		{
			widthGoal = newWidthGoal;
		}
	}

	// Read Image
	wxString imageStr = "", byteStr = "";

	FILE *imageFile;

	imageFile = fopen(imagePath, "rb");

	if (imageFile == NULL) {/*ERROR*/ }

	BYTE bytebuf;

	int nread = 1;
	int lineCounter = 0;

	while (nread)
	{
		nread = fread(&bytebuf, sizeof(BYTE), 1, imageFile);
		if (nread > 0)
		{
			byteStr.Printf("%02x", bytebuf);
			lineCounter++;
			imageStr += byteStr;
			if (lineCounter == 80)
			{
				imageStr += "\n";
				lineCounter = 0;
			}
		}
	}

	wxString mpic;
	mpic.Printf("\n\\qc{\\pict\\jpegblip\\picwgoal%d\\pichgoal%d\\bin\n%s\n}", widthGoal, heightGoal, imageStr);

	return mpic;
}