/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafXMLStorage.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-11 11:25:08 $
  Version:   $Revision: 1.20 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

//#include "mafIncludeWX.h" // to be removed

#include "mafXMLStorage.h"
#include "mafXMLElement.h"
#include "mafXMLString.h"
#include "mafStorable.h"
#include "mafDirectory.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
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

#ifndef MAF_USE_WX
#error "XML Storage cannot be compiled without wxWidgets"
#endif

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
mafCxxTypeMacro(mafXMLStorage)
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
const char* mafXMLStorage::GetTmpFolder()
//------------------------------------------------------------------------------
{
  if (m_TmpFolder.IsEmpty())
  {
    wxString path=wxPathOnly(m_URL.GetCStr());
    if (!path.IsEmpty())
    {
      m_DefaultTmpFolder=path;
      m_DefaultTmpFolder<<"/";
    }
    else
    {
      m_DefaultTmpFolder="";
    }
    
    return m_DefaultTmpFolder;
  }
  else
  {
    return Superclass::GetTmpFolder();
  }
}

//----------------------------------------------------------------------------
int mafXMLStorage::OpenDirectory(const char *pathname)
//----------------------------------------------------------------------------
{
  mafDirectory dir;
  if (mafString::IsEmpty(pathname))
  {
    if (!dir.Load("."))
      return MAF_ERROR;
  }
  else
  {
    if (!dir.Load(pathname))
      return MAF_ERROR;
  }
  
  m_FilesDictionary.clear();

  for (int i=0;i<dir.GetNumberOfFiles();i++)
  {
    const char *fullname=dir.GetFile(i);  
    const char *filename=mafString::BaseName(fullname);
    m_FilesDictionary.insert(filename);
  }

  return MAF_OK;
}

//------------------------------------------------------------------------------
void mafXMLStorage::SetURL(const char *name)
//------------------------------------------------------------------------------
{
  if (m_URL!=name)
  {
    // when saving to a new file or loading a different file
    // simply clear the list of URLs to be released.
    m_GarbageCollector.clear();
    Superclass::SetURL(name);
  }
}

//------------------------------------------------------------------------------
int mafXMLStorage::ResolveInputURL(const char * url, mafString &filename, mafObserver *observer)
//------------------------------------------------------------------------------
{
  // currently no real URL support
  wxString path;
  path=wxPathOnly(url);
  if (path.IsEmpty())
  {
    wxString base_path;
    base_path=wxPathOnly(m_ParserURL.GetCStr());

    filename=base_path;
    
    if (!base_path.IsEmpty())
      filename<<"/";

    filename<<url;
  }
  else
  {
    filename=url;
  }
  
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafXMLStorage::StoreToURL(const char * filename, const char * url)
//------------------------------------------------------------------------------
{
  assert(url); // NULL url not yet supported

  // currently no real URL support
  wxString path;
  path=wxPathOnly(url);

  if (path.IsEmpty())
  {
    // if local file prepend base_path
    wxString base_path,fullpathname;
    base_path=wxPathOnly(m_URL.GetCStr());
    if (!base_path.IsEmpty())
    {
      fullpathname=base_path+"/"+url;
    }
    else
    {
      fullpathname=url;
    }
    
    if (IsFileInDirectory(url)) // IsFileInDirectory accepts URL specifications
    {
      // remove old file if present
      DeleteURL(url);
    }

    // currently only local files are supported
    return wxRenameFile(filename,fullpathname)?MAF_OK:MAF_ERROR;
  }
  else
  {
    // remove old file if present
    wxRemoveFile(url);
    // currently only local files are supported
    return wxRenameFile(filename,url)?MAF_OK:MAF_ERROR;
  } 
}

//------------------------------------------------------------------------------
int mafXMLStorage::ReleaseURL(const char *url)
//------------------------------------------------------------------------------
{
  // add to list of files to be deleted
  m_GarbageCollector.insert(url);
  return MAF_OK;
}


//------------------------------------------------------------------------------
int mafXMLStorage::DeleteURL(const char *url)
//------------------------------------------------------------------------------
{
  // currently no real URL support
  wxString path;
  path=wxPathOnly(url);

  if (path.IsEmpty())
  {
    // if local file prepend base_path
    wxString base_path,fullpathname;
    base_path=wxPathOnly(m_URL.GetCStr());
    fullpathname=base_path+"/"+url;

    if (IsFileInDirectory(url))
    {
      // remove old file if present
      wxRemoveFile(fullpathname);
      return MAF_OK;
    }

    return MAF_ERROR;
  }
  else
  {
    return (wxRemoveFile(url)?MAF_OK:MAF_ERROR);
  }

}


//------------------------------------------------------------------------------
void mafXMLStorage::SetFileType(const char *filetype)
//------------------------------------------------------------------------------
{
  m_FileType=filetype; // force string copying
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
  m_Version=version; // force string copying
}
//------------------------------------------------------------------------------
const char *mafXMLStorage::GetVersion()
//------------------------------------------------------------------------------
{
  return m_Version;
}

//------------------------------------------------------------------------------
void mafXMLStorage::EmptyGarbageCollector()
//------------------------------------------------------------------------------
{
  for (std::set<mafString>::iterator it=m_GarbageCollector.begin();it!=m_GarbageCollector.end();it++)
  {
    DeleteURL(*it);
  }
  m_GarbageCollector.clear();
}
//------------------------------------------------------------------------------
int mafXMLStorage::InternalStore()
//------------------------------------------------------------------------------
{
  int errorCode=0;
  // initialize the XML library
  try
  {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
  }

  catch(const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch)
  {
      mafErrorMessageMacro("Error during Xerces-C Initialization.\nException message:" << mafXMLString(toCatch.getMessage()));      
      return MAF_ERROR;
  }

  // get a serializer, an instance of DOMWriter (the "LS" stands for load-save).
  m_DOM->m_XMLImplement = XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationRegistry::getDOMImplementation(mafXMLString("LS"));

  if (m_DOM->m_XMLImplement)
  {
    m_DOM->m_XMLSerializer = ( (XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationLS*)m_DOM->m_XMLImplement )->createDOMWriter();

    mafString filename;

    // initially store to a tmp file
    GetTmpFile(filename);

    m_DOM->m_XMLTarget = new XERCES_CPP_NAMESPACE_QUALIFIER LocalFileFormatTarget(filename);

    // set user specified end of line sequence and output encoding
    m_DOM->m_XMLSerializer->setNewLine( mafXMLString("\r") );

    // set serializer features 
 	  m_DOM->m_XMLSerializer->setFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTSplitCdataSections, false);
  	m_DOM->m_XMLSerializer->setFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTDiscardDefaultContent, false);
  	m_DOM->m_XMLSerializer->setFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true);
  	m_DOM->m_XMLSerializer->setFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTBOM, false);

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
        XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *root = m_DOM->m_XMLDoc->getDocumentElement();
        assert(root);
        m_DocumentElement = new mafXMLElement(new mmuXMLDOMElement(root),NULL,this);

        // attach version attribute to the root node
        m_DocumentElement->SetAttribute("Version",m_Version);
      
        // call Store function of the m_Document object. The root is passed
        // as parent the DOM root element. A tree root is usually a special
        // kind of object and can decide to store itself in the root
        // object itself, or below it as it happens for other nodes.
        assert(m_Document);
        m_Document->Store(m_DocumentElement);

        // write the tree to disk
        m_DOM->m_XMLSerializer->writeNode(m_DOM->m_XMLTarget, *(m_DOM->m_XMLDoc));

        // destroy all intermediate objects
        cppDEL (m_DocumentElement);  
        cppDEL (m_DOM->m_XMLTarget);
        cppDEL (m_DOM->m_XMLDoc);
        errorCode=0;
      }    
    }
    catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException& e)
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

    // move to destination URL
    if (errorCode==0)
    {
      if (StoreToURL(filename,m_URL)!=MAF_OK)
      {
        mafErrorMessage("Unable to resolve URL for output XML file, a copy of the file can be found in: %s",filename.GetCStr());
        errorCode = 4;
      }
      else
      {
        //
        // clean the storage file directory
        //

        ReleaseTmpFile(filename); // remove the storage tmp file

        EmptyGarbageCollector();
      }
    }    
  }
  else
  {
    // implementation retrieve failed
    mafErrorMessage("Requested XML implementation is not supported");
    errorCode = 1;
  }
 
  // terminate the XML library
  XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();

  return errorCode;
}

//------------------------------------------------------------------------------
int mafXMLStorage::InternalRestore()
//------------------------------------------------------------------------------
{
  assert (m_Document);

  if (!m_Document)
    return MAF_ERROR;
  
  int errorCode=0;
  
  // initialize the XML library
  try
  {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
  }

  catch(const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch)
  {
    mafErrorMessageMacro( "Error during Xerces-C Initialization.\nException message:" <<mafXMLString(toCatch.getMessage()));
    return MAF_ERROR;
  }
  
  //
  //  Create our parser, then attach an error handler to the parser.
  //  The parser will call back to methods of the ErrorHandler if it
  //  discovers errors during the course of parsing the XML document.
  //
  m_DOM->m_XMLParser = new XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser;

  if (m_DOM->m_XMLParser)
  {
    m_DOM->m_XMLParser->setValidationScheme(XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser::Val_Auto);
    m_DOM->m_XMLParser->setDoNamespaces(false);
    m_DOM->m_XMLParser->setDoSchema(false);
    m_DOM->m_XMLParser->setCreateEntityReferenceNodes(false);

    mmuDOMTreeErrorReporter *errReporter = new mmuDOMTreeErrorReporter();
    m_DOM->m_XMLParser->setErrorHandler(errReporter);

    // here I should resolve the XML file name
    mafString filename;
    if (ResolveInputURL(m_ParserURL,filename) != MAF_ERROR)
    {
      int errorCode  = 0;

      try
      {
        m_DOM->m_XMLParser->parse(filename);
        int errorCount = m_DOM->m_XMLParser->getErrorCount(); 

        if (errorCount != 0)
        {
          // errors while parsing...
          mafErrorMessage("Errors while parsing XML file");
          errorCode = IO_XML_PARSE_ERROR;
        }
        else
        {
          // extract the root element and wrap inside a mafXMLElement
          m_DOM->m_XMLDoc = m_DOM->m_XMLParser->getDocument();
          XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *root = m_DOM->m_XMLDoc->getDocumentElement();
          assert(root);
          m_DocumentElement = new mafXMLElement(new mmuXMLDOMElement(root),NULL,this);

          if (m_FileType == m_DocumentElement->GetName())
          {
            mafString doc_version;
            if (m_DocumentElement->GetAttribute("Version",doc_version))
            {
              double doc_version_f = atof(doc_version);
              double my_version_f = atof(m_Version);
            
              if (my_version_f <= doc_version_f)
              {
                // Start tree restoring from root node
                if (m_Document->Restore(m_DocumentElement) != MAF_OK)
                  errorCode = IO_RESTORE_ERROR;
              }
              else
              {
                // Paolo 30-11-2007: due to changes on name for mafVMEScalar (to mafVMEScalarMatrix)
                if (doc_version_f < 2.0)
                {
                  mafErrorMacro("XML parsing error: wrong file version v"<<doc_version.GetCStr()<<", should be > v"<<m_Version.GetCStr());
                  errorCode = IO_WRONG_FILE_VERSION;
                }
                else
                {
                  // Upgrade document to the actual version
                  m_DocumentElement->SetAttribute("Version", my_version_f);
                  m_NeedsUpgrade = true;
                  if (m_Document->Restore(m_DocumentElement) != MAF_OK)
                    errorCode = IO_RESTORE_ERROR;
                }
              }
            }
          }
          else
          {
            mafErrorMacro("XML parsing error: wrong file type, expected \""<<m_FileType<<"\", found "<<m_DocumentElement->GetName());
            errorCode = IO_WRONG_FILE_TYPE;
          }
          
          // destroy the root XML element
          cppDEL(m_DocumentElement);
        }
      }

      catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& e)
      {
        mafString err;
        err << "An error occurred during XML parsing.\n Message: " << mafXMLString(e.getMessage());
        mafErrorMessage(err);
        errorCode = IO_XML_PARSE_ERROR;
      }

      catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException& e)
      { 
        mafString err;
        err << "DOM-XML Error while parsing file '" << m_ParserURL << "'\n";
        err << "DOMException code is: " << mafString(e.code);

        if (e.getMessage())
          err << "DOMException msg is: " << mafXMLString(e.getMessage());
      
        mafErrorMessage(err);
        errorCode = IO_DOM_XML_ERROR;
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
        errorCode = IO_XML_PARSE_ERROR;
      }
    }
    else
    {
      mafErrorMessage("Unable to resolve URL for input XML file");
      errorCode = IO_WRONG_URL;
    }

    cppDEL (errReporter);
    cppDEL (m_DOM->m_XMLParser);
  }
  else
  {
    // parser allocation error
    mafErrorMessage("Failed to allocate XML parser");
    errorCode = IO_XML_PARSER_INTERNAL_ERROR;
  }

  // terminate the XML library
  XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();
  
  if (GetErrorCode()==0)
    SetErrorCode(errorCode);

  return GetErrorCode();
}
