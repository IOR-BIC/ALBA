/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafXMLStorage.h,v $
  Language:  C++
  Date:      $Date: 2004-12-28 19:45:27 $
  Version:   $Revision: 1.2 $
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
class DOMDocumentType;
class DOMDocument;
class DOMNode;
class DOMImplementation;
class DOMWriter;
class XMLFormatTarget;
class XercesDOMParser;

/** mafXMLStorage 
  @todo to be written
 */  
class mafXMLStorage: public mafStorage
{
public:

  /** 
    Return the instance of the DOM document used while reading and writing.
    This object is created when Store/Restore starts and destroyed when stops.*/
  DOMDocument *GetXMLDocument() {return m_XMLDoc;}

  /** The TAG identifying the type (i.e. format) of file. (e.g. "MSF") */
  void SetFileType(const char *filetype);
  /** The TAG identifying the type (i.e. format) of file. (e.g. "MSF") */
  const char *GetFileType();

  /** The version of the file format used type of file. (default "1.1") */
  void SetVersion(const char *version);
  /** The version of the file format used type of file. (default "1.1") */
  const char *GetVersion();

protected:
  /** This is called by Store() and must be reimplemented by subclasses */
  virtual int InternalStore();

  /** This is called by Restore() and must be reimplemented by subclasses */
  virtual int InternalRestore();

  mafString         m_FileType;
  mafString         m_Version;

  DOMDocumentType   *m_XMLDoctype;
  DOMDocument       *m_XMLDoc;
  DOMNode           *m_XMLNode;
  DOMImplementation *m_XMLImplement;
  DOMWriter         *m_XMLSerializer;
  XMLFormatTarget   *m_XMLTarget;
  XercesDOMParser   *m_XMLParser;
  

};
#endif // _mafXMLStorage_h_
