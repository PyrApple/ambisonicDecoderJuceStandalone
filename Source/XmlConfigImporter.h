#ifndef XMLCONFIGIMPORTER_H_INCLUDED
#define XMLCONFIGIMPORTER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Utils.h"
#include <vector>

class XmlConfigImporter
{
public:
    
    XmlConfigImporter(){}
    
    ~XmlConfigImporter() {}
    
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
                alertWindow("wrong convention " + convention);
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
    
    // display xml import error
    void alertWindow( const String& msg )
    {
        AlertWindow::showMessageBoxAsync( AlertWindow::WarningIcon, "Corrupted configuration file", msg, "OK" );
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XmlConfigImporter)
    
};

#endif // XMLCONFIGIMPORTER_H_INCLUDED
