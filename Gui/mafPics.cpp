/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPics.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-07 08:34:28 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafPics.h" 
#include <map>
#include <string>

//----------------------------------------------------------------------------
// ThePicMap
//----------------------------------------------------------------------------

typedef std::map<std::string,wxImage> mafPicMap;
mafPicMap ThePicMap;

//----------------------------------------------------------------------------
// ThePicMapInitializer
//----------------------------------------------------------------------------
class PicMapInitializer
{
public:
  PicMapInitializer()
  {
    #include <pic/FRAME_ICON16x16.xpm>   
    mafADDPIC(FRAME_ICON16x16);
    #include <pic/FRAME_ICON32x32.xpm>   
    mafADDPIC(FRAME_ICON32x32);
    #include <pic/CLOSE_SASH.xpm>      
    mafADDPIC(CLOSE_SASH);   
    #include <pic/FILE_NEW.xpm>        
    mafADDPIC(FILE_NEW);
    #include <pic/FILE_OPEN.xpm>       
    mafADDPIC(FILE_OPEN);    
    #include <pic/FILE_SAVE.xpm>       
    mafADDPIC(FILE_SAVE);    
    #include <pic/FLYTO.xpm>           
    mafADDPIC(FLYTO);        
    #include <pic/MDICHILD_ICON.xpm>   
    mafADDPIC(MDICHILD_ICON);
    #include <pic/OP_COPY.xpm>         
    mafADDPIC(OP_COPY);      
    #include <pic/OP_CUT.xpm>          
    mafADDPIC(OP_CUT);       
    #include <pic/OP_PASTE.xpm>        
    mafADDPIC(OP_PASTE);     
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
    #include <pic/ZOOM.xpm>            
    mafADDPIC(ZOOM);         
    #include <pic/ZOOM_ALL.xpm>        
    mafADDPIC(ZOOM_ALL);     
    #include <pic/ZOOM_SEL.xpm>        
    mafADDPIC(ZOOM_SEL);     
  }
};
PicMapInitializer ThePicMapInitializer;

//----------------------------------------------------------------------------
void mafAddPic(wxString id,char** xpm)
//----------------------------------------------------------------------------
{
  ThePicMap[id.c_str()]= wxImage(xpm);
}
//----------------------------------------------------------------------------
wxBitmap mafGetBmp(wxString id)
//----------------------------------------------------------------------------
{
  mafPicMap::iterator it=ThePicMap.find(id.c_str());
  if (it!=ThePicMap.end())
    return wxBitmap((*it).second);
  else
    return wxNullBitmap;
}
//----------------------------------------------------------------------------
wxImage mafGetImg(wxString id)
//----------------------------------------------------------------------------
{
  mafPicMap::iterator it=ThePicMap.find(id.c_str());
  if (it!=ThePicMap.end())
    return (*it).second;
  else
    return wxNullImage;
}
//----------------------------------------------------------------------------
wxIcon mafGetIcon(wxString id)
//----------------------------------------------------------------------------
{
  mafPicMap::iterator it=ThePicMap.find(id.c_str());
  if (it!=ThePicMap.end())
  {
    wxBitmap bmp = wxBitmap((*it).second);
    wxIcon icon;
    icon.CopyFromBitmap(bmp);
    return icon;
  }
  else
    return wxNullIcon;
}


/*
NOTE FOR SILVANO

#include "mafPic.h"
#include "vme.xpm"
mafAddPic(vme_xpm);            //syntax error
int foo = mafAddPic(vme_xpm);  //correct, but will be called before ThePicMap has been created
*/
