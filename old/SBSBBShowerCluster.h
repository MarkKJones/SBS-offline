#ifndef ROOT_SBSBBShowerCluster
#define ROOT_SBSBBShowerCluster

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// SBSBBShowerCluster                                                      //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "SBSShowerBlock.h"

class SBSBBShowerCluster : public TObject {

public:

    SBSBBShowerCluster();
    SBSBBShowerCluster(Int_t nmaxblk);
    SBSBBShowerCluster(Int_t nmaxblk, SBSShowerBlock* block);
    virtual ~SBSBBShowerCluster();

    Float_t GetX() const {return fX;}
    Float_t GetY() const {return fY;}
    Float_t GetE() const {return fE;}
    Int_t   GetMult() const {return fMult;}

    Int_t GetNMaxBlocks() const {return fNMaxBlocks;}

    void SetX(Float_t var) {fX=var;}
    void SetY(Float_t var) {fY=var;}
    void SetE(Float_t var) {fE=var;}
    void SetMult(Int_t var) {fMult=var;}

    SBSShowerBlock** GetBlocks() {return fBlocks;}
    SBSShowerBlock* GetBlock(int i) {
      if(i<fMult){
	return fBlocks[i];
      }else{
	return 0;
      }
    }

    Int_t GetSize() {return fMult;}

    void AddBlock(SBSShowerBlock* block);

    void ClearEvent();
    void DeleteArrays();

private:

    Float_t fX;       // x position of the center
    Float_t fY;       // y position of the center

    Float_t fE;       // Energy deposit in block

    Int_t fMult;      // Number of blocks in the cluster
    Int_t fNMaxBlocks;// Max number of blocks


    SBSShowerBlock** fBlocks; //[fNMaxBlocks] List of blocks in cluster

    ClassDef(SBSBBShowerCluster,0)   // Generic shower cluster class
};

#endif
