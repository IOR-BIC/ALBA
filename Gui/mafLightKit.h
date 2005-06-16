/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafLightKit.h,v $
  Language:  C++
  Date:      $Date: 2005-06-16 11:04:17 $
  Version:   $Revision: 1.1 $
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
class mmgButton;
class vtkLight;
class vtkRenderer;
class mmgGui;

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
	mmgGui *GetGui() {return m_Gui;};

protected:
	class LightItem {
		public:
			LightItem(wxString n, int onoff,float az,float el,float inten,wxColour c, vtkLight *l) {name = n; on_off = onoff; azimuth = az; elevation = el; intensity = inten; colour = c; light = l;};
			~LightItem() {};

			wxString name;
			int on_off;
			float	azimuth,elevation,intensity;
			wxColour colour;
			vtkLight *light;
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

	mmgButton  *m_ButtonAdd; 
	mmgButton  *m_ButtonRemove;

	int				 m_LightCounter;
	wxListBox	*m_LightList;
	LightItem *m_SelectedLight;
	LightItem *m_LightItemList[MAX_NUM_LIGHT];
	float			 m_LightAzimuth;
	float			 m_LightElevation;
	float			 m_LightIntensity;
	int				 m_LightOnOff;
	wxColour	 m_LightColor;

	vtkRenderer	*m_LightRenderer;
	
	wxWindow	  *m_LightParentPanel;
	mmgGui			*m_Gui;
};
#endif
