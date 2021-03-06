/// \file HeatExchanger.h
/// \brief MFT heat exchanger builder
/// \author P. Demongandin, Raphael Tieulent <raphael.tieulent@cern.ch>
/// \date 09/06/2015

#ifndef ALICEO2_MFT_HEATEXCHANGER_H_
#define ALICEO2_MFT_HEATEXCHANGER_H_

#include "TNamed.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"

namespace AliceO2 {
namespace MFT {

class HeatExchanger : public TNamed {

 public:

  HeatExchanger();
  HeatExchanger(Double_t Rwater, Double_t DRPipe, Double_t HeatExchangerThickness, Double_t CarbonThickness);
  
  virtual ~HeatExchanger() {};
  
  TGeoVolumeAssembly* Create(Int_t kHalf, Int_t disk);

  void CreateHalfDisk0(Int_t half);
  void CreateHalfDisk1(Int_t half);
  void CreateHalfDisk2(Int_t half);
  void CreateHalfDisk3(Int_t half);
  void CreateHalfDisk4(Int_t half);
  void CreateManyfold(Int_t disk);

  Double_t GetWaterRadius() { return mRWater; }
  void SetWaterRadius(Double_t &Rwater) { mRWater = Rwater; }

  Double_t GetPipeThickness() { return mDRPipe; }
  void SetPipeThickness(Double_t &DRPipe) { mDRPipe = DRPipe; }

  Double_t GetExchangerWidth() { return mHeatExchangerThickness; }
  void SetExchangerWidth(Double_t &HeatExchangerThickness) { mHeatExchangerThickness = HeatExchangerThickness; }

  Double_t GetCarbonThickness() { return mCarbonThickness; }
  void SetCarbonThickness(Double_t &CarbonThickness) { mCarbonThickness = CarbonThickness; }

  TGeoMaterial *material;
  TGeoMedium *water;
  TGeoMedium *pipe;
  TGeoMedium *carbon;
  TGeoMedium *rohacell;

 private:

  void InitParameters();

  const static Int_t fNMaxDisks;

  enum {kBottom, kTop, kNHalves};

  TGeoVolumeAssembly *mHalfDisk;
  
  TGeoRotation ***mHalfDiskRotation;
  TGeoCombiTrans ***mHalfDiskTransformation;

  Double_t mRWater;  // Radius of the water 
  Double_t mDRPipe;  // Thickness of the pipe
  Double_t mHeatExchangerThickness; //width of the heat exchanger
  Double_t mCarbonThickness; //thickness of carbon plate over 2
  Double_t mHalfDiskGap; //gap between half disks

  Double_t mRohacellThickness;

  //Parameters for carbon and Rohacell
  Int_t mNDisks;  //number of planes
  Int_t fnPart[5]; //number of part of each half-disk
  Double_t mRMin[5]; // radius of the central hole of each disk
  Double_t mZPlan[5]; //position on z axis of each plane

  //Dimensions of carbon and Rohacell planes
  Double_t **mSupportXDimensions;
  Double_t **mSupportYDimensions;

  //Parameters for disk0, disk1 and disk2
  Double_t mLWater; // Length of tube part
  Double_t mXPosition0[3]; //position on x axis of each tube for disk 0, 1 and 2 
  Double_t fangle0; //angle of the sides torus part of each pipe for disk 0, 1 and 2
  Double_t fradius0; // radius of the sides torus part for disk 0, 1 and 2
  Double_t mLpartial0; // length of partial tube part

   //Parameters for disk3 
  Double_t mLWater3[3]; // length of tube part for third plan 
  Double_t mXPosition3[4]; // tube position on x axe of each tube for disk 3
  Double_t fangle3[3]; // angle of sides torus of each pipe for disk 3
  Double_t fradius3[3]; // radius of the sides torus for disk 3
  Double_t fangleThirdPipe3; // angle with x axe of tube part of third pipe
  Double_t mLpartial3[2]; // length of partial tube

  Double_t fradius3fourth[4]; // radius of fourth pipe torus of fourth pipe
  Double_t fangle3fourth[4]; // angle of fourth pipe torus of fourth pipe
  Double_t fbeta3fourth[3]; // shift angle of different torus part of fourth pipe of disk 3

  //Parameters for disk4
  Double_t mLwater4[3]; // length of tube part for fourth plan
  Double_t mXposition4[5]; // tube position on x axe of each tube for disk 4
  Double_t fangle4[6]; // angle of sides torus of each pipe for disk 4
  Double_t fradius4[5]; // radius of the sides torus for disk 4
  Double_t mLpartial4[2]; // length of partial tube for first and second pipe of disk 4
  Double_t fangle4fifth[4]; // angle of torus for fifth pipe of disk 4
  Double_t fradius4fifth[4]; // radius of torus for fifth pipe of disk 4

  /// \cond CLASSIMP
  ClassDef(HeatExchanger, 2);
  /// \endcond

};

}
}

#endif
