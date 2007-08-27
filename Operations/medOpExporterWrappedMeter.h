/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpExporterWrappedMeter.h,v $
  Language:  C++
  Date:      $Date: 2007-08-27 12:59:10 $
  Version:   $Revision: 1.1 $
  Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpExporterWrappedMeter_H__
#define __medOpExporterWrappedMeter_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class medVMEWrappedMeter;
class mafGui;
class mafEvent;
//----------------------------------------------------------------------------
// medOpExporterWrappedMeter :
//----------------------------------------------------------------------------
/** */
class medOpExporterWrappedMeter: public mafOp
{
public:
  medOpExporterWrappedMeter(const wxString &label = "WrappedMeter");
  ~medOpExporterWrappedMeter(); 

  mafTypeMacro(medOpExporterWrappedMeter, mafOp);

  mafOp* Copy();
	void OnEvent(mafEventBase *maf_event);

 	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

	/** Builds operation's interface. */
  void OpRun();

  /** Set the filename for the .stl to export */
  void SetFileName(const char *file_name) {m_File = file_name;};
  
  /** Export the Wrapped meter coordinates in file. */
  void ExportWrappedMeterCoordinates();

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);
  
  mafString           m_File;
};
#endif
