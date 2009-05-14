/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeDataSetCallback.h,v $ 
  Language: C++ 
  Date: $Date: 2009-05-14 15:03:31 $ 
  Version: $Revision: 1.1.2.1 $ 
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
	mafObserver* m_Listener;
public:
	inline static vtkMAFLargeDataSetCallback* New() {
		return new vtkMAFLargeDataSetCallback();
	}

	//set the MAF listener that should process the MAF generated events
	inline void SetListener(mafObserver* listener) {
		m_Listener = listener;
	}

	//callback routine called by VTK, translate events into MAF events
	virtual void Execute(vtkObject* caller, unsigned long eventId, void* callData);

protected:
	vtkMAFLargeDataSetCallback() {
		m_Listener = NULL;
	}

	~vtkMAFLargeDataSetCallback() {
		
	}
};

#endif //__vtkMAFLargeDataSetCallback__