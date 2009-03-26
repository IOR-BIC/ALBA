/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafLightKit.h,v $
  Language:  C++
  Date:      $Date: 2009-03-26 16:52:47 $
  Version:   $Revision: 1.3.2.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafLightKit_H__
#define __mafLightKit_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"

#define MAX_NUM_LIGHT 10

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUIButton;
class vtkLight;
class vtkRenderer;
class mafGUI;

//----------------------------------------------------------------------------
// mafLightKit :
//----------------------------------------------------------------------------
/**
*/
class mafLightKit : public mafObserver
{
public:
	mafLightKit(wxWindow* parent, vtkRenderer *r, mafObserver *Listener = NULL);
	~mafLightKit(); 
	
	void OnEvent(mafEventBase *maf_event);
	void SetListener(mafObserver *Listener) {m_Listener = Listener;};
	void CreateGui();

	/** Returns the mafLightKit's GUI */
	mafGUI *GetGui() {return m_Gui;};

protected:
	class LightItem {
		public:
			LightItem(wxString n, int onoff,float az,float el,float inten,wxColour c, vtkLight *l) {m_Name = n; m_OnOff = onoff; m_Azimuth = az; m_Elevation = el; m_Intensity = inten; m_Colour = c; m_Light = l;};
			~LightItem() {};

			wxString m_Name;
			int m_OnOff;
			float	m_Azimuth,m_Elevation,m_Intensity;
			wxColour m_Colour;
			vtkLight *m_Light;
	};
	
	/** Add a light to the scene. */
	void AddLight();

	/** Remove the selected light (into the light list listbox) from the scene. */
	void RemoveLight(int sel);

	/** Initialize the interface's parameters. */
	void ParametersInit();

	/** Enable-Disable interface's widgets. */
	void EnableWidget(bool enable);

	mafObserver	*m_Listener;

	mafGUIButton  *m_ButtonAdd; 
	mafGUIButton  *m_ButtonRemove;

	int				 m_LightCounter;
	wxListBox	*m_LightList;
	LightItem *m_SelectedLight;
	LightItem *m_LightItemList[MAX_NUM_LIGHT];
	double		 m_LightAzimuth;
	double		 m_LightElevation;
	double		 m_LightIntensity;
	int				 m_LightOnOff;
	wxColour	 m_LightColor;

	vtkRenderer	*m_LightRenderer;
	
	wxWindow	  *m_LightParentPanel;
	mafGUI			*m_Gui;
};
#endif
