/*=========================================================================

 Program: MAF2
 Module: mafTextKit
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafTextKit_H__
#define __mafTextKit_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
#include "mafServiceClient.h"
#include "mafAbsLogicManager.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUI;
class vtkRenderer;
class vtkActor2D;
class vtkTextMapper;

//----------------------------------------------------------------------------
// mafTextKit :
//----------------------------------------------------------------------------
/**
mafTextKit is tool to visualize text into a given mafRWI.
\sa mafRWI
*/
class MAF_EXPORT mafTextKit : public mafObserver, public mafServiceClient
{
public:
	mafTextKit(wxWindow* parent, vtkRenderer *renderer, mafObserver *Listener = NULL);
	~mafTextKit(); 
	
	void OnEvent(mafEventBase *maf_event);
	void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  enum TEXT_KIT_ALIGN_MODALITY
  {
    UPPER_LEFT = 0,
    UPPER_CENTER,
    UPPER_RIGHT,
    CENTER_LEFT,
    CENTER_CENTER,
    CENTER_RIGHT,
    LOWER_LEFT,
    LOWER_CENTER,
    LOWER_RIGHT
  };

  /** Insert a text into the render window.*/
  void SetText(const char *text);

  /** Position the text into the render window at normalized view's position x,y. (0,0) is the lower left corner.*/
  void SetTextPosition(double nv_x, double nv_y);

  /** Allow to change the text color. Values passed are from 0 (black) to 255 (white).*/
  void SetTextColor(int textColor[3]);
  void SetTextColor(int textColorRed,int textColorGreen,int textColorBlue );

  /** Allow to change the font size */
  void SetTextSize(int size);

  /** Align text into the render window.*/
  void SetTextAlignment(int align = UPPER_LEFT);

	/** Returns the mafTextKit's GUI */
	mafGUI *GetGui() {return m_Gui;};

protected:
  /** Create GUI for AttachCamera module.*/
  void CreateGui();

  int m_ShowText;
  int m_TextAlign;
  int m_TextSize; 
  double m_TextPosition[2]; ///< Position of the text in normalized view coordinate system. (0,0) is the lower left corner.
  mafString m_TextInView; ///< Text visualized inside the view.
  double m_TextOffset; ///< Offset in positioning the text
  wxColor m_TextColor; ///< Color applied to text.

  vtkTextMapper	*m_TextMapper;
  vtkActor2D    *m_TextActor;

  mafObserver	*m_Listener;
	mafGUI			*m_Gui;
	vtkRenderer *m_TextRenderer;
	wxWindow	  *m_ParentPanel;
};
#endif
