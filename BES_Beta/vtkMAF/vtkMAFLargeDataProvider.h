/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeDataProvider.h,v $ 
  Language: C++ 
  Date: $Date: 2009-05-14 15:03:31 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
  Abstract class - every large data provider (memory mapped, distributed, etc.
  is supposed to be derived from this class. It provides access to large data
  sets for both reading and writing. A large data set comprises of one or
  more named data arrays (i.e., scalars, vectors, normals, tensors) that
  are described by descriptors of vtkMAFDataArrayDescriptor class. 
  The data arrays are stored on the physical medium as it is described by
  vtkMAFDataArrayLayout. By the default, they are stored sequentially one after 
  another (components in interleaved mode), there might be an starting offset
  of the first data array (denoted by HeaderSize)
  =========================================================================*/

#ifndef __vtkMAFLargeDataProvider_h
#define __vtkMAFLargeDataProvider_h

#include "vtkObject.h"
#include "vtkDataSetAttributes.h"
#include "vtkMAFDataArrayDescriptor.h"
#include "vtkMAFDataArrayLayout.h"
#include <vector>
#include <map>

class vtkFieldData;
class vtkTimeStamp;

#pragma warning(disable: 4068)	//Unknown pragma: VC 2003 does not support regions

class VTK_COMMON_EXPORT vtkMAFLargeDataProvider : public vtkObject
{	
protected:
  //Data Array Layout Descriptor
  typedef struct DALD
  {
    vtkMAFDataArrayDescriptor* pDAD;  //<descriptor
    vtkMAFDataArrayLayout* pDAL;      //<physical layout
  } DALD;

protected:
	//General tags
	vtkFieldData* TagArray;

	//descriptor of data
	typedef std::vector< DALD > DescriptorVector;
	DescriptorVector m_Descriptors;

	typedef std::map< const char*, int > StringToIntMap;
	StringToIntMap m_DescriptorsMap;

	//special descriptors positions
	int m_SpecDescPos[vtkDataSetAttributes::NUM_ATTRIBUTES];

	// Denotes whether the bytes should be swapped 
	bool SwapBytes;

  //true, if the data layout is automatically computed from 
  //HeaderSize and sizes of data arrays
  bool DefaultLayout;

	//the size of header in bytes (before the first data array - it will be skipped)
	vtkIdType64 HeaderSize;

  //global offsets
  vtkTimeStamp m_OffsetsComputeTime;

	//Setters and getters
public:
	//Get/Set byte order of the binary input. Valid values are BigEndian or LittleEndian.
	vtkGetMacro(SwapBytes, bool);
	vtkSetMacro(SwapBytes, bool);
	vtkBooleanMacro(SwapBytes, bool);

  //Get/Set the physical layout mode of data arrays
  vtkGetMacro(DefaultLayout, bool);
  vtkSetMacro(DefaultLayout, bool);
  vtkBooleanMacro(DefaultLayout, bool);  

	//Get/Sets the header size (in bytes) - only for Default Layout
	inline virtual vtkIdType64 GetHeaderSize() {
		return HeaderSize;
	}

	inline virtual void SetHeaderSize(vtkIdType64 header) 
	{
		if (HeaderSize != header) {
			HeaderSize = header;
			this->Modified();
		}
	}
	
	//Returns an array of tags associated with the given data (i.e., name of file, URL,
	//configurations, etc.
	inline virtual vtkFieldData* GetTagArray() {
		return TagArray;
	}
	
public:
	vtkTypeRevisionMacro(vtkMAFLargeDataProvider,vtkObject);    

	// Return this object's modified time.
	/*virtual*/ unsigned long GetMTime();

#pragma region DESCRIPTOR OPERATIONS
	// Release all descriptors but do not delete this object.
	// Also, the underlaying data is untouched.
	virtual void InitializeDescriptors();

	//Adds a new data array descriptor into the collection of descriptors.
	//The caller is supposed to Delete the given array when it is no longer needed
	//Returns -1 if an error occurs, otherwise, it returns the index of descriptor 
	virtual int AddDescriptor(vtkMAFDataArrayDescriptor* dad);	

	// Return the i-th descriptor. A NULL is returned if the index i is out of range.
	inline virtual vtkMAFDataArrayDescriptor* GetDescriptor(int i)
	{
		if ( i < 0 || i >= this->GetNumberOfDescriptors())		
			return NULL;		

		return this->m_Descriptors[i].pDAD;
	}

  //Gets the data array descriptor having the specified name
  //NB: The caller may not Delete the returned array ->
  //DO NOT USE SMART POINTERS FOR THE RETURNED REFERENCE
  inline virtual vtkMAFDataArrayDescriptor* GetDescriptor(const char* name) {
    return GetDescriptor(GetIndexOfDescriptor(name));
  }

	// Removes an descriptor (with the given name) from the collection of descriptors.
	inline virtual void RemoveDescriptor(const char *name) {
		RemoveDescriptor(GetIndexOfDescriptor(name));
	}

	// Remove the descriptor at index i from the collection of descriptors.
	virtual void RemoveDescriptor(int i);

	//Returns index of the descriptor given by name, or -1 if no such descriptor
	//exists in the collection
	virtual int GetIndexOfDescriptor(const char* name);

	//returns number of descriptors of arrays
	inline int GetNumberOfDescriptors() {
		return (int)m_Descriptors.size();
	}

//SPECIAL DESCRIPTORS
#pragma region SPECIAL DESCRIPTORS
	//Gets the index of the scalars descriptor with the specified name (or the active one, if not name is NULL)
	inline int GetIndexOfScalarsDescriptor() {
		return GetIndexOfDescriptor(vtkDataSetAttributes::SCALARS);
	}

	//Gets the index of the vectors descriptor with the specified name (or the active one, if not name is NULL)
	inline int GetIndexOfVectorsDescriptor() {
		return GetIndexOfDescriptor(vtkDataSetAttributes::VECTORS);
	}

	//Gets the index of the normals descriptor with the specified name (or the active one, if not name is NULL)
	inline int GetIndexOfNormalsDescriptor() {
		return GetIndexOfDescriptor(vtkDataSetAttributes::NORMALS);
	}

	//Gets the index of the texture coordinates descriptor with the specified name (or the active one, if not name is NULL)
	inline int GetIndexOfTCoordsDescriptor() {
		return GetIndexOfDescriptor(vtkDataSetAttributes::TCOORDS);
	}

	//Gets the index of the tensor descriptor with the specified name (or the active one, if not name is NULL)
	inline int GetIndexOfTensorsDescriptor() {
		return GetIndexOfDescriptor(vtkDataSetAttributes::TENSORS);
	}


	//Sets the index of the scalars descriptor with the specified name (or the active one, if not name is NULL)
	inline void SetIndexOfScalarsDescriptor(int idx) {
		SetIndexOfDescriptor(vtkDataSetAttributes::SCALARS, idx);
	}

	//Sets the index of the vectors descriptor with the specified name (or the active one, if not name is NULL)
	inline void SetIndexOfVectorsDescriptor(int idx) {
		SetIndexOfDescriptor(vtkDataSetAttributes::VECTORS, idx);
	}

	//Sets the index of the normals descriptor with the specified name (or the active one, if not name is NULL)
	inline void SetIndexOfNormalsDescriptor(int idx) {
		SetIndexOfDescriptor(vtkDataSetAttributes::NORMALS, idx);
	}

	//Sets the index of the texture coordinates descriptor with the specified name (or the active one, if not name is NULL)
	inline void SetIndexOfTCoordsDescriptor(int idx) {
		SetIndexOfDescriptor(vtkDataSetAttributes::TCOORDS, idx);
	}

	//Sets the index of the tensor descriptor with the specified name (or the active one, if not name is NULL)
	inline void SetIndexOfTensorsDescriptor(int idx) {
		SetIndexOfDescriptor(vtkDataSetAttributes::TENSORS, idx);
	}

	//Gets the index of the special descriptor
	virtual int GetIndexOfDescriptor(int type);

	//Sets the index of the special descriptor
	virtual void SetIndexOfDescriptor(int type, int idx);


	//Gets the scalars descriptor with the specified name (or the active one, if not name is NULL)
	inline vtkMAFDataArrayDescriptor* GetScalarsDescriptor(const char* name = NULL) {
		return GetDescriptor(vtkDataSetAttributes::SCALARS, name);
	}

	//Gets the vectors descriptor with the specified name (or the active one, if not name is NULL)
	inline vtkMAFDataArrayDescriptor* GetVectorsDescriptor(const char* name = NULL) {
		return GetDescriptor(vtkDataSetAttributes::VECTORS, name);
	}

	//Gets the normals descriptor with the specified name (or the active one, if not name is NULL)
	inline vtkMAFDataArrayDescriptor* GetNormalsDescriptor(const char* name = NULL) {
		return GetDescriptor(vtkDataSetAttributes::NORMALS, name);
	}

	//Gets the texture coordinates descriptor with the specified name (or the active one, if not name is NULL)
	inline vtkMAFDataArrayDescriptor* GetTCoordsDescriptor(const char* name = NULL) {
		return GetDescriptor(vtkDataSetAttributes::TCOORDS, name);
	}

	//Gets the tensor descriptor with the specified name (or the active one, if not name is NULL)
	inline vtkMAFDataArrayDescriptor* GetTensorsDescriptor(const char* name = NULL) {
		return GetDescriptor(vtkDataSetAttributes::TENSORS, name);
	}

	//Sets the descriptor for scalars, returns -1 if an error occurs, otherwise,
	//it returns the index of descriptor (that can be used e.g. in GetDescriptor)	
	inline int SetScalarsDescriptor(vtkMAFDataArrayDescriptor* dad) {
		return SetDescriptor(vtkDataSetAttributes::SCALARS, dad);
	}

	//Sets the descriptor for vectors, returns -1 if an error occurs, otherwise,
	//it returns the index of descriptor (that can be used e.g. in GetDescriptor)
	inline int SetVectorsDescriptor(vtkMAFDataArrayDescriptor* dad) {
		return SetDescriptor(vtkDataSetAttributes::VECTORS, dad);
	}

	//Sets the descriptor for normals, returns -1 if an error occurs, otherwise,
	//it returns the index of descriptor (that can be used e.g. in GetDescriptor)
	inline int SetNormalsDescriptor(vtkMAFDataArrayDescriptor* dad) {
		return SetDescriptor(vtkDataSetAttributes::NORMALS, dad);
	}

	//Sets the descriptor for texture coordinates, returns -1 if an error occurs, otherwise,
	//it returns the index of descriptor (that can be used e.g. in GetDescriptor)
	inline int SetTCoordsDescriptor(vtkMAFDataArrayDescriptor* dad) {
		return SetDescriptor(vtkDataSetAttributes::TCOORDS, dad);
	}

	//Sets the descriptor for tensors, returns -1 if an error occurs, otherwise,
	//it returns the index of descriptor (that can be used e.g. in GetDescriptor)
	inline int SetTensorsDescriptor(vtkMAFDataArrayDescriptor* dad) {
		return SetDescriptor(vtkDataSetAttributes::TENSORS, dad);
	}

protected:
	//Gets the descriptor with the specified name for the given descriptor types
	//(i.e., SCALARS, VECTORS, etc.). If name is NULL, it returns the active descriptor
	//of that type. Unlike vtkDataSetAttributes, if the descriptor could not be found, 
	//it is not created.
	virtual vtkMAFDataArrayDescriptor* GetDescriptor(int type, const char* name);

	//Sets the descriptor for the given type (e.g., SCALARS, VECTORS etc.),
	//returns -1 if an error occurs, otherwise,
	//it returns the index of descriptor (that can be used e.g. in GetDescriptor)
	virtual int SetDescriptor(int type, vtkMAFDataArrayDescriptor* dad);
		

#pragma endregion	
#pragma endregion

#pragma region LAYOUT OPERATIONS
public:
  /** Sets the physical layout of data array described by the descriptor at index iDsc
  The reference of pLayout is increased => it may be deleted after calling of this routine*/
  virtual void SetLayout(int iDsc, vtkMAFDataArrayLayout* pLayout);

  /** Gets the physical layout of data array described by the descriptor at index iDsc */
  inline virtual vtkMAFDataArrayLayout* GetLayout(int iDsc) 
  {
    if ( iDsc < 0 || iDsc >= this->GetNumberOfDescriptors())		
      return NULL;		

    return this->m_Descriptors[iDsc].pDAL;    
  }

  /** Gets the physical layout of data array described by the descriptor with the given name */
  inline virtual vtkMAFDataArrayLayout* GetLayout(const char* name) {    
    return GetLayout(GetIndexOfDescriptor(name));
  }

#pragma region LAYOUTS FOR SPECIAL DESCRIPTORS
public:
  //Gets the scalars layout with the specified name (or the active one, if not name is NULL)
  inline vtkMAFDataArrayLayout* GetScalarsLayout(const char* name = NULL) {
    return GetLayout(name != NULL ? GetIndexOfDescriptor(name) :
      GetIndexOfDescriptor(vtkDataSetAttributes::SCALARS));      
  }

  //Gets the vectors layout with the specified name (or the active one, if not name is NULL)
  inline vtkMAFDataArrayLayout* GetVectorsLayout(const char* name = NULL) {
    return GetLayout(name != NULL ? GetIndexOfDescriptor(name) :
      GetIndexOfDescriptor(vtkDataSetAttributes::VECTORS));
  }

  //Gets the normals layout with the specified name (or the active one, if not name is NULL)
  inline vtkMAFDataArrayLayout* GetNormalsLayout(const char* name = NULL) {
    return GetLayout(name != NULL ? GetIndexOfDescriptor(name) :
      GetIndexOfDescriptor(vtkDataSetAttributes::NORMALS));
  }

  //Gets the texture coordinates layout with the specified name (or the active one, if not name is NULL)
  inline vtkMAFDataArrayLayout* GetTCoordsLayout(const char* name = NULL) {
    return GetLayout(name != NULL ? GetIndexOfDescriptor(name) :
      GetIndexOfDescriptor(vtkDataSetAttributes::TCOORDS));
  }

  //Gets the tensor layout with the specified name (or the active one, if not name is NULL)
  inline vtkMAFDataArrayLayout* GetTensorsLayout(const char* name = NULL) {
    return GetLayout(name != NULL ? GetIndexOfDescriptor(name) :
      GetIndexOfDescriptor(vtkDataSetAttributes::TENSORS));
  }

  //Sets the layout for scalars, returns -1 if an error occurs, otherwise,
  //it returns the index of layout (that can be used e.g. in GetLayout)	
  inline void SetScalarsLayout(vtkMAFDataArrayLayout* dal) {
    SetLayout(GetIndexOfDescriptor(vtkDataSetAttributes::SCALARS), dal);
  }

  //Sets the layout for vectors, returns -1 if an error occurs, otherwise,
  //it returns the index of layout (that can be used e.g. in GetLayout)
  inline void SetVectorsLayout(vtkMAFDataArrayLayout* dal) {
    SetLayout(GetIndexOfDescriptor(vtkDataSetAttributes::VECTORS), dal);
  }

  //Sets the layout for normals, returns -1 if an error occurs, otherwise,
  //it returns the index of layout (that can be used e.g. in GetLayout)
  inline void SetNormalsLayout(vtkMAFDataArrayLayout* dal) {
    SetLayout(GetIndexOfDescriptor(vtkDataSetAttributes::NORMALS), dal);
  }

  //Sets the layout for texture coordinates, returns -1 if an error occurs, otherwise,
  //it returns the index of layout (that can be used e.g. in GetLayout)
  inline void SetTCoordsLayout(vtkMAFDataArrayLayout* dal) {
    SetLayout(GetIndexOfDescriptor(vtkDataSetAttributes::TCOORDS), dal);
  }

  //Sets the layout for tensors, returns -1 if an error occurs, otherwise,
  //it returns the index of layout (that can be used e.g. in GetLayout)
  inline void SetTensorsLayout(vtkMAFDataArrayLayout* dal) {
    SetLayout(GetIndexOfDescriptor(vtkDataSetAttributes::TENSORS), dal);
  }
#pragma endregion //LAYOUTS FOR SPECIAL DESCRIPTORS
#pragma endregion //LAYOUT OPERATIONS

#pragma region DATA ARRAYS READING/WRITING
public:
	//Constructs a new vtkDataArray object and fills it with a range of tuples from 
	//the data array with the given name, starting at the specified index.
	//If count is longer than the data array, the remainder of the data array is copied.
	//NB: The caller is responsible for the deletion of constructed object.
	inline virtual vtkDataArray* GetDataArray(const char* name, 
		vtkIdType64 startIndex = 0, int countTuples = -1){
			return GetDataArray(GetIndexOfDescriptor(name), startIndex, countTuples);
	}
	
	//Fills the given buffer with a range of tuples from 
	//the data array with the given name, starting at the specified index.
	//If count is longer than the data array, the remainder of the data array is copied.	
	//NB: buffer must be created by the caller (and be compatible with the descriptor)
	//If the buffer is not capable to hold the data, it is enlarged automatically
	inline virtual void GetDataArray(const char* name, vtkDataArray* buffer, 
		vtkIdType64 startIndex = 0, int countTuples = -1) {
		GetDataArray(GetIndexOfDescriptor(name), buffer, startIndex, countTuples);
	}

	//Constructs a new vtkDataArray object and fills it with a range of tuples from 
	//the data array at index idx, starting at the specified index.
	//If count is longer than the data array, the remainder of the data array is copied.
	//NB: The caller is responsible for the deletion of constructed object.
	virtual vtkDataArray* GetDataArray(int idx, vtkIdType64 startIndex = 0, int countTuples = -1);

	//Fills the given buffer with a range of tuples from 
	//the data array at index idx, starting at the specified index.
	//If count is longer than the data array, the remainder of the data array is copied.	
	//NB: buffer must be created by the caller (and be compatible with the descriptor)
	//If the buffer is not capable to hold the data, it is enlarged automatically
	virtual void GetDataArray(int idx, vtkDataArray* buffer, vtkIdType64 startIndex = 0, int countTuples = -1);

	//Fills the given buffer with elements from the data array at index idx, 
	//starting at the specified !element! index. Buffer must be capable to hold these elements.
	//The routine returns the number of stored elements (may be less than count, if the
	//amount of data available is smaller than requested)
	virtual int GetDataArray(int idx, void* buffer, int count, vtkIdType64 startIndex = 0);

	//Stores the data from the given buffer into the data array 
	//with the given name, starting at the specified index.
	inline virtual void SetDataArray(const char* name, 
		vtkDataArray* buffer, vtkIdType64 startIndex = 0) {
		SetDataArray(GetIndexOfDescriptor(name), buffer, startIndex);
	}

	//Stores the data from the given buffer into the data array 
	//denoted by the index, starting at the specified index.
	virtual void SetDataArray(int da_idx, vtkDataArray* buffer, vtkIdType64 startIndex = 0);

	//Stores the elements from the given buffer into the data array 
	//denoted by the index, starting at the specified index.
	//NB: count is given in number of elements (not bytes)
	virtual void SetDataArray(int da_idx, void* buffer, int count,
		vtkIdType64 startIndex = 0);

//SPECIAL DESCRIPTORS
#pragma region SPECIAL DATA ARRAYS
public:

	//Returns the scalars data array. See GetDataArray
	inline vtkDataArray* GetScalars(const char* name, vtkIdType64 startIndex = 0, int countTuples = -1) {
		return GetDataArray(vtkDataSetAttributes::SCALARS,
				name, startIndex, countTuples);
	}

	//Returns the scalars data array. See GetDataArray
	inline void GetScalars(const char* name, vtkDataArray* buffer, 
		vtkIdType64 startIndex = 0, int countTuples = -1) {
		GetDataArray(vtkDataSetAttributes::SCALARS, name, buffer, 
			startIndex, countTuples);
	}

	//Returns the normals data array. See GetDataArray
	inline vtkDataArray* GetNormals(const char* name, vtkIdType64 startIndex = 0, int countTuples = -1) {
		return GetDataArray(vtkDataSetAttributes::NORMALS,
				name, startIndex, countTuples);
	}

	//Returns the normals data array. See GetDataArray
	inline void GetNormals(const char* name, vtkDataArray* buffer, 
		vtkIdType64 startIndex = 0, int countTuples = -1) {
		GetDataArray(vtkDataSetAttributes::NORMALS, name, buffer, 
			startIndex, countTuples);
	}

	//Returns the vectors data array. See GetDataArray
	inline vtkDataArray* GetVectors(const char* name, vtkIdType64 startIndex = 0, int countTuples = -1) {
		return GetDataArray(vtkDataSetAttributes::VECTORS,
				name, startIndex, countTuples);
	}

	//Returns the vectors data array. See GetDataArray
	inline void GetVectors(const char* name, vtkDataArray* buffer, 
		vtkIdType64 startIndex = 0, int countTuples = -1) {
		GetDataArray(vtkDataSetAttributes::VECTORS, name, buffer, 
			startIndex, countTuples);
	}

	//Returns the texture coordinates data array. See GetDataArray
	inline vtkDataArray* GetTCoords(const char* name, vtkIdType64 startIndex = 0, int countTuples = -1) {
		return GetDataArray(vtkDataSetAttributes::TCOORDS,
				name, startIndex, countTuples);
	}

	//Returns the texture coordinates data array. See GetDataArray
	inline void GetTCoords(const char* name, vtkDataArray* buffer, 
		vtkIdType64 startIndex = 0, int countTuples = -1) {
		GetDataArray(vtkDataSetAttributes::TCOORDS, name, buffer, 
			startIndex, countTuples);
	}

	//Returns the tensors data array. See GetDataArray
	inline vtkDataArray* GetTensors(const char* name, vtkIdType64 startIndex = 0, int countTuples = -1) {
		return GetDataArray(vtkDataSetAttributes::TENSORS,
				name, startIndex, countTuples);
	}

	//Returns the tensors data array. See GetDataArray
	inline void GetTensors(const char* name, vtkDataArray* buffer, 
		vtkIdType64 startIndex = 0, int countTuples = -1) {
			GetDataArray(vtkDataSetAttributes::TENSORS, name, buffer, 
			startIndex, countTuples);
	}

		//Sets the scalars data array. See SetDataArray
	inline void SetScalars(const char* name, vtkDataArray* buffer, 
		vtkIdType64 startIndex = 0) {
		SetDataArray(vtkDataSetAttributes::SCALARS, name, buffer, 
			startIndex);
	}

	//Sets the normals data array. See SetDataArray
	inline void SetNormals(const char* name, vtkDataArray* buffer, 
		vtkIdType64 startIndex = 0) {
		SetDataArray(vtkDataSetAttributes::NORMALS, name, buffer, 
			startIndex);
	}

	//Sets the vectors data array. See SetDataArray
	inline void SetVectors(const char* name, vtkDataArray* buffer, 
		vtkIdType64 startIndex = 0) {
		SetDataArray(vtkDataSetAttributes::VECTORS, name, buffer, 
			startIndex);
	}

	//Sets the texture coordinates data array. See SetDataArray
	inline void SetTCoords(const char* name, vtkDataArray* buffer, 
		vtkIdType64 startIndex = 0) {
		SetDataArray(vtkDataSetAttributes::TCOORDS, name, buffer, 
			startIndex);
	}

	//Sets the tensors data array. See SetDataArray
	inline void SetTensors(const char* name, vtkDataArray* buffer, 
		vtkIdType64 startIndex = 0) {
			SetDataArray(vtkDataSetAttributes::TENSORS, name, buffer, 
			startIndex);
	}

protected:
	//Called by GetScalars(), etc. See public GetDataArray
	virtual vtkDataArray* GetDataArray(int type, const char* name, 
		vtkIdType64 startIndex = 0, int countTuples = -1);
	
	//Called by GetScalars(), etc. See public GetDataArray
	virtual void GetDataArray(int type, const char* name, vtkDataArray* buffer, 
		vtkIdType64 startIndex = 0, int countTuples = -1);

	//Called by SetScalars(), etc. See public SetDataArray
	virtual void SetDataArray(int type, const char* name, vtkDataArray* buffer, 
		vtkIdType64 startIndex = 0);


#pragma endregion
#pragma endregion

public:
	// Description:
	// Shallow copy, copies only references (increments refenrece counters)
	void ShallowCopy(vtkMAFLargeDataProvider *src);  

	//Deep copy. NB: Data arrays ARE NOT COPIED
	void DeepCopy(vtkMAFLargeDataProvider *src);
private:
	void InternalDataCopy(vtkMAFLargeDataProvider *src);


protected:
	vtkMAFLargeDataProvider();
	~vtkMAFLargeDataProvider();

protected:
	//replaces the name used in the lookup table, if old_name is NULL, then new entry
	//refereed to the given index is created
	void ReplaceLookupName(const char* old_name, const char* new_name, int index = -1);

  /** Reads or writes the data in interleaved mode */
  int GetSetDataArrayIM(int da_idx, void* buffer, int count, 
    vtkIdType64 startIndex, bool bGetMode);

  /** Reads or writes the data in non-interleaved mode */
  int GetSetDataArrayNIM(int da_idx, void* buffer, int count, 
    vtkIdType64 startIndex, bool bGetMode);
	
  //Updates the default layout information
  //Should be called always before the layout is used
  virtual void UpdateDefaultLayout();

	//Copies the binary data from the underlaying source into the given buffer. 
	//Copying starts at startOffset position and at most count bytes are copied.
	//The routine returns number of bytes successfully transfered.  
	virtual int ReadBinaryData(vtkIdType64 startOffset, void* buffer, int count) = 0;

	//Copies the binary data from the given buffer into the underlaying data set at
	//startOffset position. If the underlaying data set is not capable to hold the
	//specified amount of bytes to be copied (count), it is automatically enlarge
	//(nb: this feature may not be available for all providers)  
	//The routine returns number of bytes successfully transfered.	
	virtual int WriteBinaryData(vtkIdType64 startOffset, void* buffer, int count) = 0; 


private:
	vtkMAFLargeDataProvider(const vtkMAFLargeDataProvider&);  // Not implemented.
	void operator=(const vtkMAFLargeDataProvider&);  // Not implemented.
	static vtkMAFLargeDataProvider* New() {return NULL;};			//Abstract class - not implemented
};

#endif
