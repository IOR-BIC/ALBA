/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFVolumeRayCastMapper.h,v $ 
  Language: C++ 
  Date: $Date: 2011-05-26 08:33:31 $ 
  Version: $Revision: 1.1.2.3 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================

  vtkMAFVolumeRayCastMapper is equivalent to the standard vtkVolumeRayCastMapper
  but it does not suffer from incorrect rendering when the view renders also
  polydata (e.g., gizmo) and also it does not recast rays every time, only if
  it is necessary (e.g., resolution has changed, camera moved)
*/
#ifndef vtkMAFVolumeRayCastMapper_h__
#define vtkMAFVolumeRayCastMapper_h__

#include "vtkMEDConfigure.h"
#include "vtkOpenGLVolumeRayCastMapper.h"

class vtkRenderer;
class vtkVolume;

class VTK_vtkMED_EXPORT vtkMAFVolumeRayCastMapper : public vtkOpenGLVolumeRayCastMapper
{
protected:
  unsigned long LastCheckSum;   //<Here is stored CheckSum used to detect if Casting needs to be reexecuted

public:
  static vtkMAFVolumeRayCastMapper *New();
  vtkTypeRevisionMacro(vtkMAFVolumeRayCastMapper, vtkOpenGLVolumeRayCastMapper);	

public:
  // WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
  // Initialize rendering for this volume.
  virtual void Render( vtkRenderer *ren, vtkVolume *vol );

protected:
  
  /** Savenko's function to calculate checksum of byte data stream */
  inline unsigned long CalculateChecksum(unsigned char* mPtr, int nSize)
  {
    unsigned long checksum = 0;
    for (int ii = 0; ii < nSize; ii++)
      checksum += int(mPtr[ii] + 1) * (151 + ii) + int(mPtr[ii] + 7) * (31 * ii);
    return checksum;  
  }

protected:
  vtkMAFVolumeRayCastMapper() {
    LastCheckSum = 0;
  }
  ~vtkMAFVolumeRayCastMapper() {}

private:
  vtkMAFVolumeRayCastMapper(const vtkMAFVolumeRayCastMapper&);  // Not implemented.
  void operator=(const vtkMAFVolumeRayCastMapper&);  // Not implemented.
};

#endif // vtkMAFVolumeRayCastMapper_h__