/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEManager
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEManager_H__
#define __albaVMEManager_H__
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaObserver.h"
#include "albaVME.h"
#include "albaVMERoot.h"
#include "albaGUIFileHistory.h"

//----------------------------------------------------------------------------
// Forward References :
//----------------------------------------------------------------------------
class albaVMEStorage;
class wxArchiveFSHandler;
class wxFileSystem;

/** albaVMEManager : Class managing VMEs inside a ALBA application.

This class is responsible for managing the VME in a ALBA application:
It can add or remove VME to the current tree, add a new tree, load and save a tree from or to an msf (zmsf) file.

*/
//----------------------------------------------------------------------------
class ALBA_EXPORT albaVMEManager: public albaObserver
//----------------------------------------------------------------------------
{
public:
  /** constructor */
  albaVMEManager();

  /** destructor */
  ~albaVMEManager(); 

  /** Set the event receiver object*/
  void SetListener(albaObserver *listener) {m_Listener = listener;};

  /** Process events coming from other components */
  void OnEvent(albaEventBase *e);
 
  /** Destroy all nodes (also the root), for each destroyed node the manager send 
  an event VME_REMOVING, then search the root and select it. */ 
  void MSFNew();
  
	/** Open the msf filename. */
	int MSFOpen(albaString filename);
  
	/** Open the msf from file history. */
  int MSFOpen(int file_id);

  /** Open the compressed zmsf filename. */
  const char *ZIPOpen(albaString filename);

  /** Save the msf tree into a compressed file. */
  void ZIPSave(albaString file = "");
  
	/** Save the msf tree. */
  int MSFSave();
  
	/** Save the msf tree with different filename. */
  int MSFSaveAs();

	/** Return true if the tree has been modifyed. */
  bool MSFIsModified() {return m_Modified;};
  
	/** Set the modify flag. */
  void MSFModified(bool modified) {m_Modified = modified;};

	/** Add the vme to the tree. */
  void VmeAdd(albaVME *n);

	/** Remove the vme from the tree. */
  void VmeRemove(albaVME *n);

	/** Get the time bounds of the tree. */
  void TimeGetBounds(double *min, double *max);

	/** Set the time for the tree. */
  void TimeSet(double time);

	/** Send the event VME_REMOVING to inform logic that the vme and its subtree are removed from the tree. */
  void NotifyRemove(albaVME *n);

	/** Send the event VME_ADDED to inform logic that the vme and its subtree are added to the tree. */
  void NotifyAdd(albaVME *n);
  
	/** Show the dialog to confirm the save of the current tree. */
	bool AskConfirmAndSave();
 
  /** Set the filename for the current tree. */
  void SetDirName (albaString dirname) {m_MSFDir = dirname;};

	/** Set the filename for the current tree. */
  void SetFileName (albaString &filename) {m_MSFFile = filename;};
 
  /** Set the file extension. */
  void SetMsfFileExtension (albaString &extension) {m_MsfFileExtension = extension;};

	/** Get the filename of the current tree. */
  albaString &GetFileName() {return m_MSFFile;};
 
	/** Link to the main men the file history manager. */
  void SetFileHistoryMenu(wxMenu *menu);
 
	/** Return the tree's root. */
  albaVMERoot *GetRoot();

  /** Return the storage associated to the current tree.*/
  albaVMEStorage *GetStorage();

	/** Set the application stamp; this is the mark of the specific vertical application (is always equal to the application name). */
  void SetApplicationStamp(albaString &appstamp);

  /** Set other application stamps for compatibility with similar applications (the application stamp itself is already present). */
  void SetApplicationStamp(std::vector<albaString> appstamp);

	/** Set the flag for bak file generation on saving MSF file. */
	void MakeBakFileOnSave(bool bakfile = true) {m_MakeBakFile = bakfile;}
	  
  /** Remove temporary directory used for compressed msf files.*/
  void RemoveTempDirectory();

  /** Set the flag for saving binary files associated to time varying VMEs.*/
  void SetSingleBinaryFile(bool singleFile);

  /** Set the flag for test mode without GUI */
  void SetTestMode(bool testmode){m_TestMode = testmode;}; // Losi 02/16/2010 for test class

	/** Set the flag for attive controll in Open Alba Project */
	void IgnoreAppStamp(bool ignore) { m_IgnoreAppStamp = ignore; };

protected:
  /** Create a compressed msf file: zmsf.*/
  bool MakeZip(const albaString &zipname, wxArrayString *files);

  /** Set tag with creation date for the node passed as argument.*/
  void AddCreationDate(albaVME *vme);

  bool                m_Modified;         ///< Used to known when the tree has been modified...

  albaObserver*        m_Listener;         ///< Register the event receiver object
  wxConfigBase*       m_Config;           ///< Application configuration for file history management
	wxArchiveFSHandler  *m_ZipHandler;      ///< Handler for zip archive (used to open zmsf files)
  wxFileSystem        *m_FileSystem;      ///< File system manager
  albaGUIFileHistory	  m_FileHistory;      ///< Used to hold recently opened files
  int                 m_FileHistoryIdx;   ///< Identifier of the file to open

  bool                m_MakeBakFile;      ///< Flag used to create or not the backup file of the saved msf.
  albaString           m_MSFDir;           ///< Directory name in which is present the msf file.
  std::vector<albaString> m_AppStamp;      ///< Application stamps for our application.
  albaString           m_MSFFile;          ///< File name of the data associated to the tree.
  albaString           m_ZipFile;          ///< File name of compressed archive in which save the data associated to the tree.
  albaString           m_TmpDir;           ///< Temporary directory for zmsf extraction
  albaString           m_MsfFileExtension;   ///< File extension
	 
  bool                m_LoadingFlag;      ///< used when an MSF is being loaded
  bool                m_Crypting;         ///< used to enable the Encryption for the MSF
  bool                m_SingleBinaryFile; ///< used to store binary files associated to time varying VMEs as multiple files or not.
  albaVMEStorage*      m_Storage;          ///< Associated storage
  bool                m_TestMode;         ///< set true for test mode without GUI

	bool								m_IgnoreAppStamp;   /// Flag used to Open Alba Project File with appStamp verification 
  /** friend test class */
  friend class albaVMEManagerTest; // Losi 02/16/2010 for test class
};
#endif
