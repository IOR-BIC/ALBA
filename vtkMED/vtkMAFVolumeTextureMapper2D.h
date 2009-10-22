/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFVolumeTextureMapper2D.h,v $ 
  Language: C++ 
  Date: $Date: 2009-10-22 08:17:54 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================

  vtkMAFVolumeTextureMapper2D is equivalent to the standard vtkVolumeTextureMapper2D
  but it does not suffer from incorrect rendering when the view renders also
  polydata (e.g., gizmo) and also it does not rebuild texture every time, only if
  it is necessary (e.g., resolution has changed, camera moved)
*/
#ifndef vtkMAFVolumeTextureMapper2D_h__
#define vtkMAFVolumeTextureMapper2D_h__

#include "vtkOpenGLVolumeTextureMapper2D.h"

class vtkRenderer;
class vtkVolume;

class VTK_RENDERING_EXPORT vtkMAFVolumeTextureMapper2D : public vtkOpenGLVolumeTextureMapper2D
{
protected:
  unsigned long LastCheckSum;   //<Here is stored CheckSum used to detect if Casting needs to be reexecuted

public:
  static vtkMAFVolumeTextureMapper2D *New();
  vtkTypeRevisionMacro(vtkMAFVolumeTextureMapper2D, vtkOpenGLVolumeTextureMapper2D);	

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
  vtkMAFVolumeTextureMapper2D() {
    LastCheckSum = 0;
  }
  ~vtkMAFVolumeTextureMapper2D() {}

private:
  vtkMAFVolumeTextureMapper2D(const vtkMAFVolumeTextureMapper2D&);  // Not implemented.
  void operator=(const vtkMAFVolumeTextureMapper2D&);  // Not implemented.
};

#endif // vtkMAFVolumeTextureMapper2D_h__