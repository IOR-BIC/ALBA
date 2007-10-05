/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUISRBBrowse.h,v $
  Language:  C++
  Date:      $Date: 2007-10-05 08:06:39 $
  Version:   $Revision: 1.3 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafGUISRBBrowse_H__
#define __mafGUISRBBrowse_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mmgDialog.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mmgGui;
class mmgTree;

//----------------------------------------------------------------------------
// const :
//----------------------------------------------------------------------------
enum SRB_BROWSING_STYLE
{
  DOWNLOAD = 0,
  UPLOAD,
  DIR_ONLY,
};

//----------------------------------------------------------------------------
// mafGUISRBBrowse :
//----------------------------------------------------------------------------
/**
This component allow to make remote file browsing.
*/
class mafGUISRBBrowse : public mmgDialog
{
public:
  mafGUISRBBrowse(mafObserver *listener = NULL, const wxString &title = _("Remote file dialog"), long style = mafRESIZABLE, long dialogStyle = DOWNLOAD);
  ~mafGUISRBBrowse(); 


  void OnEvent(mafEventBase *maf_event);
  void SetListener(mafObserver *listener) {m_Listener = listener;};

  mafString GetFile() {return m_RemoteFileName;};

  /** Return the name of the node selected*/
  mafString GetNodeName();

  /** Return the host-name if the user changed the default one.*/
  mafString GetHost() {return m_Host;};

  /** Return the user-name for remote connection.*/
  mafString GetUser() {return m_User;};

  /** Return the password for remote connection.*/
  mafString GetPassword() {return m_Pwd;};

protected:
  /** Create the GUI for the FlyTo animation */
  void CreateGui();

  /** Call the Web Service to list files on SRB Repository.*/
   int RemoteSRBList();
   
   /** Create the tree of SRB files */
   void CreateTree();

   /** Delete the files from the tree, to show only folders.*/
   void ShowOnlyFolders();

  /** Enable/Disable dialog widgets according to the protocol choose for the file.*/
  //void EnableWidgets();

  mmgTree  *m_Tree;

  mmgGui      *m_Gui;
  mmgGui      *m_GuiList;

  mafString    m_Wild;
  mafString    m_Host;
  mafString    m_User;
  mafString    m_Pwd;
  mafString    m_Domain;
  mafString    m_RemoteFileName;
  wxString     m_FileList;
  long         m_NodeId;
  long         m_NodeCounter;
  long         m_DialogStyle;
};
#endif
