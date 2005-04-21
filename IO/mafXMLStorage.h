/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafXMLStorage.h,v $
  Language:  C++
  Date:      $Date: 2005-04-21 14:03:23 $
  Version:   $Revision: 1.9 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafXMLStorage_h__
#define __mafXMLStorage_h__

#include "mafStorage.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafXMLElement;
class mmuXMLDOM;


/** Concrete implementation of Storage object using Xerces-C for storing in XML.
  This is a concrete implementation of storage object for storing XML documents
  by means of Xerces-C library (http://xml.apache.org/xerces-c/).
  This class also defines a function to access to XML/Xerces-C specific objects, 
  stored into a PIMPL class (mmuXMLDOM).
  @sa mafStorage mafXMLElement mmuXMLDOM
  @todo
    - remove "IncludeWX.h" inclusion from .cpp
    - add support for NULL destination URL
*/  
class mafXMLStorage: public mafStorage
{
public:
  mafTypeMacro(mafXMLStorage,mafStorage);
  
  enum XML_IO_ERRORS {IO_XML_PARSE_ERROR=IO_LAST_ERROR,IO_DOM_XML_ERROR,IO_RESTORE_ERROR,IO_WRONG_FILE_VERSION,IO_WRONG_URL,IO_XML_PARSER_INTRNAL_ERROR};

  mafXMLStorage();
  virtual ~mafXMLStorage();

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

  /** resolve an URL and provide local filename to be used as input */
  virtual bool ResolveInputURL(const char * url, mafString &filename);

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

protected:
  /** This is called by Store() and must be reimplemented by subclasses */
  virtual int InternalStore();

  /** This is called by Restore() and must be reimplemented by subclasses */
  virtual int InternalRestore();

  mafString  m_FileType;  ///< The type of file to be opened
  mafString  m_Version;   ///< The version of the file to be opened
  mmuXMLDOM  *m_DOM;      ///< PIMPL object storing XML objects' pointers
  std::set<mafString> m_GarbageCollector; ///< collect URL to be released
  mafString  m_DefaultTmpFolder; ///< used to store the current default tmp folder
};
#endif // _mafXMLStorage_h_
