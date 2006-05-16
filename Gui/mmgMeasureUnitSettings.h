/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgMeasureUnitSettings.h,v $
Language:  C++
Date:      $Date: 2006-05-16 11:41:31 $
Version:   $Revision: 1.3 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgMeasureUnitSettings_H__
#define __mmgMeasureUnitSettings_H__

#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mmgGui;

//----------------------------------------------------------------------------
// mmgMeasureUnitSettings :
//----------------------------------------------------------------------------
/**
*/
class mmgMeasureUnitSettings : public mafObserver
{
public:
	mmgMeasureUnitSettings(mafObserver *Listener);
	~mmgMeasureUnitSettings(); 

  enum MEASURE_UNIT_WIDGET_ID
  {
    MEASURE_STRING_ID = MINID,
    MEASURE_SCALE_FACTOR_ID,
    MEASURE_DEFAULT_UNIT_ID,
    MEASURE_UNIT_UPDATED
  };

  /** 
  Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);

  void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  /** Show/Hide the settings interface.*/
	void ChooseMeasureUnit();

  /** Return the Scale factor to map mm into new unit.*/
  double GetScaleFactor();

  /** Return measure unit name.*/
  mafString GetUnitName();

protected:
  /** Initialize measure unit used into the application.*/
  void InitializeMeasureUnit();
  mmgGui      *m_Gui;

  mafObserver *m_Listener;
  double       m_ScaleFactor;
  mafString    m_UnitName;
  int          m_ChoosedUnit;
};
#endif
