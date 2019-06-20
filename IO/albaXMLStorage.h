/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaXMLStorage
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaXMLStorage_h__
#define __albaXMLStorage_h__

#include "albaStorage.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaXMLElement;
class mmuXMLDOM;


/** Concrete implementation of Storage object using Xerces-C for storing in XML.
  This is a concrete implementation of storage object for storing XML documents
  by means of Xerces-C library (http://xml.apache.org/xerces-c/).
  This class also defines a function to access to XML/Xerces-C specific objects, 
  stored into a PIMPL class (mmuXMLDOM).
  @sa albaStorage albaXMLElement mmuXMLDOM
  @todo
    - remove "IncludeWX.h" inclusion from .cpp
    - add support for NULL destination URL
*/  
class ALBA_EXPORT albaXMLStorage: public albaStorage
{
public:
  albaTypeMacro(albaXMLStorage,albaStorage);
  
  enum XML_IO_ERRORS {IO_XML_PARSE_ERROR=IO_LAST_ERROR,IO_DOM_XML_ERROR,IO_RESTORE_ERROR,IO_WRONG_FILE_TYPE,IO_WRONG_FILE_VERSION,IO_WRONG_URL,IO_XML_PARSER_INTERNAL_ERROR};

  albaXMLStorage();
  virtual ~albaXMLStorage();

  /** 
    Return the instance of the DOM document used while reading and writing.
    This object is created when Store/Restore starts and destroyed when stops.*/
  mmuXMLDOM *GetXMLDOM() {return m_DOM;}

  /** The TAG identifying the type (i.e. format) of file. (e.g. "MSF") */
  void SetFileType(const char *filetype);
  /** The TAG identifying the type (i.e. format) of file. (e.g. "MSF") */
  const char *GetFileType();

  /** The version of the file format used type of file. (default "1.1") */
  void SetVersion(const char *version);

  /** The version of the file format used type of file. (default "1.1") */
  const char *GetVersion();

  /** Return the version of the opened document.*/
  const char *GetDocumentVersion();

  /** resolve an URL and provide local filename to be used as input */
  virtual int ResolveInputURL(const char * url, albaString &filename, albaObserver *observer = NULL);

  /** resolve an URL and provide a local filename to be used as output */
  virtual int StoreToURL(const char * filename, const char * url);

  /** release file from storage. Actually do not delete, just collect. */
  virtual int ReleaseURL(const char *url);

  /** remove the file from URL */
  virtual int DeleteURL(const char *url);

  /** populate the list of file in the directory */
  virtual int OpenDirectory(const char *pathname);

  /** Set the URL of the document to be read or written */
  virtual void SetURL(const char *name);

  virtual const char* GetTmpFolder();

  /** empty the garbage collector list deleting old files */
  virtual void EmptyGarbageCollector();

protected:
  /** This is called by Store() and must be reimplemented by subclasses */
  virtual int InternalStore();

  /** This is called by Restore() and must be reimplemented by subclasses */
  virtual int InternalRestore();

  albaString m_FileType;  ///< The type of file to be opened
  albaString m_Version;   ///< Current MSF version
  albaString m_DocumentVersion; ///< Open Document version.
  mmuXMLDOM *m_DOM;      ///< PIMPL object storing XML objects' pointers
  std::set<albaString> m_GarbageCollector; ///< collect URL to be released
  albaString  m_DefaultTmpFolder; ///< used to store the current default tmp folder
};
#endif // _albaXMLStorage_h_
