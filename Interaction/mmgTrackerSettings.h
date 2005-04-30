 /*=========================================================================

  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgTrackerSettings.h,v $
  Language:  C++
  Date:      $Date: 2005-04-30 14:34:58 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone

==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgTrackerSettings_h
#define __mmgTrackerSettings_h

#ifdef __GNUG__
    #pragma interface "mmgTrackerSettings.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mmgDeviceSettings.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mmdTracker;
class mflBounds;


/** mmgTrackerSettings is the GUI for tracker devices.
    @sa mflEvent
    @todo
    - implement a calibration tool to determine the tracked volume
    - write good documentation
*/  
class mmgTrackerSettings : public mmgDeviceSettings
{
public:
  mafTypeMacro(mmgTrackerSettings,mmgDeviceSettings);
  
  
  enum  
  {
    ID_TB_X_EXTENT=mmgDeviceSettings::ID_LAST,
    ID_TB_Y_EXTENT,
    ID_TB_Z_EXTENT,
    ID_TB_POSITION,
    ID_TB_ORIENTATION,
    ID_AVATAR_CHECK,
    ID_AVATAR_SELECT,
    
    ID_LAST
  };
  
	mmgTrackerSettings(mmdTracker *tracker=NULL);
 ~mmgTrackerSettings();
  
  /** process events from the GUI */
  virtual void OnEvent(mafEvent& e);
  
  /** Get the devices coupled to this object. */
  mmdTracker *GetTracker();
  
  /** Open a dialog to choose among available avatars */
  int AvatarChooser(wxString &avatar_name,wxString &avatar_type);
  
  /** update the current GUI with */
  virtual void Update();
protected:  
  
  /** Create the dialog that show the interface for settings. */
  virtual void CreateGui();
  
  int      AvatarFlag;
  
  double TBPosition[3]; ///< Used to store the tracked box center

};
#endif // __mmgTrackerSettings_h
