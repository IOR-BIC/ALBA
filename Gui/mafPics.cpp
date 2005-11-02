/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPics.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-02 12:42:10 $
  Version:   $Revision: 1.10 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
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
mafPictureFactory mafPics;
bool mafPics_Initialized=false;

//----------------------------------------------------------------------------
mafPictureFactory::mafPictureFactory()
//----------------------------------------------------------------------------
{
    m_p = new mafPictureFactory_Pimpl();
}
//----------------------------------------------------------------------------
void mafPictureFactory::Initialize()
//----------------------------------------------------------------------------
{
  mafPics_Initialized=true;
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
    #include <pic/INFO.xpm>
    mafADDPIC(INFO);
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
}
//----------------------------------------------------------------------------
mafPictureFactory::~mafPictureFactory()
//----------------------------------------------------------------------------
{
  cppDEL(m_p);
}
//----------------------------------------------------------------------------
void mafPictureFactory::Add(wxString id,char** xpm)
//----------------------------------------------------------------------------
{
  m_p->map[id.c_str()]= wxImage(xpm);
}
//----------------------------------------------------------------------------
wxBitmap mafPictureFactory::GetBmp(wxString id)
//----------------------------------------------------------------------------
{
  mafPicMap::iterator it=m_p->map.find(id.c_str());
  if (it!= m_p->map.end())
    return wxBitmap((*it).second);
  else
  {
    wxLogMessage("mafPictureFactory: bmp with id = %s not found",id.c_str());
    return wxNullBitmap;
  }
}
//----------------------------------------------------------------------------
wxImage mafPictureFactory::GetImg(wxString id)
//----------------------------------------------------------------------------
{
  mafPicMap::iterator it=m_p->map.find(id.c_str());
  if (it!= m_p->map.end())
    return (*it).second;
  else
  {
    wxLogMessage("mafPictureFactory: img with id = %s not found",id.c_str());
    return wxNullImage;
  }
}
//----------------------------------------------------------------------------
wxIcon mafPictureFactory::GetIcon(wxString id)
//----------------------------------------------------------------------------
{
  mafPicMap::iterator it=m_p->map.find(id.c_str());
  if (it!= m_p->map.end())
  {
    wxBitmap bmp = wxBitmap((*it).second);
    wxIcon icon;
    icon.CopyFromBitmap(bmp);
    return icon;
  }
  else
  {
    wxLogMessage("mafPictureFactory: icon with id = %s not found",id.c_str());
    return wxNullIcon;
  }
}
//----------------------------------------------------------------------------
void mafPictureFactory::AddVmePic(wxString id,char** xpm)
//----------------------------------------------------------------------------
{
  m_p->vme_map[id.c_str()]= wxImage(xpm);
}
//----------------------------------------------------------------------------
wxBitmap mafPictureFactory::GetVmePic(wxString id)
//----------------------------------------------------------------------------
{
  mafPicMap::iterator it=m_p->vme_map.find(id.c_str());
  if (it!= m_p->vme_map.end())
    return wxBitmap((*it).second);
  else
  {
    wxLogMessage("mafPictureFactory: vme-pic with id = %s not found",id.c_str());
    return wxNullBitmap;
  }
}
//----------------------------------------------------------------------------
void mafPictureFactory::GetVmeNames( std::vector<wxString>& v )
//----------------------------------------------------------------------------
{
  v.clear();
  for (mafPicMap::iterator it=m_p->vme_map.begin(); it!=m_p->vme_map.end(); it++)
  {
    wxString s = ((*it).first).c_str();
    v.push_back(s);
  }
}
/*
NOTE FOR SILVANO

#include "mafPic.h"
#include "vme.xpm"
mafAddPic(vme_xpm);            //syntax error
int foo = mafAddPic(vme_xpm);  //correct, but will be called before ThePicMap has been created
*/
