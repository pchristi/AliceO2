/// \file ChipSegmentation.cxx
/// \brief Description of the virtual segmentation of the chips
/// \author Raphael Tieulent <raphael.tieulent@cern.ch>

#include "FairLogger.h"

#include "MFTBase/Constants.h"
#include "MFTBase/ChipSegmentation.h"
#include "MFTBase/Geometry.h"
#include "MFTBase/GeometryTGeo.h"

using namespace AliceO2::MFT;

/// \cond CLASSIMP
ClassImp(ChipSegmentation);
/// \endcond

/// Default constructor

//_____________________________________________________________________________
ChipSegmentation::ChipSegmentation():
  VSegmentation()
{
}

/// Constructor

//_____________________________________________________________________________
ChipSegmentation::ChipSegmentation(UInt_t uniqueID):
  VSegmentation()
{
  // constructor
  Geometry * mftGeom = Geometry::Instance();

  SetUniqueID(uniqueID);

  SetName(Form("%s_%d_%d_%d_%d",GeometryTGeo::GetSensorName(),
               mftGeom->GetHalfMFTID(GetUniqueID()),
               mftGeom->GetHalfDiskID(GetUniqueID()),
               mftGeom->GetLadderID(GetUniqueID()),
               mftGeom->GetSensorID(GetUniqueID()) ));

  Double_t pos[3];
  pos[0] = mftGeom->GetSensorID(GetUniqueID())*(Geometry::kSensorLength + Geometry::kSensorInterspace) + Geometry::kSensorSideOffset;
  pos[1] = Geometry::kSensorTopOffset;
  pos[2] = Geometry::kFlexThickness;
  SetPosition(pos);
  
  //AliDebug(2,Form("Creating %s, UniqueID = %d, Position = (%.2f, %.2f, %.2f)",GetName(), GetUniqueID(), pos[0], pos[1], pos[2]));
  
}

/// Returns the pixel ID corresponding to a hit at (x,y) in the Sensor  frame
///
/// \param [in] xHit Double_t : x Position of the Hit
/// \param [in] yHit Double_t : y Position of the Hit
///
/// \param [out] xPixel Int_t : x position of the pixel hit on the sensor matrix
/// \param [out] yPixel Int_t : y position of the pixel hit on the sensor matrix
/// \retval <kTRUE> if hit into the active part of the sensor
/// \retval <kFALSE> if hit outside the active part
//

//_____________________________________________________________________________
Bool_t ChipSegmentation::Hit2PixelID(Double_t xHit, Double_t yHit, Int_t &xPixel, Int_t &yPixel) {
  // TODO Need to work on the Misalignment
  
  Double_t xHitLocal = xHit-Geometry::kSensorMargin;
  Double_t yHitLocal = yHit-(Geometry::kSensorMargin + Geometry::kSensorHeight - Geometry::kSensorActiveHeight);

  //Double_t xHitLocal = (xHit-(fActiveOrigin[0]+fMisalignmentShift[0]))*fSignLength[0];
  //Double_t yHitLocal = (yHit-(fActiveOrigin[1]+fMisalignmentShift[1]))*fSignLength[1];
  //AliDebug(2,Form("Hit %f %f --> Pixel Pitch %f  %f ",xHitLocal,yHitLocal,Geometry::kXPixelPitch,Geometry::kYPixelPitch));

  if (xHitLocal<0. || xHitLocal>Geometry::kSensorActiveWidth) return kFALSE;
  if (yHitLocal<0. || yHitLocal>Geometry::kSensorActiveHeight) return kFALSE;

  xPixel = Int_t( xHitLocal / Geometry::kXPixelPitch );
  yPixel = Int_t( yHitLocal / Geometry::kYPixelPitch );
  //AliDebug(1,Form("--> Hit in Pixel %d ; %d ",xPixel,yPixel));

  return kTRUE;

}

/// \brief Print out Sensor information (Name, ID, position, orientation)

//_____________________________________________________________________________
void ChipSegmentation::Print(Option_t* /*option*/){
  
  //AliInfo(Form("Sensor %s (Unique ID = %d)",GetName(),GetUniqueID()));
  GetTransformation()->Print();
}
