/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaLightKit
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaLightKit_H__
#define __albaLightKit_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"

#define MAX_NUM_LIGHT 10

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUIButton;
class vtkLight;
class vtkRenderer;
class albaGUI;

//----------------------------------------------------------------------------
// albaLightKit :
//----------------------------------------------------------------------------
/**
*/
class ALBA_EXPORT albaLightKit : public albaObserver, public albaServiceClient
{
public:
	albaLightKit(wxWindow* parent, vtkRenderer *r, albaObserver *Listener = NULL);
	~albaLightKit(); 
	
	void OnEvent(albaEventBase *alba_event);
	void SetListener(albaObserver *Listener) {m_Listener = Listener;};
	void CreateGui();

	/** Returns the albaLightKit's GUI */
	albaGUI *GetGui() {return m_Gui;};

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

	albaObserver	*m_Listener;

	albaGUIButton  *m_ButtonAdd; 
	albaGUIButton  *m_ButtonRemove;

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
	albaGUI			*m_Gui;
};
#endif
