/*=========================================================================

 Program: MAF2Medical
 Module: medOpExporterWrappedMeter
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpExporterWrappedMeter_H__
#define __medOpExporterWrappedMeter_H__

#include "mafDefines.h"
#include "mafOp.h"
#include <vnl/vnl_matrix.h>
#include "wx/busyinfo.h"
#include "mafNodeIterator.h"
#include <fstream>
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class medVMEWrappedMeter;
class mafGui;
class mafEvent;
//----------------------------------------------------------------------------
// medOpExporterWrappedMeter :
//----------------------------------------------------------------------------
/** 
class name: medOpExporterWrappedMeter
   This class provides the functionality for exporting medVMEWrappedMeter, writing all in a txt file.
*/
class MAF_EXPORT medOpExporterWrappedMeter: public mafOp
{
public:
  /** object constructor */
  medOpExporterWrappedMeter(const wxString &label = "ActionLine");
  /** object destructor */  
  ~medOpExporterWrappedMeter(); 

  std::vector<mafNode *> m_Meters;
  std::vector< vnl_matrix<double> > m_MetersCoordinatesList ;
  mafTimeStamp       m_CurrentTime;
  std::vector<mafTimeStamp> m_Times ;
  mafNode            *m_CurrentVme ;
  std::ofstream      m_OutputFile;
  std::vector<int>  m_KeyNumList;

  /** RTTI macro*/
  mafTypeMacro(medOpExporterWrappedMeter, mafOp);
  /** method for clone object */
  mafOp* Copy();
  /** method for catch the dispatched events */
	void OnEvent(mafEventBase *maf_event);

 	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

	/** Builds operation's interface. */
  void OpRun();

  /** Set the filename for the .stl to export */
  void SetFileName(const char *file_name) {m_File = file_name;};
  
  /** Export the Wrapped meter coordinates in file. */
  void ExportWrappedMeterCoordinates(int index, int indexTime);
  /** test method copy from test medOpExporterWrappedMeterTest**/
  void Test() ;

  /** specific exporter for wrapped meters */
  void ExportWrappedMeter();
  /** generic function for handling export*/
  void Export();
  /** write the data stream to a file */
  void WriteOnFile();

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);
  
  mafString           m_File;
};
#endif
