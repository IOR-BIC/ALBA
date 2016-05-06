/*=========================================================================

 Program: MAF2
 Module: mafOpExporterWrappedMeter
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpExporterWrappedMeter_H__
#define __mafOpExporterWrappedMeter_H__

#include "mafDefines.h"
#include "mafOp.h"
#include <vnl/vnl_matrix.h>
#include "wx/busyinfo.h"
#include "mafVMEIterator.h"
#include <fstream>
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEWrappedMeter;
class mafGui;
class mafEvent;
//----------------------------------------------------------------------------
// mafOpExporterWrappedMeter :
//----------------------------------------------------------------------------
/** 
class name: mafOpExporterWrappedMeter
   This class provides the functionality for exporting mafVMEWrappedMeter, writing all in a txt file.
*/
class MAF_EXPORT mafOpExporterWrappedMeter: public mafOp
{
public:
  /** object constructor */
  mafOpExporterWrappedMeter(const wxString &label = "ActionLine");
  /** object destructor */  
  ~mafOpExporterWrappedMeter(); 

  std::vector<mafVME *> m_Meters;
  std::vector< vnl_matrix<double> > m_MetersCoordinatesList ;
  mafTimeStamp       m_CurrentTime;
  std::vector<mafTimeStamp> m_Times ;
  mafVME            *m_CurrentVme ;
  std::ofstream      m_OutputFile;
  std::vector<int>  m_KeyNumList;

  /** RTTI macro*/
  mafTypeMacro(mafOpExporterWrappedMeter, mafOp);
  /** method for clone object */
  mafOp* Copy();
  /** method for catch the dispatched events */
	void OnEvent(mafEventBase *maf_event);

 	/** Return true for the acceptable vme type. */
  bool Accept(mafVME*node);

	/** Builds operation's interface. */
  void OpRun();

  /** Set the filename for the .stl to export */
  void SetFileName(const char *file_name) {m_File = file_name;};
  
  /** Export the Wrapped meter coordinates in file. */
  void ExportWrappedMeterCoordinates(int index, int indexTime);
  /** test method copy from test mafOpExporterWrappedMeterTest**/
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
