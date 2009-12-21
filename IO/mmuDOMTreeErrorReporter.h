/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmuDOMTreeErrorReporter.h,v $
  Language:  C++
  Date:      $Date: 2009-12-21 15:39:30 $
  Version:   $Revision: 1.1.2.2 $
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


/** 
class name: mmuDOMTreeErrorReporter
  Utility class to catch XML error rised while parsing XML 
*/

class mmuDOMTreeErrorReporter : public XERCES_CPP_NAMESPACE_QUALIFIER ErrorHandler
{
public:
    /** constructor */
    mmuDOMTreeErrorReporter() : m_SawErrors(false), m_TestFlag(false) {}
    /** destructor */
    ~mmuDOMTreeErrorReporter() {}

    /** Implementation of the warning handler interface */
    void warning(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch);
    /** Implementation of the error handler interface */
    void error(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch);
    /** Implementation of the fatal error handler interface */
    void fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch);
    /** reset error flag */
    void resetErrors();

    /** retrieve error flag */
    bool GetSawErrors() const {return m_SawErrors;}
    /** set test modality in order to skip, problematic log messages*/
    void SetTestMode(bool enable){m_TestFlag = enable;}

private:
    bool    m_SawErrors; ///< Set if we get any errors, used by the main code to suppress output if there are errors.
    bool m_TestFlag;
};
#endif // _mmuDOMTreeErrorReporter_h_
