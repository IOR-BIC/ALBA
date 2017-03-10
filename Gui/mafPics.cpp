/*=========================================================================

 Program: MAF2
 Module: mafPics
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafPics.h" 
#include <map>
#include <string>

//----------------------------------------------------------------------------
// mafPictureFactory_Pimpl
//----------------------------------------------------------------------------
typedef std::map<std::string,wxImage> mafPicMap;

struct mafPictureFactory_Pimpl
{
  public:
  mafPicMap map;
  mafPicMap vme_map; //here are kept the vme icons
};
//----------------------------------------------------------------------------
// the mafPic Singleton
//----------------------------------------------------------------------------
//mafPictureFactory mafPics;
static bool mafPics_Initialized = false;

//----------------------------------------------------------------------------
mafPictureFactory::mafPictureFactory()
//----------------------------------------------------------------------------
{
  m_PictureMaps = new mafPictureFactory_Pimpl();
}
//----------------------------------------------------------------------------
void mafPictureFactory::Initialize()
//----------------------------------------------------------------------------
{
  mafPics_Initialized = true;
   #include <pic/FRAME_ICON16x16.xpm>
    mafADDPIC(FRAME_ICON16x16);
    #include <pic/FRAME_ICON32x32.xpm>
    mafADDPIC(FRAME_ICON32x32);
    #include <pic/CLOSE_SASH.xpm>
    mafADDPIC(CLOSE_SASH);
    #include <pic/ROLLOUT_CLOSE.xpm>
    mafADDPIC(ROLLOUT_CLOSE);
    #include <pic/ROLLOUT_OPEN.xpm>
    mafADDPIC(ROLLOUT_OPEN);
    #include <pic/FILE_NEW.xpm>    
    mafADDPIC(FILE_NEW);
    #include <pic/FILE_OPEN.xpm>
    mafADDPIC(FILE_OPEN);
    #include <pic/FILE_SAVE.xpm>
    mafADDPIC(FILE_SAVE);
    #include <pic/FLYTO.xpm>
    mafADDPIC(FLYTO);
    #include <pic/INFO.xpm>
    mafADDPIC(INFO);
    #include <pic/PRINT.xpm>
    mafADDPIC(PRINT);
    #include <pic/PRINT_PREVIEW.xpm>
    mafADDPIC(PRINT_PREVIEW);
    #include <pic/SPLASH_SCREEN.xpm>
    mafADDPIC(SPLASH_SCREEN);
    #include <pic/MDICHILD_ICON.xpm>
    mafADDPIC(MDICHILD_ICON);
    #include <pic/OP_COPY.xpm>
    mafADDPIC(OP_COPY);
    #include <pic/OP_CUT.xpm>
    mafADDPIC(OP_CUT);
    #include <pic/OP_PASTE.xpm>
    mafADDPIC(OP_PASTE);
		#include <pic/OP_FIND.xpm>
		mafADDPIC(OP_FIND);
    #include <pic/OP_REDO.xpm>
    mafADDPIC(OP_REDO);
    #include <pic/OP_UNDO.xpm>
    mafADDPIC(OP_UNDO);
    #include <pic/PIC_BACK.xpm>
    mafADDPIC(PIC_BACK);
    #include <pic/PIC_BOTTOM.xpm>
    mafADDPIC(PIC_BOTTOM);
    #include <pic/PIC_FRONT.xpm>
    mafADDPIC(PIC_FRONT);
    #include <pic/PIC_LEFT.xpm>
    mafADDPIC(PIC_LEFT);
    #include <pic/PIC_RIGHT.xpm>
    mafADDPIC(PIC_RIGHT);
    #include <pic/PIC_TOP.xpm>
    mafADDPIC(PIC_TOP);
    #include <pic/TIME_BEGIN.xpm>
    mafADDPIC(TIME_BEGIN);
    #include <pic/TIME_END.xpm>      
    mafADDPIC(TIME_END);
    #include <pic/TIME_NEXT.xpm>
    mafADDPIC(TIME_NEXT);
    #include <pic/TIME_PLAY.xpm>
    mafADDPIC(TIME_PLAY);
    #include <pic/TIME_PREV.xpm>
    mafADDPIC(TIME_PREV);
    #include <pic/TIME_STOP.xpm>
    mafADDPIC(TIME_STOP);
    #include <pic/MOVIE_RECORD.xpm>
    mafADDPIC(MOVIE_RECORD);
    #include <pic/ZOOM.xpm>
    mafADDPIC(ZOOM);
    #include <pic/ZOOM_ALL.xpm>
    mafADDPIC(ZOOM_ALL);
    #include <pic/ZOOM_SEL.xpm>
    mafADDPIC(ZOOM_SEL);

		#include "pic/CAMERA.xpm"
		mafADDPIC(CAMERA);
		#include "pic/IMAGE_PREVIEW.xpm"
		mafADDPIC(IMAGE_PREVIEW);

    // these are used by the ListCtrl
    #include <pic/NODE_YELLOW.xpm>
    mafADDPIC(NODE_YELLOW);
    #include <pic/NODE_RED.xpm>
    mafADDPIC(NODE_RED);
    #include <pic/NODE_BLUE.xpm>
    mafADDPIC(NODE_BLUE);
    #include <pic/NODE_GRAY.xpm>
    mafADDPIC(NODE_GRAY);

    #include <pic/DISABLED.xpm>
    mafADDPIC(DISABLED);
    #include <pic/RADIO_ON.xpm>
    mafADDPIC(RADIO_ON);
    #include <pic/RADIO_OFF.xpm>
    mafADDPIC(RADIO_OFF);
    #include <pic/CHECK_ON.xpm>
    mafADDPIC(CHECK_ON);
    #include <pic/CHECK_OFF.xpm>
    mafADDPIC(CHECK_OFF);

		#include <pic/OP_HELP.xpm>
		mafADDPIC(OP_HELP);

		#include <pic/BUTTON_HELP.xpm>
		mafADDPIC(BUTTON_HELP);
}
//----------------------------------------------------------------------------
mafPictureFactory::~mafPictureFactory()
//----------------------------------------------------------------------------
{
  cppDEL(m_PictureMaps);
}
//----------------------------------------------------------------------------
void mafPictureFactory::Add(wxString id,char** xpm)
//----------------------------------------------------------------------------
{
  m_PictureMaps->map[id.c_str()] = wxImage(xpm);
}
//----------------------------------------------------------------------------
wxBitmap mafPictureFactory::GetBmp(wxString id)
//----------------------------------------------------------------------------
{
  mafPicMap::iterator it = m_PictureMaps->map.find(id.c_str());
  if (it != m_PictureMaps->map.end())
    return wxBitmap((*it).second);
  else
  {
    mafLogMessage("mafPictureFactory: bmp with id = %s not found",id.c_str());
    return wxNullBitmap;
  }
}
//----------------------------------------------------------------------------
wxImage mafPictureFactory::GetImg(wxString id)
//----------------------------------------------------------------------------
{
  mafPicMap::iterator it = m_PictureMaps->map.find(id.c_str());
  if (it != m_PictureMaps->map.end())
    return (*it).second;
  else
  {
    mafLogMessage("mafPictureFactory: img with id = %s not found",id.c_str());
    return wxNullImage;
  }
}
//----------------------------------------------------------------------------
wxIcon mafPictureFactory::GetIcon(wxString id)
//----------------------------------------------------------------------------
{
  mafPicMap::iterator it = m_PictureMaps->map.find(id.c_str());
  if (it != m_PictureMaps->map.end())
  {
    wxBitmap bmp = wxBitmap((*it).second);
    wxIcon icon;
    icon.CopyFromBitmap(bmp);
    return icon;
  }
  else
  {
    mafLogMessage("mafPictureFactory: icon with id = %s not found",id.c_str());
    return wxNullIcon;
  }
}
//----------------------------------------------------------------------------
void mafPictureFactory::AddVmePic(wxString id,char** xpm)
//----------------------------------------------------------------------------
{
  m_PictureMaps->vme_map[id.c_str()] = wxImage(xpm);
}
//----------------------------------------------------------------------------
wxBitmap mafPictureFactory::GetVmePic(wxString id)
//----------------------------------------------------------------------------
{
  mafPicMap::iterator it = m_PictureMaps->vme_map.find(id.c_str());
  if (it != m_PictureMaps->vme_map.end())
    return wxBitmap((*it).second);
  else
  {
    mafLogMessage("mafPictureFactory: vme-pic with id = %s not found",id.c_str());
    return wxNullBitmap;
  }
}
//----------------------------------------------------------------------------
void mafPictureFactory::GetVmeNames( std::vector<wxString>& v )
//----------------------------------------------------------------------------
{
  v.clear();
  for (mafPicMap::iterator it = m_PictureMaps->vme_map.begin(); it != m_PictureMaps->vme_map.end(); it++)
  {
    wxString s = ((*it).first).c_str();
    v.push_back(s);
  }
}
//----------------------------------------------------------------------------
mafPictureFactory* mafPictureFactory::GetPictureFactory()
//----------------------------------------------------------------------------
{
  static mafPictureFactory pictureFactory;
  return &pictureFactory;
}
//----------------------------------------------------------------------------
bool mafPictureFactory::GetPicsInitialized()
//----------------------------------------------------------------------------
{
  return mafPics_Initialized;
}
/*
NOTE FOR SILVANO

#include "mafPic.h"
#include "vme.xpm"
mafAddPic(vme_xpm);            //syntax error
int foo = mafAddPic(vme_xpm);  //correct, but will be called before ThePicMap has been created
*/

#ifdef MAF_USE_VTK
    #include "vtkUnsignedCharArray.h"
    #include "vtkPointData.h"

    //----------------------------------------------------------------------------
    vtkImageData* mafPictureFactory::GetVTKImg(wxString id)
    //----------------------------------------------------------------------------
    {
      mafPicMap::iterator it = m_PictureMaps->map.find(id.c_str());
      if (it == m_PictureMaps->map.end())
      {
        mafLogMessage("mafPictureFactory: icon with id = %s not found",id.c_str());
        return NULL;
      }

      wxBitmap bmp = wxBitmap((*it).second);
      wxImage  img = bmp.ConvertToImage();
      wxImage  flipped = img.Mirror(false); // flip vertical

      bool HasMask = img.HasMask();
      unsigned char maskR, maskG, maskB;
      int NumberOfComponents = 3;
      if(HasMask)
      {
        maskR = img.GetMaskRed();
        maskG = img.GetMaskGreen();
        maskB = img.GetMaskBlue();
        NumberOfComponents = 4;
      }

      vtkUnsignedCharArray *buffer;
      vtkNEW(buffer);
      buffer->SetNumberOfComponents(NumberOfComponents);

      unsigned char *p = flipped.GetData();
      assert(p);
      for(int i=0; i < img.GetWidth() * img.GetHeight(); i++)
      {
        unsigned char r = *p++;
        unsigned char g = *p++;
        unsigned char b = *p++;
        unsigned char alpha = 255;

        if(HasMask)
        {
          if( r == maskR && g == maskG && b == maskB ) alpha = 0;
          buffer->InsertNextTuple4( r, g, b, alpha );
        }
        else
          buffer->InsertNextTuple3( r, g, b );
      }

      vtkImageData *vtkimg;
      vtkNEW(vtkimg);
      vtkimg->SetNumberOfScalarComponents(NumberOfComponents);
      vtkimg->SetScalarTypeToUnsignedChar();
      vtkimg->SetDimensions( img.GetWidth(), img.GetHeight(), 1 );
      vtkimg->SetUpdateExtentToWholeExtent();
      assert( vtkimg->GetPointData() );
      vtkimg->GetPointData()->SetScalars(buffer);
      return  vtkimg; // SHOULD BE DELETED BY THE CALLER 
    }
#endif  //MAF_USE_VTK
