/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdP5Glove.h,v $
  Language:  C++
  Date:      $Date: 2005-07-13 13:53:00 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmdP5Glove_h
#define __mmdP5Glove_h

#ifdef __GNUG__
    #pragma interface "mmdP5Glove.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mmdTracker.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkMatrix4x4;
class CP5DLL;

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
class mmdP5Glove : public mmdTracker
{
public:
  
  static mmdP5Glove *New() {return new mmdP5Glove;}
  vtkTypeMacro(mmdP5Glove,mmdTracker);

  // For factoring purpouses
  static vtkObject *NewObjectInstance() {return mmdP5Glove::New();}
  static const char *GetTypeName() {return "mmdP5Glove";}

  /** Get the finger bend sensor current value, range [0,63] */
  vtkGetMacro(IndexValue,int);
  /** Get the finger bend sensor current value, range [0,63] */
  vtkGetMacro(MiddleValue,int);
  /** Get the finger bend sensor current value, range [0,63] */
  vtkGetMacro(RingValue,int);
  /** Get the finger bend sensor current value, range [0,63] */
  vtkGetMacro(PinkyValue,int);
  /** Get the finger bend sensor current value, range [0,63] */
  vtkGetMacro(ThumbValue,int);

  /** Set the finger bend sensor sensitivity, range [0,30] */
  vtkSetMacro(IndexSensitivity,int);
  /** Set the finger bend sensor sensitivity, range [0,30] */
  vtkSetMacro(MiddleSensitivity,int);
  /** Set the finger bend sensor sensitivity, range [0,30] */
  vtkSetMacro(RingSensitivity,int);
  /** Set the finger bend sensor sensitivity, range [0,30] */
  vtkSetMacro(PinkySensitivity,int);
  /** Set the finger bend sensor sensitivity, range [0,30] */
  vtkSetMacro(ThumbSensitivity,int);

  /** Get the finger bend sensor sensitivity, range [0,30] */
  vtkGetMacro(IndexSensitivity,int);
  /** Get the finger bend sensor sensitivity, range [0,30] */
  vtkGetMacro(MiddleSensitivity,int);
  /** Get the finger bend sensor sensitivity, range [0,30] */
  vtkGetMacro(RingSensitivity,int);
  /** Get the finger bend sensor sensitivity, range [0,30] */
  vtkGetMacro(PinkySensitivity,int);
  /** Get the finger bend sensor sensitivity, range [0,30] */
  vtkGetMacro(ThumbSensitivity,int);

protected:
  mmdP5Glove();
  virtual ~mmdP5Glove();

  /** performs polling of P5 glove */
  int InternalUpdate();
  /** initialize P5DLL */
  int InternalInitialize();
  /** close P5DLL */
  void InternalShutdown();

  int IndexValue;
  int MiddleValue;
  int RingValue;
  int PinkyValue;
  int ThumbValue;
  
  int IndexSensitivity;
  int MiddleSensitivity;
  int RingSensitivity;
  int PinkySensitivity;
  int ThumbSensitivity;
  
  vtkMatrix4x4 *TmpPose;
  static CP5DLL *P5;

private:
  mmdP5Glove(const mmdP5Glove&);  // Not implemented.
  void operator=(const mmdP5Glove&);  // Not implemented.
};

#endif
