/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPics
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaPics_H__
#define __albaPics_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGUICheckTree.h" // friend class
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <vector>

#ifdef ALBA_USE_VTK
  #include "vtkImageData.h"
#endif
//----------------------------------------------------------------------------
// Forward refs:
//----------------------------------------------------------------------------
struct albaPictureFactory_Pimpl;
//----------------------------------------------------------------------------
// albaPictureFactory 
//----------------------------------------------------------------------------
/** This class is normally instantiated as the "albaPics" singleton. 
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

-CLOSE_SASH : used by albaNamedPanel

-NODE_YELLOW,NODE_GRAY,NODE_RED,NODE_BLUE : used by ListCtrl

NOTE:
- Icons for the Toolbar must be 20x20 in size, and with transparent background (replacing blank color with "None" in the XPM)
- Icons to be used in one ImageList (albaGUITree,albaGUICheckTree,...) must be all the same size, and with white background.
- Icons to be used in albaGUICheckTree (vme icons) must be 16x16 with white background
*/
class ALBA_EXPORT albaPictureFactory 
{
public:
  /** initialize the factory with the standard icons */
   albaPictureFactory();
  ~albaPictureFactory();
  void Initialize();

  /** add a picture to the factory -- if id already exist it is overwritten */
  void Add(wxString id,char** xpm);

  /** retrieve a picture from the Factory as a wxBitmap */
  wxBitmap GetBmp(wxString id);

  /** retrieve a picture from the Factory as a wxImage */
  wxImage GetImg(wxString id);

  /** retrieve a picture from the Factory as a wxIcon */
  wxIcon GetIcon(wxString id);

#ifdef ALBA_USE_VTK
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

  static albaPictureFactory* GetPictureFactory();

  static bool GetPicsInitialized();


protected:
  albaPictureFactory_Pimpl *m_PictureMaps;
};
//----------------------------------------------------------------------------
// macro
//----------------------------------------------------------------------------
/* shotcut to add a picture to the factory */
#define albaADDPIC(X) albaPictureFactory::GetPictureFactory()->Add( #X , X##_xpm )

/* shotcut to add a vmepicture to the factory */
#define albaADDVMEPIC(X) albaPictureFactory::GetPictureFactory()->AddVmePic( #X , X##_xpm )

//----------------------------------------------------------------------------
// the albaPics and albaVmePics singleton
//----------------------------------------------------------------------------
// extern "C" __declspec(dllexport) albaPictureFactory albaPics;
// extern bool albaPics_Initialized;
#endif // __albaPics_H__
