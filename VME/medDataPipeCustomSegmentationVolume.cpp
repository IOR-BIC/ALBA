/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medDataPipeCustomSegmentationVolume.cpp,v $
Language:  C++
Date:      $Date: 2010-04-21 10:41:15 $
Version:   $Revision: 1.1.2.4 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2010
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medDataPipeCustomSegmentationVolume.h"

#include "mafVME.h"
#include "medVMESegmentationVolume.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMath.h"
#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkShortArray.h"
#include "vtkDoubleArray.h"
#include "vtkImageCast.h"
#include "vtkPointData.h"
#include "vtkStructuredPoints.h"
#include "vtkUnsignedCharArray.h"

#include "itkImageToVTKImageFilter.h"
#include "itkVTKImageToImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"


#define round(x) (x<0?ceil((x)-0.5):floor((x)+0.5))

typedef  itk::Image< double, 3> RealImage;

//------------------------------------------------------------------------------
mafCxxTypeMacro(medDataPipeCustomSegmentationVolume)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
medDataPipeCustomSegmentationVolume::medDataPipeCustomSegmentationVolume()
//------------------------------------------------------------------------------
{
  m_Volume = NULL;
  m_ManualVolumeMask = NULL;
  m_RG = NULL;
  vtkNEW(m_RG);
  m_SP = NULL;
  vtkNEW(m_SP);
  m_AutomaticRG = NULL;
  vtkNEW(m_AutomaticRG);
  m_AutomaticSP = NULL;
  vtkNEW(m_AutomaticSP);
  m_ManualRG = NULL;
  vtkNEW(m_ManualRG);
  m_ManualSP = NULL;
  vtkNEW(m_ManualSP);

  m_ChangedAutomaticData = false;
  m_ChangedManualData = false;

  SetInput(NULL);
}

//------------------------------------------------------------------------------
medDataPipeCustomSegmentationVolume::~medDataPipeCustomSegmentationVolume()
//------------------------------------------------------------------------------
{
  vtkDEL(m_RG);
  vtkDEL(m_SP);
  vtkDEL(m_AutomaticRG);
  vtkDEL(m_AutomaticSP);
  vtkDEL(m_ManualRG);
  vtkDEL(m_ManualSP);
  //////////////////////////////////////////////////////////////////////////
  for (int i=0;i<m_AutomaticSegmentationRanges.size();i++)
  {
    delete []m_AutomaticSegmentationRanges[i];
  }
  m_AutomaticSegmentationRanges.clear();
}
//------------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::DeepCopy(mafDataPipe *pipe)
//------------------------------------------------------------------------------
{
  if (mafDataPipe::DeepCopy(pipe) == MAF_OK)
  {
    medDataPipeCustomSegmentationVolume *dpcp = medDataPipeCustomSegmentationVolume::SafeDownCast(pipe);
    if (dpcp != NULL)
    {
      m_Volume = ((medDataPipeCustomSegmentationVolume*)pipe)->GetVolume();
      m_ManualVolumeMask = ((medDataPipeCustomSegmentationVolume*)pipe)->GetManualVolumeMask();

      m_AutomaticSegmentationThresholdModality = ((medDataPipeCustomSegmentationVolume*)pipe)->GetAutomaticSegmentationThresholdModality();
      m_AutomaticSegmentationGlobalThreshold = ((medDataPipeCustomSegmentationVolume*)pipe)->GetAutomaticSegmentationGlobalThreshold();

      for (int i=0;i<((medDataPipeCustomSegmentationVolume*)pipe)->GetNumberOfRanges();i++)
      {
        int startSlice,endSlice;
        double threshold;
        ((medDataPipeCustomSegmentationVolume*)pipe)->GetRange(i,startSlice,endSlice,threshold);
        this->AddRange(startSlice,endSlice,threshold);
      }

      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//------------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::SetAutomaticSegmentationThresholdModality(int modality)
//------------------------------------------------------------------------------
{
  m_AutomaticSegmentationThresholdModality = modality;
  m_ChangedAutomaticData = true;
  Modified();
}
//------------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::ApplyManualSegmentation()
//------------------------------------------------------------------------------
{
  if (m_ManualVolumeMask == NULL)
  {
    return;
  }

  mafVME *vol = mafVME::SafeDownCast(m_Volume);
  vol->GetOutput()->Update();
  vtkDataSet *volumeData = vol->GetOutput()->GetVTKData();
  volumeData->Update();

  vtkDataSet *maskVolumeData = mafVME::SafeDownCast(m_ManualVolumeMask)->GetOutput()->GetVTKData();
  maskVolumeData->Update();
  
  vtkDataArray *maskScalar = maskVolumeData->GetPointData()->GetScalars();
  vtkDataArray *automaticScalar;
  
  if(maskVolumeData->IsA("vtkRectilinearGrid")) 
  {
    m_AutomaticRG->Update();
    if (m_AutomaticRG->GetPointData()->GetScalars()->GetNumberOfTuples() != maskScalar->GetNumberOfTuples())
    {
      return;
    }
    automaticScalar = m_AutomaticRG->GetPointData()->GetScalars();
  }
  else if (maskVolumeData->IsA("vtkStructuredPoints"))
  {
    m_AutomaticSP->Update();
    if (m_AutomaticSP->GetPointData()->GetScalars()->GetNumberOfTuples() != maskScalar->GetNumberOfTuples())
    {
      return;
    }
    automaticScalar = m_AutomaticSP->GetPointData()->GetScalars();
  }

  vtkMAFSmartPointer<vtkUnsignedCharArray> newScalars;
  newScalars->SetName("SCALARS");
  newScalars->SetNumberOfTuples(maskScalar->GetNumberOfTuples());

  long progress = 0;

  mafEvent e(this,PROGRESSBAR_SHOW);
  this->GetVME()->ForwardUpEvent(&e);

  int step = ceil((double)maskScalar->GetNumberOfTuples()/100);
  double invStep = (double)1/step;
  for (int i=0;i<maskScalar->GetNumberOfTuples();i++)
  {
    if ((i%step) == 0)
    {
	    progress = (i*100/maskScalar->GetNumberOfTuples());
	    mafEvent eUpdate(this,PROGRESSBAR_SET_VALUE,progress);
	    this->GetVME()->ForwardUpEvent(&eUpdate);
    }

    if (maskScalar->GetTuple1(i) == 0)
    {
      double value = automaticScalar->GetTuple1(i);
      newScalars->SetTuple1(i,value);
    }
    if (maskScalar->GetTuple1(i) == 255)
    {
      double value = automaticScalar->GetTuple1(i);
      newScalars->SetTuple1(i,255-value);
    }
  }

  if (volumeData->IsA("vtkStructuredPoints"))
  {
    vtkMAFSmartPointer<vtkStructuredPoints> newSP;
    newSP->CopyStructure(vtkStructuredPoints::SafeDownCast(volumeData));
    newSP->Update();
    newSP->GetPointData()->AddArray(newScalars);
    newSP->GetPointData()->SetActiveScalars("SCALARS");
    newSP->SetScalarTypeToUnsignedChar();
    newSP->Update();

    m_ManualSP->DeepCopy(newSP);
    m_ManualSP->Update();
    m_SP->DeepCopy(newSP);
    m_SP->Update();
  }
  else
  {
    vtkMAFSmartPointer<vtkRectilinearGrid> newRG;
    newRG->CopyStructure(vtkRectilinearGrid::SafeDownCast(volumeData));
    newRG->Update();
    newRG->GetPointData()->AddArray(newScalars);
    newRG->GetPointData()->SetActiveScalars("SCALARS");
    newRG->Update();

    m_ManualRG->DeepCopy(newRG);
    m_ManualRG->Update();
    m_RG->DeepCopy(newRG);
    m_RG->Update();
  }

  mafEvent eHideProgress(this,PROGRESSBAR_HIDE);
  this->GetVME()->ForwardUpEvent(&eHideProgress);  

}
//------------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::ApplyAutomaticSegmentation()
//------------------------------------------------------------------------------
{
  mafVME *vol = mafVME::SafeDownCast(m_Volume);
  vol->GetOutput()->Update();
  vtkDataSet *volumeData = vol->GetOutput()->GetVTKData();
  volumeData->Update();

  typedef itk::BinaryThresholdImageFilter<RealImage, RealImage> BinaryThresholdFilter;
  BinaryThresholdFilter::Pointer binaryThreshold = BinaryThresholdFilter::New();

  int volumeDimensions[3];
  if(volumeData->IsA("vtkRectilinearGrid")) {
    vtkRectilinearGrid *rectilinearGrid=vtkRectilinearGrid::SafeDownCast(volumeData);
    rectilinearGrid->GetDimensions(volumeDimensions);
  }
  else if (volumeData->IsA("vtkStructuredPoints")) {
    vtkStructuredPoints *imageData=vtkStructuredPoints::SafeDownCast(volumeData);
    imageData->GetDimensions(volumeDimensions);
  }

  long progress = 0;

  mafEvent e(this,PROGRESSBAR_SHOW);
  this->GetVME()->ForwardUpEvent(&e);

  vtkMAFSmartPointer<vtkUnsignedCharArray> newScalars;
  newScalars->SetName("SCALARS");
  newScalars->SetNumberOfTuples(volumeDimensions[0]*volumeDimensions[1]*volumeDimensions[2]);

  int step = ceil((double)volumeDimensions[2]/100);
  double invStep = (double)1/step;

  for (int i=0;i<volumeDimensions[2];i++)
  {
    if (i%step == 0)
    {
	    progress = (i*100/volumeDimensions[2]);
	    mafEvent eUpdate(this,PROGRESSBAR_SET_VALUE,progress);
	    this->GetVME()->ForwardUpEvent(&eUpdate);
    }

    double localThreshold;
    if(m_AutomaticSegmentationThresholdModality == medVMESegmentationVolume::RANGE)
    {
      //Find the correct threshold for the slice
      int rangeIndex = -1;
      for (int j=0;j<m_AutomaticSegmentationRanges.size();j++)
      {
        if (i>=(m_AutomaticSegmentationRanges[j][0]) && i<=(m_AutomaticSegmentationRanges[j][1]))
        {
          rangeIndex = j;
          localThreshold = m_AutomaticSegmentationThresholds[j];
        }
      }

      if (rangeIndex == -1)
      {
        return;
      }
    }
    else
    {
      localThreshold = m_AutomaticSegmentationGlobalThreshold;
    }
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    //Generate the slice image data for itk filters
    //////////////////////////////////////////////////////////////////////////
    double spacing[3];

    if (volumeData->IsA("vtkStructuredPoints"))
    {
      vtkStructuredPoints::SafeDownCast(volumeData)->GetSpacing(spacing); 
    }
    else if (volumeData->IsA("vtkRectilinearGrid"))
    {
      vtkDataArray *x = vtkRectilinearGrid::SafeDownCast(volumeData)->GetXCoordinates();
      vtkDataArray *y = vtkRectilinearGrid::SafeDownCast(volumeData)->GetYCoordinates();

      //assumed that the slices are image data
      spacing[0] = x->GetTuple1(1) - x->GetTuple1(0);
      spacing[1] = y->GetTuple1(1) - y->GetTuple1(0);
      spacing[2] = 0.0;
    }

    vtkDataArray *inputScalars = volumeData->GetPointData()->GetScalars();
    vtkMAFSmartPointer<vtkDoubleArray> scalars;
    scalars->SetName("SCALARS");
    scalars->SetNumberOfTuples(volumeDimensions[0]*volumeDimensions[1]);
    for (int k=0;k<(volumeDimensions[0]*volumeDimensions[1]);k++)
    {
      double value = inputScalars->GetTuple1(k+i*(volumeDimensions[0]*volumeDimensions[1]));
      scalars->SetTuple1(k,value);
    }

    vtkMAFSmartPointer<vtkImageData> im;
    im->SetDimensions(volumeDimensions[0],volumeDimensions[1],1);
    im->SetSpacing(spacing[0],spacing[1],0.0);
    im->SetScalarTypeToDouble();
    im->GetPointData()->AddArray(scalars);
    im->GetPointData()->SetActiveScalars("SCALARS");
    im->Update();

//     vtkMAFSmartPointer<vtkImageCast> vtkImageToFloat;
//     vtkImageToFloat->SetOutputScalarTypeToFloat();
//     vtkImageToFloat->SetInput(im);
//     vtkImageToFloat->Modified();
//     vtkImageToFloat->Update();
    //////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////
    //ITK pipeline to binarize
    //////////////////////////////////////////////////////////////////////////
    typedef itk::VTKImageToImageFilter< RealImage > ConvertervtkTOitk;
    ConvertervtkTOitk::Pointer vtkTOitk = ConvertervtkTOitk::New();
    vtkTOitk->SetInput( im );
    vtkTOitk->Update();

    double range[2];
    volumeData->GetScalarRange(range);

    binaryThreshold->SetInput( ((RealImage*)vtkTOitk->GetOutput()) );
    binaryThreshold->SetInsideValue(255);
    binaryThreshold->SetOutsideValue(0);
    binaryThreshold->SetLowerThreshold(localThreshold);
    binaryThreshold->SetUpperThreshold(range[1]);

    try
    {
      binaryThreshold->Update();
    }
    catch ( itk::ExceptionObject &err )
    {
      std::cout << "ExceptionObject caught !" << std::endl; 
      std::cout << err << std::endl; 
    }

    typedef itk::ImageToVTKImageFilter< RealImage > ConverteritkTOvtk;
    ConverteritkTOvtk::Pointer itkTOvtk = ConverteritkTOvtk::New();
    itkTOvtk->SetInput( binaryThreshold->GetOutput() );
    itkTOvtk->Update();
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    //Copy the new scalars
    //////////////////////////////////////////////////////////////////////////
    vtkImageData *imageBinary = ((vtkImageData*)itkTOvtk->GetOutput());
    imageBinary->Update();

    vtkDataArray *binaryScalars = imageBinary->GetPointData()->GetScalars();
    for (int k=0;k<imageBinary->GetNumberOfPoints();k++)
    {
      char value = binaryScalars->GetTuple1(k);
      newScalars->SetTuple1(k+i*(volumeDimensions[0]*volumeDimensions[1]),value);
    }
    //////////////////////////////////////////////////////////////////////////

  }

  if (volumeData->IsA("vtkStructuredPoints"))
  {
    vtkMAFSmartPointer<vtkStructuredPoints> newSP;
    newSP->CopyStructure(vtkStructuredPoints::SafeDownCast(volumeData));
    newSP->Update();
    newSP->GetPointData()->AddArray(newScalars);
    newSP->GetPointData()->SetActiveScalars("SCALARS");
    //newSP->SetScalarTypeToShort();
    newSP->Update();

    m_AutomaticSP->DeepCopy(newSP);
    m_AutomaticSP->Update();
    m_SP->DeepCopy(newSP);
    m_SP->Update();
  }
  else
  {
    vtkMAFSmartPointer<vtkRectilinearGrid> newRG;
    newRG->CopyStructure(vtkRectilinearGrid::SafeDownCast(volumeData));
    newRG->Update();
    newRG->GetPointData()->AddArray(newScalars);
    newRG->GetPointData()->SetActiveScalars("SCALARS");
    newRG->Update();

    m_AutomaticRG->DeepCopy(newRG);
    m_AutomaticRG->Update();
    m_RG->DeepCopy(newRG);
    m_RG->Update();
  }

  mafEvent eHideProgress(this,PROGRESSBAR_HIDE);
  this->GetVME()->ForwardUpEvent(&eHideProgress);


}
//------------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::PreExecute()
//------------------------------------------------------------------------------
{

  mafVME *vol = mafVME::SafeDownCast(m_Volume);

  if(vol)
  {
    vol->GetOutput()->Update();
    vtkDataSet *volumeData = vol->GetOutput()->GetVTKData();
    if(volumeData)
    {
      if (CheckNumberOfThresholds() && m_ChangedAutomaticData)
      {
        ApplyAutomaticSegmentation();
      }
      if ((m_ChangedAutomaticData && CheckNumberOfThresholds()) || m_ChangedManualData)
      {
        ApplyManualSegmentation();
      }

      m_ChangedAutomaticData = false;
      m_ChangedManualData = false;
    }
  }
  else
  {
    SetInput(NULL);
  }
}
//------------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::Execute()
//------------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::AddRange(int startSlice,int endSlice,double threshold)
//----------------------------------------------------------------------------
{
  if (!this->GetVME()->IsDataAvailable())
  {
    return MAF_ERROR;
  }

  if (AutomaticCheckRange(startSlice,endSlice) == MAF_ERROR)
  {
    return MAF_ERROR;
  }
  
  int *range = new int[2];
  range[0] = startSlice;
  range[1] = endSlice;
  m_AutomaticSegmentationRanges.push_back(range);
  m_AutomaticSegmentationThresholds.push_back(threshold);

  m_ChangedAutomaticData = true;
  Modified();
  return MAF_OK;
}
//----------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::GetRange(int index,int &startSlice, int &endSlice, double &threshold)
//----------------------------------------------------------------------------
{
  if (index<0 || index>m_AutomaticSegmentationRanges.size()-1)
  {
    return MAF_ERROR;
  }

  startSlice = m_AutomaticSegmentationRanges[index][0];
  endSlice = m_AutomaticSegmentationRanges[index][1];
  threshold = m_AutomaticSegmentationThresholds[index];

  return MAF_OK;
}
//------------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::SetVolume(mafNode *volume)
//------------------------------------------------------------------------------
{
  m_Volume = volume;
  
  if (m_Volume == NULL)
  {
    return;
  }

  if (mafVME::SafeDownCast(m_Volume)->GetOutput()->GetVTKData() && mafVME::SafeDownCast(m_Volume)->GetOutput()->GetVTKData()->IsA("vtkRectilinearGrid"))
  {
    SetInput(m_RG);
    Modified();
  }
  else if (mafVME::SafeDownCast(m_Volume)->GetOutput()->GetVTKData() && mafVME::SafeDownCast(m_Volume)->GetOutput()->GetVTKData()->IsA("vtkStructuredPoints"))
  {
    SetInput(m_SP);
    Modified();
  }

  m_ChangedAutomaticData = true;
  m_ChangedManualData = true;
}
//------------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::SetManualVolumeMask(mafNode *volume)
//------------------------------------------------------------------------------
{
  m_ManualVolumeMask = volume;
  
  m_ChangedManualData = true;
  Modified();
}
//------------------------------------------------------------------------------
vtkDataSet *medDataPipeCustomSegmentationVolume::GetAutomaticOutput()
//------------------------------------------------------------------------------
{
  mafVME *vol = mafVME::SafeDownCast(m_Volume);
  if(vol)
  {
    vol->GetOutput()->Update();
    vtkDataSet *volumeData = vol->GetOutput()->GetVTKData();
    if(volumeData)
    {
      volumeData->Update();
      if(volumeData->IsA("vtkRectilinearGrid"))
      {
        return m_AutomaticRG;
      }
      else if (volumeData->IsA("vtkImageData"))
      {
        return m_AutomaticSP;
      }
    }
  }
}
//----------------------------------------------------------------------------
bool medDataPipeCustomSegmentationVolume::CheckNumberOfThresholds()
//----------------------------------------------------------------------------
{
  //////////////////////////////////////////////////////////////////////////
  //Check that all slices have a threshold

  if (m_AutomaticSegmentationThresholdModality == medVMESegmentationVolume::GLOBAL)
  {
    return true;
  }

  mafVME *vol = mafVME::SafeDownCast(m_Volume);

  if(vol)
  {
    vol->GetOutput()->Update();
    vtkDataSet *volumeData = vol->GetOutput()->GetVTKData();
    if(volumeData)
    {
      volumeData->Update();

      int volumeDimensions[3];
      if(volumeData->IsA("vtkRectilinearGrid")) {
        vtkRectilinearGrid *rectilinearGrid=vtkRectilinearGrid::SafeDownCast(volumeData);
        rectilinearGrid->GetDimensions(volumeDimensions);
      }
      else if (volumeData->IsA("vtkStructuredPoints")) {
        vtkStructuredPoints *sp=vtkStructuredPoints::SafeDownCast(volumeData);
        sp->GetDimensions(volumeDimensions);
      }

      for (int i=0;i<volumeDimensions[2];i++)
      {
        bool found = false;
        for (int j=0;j<m_AutomaticSegmentationRanges.size();j++)
        {
          if (i>=(m_AutomaticSegmentationRanges[j][0]) && i<=(m_AutomaticSegmentationRanges[j][1]))
          {
            found = true;
            break;
          }
        }
        if (!found)
        {
          mafLogMessage("Slice %d° hasn't a threshold",i+1);
          return false;
        }
      }

      return true;
    }
  }
  else
  {
    return false;
  }
  //////////////////////////////////////////////////////////////////////////
}
//----------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::UpdateRange(int index,int &startSlice, int &endSlice, double &threshold)
//----------------------------------------------------------------------------
{
  if (m_AutomaticSegmentationRanges.size()==0 || index<0 || index>(m_AutomaticSegmentationRanges.size()-1) || AutomaticCheckRange(startSlice,endSlice,index) == MAF_ERROR)
  {
    return MAF_ERROR;
  }
  m_AutomaticSegmentationRanges[index][0] = startSlice;
  m_AutomaticSegmentationRanges[index][1] = endSlice;
  m_AutomaticSegmentationThresholds[index] = threshold;

  m_ChangedAutomaticData = true;
  Modified();
  return MAF_OK;
}
//----------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::RemoveAllRanges()
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_AutomaticSegmentationRanges.size();i++)
  {
    delete []m_AutomaticSegmentationRanges[i];
  }
  m_AutomaticSegmentationRanges.clear();
  m_AutomaticSegmentationThresholds.clear();

  m_ChangedAutomaticData = true;
  Modified();

  return MAF_OK;
}
//----------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::DeleteRange(int index)
//----------------------------------------------------------------------------
{
  if (m_AutomaticSegmentationRanges.size()==0 || index<0 || index>(m_AutomaticSegmentationRanges.size()-1))
  {
    return MAF_ERROR;
  }

  for (int i=0,j=0;i<m_AutomaticSegmentationRanges.size();i++)
  {
    if (i != index)
    {
      j++;
      m_AutomaticSegmentationRanges[j][0] = m_AutomaticSegmentationRanges[i][0];
      m_AutomaticSegmentationRanges[j][1] = m_AutomaticSegmentationRanges[i][1];
      m_AutomaticSegmentationThresholds[j] = m_AutomaticSegmentationThresholds[i];
    }
  }

  delete []m_AutomaticSegmentationRanges[m_AutomaticSegmentationRanges.size()-1];
  m_AutomaticSegmentationRanges.pop_back();
  m_AutomaticSegmentationThresholds.pop_back();

  m_ChangedAutomaticData = true;
  Modified();
  return MAF_OK;
}
//------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::SetAutomaticSegmentationGlobalThreshold(double threshold)
//------------------------------------------------------------------------
{
  m_AutomaticSegmentationGlobalThreshold = threshold;
  m_ChangedAutomaticData = true;
  Modified();
}
//------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::AutomaticCheckRange(int startSlice,int endSlice,int indexToExclude)
//------------------------------------------------------------------------
{
  int minInt,maxInt;
  minInt = startSlice;
  maxInt = endSlice;

  if (minInt > maxInt)
  {
    mafLogMessage("Lower slice index is higher than upper slice index");
    return MAF_ERROR;
  }

  //Check if any range overlaps with the actual
  for (int i=0;i<m_AutomaticSegmentationRanges.size();i++)
  {
    if (indexToExclude != i)
    {
	    if (m_AutomaticSegmentationRanges[i][1] == maxInt || m_AutomaticSegmentationRanges[i][0] == minInt)
	    {
	      mafLogMessage("Slice already inserted in the automatic segmentation");
	      return MAF_ERROR;
	    }
	
	    if (m_AutomaticSegmentationRanges[i][1] >= minInt && m_AutomaticSegmentationRanges[i][1] <= maxInt)
	    {
	      mafLogMessage("Overlaps of slices range");
	      return MAF_ERROR;
	    }
	
	    if (m_AutomaticSegmentationRanges[i][0] >= minInt && m_AutomaticSegmentationRanges[i][0] <= maxInt)
	    {
	      mafLogMessage("Overlaps of slices range");
	      return MAF_ERROR;
	    }
    }
  }

  return MAF_OK;
}
