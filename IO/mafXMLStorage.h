/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafXMLStorage.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:18:11 $
  Version:   $Revision: 1.7 $
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
  mafTypeMacro(mafXMLStorage,mafStorage)

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

  /** delete file from storage */
  virtual int ReleaseURL(const char *url);

  /** populate the list of file in the directory */
  virtual int OpenDirectory(const char *pathname);

protected:
  /** This is called by Store() and must be reimplemented by subclasses */
  virtual int InternalStore();

  /** This is called by Restore() and must be reimplemented by subclasses */
  virtual int InternalRestore();

  mafString  m_FileType;  ///< The type of file to be opened
  mafString  m_Version;   ///< The version of the file to be opened
  mmuXMLDOM  *m_DOM;      ///< PIMPL object storing XML objects' pointers
};
#endif // _mafXMLStorage_h_
