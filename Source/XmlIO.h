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
    bool readConfig( const File & file, std::vector<Speaker> & speakers )
    {
        // parse XML
        XmlDocument mainDocument( file );
        
        // get main element
        ScopedPointer<XmlElement> mainElement = mainDocument.getDocumentElement();
        
        //check for validity against the DTD
        if (mainElement == 0)
        {
            alertWindow(mainDocument.getLastParseError());
            return false;
        }
        
        // resize input spk vector
        unsigned int numSpk = mainElement->getNumChildElements();
        speakers.resize( numSpk );
        
        // loop over xml elements (speakers)
        unsigned int count = 0;
        bool isValidXmlFile = true;
        forEachXmlChildElementWithTagName(*mainElement, spkElmt, "speaker")
        {
            // create speaker
            Speaker spk;
            
            // sanity check
            if( spkElmt->getNumAttributes() != 2 ){
                alertWindow("wrong number of attributes in spk definition");
                isValidXmlFile = false;
            }
            
            // get convention: xyz or aed and spk id
            String convention = "xyz";
            for( int i = 0; i < spkElmt->getNumAttributes(); i++ ){
                if( spkElmt->getAttributeName(i) == "conv" ){
                    convention = spkElmt->getAttributeValue(i);
                    if( convention != "xyz"  && convention != "aed" ){
                        alertWindow("wrong coordinate convention " + convention);
                        isValidXmlFile = false;
                    }
                }
                else if( spkElmt->getAttributeName(i) == "id" ){
                    spk.id = spkElmt->getAttributeValue(i).getIntValue();
                }
                else{
                    alertWindow("wrong spk attribute: " + spkElmt->getAttributeName(i));
                    isValidXmlFile = false;
                }
            }
            
            // discard if xml file not valid
            if( !isValidXmlFile ){ return false; }

            // get xyz / aed coordinates
            StringArray coordArray;
            coordArray.addTokens (spkElmt->getAllSubText(), ",", "\"");
            if( coordArray.size() != 3 ){
                alertWindow("wrong number of coordinates for speaker " + String(spk.id));
                return false;
            }
            Eigen::Vector3f coord(coordArray[0].getDoubleValue(), coordArray[1].getDoubleValue(), coordArray[2].getDoubleValue());
            
            // convert xyz to aed
            if( convention == "xyz" ){ spk.aed = cartesianToSpherical(coord); }
            else{
                spk.aed = deg2radVect( coord );
            }
            
            // store and increment
            speakers[count] = spk;
            count += 1;
        }
        
        return true;
        
    }
    
    void writeConfig( const File & file, const std::vector<Speaker> & speakers ){
        
        // create output string: loop over speakers
        String data = "<speakerconfig>\n\n";
        for( int i = 0; i < speakers.size(); i++ ){
            
            // add speaker id
            data += "\t<speaker id='" + String( speakers[i].id ) + "' conv='aed'> ";
            
            // add speaker coordinates
            Eigen::Vector3f p = rad2degVect( speakers[i].aed );
            for( int j = 0; j < p.size(); j++ ){
                data += String( rmNearZero( p[j] ) );
                if( j < p.size() - 1){ data += ", "; }
            }

            // end line
            data += " </speaker>\n";
        }
        data += "\n</speakerconfig>";
        
        // write to file
        file.replaceWithText ( data );
        
        // notify user
        AlertWindow::showMessageBoxAsync( AlertWindow::InfoIcon, "file export", ".xml file saved to desktop", "OK" );
    }
    
    void writeGains( const File & file, const std::vector<Speaker> & speakers, const Eigen::MatrixXf ambiGains ){
        
        // create output string: loop over speakers
        std::cout << getMaxAmbiOrder( ambiGains.cols() ) << std::endl;
        String data = "<speakergains order='" + String( getMaxAmbiOrder(ambiGains.cols()) ) + "'>\n";
        for( int i = 0; i < speakers.size(); i++ ){
            
            // add speaker id
            data += "\n\t<speaker id='" + String( speakers[i].id ) + "'>\n";
            
            // add speaker pos (cartesian)
            data += "\t\t<pos conv='xyz'> ";
            Eigen::Vector3f p = sphericalToCartesian(speakers[i].aed);
            for( int j = 0; j < p.size(); j++ ){
                data += String( rmNearZero( p[j] ) );
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
