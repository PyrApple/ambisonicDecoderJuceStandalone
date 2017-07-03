//
//  ambisonicDecoder.h
//  AmbisonicDecoder
//
//  Created by David Poirier-Quinot on 21/06/2017.
//  Copyright © 2017 ICL. All rights reserved.
//

#ifndef AmbisonicDecoder_hpp
#define AmbisonicDecoder_hpp

#include <boost/math/special_functions/spherical_harmonic.hpp>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Eigenvalues>

class AmbisonicDecoder
{
    
//==========================================================================
// ATTRIBUTES
    
public:
    
    
private:
    
//==========================================================================
// METHODS
    
public:
    
    AmbisonicDecoder() {}
    
    ~AmbisonicDecoder() {}
    
    Eigen::MatrixXf getDecodingMatrix( const Eigen::MatrixXf & spkAzimElev, const unsigned int order, const bool useEpad )
    {
        // init
        unsigned long numSpk = spkAzimElev.cols();
        unsigned int numCh = getNumAmbiCh( order );
        Eigen::MatrixXf ambiGains ( numSpk, numCh ) ;
        Eigen::VectorXf gains( numCh );
        float azim; float elev;
        
        // loop over speaker positions
        for( int i = 0; i < numSpk; i++ ){
            // get spk azim elev
            azim = spkAzimElev(0, i);
            elev = spkAzimElev(1, i);
            
            // get spherical harmonic coefficients
            getRSH( order, azim, elev, gains );
            
            // fill output
            for( int j = 0; j < numCh; j++ ){
                ambiGains(i,j) = gains[j];
            }
        }
        
        if( useEpad ){
            // singular value decomposition
            Eigen::JacobiSVD<Eigen::MatrixXf> svd(ambiGains.transpose(), Eigen::ComputeThinU | Eigen::ComputeThinV);
            // get ambiGains out of left / right matrices
            ambiGains = svd.matrixV() * svd.matrixU().transpose();
        }
        
        // normalization (only step required for 'SAD')
        float norm = 4 * M_PI / numSpk;
        ambiGains *= norm;
        
        return ambiGains;
    }
    
    // get real spherical harmonics
    void getRSH( const unsigned int n, const float azim, const float elev, Eigen::VectorXf & gains )
    {
        // init
        float r;
        float ri;
        
        // convert from polarch coord. system to boost's
        float theta = deg2rad( 90 - elev );
        float phi = deg2rad( azim );
        
        // order 0
        gains[0] = 1.0 / sqrt(4*M_PI);
        
        // loop over spherical harmonic indices
        int index = 1;
        for( int nn = 1; nn <= n; nn += 1){
            for( int m = -nn; m <= abs(nn); m += 1){
                r = boost::math::spherical_harmonic_r(nn, m, theta, phi);
                ri = boost::math::spherical_harmonic_i(nn, m, theta, phi);
                
                if( m != 0 ){ r = pow(-1, m) * sqrt(2) * r; }
                ri = - sqrt(2) * ri;
                
                if( m < 0 ){ gains[index] = ri; }
                else{ gains[index] = r; }
                index++;
            }
        }
    }
    
};

#endif /* AmbisonicDecoder_hpp */
