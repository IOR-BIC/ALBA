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


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaPics.h" 
#include <map>
#include <string>

//----------------------------------------------------------------------------
// albaPictureFactory_Pimpl
//----------------------------------------------------------------------------
typedef std::map<wxString,wxImage> albaPicMap;

struct albaPictureFactory_Pimpl
{
  public:
  albaPicMap map;
  albaPicMap vme_map; //here are kept the vme icons
};
//----------------------------------------------------------------------------
// the albaPic Singleton
//----------------------------------------------------------------------------
//albaPictureFactory albaPics;
static bool albaPics_Initialized = false;

//----------------------------------------------------------------------------
albaPictureFactory::albaPictureFactory()
//----------------------------------------------------------------------------
{
  m_PictureMaps = new albaPictureFactory_Pimpl();
}
//----------------------------------------------------------------------------
void albaPictureFactory::Initialize()
//----------------------------------------------------------------------------
{
  albaPics_Initialized = true;
   #include <pic/FRAME_ICON16x16.xpm>
    albaADDPIC(FRAME_ICON16x16);
    #include <pic/FRAME_ICON32x32.xpm>
    albaADDPIC(FRAME_ICON32x32);
    #include <pic/CLOSE_SASH.xpm>
    albaADDPIC(CLOSE_SASH);
    #include <pic/ROLLOUT_CLOSE.xpm>
    albaADDPIC(ROLLOUT_CLOSE);
    #include <pic/ROLLOUT_OPEN.xpm>
    albaADDPIC(ROLLOUT_OPEN);
    #include <pic/FILE_NEW.xpm>    
    albaADDPIC(FILE_NEW);
    #include <pic/FILE_OPEN.xpm>
    albaADDPIC(FILE_OPEN);
    #include <pic/FILE_SAVE.xpm>
    albaADDPIC(FILE_SAVE);
    #include <pic/FLYTO.xpm>
    albaADDPIC(FLYTO);
    #include <pic/INFO.xpm>
    albaADDPIC(INFO);
    #include <pic/PRINT.xpm>
    albaADDPIC(PRINT);
    #include <pic/PRINT_PREVIEW.xpm>
    albaADDPIC(PRINT_PREVIEW);
    #include <pic/SPLASH_SCREEN.xpm>
    albaADDPIC(SPLASH_SCREEN);
    #include <pic/MDICHILD_ICON.xpm>
    albaADDPIC(MDICHILD_ICON);
    #include <pic/OP_COPY.xpm>
    albaADDPIC(OP_COPY);
    #include <pic/OP_CUT.xpm>
    albaADDPIC(OP_CUT);
    #include <pic/OP_PASTE.xpm>
    albaADDPIC(OP_PASTE);
	#include <pic/OP_RENAME.xpm>
	albaADDPIC(OP_RENAME);
	#include <pic/OP_FIND.xpm>
	albaADDPIC(OP_FIND);
    #include <pic/OP_REDO.xpm>
    albaADDPIC(OP_REDO);
    #include <pic/OP_UNDO.xpm>
    albaADDPIC(OP_UNDO);
    #include <pic/PIC_BACK.xpm>
    albaADDPIC(PIC_BACK);
    #include <pic/PIC_BOTTOM.xpm>
    albaADDPIC(PIC_BOTTOM);
    #include <pic/PIC_FRONT.xpm>
    albaADDPIC(PIC_FRONT);
    #include <pic/PIC_LEFT.xpm>
    albaADDPIC(PIC_LEFT);
    #include <pic/PIC_RIGHT.xpm>
    albaADDPIC(PIC_RIGHT);
    #include <pic/PIC_TOP.xpm>
    albaADDPIC(PIC_TOP);
		#include <pic/PIC_ORTHO.xpm>
		albaADDPIC(PIC_ORTHO);
    #include <pic/TIME_BEGIN.xpm>
    albaADDPIC(TIME_BEGIN);
    #include <pic/TIME_END.xpm>      
    albaADDPIC(TIME_END);
    #include <pic/TIME_NEXT.xpm>
    albaADDPIC(TIME_NEXT);
    #include <pic/TIME_PLAY.xpm>
    albaADDPIC(TIME_PLAY);
    #include <pic/TIME_PREV.xpm>
    albaADDPIC(TIME_PREV);
    #include <pic/TIME_STOP.xpm>
    albaADDPIC(TIME_STOP);
    #include <pic/MOVIE_RECORD.xpm>
    albaADDPIC(MOVIE_RECORD);
    #include <pic/ZOOM.xpm>
    albaADDPIC(ZOOM);
    #include <pic/ZOOM_ALL.xpm>
    albaADDPIC(ZOOM_ALL);
    #include <pic/ZOOM_SEL.xpm>
    albaADDPIC(ZOOM_SEL);

		#include "pic/CAMERA.xpm"
		albaADDPIC(CAMERA);
		#include "pic/IMAGE_PREVIEW.xpm"
		albaADDPIC(IMAGE_PREVIEW);

    // these are used by the ListCtrl
    #include <pic/NODE_YELLOW.xpm>
    albaADDPIC(NODE_YELLOW);
    #include <pic/NODE_RED.xpm>
    albaADDPIC(NODE_RED);
    #include <pic/NODE_BLUE.xpm>
    albaADDPIC(NODE_BLUE);
    #include <pic/NODE_GRAY.xpm>
    albaADDPIC(NODE_GRAY);

    #include <pic/DISABLED.xpm>
    albaADDPIC(DISABLED);
    #include <pic/RADIO_ON.xpm>
    albaADDPIC(RADIO_ON);
    #include <pic/RADIO_OFF.xpm>
    albaADDPIC(RADIO_OFF);
    #include <pic/CHECK_ON.xpm>
    albaADDPIC(CHECK_ON);
    #include <pic/CHECK_OFF.xpm>
    albaADDPIC(CHECK_OFF);

		#include <pic/OP_HELP.xpm>
		albaADDPIC(OP_HELP);

		#include <pic/BUTTON_HELP.xpm>
		albaADDPIC(BUTTON_HELP);

		#include <pic/HINT_IMAGE_ICON.xpm>
		albaADDPIC(HINT_IMAGE_ICON);
		#include <pic/INFO_IMAGE_ICON.xpm>
		albaADDPIC(INFO_IMAGE_ICON);
		#include <pic/WARN_IMAGE_ICON.xpm>
		albaADDPIC(WARN_IMAGE_ICON);

#include <pic/TOOL_PEN.xpm>
		albaADDPIC(TOOL_PEN);
#include <pic/TOOL_FILL.xpm>
		albaADDPIC(TOOL_FILL);
#include <pic/TOOL_BRUSH.xpm>
		albaADDPIC(TOOL_BRUSH);
#include <pic/TOOL_BRUSH2.xpm>
		albaADDPIC(TOOL_BRUSH2);
#include <pic/TOOL_ERASE.xpm>
		albaADDPIC(TOOL_ERASE);
#include <pic/TOOL_CONNECTIVITY.xpm>
		albaADDPIC(TOOL_CONNECTIVITY);		

#include <pic/REVERSE_LUT_ICON.xpm>
		albaADDPIC(REVERSE_LUT_ICON);
#include <pic/SHOW_RULER_ICON.xpm>
		albaADDPIC(SHOW_RULER_ICON);		
}
//----------------------------------------------------------------------------
albaPictureFactory::~albaPictureFactory()
//----------------------------------------------------------------------------
{
  cppDEL(m_PictureMaps);
}
//----------------------------------------------------------------------------
void albaPictureFactory::Add(wxString id,char** xpm)
//----------------------------------------------------------------------------
{
  m_PictureMaps->map[id] = wxImage(xpm);
}
//----------------------------------------------------------------------------
wxBitmap albaPictureFactory::GetBmp(wxString id)
//----------------------------------------------------------------------------
{
  albaPicMap::iterator it = m_PictureMaps->map.find(id.c_str());
  if (it != m_PictureMaps->map.end())
    return wxBitmap((*it).second);
  else
  {
    albaLogMessage("albaPictureFactory: bmp with id = %s not found",id.c_str());
    return wxNullBitmap;
  }
}
//----------------------------------------------------------------------------
wxImage albaPictureFactory::GetImg(wxString id)
//----------------------------------------------------------------------------
{
  albaPicMap::iterator it = m_PictureMaps->map.find(id.c_str());
  if (it != m_PictureMaps->map.end())
    return (*it).second;
  else
  {
    albaLogMessage("albaPictureFactory: img with id = %s not found",id.c_str());
    return wxNullImage;
  }
}
//----------------------------------------------------------------------------
wxIcon albaPictureFactory::GetIcon(wxString id)
//----------------------------------------------------------------------------
{
  albaPicMap::iterator it = m_PictureMaps->map.find(id.c_str());
  if (it != m_PictureMaps->map.end())
  {
    wxBitmap bmp = wxBitmap((*it).second);
    wxIcon icon;
    icon.CopyFromBitmap(bmp);
    return icon;
  }
  else
  {
    albaLogMessage("albaPictureFactory: icon with id = %s not found",id.c_str());
    return wxNullIcon;
  }
}
//----------------------------------------------------------------------------
void albaPictureFactory::AddVmePic(wxString id,char** xpm)
//----------------------------------------------------------------------------
{
  m_PictureMaps->vme_map[id.c_str()] = wxImage(xpm);
}
//----------------------------------------------------------------------------
wxBitmap albaPictureFactory::GetVmePic(wxString id)
//----------------------------------------------------------------------------
{
  albaPicMap::iterator it = m_PictureMaps->vme_map.find(id.c_str());
  if (it != m_PictureMaps->vme_map.end())
    return wxBitmap((*it).second);
  else
  {
    albaLogMessage("albaPictureFactory: vme-pic with id = %s not found",id.c_str());
    return wxNullBitmap;
  }
}
//----------------------------------------------------------------------------
void albaPictureFactory::GetVmeNames( std::vector<wxString>& v )
//----------------------------------------------------------------------------
{
  v.clear();
  for (albaPicMap::iterator it = m_PictureMaps->vme_map.begin(); it != m_PictureMaps->vme_map.end(); it++)
  {
    wxString s = ((*it).first).c_str();
    v.push_back(s);
  }
}
//----------------------------------------------------------------------------
albaPictureFactory* albaPictureFactory::GetPictureFactory()
//----------------------------------------------------------------------------
{
  static albaPictureFactory pictureFactory;
  return &pictureFactory;
}
//----------------------------------------------------------------------------
bool albaPictureFactory::GetPicsInitialized()
//----------------------------------------------------------------------------
{
  return albaPics_Initialized;
}
/*
NOTE FOR SILVANO

#include "albaPic.h"
#include "vme.xpm"
albaAddPic(vme_xpm);            //syntax error
int foo = albaAddPic(vme_xpm);  //correct, but will be called before ThePicMap has been created
*/

#ifdef ALBA_USE_VTK
    #include "vtkUnsignedCharArray.h"
    #include "vtkPointData.h"

    //----------------------------------------------------------------------------
    vtkImageData* albaPictureFactory::GetVTKImg(wxString id)
    //----------------------------------------------------------------------------
    {
      albaPicMap::iterator it = m_PictureMaps->map.find(id.c_str());
      if (it == m_PictureMaps->map.end())
      {
        albaLogMessage("albaPictureFactory: icon with id = %s not found",id.c_str());
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
#endif  //ALBA_USE_VTK
