/// \file HalfCone.h
/// \brief Class building geometry of one half of one MFT half-cone
/// \author sbest@pucp.pe, eric.endress@gmx.de, franck.manso@clermont.in2p3.fr
/// \date 15/12/2016

#ifndef ALICEO2_MFT_HALFCONE_H_
#define ALICEO2_MFT_HALFCONE_H_

#include "TNamed.h"

class TGeoVolumeAssembly;

namespace AliceO2 {
namespace MFT {

class HalfCone : public TNamed {
  
public:
  
  HalfCone();
  
  virtual ~HalfCone();
  
  TGeoVolumeAssembly* CreateHalfCone(Int_t half);

protected:

  TGeoVolumeAssembly * mHalfCone;

private:
  
  ClassDef(HalfCone,1)
  
};

}
}

#endif

