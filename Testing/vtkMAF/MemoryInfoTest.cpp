/*=========================================================================

 Program: MAF2Medical
 Module: MemoryInfoTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "MemoryInfoTest.h"

#include "vtkMEDPoissonSurfaceReconstruction.h"

//-------------------------------------------------------------------------
void MemoryInfoTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
    MemoryInfo *mem = new MemoryInfo();

    delete mem;
}
//-------------------------------------------------------------------------
void MemoryInfoTest::TestStaticAllocation()
//-------------------------------------------------------------------------
{
    MemoryInfo mem;
}
//-------------------------------------------------------------------------
void MemoryInfoTest::TestSet()
//-------------------------------------------------------------------------
{
    MemoryInfo mem;
    mem.set();
}
//-------------------------------------------------------------------------
void MemoryInfoTest::TestUsage1()
//-------------------------------------------------------------------------
{
    MemoryInfo mem;
    size_t size = mem.usage();
    CPPUNIT_ASSERT(size == (mem.TotalVirtualAddressSpace-mem.FreeVirtualAddressSpace));
}
//-------------------------------------------------------------------------
void MemoryInfoTest::TestUsage2()
//-------------------------------------------------------------------------
{
    MemoryInfo mem;
    size_t size = mem.Usage();

    MEMORY_BASIC_INFORMATION mbi; 
    size_t      valToCheck = 0; 
    PVOID      pvAddress = 0;
    memset(&mbi, 0, sizeof(MEMORY_BASIC_INFORMATION)); 
    while(VirtualQuery(pvAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == sizeof(MEMORY_BASIC_INFORMATION)){ 
        if(mbi.State == MEM_COMMIT && mbi.Type == MEM_PRIVATE){valToCheck += mbi.RegionSize;}
        pvAddress = ((BYTE*)mbi.BaseAddress) + mbi.RegionSize; 
    }

    CPPUNIT_ASSERT(size == valToCheck);
}