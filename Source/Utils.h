#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <eigen3/Eigen/Dense>
#include <vector>
#include <unordered_map>

// define colours
const Colour colourBkg(PixelARGB(250, 40, 40, 40));
const Colour colourMain(PixelARGB(250, 220, 220, 220));

// speaker structure
struct Speaker
{
    int id;
    Eigen::Vector3f aed;
};

// get max speaker radius out of config
inline float getMaxRadius(std::vector<Speaker> & speakers){
    float r = 0.1;
    for( int i = 0; i < speakers.size(); i++){
        r = fmax(r, speakers[i].aed[2]);
    }
    return r;
}

// Ambisonic methods
inline unsigned int getNumAmbiCh( const unsigned int order )
{
    return pow(order + 1, 2);
}

// return max Ambisonic order for numSpk number of speakers, -1 for
inline unsigned int getMaxAmbiOrder( const unsigned int numSpk )
{
    int maxOrder = floor( sqrt(numSpk) - 1 );
    return fmax( maxOrder, 0);
}

// coordinate convention: X front, Y left, Z up.
// azimuth in (xOy) 0° is facing x, clockwise,
// elevation in (zOx), 0° is on (xOy), 90° on z+, -90° on z-

inline Eigen::Vector3f cartesianToSpherical(const Eigen::Vector3f& p)
{
    float radius = std::sqrt(p(0) * p(0) + p(1) * p(1) + p(2) * p(2));
    float elevation = std::asin(p(2) / radius);
    float azimuth = std::atan2(p(1), p(0));
    if (p(1) < 0 && p(2) < 0)
        elevation += 2 * M_PI;
    
    return Eigen::Vector3f (azimuth, elevation, radius);
}

inline Eigen::Vector3f sphericalToCartesian(const Eigen::Vector3f& p)
{
    float x = p[2] * cos(p[0]) * cos(p[1]);
    float y = p[2] * sin(p[0]) * cos(p[1]);
    float z = p[2] * sin(p[1]);
    
    return Eigen::Vector3f (x, y, z);
}

inline void rmNearZero( Eigen::MatrixXf& m, float threshold = 10e-5)
{
    for( int i = 0; i < m.rows(); i++){
        for( int j = 0; j < m.cols(); j++){
            if( std::abs(m(i,j)) < threshold ){ m(i,j) = 0; }
        }
    }
}

inline float rmNearZero( float val, float threshold = 10e-5)
{
    if( std::abs( val ) < threshold ){ return 0; }
    else{ return val; }
}

template <typename Type>
inline Type deg2rad(Type deg) { return deg * M_PI / 180.0; }

inline Eigen::Vector3f deg2radVect(Eigen::Vector3f vect) {
    vect[0] = deg2rad(vect[0]);
    vect[1] = deg2rad(vect[1]);
    return vect;
}

template <typename Type>
inline Type rad2deg(Type rad) { return rad * 180.0 / M_PI; }

inline Eigen::Vector3f rad2degVect(Eigen::Vector3f vect) {
    vect[0] = rad2deg(vect[0]);
    vect[1] = rad2deg(vect[1]);
    return vect;
}
