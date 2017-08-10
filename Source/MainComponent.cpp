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
        { &openDisplayButton, "Display \nspeakers" },
        { &computeGainsButton, "Compute \ngains" },
        { &exportGainsButton, "Export gains file (.xml)" },
        { &addSpkButton, "+" },
        { &clearSpkButton, "clear" }
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
    
    // ambisonic order combo box
    ambiOrderComboBox.setEditableText( false );
    ambiOrderComboBox.setJustificationType (Justification::centred);
    ambiOrderComboBox.addItem("0", 1);
    ambiOrderComboBox.setSelectedId( 1, dontSendNotification );
    ambiOrderComboBox.setColour(ComboBox::outlineColourId, colourBkg);
    ambiOrderComboBox.setColour(ComboBox::backgroundColourId, colourBkg);
    addAndMakeVisible( ambiOrderComboBox );
    
    // ambisonic order label
    ambiOrderLabel.setColour( TextButton::textColourOffId, colourBkg );
    ambiOrderLabel.setText("ambi order", dontSendNotification);
    ambiOrderLabel.setEditable( false );
    ambiOrderLabel.setJustificationType(Justification::right);
    addAndMakeVisible( ambiOrderLabel );
    
    // speaker tree
    addAndMakeVisible( speakerTree );
    
    // images
    logo3dtiImage = ImageCache::getFromMemory( BinaryData::_3dti_logo_png, BinaryData::_3dti_logo_pngSize );
    logoIclImage = ImageCache::getFromMemory( BinaryData::icl_logo_png, BinaryData::icl_logo_pngSize );
    float sFact = .4f;
    logo3dtiImage = logo3dtiImage.rescaled( sFact*logo3dtiImage.getWidth(), sFact*logo3dtiImage.getHeight(), Graphics::mediumResamplingQuality );
    logoIclImage = logoIclImage.rescaled( sFact*logoIclImage.getWidth(), sFact*logoIclImage.getHeight(), Graphics::mediumResamplingQuality);
    
    // start timer (updating available ambisonic order values)
    startTimer(3000);
}

MainContentComponent::~MainContentComponent()
{
    if( displayWindow != nullptr ){ delete displayWindow; }
}

void MainContentComponent::paint( Graphics& g )
{
    float margin = 20.f;
    
    // Our component is opaque, so we must completely fill the background with a solid colour
    g.fillAll( colourBkg );
    g.setColour( colourMain );
    g.drawText( "Ambisonic decode gains", margin, logTextBox.getY()-15.f, 140, 10, Justification::left );
    
    // images (logos) and credits
    g.setFont(10);
    g.drawText("Designed by D. Poirier-Quinot, Dyson School of Design and Engineering, Imperial College, 2017", margin, getHeight()-15.f, getWidth()-2*margin, 5, Justification::right);
    
    // skip logos if not enought width
    if( getWidth() < logoIclImage.getWidth()*3 ){ return; }
    float y = speakerTree.getY() - margin;
    float x = getWidth() - margin - logoIclImage.getWidth();
    g.drawImageAt( logoIclImage, x, y - logoIclImage.getHeight()/2);
    g.drawImageAt( logo3dtiImage, x - margin - logo3dtiImage.getWidth(), y - logo3dtiImage.getHeight()/2);
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
    float yRemain = getHeight() - y - 3*margin;
    speakerTree.setBounds(2*margin, y, getWidth() - 3*margin, 0.5*yRemain);
    addSpkButton.setBounds( margin, y, 20, 20 );
    
    // log window
    y = 2*margin + speakerTree.getY() + speakerTree.getHeight();
    logTextBox.setBounds( margin, y, getWidth() - 2*margin, 0.5*yRemain );
    
    // above log window gui elements
    clearSpkButton.setBounds( getWidth() - margin - 40, y - 1.5*margin, 40, 20 );
    ambiOrderComboBox.setBounds( clearSpkButton.getX() - margin - 60, y - 1.5*margin, 60, 20);
    ambiOrderLabel.setBounds( ambiOrderComboBox.getX() - margin - 100, y - 1.5*margin, 100, 20);
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
        else{ logTextBox.setText(""); }
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
        speakerTree.getConfiguration( speakers );
        updateAmbiOrderComboBox();
    }

    // clear all speakers from configuration
    else if( button == &clearSpkButton )
    {
        speakerTree.clear();
        speakerTree.getConfiguration( speakers );
        updateAmbiOrderComboBox();
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
    
    // update available ambi orders
    updateAmbiOrderComboBox();
}

void MainContentComponent::updateAmbiOrderComboBox()
{
    // get max ambisonic order
    int order = getMaxAmbiOrder( speakers.size() );
    
    // need to change available ambisonic orders
    if( ambiOrderComboBox.getNumItems() != order + 1 ){
        ambiOrderComboBox.clear();
        for( int i = 0; i < order+1; i ++ ){
            ambiOrderComboBox.addItem(String(i), i+1);
        }
        // set highest order as selected item
        ambiOrderComboBox.setSelectedId( order + 1, dontSendNotification );
    }
}

// timer based check of current speaker configuration to update available ambisonic order combobox
// had to use this mechanism to "listen" to speaker removals from speaker tree based on local button
// (it was that or burderning the code with addListener(owner.owner.owner...) like call).
void MainContentComponent::timerCallback()
{
    speakerTree.getConfiguration( speakers );
    updateAmbiOrderComboBox();
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
    int maxOrder = getMaxAmbiOrder( speakers.size() );
    
    // get desired ambisonic order
    int order = ambiOrderComboBox.getSelectedId() - 1;
    
    // security to make sure desired order is not above max (should not arise, not be available in combo box)
    order = fmin( maxOrder, order );
    
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
