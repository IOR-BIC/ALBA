/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGuiTransformTextEntries.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:03:38 $
  Version:   $Revision: 1.4 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafGUITransformTextEntries_H__
#define __mafGUITransformTextEntries_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObserver.h"
#include "mafGUITransformInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUI;
class mafGUIButton;
class mmiGenericMouse;
class mmiCompositorMouse;
class mafInteractor;
class mafVME;
class mafMatrix;

//----------------------------------------------------------------------------
/** text entries gui component for transform operations 

  plug it into an operation to allow vme transformation through text entries.
  scaling gui can be disabled through a bool flag in the constructor; default is
  scaling set to on.

  @sa
  mafOpMAFTransform for an example on how to use this component into a transform
  operation

  @todo

*/
class mafGUITransformTextEntries : public mafGUITransformInterface
{
public:

  mafGUITransformTextEntries(mafVME *input, mafObserver *listener = NULL, bool enableScaling = true);
	~mafGUITransformTextEntries(); 

  void OnEvent(mafEventBase *maf_event);

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
  void SetAbsPose(mafMatrix *absPose, mafTimeStamp timeStamp = -1);

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
};
#endif
