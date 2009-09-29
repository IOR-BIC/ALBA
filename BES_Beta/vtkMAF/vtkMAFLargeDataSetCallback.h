/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeDataSetCallback.h,v $ 
  Language: C++ 
  Date: $Date: 2009-09-29 09:33:32 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#ifndef __vtkMAFLargeDataSetCallback__
#define __vtkMAFLargeDataSetCallback__

#include "vtkCommand.h"
#include "mafObserver.h"

//this class handles events produced by LargeDataSet
class vtkMAFLargeDataSetCallback : public vtkCommand
{
protected:
	mafObserver* Listener;
public:
	inline static vtkMAFLargeDataSetCallback* New() {
		return new vtkMAFLargeDataSetCallback();
	}

	//set the MAF listener that should process the MAF generated events
	inline void SetListener(mafObserver* listener) {
		Listener = listener;
	}

	//callback routine called by VTK, translate events into MAF events
	virtual void Execute(vtkObject* caller, unsigned long eventId, void* callData);

protected:
	vtkMAFLargeDataSetCallback() {
		Listener = NULL;
	}

	~vtkMAFLargeDataSetCallback() {
		
	}
};

#endif //__vtkMAFLargeDataSetCallback__