/*=========================================================================

 Program: MAF2
 Module: mmuDOMTreeErrorReporter
 Authors: Marco Petrone m.petrone@cineca.it
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "mafDefines.h"
//#include "mafIncludeWX.h" // to be removed

#include "mmuDOMTreeErrorReporter.h"
#include "mafXMLString.h"


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
    mafErrorMessageMacro("Error at file \"" << toCatch.getSystemId() \
      << "\", line " << toCatch.getLineNumber() \
      << ", column " << toCatch.getColumnNumber() \
      << "\n   Message: " << mafXMLString(toCatch.getMessage()) \
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
    mafErrorMessageMacro("Fatal Error at file \"" << mafXMLString(toCatch.getSystemId()) \
	  << "\", line " << toCatch.getLineNumber() \
	  << ", column " << toCatch.getColumnNumber() \
    << "\n   Message: " << mafXMLString(toCatch.getMessage()) \
    );
  }
  
}
//------------------------------------------------------------------------------
void mmuDOMTreeErrorReporter::resetErrors()
//------------------------------------------------------------------------------
{
  m_SawErrors = false;
}
