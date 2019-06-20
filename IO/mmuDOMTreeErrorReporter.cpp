/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmuDOMTreeErrorReporter
 Authors: Marco Petrone m.petrone@cineca.it
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"
//#include "albaIncludeWX.h" // to be removed

#include "mmuDOMTreeErrorReporter.h"
#include "albaXMLString.h"


#include <assert.h>


//------------------------------------------------------------------------------
void mmuDOMTreeErrorReporter::warning(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException&)
//------------------------------------------------------------------------------
{
  // Ignore all warnings.
}
//------------------------------------------------------------------------------
void mmuDOMTreeErrorReporter::error(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch)
//------------------------------------------------------------------------------
{
  m_SawErrors = true;
  if(m_TestFlag == false)
  {
    albaErrorMessageMacro("Error at file \"" << toCatch.getSystemId() \
      << "\", line " << toCatch.getLineNumber() \
      << ", column " << toCatch.getColumnNumber() \
      << "\n   Message: " << albaXMLString(toCatch.getMessage()) \
      );
  }
}
//------------------------------------------------------------------------------
void mmuDOMTreeErrorReporter::fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch)
//------------------------------------------------------------------------------
{
  m_SawErrors = true;
  if(m_TestFlag == false)
  {
    albaErrorMessageMacro("Fatal Error at file \"" << albaXMLString(toCatch.getSystemId()) \
	  << "\", line " << toCatch.getLineNumber() \
	  << ", column " << toCatch.getColumnNumber() \
    << "\n   Message: " << albaXMLString(toCatch.getMessage()) \
    );
  }
  
}
//------------------------------------------------------------------------------
void mmuDOMTreeErrorReporter::resetErrors()
//------------------------------------------------------------------------------
{
  m_SawErrors = false;
}
