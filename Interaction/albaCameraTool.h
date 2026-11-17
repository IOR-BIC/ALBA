/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCameraTool
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaCameraTool_h
#define __albaCameraTool_h

#include "albaInteractionTool.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaDevice;
class albaCameraMouseTool;
class albaCameraTrackerTool;

/** Tool moving the camera, always at the bottom of the tool stack of the PER.
  It is made of the mouse camera tool and of the tracker camera tool: each event
  goes first to the mouse tool, the common case, and to the tracker tool only
  if the mouse tool did not use it. Removing the tracker support means removing
  this class and the tracker tool and renaming the mouse tool as albaCameraTool. */
class ALBA_EXPORT albaCameraTool : public albaInteractionTool
{
public:
  albaTypeMacro(albaCameraTool, albaInteractionTool);

  virtual bool OnLeftButtonDown(albaEventInteraction *e);
  virtual bool OnLeftButtonUp(albaEventInteraction *e);
  virtual bool OnMiddleButtonDown(albaEventInteraction *e);
  virtual bool OnMiddleButtonUp(albaEventInteraction *e);
  virtual bool OnRightButtonDown(albaEventInteraction *e);
  virtual bool OnRightButtonUp(albaEventInteraction *e);
  virtual bool OnDrag(albaEventInteraction *e);
  virtual bool OnWheel(albaEventInteraction *e);

  /** Return the tool moving the camera because of the device, NULL if none */
  albaInteractionTool *GetInteractingTool(albaDevice *device);

  albaCameraMouseTool *GetMouseTool() {return m_MouseTool;}
  albaCameraTrackerTool *GetTrackerTool() {return m_TrackerTool;}

protected:
  albaCameraTool();
  virtual ~albaCameraTool();

  albaCameraMouseTool   *m_MouseTool;
  albaCameraTrackerTool *m_TrackerTool;

private:
  albaCameraTool(const albaCameraTool&);  // Not implemented.
  void operator=(const albaCameraTool&);  // Not implemented.
};

#endif
