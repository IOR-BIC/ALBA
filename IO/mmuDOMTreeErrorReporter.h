/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmuDOMTreeErrorReporter.h,v $
  Language:  C++
  Date:      $Date: 2009-02-23 14:47:48 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmuDOMTreeErrorReporter_h__
#define __mmuDOMTreeErrorReporter_h__

//------------------------------------------------------------------------------
// Include:
//------------------------------------------------------------------------------
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>


//------------------------------------------------------------------------------
// mmuDOMTreeErrorReporter
//------------------------------------------------------------------------------
/** Utility class to catch XML error rised while parsing XML */
class mmuDOMTreeErrorReporter : public XERCES_CPP_NAMESPACE_QUALIFIER ErrorHandler
{
public:
    mmuDOMTreeErrorReporter() : m_SawErrors(false) {}
    ~mmuDOMTreeErrorReporter() {}

    /** Implementation of the error handler interface */
    void warning(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch);
    void error(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch);
    void fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch);
    void resetErrors();

    bool GetSawErrors() const {return m_SawErrors;}
private:
    bool    m_SawErrors; ///< Set if we get any errors, used by the main code to suppress output if there are errors.
};
#endif // _mmuDOMTreeErrorReporter_h_
