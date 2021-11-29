/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUITransformTextEntries
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUITransformTextEntries_H__
#define __albaGUITransformTextEntries_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaObserver.h"
#include "albaGUITransformInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUI;
class albaGUIButton;
class albaInteractorGenericMouse;
class albaInteractorCompositorMouse;
class albaInteractor;
class albaVME;
class albaMatrix;

//----------------------------------------------------------------------------
/** text entries gui component for transform operations 

  plug it into an operation to allow vme transformation through text entries.
  scaling gui can be disabled through a bool flag in the constructor; default is
  scaling set to on.
  
  Example Usage:
  ...
  guiTransformTextEntries = new albaGUITransformTextEntries(vmeToMove, observerOperation);
  observerOperation->GetGui()->AddGui(guiTransformTextEntries->GetGui());
  ...

  @sa
  albaOpTransformOld for an example on how to plug this component into a transform
  operation

  @todo


*/
class ALBA_EXPORT albaGUITransformTextEntries : public albaGUITransformInterface
{
public:

  albaGUITransformTextEntries(albaVME *input, albaObserver *listener = NULL, bool enableScaling = true, bool testMode = false);
	~albaGUITransformTextEntries(); 

  void OnEvent(albaEventBase *alba_event);

  /** Enable-Disable the GUI's widgets */
	void EnableWidgets(bool enable);

  //----------------------------------------------------------------------------
  //gui constants: 
  //----------------------------------------------------------------------------

  // this constants must be visible from the owner object  
  enum TRANSFORMTEXTENTRIES_WIDGET_ID
  {
      ID_TRANSLATE_X = MINID,
      ID_TRANSLATE_Y,
      ID_TRANSLATE_Z,
      ID_ROTATE_X,
      ID_ROTATE_Y,
      ID_ROTATE_Z,
      ID_SCALE_X,
      ID_SCALE_Y,
      ID_SCALE_Z,
  };

  /** Override superclass */
  void Reset();
  
  /** Set abs pose and update position, orientation and scale text entries according to current reference system*/
  void SetAbsPose(albaMatrix *absPose, albaTimeStamp timeStamp = -1);

protected:

  /** override superclass */
  void RefSysVmeChanged();  

  /** action to be performed when one text entry has changed */
  void TextEntriesChanged();

  /** override superclass */ 
  void CreateGui();

  double m_Position[3];
  double m_Orientation[3];
  double m_Scaling[3];

  bool m_EnableScaling;

  /** test friend */
  friend class albaGUITransformTextEntriesTest;
};
#endif
