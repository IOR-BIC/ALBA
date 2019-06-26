/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceButtonsPadTrackerP5Glove
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaDeviceButtonsPadTrackerP5Glove_h
#define __albaDeviceButtonsPadTrackerP5Glove_h

#include "albaDeviceButtonsPadTracker.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaMatrix;
class CP5DLL;

/** class supporting device P5 glove from Essential Reality Inc.
  .SECTION Description
  This class implements Device P5 management as a Tracker device. It currently
  support only one P5 device!
  @sa  albaDeviceButtonsPadTracker albaDevice
  @todo
  - create base class for gloves
  - create GUI with specific settings
  - buttons support
  - mouse mode support
  - improve stability
  - support multiple P5 gloves
*/
class albaDeviceButtonsPadTrackerP5Glove : public albaDeviceButtonsPadTracker
{
public:
  albaTypeMacro(albaDeviceButtonsPadTrackerP5Glove,albaDeviceButtonsPadTracker);

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
  albaDeviceButtonsPadTrackerP5Glove();
  virtual ~albaDeviceButtonsPadTrackerP5Glove();

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
  
  albaMatrix  *m_TmpPose;
  static CP5DLL *m_P5;

private:
  albaDeviceButtonsPadTrackerP5Glove(const albaDeviceButtonsPadTrackerP5Glove&);  // Not implemented.
  void operator=(const albaDeviceButtonsPadTrackerP5Glove&);  // Not implemented.
};

#endif
