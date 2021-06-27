#ifndef SBSGenericDetector_h
#define SBSGenericDetector_h

////////////////////////////////////////////////////////////////////////////////
//
//SBSGenericDetector
//
// A generic detector class which could contain the following types
// of data:
// - ADC
//  - Can be single valued OR
//  - Can contain pulse information such as integral, amplitude and time
//  - Can be using Waveform data
// - TDC
//  - Can be single valued (leading edge) OR:
//  - Can contain both leading and trailing edge and Time-Over-Threshold
//
//  Organized into row, col, and layer structure (but makes no assumption about
//  number of rows, cols or layers are constant throughout).
//
////////////////////////////////////////////////////////////////////////////////

#include "THaNonTrackingDetector.h"
#include "TRotation.h"
#include "TVector3.h"
#include "THaDetMap.h"
#include "SBSElement.h"

namespace SBSModeADC {
  enum Mode{
    kNone,
    kADC,       //< Contains pulse information also
    kADCSimple, //< Does not contain pulse information (nor reference info)
    kWaveform,  //< Contains waveform data
  };
}

namespace SBSModeTDC {
  enum Mode{
    kNone,
    kIgnore,        //< Useful to preserve DB but ignore ADCs otherwise
    kTDC,       //< Includes Trailing edge (and ToT)
    kTDCSimple,  //< Does not contain trailing edge, and hence no ToT info
  };
}


// This structure has output data when the user wants every hit to be stored
// in the rootfile.
struct SBSGenericOutputData {
  // Note [] means it can be variable sized data per event/module
  // Module info
  std::vector<Int_t> row;         //< [] row
  std::vector<Int_t> col;         //< [] col
  std::vector<Int_t> ped;         //< [] pedestal
  std::vector<Int_t> layer;       //< [] layer
  // ADC variables
  std::vector<Float_t> a;         //< [] ADC integral
  std::vector<Float_t> a_p;         //< [] ADC integral -pedestal
  std::vector<Float_t> a_c;         //< [] (ADC integral -pedestal)*calib
  std::vector<Float_t> a_amp;     //< [] ADC pulse amplitude
  std::vector<Float_t> a_amp_p;     //< [] ADC pulse amplitude -pedestal
  std::vector<Float_t> a_time;    //< [] ADC pulse time
  // TDC variables
  std::vector<Float_t> t;         //< [] TDC (leading edge) time
  std::vector<Float_t> t_te;      //< [] TDC trailing edge time
  std::vector<Float_t> t_ToT;     //< [] TDC Time-Over-Threshold
  // Waveform variables
  std::vector<Int_t> nsamps;      //< [] Number of ADC samples
  std::vector<Int_t> sidx;        //< [] Index of start of ADC samples in for this row-col-layer
  std::vector<Float_t> samps;     //< []*nsamps ADC samples

  // Quick clear class
  void clear() {
    row.clear();
    col.clear();
    ped.clear();
    layer.clear();
    a.clear();
    a_p.clear();
    a_c.clear();
    a_amp.clear();
    a_amp_p.clear();
    a_time.clear();
    t.clear();
    t_te.clear();
    t_ToT.clear();
    nsamps.clear();
    sidx.clear();
    samps.clear();
  }
};

class SBSGenericDetector : public THaNonTrackingDetector {
  //class SBSGenericDetector : public THaShower {

public:
  SBSGenericDetector( const char* name, const char* description = "",
      THaApparatus* a = NULL);
  virtual ~SBSGenericDetector();

  virtual void Clear( Option_t* opt="" );
  virtual void ClearEvent();
  virtual void ClearOutputVariables();

  void SetModeADC(SBSModeADC::Mode mode);
  void SetModeTDC(SBSModeTDC::Mode mode) { fModeTDC = mode; }
  void SetDisableRefADC(Bool_t b) { fDisableRefADC = b; }
  void SetDisableRefTDC(Bool_t b) { fDisableRefTDC = b; }
  void SetStoreRawHits(Bool_t var) { fStoreRawHits = var; }
  void SetStoreEmptyElements(Bool_t b) { fStoreEmptyElements = b; }

  Bool_t WithTDC() { return fModeTDC != SBSModeTDC::kNone; };
  Bool_t WithADC() { return fModeADC != SBSModeADC::kNone; };

  // Standard apparatus re-implemented functions
  virtual Int_t      Decode( const THaEvData& );
  virtual Int_t      CoarseProcess(TClonesArray& tracks);
  virtual Int_t      FineProcess(TClonesArray& tracks);

  virtual Int_t      DecodeADC( const THaEvData&, SBSElement *blk,
      THaDetMap::Module *d, Int_t chan);
  virtual Int_t      DecodeTDC( const THaEvData&, SBSElement *blk,
      THaDetMap::Module *d, Int_t chan);
  virtual Int_t      DecodeRefTDC( const THaEvData&, SBSElement *blk,
      THaDetMap::Module *d, Int_t chan);

  // Utility functions
  // Can be re-implemented by other classes to specify a different
  // SBSElement sub-class (i.e. useful when one wants to change the logic
  // in SBSElement::CoarseProcess()
  virtual SBSElement* MakeElement(Float_t x, Float_t y, Float_t z, Int_t row,
      Int_t col, Int_t layer, Int_t id = 0);

protected:

  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );
  virtual Int_t  FindGoodHit(SBSElement *){ return 0; } // Optionally implemented by derived classes

  // Configuration
  Int_t  fNrows;        ///< Number of rows
  std::vector<Int_t>  fNcols; ///< Number of columns per row
  Int_t fNcolsMax;      ///< Max number of columns out of all rows
  Int_t  fNlayers;      ///< Number of layers (in z-direction)
  SBSModeADC::Mode fModeADC;      //< ADC Mode
  SBSModeTDC::Mode fModeTDC;      //< TDC Mode
  Bool_t fDisableRefADC; //< Reference ADC may be optionally disabled
  Bool_t fDisableRefTDC; //< Reference TDC may be optionally disabled
  Bool_t fStoreEmptyElements; //< Do not store data for empty elements in rootfile

  // Mapping (see also fDetMap)
  UShort_t   fChanMapStart; ///< Starting number for element number (i.e. 0 or 1)
  std::vector<std::vector<Int_t> > fChanMap; //< Maps modules in THaDetMap to calorimeter element number

  // Output variable containers
  SBSGenericOutputData fGood;     //< Good data output
  SBSGenericOutputData fRaw;      //< All hits

  // Blocks, where the grid is just for easy access to the elements by row,col,layer
  std::vector<SBSElement*> fElements;
  std::vector<SBSElement*> fRefElements; //< Reference elements (for TDCs and multi-function ADCs)
  std::vector<std::vector<std::vector<SBSElement*> > > fElementGrid;
  // Clusters for this event

  // Other parameters
  Bool_t fCoarseProcessed;  //< Was CourseProcessed already called?
  Bool_t fFineProcessed;    //< Was fine processed already called

  //Gain correction 
  Float_t   fConst;     // const from gain correction 
  Float_t   fSlope;     // slope for gain correction
  Float_t   fAccCharge; // accumulated charge

  // Per event data
  Int_t      fNhits;     ///< Number of hits in event
  Int_t      fNRefhits;     ///< Number of reference hits in event
  Int_t      fNGoodhits;     ///< Number of good hits in event

  // Flags for enabling and disabling various features
  Bool_t    fStoreRawHits; ///< Store the raw data in the root tree?

private:
  // Simple and quick routine to init and clear most vectors
  // (of integers, floats, doubles, etc...)
  // Reset/Init 1D vector
  template<class T>
  void InitVector(std::vector<T> &vec, T val = 0, size_t n = 0) {
    vec.resize(n);
    ResetVector(vec,val);
  }

  template<class T>
  void ResetVector(std::vector<T> &vec, T val = 0, size_t n = 0) {
    if(n > 0) {
      vec.clear();
      vec.resize(n);
    }
    for(size_t i = 0; i < vec.size(); i++) {
      vec[i] = val;
    }
  }

  // Reset 2D vector
  template<class T>
  void ResetVector(std::vector<std::vector<T> > &vec, T val = 0,
      size_t nr = 0, size_t nc = 0) {
    if(nr > 0) {
      vec.clear();
      vec.resize(nr);
    }
    for(size_t i = 0; i < vec.size(); i++) {
      ResetVector(vec[i],val,nc);
    }
  }

  ClassDef(SBSGenericDetector,0)     //Generic shower detector class
};

/*inline Int_t SBSGenericDetector::blkidx(Int_t row, Int_t col, Int_t layer)
{
  return fNlayers*(fNcols[row]*row + col) + layer;
}

inline void SBSGenericDetector::blkrcl(Int_t index, Int_t &row, Int_t &col,
    Int_t &layer)
{
  row = index/(fNlayers*fNcols);
  index -= row*fNlayers*fNcols;
  col = index/fNlayers;
  layer = index%fNlayers;
}
*/

////////////////////////////////////////////////////////////////////////////////
// Specify some default sub-classes available to the user

#endif // SBSGenericDetector_h
