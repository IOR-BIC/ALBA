/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPics.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:03:23 $
  Version:   $Revision: 1.11 $
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
#include "mafGUICheckTree.h" // friend class
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <vector>

#ifdef MAF_USE_VTK
  #include "vtkImageData.h"
#endif
//----------------------------------------------------------------------------
// Forward refs:
//----------------------------------------------------------------------------
struct mafPictureFactory_Pimpl;
//----------------------------------------------------------------------------
// mafPictureFactory 
//----------------------------------------------------------------------------
/** This class is normally instantiated as the "mafPics" singleton. 
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

NOTE:
- Icons for the Toolbar must be 20x20 in size, and with transparent background (replacing blank color with "None" in the XPM)
- Icons to be used in one ImageList (mafGUITree,mafGUICheckTree,...) must be all the same size, and with white background.
- Icons to be used in mafGUICheckTree (vme icons) must be 16x16 with white background
*/
class mafPictureFactory 
{
public:
  /** initialize the factory with the standard icons */
   mafPictureFactory();
  ~mafPictureFactory();
  void Initialize();

  /** add a picture to the factory -- if id already exist it is overwritten */
  void Add(wxString id,char** xpm);

  /** retrieve a picture from the Factory as a wxBitmap */
  wxBitmap GetBmp(wxString id);

  /** retrieve a picture from the Factory as a wxImage */
  wxImage GetImg(wxString id);

  /** retrieve a picture from the Factory as a wxIcon */
  wxIcon GetIcon(wxString id);

#ifdef MAF_USE_VTK
  /** retrieve a picture from the Factory as a vtkImageData -- this picture must be deleted by the user */
  vtkImageData* GetVTKImg(wxString id);
#endif

  /** Fills the given vectors with the available Pic Id's */
  void GetPicIds(std::vector<wxString>& v);

  /** add a vme-picture to the factory. 
      id should be the ClassName.
      if id already exist it is overwritten.
  */
  void AddVmePic(wxString id,char** xpm);

  /** retrieve a vme-picture from the Factory as a wxBitmap. */
  wxBitmap GetVmePic(wxString id);

  /** retrieve the names of the registered vme-pics */
  void GetVmeNames( std::vector<wxString>& v );


protected:
  mafPictureFactory_Pimpl *m_PictureMaps;
};
//----------------------------------------------------------------------------
// macro
//----------------------------------------------------------------------------
/* shotcut to add a picture to the factory */
#define mafADDPIC(X) mafPics.Add( #X , X##_xpm )

/* shotcut to add a vmepicture to the factory */
#define mafADDVMEPIC(X) mafPics.AddVmePic( #X , X##_xpm )

//----------------------------------------------------------------------------
// the mafPics and mafVmePics singleton
//----------------------------------------------------------------------------
extern mafPictureFactory mafPics;
extern bool mafPics_Initialized;
#endif // __mafPics_H__
