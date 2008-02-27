/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDeviceTrackerWIIMote.h,v $
  Language:  C++
  Date:      $Date: 2008-02-27 17:20:16 $
  Version:   $Revision: 1.1 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafDeviceTrackerWIIMote_h
#define __mafDeviceTrackerWIIMote_h

#include "mmdTracker.h"
#include <vector>
using namespace std;
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafMatrix;
class Wiimote;

/** class supporting device P5 glove from Essential Reality Inc.
  .SECTION Description
  This class implements Device P5 management as a Tracker device. It currently
  support only one P5 device!
  @sa  mmdTracker mafDevice
  @todo
  - create base class for gloves
  - create GUI with specific settings
  - buttons support
  - mouse mode support
  - improve stability
  - support multiple P5 gloves
*/
class mafDeviceTrackerWIIMote : public mmdTracker
{
public:
  mafTypeMacro(mafDeviceTrackerWIIMote,mmdTracker);

  /** Get the finger bend sensor current value, range [0,63] */
  int GetIndexValue() {return m_IndexValue;}
  /** Get the finger bend sensor current value, range [0,63] */
  int GetMiddleValue() {return m_MiddleValue;}
  /** Get the finger bend sensor current value, range [0,63] */
  int GetRingValue() {return m_RingValue;}
  /** Get the finger bend sensor current value, range [0,63] */
  int GetPinkyValue() {return m_PinkyValue;}
  /** Get the finger bend sensor current value, range [0,63] */
  int GetThumbValue() {return m_ThumbValue;}

  /** Set the finger bend sensor sensitivity, range [0,30] */
  void SetIndexSensitivity(int value) {m_IndexSensitivity=value;}
  /** Set the finger bend sensor sensitivity, range [0,30] */
  void SetMiddleSensitivity(int value) {m_MiddleSensitivity=value;}
  /** Set the finger bend sensor sensitivity, range [0,30] */
  void SetRingSensitivity(int value) {m_RingSensitivity=value;}
  /** Set the finger bend sensor sensitivity, range [0,30] */
  void SetPinkySensitivity(int value) {m_PinkySensitivity=value;}
  /** Set the finger bend sensor sensitivity, range [0,30] */
  void SetThumbSensitivity(int value) {m_ThumbSensitivity=value;}

  /** Get the finger bend sensor sensitivity, range [0,30] */
  int GetIndexSensitivity(int value) {m_IndexSensitivity=value;}
  /** Get the finger bend sensor sensitivity, range [0,30] */
  int GetMiddleSensitivity(int value) {m_MiddleSensitivity=value;}
  /** Get the finger bend sensor sensitivity, range [0,30] */
  int GetRingSensitivity(int value) {m_RingSensitivity=value;}
  /** Get the finger bend sensor sensitivity, range [0,30] */
  int GetPinkySensitivity(int value) {m_PinkySensitivity=value;}
  /** Get the finger bend sensor sensitivity, range [0,30] */
  int GetThumbSensitivity(int value) {m_ThumbSensitivity=value;}

protected:
  mafDeviceTrackerWIIMote();
  virtual ~mafDeviceTrackerWIIMote();

  /** performs polling of P5 glove */
  int InternalUpdate();
  
  /** initialize P5DLL */
  virtual int InternalInitialize();
  /** close P5DLL */
  virtual void InternalShutdown();

  int m_IndexValue;
  int m_MiddleValue;
  int m_RingValue;
  int m_PinkyValue;
  int m_ThumbValue;
  
  int m_IndexSensitivity;
  int m_MiddleSensitivity;
  int m_RingSensitivity;
  int m_PinkySensitivity;
  int m_ThumbSensitivity;
  
  mafMatrix  *m_TmpPose;
	vector<Wiimote> wiimotes;

private:
  mafDeviceTrackerWIIMote(const mafDeviceTrackerWIIMote&);  // Not implemented.
  void operator=(const mafDeviceTrackerWIIMote&);  // Not implemented.
};

#endif

