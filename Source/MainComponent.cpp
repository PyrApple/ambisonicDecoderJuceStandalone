#include "MainComponent.h"
#include "Utils.h"
#include <cmath>

MainContentComponent::MainContentComponent()
{
    // set main window size
    setSize (600, 400);
    
    // text buttons
    buttonMap.insert({
        { &openConfigButton, "Open config file (.xml)" },
        { &exportConfigButton, "Export config file (.xml)" },
        { &openDisplayButton, "Display speakers config" },
        { &computeGainsButton, "Compute gains" },
        { &exportGainsButton, "Export gains (.xml)" },
        { &addSpkButton, "+" },
        { &clearSpkButton, "clear all" }
    });
    for( auto& pair : buttonMap )
    {
        pair.first->setButtonText( pair.second );
        pair.first->addListener( this );
        pair.first->setColour( TextButton::buttonColourId, colourMain );
        pair.first->setColour( TextButton::textColourOffId, colourBkg );
        addAndMakeVisible( pair.first );
    }
    
    // log text box
    addAndMakeVisible( logTextBox );
    logTextBox.setMultiLine( true );
    logTextBox.setReturnKeyStartsNewLine( true );
    logTextBox.setReadOnly( true );
    logTextBox.setScrollbarsShown( true );
    logTextBox.setColour( TextEditor::textColourId, colourMain );
    logTextBox.setColour( TextEditor::backgroundColourId, Colours::transparentBlack );
    logTextBox.setColour( TextEditor::outlineColourId, colourMain );
    
    // speaker tree
    addAndMakeVisible( speakerTree );
    
    // images
    logo3dtiImage = ImageCache::getFromMemory( BinaryData::_3dti_logo_png, BinaryData::_3dti_logo_pngSize );
    logoIclImage = ImageCache::getFromMemory( BinaryData::icl_logo_png, BinaryData::icl_logo_pngSize );
    float sFact = .4f;
    logo3dtiImage = logo3dtiImage.rescaled( sFact*logo3dtiImage.getWidth(), sFact*logo3dtiImage.getHeight(), Graphics::mediumResamplingQuality );
    logoIclImage = logoIclImage.rescaled( sFact*logoIclImage.getWidth(), sFact*logoIclImage.getHeight(), Graphics::mediumResamplingQuality);
}

MainContentComponent::~MainContentComponent()
{
    if( displayWindow != nullptr ){ delete displayWindow; }
}

void MainContentComponent::paint( Graphics& g )
{
    // Our component is opaque, so we must completely fill the background with a solid colour
    g.fillAll( colourBkg );
    g.setColour( colourMain );
    g.drawText( "Ambisonic decode gains:", 20.f, 85.f, getWidth() - 2*20.f, 10, Justification::left );
    
    // images (logos) and credits
    float margin = 10.f;
    g.setFont(10);
    g.drawText("Designed by D. Poirier-Quinot, Dyson School of Design and Engineering, Imperial College, 2017", 20.f, getHeight()-15.f, getWidth()-margin-20.f, 5, Justification::left);
    
    // skip logos if not enought width
    if( getWidth() < logoIclImage.getWidth()*3 ){ return; }
    
    g.drawImageAt( logoIclImage, getWidth()-logoIclImage.getWidth() - 2.5*margin, 40 + 2.7*margin );
    g.drawImageAt( logo3dtiImage, getWidth() - logo3dtiImage.getWidth() - logoIclImage.getWidth() - 5*margin, 40 + 2.7*margin );
}

void MainContentComponent::resized()
{
    // buttons
    float margin = 20.f;
    float h = 40.f;
    float w = ( getWidth() - 6*margin )/5.f;
    openConfigButton.setBounds( margin, margin, w, h );
    exportConfigButton.setBounds( openConfigButton.getX() + w + margin, margin,  w, h );
    openDisplayButton.setBounds( exportConfigButton.getX() + w + margin, margin,  w, h );
    computeGainsButton.setBounds( openDisplayButton.getX() + w + margin, margin,  w, h );
    exportGainsButton.setBounds( computeGainsButton.getX() + w + margin, margin,  w, h );
    
    // speaker tree
    float y = 2*margin + openConfigButton.getY() + openConfigButton.getHeight();
    addSpkButton.setBounds( getWidth() - margin, y, 20, 20 );
    clearSpkButton.setBounds( getWidth() - margin, addSpkButton.getY() + 2*margin, 20, 20 );
    speakerTree.setBounds(margin, y, getWidth() - 2*margin, 100.0f);
    
    // log window
    y = 2*margin + speakerTree.getY() + speakerTree.getHeight();
    logTextBox.setBounds( margin, y, getWidth() - 2*margin, getHeight() - y - margin );
}

void MainContentComponent::buttonClicked( Button* button )
{
    // get / load file
    if( button == &openConfigButton )
    {
        // clear current config
        speakerTree.clear();
        
        // get / load file
        FileChooser chooser( "Select a configuration file", File::nonexistent, "*.xml" );
        if( chooser.browseForFileToOpen() )
        {
            File file( chooser.getResult() );
            loadConfigFromFile( file );
        }
    }
    
    // export speaker config
    else if( button == &exportConfigButton )
    {
        speakerTree.getConfiguration( speakers );
        exportConfig();
    }
    
    // open display speakers window
    else if( button == &openDisplayButton )
    {
        speakerTree.getConfiguration( speakers );
        displaySpeakerConfigWindow();
    }
    
    // compute Ambisonic gains
    else if( button == &computeGainsButton )
    {
        speakerTree.getConfiguration( speakers );
        if( speakers.size() > 0 ){ computeGains(); }
    }
    
    // export Ambisonic gains to desktop
    else if( button == &exportGainsButton )
    {
        exportGains();
    }

    // add speaker item to configuration
    else if( button == &addSpkButton )
    {
        speakerTree.addSpkItem();
    }

    // clear all speakers from configuration
    else if( button == &clearSpkButton )
    {
        speakerTree.clear();
    }
}

// load config file
void MainContentComponent::loadConfigFromFile( File & file )
{
    // import content of xml file to speakers
    bool importSucess = xmlIO.readConfig( file, speakers );
    if( !importSucess ){ return; }
    
    // update speaker tree GUI
    speakerTree.setConfiguration( speakers );
}

// compute ambisonic gains for current speaker config
void MainContentComponent::computeGains()
{
    // format speaker pos to matrix to use in Ambi decode
    Eigen::MatrixXf spkAzimElev( 2, speakers.size() );
    for( int i = 0; i < speakers.size(); i++ ){
        spkAzimElev(0,i) = rad2deg( speakers[i].aed[0] );
        spkAzimElev(1,i) = rad2deg( speakers[i].aed[1] );
    }
    
    // get max ambisonic order
    int order = getMaxAmbiOrder( speakers.size() );
    
    // get decoding matrix
    bool useEpad = true;
    ambiGains = ambisonicDecoder.getDecodingMatrix( spkAzimElev, order, useEpad );
    rmNearZero( ambiGains );
    
    // apply distance gain compensation
    for( int i = 0; i < speakers.size(); i++ ){
        float distNorm = pow( speakers[i].aed[2], 2 );
        for( int j = 0; j < ambiGains.cols(); j++ ){
            ambiGains(i,j) *= distNorm;
        }
    }
    
    // write matrix to log window
    std::stringstream buffer;
    buffer << ambiGains << std::endl;
    logTextBox.setText( String(buffer.str()) );
    
    // enable export button
    exportGainsButton.setEnabled( true );
}

// open display speaker config window
void MainContentComponent::displaySpeakerConfigWindow()
{
    // delete existing window when loading new config
    if( displayWindow != nullptr ){ delete displayWindow; }
    
    bool native = true; // use native title bar
    
    // create window
    displayWindow = new Display( "Speaker Configuration", colourBkg, DocumentWindow::allButtons, speakers );
    
    // define window pos
    RectanglePlacement placement( (native ? RectanglePlacement::xLeft : RectanglePlacement::xRight)
                                  | RectanglePlacement::yTop | RectanglePlacement::doNotResize);
    Rectangle<int> area( 0, 0, 600, 300 );
    Rectangle<int> result( placement.appliedTo( area, Desktop::getInstance().getDisplays()
                                                .getMainDisplay().userArea.reduced (20) ));
    displayWindow->setBounds( result );
    
    // config window
    displayWindow->setResizable( true, ! native );
    displayWindow->setUsingNativeTitleBar( native );
    displayWindow->setVisible( true );
}

// save speaker config to desktop
void MainContentComponent::exportConfig()
{
    // get export file
    const File file (File::getSpecialLocation (File::userDesktopDirectory).getNonexistentChildFile ("SpkeakerConfig", ".xml"));
    
    // save gains to xml file
    xmlIO.writeConfig( file, speakers );
}

// save content of data to desktop in fileName, format: SpkID, x, y, z, gW, gX, gY, ...
void MainContentComponent::exportGains()
{
    // get export file
    const File file (File::getSpecialLocation (File::userDesktopDirectory).getNonexistentChildFile ("AmbiGains", ".xml"));
    
    // save gains to xml file
    xmlIO.writeGains( file, speakers, ambiGains );
}
