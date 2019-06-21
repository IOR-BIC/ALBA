/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaHTMLTemplateParser
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaHTMLTemplateParser_H__
#define __albaHTMLTemplateParser_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaHTMLTemplateParserBlock.h"

/**
 class name: albaHTMLTemplateParser
    class for Template parsing of a text file/string (typically an html)
    with management of Variables/Loop/If-Else condition
    Tags Are: 
    [ALBAVariable varName]
    [ALBALoop loopName] [/ALBALoop loopName] 
    [ALBAIf ifName] [ALBAElse ifName] [/ALBAIf ifName] (else is optional)*
*/
class ALBA_EXPORT albaHTMLTemplateParser : public albaHTMLTemplateParserBlock
{
public:
  
  /** constructor*/
  albaHTMLTemplateParser();
  /** destructor*/
  ~albaHTMLTemplateParser(); 

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
  //albaHTMLTemplateParserBlock *m_MainBlock;
  wxString m_Template;
  wxString m_Output;
};
#endif
