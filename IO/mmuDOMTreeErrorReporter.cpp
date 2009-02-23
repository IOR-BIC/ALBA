/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmuDOMTreeErrorReporter.cpp,v $
  Language:  C++
  Date:      $Date: 2009-02-23 14:47:48 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
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
  mafErrorMessageMacro("Error at file \"" << toCatch.getSystemId() \
	 << "\", line " << toCatch.getLineNumber() \
	 << ", column " << toCatch.getColumnNumber() \
   << "\n   Message: " << mafXMLString(toCatch.getMessage()) \
  );
}
//------------------------------------------------------------------------------
void mmuDOMTreeErrorReporter::fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch)
//------------------------------------------------------------------------------
{
  m_SawErrors = true;
  mafErrorMessageMacro("Fatal Error at file \"" << mafXMLString(toCatch.getSystemId()) \
	 << "\", line " << toCatch.getLineNumber() \
	 << ", column " << toCatch.getColumnNumber() \
   << "\n   Message: " << mafXMLString(toCatch.getMessage()) \
  );
  
}
//------------------------------------------------------------------------------
void mmuDOMTreeErrorReporter::resetErrors()
//------------------------------------------------------------------------------
{
  m_SawErrors = false;
}
