/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafXMLStorage.h,v $
  Language:  C++
  Date:      $Date: 2004-12-27 18:22:26 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafXMLStorage_h__
#define __mafXMLStorage_h__

#include "mafObject.h"

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

/** mafXMLStorage 
  @todo to be written
 */  
class mafXMLStorage
{
public:

  DOMDocument *GetXMLDocument() {return m_XMLDoc;}
    
protected:
  /**
    This is called by Store() and must be reimplemented by subclasses. */
  virtual void InternalStore(const char *root_tag);

  /** 
    This is called by Restore() and must be reimplemented by subclasses. */
  virtual int InternalRestore(const char *root_tag);

  DOMDocumentType   *m_XMLDoctype;
  DOMDocument       *m_XMLDoc;
  DOMNode           *m_XMLNode;
  DOMImplementation *m_XMLImplement;
  DOMWriter         *m_XMLSerializer;
  XMLFormatTarget   *m_XMLTarget;

};
#endif // _mafXMLStorage_h_
