/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkALBADataArrayDescriptor.cxx,v $ 
  Language: C++ 
  Date: $Date: 2009-05-14 15:03:31 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#pragma warning(disable: 4996)	//depricated

#include "vtkALBADataArrayDescriptor.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkALBADataArrayDescriptor);

// Construct object with default tuple dimension (number of components) of 1.
vtkALBADataArrayDescriptor::vtkALBADataArrayDescriptor()
{
	this->DataType = VTK_DOUBLE;
	this->NumberOfComponents = 1;
	this->NumberOfTuples = 0;
	this->Name = NULL;
}

vtkALBADataArrayDescriptor::~vtkALBADataArrayDescriptor()
{
	delete[] this->Name;
}

void vtkALBADataArrayDescriptor::SetName(const char* name)
{
	delete[] this->Name;
	this->Name = 0;
	if (name)
	{
		int size = static_cast<int>(strlen(name));
		this->Name = new char[size+1];
		strcpy(this->Name, name);
	}
}

const char* vtkALBADataArrayDescriptor::GetName()
{
	return this->Name;
}

/*static*/ int vtkALBADataArrayDescriptor::GetDataTypeSize(int dataType)
{	
	switch (dataType)
	{
	case VTK_BIT:            return 0;
	case VTK_UNSIGNED_CHAR:   return static_cast<int>(sizeof(unsigned char));
	case VTK_CHAR:            return static_cast<int>(sizeof(char));
	case VTK_UNSIGNED_SHORT:  return static_cast<int>(sizeof(unsigned short));
	case VTK_SHORT:           return static_cast<int>(sizeof(short));
	case VTK_UNSIGNED_INT:    return static_cast<int>(sizeof(unsigned int));
	case VTK_INT:             return static_cast<int>(sizeof(int));
	case VTK_UNSIGNED_LONG:   return static_cast<int>(sizeof(unsigned long));
	case VTK_LONG:            return static_cast<int>(sizeof(long));
	case VTK_FLOAT:           return static_cast<int>(sizeof(float));
	case VTK_DOUBLE:          return static_cast<int>(sizeof(double));
	default: return 0;
	}
}

void vtkALBADataArrayDescriptor::GetDataTypeRange(double range[2])
{
	range[0] = this->GetDataTypeMin();
	range[1] = this->GetDataTypeMax();
}

double vtkALBADataArrayDescriptor::GetDataTypeMin()
{
	int dataType=this->GetDataType();
	switch (dataType)
	{
	case VTK_BIT:            return (double)VTK_BIT_MIN;
	case VTK_UNSIGNED_CHAR:  return (double)VTK_UNSIGNED_CHAR_MIN;
	case VTK_CHAR:           return (double)VTK_CHAR_MIN;
	case VTK_UNSIGNED_SHORT: return (double)VTK_UNSIGNED_SHORT_MIN;
	case VTK_SHORT:          return (double)VTK_SHORT_MIN;
	case VTK_UNSIGNED_INT:   return (double)VTK_UNSIGNED_INT_MIN;
	case VTK_INT:            return (double)VTK_INT_MIN;
	case VTK_UNSIGNED_LONG:  return (double)VTK_UNSIGNED_LONG_MIN;
	case VTK_LONG:           return (double)VTK_LONG_MIN;
	case VTK_FLOAT:          return (double)VTK_FLOAT_MIN;
	case VTK_DOUBLE:         return (double)VTK_DOUBLE_MIN;
	default: return 0;
	}
}

double vtkALBADataArrayDescriptor::GetDataTypeMax()
{
	int dataType=this->GetDataType();
	switch (dataType)
	{
	case VTK_BIT:            return (double)VTK_BIT_MAX;
	case VTK_UNSIGNED_CHAR:  return (double)VTK_UNSIGNED_CHAR_MAX;
	case VTK_CHAR:           return (double)VTK_CHAR_MAX;
	case VTK_UNSIGNED_SHORT: return (double)VTK_UNSIGNED_SHORT_MAX;
	case VTK_SHORT:          return (double)VTK_SHORT_MAX;
	case VTK_UNSIGNED_INT:   return (double)VTK_UNSIGNED_INT_MAX;
	case VTK_INT:            return (double)VTK_INT_MAX;
	case VTK_UNSIGNED_LONG:  return (double)VTK_UNSIGNED_LONG_MAX;
	case VTK_LONG:           return (double)VTK_LONG_MAX;
	case VTK_FLOAT:          return (double)VTK_FLOAT_MAX;
	case VTK_DOUBLE:         return (double)VTK_DOUBLE_MAX;
	default: return 1;
	}
}

void vtkALBADataArrayDescriptor::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

	const char* name = this->GetName();
	if (name)
	{
		os << indent << "Name: " << name << "\n";
	}
	else
	{
		os << indent << "Name: (none)\n";
	}
	os << indent << "Number Of Components: " << this->NumberOfComponents << "\n";
	os << indent << "Number Of Tuples: " << this->GetNumberOfTuples() << "\n";
	os << indent << "Type: " << this->GetDataType() << "\n";
	os << indent << "Size: " << this->GetSize() << "\n";
	os << indent << "MemSize: " << this->GetActualMemorySize() << "\n";
}

//Deep copy. 
void vtkALBADataArrayDescriptor::DeepCopy(vtkALBADataArrayDescriptor *src)
{
  this->DataType = src->DataType;
  this->NumberOfComponents = src->NumberOfComponents;
  this->NumberOfTuples = src->NumberOfTuples;
  SetName(src->Name); //creates a copy
}


