/// \file TrivialClustererTask.h
/// \brief Definition of the ITS cluster finder task

#ifndef ALICEO2_ITS_TRIVIALCLUSTERERTASK
#define ALICEO2_ITS_TRIVIALCLUSTERERTASK

#include "FairTask.h" 

#include "ITSBase/GeometryTGeo.h"
#include "ITSReconstruction/TrivialClusterer.h"

class TClonesArray;

namespace AliceO2
{
namespace ITS
{
class TrivialClustererTask : public FairTask
{
 public:
  TrivialClustererTask();
  virtual ~TrivialClustererTask();

  virtual InitStatus Init();
  virtual void Exec(Option_t* option);

 private:
  GeometryTGeo mGeometry; ///< ITS geometry
  TrivialClusterer mTrivialClusterer;   ///< Cluster finder

  TClonesArray* mDigitsArray;   ///< Array of digits
  TClonesArray* mClustersArray; ///< Array of clusters

  ClassDef(TrivialClustererTask, 2)
};
}
}

#endif /* ALICEO2_ITS_TRIVIALCLUSTERERTASK */
