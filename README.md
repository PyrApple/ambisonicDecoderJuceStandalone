# JUCE (C++) Ambisonic Decoder Gains Exporter

Export Ambisonic gains for irregular speaker configurations standalone. Based on the EPAD scheme [1]

## How to use

Define speaker configuration in a xml file, coordinates system is either carthesian ``xyz`` (in meters) or spherical ``aed`` (azimuth, elevation, in degrees, distance in meters):

```xml
<speakerconfig>
  <speaker conv="xyz"> 1, 0, 0 </speaker>
  <speaker conv="aed"> 180, 0, 1 </speaker>
  <!-- ... -->
  <speaker conv="xyz"> 0, 0, -1 </speaker>
</speakerconfig>
```
Upon import of this xml file in the exporter, EPAD Ambisonic channels gains will be displayed on its log window. Said gains can then be exported to a xml file. The estimated maximum Ambisonic order for the given speaker configuration is specified by the ``order`` flag:

```xml
<speakergains order='1'>
    <speaker>
        <pos conv='xyz'> 1, 0, 0 </pos>
        <gains> 0.855033, 0, 0, 1.48096 </gains>
    </speaker>
    <!-- ... -->
    <speaker>
        <pos conv='xyz'> 0, 0, -1 </pos>
        <gains> 0.855033, 0, -1.48096, 0 </gains>
    </speaker>
</speakergains>
```

A compensation factor is added to these gains, to compensate for speaker distance from the origin, following the inverse square law.

## Convention
* Channel ordering: ACN
* Normalization: N3D
* Polarization: NOT Condon-Shortley phase convention
* Coord system: see coordinate-system.png file

## Build Requirements

requirements from the [ambisonicDecoder](https://github.com/PyrApple/ambisonicDecoder) library:
* [Eigen](http://eigen.tuxfamily.org) for singular value decomposition.
* [Boost](http://www.boost.org) for spherical harmonics.

install on OSX:
```
brew install eigen boost
```

## Acknowledgements

Library developed with the Imperial College of London - Dyson School of Design and Engineering, as part of the [3D-Tune-In project](http://3d-tune-in.eu/), who received fundings from the European Union’s Horizon 2020 research and innovation programme under grant agreement No 644051.

## References

[1] Franz Zotter, Hannes Pomberger, Markus Noisternig. Energy-preserving Ambisonics decoding. Acta Acustica united with Acustica, Hirzel Verlag, 2012, 98 (1), pp.37-47.
