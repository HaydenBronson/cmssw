/*! \class   TTStubAlgorithm_globalgeometry
 *  \brief   Class for "globalgeometry" algorithm to be used
 *           in TTStubBuilder
 *  \details Makes use only of global coordinates to accept the stub
 *           above threshold and to backproject it to the luminous region.
 *           After moving from SimDataFormats to DataFormats,
 *           the template structure of the class was maintained
 *           in order to accomodate any types other than PixelDigis
 *           in case there is such a need in the future.
 *
 *  \author Andrew W. Rose
 *  \author Nicola Pozzobon
 *  \date   2013, Jul 18
 *
 */

#ifndef L1_TRACK_TRIGGER_STUB_ALGO_GLOBALGEOMETRY_H
#define L1_TRACK_TRIGGER_STUB_ALGO_GLOBALGEOMETRY_H

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "L1Trigger/TrackTrigger/interface/TTStubAlgorithm.h"
#include "L1Trigger/TrackTrigger/interface/TTStubAlgorithmRecord.h"

#include <memory>
#include <string>
#include <map>

template< typename T >
class TTStubAlgorithm_globalgeometry : public TTStubAlgorithm< T >
{
  private:
    /// Data members
    double mCompatibilityScalingFactor;
    double mIPWidth;

  public:
    /// Constructor
    TTStubAlgorithm_globalgeometry( const TrackerGeometry* const theTrackerGeom, const TrackerTopology* const theTrackerTopo, 
                                    double aCompatibilityScalingFactor,
                                    double aIPWidth ) : TTStubAlgorithm< T >( theTrackerGeom, theTrackerTopo, __func__ )
    {
      mCompatibilityScalingFactor = aCompatibilityScalingFactor;
      mIPWidth = aIPWidth;
    }

    /// Destructor
    ~TTStubAlgorithm_globalgeometry(){}

    /// Matching operations
    void PatternHitCorrelation( bool &aConfirmation,
                                int &aDisplacement,
                                int &anOffset,
                                const TTStub< T > &aTTStub ) const;

}; /// Close class

/*! \brief   Implementation of methods
 *  \details Here, in the header file, the methods which do not depend
 *           on the specific type <T> that can fit the template.
 *           Other methods, with type-specific features, are implemented
 *           in the source file.
 */

/// Matching operations
template< >
void TTStubAlgorithm_globalgeometry< Ref_Phase2TrackerDigi_ >::PatternHitCorrelation( bool &aConfirmation,
                                                                              int &aDisplacement,
                                                                              int &anOffset,
                                                                              const TTStub< Ref_Phase2TrackerDigi_ > &aTTStub ) const;

/*! \class   ES_TTStubAlgorithm_globalgeometry
 *  \brief   Class to declare the algorithm to the framework
 *
 *  \author Andrew W. Rose
 *  \date   2013, Jul 18
 *
 */

template< typename T >
class  ES_TTStubAlgorithm_globalgeometry : public edm::ESProducer
{
  private:
    /// Data members
    std::shared_ptr< TTStubAlgorithm< T > > _theAlgo;
    double mPtThreshold;
    double mIPWidth;

  public:
    /// Constructor
    ES_TTStubAlgorithm_globalgeometry( const edm::ParameterSet & p ) :
                                            mPtThreshold( p.getParameter< double >("minPtThreshold") ),
                                            mIPWidth( p.getParameter< double >("ipWidth") )
    {
      setWhatProduced( this );
    }

    /// Destructor
    virtual ~ES_TTStubAlgorithm_globalgeometry(){}

    /// Implement the producer
    std::shared_ptr< TTStubAlgorithm< T > > produce( const TTStubAlgorithmRecord & record )
    { 
      /// Get magnetic field
      edm::ESHandle< MagneticField > magnet;
      record.getRecord< IdealMagneticFieldRecord >().get(magnet);
      double mMagneticFieldStrength = magnet->inTesla( GlobalPoint(0,0,0) ).z();

      /// Calculate scaling factor based on B and Pt threshold
      double mCompatibilityScalingFactor = ( CLHEP::c_light * mMagneticFieldStrength ) / ( 100.0 * 2.0e+9 * mPtThreshold );

      edm::ESHandle< TrackerGeometry > tGeomHandle;
      record.getRecord< TrackerDigiGeometryRecord >().get( tGeomHandle );
      const TrackerGeometry* const theTrackerGeom = tGeomHandle.product();
      edm::ESHandle<TrackerTopology> tTopoHandle;
      record.getRecord<IdealGeometryRecord>().get(tTopoHandle);
      const TrackerTopology* const theTrackerTopo = tTopoHandle.product();

      TTStubAlgorithm< T >* TTStubAlgo = new TTStubAlgorithm_globalgeometry< T >( theTrackerGeom, theTrackerTopo, mCompatibilityScalingFactor, mIPWidth );
      _theAlgo = std::shared_ptr< TTStubAlgorithm< T > >( TTStubAlgo );
      return _theAlgo;
    }

}; /// Close class

#endif

