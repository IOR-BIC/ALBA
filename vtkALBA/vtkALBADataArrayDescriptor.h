/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkALBADataArrayDescriptor.h,v $ 
  Language: C++ 
  Date: $Date: 2011-05-26 08:51:00 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#ifndef __vtkALBADataArrayDescriptor_h
#define __vtkALBADataArrayDescriptor_h

#include "vtkObject.h"
#include "vtkALBAIdType64.h"

class vtkIdList;

class ALBA_EXPORT vtkALBADataArrayDescriptor : public vtkObject 
{
public:
	vtkTypeMacro(vtkALBADataArrayDescriptor,vtkObject);
	void PrintSelf(ostream& os, vtkIndent indent);
	static vtkALBADataArrayDescriptor* New();

	// Description:
	// Return the underlying data type. An integer indicating data type is 
	// returned as specified in vtkSetGet.h.
	vtkSetClampMacro(DataType, int, VTK_BIT, VTK_ID_TYPE);
	inline int GetDataType() {
		return DataType;
	}

	// Description:
	// Return the size of the underlying data type.  For a bit, 0 is returned.
	inline int GetDataTypeSize() {
		return GetDataTypeSize(DataType);
	}

	// Description: static method to be called from any routine
	// Return the size of the underlying data type.  For a bit, 0 is returned.
	static int GetDataTypeSize(int dataType);

	// Description:
	// Set/Get the dimension (n) of the components. Must be >= 1. 
	vtkSetClampMacro(NumberOfComponents,int,1,VTK_INT_MAX);
	int GetNumberOfComponents() {
		return this->NumberOfComponents;
	};

	// Description:
	// Set the number of tuples (a component group) in the array. 
	void SetNumberOfTuples(vtkIdType64 number) {
		this->NumberOfTuples = number;
	}

	// Description:
	// Get the number of tuples (a component group) in the array.
	vtkIdType64 GetNumberOfTuples() {
		return this->NumberOfTuples;
	}

	// Description:
	// Return the size of the data (in elements)
	vtkIdType64 GetSize() {
		return ((vtkIdType64)this->NumberOfTuples)*this->NumberOfComponents;
	}

	// Description:
	// Return the memory in kilobytes consumed by this data array. Used to
	// support streaming and reading/writing data. The value returned is
	// guaranteed to be greater than or equal to the memory required to
	// actually represent the data represented by this object. The 
	// information returned is valid only after the pipeline has 
	// been updated.
	inline unsigned long GetActualMemorySize() {
		return (DataType == VTK_BIT ? 
			(GetSize() + 7) / 8 : GetSize()*GetDataTypeSize());
	}

	// Description:
	// Set/get array's name
	void SetName(const char* name);
	const char* GetName();

	// Description:
	// These methods return the Min and Max possible range of the native
	// data type. For example for unsigned char data these will return (0,255). 
	void GetDataTypeRange(double range[2]);
	double GetDataTypeMin();
	double GetDataTypeMax();   

  //Deep copy. 
  void DeepCopy(vtkALBADataArrayDescriptor *src);

protected:
	// Construct object with default tuple dimension (number of components) of 1.
	vtkALBADataArrayDescriptor();
	~vtkALBADataArrayDescriptor();

	int NumberOfComponents; // the number of components per tuple
	vtkIdType64 NumberOfTuples;		// the number of tuples
	int DataType;			// the data type of components
	char* Name;				// the name of this data array

private:
	vtkALBADataArrayDescriptor(const vtkALBADataArrayDescriptor&);  // Not implemented.
	void operator=(const vtkALBADataArrayDescriptor&);  // Not implemented.
};

#endif
