/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafXMLStorage.h,v $
  Language:  C++
  Date:      $Date: 2005-01-10 00:18:08 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
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


/** mafXMLStorage 
  @todo to be written
*/  
class mafXMLStorage: public mafStorage
{
public:
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
  virtual bool ResolveInputURL(const mafString &url, mafString &filename);

  /** resolve an URL and provide a local filename to be used as output */
  virtual bool ResolveOutputURL(const mafString &url, mafString &filename);

protected:
  /** This is called by Store() and must be reimplemented by subclasses */
  virtual int InternalStore();

  /** This is called by Restore() and must be reimplemented by subclasses */
  virtual int InternalRestore();

  mafString  m_FileType;
  mafString  m_Version;
  mmuXMLDOM  *m_DOM; ///< PIMPL object storing XML objects' pointers
};
#endif // _mafXMLStorage_h_
