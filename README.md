# JUCE (C++) Ambisonic Decoder Gains Exporter

Export Ambisonic gains for irregular speaker configurations standalone.

## Requirements

from [ambisonicDecoder](https://github.com/PyrApple/ambisonicDecoder):
* [Eigen](http://eigen.tuxfamily.org) for singular value decomposition.
* [Boost](http://www.boost.org) for spherical harmonics.

install on OSX:
```
brew install eigen boost
```

## Convention
* Channel ordering: ACN
* Normalization: N3D
* Polarization: Condon-Shortley phase convention
* Azimuth is 0° front, 90° right. Elevation is 0° front 90° up.


## Acknowledgements

Library developed with the Imperial College of London - Dyson School of Design and Engineering, as part of the [3D-Tune-In project](http://3d-tune-in.eu/), who received fundings from the European Union’s Horizon 2020 research and innovation programme under grant agreement No 644051.
