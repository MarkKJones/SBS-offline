///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// SBSBBShowerCluster                                                        //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include "SBSBBShowerCluster.h"
#include <iostream>

using namespace std;

//const Float_t SBSBBShowerCluster::kBig =(Float_t)1e15;

//_____________________________________________________________
SBSBBShowerCluster::SBSBBShowerCluster(Int_t nmaxblk, SBSShowerBlock* block) 
: fNMaxBlocks(nmaxblk)
{
    fBlocks = new SBSShowerBlock*[fNMaxBlocks];
    fBlocks[0] = block;
    fX = block->GetX();
    fY = block->GetY();
    fE = block->GetE();
    fMult = 1;
}


//_____________________________________________________________
SBSBBShowerCluster::SBSBBShowerCluster(Int_t nmaxblk) 
: fNMaxBlocks(nmaxblk)
{
    fBlocks = new SBSShowerBlock*[fNMaxBlocks];
    fX = 0;
    fY = 0;
    fE = 0;
    fMult = 0;
}


//_____________________________________________________________
SBSBBShowerCluster::SBSBBShowerCluster() {
    fX = kBig;
    fY = kBig;
    fE = kBig;
    fMult = 0;
}

//_____________________________________________________________
SBSBBShowerCluster::~SBSBBShowerCluster() {

    DeleteArrays();

}

//_____________________________________________________________
void SBSBBShowerCluster::AddBlock(SBSShowerBlock* block) {

    if (fMult<fNMaxBlocks) {
        fBlocks[fMult] = block;
	//cout << fBlocks[fMult] << " " << fBlocks[fMult+1] << endl;
        block->SetStat(1);
        fMult++;
        //    cout << fX << " " << fE << " " << fMult << " " 
        // 	 << block->GetX() << " " << block->GetE() << endl;
        //     cout << fX*fE*(fMult-1) << " " << block->GetX()*block->GetE() << endl;
        fX = (fX*fE*(fMult-1) + block->GetX()*block->GetE()) / 
            (fE+block->GetE());
        fY = (fY*fE*(fMult-1) + block->GetY()*block->GetE()) / 
            (fE+block->GetE());
        fE += block->GetE();
    }

}

//_____________________________________________________________
void SBSBBShowerCluster::ClearEvent() {
    fMult=0;fX=fY=fE=0.;
    DeleteArrays();
    fBlocks = new SBSShowerBlock*[fNMaxBlocks];
}

//_____________________________________________________________
void SBSBBShowerCluster::DeleteArrays() {
    delete [] fBlocks; fBlocks = 0;
}

ClassImp(SBSBBShowerCluster)

