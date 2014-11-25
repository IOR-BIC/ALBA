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

#ifndef __medHTMLTemplateParser_H__
#define __medHTMLTemplateParser_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "medHTMLTemplateParserBlock.h"

/**
 class name: medHTMLTemplateParser
    class for Template parsing of a text file/string (typically an html)
    with management of Variables/Loop/If-Else condition
    Tags Are: 
    [MAFVariable varName]
    [MAFLoop loopName] [/MAFLoop loopName] 
    [MAFIf ifName] [MAFElse ifName] [/MAFIf ifName] (else is optional)*
*/
class MAF_EXPORT medHTMLTemplateParser : public medHTMLTemplateParserBlock
{
public:
  
  /** constructor*/
  medHTMLTemplateParser();
  /** destructor*/
  ~medHTMLTemplateParser(); 

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
  //medHTMLTemplateParserBlock *m_MainBlock;
  wxString m_Template;
  wxString m_Output;
};
#endif
