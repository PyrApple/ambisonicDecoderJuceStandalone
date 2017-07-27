#ifndef XMLIO_H_INCLUDED
#define XMLIO_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Utils.h"
#include <vector>

class XmlIO
{
public:
    
    XmlIO(){}
    
    ~XmlIO() {}
    
    // import xml config from file into speakers
    void importConfig( const File & file, std::vector<Speaker> & speakers )
    {
        // parse XML
        XmlDocument mainDocument( file );
        
        // get main element
        ScopedPointer<XmlElement> mainElement = mainDocument.getDocumentElement();
        
        //check for validity against the DTD
        if (mainElement == 0)
        {
            alertWindow(mainDocument.getLastParseError());
            return;
        }
        
        // resize input spk vector
        unsigned int numSpk = mainElement->getNumChildElements();
        speakers.resize( numSpk );
        
        // loop over xml elements (speakers)
        unsigned int count = 0;
        forEachXmlChildElementWithTagName(*mainElement, spkElmt, "speaker")
        {
            // get convention: xyz or aed
            String convention = "xyz";
            if( spkElmt->getNumAttributes() == 1 ){
                convention = spkElmt->getAttributeValue(0);
            }
            if( convention != "xyz"  && convention != "aed" ){
                alertWindow("wrong coordinate convention " + convention);
                return;
            }
            
            // get xyz / aed coordinates
            StringArray coordArray;
            coordArray.addTokens (spkElmt->getAllSubText(), ",", "\"");
            if( coordArray.size() != 3 ){
                alertWindow("wrong number of coordinates for speaker " + String(count));
                return;
            }
            Eigen::Vector3f coord(coordArray[0].getDoubleValue(), coordArray[1].getDoubleValue(), coordArray[2].getDoubleValue());
            
            // create speaker
            Speaker spk;
            spk.id = count;
            
            // convert xyz to aed
            if( convention == "xyz" ){ spk.aed = cartesianToSpherical(coord); }
            else{
                spk.aed = coord;
                spk.aed[0] = deg2rad(spk.aed[0]);
                spk.aed[1] = deg2rad(spk.aed[1]);
            }
            
            // store and increment
            speakers[spk.id] = spk;
            count += 1;
        }
        
    }
    
    void saveConfig( const File & file, const std::vector<Speaker> & speakers, const Eigen::MatrixXf ambiGains ){
        
        // create output string: loop over speakers
        String data = "<speakergains order='" + String( getMaxAmbiOrder(ambiGains.cols()) ) + "'>\n";
        for( int i = 0; i < speakers.size(); i++ ){
            
            // add speaker id
            data += "\n\t<speaker>\n";
            // data += String( speakers[i].id ) + ", ";
            
            // add speaker pos (cartesian)
            data += "\t\t<pos conv='xyz'> ";
            Eigen::Vector3f p = sphericalToCartesian(speakers[i].aed);
            for( int j = 0; j < p.size(); j++ ){ if( std::abs(p[j]) < 10e-7 ){ p[j] = 0; } } // round
            for( int j = 0; j < p.size(); j++ ){
                data += String(p[j]);
                if( j < p.size() - 1){ data += ", "; }
            }
            data += " </pos>\n";
            
            // add speaker gains
            data += "\t\t<gains> ";
            for( int j = 0; j < ambiGains.cols(); j++ ){
                data += String(ambiGains(i,j));
                if( j < ambiGains.cols() - 1){ data += ", "; }
            }
            data += " </gains>\n";
            
            // end line
            data += "\t</speaker>\n";
        }
        data += "\n</speakergains>";
        
        // write to file
        file.replaceWithText ( data );
        
        // notify user
        AlertWindow::showMessageBoxAsync( AlertWindow::InfoIcon, "file export", ".xml file saved to desktop", "OK" );
    }
    
    // display xml import error
    void alertWindow( const String& msg )
    {
        AlertWindow::showMessageBoxAsync( AlertWindow::WarningIcon, "Corrupted configuration file", msg, "OK" );
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XmlIO)
    
};

#endif // XMLIO_H_INCLUDED
