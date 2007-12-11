/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorage.h,v $
  Language:  C++
  Date:      $Date: 2007-12-11 11:25:08 $
  Version:   $Revision: 1.16 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafStorage_h__
#define __mafStorage_h__

#include "mafObject.h"
#include "mafString.h"
#include <set>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafStorageElement;
class mafStorable;
class mafObserver;

/** Abstract class for an abject mastering the storing/restoring of objects
  This is an abstract class providing defining APIs for objects mastering storing/restoring
  of storing and restoring of mafStorable objects. Concrete implementation will define a concrete encoding.
  The basic idea of storing and restoring is the creation of a parallel tree, with all information
  to be stored or restored. The tree is made of mafStorageElement nodes (or better, concrete classes inherited
  from it). The storage class provides an access to the document element and defines a couple of functions InternalStore
  and InternalRestore for implementing the concrete mechanisms. Users of this kind of object should provide an URL of 
  the document to be opened/saved, and than call Store() or Restore(). In case of Restoring, the Document element is automatically created
  by the storage class, while when storing it must be set (or can have be created during a previous restoring).
  This class will also provide a high level mechanism for resolving URLs, that will try to copy locally remote or zipped files
  and allow a simple file access to I/O classes.
  @sa mafStorageElement mafStorable
  @todo
  - implement packing (single file)
  - remote files (URL access)
  - improve tmp files management
 */  
class MAF_EXPORT mafStorage: public mafObject
{
public:
  mafAbstractTypeMacro(mafStorage,mafObject);

  enum STORAGE_IO_ERRORS {IO_OK=0,IO_GENERIC_ERROR,IO_WRONG_OBJECT_TYPE,IO_LAST_ERROR};

  mafStorage();
  virtual ~mafStorage() {}

  /** Set the URL of the document to be read or written */
  virtual void SetURL(const char *name);

  /** Force Copy storage URL to parser URL (used by the MAF1.x importer) */
  void ForceParserURL();

  /** Return the URL of the document to be read or written */
  const char *GetURL();
  
  /** 
    Return the name of the last read file, this is internally used when writing to
    a new file to read and copy information between the old and new file */
  const char *GetPareserURL();

  /** perform storing. the argument is the tag of the document node */
  int Store();
  
  /** perform restoring. the argument is the tag of the document node  */
  int Restore();
  
  /** set the document element to be stored */
  void SetDocument (mafStorable *doc);

  /** return the document object restored */
  mafStorable *GetDocument();

  /** resolve an URL and provide local filename to be used as input */
  virtual int ResolveInputURL(const char * url, mafString &filename, mafObserver *observer = NULL)=0;

  /** resolve an URL and provide a local filename to be used as output */
  //virtual bool ResolveOutputURL(const mafCString url, mafString &filename)=0;

  /** 
    store a file to an URL. This API transfer a tmp local file to URL.
    If no URL is specified use the storage URL as a base URL. */
  virtual int StoreToURL(const char *filename, const char *url=NULL) = 0;

  /** 
    Remove a file from the specified URL. Used when a file in the storage
    is no more necessary. If the specified URL is a local file name try 
    to prepend the storage URL as a base URL name.*/
  virtual int ReleaseURL(const char *url) = 0;

  /** 
    Return a name of file to be used as tmp file during store/restore.
    This is usually located insed the MSF file directory */
  virtual void GetTmpFile(mafString &filename);

  /** remove the tmp file */
  void ReleaseTmpFile(const char *filename);

  /** 
    Check if a file is present in the storage directory. The directory list is 
    open when Store() is called and directory data is updated at that time. */
  bool IsFileInDirectory(const char *filename);

  /** 
    Set the folder where to store tmp files. If not specified the storage 
    will automatically use a default folder, like the current directory 
    or the storage file folder. */
  virtual void SetTmpFolder(const char *folder) {m_TmpFolder=folder;}

  /** return the folder where tmp files are stored */
  virtual const char* GetTmpFolder() {return m_TmpFolder;}

  void SetErrorCode(int err) {m_ErrorCode=err;}
  int GetErrorCode() {return m_ErrorCode;}

  bool NeedsUpgrade() {return m_NeedsUpgrade;};

protected:
  /** This is called by Store() and must be reimplemented by subclasses */
  virtual int InternalStore()=0;

  /** This is called by Restore() and must be reimplemented by subclasses */
  virtual int InternalRestore()=0;

  /** populate the list of files in the storage folder */
  virtual int OpenDirectory(const char *dir_name)=0;

  bool                 m_NeedsUpgrade; ///< Flag used to enable or not the upgrade mechanism.
  mafStorable         *m_Document;        ///< document object to be stored, or being restored
  mafStorageElement   *m_DocumentElement; ///< document stored element

  mafString           m_URL;          ///< name of the file being accessed
  mafString           m_ParserURL;    ///< name of the last parsed file (used for SaveAs)
  mafID               m_TmpFileId;    ///< counter for unique tmp file naming
  mafString           m_TmpFolder;    ///< folder where to store tmp files
  
  std::set<mafString> m_TmpFileNames; ///< name of tmp files in the MSF dir
  std::set<mafString> m_FilesDictionary; ///< list of files in the storage folder: to be populated by OpenDirectory()
  int                 m_ErrorCode;    ///< the error code 0==OK
};
#endif // _mafStorage_h_
