/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkBSplineInterpolateImageFunctionTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <iostream>

#include "itkImage.h"
#include "itkSize.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkImageRegionIterator.h"


#include "vnl/vnl_math.h"

  typedef double InputPixelType;
  typedef double CoordRepType;

// Set up for 1D Images
  enum { ImageDimension1D = 1 };

  typedef itk::Image< InputPixelType, ImageDimension1D > ImageType1D;
  typedef ImageType1D::Pointer ImageTypePtr1D;
  typedef ImageType1D::SizeType SizeType1D;
  typedef itk::BSplineInterpolateImageFunction<ImageType1D,CoordRepType> InterpolatorType1D;
  typedef InterpolatorType1D::IndexType                 IndexType1D;
  typedef InterpolatorType1D::PointType                 PointType1D;
  typedef InterpolatorType1D::ContinuousIndexType       ContinuousIndexType1D;

  void set1DData(ImageType1D::Pointer);

  // Set up for 2D Images
  enum { ImageDimension2D = 2 };

  typedef itk::Image< InputPixelType, ImageDimension2D > ImageType2D;
  typedef ImageType2D::Pointer ImageTypePtr2D;
  typedef ImageType2D::SizeType SizeType2D;
  typedef itk::BSplineInterpolateImageFunction<ImageType2D,CoordRepType> InterpolatorType2D;
  typedef InterpolatorType2D::IndexType                 IndexType2D;
  typedef InterpolatorType2D::PointType                 PointType2D;
  typedef InterpolatorType2D::ContinuousIndexType       ContinuousIndexType2D;

  void set2DData(ImageType2D::Pointer);

  // Set up for 3D Images
  enum { ImageDimension3D = 3 };

  typedef itk::Image< InputPixelType, ImageDimension3D > ImageType3D;
  typedef ImageType3D::Pointer ImageTypePtr3D;
  typedef ImageType3D::SizeType SizeType3D;
  typedef itk::BSplineInterpolateImageFunction<ImageType3D,CoordRepType> InterpolatorType3D;
  typedef InterpolatorType3D::IndexType                 IndexType3D;
  typedef InterpolatorType3D::PointType                 PointType3D;
  typedef InterpolatorType3D::ContinuousIndexType       ContinuousIndexType3D;

  void set3DData(ImageType3D::Pointer);

/**
 * Test a geometric point. Returns true if test has passed,
 * returns false otherwise
 */
template <class TInterpolator, class PointType>
bool TestGeometricPoint(
const TInterpolator * interp,
const PointType& point,
bool isInside,
double trueValue )
{

  std::cout << " Point: " << point;

  bool bvalue = interp->IsInsideBuffer( point );
  std::cout << " Inside: " << bvalue << " ";

  if( bvalue != isInside )
    {
    std::cout << "*** Error: inside should be " << isInside << std::endl;
    return false;
    }

  if( isInside )
    {
    double value = interp->Evaluate( point );
    printf(" %.9f\n",value);
    std::cout << " Value: " << value;

    if( vnl_math_abs( value - trueValue ) > 1e-9 )
      {
      std::cout << "*** Error: value should be " << trueValue << std::endl;
      return false;
      }
    }

  std::cout << std::endl;
  return true;

}

/**
 * Test a continuous index. Returns true if test has passed,
 * returns false otherwise
 */
template<class TInterpolator, class ContinuousIndexType>
bool TestContinuousIndex(
const TInterpolator * interp,
const ContinuousIndexType& index,
bool isInside,
double trueValue )
{

  std::cout << " Index: " << index;

  bool bvalue = interp->IsInsideBuffer( index );
  std::cout << " Inside: " << bvalue;

  if( bvalue != isInside )
    {
    std::cout << "*** Error: inside should be " << isInside << std::endl;
    return false;
    }

  if( isInside )
    {
    double value = interp->EvaluateAtContinuousIndex( index );
    std::cout << " Value: " << value;

    if( vnl_math_abs( value - trueValue ) > 1e-4 )
      {
      std::cout << "*** Error: value should be " << trueValue << std::endl;
      return false;
      }
    }

  std::cout << std::endl;
  return true;

}


// Run a series of tests to validate the 1D 
// cubic spline implementation.
int test1DCubicSpline()
{
  int flag = 0;

  // Allocate a simple test image 
  ImageTypePtr1D image = ImageType1D::New();

  set1DData(image);

  // Set origin and spacing of physical coordinates 
  double origin [] = { 0.5  };
  double spacing[] = { 0.1  };
  image->SetOrigin(origin);
  image->SetSpacing(spacing);

  // Create and initialize the interpolator 
  InterpolatorType1D::Pointer interp = InterpolatorType1D::New();
// interp->SetSplineOrder(1);
  interp->SetInputImage(image);
  interp->Print( std::cout );

  // Test evaluation at continuous indices and corresponding
  //gemetric points 
  std::cout << "Testing 1D Cubic B-Spline:\n";
  std::cout << "Evaluate at: " << std::endl;
  ContinuousIndexType1D cindex;
  PointType1D point;
  bool passed;

  // These values test 1) near border, 
  //    2) inside
  //    3) integer value
  //    4) outside image
#define NPOINTS 4  // number of points 
  double darray1[NPOINTS] = {1.4, 8.9, 10.0, 40.0};
  double truth[NPOINTS] = {334.41265437584, 18.158173426944, 4.0000, 0};
  bool b_Inside[NPOINTS] = {true, true, true, false};

  // an integer position inside the image
  for (int ii=0; ii < NPOINTS; ii++)
    {
  
    cindex = ContinuousIndexType1D(&darray1[ii]);
    passed = TestContinuousIndex<InterpolatorType1D, ContinuousIndexType1D >( interp, cindex, b_Inside[ii], truth[ii] );
  
    if( !passed ) flag += 1;
  
    interp->ConvertContinuousIndexToPoint( cindex, point );
    passed = TestGeometricPoint<InterpolatorType1D, PointType1D>( interp, point, b_Inside[ii], truth[ii]  );
  
    if( !passed ) flag += 1;
    }

  return (flag);
}


int test2DSpline()
{
  int flag = 0;

  /* Allocate a simple test image */
  ImageTypePtr2D image = ImageType2D::New();

  set2DData(image);

  /* Set origin and spacing of physical coordinates */
  double origin [] = { 0.5, 1.0 };
  double spacing[] = { 0.1, 0.5  };
  image->SetOrigin(origin);
  image->SetSpacing(spacing);

  /* Create and initialize the interpolator */
  for (int splineOrder = 0; splineOrder<=5; splineOrder++)
    {
    InterpolatorType2D::Pointer interp = InterpolatorType2D::New();
    interp->SetSplineOrder(splineOrder);

    interp->SetInputImage(image);
    interp->Print( std::cout );

    /* Test evaluation at continuous indices and corresponding
    gemetric points */
    std::cout << "Testing 2D B-Spline of Order "<< splineOrder << ":\n";
    std::cout << "Evaluate at: " << std::endl;
    ContinuousIndexType2D cindex;
    PointType2D point;
    bool passed;

    // These values test 1) near border, 
    //    2) inside
    //    3) integer value
    //    4) outside image
#define NPOINTS2 4  // number of points 

      double darray1[NPOINTS2][2] = {{0.1, 0.2}, {3.4, 5.8}, {4.0, 6.0}, { 2.1, 8.0}};
      double truth[NPOINTS2][6] = {{154.5, 140.14, 151.86429192392, 151.650316034, 151.865916515, 151.882483111},
        { 0, 13.84, 22.688125812495, 22.411473093, 22.606968306, 22.908345604},
        { 36.2, 36.2, 36.2, 36.2, 36.2, 36.2 },
        {0, 0, 0,0,0,0}};
    bool b_Inside[NPOINTS2] = {true, true, true, false};
   // double darray1[2];

    // an integer position inside the image
    for (int ii=0; ii < NPOINTS2; ii++)
      {
     // darray1[0] = darray[ii][0];
     // darray1[1] = darray[ii][1];
      cindex = ContinuousIndexType2D(&darray1[ii][0]);
      passed = TestContinuousIndex<InterpolatorType2D, ContinuousIndexType2D >( interp, cindex, b_Inside[ii], truth[ii][splineOrder] );
  
      if( !passed ) flag += 1;
  
      interp->ConvertContinuousIndexToPoint( cindex, point );
      passed = TestGeometricPoint<InterpolatorType2D, PointType2D>( interp, point, b_Inside[ii], truth[ii][splineOrder ]  );
  
      if( !passed ) flag += 1;
      }
    }  // end of splineOrder

  return (flag);
}

int test3DSpline()
{
  int flag = 0;

  /* Allocate a simple test image */
  ImageTypePtr3D image = ImageType3D::New();

  set3DData(image);

  /* Set origin and spacing of physical coordinates */
  double origin [] = { 0.5, 1.0, 1.333};
  double spacing[] = { 0.1, 0.5, 0.75  };
  image->SetOrigin(origin);
  image->SetSpacing(spacing);

  /* Create and initialize the interpolator */
  for (int splineOrder = 2; splineOrder<=5; splineOrder++)
    {
    InterpolatorType3D::Pointer interp = InterpolatorType3D::New();
    interp->SetSplineOrder(splineOrder);
    interp->SetInputImage(image);
    interp->Print( std::cout );

    /* Test evaluation at continuous indices and corresponding
    gemetric points */
    std::cout << "Testing 3D B-Spline of Order "<< splineOrder << ":\n";
    std::cout << "Evaluate at: " << std::endl;
    ContinuousIndexType3D cindex;
    PointType3D point;
    bool passed;

    // These values test 
    //    1) near border, 
    //    2) inside
    //    3) integer value
    //    4) outside image
#define NPOINTS3 4  // number of points 

      double darray1[NPOINTS3][ImageDimension3D] = {{0.1, 20.1, 28.4}, {21.58, 34.5, 17.2 }, {10, 20, 12}, { 15, 20.2, 31}};
      double truth[NPOINTS3][4] = {{48.621593795, 48.651173138, 48.656914878, 48.662256571},
        { 73.280126903, 73.280816965, 73.282780615, 73.285315943},
        { 42.0, 42.0, 42.0, 42.0},
        {0,0,0,0}};
    bool b_Inside[NPOINTS3] = {true, true, true, false};
   // double darray1[2];

    // an integer position inside the image
    for (int ii=0; ii < NPOINTS3; ii++)
      {
     // darray1[0] = darray[ii][0];
     // darray1[1] = darray[ii][1];
      cindex = ContinuousIndexType3D(&darray1[ii][0]);
      passed = TestContinuousIndex<InterpolatorType3D, ContinuousIndexType3D >( interp, cindex, b_Inside[ii], truth[ii][splineOrder -2] );
  
      if( !passed ) flag += 1;
  
      interp->ConvertContinuousIndexToPoint( cindex, point );
      passed = TestGeometricPoint<InterpolatorType3D, PointType3D>( interp, point, b_Inside[ii], truth[ii][splineOrder -2]  );
  
      if( !passed ) flag += 1;
      }
    }  // end of splineOrder

  return (flag);
}

int 
main(
    int argc,
    char *argv[])
{
  int flag = 0;           /* Did this test program work? */

  std::cout << "Testing B Spline interpolation methods:\n";

  flag += test1DCubicSpline();

  flag += test2DSpline();

  flag += test3DSpline();




  /* Return results of test */
  if (flag != 0) {
    std::cout << "*** " << flag << " tests failed" << std::endl;
  
    return 1; }
  else {
    std::cout << "All tests successfully passed" << std::endl;
    return 0; }

}

void set1DData(ImageType1D::Pointer imgPtr)
{

  SizeType1D size = {36};
  double mydata[36] = {454.0000,  369.4000,  295.2000,  230.8000,  175.6000,  129.0000,   90.4000, 59.2000,   34.8000,   16.6000,    4.0000,   -3.6000,   -6.8000,   -6.2000,
    -2.4000,    4.0000,   12.4000,   22.2000,   32.8000,   43.6000,   54.0000, 63.4000,   71.2000,   76.8000,   79.6000,   79.0000,   74.4000,   65.2000,
    50.8000,   30.6000,    4.0000,  -29.6000,  -70.8000, -120.2000, -178.4000, -246.0000 };

  ImageType1D::RegionType region;
  region.SetSize( size );

  imgPtr->SetLargestPossibleRegion( region );
  imgPtr->SetBufferedRegion( region );
  imgPtr->Allocate();

  unsigned int numPixels = region.GetNumberOfPixels(); 

  typedef itk::ImageRegionIterator<ImageType1D>    InputIterator;

  InputIterator inIter( imgPtr, region );

  int j = 0;
  while( !inIter.IsAtEnd() )
    {
    inIter.Set(mydata[j]);
    ++inIter;
    ++j;
    }

  
}

void set2DData(ImageType2D::Pointer imgPtr)
{
  SizeType2D size = {7,7};
  double mydata[ 49 ] = {  154.5000,   82.4000,   30.9000,         0,  -10.3000,         0,   30.9000 ,
    117.0000,   62.4000,   23.4000,         0,   -7.8000,         0,   23.4000 ,
   18.0000,    9.6000,    3.6000,         0,   -1.2000,         0,    3.6000 ,
 -120.0000,  -64.0000,  -24.0000,         0,    8.0000,         0,  -24.0000 ,
 -274.5000, -146.4000,  -54.9000,         0,   18.3000,         0,  -54.9000 ,
 -423.0000, -225.6000,  -84.6000,         0,   28.2000,         0,  -84.6000 ,
 -543.0000, -289.6000, -108.6000,         0,   36.2000,         0, -108.6000  };

  ImageType2D::RegionType region;
  region.SetSize( size );

  imgPtr->SetLargestPossibleRegion( region );
  imgPtr->SetBufferedRegion( region );
  imgPtr->Allocate();

  unsigned int numPixels = region.GetNumberOfPixels(); 

  typedef itk::ImageRegionIterator<ImageType2D>  InputIterator;

  InputIterator inIter( imgPtr, region );

  int j = 0;
  while( !inIter.IsAtEnd() )
    {
    inIter.Set(mydata[j]);
    ++inIter;
    ++j;
    }

  
}


void set3DData(ImageType3D::Pointer imgPtr)
{
  SizeType3D size = {80,40,30};

  /* Allocate a simple test image */
  ImageType3D::RegionType region;
  region.SetSize( size );

  imgPtr->SetLargestPossibleRegion( region );
  imgPtr->SetBufferedRegion( region );
  imgPtr->Allocate();


  /* Set origin and spacing of physical coordinates */

  /* Initialize the image contents */
  IndexType3D index;
  for (int slice = 0; slice < size[2]; slice++) {
      index[2] = slice;
      for (int row = 0; row < size[1]; row++) {
          index[1] = row;
          for (int col = 0; col < size[0]; col++) {
              index[0] = col;
              imgPtr->SetPixel(index, slice+row+col);
          }
      }
  }

}

