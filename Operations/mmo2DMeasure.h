/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmo2DMeasure.h,v $
  Language:  C++
  Date:      $Date: 2005-10-18 13:44:02 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmo2DMeasure_H__
#define __mmo2DMeasure_H__

#include "mafOp.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmi2DMeter;
class mafEvent;

//----------------------------------------------------------------------------
// mmo2DMeasure :
//----------------------------------------------------------------------------
/** */
class mmo2DMeasure: public mafOp
{
public:
	mmo2DMeasure(wxString label);
	~mmo2DMeasure(); 
	virtual void OnEvent(mafEventBase *maf_event);
  
  /** Return a copy of itself, this needs to put the operation into the undo stack. */
	mafOp* Copy();
	
	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node) {return true;};

	/** Set the input vme for the operation. */
	void OpRun();

protected:
  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

  mmi2DMeter *m_per2DMeter;
  mafString m_dist;
  mafString m_acute_angle;
  mafString m_obtuse_angle;
  wxString m_measure_text;
  int m_measure_type;
  wxListBox *m_measure_list;
};
#endif
