/*=========================================================================

 Program: MAF2
 Module: vtkMAFLargeDataProvider
 Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#pragma warning(disable: 4996)	//depricated

#include "vtkMAFLargeDataProvider.h"
#include "vtkObjectFactory.h"
#include "vtkFieldData.h"
#include "vtkIntArray.h"
#include "vtkByteSwap.h"



vtkCxxRevisionMacro(vtkMAFLargeDataProvider, "$Revision: 1.1.2.3 $");

#include "mafMemDbg.h"
#include <assert.h>

vtkMAFLargeDataProvider::vtkMAFLargeDataProvider()
{
	TagArray = vtkFieldData::New();
	
	HeaderSize = 0;
	SwapBytes = false;
  DefaultLayout = true;

	InitializeDescriptors();
}

vtkMAFLargeDataProvider::~vtkMAFLargeDataProvider() 
{
	TagArray->Delete();	
	InitializeDescriptors();
}

// Release all descriptors but do not delete this object.
// Also, the underlaying data is untouched.
/*virtual*/ void vtkMAFLargeDataProvider::InitializeDescriptors()
{
	StringToIntMap::iterator it = DescriptorsMap.begin();
	while (it != DescriptorsMap.end())
	{
		delete[] it->first;
		it++;
	}

	DescriptorsMap.clear();

	for (int i = 0; i < (int)Descriptors.size(); i++)
	{
		if (Descriptors[i].pDAD != NULL)
			Descriptors[i].pDAD->UnRegister(this);

    if (Descriptors[i].pDAL != NULL)
      Descriptors[i].pDAL->UnRegister(this);
	}
	
	Descriptors.clear();

	memset(SpecDescPos, -1, sizeof(SpecDescPos));
	this->Modified();
}

// Return this object's modified time.
/*virtual*/ unsigned long vtkMAFLargeDataProvider::GetMTime()
{
	unsigned long mt = Superclass::GetMTime();
	for (int i = 0; i < (int)Descriptors.size(); i++) 
	{
		unsigned long t = Descriptors[i].pDAD->GetMTime();
		if (t > mt)
			mt = t;

    t = Descriptors[i].pDAL->GetMTime();
    if (t > mt)
      mt = t;
	}

	return mt;
}

//Sets the data array descriptor having the specified name
//The caller is supposed to Delete the given array when it is no longer needed
/*virtual*/ int vtkMAFLargeDataProvider::AddDescriptor(vtkMAFDataArrayDescriptor* dad)
{	
	assert(dad != NULL);

	if (dad == NULL)
		return -1;

  DALD item;
  item.pDAD = dad;
	dad->Register(this);
  item.pDAL = vtkMAFDataArrayLayout::New();
	Descriptors.push_back(item);
	int index = (int)Descriptors.size() - 1;

	ReplaceLookupName(NULL, dad->GetName(), index);
	this->Modified();	//we have changed it
	return index;
}

//Returns index of the descriptor given by name, or -1 if no such descriptor
//exists in the collection
/*virtual*/ int vtkMAFLargeDataProvider::GetIndexOfDescriptor(const char* name)
{
	assert(name != NULL);

	StringToIntMap::iterator it = DescriptorsMap.find(name);
	if (it == DescriptorsMap.end())
		return -1;		//not found

	return it->second;
}

// Remove an array (with the given name) from the list of arrays.
/*virtual*/ void vtkMAFLargeDataProvider::RemoveDescriptor(int i)
{
	if (i < 0 || i >= this->GetNumberOfDescriptors())		
		return;		//invalid index

	StringToIntMap::iterator itrem, it = DescriptorsMap.begin();
	while (it != DescriptorsMap.end())
	{
		if (it->second >= i) 
		{
			//this is index that must be shifted
			if (it->second == i) 
			{			
				//this is the item to be removed
				delete const_cast<char*>(it->first);
				itrem = it;
			}

			it->second--;
		}

		it++;
	}

	DescriptorsMap.erase(itrem);
	Descriptors[i].pDAD->UnRegister(this);
  Descriptors[i].pDAL->UnRegister(this);
	Descriptors.erase(Descriptors.begin() + i);

	//fix m_SpecDescPos
	for (int j = 0; j < vtkDataSetAttributes::NUM_ATTRIBUTES; j++) 
	{
		if (SpecDescPos[j] >= i)
		{
			if (SpecDescPos[j] == i)
				SpecDescPos[j] = -1;	//remove it
			else
				SpecDescPos[j]--;
		}
	}
}

//Gets the descriptor with the specified name for the given descriptor types
//(i.e., SCALARS, VECTORS, etc.). If name is NULL, it returns the active descriptor
//of that type. Unlike vtkDataSetAttributes, if the descriptor could not be found, 
//it is not created.
/*virtual*/ vtkMAFDataArrayDescriptor* vtkMAFLargeDataProvider::GetDescriptor(int type, const char* name)
{
  if (type < 0 || type >= vtkDataSetAttributes::NUM_ATTRIBUTES) 
  {
	  assert(type >= 0 && type < vtkDataSetAttributes::NUM_ATTRIBUTES);
    return NULL;
  }

	if (name == NULL || *name != '\0')
		return GetDescriptor(SpecDescPos[type]);

	//if name is specified, try to find it
	return GetDescriptor(name);
}

//Sets the descriptor for the given type (e.g., SCALARS, VECTORS etc.),
//returns -1 if an error occurs, otherwise,
//it returns the index of descriptor (that can be used e.g. in GetDescriptor)
/*virtual*/ int vtkMAFLargeDataProvider::SetDescriptor(int type, vtkMAFDataArrayDescriptor* dad)
{
  if (type < 0 || type >= vtkDataSetAttributes::NUM_ATTRIBUTES) 
  {
    assert(type >= 0 && type < vtkDataSetAttributes::NUM_ATTRIBUTES);
    return -1;
  }
	
	int curPos = SpecDescPos[type];

	// If there is an existing attribute, replace it
	if (curPos < 0)
		curPos = AddDescriptor(dad);
	else
	{
		if (GetDescriptor(curPos) == dad)		
			return curPos;	//the same, no change
		
		if (dad == NULL)
			RemoveDescriptor(curPos);
		else
		{
			//we will have to modify it
      DALD& item = Descriptors[curPos];
			vtkMAFDataArrayDescriptor* old = item.pDAD;
			ReplaceLookupName(old->GetName(), dad->GetName());
			old->UnRegister(this);			

			item.pDAD = dad;
			dad->Register(this);
		}
	}
	
	SpecDescPos[type] = curPos;
	this->Modified();
	return curPos;
}


//returns index of the special descriptor
/*virtual*/ int vtkMAFLargeDataProvider::GetIndexOfDescriptor(int type)
{
  if (type < 0 || type >= vtkDataSetAttributes::NUM_ATTRIBUTES) 
  {
    assert(type >= 0 && type < vtkDataSetAttributes::NUM_ATTRIBUTES);
    return -1;  //error
  }
	
	return SpecDescPos[type];
}

//sets index of the special descriptor
/*virtual*/ void vtkMAFLargeDataProvider::SetIndexOfDescriptor(int type, int idx)
{
  if (type < 0 || type >= vtkDataSetAttributes::NUM_ATTRIBUTES) 
  {
    assert(type >= 0 && type < vtkDataSetAttributes::NUM_ATTRIBUTES);
    return;  //error
  }
	
  assert(idx >= 0 && idx < (int)Descriptors.size());  
	SpecDescPos[type] = idx;
}

//replaces the name used in the lookup table, if old_name is NULL, then new entry
//refereed to the given index is created
void vtkMAFLargeDataProvider
	::ReplaceLookupName(const char* old_name, const char* new_name, int index)
{	
	if (old_name == NULL && new_name == NULL)
		return;	//nothing to be replaced, lets quit
	
	if (old_name != NULL)	
	{
		StringToIntMap::iterator it = DescriptorsMap.find(old_name);
		if (it != DescriptorsMap.end())
		{
			delete[] it->first;
			DescriptorsMap.erase(it);
		}
	}

	//construct the copy of string
	char* new_name_cp = NULL;
	if (new_name != NULL)
	{
		assert(index >= 0);

		new_name_cp = new char[(int)strlen(new_name) + 1];
		strcpy(new_name_cp, new_name);

		DescriptorsMap.insert(StringToIntMap::value_type(new_name_cp, index));	
	}	
}

//------------------------------------------------------------------------
//Sets the physical layout of data array described by the descriptor at index iDsc
//The reference of pLayout is increased => it may be deleted after calling of this routine
/*virtual*/ void vtkMAFLargeDataProvider::SetLayout(int iDsc, vtkMAFDataArrayLayout* pLayout)
//------------------------------------------------------------------------
{
  if (pLayout == NULL || iDsc < 0 || iDsc >= this->GetNumberOfDescriptors())	{
    assert(false); return;		//invalid arguments
  }

  DALD& item = this->Descriptors[iDsc];
  if (item.pDAL != pLayout)
  {
    pLayout->Register(this);
    item.pDAL->UnRegister(this);
    item.pDAL = pLayout;
    this->Modified();
  }
}

//Updates the default layout information
//Should be called always before the layout is used
/*virtual*/ void vtkMAFLargeDataProvider::UpdateDefaultLayout()
{
  if (this->DefaultLayout && this->GetMTime() > OffsetsComputeTime)
  {
    //we have to recalculate global offsets table				
    vtkIdType64 ofs = this->GetHeaderSize();				
    for (int i = 0; i < (int)Descriptors.size(); i++)
    {
      DALD& item = Descriptors[i];
      item.pDAL->SetStartOffset(ofs);
      item.pDAL->SetNumberOfComponents(item.pDAD->GetNumberOfComponents());
      ofs += item.pDAD->GetActualMemorySize();			
    }

    OffsetsComputeTime.Modified();
  }  
}

//Constructs a new vtkDataArray object and fills it with a range of tuples from 
//the data array at index idx, starting at the specified index.
//If count is longer than the data array, the remainder of the data array is copied.
//NB: The caller is responsible for the deletion of constructed object.
/*virtual*/ vtkDataArray* vtkMAFLargeDataProvider
	::GetDataArray(int idx, vtkIdType64 startIndex, int countTuples)
{
	vtkMAFDataArrayDescriptor* desc = GetDescriptor(idx);
	if (desc == NULL)
		return NULL;

	vtkDataArray* retArray = vtkDataArray::CreateDataArray(desc->GetDataType());
  
  //number of tuples and components set in the following routine	
	GetDataArray(idx, retArray, startIndex, countTuples);
	return retArray;
}

//Fills the given buffer with a range of tuples from 
//the data array at index idx, starting at the specified index.
//If count is longer than the data array, the remainder of the data array is copied.	
//NB: buffer must be created by the caller (and be compatible with the descriptor)
//If the buffer is not capable to hold the data, it is enlarged automatically
/*virtual*/ void vtkMAFLargeDataProvider
	::GetDataArray(int idx, vtkDataArray* buffer, vtkIdType64 startIndex, int countTuples)
{	
	vtkMAFDataArrayDescriptor* desc = GetDescriptor(idx);
	if (desc == NULL)
		buffer->Reset();	//set the array empty
	else
	{
		//readjust the amount of tuples to be taken
		vtkIdType64 size = desc->GetNumberOfTuples();
		vtkIdType64 endIndex = countTuples < 0 ? size : startIndex + countTuples;
		if (endIndex >= size)
			countTuples = endIndex - startIndex;

		//get the buffer to direct access
		int numComps = desc->GetNumberOfComponents(); 
    buffer->SetNumberOfComponents(numComps);    
    buffer->SetNumberOfTuples(countTuples);        

    //N.B. SetNumberOfTuples allocated memory for numComps*countTuples elements
		vtkIdType64 read = GetDataArray(idx, buffer->GetVoidPointer(0), 
      countTuples*numComps, startIndex*numComps);
    buffer->SetNumberOfTuples(read / numComps);   //set the read size
	}
}

//Called by GetScalars(), etc. See public GetDataArray
/*virtual*/ vtkDataArray* vtkMAFLargeDataProvider::GetDataArray(int type, const char* name, 
									   vtkIdType64 startIndex, int countTuples)
{
  if (type < 0 || type >= vtkDataSetAttributes::NUM_ATTRIBUTES) 
  {
    assert(type >= 0 && type < vtkDataSetAttributes::NUM_ATTRIBUTES);
    return NULL;  //error
  }

	if (name == NULL || *name != '\0')
		return GetDataArray(SpecDescPos[type], startIndex, countTuples);

	//if name is specified, try to find it
	return GetDataArray(name, startIndex, countTuples);
}

//Called by GetScalars(), etc. See public GetDataArray
/*virtual*/ void vtkMAFLargeDataProvider::GetDataArray(int type, const char* name, 
							vtkDataArray* buffer, vtkIdType64 startIndex, int countTuples)
{
  if (type < 0 || type >= vtkDataSetAttributes::NUM_ATTRIBUTES) 
  {
    assert(type >= 0 && type < vtkDataSetAttributes::NUM_ATTRIBUTES);
    return;  //error
  }

	if (name == NULL || *name != '\0')
		GetDataArray(SpecDescPos[type], buffer, startIndex, countTuples);
	else
		//if name is specified, try to find it
		GetDataArray(name, buffer, startIndex, countTuples);
}

//------------------------------------------------------------------------
//Fills the given buffer with elements from the data array at index idx, 
//starting at the specified !element! index. Buffer must be capable to hold these elements.
//The routine returns the number of stored elements (may be less than count, if the
//amount of data available is smaller than requested)
/*virtual*/ int vtkMAFLargeDataProvider::GetDataArray(int idx, void* buffer, 
                                                              int count, vtkIdType64 startIndex)
{
  //For the caller convenience| the data is supposed to be stored linearly as follows:
  //
  //ELEMENT INDEX:   |0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20| ...     
  //COMPONENT INDEX: |0|1|2|0|1|2|0|1|2|0| 1| 2|0 | 1| 2|0 | 1| 2|0 | 1| 2| ... 
  //TUPLE INDEX:     |  0  |  1  |  2  |  3    |    4   |   5    |   6    | ...
  //so supposing that startIndex is 13 and count is 6 than we will retrieve
  //the second and the third component of the fifth tuple, all components
  //from the sixth tuple and the first component from the seventh component
  //the physical layout of data may be, however, different. The worst scenario:
  //????|20|???|0|??????????|1|?|2|3| ....
  //we support starting offset (i.e., element index 0 may not be 0 in the media)
  //gaps between tuples (same gap) and gaps between components (they may differ)
  //and also the option that components are stored in non-interleaved mode
  //N.B. there is no difference between the setting of the gap after the last 
  //component to zero and the tuple gap to G and the setting of the gap after 
  //the last component to G and the tuple gap to zero
  //
  //The general physical layout in the interleaved mode is:
  //|startOffset|c1|gap(c1)|c2|gap(c2) ...cn|gap(cn)|tuple gap|c1|gap(c1)| ...
  //and in the non-interleaved mode (tuple size is given as NonInterleavedSize:
  //|startOffset|c1|c1|...|c1|gap(c1)|c2|c2|...|c2|gap(c2) ...|cn|gap(cn)|tuple gap|c1| ...

  vtkMAFDataArrayDescriptor* pDAD = GetDescriptor(idx);
  if (pDAD == NULL)
    return 0;

  if (pDAD->GetDataType() == VTK_BIT) {
    vtkErrorMacro(<< "Binary arrays are not supported by vtkMAFLargeDataProvider.");
    return 0;	//bit arrays are not supported  
  }

  //get the physical layout
  vtkMAFDataArrayLayout* pDAL = GetLayout(idx);
  UpdateDefaultLayout();  //and update the information

  //readjust the amount of elements to be taken
  int elemSize = pDAD->GetDataTypeSize();
  vtkIdType64 size = pDAD->GetSize();
  vtkIdType64 endIndex = startIndex + count;
  if (endIndex >= size)
    count = size - startIndex;	  
  
  int totalRead = 0;  
  if (pDAL->GetNonInterleaved() != 0) {
    //Non-Interleaved mode => we will need to combine data
    totalRead = GetSetDataArrayNIM(idx, buffer, count, startIndex, true);
  }
  else
  {
    //Interleaved mode => components ordered      
    if (pDAL->GetTupleGap() != 0 || pDAL->GetComponentGaps() != NULL) {
      totalRead = GetSetDataArrayIM(idx, buffer, count, startIndex, true);
    }
    else
    {
      //the simplest option, the logical and physical data layout is compatible            
      //now, we need to read count elements starting from startIndex into the given buffer
      //we must compute the physical address of every element and load it  
      vtkIdType64 ofset = pDAL->GetStartOffset() + startIndex*elemSize;
      totalRead = ReadBinaryData(ofset, buffer, count*elemSize);
    }    
  }
 
  // handle swapping
  if (GetSwapBytes())
    vtkByteSwap::SwapVoidRange(buffer, totalRead / elemSize, elemSize);

  return totalRead / elemSize;
}


//Stores the data from the given buffer into the data array 
//denoted by the index, starting at the specified index.
/*virtual*/ void vtkMAFLargeDataProvider::SetDataArray(int da_idx, vtkDataArray* buffer, vtkIdType64 startIndex)
{
	vtkMAFDataArrayDescriptor* desc = GetDescriptor(da_idx);
	if (desc == NULL) {
		assert(false);
		return;			//error
	}

	//get the buffer to direct access
	int numComps = desc->GetNumberOfComponents();
	vtkIdType64 buff_sz = buffer->GetMaxId() + 1;
	SetDataArray(da_idx, buffer->GetVoidPointer(0), buff_sz, startIndex*numComps);
}

//Stores the elements from the given buffer into the data array 
//denoted by the index, starting at the specified index.
//NB: count is given in number of elements (not bytes)
/*virtual*/ void vtkMAFLargeDataProvider::SetDataArray(int da_idx, void* buffer, int count,
													vtkIdType64 startIndex)
{
	vtkMAFDataArrayDescriptor* desc = GetDescriptor(da_idx);
	if (desc == NULL) {
		assert(false);
		return;
	}

	if (desc->GetDataType() == VTK_BIT) {
		assert(false);
		vtkErrorMacro(<< "Binary arrays are not supported by vtkMAFLargeDataProvider.");
		return;	//bit arrays are not supported
	}
  
  //get the physical layout
  vtkMAFDataArrayLayout* pDAL = GetLayout(da_idx);
  UpdateDefaultLayout();  //and update the information

  //compute position in bytes	into the underlaying data source
  int elemSize = desc->GetDataTypeSize(); 

  //we will need to save the data in buf
  char* buf = (char*)buffer;  
  if (GetSwapBytes())
  {
    // handle swapping
    buf = new char[count*elemSize];
    memcpy(buf, buffer, count);
    vtkByteSwap::SwapVoidRange(buf, count, elemSize);
  }
	
  int totalWritten = 0;
  if (pDAL->GetNonInterleaved() != 0) {
    //non-interleaved mode
     totalWritten = GetSetDataArrayNIM(da_idx, buf, count, startIndex, false);
  }
  else
  {
    //Interleaved mode => components ordered  
    if (pDAL->GetTupleGap() != 0 || pDAL->GetComponentGaps() != NULL) {
      totalWritten = GetSetDataArrayIM(da_idx, buf, count, startIndex, false);
    }
    else
    {
      //the simplest option, the logical and physical data layout is compatible            
      vtkIdType64 ofset = pDAL->GetStartOffset() + startIndex*elemSize;
      totalWritten = WriteBinaryData(ofset, buf, count*elemSize);
    }    
  }

	if (GetSwapBytes())
		delete[] buf;   //release memory

  assert(totalWritten == count*elemSize);
	this->Modified();  
}


//------------------------------------------------------------------------
//Reads or writes the data in interleaved mode 
int vtkMAFLargeDataProvider::GetSetDataArrayIM(int da_idx, void* buffer, 
                         int count, vtkIdType64 startIndex, bool bGetMode)
//------------------------------------------------------------------------
{
  //get descriptor
  vtkMAFDataArrayDescriptor* pDAD = GetDescriptor(da_idx);
  if (pDAD == NULL) return 0;   //should not happen
  int elemSize = pDAD->GetDataTypeSize();
  
  //get layout
  vtkMAFDataArrayLayout* pDAL = GetLayout(da_idx);
  if (pDAL == NULL) return 0; //should not happen
  vtkIdType64 nTupleGapSize = pDAL->GetTupleGap();
  vtkIdType64* pCompGaps = pDAL->GetComponentGaps();  
  int nNumOfComps = pDAL->GetNumberOfComponents();
  assert(nNumOfComps == pDAD->GetNumberOfComponents());

  //we have some gaps in the physic data layout, so it is incompatible
  //create vector of increments
  vtkIdType64* pIncr = new vtkIdType64[nNumOfComps];
  if (pCompGaps == NULL)
  {
    for (int i = 0; i < nNumOfComps; i++) {
      pIncr[i] = elemSize;  //advance to the next component
    }
  }
  else
  {
    for (int i = 0; i < nNumOfComps; i++) {
      pIncr[i] = elemSize + pCompGaps[i];
    }
  }

  pIncr[nNumOfComps - 1] += nTupleGapSize;

  //compute now sum vector
  vtkIdType64* pIncrSum = new vtkIdType64[nNumOfComps];
  pIncrSum[0] = pIncr[0];
  for (int i = 1; i < nNumOfComps; i++) {
    pIncrSum[i] = pIncrSum[i - 1] + pIncr[i];
  }

  //create the buffer for data      
  int nReadSize = ((count / nNumOfComps) + ((count % nNumOfComps) != 0))*pIncrSum[nNumOfComps - 1];
  int nMaxBufSize = count*elemSize*8;   //we allow 8x more data at most
  if (nMaxBufSize > 8192)               //and at most 8KB for buffer
    nMaxBufSize = 8192;                

  int nBufSize = nMaxBufSize < nReadSize ? nMaxBufSize : nReadSize;
  BYTE* pBuf = new BYTE[nBufSize]; 

  //compute the physical offset of the first component to be read        
  vtkIdType64 nTuplesSoFar = (startIndex / nNumOfComps);  //tuples processed completely
  int nCompsSoFar = (int)(startIndex % nNumOfComps);      //components in the last tuple
  vtkIdType64 curOffset = pDAL->GetStartOffset() +        //global offset
    nTuplesSoFar*pIncrSum[nNumOfComps - 1] +              //tuples so far
    nCompsSoFar*pIncrSum[nCompsSoFar];                    //components so far

  int totalProcessed = 0;
  int nElemsToProcess = count;           //the total number of elements to process
  while (nElemsToProcess > 0)
  {
    nReadSize = ((nElemsToProcess / nNumOfComps) + 
      ((nElemsToProcess % nNumOfComps) != 0))*pIncrSum[nNumOfComps - 1];
    int nToRead = nReadSize < nBufSize ? nReadSize : nBufSize;        
    int nRead = ReadBinaryData(curOffset, pBuf, nToRead);
    
    if (bGetMode)
      curOffset += nRead;  //advance the position (if we are in read mode)
    else
    {
      //write mode, if we have reached EOF, reset the rest of buffer
      if (nRead < nToRead)
      {
        memset(&pBuf[nRead], 0, nToRead - nRead);
        nRead = nToRead;
      }
    }

    //compute number of elements that were read
    int nReadElements = 0;
    int iCurComp = (int)((startIndex + totalProcessed) % nNumOfComps);  //current component is
    while (iCurComp != 0 && nRead >= elemSize)
    {          
      nReadElements++;
      nRead -= pIncr[iCurComp];
      iCurComp = (iCurComp + 1) % nNumOfComps;
    }

    //add the whole number of tuples
    if (nRead >= elemSize)
    {
      int nTuples = nRead / pIncrSum[nNumOfComps - 1];
      nReadElements += nTuples*nNumOfComps;
      nRead -= nTuples * pIncrSum[nNumOfComps - 1];

      //there is still some data in the buffer
      while (nRead >= elemSize)
      {
        nReadElements++;
        nRead -= pIncr[iCurComp];
        iCurComp = (iCurComp + 1) & nNumOfComps;            
      }
    }

    if (nReadElements == 0)
      break;  //feof, unable to read/write more elements        

    if (nElemsToProcess < nReadElements)
      nReadElements = nElemsToProcess;    //this may happen, if we have some zeros in pIncr

    if (bGetMode)
    {         
      //read mode
      //copy elements from the buffer into the user array        
      BYTE* pSrc = pBuf;
      BYTE* pDst = &((BYTE*)buffer)[totalProcessed];   
      iCurComp = (int)((startIndex + totalProcessed) % nNumOfComps);  //current component is

      curOffset -= nRead;               //adjust the current position
      nElemsToProcess -= nReadElements; //decrease number of bytes to be read
      totalProcessed += nReadElements;
      while (nReadElements)
      {          
        for (int j = 0; j < elemSize; j++) {
          pDst[j] = pSrc[j]; //copy one byte            
        }

        pSrc += pIncr[iCurComp];
        pDst += elemSize;

        iCurComp = (iCurComp + 1) % nNumOfComps;          
        nReadElements--; //another element processed          
      } //while (nReadElements)
    }
    else
    {
      //write mode
      //copy elements from the buffer into the user array        
      BYTE* pDst = pBuf;
      BYTE* pSrc = &((BYTE*)buffer)[totalProcessed];      
      iCurComp = (int)((startIndex + totalProcessed) % nNumOfComps);  //current component is
      
      nElemsToProcess -= nReadElements; //decrease number of bytes to be read/written
      totalProcessed += nReadElements;
      while (nReadElements > 0)
      {
        for (int j = 0; j < elemSize; j++) {
          pDst[j] = pSrc[j]; //copy one byte            
        }

        pSrc += elemSize;
        pDst += pIncr[iCurComp];

        iCurComp = (iCurComp + 1) % nNumOfComps;          
        nReadElements--; //another element processed          
      } //while (nReadElements)

      //write the data
      WriteBinaryData(curOffset, pBuf, (pDst - pBuf) - 
        pIncr[(iCurComp + nNumOfComps - 1) % nNumOfComps] + elemSize);
      curOffset += pDst - pBuf;               //adjust the current position      
    }    
  } //while (nElemsToProcess > 0)

  delete[] pBuf;
  delete[] pIncrSum;
  delete[] pIncr;

  return totalProcessed*elemSize;
}

//------------------------------------------------------------------------
//Reads or writes the data in non-interleaved mode
int vtkMAFLargeDataProvider::GetSetDataArrayNIM(int da_idx, void* buffer, 
                       int count, vtkIdType64 startIndex, bool bGetMode)
//------------------------------------------------------------------------
{
  //get descriptor
  vtkMAFDataArrayDescriptor* pDAD = GetDescriptor(da_idx);
  if (pDAD == NULL) return 0;   //should not happen
  int elemSize = pDAD->GetDataTypeSize();

  //get layout
  vtkMAFDataArrayLayout* pDAL = GetLayout(da_idx);
  if (pDAL == NULL) return 0; //should not happen
  vtkIdType64 nTupleGapSize = pDAL->GetTupleGap();
  vtkIdType64* pCompGaps = pDAL->GetComponentGaps();  
  int nNumOfComps = pDAL->GetNumberOfComponents();
  assert(nNumOfComps == pDAD->GetNumberOfComponents());

  //create vector of increments (this will handle gaps as well)
  vtkIdType64* pIncr = new vtkIdType64[nNumOfComps];
  if (pCompGaps == NULL)
  {
    for (int i = 0; i < nNumOfComps; i++) {
      pIncr[i] = elemSize;  //advance to the next component
    }
  }
  else
  {
    for (int i = 0; i < nNumOfComps; i++) {
      pIncr[i] = elemSize + pCompGaps[i];
    }
  }

  pIncr[nNumOfComps - 1] += nTupleGapSize;

  //compute now sum vector
  vtkIdType64* pIncrSum = new vtkIdType64[nNumOfComps];
  pIncrSum[0] = pIncr[0];
  for (int i = 1; i < nNumOfComps; i++) {
    pIncrSum[i] = pIncrSum[i - 1] + pIncr[i];
  }

  //create the buffer for data           
  vtkIdType64 nNonInterleavedSize = pDAL->GetNonInterleavedSize();
  int nReadSize = count;
  if (nReadSize > nNonInterleavedSize)    
    nReadSize = (int)nNonInterleavedSize;  //makes no sense to read more than NIS elements    
  nReadSize *= elemSize; 

  //we will use most 8KB for buffer
  int nMaxBufSize = ((8192 / elemSize) + 1)*elemSize;
  int nBufSize = nMaxBufSize < nReadSize ? nMaxBufSize : nReadSize;
  BYTE* pBuf = new BYTE[nBufSize];

  //compute the physical offset of the first component to be read
  vtkIdType64 nTuplesSoFar = (startIndex / nNumOfComps);  //tuples processed completely
  int nCompsSoFar = (int)(startIndex % nNumOfComps);      //components in the last tuple

  //nTuplesSoFar also denotes the number of elements of one kind so far
  //these are grouped into nTB blocks (so there is total number nTB*nNumOfComps
  //of blocks for all components), every block has nNonInterleavedSize*elemSize
  //bytes for elements and some bytes for the component gap
  int nTB = (int)(nTuplesSoFar / nNonInterleavedSize);
  vtkIdType64 startOffset = pDAL->GetStartOffset() +
    nTB * (nNumOfComps * nNonInterleavedSize * elemSize + pIncrSum[nNumOfComps - 1]);

  //process every component separately 
  BYTE* pDstEnd = ((BYTE*)buffer) + count;
  for (int iDstComp = 0; iDstComp < nNumOfComps; iDstComp++)
  {
    //compute number of elements to read for the iDstComp component
    int nElemsToProcess = count / nNumOfComps;
    if ((count % nNumOfComps) > ((iDstComp + nNumOfComps - nCompsSoFar) % nNumOfComps))
      nElemsToProcess++;  //the current component is present in the last tuple      

    //compute the index where the data starts in the current block
    vtkIdType64 idxStart = (nTuplesSoFar % nNonInterleavedSize);
    if (iDstComp < nCompsSoFar)
      idxStart++; //skip the beginning of tuple      

    //compute the physical offset for the first component
    vtkIdType64 curOffset = startOffset + (iDstComp*nNonInterleavedSize + idxStart)*elemSize;
    BYTE* pDst = &((BYTE*)buffer)[(iDstComp + nCompsSoFar) % nNumOfComps];
    while (nElemsToProcess > 0)
    { 
      int nTotalReadSize = nElemsToProcess;
      vtkIdType64 nElemsInCurBlock = nNonInterleavedSize - idxStart;
      if (nTotalReadSize > nElemsInCurBlock)
        nTotalReadSize = (int)nElemsInCurBlock;

      nTotalReadSize *= elemSize;
      while (nTotalReadSize > 0)
      {
        nReadSize = nTotalReadSize < nBufSize ? nTotalReadSize : nBufSize;
        int nRead = ReadBinaryData(curOffset, pBuf, nReadSize);
        if (bGetMode)
        {
          //read mode
          nTotalReadSize -= nRead;  //decrease the number of read elements
          curOffset += nRead;       //advance the position

          //compute number of elements that were read
          int nReadElements = nRead / elemSize;          
          if (nReadElements == 0) {
            break;  //feof, unable to read more elements
          }

          nElemsToProcess -= nReadElements;

          //copy elements from the buffer into the user array        
          BYTE* pSrc = pBuf;
          while (nReadElements > 0)
          {          
            for (int j = 0; j < elemSize; j++) {
              pDst[j] = pSrc[j]; //copy one byte            
            }

            pSrc += elemSize;
            pDst += elemSize*nNumOfComps;

            nReadElements--; //another element processed          
          } //while (nReadElements)
        }
        else
        {
          //write mode
          if (nRead < nReadSize) {
            memset(&pBuf[nRead], 0, nReadSize - nRead);
            nRead = nReadSize;
          }
          
          //compute number of elements that were read
          int nReadElements = nRead / elemSize;          
          if (nReadElements == 0) {
            break;  //feof, unable to read more elements
          }

          if (nElemsToProcess < nReadElements)
            nReadElements = nElemsToProcess;
          nElemsToProcess -= nReadElements;

          //copy elements from the buffer into the user array        
          BYTE* pDst2 = pBuf;
          BYTE* pDst2End = pDst2 + nReadElements*elemSize;

          int nWrElements = 0;
          while (pDst2 < pDst2End)
          {          
            for (int j = 0; j < elemSize; j++) {
              pDst2[j] = pDst[j]; //copy one byte            
            }

            pDst += elemSize*nNumOfComps;
            pDst2 += elemSize;

            nWrElements++;
          } //while (nReadElements)          

          WriteBinaryData(curOffset, pBuf, nWrElements*elemSize);
          nRead = nReadElements*elemSize;
          nTotalReadSize -= nRead;  //decrease the number of read elements
          curOffset += nRead;       //advance the position
        } //if (bGetMode)
      } //while (nTotalReadSize > 0)

      if (nTotalReadSize != 0)
      {
        //oops, we have reached unexpectedly EOF
        if (pDst < pDstEnd)
          pDstEnd = pDst;

        nElemsToProcess = 0;
      }

      //advance the position to the next block
      curOffset += pIncrSum[nNumOfComps - 1] + //all gaps
        nNonInterleavedSize*(nNumOfComps - 1)*elemSize;
      startIndex = 0; 
    } //while (nElemsToProcess > 0)
  } //for (int iDstComp = 0; iDstComp < nNumOfComps; iDstComp++)

  delete[] pBuf;
  delete[] pIncrSum;
  delete[] pIncr;
  
  if (bGetMode)
    return pDstEnd - ((BYTE*)buffer);
  else
    return count*elemSize;
}


//Called by SetScalars(), etc. See public SetDataArray
/*virtual*/ void vtkMAFLargeDataProvider::SetDataArray(int type, const char* name, 
													vtkDataArray* buffer, 
													vtkIdType64 startIndex)
{
  if (type < 0 || type >= vtkDataSetAttributes::NUM_ATTRIBUTES) 
  {
    assert(type >= 0 && type < vtkDataSetAttributes::NUM_ATTRIBUTES);
    return;  //error
  }

	if (name == NULL || *name != '\0')
		SetDataArray(SpecDescPos[type], buffer, startIndex);
	else
		//if name is specified, try to find it
		SetDataArray(name, buffer, startIndex);
}

// Description:
// Shallow copy, copies only references (increments refenrece counters)
void vtkMAFLargeDataProvider::ShallowCopy(vtkMAFLargeDataProvider *src)
{
	if (src != NULL)
	{
		InternalDataCopy(src);

		for (int i = 0; i < (int)src->Descriptors.size(); i++)
		{
      DALD& item = src->Descriptors[i];
      item.pDAD->Register(this);
      item.pDAL->Register(this);
			this->Descriptors.push_back(item);
		}
	}
}

//Deep copy. NB: Data arrays ARE NOT COPIED
void vtkMAFLargeDataProvider::DeepCopy(vtkMAFLargeDataProvider *src)
{
	if (src != NULL)
	{
		InternalDataCopy(src);

		for (int i = 0; i < (int)src->Descriptors.size(); i++)
		{
      DALD item = src->Descriptors[i];			
			vtkMAFDataArrayDescriptor* dd = vtkMAFDataArrayDescriptor::New();
			dd->DeepCopy(item.pDAD);

      vtkMAFDataArrayLayout* dl = vtkMAFDataArrayLayout::New();
      dl->DeepCopy(item.pDAL);

			item.pDAD = dd; item.pDAL = dl;
			this->Descriptors.push_back(item);
		}
	}
}

void vtkMAFLargeDataProvider::InternalDataCopy(vtkMAFLargeDataProvider *src)
{
	this->InitializeDescriptors();

	memcpy(this->SpecDescPos, src->SpecDescPos, sizeof(SpecDescPos));
	this->OffsetsComputeTime = src->OffsetsComputeTime;
	this->SwapBytes = src->SwapBytes;
	this->HeaderSize = src->HeaderSize;
  this->DefaultLayout = src->DefaultLayout;

	for (StringToIntMap::iterator it = src->DescriptorsMap.begin();
		it != src->DescriptorsMap.end(); it++)
	{
		this->ReplaceLookupName(NULL, it->first, it->second);
	}
}