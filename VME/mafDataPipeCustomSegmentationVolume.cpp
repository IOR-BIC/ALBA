/*=========================================================================

 Program: MAF2
 Module: mafDataPipeCustomSegmentationVolume
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
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

#include "mafDataPipeCustomSegmentationVolume.h"

#include "mafVME.h"
#include "mafVMESegmentationVolume.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMath.h"
#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkShortArray.h"
#include "vtkDoubleArray.h"
#include "vtkImageCast.h"
#include "vtkPointData.h"
#include "vtkRectilinearGrid.h"
#include "vtkImageData.h"
#include "vtkUnsignedCharArray.h"

#include "itkImageToVTKImageFilter.h"
#include "itkVTKImageToImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"
#include "mafProgressBarHelper.h"


typedef  itk::Image< double, 3> RealImage;
typedef  itk::Image< unsigned char, 3> UCharImage;

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafDataPipeCustomSegmentationVolume)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafDataPipeCustomSegmentationVolume::mafDataPipeCustomSegmentationVolume()
//------------------------------------------------------------------------------
{
  m_Volume = NULL;
  m_ManualVolumeMask = NULL;
  m_RefinementVolumeMask = NULL;
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
  m_RefinementRG = NULL;
  vtkNEW(m_RefinementRG);
  m_RefinementSP = NULL;
  vtkNEW(m_RefinementSP);
  m_RegionGrowingRG = NULL;
  vtkNEW(m_RegionGrowingRG);
  m_RegionGrowingSP = NULL;
  vtkNEW(m_RegionGrowingSP);
  
  m_ChangedAutomaticData = false;
  m_ChangedManualData = false;
  m_ChangedRefinementData = false;
  m_ChangedRegionGrowingData = false;

  SetInput(NULL);
}

//------------------------------------------------------------------------------
mafDataPipeCustomSegmentationVolume::~mafDataPipeCustomSegmentationVolume()
//------------------------------------------------------------------------------
{
  vtkDEL(m_RG);
  vtkDEL(m_SP);
  vtkDEL(m_AutomaticRG);
  vtkDEL(m_AutomaticSP);
  vtkDEL(m_ManualRG);
  vtkDEL(m_ManualSP);
  vtkDEL(m_RefinementRG);
  vtkDEL(m_RefinementSP);
  vtkDEL(m_RegionGrowingRG);
  vtkDEL(m_RegionGrowingSP);
  //////////////////////////////////////////////////////////////////////////
  for (int i=0;i<m_AutomaticSegmentationRanges.size();i++)
  {
    delete []m_AutomaticSegmentationRanges[i];
  }
  m_AutomaticSegmentationRanges.clear();

  for (int i=0;i<m_RegionGrowingSeeds.size();i++)
  {
    delete []m_RegionGrowingSeeds[i];
  }
  m_RegionGrowingSeeds.clear();
}
//------------------------------------------------------------------------------
int mafDataPipeCustomSegmentationVolume::DeepCopy(mafDataPipe *pipe)
//------------------------------------------------------------------------------
{
  if (mafDataPipe::DeepCopy(pipe) == MAF_OK)
  {
    mafDataPipeCustomSegmentationVolume *dpcp = mafDataPipeCustomSegmentationVolume::SafeDownCast(pipe);
    if (dpcp != NULL)
    {
      m_Volume = ((mafDataPipeCustomSegmentationVolume*)pipe)->GetVolume();
      m_ManualVolumeMask = ((mafDataPipeCustomSegmentationVolume*)pipe)->GetManualVolumeMask();

      m_AutomaticSegmentationThresholdModality = ((mafDataPipeCustomSegmentationVolume*)pipe)->GetAutomaticSegmentationThresholdModality();
      m_UseDoubleThreshold = ((mafDataPipeCustomSegmentationVolume*)pipe)->GetDoubleThresholdModality();

      m_AutomaticSegmentationGlobalThreshold = ((mafDataPipeCustomSegmentationVolume*)pipe)->GetAutomaticSegmentationGlobalThreshold();

      for (int i=0;i<((mafDataPipeCustomSegmentationVolume*)pipe)->GetNumberOfRanges();i++)
      {
        int startSlice,endSlice;
        double threshold;
        ((mafDataPipeCustomSegmentationVolume*)pipe)->GetRange(i,startSlice,endSlice,threshold);
        this->AddRange(startSlice,endSlice,threshold);
      }

      m_RegionGrowingUpperThreshold = ((mafDataPipeCustomSegmentationVolume*)pipe)->GetRegionGrowingUpperThreshold();
      m_RegionGrowingLowerThreshold = ((mafDataPipeCustomSegmentationVolume*)pipe)->GetRegionGrowingLowerThreshold();

      for (int i=0;i<((mafDataPipeCustomSegmentationVolume*)pipe)->GetNumberOfSeeds();i++)
      {
        int seed[3];
        ((mafDataPipeCustomSegmentationVolume*)pipe)->GetSeed(i,seed);
        this->AddSeed(seed);
      }

      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//------------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolume::SetAutomaticSegmentationThresholdModality(int modality)
//------------------------------------------------------------------------------
{
  m_AutomaticSegmentationThresholdModality = modality;
  m_ChangedAutomaticData = true;
  Modified();
}

//------------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolume::SetDoubleThresholdModality(int modality)
//------------------------------------------------------------------------------
{
 
  m_UseDoubleThreshold = modality;
  m_ChangedAutomaticData = true;
  Modified();
}

//------------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolume::ApplyManualSegmentation()
//------------------------------------------------------------------------------
{
  if (m_ManualVolumeMask == NULL)
  {
    return;
  }

  m_Volume->GetOutput()->Update();
  vtkDataSet *volumeData = m_Volume->GetOutput()->GetVTKData();

  vtkDataSet *maskVolumeData = m_ManualVolumeMask->GetOutput()->GetVTKData();
  
  if (maskVolumeData==NULL || maskVolumeData->GetNumberOfPoints() != volumeData->GetNumberOfPoints() || maskVolumeData->GetPointData()->GetScalars()==NULL)
  {
    mafLogMessage("Error! Wrong manual volume mask");
    return;
  }
  
  vtkDataArray *maskScalar = maskVolumeData->GetPointData()->GetScalars();
  vtkDataArray *regionGrowingScalar;
  
  if(maskVolumeData->IsA("vtkRectilinearGrid")) 
  {
    if (m_RegionGrowingRG->GetPointData()->GetScalars() == NULL)
    {
      return;
    }
    if (m_RegionGrowingRG->GetPointData()->GetScalars()->GetNumberOfTuples() != maskScalar->GetNumberOfTuples())
    {
      return;
    }
    regionGrowingScalar = m_RegionGrowingRG->GetPointData()->GetScalars();
  }
  else if (maskVolumeData->IsA("vtkImageData"))
  {
    if (m_RegionGrowingSP->GetPointData()->GetScalars() == NULL)
    {
      return;
    }
    if (m_RegionGrowingSP->GetPointData()->GetScalars()->GetNumberOfTuples() != maskScalar->GetNumberOfTuples())
    {
      return;
    }
    regionGrowingScalar = m_RegionGrowingSP->GetPointData()->GetScalars();
  }

  vtkMAFSmartPointer<vtkUnsignedCharArray> newScalars;
  newScalars->SetName("SCALARS");
  newScalars->SetNumberOfTuples(maskScalar->GetNumberOfTuples());

	mafProgressBarHelper progressHelper(this->GetVME());
	progressHelper.InitProgressBar("",false);

  for (int i=0;i<maskScalar->GetNumberOfTuples();i++)
  {
    progressHelper.UpdateProgressBar(i*100/maskScalar->GetNumberOfTuples());

    if (maskScalar->GetTuple1(i) == 0)
    {
      double value = regionGrowingScalar->GetTuple1(i);
      newScalars->SetTuple1(i,value);
    }
    else if (maskScalar->GetTuple1(i) == 255)
    {
      double value = regionGrowingScalar->GetTuple1(i);
      newScalars->SetTuple1(i,255-value);
    }
  }

  if (volumeData->IsA("vtkImageData"))
  {
    vtkMAFSmartPointer<vtkImageData> newSP;
    newSP->CopyStructure(vtkImageData::SafeDownCast(volumeData));
    newSP->GetPointData()->AddArray(newScalars);
    newSP->GetPointData()->SetActiveScalars("SCALARS");

    m_ManualSP->DeepCopy(newSP);
    m_SP->DeepCopy(newSP);
  }
  else
  {
    vtkMAFSmartPointer<vtkRectilinearGrid> newRG;
    newRG->CopyStructure(vtkRectilinearGrid::SafeDownCast(volumeData));
    newRG->GetPointData()->AddArray(newScalars);
    newRG->GetPointData()->SetActiveScalars("SCALARS");

    m_ManualRG->DeepCopy(newRG);

    m_RG->DeepCopy(newRG);
  }
}
//------------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolume::ApplyAutomaticSegmentation()
//------------------------------------------------------------------------------
{
  m_Volume->GetOutput()->Update();
  vtkDataSet *volumeData = m_Volume->GetOutput()->GetVTKData();
  
  int volumeDimensions[3];
  if(volumeData->IsA("vtkRectilinearGrid")) {
    vtkRectilinearGrid *rectilinearGrid=vtkRectilinearGrid::SafeDownCast(volumeData);
    rectilinearGrid->GetDimensions(volumeDimensions);
  }
  else if (volumeData->IsA("vtkImageData")) {
    vtkImageData *imageData=vtkImageData::SafeDownCast(volumeData);
    imageData->GetDimensions(volumeDimensions);
  }

	mafProgressBarHelper progressHelper(this->GetVME());
	progressHelper.SetTextMode(this->GetVME()->GetTestMode());
	progressHelper.InitProgressBar("",false);


  vtkMAFSmartPointer<vtkUnsignedCharArray> newScalars;
  newScalars->SetName("SCALARS");
  newScalars->SetNumberOfTuples(volumeDimensions[0]*volumeDimensions[1]*volumeDimensions[2]);

  int step = ceil((double)volumeDimensions[2]/100);
  double invStep = (double)1/step;

  for (int i=0;i<volumeDimensions[2];i++)
  {
	  progressHelper.UpdateProgressBar(i*100/volumeDimensions[2]);
	    
		double localThreshold;
    double localUpperTheshold;
    bool inRange = false;
    if(m_AutomaticSegmentationThresholdModality == mafVMESegmentationVolume::RANGE)
    {
      //Find the correct threshold for the slice
      int rangeIndex = -1;
      for (int j=0;j<m_AutomaticSegmentationRanges.size();j++)
      {
        if (i>=(m_AutomaticSegmentationRanges[j][0]) && i<=(m_AutomaticSegmentationRanges[j][1]))
        {
          rangeIndex = j;
          localThreshold = m_AutomaticSegmentationThresholds[j];
          localUpperTheshold = m_AutomaticSegmentationUpperThresholds[j];
          inRange = true;
          break;
        }
      }
    }
    else
    {
      inRange = true;
      localThreshold = m_AutomaticSegmentationGlobalThreshold;
      localUpperTheshold = m_AutomaticSegmentationGlobalUpperThreshold;
    }
    //////////////////////////////////////////////////////////////////////////

    int numberOfPoints = volumeDimensions[0] * volumeDimensions[1];
    if(!inRange)
    // range not specified
    {
      
      for (int k=0;k<numberOfPoints;k++)
      {
        char value = 0;
        newScalars->SetTuple1(k + i*numberOfPoints,value);
      }
    }

    else
    {
      for (int k=0;k<(numberOfPoints);k++)
      {
        vtkDataArray *inputScalars = volumeData->GetPointData()->GetScalars();       
        double value = inputScalars->GetTuple1(k + i*numberOfPoints);

        if((!m_UseDoubleThreshold && value >= localThreshold) || (value>= localThreshold && value <= localUpperTheshold))
          newScalars->SetTuple1(k + i*numberOfPoints,255);
        else    
          newScalars->SetTuple1(k + i*numberOfPoints,0);
      }
    }
  }


  if (volumeData->IsA("vtkImageData"))
  {
    vtkMAFSmartPointer<vtkImageData> newSP;
    newSP->CopyStructure(vtkImageData::SafeDownCast(volumeData));
    newSP->GetPointData()->AddArray(newScalars);
    newSP->GetPointData()->SetActiveScalars("SCALARS");
    //newSP->SetScalarTypeToShort();

    m_AutomaticSP->DeepCopy(newSP);
    m_SP->DeepCopy(newSP); 
  }
  else
  {
    vtkMAFSmartPointer<vtkRectilinearGrid> newRG;
    newRG->CopyStructure(vtkRectilinearGrid::SafeDownCast(volumeData));
    newRG->GetPointData()->AddArray(newScalars);
    newRG->GetPointData()->SetActiveScalars("SCALARS");

    double sr[2];
    int n;
    vtkDataArray *x = newRG->GetXCoordinates();
    x->GetRange(sr);
    n = x->GetNumberOfTuples();

    m_AutomaticRG->DeepCopy(newRG);
    m_RG->DeepCopy(newRG);
  }
}
//------------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolume::ApplyRefinementSegmentation()
//------------------------------------------------------------------------------
{
  if (m_RefinementVolumeMask == NULL)
  {
    return;
  }

  m_Volume->GetOutput()->Update();
  vtkDataSet *volumeData = m_Volume->GetOutput()->GetVTKData();
	
	m_RefinementVolumeMask->GetOutput()->Update();
  vtkDataSet *maskVolumeData = m_RefinementVolumeMask->GetOutput()->GetVTKData();

  if (maskVolumeData==NULL || maskVolumeData->GetNumberOfPoints() != volumeData->GetNumberOfPoints() || maskVolumeData->GetPointData()->GetScalars()==NULL)
  {
    mafLogMessage("Error! Wrong refinement volume mask");
    return;
  }

  vtkDataArray *maskScalar = maskVolumeData->GetPointData()->GetScalars();
  vtkDataArray *manualScalar;

  if(maskVolumeData->IsA("vtkRectilinearGrid")) 
  {
    if (m_ManualRG == NULL)
    {
      return;
    }

    if (m_ManualRG->GetPointData()->GetScalars() == NULL)
    {
      return;
    }
    if (m_ManualRG->GetPointData()->GetScalars()->GetNumberOfTuples() != maskScalar->GetNumberOfTuples())
    {
      return;
    }
    manualScalar = m_ManualRG->GetPointData()->GetScalars();
  }
  else if (maskVolumeData->IsA("vtkImageData"))
  {
    if (m_ManualSP == NULL)
    {
      return;
    }

    if (m_ManualSP->GetPointData()->GetScalars() == NULL)
    {
      return;
    }
    if (m_ManualSP->GetPointData()->GetScalars()->GetNumberOfTuples() != maskScalar->GetNumberOfTuples())
    {
      return;
    }
    manualScalar = m_ManualSP->GetPointData()->GetScalars();
  }

  vtkMAFSmartPointer<vtkUnsignedCharArray> newScalars;
  newScalars->SetName("SCALARS");
  newScalars->SetNumberOfTuples(maskScalar->GetNumberOfTuples());

	mafProgressBarHelper progressHelper(this->GetVME());
	progressHelper.InitProgressBar("",false);

  int step = ceil((double)maskScalar->GetNumberOfTuples()/100);
  double invStep = (double)1/step;
  for (int i=0;i<maskScalar->GetNumberOfTuples();i++)
  {
    progressHelper.UpdateProgressBar(i*100/maskScalar->GetNumberOfTuples());

    if (maskScalar->GetTuple1(i) == 0)
    {
      double value = manualScalar->GetTuple1(i);
      newScalars->SetTuple1(i,value);
    }
    if (maskScalar->GetTuple1(i) == 255)
    {
      double value = manualScalar->GetTuple1(i);
      newScalars->SetTuple1(i,255-value);
    }
  }

  if (volumeData->IsA("vtkImageData"))
  {
    vtkMAFSmartPointer<vtkImageData> newSP;
    newSP->CopyStructure(vtkImageData::SafeDownCast(volumeData));
    newSP->GetPointData()->AddArray(newScalars);
    newSP->GetPointData()->SetActiveScalars("SCALARS");

    m_RefinementSP->DeepCopy(newSP);
    m_SP->DeepCopy(newSP);
  }
  else
  {
    vtkMAFSmartPointer<vtkRectilinearGrid> newRG;
    newRG->CopyStructure(vtkRectilinearGrid::SafeDownCast(volumeData));
    newRG->GetPointData()->AddArray(newScalars);
    newRG->GetPointData()->SetActiveScalars("SCALARS");

    m_RefinementRG->DeepCopy(newRG);
    m_RG->DeepCopy(newRG);
  }

}
//------------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolume::ApplyRegionGrowingSegmentation()
//------------------------------------------------------------------------------
{
	mafProgressBarHelper progressHelper(this->GetVME());
	progressHelper.InitProgressBar("",false);

  typedef itk::ConnectedThresholdImageFilter<RealImage, RealImage> ITKConnectedThresholdFilter;
  ITKConnectedThresholdFilter::Pointer connectedThreshold = ITKConnectedThresholdFilter::New();

  m_Volume->GetOutput()->Update();
  vtkDataSet *volumeData = m_Volume->GetOutput()->GetVTKData();
  
  if (m_RegionGrowingSeeds.size()==0)
  {
    if (volumeData->IsA("vtkImageData"))
    {
      m_RegionGrowingSP->DeepCopy(m_AutomaticSP);
      m_SP->DeepCopy(m_RegionGrowingSP);
    }
    else if (volumeData->IsA("vtkRectilinearGrid"))
    {
      m_RegionGrowingRG->DeepCopy(m_AutomaticRG);
      m_RG->DeepCopy(m_RegionGrowingRG);
    }
     
    return;
  }

  progressHelper.UpdateProgressBar(5);

  vtkMAFSmartPointer<vtkImageData> spInputOfRegionGrowing;
  vtkDataSet *automaticData;
  if (volumeData->IsA("vtkImageData"))
  {
    spInputOfRegionGrowing->DeepCopy(vtkImageData::SafeDownCast(volumeData));

    automaticData = m_AutomaticSP;
  }
  else if (volumeData->IsA("vtkRectilinearGrid"))
  {
    int dim[3];
    vtkRectilinearGrid::SafeDownCast(volumeData)->GetDimensions(dim);
    vtkMAFSmartPointer<vtkDoubleArray> oldScalars;
    oldScalars->SetNumberOfTuples(volumeData->GetNumberOfPoints());
    oldScalars->SetName("SCALARS");
    for (int k=0;k<volumeData->GetNumberOfPoints();k++)
    {
      oldScalars->SetTuple1(k,volumeData->GetPointData()->GetScalars()->GetTuple1(k));
    }

    spInputOfRegionGrowing->SetDimensions(dim);
    spInputOfRegionGrowing->SetSpacing(1.0,1.0,1.0);
    spInputOfRegionGrowing->GetPointData()->AddArray(oldScalars);
    spInputOfRegionGrowing->GetPointData()->SetActiveScalars("SCALARS");

    automaticData = m_AutomaticRG;
    
  }

  progressHelper.UpdateProgressBar(10);

  vtkMAFSmartPointer<vtkImageCast> vtkImageToFloat;
  vtkImageToFloat->SetOutputScalarTypeToDouble();
  vtkImageToFloat->SetInputData(spInputOfRegionGrowing);
  vtkImageToFloat->Modified();
  vtkImageToFloat->Update();

  typedef itk::VTKImageToImageFilter< RealImage > ConvertervtkTOitk;
  ConvertervtkTOitk::Pointer vtkTOitk = ConvertervtkTOitk::New();
  vtkTOitk->SetInput( vtkImageToFloat->GetOutput() );
  vtkTOitk->Update();

  connectedThreshold->SetLower(m_RegionGrowingLowerThreshold);
  connectedThreshold->SetUpper(m_RegionGrowingUpperThreshold);
  connectedThreshold->SetReplaceValue(255);

  RealImage::IndexType seed;
  for (int iSeeds=0;iSeeds<m_RegionGrowingSeeds.size();iSeeds++)
  {
	  for (int i=0;i<3;i++)
	  {
	    seed[i]=m_RegionGrowingSeeds[iSeeds][i];
	  }
    connectedThreshold->AddSeed(seed);
  }

  connectedThreshold->SetInput( ((RealImage*)vtkTOitk->GetOutput()) );

  progressHelper.UpdateProgressBar(15);

  try
  {
    connectedThreshold->Update();
  }
  catch ( itk::ExceptionObject &err )
  {
    std::cout << "ExceptionObject caught !" << std::endl; 
    std::cout << err << std::endl; 
  }

  typedef itk::ImageToVTKImageFilter< RealImage > ConverteritkTOvtk;
  ConverteritkTOvtk::Pointer itkTOvtk = ConverteritkTOvtk::New();
  itkTOvtk->SetInput( connectedThreshold->GetOutput() );
  itkTOvtk->Update();

  vtkImageData *spOutputRegionGrowing = ((vtkImageData*)itkTOvtk->GetOutput());

  progressHelper.UpdateProgressBar(65);


  //////////////////////////////////////////////////////////////////////////
  //Perform the OR operation between region growing and automatic output
  //////////////////////////////////////////////////////////////////////////


  int volumeDimensions[3];
  if(volumeData->IsA("vtkRectilinearGrid")) {
    vtkRectilinearGrid *rectilinearGrid=vtkRectilinearGrid::SafeDownCast(volumeData);
    rectilinearGrid->GetDimensions(volumeDimensions);
  }
  else if (volumeData->IsA("vtkImageData")) {
    vtkImageData *imageData=vtkImageData::SafeDownCast(volumeData);
    imageData->GetDimensions(volumeDimensions);
  }

  vtkMAFSmartPointer<vtkUnsignedCharArray> newScalars;
  newScalars->SetNumberOfTuples(volumeData->GetNumberOfPoints());
  newScalars->SetName("SCALARS");
  newScalars->SetNumberOfTuples(volumeDimensions[0]*volumeDimensions[1]*volumeDimensions[2]);

  int numberOfPoints = volumeDimensions[0] * volumeDimensions[1];
  
  for (int i=0;i<volumeDimensions[2];i++)
  {

    for (int k=0;k<numberOfPoints;k++)
    {
      int index = k+i*(volumeDimensions[0]*volumeDimensions[1]);
      double automaticValue = automaticData->GetPointData()->GetScalars()->GetTuple1(index);
      double regionGrowingValue = spOutputRegionGrowing->GetPointData()->GetScalars()->GetTuple1(index);

      if(i>m_RegionGrowingEndSlice || i<m_RegionGrowingStartSlice)
      {
        newScalars->SetTuple1(k+i*(volumeDimensions[0]*volumeDimensions[1]),automaticValue);
      }
      else
      {
        if (automaticValue == 255 || regionGrowingValue == 255)
        {
          newScalars->SetTuple1(index,255);
        }
        else
        {
          newScalars->SetTuple1(index,0);
        }

      }
      
    }
  }




 /* for (int i=0;i<volumeData->GetNumberOfPoints();i++)
  {
    if (automaticData->GetPointData()->GetScalars()->GetTuple1(i) == 255 || spOutputRegionGrowing->GetPointData()->GetScalars()->GetTuple1(i) == 255)
    {
      newScalars->SetTuple1(i,255);
    }
    else
    {
      newScalars->SetTuple1(i,0);
    }
  }*/
  //////////////////////////////////////////////////////////////////////////

  progressHelper.UpdateProgressBar(85);

  if (volumeData->IsA("vtkImageData"))
  {
    m_RegionGrowingSP->CopyStructure(vtkImageData::SafeDownCast(volumeData));
    m_RegionGrowingSP->GetPointData()->AddArray(newScalars);
    m_RegionGrowingSP->GetPointData()->SetActiveScalars("SCALARS");
    
    m_SP->DeepCopy(m_RegionGrowingSP);
  }
  else if (volumeData->IsA("vtkRectilinearGrid"))
  {
    m_RegionGrowingRG->CopyStructure(vtkRectilinearGrid::SafeDownCast(volumeData));
    m_RegionGrowingRG->GetPointData()->AddArray(newScalars);
    m_RegionGrowingRG->GetPointData()->SetActiveScalars("SCALARS");

    m_RG->DeepCopy(m_RegionGrowingRG);
  }
}
//------------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolume::PreExecute()
//------------------------------------------------------------------------------
{
  if(m_Volume)
  {
    m_Volume->GetOutput()->Update();
    vtkDataSet *volumeData = m_Volume->GetOutput()->GetVTKData();
    if(volumeData)
    {
      if (m_ChangedAutomaticData)
      {
        ApplyAutomaticSegmentation();
      }
      if ((m_ChangedAutomaticData ) || m_ChangedRegionGrowingData)
      {
        ApplyRegionGrowingSegmentation();
      }
      if ((m_ChangedAutomaticData) || m_ChangedRegionGrowingData || m_ChangedManualData)
      {
        ApplyManualSegmentation();
      }
      if ((m_ChangedAutomaticData) || m_ChangedRegionGrowingData || m_ChangedManualData || m_ChangedRefinementData)
      {
        ApplyRefinementSegmentation();
      }

      m_ChangedAutomaticData = false;
      m_ChangedManualData = false;
      m_ChangedRefinementData = false;
      m_ChangedRegionGrowingData = false;
    }
  }
  else
  {
    SetInput(NULL);
  }
}
//------------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolume::Execute()
//------------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
int mafDataPipeCustomSegmentationVolume::AddRange(int startSlice,int endSlice,double threshold, double upperThreshold)
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
  m_AutomaticSegmentationUpperThresholds.push_back(upperThreshold);

  m_ChangedAutomaticData = true;
  Modified();
  return MAF_OK;
}
//----------------------------------------------------------------------------
int mafDataPipeCustomSegmentationVolume::GetRange(int index,int &startSlice, int &endSlice, double &threshold)
//----------------------------------------------------------------------------
{
  if (index<0 || index+1>m_AutomaticSegmentationRanges.size())
  {
    return MAF_ERROR;
  }

  startSlice = m_AutomaticSegmentationRanges[index][0];
  endSlice = m_AutomaticSegmentationRanges[index][1];
  threshold = m_AutomaticSegmentationThresholds[index];

  return MAF_OK;
}

//----------------------------------------------------------------------------
int mafDataPipeCustomSegmentationVolume::GetRange(int index,int &startSlice, int &endSlice, double &threshold, double &upperThreshold)
//----------------------------------------------------------------------------
{
  if (index<0 || index+1>m_AutomaticSegmentationRanges.size())
  {
    return MAF_ERROR;
  }

  startSlice = m_AutomaticSegmentationRanges[index][0];
  endSlice = m_AutomaticSegmentationRanges[index][1];
  threshold = m_AutomaticSegmentationThresholds[index];
  upperThreshold = m_AutomaticSegmentationUpperThresholds[index];

  return MAF_OK;
}


//------------------------------------------------------------------------------
int mafDataPipeCustomSegmentationVolume::SetVolume(mafVME *volume)
//------------------------------------------------------------------------------
{
  m_Volume = volume;
  
  if (m_Volume == NULL || !m_Volume->GetOutput()->GetVTKData() || 
    ( !m_Volume->GetOutput()->GetVTKData()->IsA("vtkImageData")
    && !m_Volume->GetOutput()->GetVTKData()->IsA("vtkRectilinearGrid") )
    )
  {
    return MAF_ERROR;
  }

  if (m_Volume->GetOutput()->GetVTKData() && m_Volume->GetOutput()->GetVTKData()->IsA("vtkRectilinearGrid"))
  {
    SetInput(m_RG);
    Modified();
  }
  else if (m_Volume->GetOutput()->GetVTKData() && m_Volume->GetOutput()->GetVTKData()->IsA("vtkImageData"))
  {
    SetInput(m_SP);
    Modified();
  }

  m_ChangedAutomaticData = true;

  return MAF_OK;
}
//------------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolume::SetManualVolumeMask(mafVME *volume)
//------------------------------------------------------------------------------
{
  m_ManualVolumeMask = volume;
  
  m_ChangedManualData = true;
  Modified();
}
//------------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolume::SetRefinementVolumeMask(mafVME *volume)
//------------------------------------------------------------------------------
{
  m_RefinementVolumeMask = volume;

  m_ChangedRefinementData = true;
  Modified();
}
//------------------------------------------------------------------------------
vtkDataSet *mafDataPipeCustomSegmentationVolume::GetAutomaticOutput()
//------------------------------------------------------------------------------
{
  if(m_Volume)
  {
    m_Volume->GetOutput()->Update();
    vtkDataSet *volumeData = m_Volume->GetOutput()->GetVTKData();
    if(volumeData)
    {
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

  return NULL;
}
//------------------------------------------------------------------------------
vtkDataSet *mafDataPipeCustomSegmentationVolume::GetManualOutput()
//------------------------------------------------------------------------------
{
  if(m_Volume)
  {
    m_Volume->GetOutput()->Update();
    vtkDataSet *volumeData = m_Volume->GetOutput()->GetVTKData();
    if(volumeData)
    {
      if(volumeData->IsA("vtkRectilinearGrid"))
      {
        return m_ManualRG;
      }
      else if (volumeData->IsA("vtkImageData"))
      {
        return m_ManualSP;
      }
    }
  }

  return NULL;
}
//------------------------------------------------------------------------------
vtkDataSet *mafDataPipeCustomSegmentationVolume::GetRefinementOutput()
//------------------------------------------------------------------------------
{
  if(m_Volume)
  {
    m_Volume->GetOutput()->Update();
    vtkDataSet *volumeData = m_Volume->GetOutput()->GetVTKData();
    if(volumeData)
    {
      if(volumeData->IsA("vtkRectilinearGrid"))
      {
        return m_RefinementRG;
      }
      else if (volumeData->IsA("vtkImageData"))
      {
        return m_RefinementSP;
      }
    }
  }

  return NULL;
}
//------------------------------------------------------------------------------
vtkDataSet *mafDataPipeCustomSegmentationVolume::GetRegionGrowingOutput()
//------------------------------------------------------------------------------
{
  if(m_Volume)
  {
    m_Volume->GetOutput()->Update();
    vtkDataSet *volumeData = m_Volume->GetOutput()->GetVTKData();
    if(volumeData)
    {
      if(volumeData->IsA("vtkRectilinearGrid"))
      {
        return m_RegionGrowingRG;
      }
      else if (volumeData->IsA("vtkImageData"))
      {
        return m_RegionGrowingSP;
      }
    }
  }

  return NULL;
}
//----------------------------------------------------------------------------
bool mafDataPipeCustomSegmentationVolume::CheckNumberOfThresholds()
//----------------------------------------------------------------------------
{
  //////////////////////////////////////////////////////////////////////////
  //Check that all slices have a threshold

  if (m_AutomaticSegmentationThresholdModality == mafVMESegmentationVolume::GLOBAL)
  {
    return true;
  }

  if(m_Volume)
  {
    m_Volume->GetOutput()->Update();
    vtkDataSet *volumeData = m_Volume->GetOutput()->GetVTKData();
    if(volumeData)
    {
      int volumeDimensions[3];
      if(volumeData->IsA("vtkRectilinearGrid")) {
        vtkRectilinearGrid *rectilinearGrid=vtkRectilinearGrid::SafeDownCast(volumeData);
        rectilinearGrid->GetDimensions(volumeDimensions);
      }
      else if (volumeData->IsA("vtkImageData")) {
        vtkImageData *sp=vtkImageData::SafeDownCast(volumeData);
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
          mafLogMessage("Slice %d� hasn't a threshold",i+1);
          return false;
        }
      }

      return true;
    }
  }
  	
  return false;  
  //////////////////////////////////////////////////////////////////////////
}
//----------------------------------------------------------------------------
int mafDataPipeCustomSegmentationVolume::UpdateRange(int index,int startSlice, int endSlice, double threshold, double upperThreshold)
//----------------------------------------------------------------------------
{
  if (m_AutomaticSegmentationRanges.size()==0 || index<0 || index+1>m_AutomaticSegmentationRanges.size() || AutomaticCheckRange(startSlice,endSlice,index) == MAF_ERROR)
  {
    return MAF_ERROR;
  }
  m_AutomaticSegmentationRanges[index][0] = startSlice;
  m_AutomaticSegmentationRanges[index][1] = endSlice;
  m_AutomaticSegmentationThresholds[index] = threshold;
  m_AutomaticSegmentationUpperThresholds[index] = upperThreshold;


  m_ChangedAutomaticData = true;
  Modified();
  return MAF_OK;
}
//----------------------------------------------------------------------------
int mafDataPipeCustomSegmentationVolume::RemoveAllRanges()
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_AutomaticSegmentationRanges.size();i++)
  {
    delete []m_AutomaticSegmentationRanges[i];
  }
  m_AutomaticSegmentationRanges.clear();
  m_AutomaticSegmentationThresholds.clear();
  m_AutomaticSegmentationUpperThresholds.clear();

  m_ChangedAutomaticData = true;
  Modified();

  return MAF_OK;
}
//----------------------------------------------------------------------------
int mafDataPipeCustomSegmentationVolume::RemoveAllSeeds()
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_RegionGrowingSeeds.size();i++)
  {
    delete []m_RegionGrowingSeeds[i];
  }
  m_RegionGrowingSeeds.clear();

  m_ChangedRegionGrowingData = true;
  Modified();

  return MAF_OK;
}
//----------------------------------------------------------------------------
int mafDataPipeCustomSegmentationVolume::DeleteSeed(int index)
//----------------------------------------------------------------------------
{
  if (m_RegionGrowingSeeds.size()==0 || index<0 || index+1>(m_RegionGrowingSeeds.size()))
  {
    return MAF_ERROR;
  }

  for (int i=0,j=0;i<m_RegionGrowingSeeds.size();i++)
  {
    if (i != index)
    {
      m_RegionGrowingSeeds[j][0] = m_RegionGrowingSeeds[i][0];
      m_RegionGrowingSeeds[j][1] = m_RegionGrowingSeeds[i][1];
      m_RegionGrowingSeeds[j][2] = m_RegionGrowingSeeds[i][2];
      j++;
    }
  }

  delete []m_RegionGrowingSeeds[m_RegionGrowingSeeds.size()-1];
  m_RegionGrowingSeeds.pop_back();

  m_ChangedRegionGrowingData = true;
  Modified();
  return MAF_OK;
}
//----------------------------------------------------------------------------
int mafDataPipeCustomSegmentationVolume::DeleteRange(int index)
//----------------------------------------------------------------------------
{
  if (m_AutomaticSegmentationRanges.size()==0 || index<0 || index+1>m_AutomaticSegmentationRanges.size())
  {
    return MAF_ERROR;
  }

  for (int i=0,j=0;i<m_AutomaticSegmentationRanges.size();i++)
  {
    if (i != index)
    {
      m_AutomaticSegmentationRanges[j][0] = m_AutomaticSegmentationRanges[i][0];
      m_AutomaticSegmentationRanges[j][1] = m_AutomaticSegmentationRanges[i][1];
      m_AutomaticSegmentationThresholds[j] = m_AutomaticSegmentationThresholds[i];
      m_AutomaticSegmentationUpperThresholds[j] = m_AutomaticSegmentationUpperThresholds[i];
      j++;
    }
  }

  delete []m_AutomaticSegmentationRanges[m_AutomaticSegmentationRanges.size()-1];
  m_AutomaticSegmentationRanges.pop_back();
  m_AutomaticSegmentationThresholds.pop_back();
  m_AutomaticSegmentationUpperThresholds.pop_back();


  m_ChangedAutomaticData = true;
  Modified();
  return MAF_OK;
}
//------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolume::SetAutomaticSegmentationGlobalThreshold(double threshold, double upperThreshold)
//------------------------------------------------------------------------
{
  m_AutomaticSegmentationGlobalThreshold = threshold;
  m_AutomaticSegmentationGlobalUpperThreshold = upperThreshold;
  m_ChangedAutomaticData = true;
  Modified();
}
//------------------------------------------------------------------------
int mafDataPipeCustomSegmentationVolume::AddSeed(int seed[3])
//------------------------------------------------------------------------
{
  int *newSeed = new int[3];
  newSeed[0] = seed[0];
  newSeed[1] = seed[1];
  newSeed[2] = seed[2];

  m_RegionGrowingSeeds.push_back(newSeed);

  m_ChangedRegionGrowingData = true;
  Modified();

  return MAF_OK;
}
//----------------------------------------------------------------------------
int mafDataPipeCustomSegmentationVolume::GetSeed(int index,int seed[3])
//----------------------------------------------------------------------------
{
 
  if (index<0 || index+1>m_RegionGrowingSeeds.size())
  {
    return MAF_ERROR;
  }
  seed[0] = m_RegionGrowingSeeds[index][0];
  seed[1] = m_RegionGrowingSeeds[index][1];
  seed[2] = m_RegionGrowingSeeds[index][2];

  return MAF_OK;
}
//------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolume::SetRegionGrowingLowerThreshold(double value)
//------------------------------------------------------------------------
{
  m_RegionGrowingLowerThreshold = value;

  m_ChangedRegionGrowingData = true;
  Modified();
}
//------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolume::SetRegionGrowingUpperThreshold(double value)
//------------------------------------------------------------------------
{
  m_RegionGrowingUpperThreshold = value;

  m_ChangedRegionGrowingData = true;
  Modified();
}
//------------------------------------------------------------------------
int mafDataPipeCustomSegmentationVolume::AutomaticCheckRange(int startSlice,int endSlice,int indexToExclude)
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

//------------------------------------------------------------------------
void mafDataPipeCustomSegmentationVolume::SetRegionGrowingSliceRange(int startSlice, int endSlice)
//------------------------------------------------------------------------
{
  m_RegionGrowingStartSlice = startSlice;
  m_RegionGrowingEndSlice = endSlice;
}

//------------------------------------------------------------------------
int mafDataPipeCustomSegmentationVolume::GetRegionGrowingSliceRange(int &startSlice, int &endSlice)
//------------------------------------------------------------------------
{
  startSlice = m_RegionGrowingStartSlice;
  endSlice = m_RegionGrowingEndSlice;

  return MAF_OK;
}