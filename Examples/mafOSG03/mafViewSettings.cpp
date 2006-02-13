/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewSettings.cpp,v $
  Language:  C++
  Date:      $Date: 2006-02-13 15:50:49 $
  Version:   $Revision: 1.1 $
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

#include "mmgGui.h"
#include "mafViewSettings.h"
#include "mmgCheckListBox.h"

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------
enum 
{
  ID_FOO = MINID,
  ID_GETPOS,
  ID_SETPOS,
  ID_LOADPOSINI,
  ID_SAVEPOSINI,
  ID_FOGCOMBO,
  ID_FOGCOLOR,
  ID_SETFOGDIST,
  ID_GETFOG,
  ID_LOADFOGINI,
  ID_SAVEFOGINI,
  ID_SKYCOMBO,
  ID_SKYCOLOR,
  ID_GROUNDLEVEL,
  ID_LOADSKYINI,
  ID_SAVESKYINI,
  ID_VOLFOG,
  ID_LOADVOLFOGINI,
  ID_SAVEVOLFOGINI,
  ID_DRAWSTYLECOMBO,
};
//----------------------------------------------------------------------------
mafViewSettings::mafViewSettings(mafObserver *listener)
//----------------------------------------------------------------------------
{
  assert(listener);
  m_Listener = listener;
  //m_pf = NULL;

  //------------
  // create the GUI:
  //------------
  m_gui = new mmgGui(this);
  m_gui->SetListener(this);

  //m_gui->Label(""); 

  // Camera Position
  m_gui->Label("CAMERA POSITION:",true); 
  m_gui->Label(""); 
  m_gui->Vector (ID_SETPOS  , "x,y,z", m_xyz);
  m_gui->Vector (ID_SETPOS  , "h,p,r", m_hpr, 0.0f, 10000.0f, 0.0f, 10000.0f, 0.0f, 10000.0f);
  m_gui->VectorN(ID_SETPOS  , "steps: h,v", m_steps,	2);
  m_gui->Button(ID_GETPOS,"get camera view");
  m_gui->Button(ID_LOADPOSINI,"load from ini");
  m_gui->Button(ID_SAVEPOSINI,"save to ini");

  //m_gui->Divider(); 
  //m_gui->Divider(1); // gray line 
  //m_gui->Label(""); 

  // Fog
  m_gui->Label("FOG:",true); 
  m_gui->Label(""); 
  m_fog_idx = 0;
  m_fog_choices[0] = "None";
  m_fog_choices[1] = "Linear";
  m_fog_choices[2] = "Exponential";
  m_fog_choices[3] = "Exponential^2";
  m_gui->Combo(ID_FOGCOMBO, "type", &m_fog_idx, 4, m_fog_choices);
  m_fogcolor.Set(255, 255, 255);
  m_gui->Color(ID_FOGCOLOR, "color", &m_fogcolor);
  m_fogdist[0] = m_fogdist[1] = 0.0f;
  m_gui->VectorN(ID_SETFOGDIST, "min, max", m_fogdist,	2);
  m_gui->Button(ID_GETFOG,"get fog settings");
  m_gui->Button(ID_LOADFOGINI,"load fog settings from ini");
  m_gui->Button(ID_SAVEFOGINI,"save fog settings to ini");

  //m_gui->Divider();
  //m_gui->Divider(1); // gray line 
  //m_gui->Label(""); 

  // Sky
  m_gui->Label("SKY:",true); 
  m_gui->Label(""); 
  m_sky_idx = m_last_sky_pos = 0;
  m_sky_choices[0] = "sky top";
  m_sky_choices[1] = "sky bottom";
  m_sky_choices[2] = "horizon";
  m_sky_choices[3] = "ground far";
  m_sky_choices[4] = "ground near";
  m_gui->Combo(ID_SKYCOMBO, "color", &m_sky_idx, 5, m_sky_choices);
  m_fogcolor.Set(255, 255, 255);
  m_gui->Color(ID_SKYCOLOR, "color", &m_skycolor);
//  m_gui->Float(ID_GROUNDLEVEL, "ground level", &m_Sky_Data.GroundLevel, -1000.0f, 1000.0f);
  m_gui->Button(ID_LOADSKYINI, "load sky settings from ini");
  m_gui->Button(ID_SAVESKYINI, "save sky settings to ini");

  //m_gui->Divider();
  //m_gui->Divider(1); // gray line 
  //m_gui->Label(""); 
/*
  // Vol Fog
  m_gui->Label("LAYERED FOG:",true); 
  m_gui->Label(""); 
  m_gui->Label("                 Quote    Density"); 
  m_gui->VectorN(ID_VOLFOG, "ground b.", &m_VolFog_Data.ground0_el,	2);
  m_gui->VectorN(ID_VOLFOG, "ground t.", &m_VolFog_Data.ground1_el,	2);
  m_gui->VectorN(ID_VOLFOG, "transition b.", &m_VolFog_Data.transition0_el,	2);
  m_gui->VectorN(ID_VOLFOG, "clouds b.", &m_VolFog_Data.clouds0_el,	2);
  m_gui->VectorN(ID_VOLFOG, "clouds t.", &m_VolFog_Data.clouds1_el,	2);
  m_gui->VectorN(ID_VOLFOG, "transition t.", &m_VolFog_Data.transition1_el,	2);
  m_gui->VectorN(ID_VOLFOG, "max distance", &m_VolFog_Data.max_distance, 1);
  m_gui->Bool_  (ID_VOLFOG, "enabled", &m_VolFog_Data.enabled);
  m_gui->Button (ID_LOADVOLFOGINI, "load vol. fog settings from ini");
  m_gui->Button (ID_SAVEVOLFOGINI, "save vol. fog settings to ini");
*/
  //m_gui->Divider();
  //m_gui->Divider(1); // gray line 
  //m_gui->Label(""); 

  // Draw Style
  m_gui->Label("DRAW STYLE:",true);
  m_gui->Label(""); 
  m_draw_style = 5;
  m_drawstyle_chiose[0] = "Points";
  m_drawstyle_chiose[1] = "Lines";
  m_drawstyle_chiose[2] = "Dashed";
  m_drawstyle_chiose[3] = "Haloed";
  m_drawstyle_chiose[4] = "Hidden";
  m_drawstyle_chiose[5] = "Filled (standard)";
  m_drawstyle_chiose[6] = "Scribed";
  m_gui->Combo(ID_DRAWSTYLECOMBO, "style", &m_draw_style, 7, m_drawstyle_chiose);

  m_gui->Label("");
}
//----------------------------------------------------------------------------
mafViewSettings::~mafViewSettings( ) 
//----------------------------------------------------------------------------
{
	cppDEL(m_gui);
}
/*
//----------------------------------------------------------------------------
void mafViewSettings::SetEngine(TPerformer *pf)
//----------------------------------------------------------------------------
{
  assert( pf );
  assert( m_pf == NULL );
  m_pf = pf;
  m_skycolor.Set((char)m_Sky_Data.SkyTopColors[0] * 255,
          (char)m_Sky_Data.SkyTopColors[1] * 255, (char)m_Sky_Data.SkyTopColors[2] * 255);
}
*/
//----------------------------------------------------------------------------
mmgGui* mafViewSettings::GetGui()
//----------------------------------------------------------------------------
{
	return m_gui;
}
//----------------------------------------------------------------------------
void mafViewSettings::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  /*
  TCameraData *Camera;
  wxString integer;
  V_IniFile *ini;
  int fogtype, res;
  float r, g, b;
  TFog_Data FogStruct;
  Camera = new TCameraData;
  */

  if (mafEvent *e = mafEvent::SafeDownCast(maf_event)) //SIL - adesso si deve fare un cast dell'evento
  {
    switch(e->GetId())
    {
      /*
      case ID_GETPOS:
        m_pf->PerformerGetCameraView(Camera);
        LoadEditsFromCamera(Camera);
        m_gui->Update();
      break;
      case ID_SETPOS:
        Camera->Camera_X = m_xyz[0];
        Camera->Camera_Y = m_xyz[1];
        Camera->Camera_Z = m_xyz[2];
        Camera->Camera_H = m_hpr[0];
        Camera->Camera_P = m_hpr[1];
        Camera->Camera_R = m_hpr[2];
        m_pf->PerformerSetCameraView(Camera);
        m_pf->pf_step_h = m_steps[0];
        m_pf->pf_step_v = m_steps[1];
        m_pf->PerformerRender(false);
      break;
      case ID_LOADPOSINI:
        ini = new V_IniFile(wxString(m_pf->pf_model_pathname) + INI_FILE);
        Camera = new TCameraData;
        if (m_pf->pf_PViewOn == 0)
          res = ini->GetCameraData(FILES_CAMERA, Camera);
        else
        {
          integer.Printf("PVIEW_%d", m_pf->pf_PViewOn);
          res = ini->GetCameraData(integer, Camera);
        }
        if (res != -1)
        {
          LoadEditsFromCamera(Camera);
          m_pf->PerformerSetCameraView(Camera);
          m_pf->PerformerRender(false);
        }
        delete ini;
      break;
      case ID_SAVEPOSINI:
        ini = new V_IniFile(wxString(m_pf->pf_model_pathname) + INI_FILE);
        Camera = new TCameraData;
        Camera->Camera_X = m_xyz[0];
        Camera->Camera_Y = m_xyz[1];
        Camera->Camera_Z = m_xyz[2];
        Camera->Camera_H = m_hpr[0];
        Camera->Camera_P = m_hpr[1];
        Camera->Camera_R = m_hpr[2];

        if (m_pf->pf_PViewOn == 0)
        {
          ini->SetCameraData(FILES_CAMERA, Camera);
          ini->SetStepAmount(FILES_STEPAMOUNT, m_pf->pf_step_h, m_pf->pf_step_v);
        }
        else
        {
          integer.Printf("PVIEW_%d", m_pf->pf_PViewOn);
          ini->SetCameraData(integer, Camera);
          ini->SetStepAmount(integer, m_pf->pf_step_h, m_pf->pf_step_v);
        }
        delete Camera;
        delete ini;
      break;
      
      case ID_FOGCOMBO:
      case ID_FOGCOLOR:
      case ID_SETFOGDIST:  
        switch (m_fog_idx)
        {
          case 0:
            fogtype = 0;
            break;
          case 1:
            fogtype = PFFOG_PIX_LIN;
            break;
          case 2:
            fogtype = PFFOG_PIX_EXP;
            break;
          case 3:
            fogtype = PFFOG_PIX_EXP2;
            break;
        }
        r = m_fogcolor.Red() / 255.0f;
        g = m_fogcolor.Green() / 255.0f;
        b = m_fogcolor.Blue() / 255.0f;
        m_pf->PerformerSetFog(fogtype, r, g, b, m_fogdist[0], m_fogdist[1]); 
        m_pf->PerformerRender(false);
      break;

      case ID_GETFOG:
        LoadEditsFromFog();
      break;

      case ID_LOADFOGINI:
        ini = new V_IniFile(wxString(m_pf->pf_model_pathname) + INI_FILE);
        ini->GetFogData(FILES_FOG, &FogStruct);
        m_pf->PerformerSetFog(FogStruct.Type, FogStruct.Color[0], FogStruct.Color[1], FogStruct.Color[2],
                      FogStruct.Near, FogStruct.Far);
        m_pf->PerformerRender(false);
        LoadEditsFromFog();
        delete ini;
      break;

      case ID_SAVEFOGINI:
        ini = new V_IniFile(wxString(m_pf->pf_model_pathname) + INI_FILE);
        FogStruct.Color[0] = m_fogcolor.Red() / 255.0f;
        FogStruct.Color[1] = m_fogcolor.Green() / 255.0f;
        FogStruct.Color[2] = m_fogcolor.Blue() / 255.0f;
        FogStruct.Near = m_fogdist[0];
        FogStruct.Far = m_fogdist[1];
        switch (m_fog_idx)
        {
          case 0:
            fogtype = 0;
            break;
          case 1:
            fogtype = PFFOG_PIX_LIN;
            break;
          case 2:
            fogtype = PFFOG_PIX_EXP;
            break;
          case 3:
            fogtype = PFFOG_PIX_EXP2;
            break;
        }
        FogStruct.Type = fogtype;
        ini->SetFogData(FILES_FOG, FogStruct);
        delete ini;
      break;

      case ID_SKYCOLOR:
        if (m_sky_idx == 0)
        {
          m_Sky_Data.SkyTopColors[0] = m_skycolor.Red() / 255.0f;
          m_Sky_Data.SkyTopColors[1] = m_skycolor.Green() / 255.0f;
          m_Sky_Data.SkyTopColors[2] = m_skycolor.Blue() / 255.0f;
        }
        else if (m_sky_idx == 1)
        {
          m_Sky_Data.SkyBotColors[0] = m_skycolor.Red() / 255.0f;
          m_Sky_Data.SkyBotColors[1] = m_skycolor.Green() / 255.0f;
          m_Sky_Data.SkyBotColors[2] = m_skycolor.Blue() / 255.0f;
        }
        else if (m_sky_idx == 2)
        {
          m_Sky_Data.SkyHorizColors[0] = m_skycolor.Red() / 255.0f;
          m_Sky_Data.SkyHorizColors[1] = m_skycolor.Green() / 255.0f;
          m_Sky_Data.SkyHorizColors[2] = m_skycolor.Blue() / 255.0f;
        }
        else if (m_sky_idx == 3)
        {
          m_Sky_Data.GroundFarColors[0] = m_skycolor.Red() / 255.0f;
          m_Sky_Data.GroundFarColors[1] = m_skycolor.Green() / 255.0f;
          m_Sky_Data.GroundFarColors[2] = m_skycolor.Blue() / 255.0f;
        }
        else if (m_sky_idx == 4)
        {
          m_Sky_Data.GroundNearColors[0] = m_skycolor.Red() / 255.0f;
          m_Sky_Data.GroundNearColors[1] = m_skycolor.Green() / 255.0f;
          m_Sky_Data.GroundNearColors[2] = m_skycolor.Blue() / 255.0f;
        }
        SetSkyColors();
      break;

      case ID_SKYCOMBO:
        if (m_sky_idx == m_last_sky_pos) break;
        if (m_sky_idx == 0)
          m_skycolor.Set((char)m_Sky_Data.SkyTopColors[0] * 255,
            (char)m_Sky_Data.SkyTopColors[1] * 255, (char)m_Sky_Data.SkyTopColors[2] * 255);
        else if (m_sky_idx == 1)
          m_skycolor.Set((char)m_Sky_Data.SkyBotColors[0] * 255,
            (char)m_Sky_Data.SkyBotColors[1] * 255, (char)m_Sky_Data.SkyBotColors[2] * 255);
        else if (m_sky_idx == 2)
          m_skycolor.Set((char)m_Sky_Data.SkyHorizColors[0] * 255,
            (char)m_Sky_Data.SkyHorizColors[1] * 255, (char)m_Sky_Data.SkyHorizColors[2] * 255);
        else if (m_sky_idx == 3)
          m_skycolor.Set((char)m_Sky_Data.GroundFarColors[0] * 255,
            (char)m_Sky_Data.GroundFarColors[1] * 255, (char)m_Sky_Data.GroundFarColors[2] * 255);
        else if (m_sky_idx == 4)
          m_skycolor.Set((char)m_Sky_Data.GroundNearColors[0] * 255,
            (char)m_Sky_Data.GroundNearColors[1] * 255, (char)m_Sky_Data.GroundNearColors[2] * 255);
        m_gui->Update();
        m_last_sky_pos = m_sky_idx;
      break;

      case ID_LOADSKYINI:
        if (m_pf->pf_PViewOn == 0)
          integer.Printf(FILES_SKYGROUND);
        else
          integer.Printf("FILES_PVIEW%d", m_pf->pf_PViewOn);

        ini = new V_IniFile(wxString(m_pf->pf_model_pathname) + INI_FILE);
        if (ini->GetSkyGroundData(integer, &m_Sky_Data))
        {
          m_pf->PerformerSetSkyColors(&m_Sky_Data);
          m_pf->PerformerRender(false);
          m_gui->Update();
        }
        delete ini;
      break;

      case ID_SAVESKYINI:
        if (m_pf->pf_PViewOn == 0)
          integer.Printf(FILES_SKYGROUND);
        else
          integer.Printf("FILES_PVIEW%d", m_pf->pf_PViewOn);

        ini = new V_IniFile(wxString(m_pf->pf_model_pathname) + INI_FILE);
        ini->SetSkyGroundData(integer, m_Sky_Data);
        delete ini;
      break;

      case ID_GROUNDLEVEL:
        SetSkyColors();
      break;

      case ID_VOLFOG:
        SetVolFog();
      break;

      case ID_LOADVOLFOGINI:
        ini = new V_IniFile(wxString(m_pf->pf_model_pathname) + INI_FILE);
        ini->GetVolFogData(&m_VolFog_Data);
        SetVolFog();
        m_gui->Update();
        delete ini;
      break;

      case ID_SAVEVOLFOGINI:
        ini = new V_IniFile(wxString(m_pf->pf_model_pathname) + INI_FILE);
          ini->SetVolFogData(m_VolFog_Data);
        delete ini;
      break;

      case ID_DRAWSTYLECOMBO:
        int DrawStyle; 
        switch(m_draw_style)
        {
          case 0:
            DrawStyle = PFUSTYLE_POINTS;
          break;

          case 1:
            DrawStyle = PFUSTYLE_LINES;
          break;

          case 2:
            DrawStyle = PFUSTYLE_DASHED;
          break;

          case 3:
            DrawStyle = PFUSTYLE_HALOED;
          break;

          case 4:
            DrawStyle = PFUSTYLE_HIDDEN;
          break;

          case 5:
            DrawStyle = PFUSTYLE_FILLED;
          break;

          case 6:
            DrawStyle = PFUSTYLE_SCRIBED;
          break;
        }
        m_pf->PerformerSetDrawStyle(DrawStyle, 1.0f, 1.0f, 1.0f);   
        m_pf->PerformerRender(false);
      break;
      */
      default:
        mafEventMacro(*maf_event);
      break; 
    }
  }
}
/*
//----------------------------------------------------------------------------
void mafViewSettings::LoadEditsFromCamera (TCameraData *Camera)
//----------------------------------------------------------------------------
{
  m_xyz[0] = Camera->Camera_X;
  m_xyz[1] = Camera->Camera_Y;
  m_xyz[2] = Camera->Camera_Z;
  m_hpr[0] = Camera->Camera_H;
  m_hpr[1] = Camera->Camera_P;
  m_hpr[2] = Camera->Camera_R;
  m_steps[0] = m_pf->pf_step_h;
  m_steps[1] = m_pf->pf_step_v; 
  m_gui->Update();
}
//----------------------------------------------------------------------------
void mafViewSettings::LoadEditsFromFog(void)
//----------------------------------------------------------------------------
{
  pfFog* fog;
  float r, g, b, min, max;
  int fogtype;

  if (pfGetEnable(PFEN_FOG) == PF_OFF)
  {
    m_fog_idx = 0;
    m_fogcolor.Set(200, 200, 200);
    fogtype = 0;
    m_fogdist[0] = 0.0f;
    m_fogdist[1] = 1000.0f;
  }
  else
  {
    fog = m_pf->PerformerGetFog();
    fogtype = pfGetFogType(fog);
    switch(fogtype)
    {
        case PFFOG_PIX_LIN:
          m_fog_idx = 1;
          break;

        case PFFOG_PIX_EXP:
          m_fog_idx = 2;
          break;

        case PFFOG_PIX_EXP2:
          m_fog_idx = 3;
          break;
    }
    pfGetFogColor(fog, &r, &g, &b);
    m_fogcolor.Set((char)r * 256, (char)g * 256, (char)b * 256);
    pfGetFogRange(fog, &min, &max);
    m_fogdist[0] = min;
    m_fogdist[1] = max;
  }
  m_gui->Update();
}
//----------------------------------------------------------------------------
void mafViewSettings::SetSkyColors(void)
//----------------------------------------------------------------------------
{
  m_pf->PerformerSetSkyColors(&m_Sky_Data);
  m_pf->PerformerRender(false);
}
//----------------------------------------------------------------------------
void mafViewSettings::SetVolFog(void)
//----------------------------------------------------------------------------
{
  if (m_VolFog_Data.enabled)
  {
    m_pf->PerformerDeleteVolFog();
    m_pf->PerformerCreateVolFog();
    m_pf->PerformerAddVolFogPoints(&m_VolFog_Data);
  }
  else
    m_pf->PerformerDeleteVolFog();
  m_pf->PerformerRender(false);
}
*/