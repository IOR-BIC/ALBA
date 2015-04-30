/*=========================================================================

 Program: MAF2
 Module: mafHTMLTemplateParser
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafHTMLTemplateParser_H__
#define __mafHTMLTemplateParser_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafHTMLTemplateParserBlock.h"

/**
 class name: mafHTMLTemplateParser
    class for Template parsing of a text file/string (typically an html)
    with management of Variables/Loop/If-Else condition
    Tags Are: 
    [MAFVariable varName]
    [MAFLoop loopName] [/MAFLoop loopName] 
    [MAFIf ifName] [MAFElse ifName] [/MAFIf ifName] (else is optional)*
*/
class MAF_EXPORT mafHTMLTemplateParser : public mafHTMLTemplateParserBlock
{
public:
  
  /** constructor*/
  mafHTMLTemplateParser();
  /** destructor*/
  ~mafHTMLTemplateParser(); 

  /** Set the template from a file */
  void SetTemplateFromFile(wxString filename);
  /** Set the template from a string */
  void SetTemplateFromString(wxString templateString);
  /** Return the Parsed output */
  wxString GetOutputString();
  /** Writes the output to disk */
  void WriteOutputFile(wxString filename);
  
  /** Template parsing   
      Updates all the structures and generate internal output 
      Note: this function must be called before GetOutputString() or 
      WriteOutput() functions*/
  void Parse();
  
protected:
  //mafHTMLTemplateParserBlock *m_MainBlock;
  wxString m_Template;
  wxString m_Output;
};
#endif
