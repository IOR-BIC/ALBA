/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceTrackerWIIMote
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaDeviceTrackerWIIMote_h
#define __albaDeviceTrackerWIIMote_h

#include "albaDeviceButtonsPadTracker.h"

#include "Utils.h"
#include "Wiimote.h"

#include <vector>
using namespace std;
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaMatrix;
class Wiimote;

/** class supporting device WII Mote from Nintendo WII Console.
  .SECTION Description
  This class implements Device WII Mote management as a Tracker device. It currently
  support only one WII Mote!
*/
class albaDeviceTrackerWIIMote : public albaDeviceButtonsPadTracker
{
public:
  albaTypeMacro(albaDeviceTrackerWIIMote,albaDeviceButtonsPadTracker);

protected:
  albaDeviceTrackerWIIMote();
  virtual ~albaDeviceTrackerWIIMote();

  /** performs polling of P5 glove */
  int InternalUpdate();
  
  /** initialize P5DLL */
  virtual int InternalInitialize();
  /** close P5DLL */
  virtual void InternalShutdown();

	void Calibration();
	void SetTranlationAndRotation(MotionData p);
  
  albaMatrix  *m_TmpPose;
	vector<Wiimote> m_Wiimotes;

	
	double m_XTranslation, m_YTranslation, m_ZTranslation;
  double m_TranslationCalibration;

	double m_Angle;

	vector<MotionData> m_LastPositions;


private:
  albaDeviceTrackerWIIMote(const albaDeviceTrackerWIIMote&);  // Not implemented.
  void operator=(const albaDeviceTrackerWIIMote&);  // Not implemented.
};

#endif

