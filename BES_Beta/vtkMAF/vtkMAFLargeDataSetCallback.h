/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeDataSetCallback.h,v $ 
  Language: C++ 
  Date: $Date: 2012-04-06 10:17:54 $ 
  Version: $Revision: 1.1.2.4 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#ifndef __vtkMAFLargeDataSetCallback__
#define __vtkMAFLargeDataSetCallback__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medCommonDefines.h"
#include "vtkCommand.h"

//----------------------------------------------------------------------------
// Forward declarations:
//----------------------------------------------------------------------------
class mafObserver;

//this class handles events produced by LargeDataSet
class MED_COMMON_EXPORT vtkMAFLargeDataSetCallback : public vtkCommand
{
  //vtkTypeMacro(vtkMAFLargeDataSetCallback,vtkCommand);

protected:
	mafObserver* Listener;
public:
	inline static vtkMAFLargeDataSetCallback* New() {
		return new vtkMAFLargeDataSetCallback();
	}

	//set the MAF listener that should process the MAF generated events
  void SetListener(mafObserver* listener);

	//callback routine called by VTK, translate events into MAF events
	virtual void Execute(vtkObject* caller, unsigned long eventId, void* callData);
  
  virtual const char *GetClassName() const {return "vtkMAFLargeDataSetCallback";};


protected:
	vtkMAFLargeDataSetCallback() {
		Listener = NULL;
	}

	~vtkMAFLargeDataSetCallback() {
		
	}
};

#endif //__vtkMAFLargeDataSetCallback__