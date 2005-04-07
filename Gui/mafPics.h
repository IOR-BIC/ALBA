/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPics.h,v $
  Language:  C++
  Date:      $Date: 2005-04-07 11:39:44 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafPics_H__
#define __mafPics_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h" //important: mafDefines should always be included as first
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>

//----------------------------------------------------------------------------
// Forward refs:
//----------------------------------------------------------------------------
struct mafPictureFactory_Pimpl;
//----------------------------------------------------------------------------
// mafPictureFactory 
//----------------------------------------------------------------------------
/** This class is normally istantiated as the "mafPics" singleton. 
 icons available by default:    

-FILE_NEW,FILE_OPEN,FILE_SAVE : standard file operation
-OP_COPY,OP_CUT,OP_PASTE,OP_REDO,OP_UNDO : standard edit operation
-TIME_END,TIME_NEXT,TIME_PLAY,TIME_PREV,TIME_STOP : time navigation
-FLYTO,ZOOM,ZOOM_ALL,-ZOOM_SEL : standard camera movement
 
 all of the above are normally used in the program toolbar

-PIC_BACK,PIC_BOTTOM,PIC_FRONT,PIC_LEFT,PIC_RIGHT,PIC_TOP,TIME_BEGIN : set of camera orientations

-APP_ICON16x16,APP_ICON32x32 : these two picture are combined in one wxIconBundle 
 and assigned to the MDIFrameWindow (the small is visible in the frame and in the taskbar,
 the big is visible when pressing CTRL-TAB to switch among applications).
 Note: it is not possible to set in this way the Application Icon. The application icon must 
 be inserted in the resource file - see app.rc and app.ico

-MDICHILD_ICON : icon assigned to all MDIChild Windows

-CLOSE_SASH : used by mafNamedPanel

-NODE_YELLOW,NODE_GRAY,NODE_RED,NODE_BLUE : used by ListCtrl

*/
class mafPictureFactory 
{
public:
  /* initialize the factory with the standard icons */
   mafPictureFactory();
  ~mafPictureFactory();

  /* add a picture to the factory -- if id already exist it is overwritten */
  void Add(wxString id,char** xpm);

  /* retrieve a picture from the Factory as a wxBitmap */
  wxBitmap GetBmp(wxString id);

  /* retrieve a picture from the Factory as a wxImage */
  wxImage GetImg(wxString id);

  /* retrieve a picture from the Factory as a wxIcon */
  wxIcon GetIcon(wxString id);

protected:
  mafPictureFactory_Pimpl *m_p;
};
//----------------------------------------------------------------------------
// macro
//----------------------------------------------------------------------------
/* shotcut to add a picture to the factory */
#define mafADDPIC(X) mafPics.Add( #X , X##_xpm )

//----------------------------------------------------------------------------
// the mafPics singleton
//----------------------------------------------------------------------------
extern mafPictureFactory mafPics;

#endif // __mafPics_H__






