/*=========================================================================

 Program: MAF2
 Module: mafDeviceTrackerWIIMote
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafDeviceTrackerWIIMote_h
#define __mafDeviceTrackerWIIMote_h

#include "mafDeviceButtonsPadTracker.h"

#include "Utils.h"
#include "Wiimote.h"

#include <vector>
using namespace std;
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafMatrix;
class Wiimote;

/** class supporting device WII Mote from Nintendo WII Console.
  .SECTION Description
  This class implements Device WII Mote management as a Tracker device. It currently
  support only one WII Mote!
*/
class mafDeviceTrackerWIIMote : public mafDeviceButtonsPadTracker
{
public:
  mafTypeMacro(mafDeviceTrackerWIIMote,mafDeviceButtonsPadTracker);

protected:
  mafDeviceTrackerWIIMote();
  virtual ~mafDeviceTrackerWIIMote();

  /** performs polling of P5 glove */
  int InternalUpdate();
  
  /** initialize P5DLL */
  virtual int InternalInitialize();
  /** close P5DLL */
  virtual void InternalShutdown();

	void Calibration();
	void SetTranlationAndRotation(MotionData p);
  
  mafMatrix  *m_TmpPose;
	vector<Wiimote> m_Wiimotes;

	
	double m_XTranslation, m_YTranslation, m_ZTranslation;
  double m_TranslationCalibration;

	double m_Angle;

	vector<MotionData> m_LastPositions;


private:
  mafDeviceTrackerWIIMote(const mafDeviceTrackerWIIMote&);  // Not implemented.
  void operator=(const mafDeviceTrackerWIIMote&);  // Not implemented.
};

#endif

