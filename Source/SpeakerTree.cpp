
#include "SpeakerTree.h"

SpeakerTreeComponent::SpeakerTreeComponent( SpeakerTreeItem & _owner ): owner(_owner)
{
    // init labels
    labelMap.insert({
        { &coord1, "0.0" },
        { &coord2, "0.0" },
        { &coord3, "0.0" },
        { &id, "0" }
    });
    for( auto& pair : labelMap )
    {
        pair.first->setColour( TextButton::textColourOffId, colourBkg );
        pair.first->setText(pair.second, dontSendNotification);
        pair.first->setEditable( true );
        pair.first->addListener( this );
        addAndMakeVisible( pair.first );
    }
    
    // init convention combo box
    convention.setEditableText( false );
    convention.setJustificationType (Justification::centred);
    convention.addItem("aed", 1);
    convention.addItem("xyz", 2);
    convention.setColour(ComboBox::outlineColourId, colourBkg);
    convention.setColour(ComboBox::backgroundColourId, colourBkg);
    convention.addListener( this );
    addAndMakeVisible( convention );
    
    // init rm speaker button
    rmSpk.setButtonText( "-" );
    rmSpk.setColour( TextButton::buttonColourId, colourMain );
    rmSpk.setColour( TextButton::textColourOffId, colourBkg );
    addAndMakeVisible( rmSpk );
    rmSpk.addListener( &owner );
    
    // update fields from owner values
    id.setText(String(owner.speaker.id), dontSendNotification);
    if( owner.convention == "aed" ){
        convention.setSelectedId( 1, dontSendNotification );
        setCoords( rad2degVect( owner.speaker.aed ) );
    }
    else{
        convention.setSelectedId( 2, dontSendNotification );
        setCoords( sphericalToCartesian( owner.speaker.aed ) );
    }
}

void SpeakerTreeComponent::resized()
{
    int w = 70;
    int h = 20;
    int margin = 10;
    int offset = margin;
    id.setBounds(offset, 0, w, h);
    offset += w + margin;
    convention.setBounds(offset, 0, w, h);
    offset += 1.5*w + margin;
    coord1.setBounds(offset, 0, w, h);
    offset += w + margin;
    coord2.setBounds(offset, 0, w, h);
    offset += w + margin;
    coord3.setBounds(offset, 0, w, h);
    rmSpk.setBounds(getWidth()-20-margin, 0, 20, 17);
}

void SpeakerTreeComponent::paint (Graphics& g)
{
    g.setColour(colourMain);
}

void SpeakerTreeComponent::comboBoxChanged (ComboBox *comboBoxThatHasChanged)
{
    // get coords
    Eigen::Vector3f coords = getCoords();

    // update locals: change coord system
    if( comboBoxThatHasChanged->getText() == "xyz" )
    {
        setCoords( sphericalToCartesian( deg2radVect( coords )) );
    }
    else{
        setCoords( rad2degVect( cartesianToSpherical( coords ) ) );
    }
    
    // update owner
    owner.convention = comboBoxThatHasChanged->getText();
}

void SpeakerTreeComponent::labelTextChanged (Label *labelThatHasChanged)
{
    // update owner id
    owner.speaker.id = id.getText().getIntValue();
    
    // update owner coord
    Eigen::Vector3f coords = getCoords();
    if( convention.getText() == "xyz" ){ coords = cartesianToSpherical(coords); }
    else{ coords = deg2radVect( coords ); }
    owner.speaker.aed = coords;
}

// set coords to labels
void SpeakerTreeComponent::setCoords( const Eigen::Vector3f & coords )
{
    coord1.setText( String( rmNearZero(coords[0]) ), dontSendNotification );
    coord2.setText( String( rmNearZero(coords[1]) ), dontSendNotification );
    coord3.setText( String( rmNearZero(coords[2]) ), dontSendNotification );
}

// get coords from label
Eigen::Vector3f SpeakerTreeComponent::getCoords()
{
    Eigen::Vector3f coords;
    coords[0] = coord1.getText().getFloatValue();
    coords[1] = coord2.getText().getFloatValue();
    coords[2] = coord3.getText().getFloatValue();
    return coords;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

SpeakerTreeItem::SpeakerTreeItem( const Speaker & _speaker, const String & _convention, SpeakerTreeItemHolder & _owner ):
owner(_owner)
{
    speaker = _speaker;
    convention = _convention;
}

SpeakerTreeComponent* SpeakerTreeItem::createItemComponent()
{
    return new SpeakerTreeComponent( *this );
}

bool SpeakerTreeItem::mightContainSubItems()
{
    return getNumSubItems() != 0;
}

void SpeakerTreeItem::buttonClicked( Button* button )
{
    owner.removeSpkItem( getRowNumberInTree() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////

SpeakerTreeItemHolder::SpeakerTreeItemHolder()
{
}

void SpeakerTreeItemHolder::paintItem(Graphics& g, int width, int height)
{
}

bool SpeakerTreeItemHolder::mightContainSubItems()
{
    return getNumSubItems() != 0;
}

void SpeakerTreeItemHolder::addSpkItem( const Speaker & speaker, const String & convention, bool overwriteId )
{
    Speaker spk = { speaker.id, speaker.aed };
    if( overwriteId ){ spk.id = getNumSubItems(); }
    addSubItem( new SpeakerTreeItem( spk, convention, *this ) );
}

void SpeakerTreeItemHolder::removeSpkItem( int itemId )
{
    removeSubItem( itemId );
}

//////////////////////////////////////////////////////////////////////////////////////////////////

SpeakerTree::SpeakerTree()
{
    addAndMakeVisible(tree);
    tree.setRootItem( new SpeakerTreeItemHolder() );
    tree.setRootItemVisible(false);
}

SpeakerTree::~SpeakerTree()
{
    tree.deleteRootItem(); // this deletes the children too...
}

void SpeakerTree::resized() { tree.setBounds(getLocalBounds()); }

void SpeakerTree::getConfiguration( std::vector<Speaker> & speakers )
{
    // resize input speaker array
    auto root = tree.getRootItem();
    speakers.resize( root->getNumSubItems() );
    
    // loop over tree nodes to get speakers configurations
    for( int i = 0; i < root->getNumSubItems(); i++ ){
        SpeakerTreeItem * item = dynamic_cast <SpeakerTreeItem *> (root->getSubItem(i));
        speakers[i] = item->speaker;
    }
}

void SpeakerTree::setConfiguration( const std::vector<Speaker> & speakers )
{
    // get tree root
    SpeakerTreeItemHolder * root = dynamic_cast <SpeakerTreeItemHolder *> (tree.getRootItem());
    
    // loop over speakers and add leaves to tree
    String convention = "aed";
    for( int i = 0; i < speakers.size(); i++ ){
        root->addSpkItem( speakers[i], convention, false );
    }
}

void SpeakerTree::addSpkItem()
{
    SpeakerTreeItemHolder * root = dynamic_cast <SpeakerTreeItemHolder *> (tree.getRootItem());
    // define new speaker
    Speaker spk = { 0, Eigen::Vector3f (0,0,1) };
    String convention = "aed"; // default convention
    // get last speaker convention / coord
    if( root->getNumSubItems() > 0 ){
        SpeakerTreeItem * item = dynamic_cast <SpeakerTreeItem *> (root->getSubItem( root->getNumSubItems() - 1 ));
        convention = item->convention;
        spk = { 0, item->speaker.aed };
    }
    // add speaker to tree
    root->addSpkItem( spk, convention, true );
}

void SpeakerTree::clear()
{
    SpeakerTreeItemHolder * root = dynamic_cast <SpeakerTreeItemHolder *> (tree.getRootItem());
    root->clearSubItems();
}

