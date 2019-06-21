/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUITransformSliders
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUITransformSliders_H__
#define __albaGUITransformSliders_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaObserver.h"
#include "albaGUITransformInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUI;
class albaGUIButton;
class mmiGenericMouse;
class mmiCompositorMouse;
class albaInteractor;
class albaVME;
class albaMatrix;
class albaGUIFloatSlider;

//----------------------------------------------------------------------------
/** Class name: albaGUITransformSliders.

A GUI component with sliders widgets for transform operations.
*/
class ALBA_EXPORT albaGUITransformSliders : public albaGUITransformInterface
{
public:

  /** Constructor. */
  albaGUITransformSliders(albaVME *input, double translationRange[6], albaObserver *listener = NULL, bool enableScaling = true,bool testMode = false);

  /** Destructor */
  ~albaGUITransformSliders(); 

  /** Override superclass */
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
    ID_RESET,
  };

  /** Override superclass */
  void Reset();

  /** Set abs pose and update position, orientation and scale text entries according to current reference system. */
  void SetAbsPose(albaMatrix *absPose, albaTimeStamp timeStamp = -1);

  /** Return the current position */
  void GetPosition(double pos[3]);

  /** Return the current orientation */
  void GetOrientation(double orientation[3]);

  /** Return the current scaling */
  void GetScaling(double scaling[3]);

  /** Set the current position */
  void SetPosition(double pos[3]);

  /** Set the current orientation */
  void SetOrientation(double orientation[3]);

  /** Set the current scaling */
  void SetScaling(double scaling[3]);


protected:

  /** override superclass */
  void RefSysVmeChanged();  

  /** action to be performed when a slider has changed its position. */
  void SlidersValuesChanged();

  /** override superclass */ 
  void CreateGui();

  double m_Position[3];
  double m_Orientation[3];
  double m_Scaling[3];
  double m_TranslationRange[6];

  bool m_EnableScaling;

  albaMatrix *m_OldAbsMatrix;
};
#endif
