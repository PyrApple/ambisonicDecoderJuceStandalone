/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "XmlIO.h"
#include "Utils.h"
#include "Display.h"
#include "SpeakerTree.h"
#include "AmbisonicDecoder.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

class MainContentComponent:
public Component,
public Button::Listener,
public Timer
{
    friend class Display;
    
public:
 
    MainContentComponent();
    ~MainContentComponent();

    // draw methods
    void paint( Graphics& ) override;
    void resized() override;
    
    // override virtual methods Button::Listener
    void buttonClicked( Button* button ) override;
    void timerCallback() override;
    
    // local methods
    void exportConfig();
    void computeGains();
    void exportGains();
    void displaySpeakerConfigWindow();
    void loadConfigFromFile( File & file );
    void updateAmbiOrderComboBox();
    
    // GUI components
    TextButton openConfigButton;
    TextButton exportConfigButton;
    TextButton openDisplayButton;
    TextButton computeGainsButton;
    TextButton exportGainsButton;
    TextButton addSpkButton;
    TextButton clearSpkButton;
    TextEditor logTextBox;
    Image logo3dtiImage;
    Image logoIclImage;
    ComboBox ambiOrderComboBox;
    Label ambiOrderLabel;
    SpeakerTree speakerTree;
    
    // misc.
    std::unordered_map< Button*, std::string > buttonMap;
    SafePointer< DocumentWindow > displayWindow;
    
    // speaker config
    XmlIO xmlIO;
    std::vector< Speaker > speakers;
    
    // ambisonic decoder
    AmbisonicDecoder ambisonicDecoder;
    Eigen::MatrixXf ambiGains;
    
private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
