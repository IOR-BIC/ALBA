/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorage.h,v $
  Language:  C++
  Date:      $Date: 2004-12-29 18:00:27 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafStorage_h__
#define __mafStorage_h__

#include "mafObject.h"
#include "mafStorable.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafStorageElement;

/** mafStorage.
  to be written
  @todo implement packing (single file) and remote files (URL access)
 */  
class mafStorage
{
public:
  mafStorage();
  virtual ~mafStorage();

  /** Set the name of the file to be read or written */
  void SetFileName(const char *name);

  /** Return the name of the file to be read or written */
  const char *GetFileName();
  
  /** 
    Return the name of the last read file, this is internally used when writing to
    a new file to read and copy information between the old and new file */
  const char *GetPareserFileName();

  /** perform storing. the argument is the tag of the root node */
  int Store();
  
  /** perform restoring. the argument is the tag of the root node  */
  int Restore();
  
  /** set the root element to be stored */
  void SetRoot (mafStorable *root);

  /** resolve an URL and provide local filename to be used as input */
  virtual bool ResolveInputURL(const mafString &url, mafString &filename)=0;

  /** resolve an URL and provide a local filename to be used as output */
  virtual bool ResolveOutputURL(const mafString &url, mafString &filename)=0;
    
protected:
  /** This is called by Store() and must be reimplemented by subclasses */
  virtual int InternalStore()=0;

  /** This is called by Restore() and must be reimplemented by subclasses */
  virtual int InternalRestore()=0;

  mafStorable *m_Root;              ///< root object to be stored, or being restored
  mafStorageElement *m_RootElement; ///< root stored element

  mafString m_FileName;             ///< name of the file being accessed
  mafString m_ParserFileName;       ///< name of the last parsed file (used for SaveAs)
};
#endif // _mafStorage_h_
