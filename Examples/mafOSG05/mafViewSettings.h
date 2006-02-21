/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafViewSettings.h,v $
Language:  C++
Date:      $Date: 2006-02-21 16:14:11 $
Version:   $Revision: 1.1 $
Authors:   Silvano Imboden
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafViewSettings_H__
#define __mafViewSettings_H__

#include <wx/notebook.h>
#include <wx/icon.h>

#include "mafDecl.h"
#include "mafEvent.h"
// #include "Performer.h"
// #include "IniFiles.h"
//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mmgGui;
// class TPerformer;
//----------------------------------------------------------------------------
// mafViewSettings :
//----------------------------------------------------------------------------
class mafViewSettings : public mafObserver
{
public:
  // TSkyGround_Data m_Sky_Data;
  // TVolFog_Data    m_VolFog_Data;

                  mafViewSettings(mafObserver *listener);
  virtual         ~mafViewSettings(); 
  virtual void    OnEvent(mafEventBase *maf_event);

  virtual mmgGui *GetGui();
      //    void    SetEngine   (TPerformer *pf); 
      //    void    LoadEditsFromCamera (TCameraData *Camera);
      //    void    LoadEditsFromFog(void);
      //    void    SetSkyColors(void);
      //    void    SetVolFog(void);

protected:  
  mafObserver      *m_Listener;
  mmgGui           *m_gui;

  float             m_xyz[3];
  float             m_hpr[3];
  double            m_steps[2];
  wxString          m_fog_choices[4];
  wxColour          m_fogcolor;
  int               m_fog_idx;
  double            m_fogdist[2];
  wxString          m_sky_choices[5];
  wxColour          m_skycolor;
  int               m_sky_idx, m_last_sky_pos;
  float             m_ground_level; //SIL. 13-12-2004: --- sembra inutilizzato
  int               m_draw_style;
  wxString          m_drawstyle_chiose[7];
};
#endif



