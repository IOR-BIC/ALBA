/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medHTMLTemplateParser.h,v $
Language:  C++
Date:      $Date: 2012-04-06 08:17:58 $
Version:   $Revision: 1.1.2.6 $
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

#ifndef __medHTMLTemplateParser_H__
#define __medHTMLTemplateParser_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medCommonDefines.h"
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
class MED_COMMON_EXPORT medHTMLTemplateParser : public medHTMLTemplateParserBlock
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
