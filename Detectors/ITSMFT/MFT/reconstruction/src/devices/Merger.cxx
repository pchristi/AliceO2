#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "FairMQLogger.h"

#include "TMessage.h"

#include "MFTSimulation/EventHeader.h"
#include "MFTReconstruction/devices/Merger.h"

using namespace AliceO2::MFT;

using namespace std;

// special class to expose protected TMessage constructor
//_____________________________________________________________________________
class MergerTMessage : public TMessage
{
  public:
  MergerTMessage(void* buf, Int_t len)
    : TMessage(buf, len)
  {
    LOG(INFO) << "Merger::TMessage >>>>> create message of length " << len << "";
    ResetBit(kIsOwner);
  }
};

//_____________________________________________________________________________
Merger::Merger()
  : FairMQDevice()
  , mEventHeader(nullptr)
  , mNofParts(2)
  , mNofPartsPerEventMap()
  , mObjectMap()
  , mInputChannelName("data-in")
  , mOutputChannelName("data-out")
  , mEvRIPair()
  , mEvRIPartTrio()
  , mRet()
  , mNofReceivedMessages(0)
  , mNofSentMessages(0)
{

}

//_____________________________________________________________________________
Merger::~Merger()
= default;

//_____________________________________________________________________________
void Merger::Init()
{

  OnData(mInputChannelName, &Merger::MergeData);

}

//_____________________________________________________________________________
bool Merger::MergeData(FairMQParts& parts, int index)
{

  bool printInfo = true;

  //bool dataDuplicationFlag = false;
  int nofReceivedParts = 0; // if set to -1, the data seems to be duplicated

  mNofReceivedMessages++;
  TObject* tempObject;
  TClonesArray* tempArrays[10];
  int nofArrays = 0;

  LOG(INFO) << "Merger::MergeData >>>>> receive " << parts.Size() << " parts" << "";

  for (int ipart = 0 ; ipart < parts.Size() ; ipart++) {

    MergerTMessage tm(parts.At(ipart)->GetData(), parts.At(ipart)->GetSize());
    tempObject = (TObject*)tm.ReadObject(tm.GetClass());

    LOG(INFO) << "Merger::MergeData >>>>> part " << ipart << " with name " << tempObject->GetName() << "";

    if (strcmp(tempObject->GetName(),"EventHeader.") == 0) {
      
      mEventHeader = (EventHeader*)tempObject;
      
      LOG(INFO) << "Merger::MergeData >>>>> got EventHeader part [" << mEventHeader->GetRunId() << "][" << mEventHeader->GetMCEntryNumber() << "][" << mEventHeader->GetPartNo() << "]";
      
      // setting how many parts were received...
      mEvRIPair    .first  = mEventHeader->GetMCEntryNumber();
      mEvRIPair    .second = mEventHeader->GetRunId();
      mEvRIPartTrio.first  = mEvRIPair;
      mEvRIPartTrio.second = mEventHeader->GetPartNo();
      
      MultiMapDef::iterator it3;
      it3 = mObjectMap.find(mEvRIPartTrio);
      if (it3 != mObjectMap.end()) {
	
	LOG(INFO) << "Merger::MergeData >>>>> shouldn't happen, already got objects for part " << mEvRIPartTrio.second 
		  << ", event " << mEvRIPair.first << ", run " << mEvRIPair.second << ". Skipping this message!!!";
	
	nofReceivedParts = -1;
	break; // break the for(ipart) loop, as nothing else is left to do
	
      }
                  
      std::map<std::pair<int,int>,int>::iterator it2;
      it2 = mNofPartsPerEventMap.find(mEvRIPair);
      if (it2 == mNofPartsPerEventMap.end()) {
	
	LOG(INFO) << "Merger::MergeData >>>>> First part of event " << mEvRIPair.first;
	
	mNofPartsPerEventMap[mEvRIPair] = 1;
	nofReceivedParts = 1;
	
      } else {
	
	LOG(INFO) << "Merger::MergeData >>>>> Second part of event " << mEvRIPair.first;
	
	it2->second += 1;
	nofReceivedParts = it2->second;
	
      }
      
      LOG(INFO) << "Merger::MergeData >>>>> got " << nofReceivedParts << " parts of event " << mEvRIPair.first;

    } else { 
      
      tempArrays[nofArrays] = (TClonesArray*)tempObject;
      nofArrays++;

      LOG(INFO) << "Merger::MergeData >>>>> got " << nofArrays << " arrays and " << nofReceivedParts << " parts of event";

    }

  } // end the for(ipart) loop, should have received TCAs in tempArrays and EventHeader

  if (nofReceivedParts == -1) return true;
  
  if (nofReceivedParts != mNofParts) { // got all the parts of the event, have to combine and send message, consisting of objects from tempArrays  
    
    LOG(INFO) << "Merger::MergeData >>>>> not all parts are yet here (" << nofReceivedParts << " of " << mNofParts << ") adding to (size = " << mObjectMap.size() << ")";
    
    LOG(INFO) << "Merger::MergeData >>>>> + " << mEventHeader->GetName() << "[" << mEvRIPartTrio.first.second << "][" << mEvRIPartTrio.first.first << "][" << mEvRIPartTrio.second << "]";
    
    mObjectMap.insert(std::pair<std::pair<std::pair<int,int>,int>,TObject*>(mEvRIPartTrio,(TObject*)mEventHeader));
    
    for (int iarray = 0 ; iarray < nofArrays ; iarray++) {
      
      LOG(INFO) << "Merger::MergeData >>>>> + " << tempArrays[iarray]->GetName() << "[" << mEvRIPartTrio.first.second << "][" << mEvRIPartTrio.first.first << "][" << mEvRIPartTrio.second << "]";
      
      mObjectMap.insert(std::pair<std::pair<std::pair<int,int>,int>,TObject*>(mEvRIPartTrio,(TObject*)tempArrays[iarray]));
      
    }
    
    LOG(INFO) << "Merger::MergeData >>>>> now we have fObjectMap (size = " << mObjectMap.size() << ")";

    if (printInfo) 
      LOG(INFO) << "Merger::MergeData::printInfo >>>>> [" << mEventHeader->GetRunId() << "][" << mEventHeader->GetMCEntryNumber() << "][" << mEventHeader->GetPartNo() << "] Received: " << mNofReceivedMessages << " // Buffered: " << mObjectMap.size() << " // Sent: " << mNofSentMessages << " <<";

  } else { 
	
    int currentEventPart = mEventHeader->GetPartNo();
    for (int iarray = 0 ; iarray < nofArrays; iarray++) {
      
      LOG(INFO) << R"(Merger::MergeData::printInfo >>>>> before adding, TCA ")" << tempArrays[iarray]->GetName() << R"(" has )" << tempArrays[iarray]->GetEntries() << " entries.";

      TClonesArray* arrayToAdd;
      
      for (int ieventpart = 0; ieventpart < mNofParts; ieventpart++) {
	
	if ( ieventpart == currentEventPart ) continue;
	
	mEvRIPartTrio.second = ieventpart;
	mRet = mObjectMap.equal_range(mEvRIPartTrio);
	
	for (auto it = mRet.first; it != mRet.second; ++it) {
	  
	  if (strcmp(tempArrays[iarray]->GetName(),it->second->GetName()) == 0) {
	    
	    arrayToAdd = (TClonesArray*)it->second;
	    tempArrays[iarray]->AbsorbObjects(arrayToAdd);
	    LOG(INFO) << "Merger::MergeData::printInfo >>>>> found one!, TCA has now " << tempArrays[iarray]->GetEntries() << " entries.";
	    
	  }
	  
	}
	
      }
      
    }
    
    for (int ieventpart = 0; ieventpart < mNofParts; ieventpart++) {
      
      if ( ieventpart == currentEventPart ) continue;
      mEvRIPartTrio.second = ieventpart;
      mRet = mObjectMap.equal_range(mEvRIPartTrio);
      mObjectMap.erase(mRet.first,mRet.second);
      
    }
    
    TMessage* messageFEH;
    TMessage* messageTCA[10];
    FairMQParts partsOut;
    
    messageFEH = new TMessage(kMESS_OBJECT);
    messageFEH->WriteObject(mEventHeader);
    partsOut.AddPart(NewMessage(messageFEH->Buffer(), 
				messageFEH->BufferSize(), 
				[](void* /*data*/, void* hint) { delete (TMessage*)hint;},messageFEH));

    for (int iarray = 0; iarray < nofArrays; iarray++) {
      
      messageTCA[iarray] = new TMessage(kMESS_OBJECT);
      messageTCA[iarray]->WriteObject(tempArrays[iarray]);
      partsOut.AddPart(NewMessage(messageTCA[iarray]->Buffer(), 
				  messageTCA[iarray]->BufferSize(), 
				  [](void* /*data*/, void* hint) { delete (TMessage*)hint;},messageTCA[iarray]));
      
    }
    
    Send(partsOut, "data-out");
    mNofSentMessages++;
    
    if (printInfo)
      LOG(INFO) << "Merger::MergeData::printInfo >>>>> after Send() [" << mEventHeader->GetRunId() << "][" << mEventHeader->GetMCEntryNumber() << "][" << mEventHeader->GetPartNo() << "] Received: " << mNofReceivedMessages << " // Buffered: " << mObjectMap.size() << " // Sent: " << mNofSentMessages << " <<";
    
  }
  
  return true;

}
