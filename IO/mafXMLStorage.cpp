/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafXMLStorage.cpp,v $
  Language:  C++
  Date:      $Date: 2005-01-28 13:58:37 $
  Version:   $Revision: 1.6 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafXMLStorage.h"
#include "mafXMLElement.h"
#include "mafXMLString.h"
#include "mafStorable.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sstream>
// Xerces-C specific
#include "mmuXMLDOM.h"
#include "mmuXMLDOMElement.h"
#include <xercesc/framework/LocalFileInputSource.hpp>
// required by error handlers
//#include <xercesc/dom/DOMErrorHandler.hpp>
//#include <xercesc/dom/DOMError.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include <assert.h>

//------------------------------------------------------------------------------
// mmuDOMTreeErrorReporter
//------------------------------------------------------------------------------
/** Utility class to catch XML error rised while parsing XML */
class mmuDOMTreeErrorReporter : public ErrorHandler
{
public:
    mmuDOMTreeErrorReporter() : m_SawErrors(false) {}
    ~mmuDOMTreeErrorReporter() {}

    /** Implementation of the error handler interface */
    void warning(const SAXParseException& toCatch);
    void error(const SAXParseException& toCatch);
    void fatalError(const SAXParseException& toCatch);
    void resetErrors();

    bool GetSawErrors() const {return m_SawErrors;}
private:
    bool    m_SawErrors; ///< Set if we get any errors, used by the main code to suppress output if there are errors.
};

//------------------------------------------------------------------------------
void mmuDOMTreeErrorReporter::warning(const SAXParseException&)
//------------------------------------------------------------------------------
{
  // Ignore all warnings.
}
//------------------------------------------------------------------------------
void mmuDOMTreeErrorReporter::error(const SAXParseException& toCatch)
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
void mmuDOMTreeErrorReporter::fatalError(const SAXParseException& toCatch)
//------------------------------------------------------------------------------
{
  m_SawErrors = true;
  mafString err;
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

//------------------------------------------------------------------------------
// mafXMLStorage
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafXMLStorage::mafXMLStorage()
//------------------------------------------------------------------------------
{
  m_DOM = new mmuXMLDOM;
  m_Version = "1.1";
}

//------------------------------------------------------------------------------
mafXMLStorage::~mafXMLStorage()
//------------------------------------------------------------------------------
{
  cppDEL(m_DOM);
}

//------------------------------------------------------------------------------
bool mafXMLStorage::ResolveInputURL(const mafString &url, mafString &filename)
//------------------------------------------------------------------------------
{
  // currently no URL support
  filename=url;
  return true;
}
//------------------------------------------------------------------------------
bool mafXMLStorage::ResolveOutputURL(const mafString &url, mafString &filename)
//------------------------------------------------------------------------------
{
  // currently no URL support
  filename=url;
  return true;
}
//------------------------------------------------------------------------------
void mafXMLStorage::SetFileType(const char *filetype)
//------------------------------------------------------------------------------
{
  m_FileType.Copy(filetype); // force string copying
}

//------------------------------------------------------------------------------
const char *mafXMLStorage::GetFileType()
//------------------------------------------------------------------------------
{
  return m_FileType;
}

//------------------------------------------------------------------------------
void mafXMLStorage::SetVersion(const char *version)
//------------------------------------------------------------------------------
{
  m_Version.Copy(version); // force string copying
}
//------------------------------------------------------------------------------
const char *mafXMLStorage::GetVersion()
//------------------------------------------------------------------------------
{
  return m_Version;
}

//------------------------------------------------------------------------------
int mafXMLStorage::InternalStore()
//------------------------------------------------------------------------------
{
  int errorCode=0;
  // initialize the XML library
  try
  {
      XMLPlatformUtils::Initialize();
  }

  catch(const XMLException& toCatch)
  {
      mafErrorMessageMacro("Error during Xerces-C Initialization.\nException message:" << mafXMLString(toCatch.getMessage()));      
      return MAF_ERROR;
  }

  // get a serializer, an instance of DOMWriter (the "LS" stands for load-save).
  m_DOM->m_XMLImplement = DOMImplementationRegistry::getDOMImplementation(mafXMLString("LS"));

  if (m_DOM->m_XMLImplement)
  {
    m_DOM->m_XMLSerializer = ( (DOMImplementationLS*)m_DOM->m_XMLImplement )->createDOMWriter();

    mafString filename;
    if (ResolveOutputURL(m_URL,filename))
    {
      m_DOM->m_XMLTarget = new LocalFileFormatTarget(filename);

      // set user specified end of line sequence and output encoding
      m_DOM->m_XMLSerializer->setNewLine( mafXMLString("\r") );

      // set serializer features 
 	    m_DOM->m_XMLSerializer->setFeature(XMLUni::fgDOMWRTSplitCdataSections, false);
  	  m_DOM->m_XMLSerializer->setFeature(XMLUni::fgDOMWRTDiscardDefaultContent, false);
  	  m_DOM->m_XMLSerializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
  	  m_DOM->m_XMLSerializer->setFeature(XMLUni::fgDOMWRTBOM, false);

      try
      {
        // create a document
        m_DOM->m_XMLDoc = m_DOM->m_XMLImplement->createDocument( NULL, mafXMLString(m_FileType), NULL ); // NO URI and NO DTD
        if (m_DOM->m_XMLDoc)
        {
          // output related nodes are prefixed with "svg"
          // to distinguish them from input nodes.
	        m_DOM->m_XMLDoc->setEncoding( mafXMLString("UTF-8") );
	        m_DOM->m_XMLDoc->setStandalone(true);
	        m_DOM->m_XMLDoc->setVersion( mafXMLString("1.0") );

          // extract root element and wrap it with an mafXMLElement object
          DOMElement *root = m_DOM->m_XMLDoc->getDocumentElement();
          assert(root);
          m_RootElement = new mafXMLElement(new mmuXMLDOMElement(root),NULL,this);

          // attach version attribute to the root node
          m_RootElement->SetAttribute("Version",m_Version);
        
          // call Store function of the m_Root object. The root is passed
          // as parent the DOM root element. A tree root is usually a special
          // kind of object and can decide to store itself in the root
          // object itself, or below it as it happens for other nodes.
          assert(m_Root);
          m_Root->Store(m_RootElement);

          // write the tree to disk
          m_DOM->m_XMLSerializer->writeNode(m_DOM->m_XMLTarget, *(m_DOM->m_XMLDoc));

          // destroy all intermediate objects
          cppDEL (m_RootElement);  
          cppDEL (m_DOM->m_XMLDoc);
        }    
      }
      catch (const DOMException& e)
      {
        mafErrorMessageMacro( "XML error, DOMException code is:  " << e.code );
        errorCode = 2;
      }
      catch (...)
      {
         mafErrorMessage("XML error, an error occurred creating the XML document!");
         errorCode = 3;
      }

      cppDEL (m_DOM->m_XMLTarget);
      cppDEL (m_DOM->m_XMLSerializer);
      cppDEL (m_DOM->m_XMLDoctype);
    }
    else 
    {
      mafErrorMessage("Unable to resolve URL for output XML file");
      errorCode = 4;
    }
    
  }
  else
  {
    // implementation retrieve failed
    mafErrorMessage("Requested XML implementation is not supported");
    errorCode = 1;
  }
 
  // terminate the XML library
  XMLPlatformUtils::Terminate();

  return errorCode;
}

//------------------------------------------------------------------------------
int mafXMLStorage::InternalRestore()
//------------------------------------------------------------------------------
{
  assert (m_Root);

  if (!m_Root)
    return MAF_ERROR;
  
  int errorCode=0;
  
  // initialize the XML library
  try
  {
      XMLPlatformUtils::Initialize();
  }

  catch(const XMLException& toCatch)
  {
    mafErrorMessageMacro( "Error during Xerces-C Initialization.\nException message:" <<mafXMLString(toCatch.getMessage()));
    return MAF_ERROR;
  }
  
  //
  //  Create our parser, then attach an error handler to the parser.
  //  The parser will call back to methods of the ErrorHandler if it
  //  discovers errors during the course of parsing the XML document.
  //
  m_DOM->m_XMLParser = new XercesDOMParser;

  if (m_DOM->m_XMLParser)
  {
    m_DOM->m_XMLParser->setValidationScheme(XercesDOMParser::Val_Auto);
    m_DOM->m_XMLParser->setDoNamespaces(false);
    m_DOM->m_XMLParser->setDoSchema(false);
    m_DOM->m_XMLParser->setCreateEntityReferenceNodes(false);

    mmuDOMTreeErrorReporter *errReporter = new mmuDOMTreeErrorReporter();
    m_DOM->m_XMLParser->setErrorHandler(errReporter);

    // here I should resolve the XML file name
    mafString filename;
    if (ResolveInputURL(m_ParserURL,filename))
    {
      int errorCode  = 0;

      try
      {
        m_DOM->m_XMLParser->parse(filename);
        int errorCount = m_DOM->m_XMLParser->getErrorCount(); 

        if (errorCount!=0)
        {
          // errors while parsing...
          mafErrorMessage("Errors while parsing XML file");
          errorCode = 1;
        }
        else
        {
          // extract the root element and wrap inside a mafXMLElement
          m_DOM->m_XMLDoc = m_DOM->m_XMLParser->getDocument();
          DOMElement *root = m_DOM->m_XMLDoc->getDocumentElement();
          assert(root);
          m_RootElement = new mafXMLElement(new mmuXMLDOMElement(root),NULL,this);

          mafString doc_version;
          if (m_RootElement->GetAttribute("Version",doc_version))
          {
            double doc_version_f=atof(doc_version);
            double my_version_f=atof(m_Version);
            
            if (my_version_f<=doc_version_f)
            {
              // Start tree restoring from root node
              if (m_Root->Restore(m_RootElement)!=MAF_OK)
                errorCode=6;
            }
            else
            {
              mafErrorMacro("XML parsing error: wrong file version ("<<doc_version<<"), should be > "<<m_Version);
              errorCode=7;
            }
          }
          

          // destroy the root XML element
          cppDEL (m_RootElement);
        }
      }

      catch (const XMLException& e)
      {
        mafString err;
        err << "An error occurred during XML parsing.\n Message: " << mafXMLString(e.getMessage());
        mafErrorMessage(err);
        errorCode = 2;
      }

      catch (const DOMException& e)
      { 
        mafString err;
        err << "DOM-XML Error while parsing file '" << m_ParserURL << "'\n";
        err << "DOMException code is: " << mafString(e.code);

        if (e.getMessage())
          err << "DOMException msg is: " << mafXMLString(e.getMessage());
      
        mafErrorMessage(err);
        errorCode = 3;
      }

      /*catch (const SAXException& e)
      {
        mafString err;
        err << "SAX-XML Error while parsing file: '" << m_ParserURL << "'\n";
        err << "SAXException msg is: " << mafXMLString(e.getMessage());
        mafErrorMessage(err);
      }*/

      catch (...)
      {
        mafErrorMessage("An error occurred during XML parsing");
        errorCode = 4;
      }
    }
    else
    {
      mafErrorMessage("Unable to resolve URL for input XML file");
    }

    cppDEL (errReporter);
    cppDEL (m_DOM->m_XMLParser);
  }
  else
  {
    // parser allocation error
    mafErrorMessage("Failed to allocate XML parser");
    errorCode = 5;
  }

  // terminate the XML library
  XMLPlatformUtils::Terminate();

  return errorCode;
}
