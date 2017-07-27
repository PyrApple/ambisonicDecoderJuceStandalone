#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Utils.h"

class Display : public DocumentWindow
{
    
private:
    
    std::vector<Speaker> speakers; // copy of mainComponent's
    float spkRad = 20.f; // speaker ellipse radius in pixels
    float normX; // draw norm factor
    float normY; // draw norm factor
    
public:
    
    Display( const String& name, Colour backgroundColour, int buttonsNeeded, std::vector<Speaker> & _speakers )
    : DocumentWindow( name, backgroundColour, buttonsNeeded )
    {
        speakers = _speakers;
    }
    
    ~Display(){}

    void paint( Graphics& g ) override
    {
        // background color
        g.fillAll( colourBkg );
        
        // frame display window
        g.setColour( colourMain );
        g.drawLine( getWidth()/2, 0.f, getWidth()/2, getHeight(), 2.0f );
        
        // update nor factors based on array max radius
        normX = 1.2 * getMaxRadius( speakers ) / ( getWidth()/4.f );
        normY = 1.2 * getMaxRadius( speakers ) / ( getHeight()/2.f );
        
        // draw speakers
        float originY = getHeight()/2.f;
        for( int i = 0; i < speakers.size(); i++ ){
            Eigen::Vector3f p = sphericalToCartesian( speakers[i].aed );
            // draw view side
            drawSpeaker( g, getWidth()/4.f, originY, -p[1], -p[0], speakers[i].id);
            // draw view top
            drawSpeaker( g, getWidth()*(3.f/4.f), originY, -p[1], -p[2], speakers[i].id);
        }
        
        // view legends
        float margin = 10.f;
        float h = getHeight()-margin-10; float w = 70;
        g.drawText("view side", margin, h, w, 10, Justification::left);
        g.drawText("view top", getWidth() - w - margin, h, w, 10, Justification::right);
        
        // view axis: horizontal Y
        float L = 40.f; h -= 10.f;
        g.setColour( Colours::lightgreen );
        g.drawLine( margin, h, margin+L, h, 1.0f ); // left-Y
        g.drawText( "Y", margin, h-15, 10, 10, Justification::left );
        g.drawLine( getWidth() - L - 2*margin - 10, h, getWidth() - 2*margin - 10, h, 1.0f ); // right-Y
        g.drawText( "Y", getWidth() - margin - 20 - L, h-15, 10, 10, Justification::left );
        
        // view axis: vertical Y/Z
        g.setColour( Colours::red );
        g.drawLine( margin+L, h, margin+L, h-L, 1.0f ); // left-X
        g.drawText( "X", margin+L-15, h-L, 10, 10, Justification::left );
        g.setColour( Colours::aliceblue );
        g.drawLine( getWidth() - 2*margin - 10, h, getWidth() - 2*margin - 10, h-L, 1.0f ); // right-Z
        g.drawText( "Z", getWidth() - 2*margin - 25, h-L, 10, 10, Justification::left ); // right-Z
    }
    
    // draw speakers as circles on screen
    void drawSpeaker( Graphics& g, float originX, float originY, float x, float y, unsigned int id ){
        float spkX = originX + x/normX;
        float spkY = originY + y/normY;
        g.setColour( colourMain );
        g.fillEllipse( spkX - spkRad, spkY - spkRad, spkRad, spkRad );
        g.setColour( colourMain );
        g.drawText( String(id), spkX, spkY, 20, 10, Justification::left );
    }
    
    void closeButtonPressed() override { delete this; }
    
    void resized() override {}

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Display)
};
