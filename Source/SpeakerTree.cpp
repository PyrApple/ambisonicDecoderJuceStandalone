
#include "SpeakerTree.h"

SpeakerTreeComponent::SpeakerTreeComponent( SpeakerTreeItem & _owner ): owner(_owner)
{
    
    coord1.setEditable(true);
    coord1.setColour(Label::textColourId, colourMain);
    addAndMakeVisible(coord1);
    coord1.addListener( this );
    
    coord2.setEditable(true);
    coord2.setColour(Label::textColourId, colourMain);
    addAndMakeVisible(coord2);
    coord2.addListener( this );
    
    coord3.setEditable(true);
    coord3.setColour(Label::textColourId, colourMain);
    addAndMakeVisible(coord3);
    coord3.addListener( this );
    
    id.setEditable(true);
    id.setColour(Label::textColourId, colourMain);
    addAndMakeVisible(id);
    id.addListener( this );
    
    convention.setEditableText( false );
    convention.setJustificationType (Justification::centred);
    convention.addItem("aed", 1);
    convention.addItem("xyz", 2);
    convention.setColour(ComboBox::outlineColourId, colourBkg);
    convention.setColour(ComboBox::backgroundColourId, colourBkg);
    convention.addListener( this );
    addAndMakeVisible( convention );
    
    rmSpk.setButtonText( "-" );
    rmSpk.setColour( TextButton::buttonColourId, colourMain );
    rmSpk.setColour( TextButton::textColourOffId, colourBkg );
    addAndMakeVisible( rmSpk );
    rmSpk.addListener( this );
    
    // update from owner values
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
    int offset = 60 + margin;
    id.setBounds(offset, 0, w, h);
    offset += w + margin;
    convention.setBounds(offset, 0, w, h);
    offset += 1.5*w + margin;
    coord1.setBounds(offset, 0, w, h);
    offset += w + margin;
    coord2.setBounds(offset, 0, w, h);
    offset += w + margin;
    coord3.setBounds(offset, 0, w, h);
    rmSpk.setBounds(getWidth()-20-margin, 0, 20, 20);
}

void SpeakerTreeComponent::paint (Graphics& g)
{
    g.setColour(colourMain);
    g.drawText("speaker", 0, 0, 60, 20, Justification::left);
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

void SpeakerTreeComponent::buttonClicked( Button* button )
{
    owner.removeSpkItem();
}

void SpeakerTreeComponent::comboBoxChanged (ComboBox *comboBoxThatHasChanged)
{
    // get coords
    Eigen::Vector3f coords = getCoords();

    // update locals: change coord system
    if( comboBoxThatHasChanged->getText() == "xyz" ){
        coords = sphericalToCartesian( deg2radVect( coords ));
        setCoords( coords );
    }
    else{
        coords = cartesianToSpherical( coords );
        setCoords( rad2degVect( coords ) );
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
    std::cout << coords << std::endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

SpeakerTreeItem::SpeakerTreeItem( const Speaker & _speaker, SpeakerTreeItemHolder & _owner ): owner(_owner)
{
    speaker = _speaker;
    convention = "aed";
}

SpeakerTreeComponent* SpeakerTreeItem::createItemComponent()
{
    component = new SpeakerTreeComponent( *this );
    return component;
}

bool SpeakerTreeItem::mightContainSubItems()
{
    return getNumSubItems() != 0;
}

void SpeakerTreeItem::removeSpkItem()
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

void SpeakerTreeItemHolder::removeSpkItem( int itemId )
{
    removeSubItem( itemId );
}

void SpeakerTreeItemHolder::addSpkItem( Speaker & speaker )
{
    speaker.id = getNumSubItems();
    addSubItem( new SpeakerTreeItem( speaker, *this ) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////

SpeakerTree::SpeakerTree()
{
    addAndMakeVisible(tree);
    tree.setRootItem(new SpeakerTreeItemHolder());
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

void SpeakerTree::setConfiguration( std::vector<Speaker> & speakers )
{
    // get tree root
    SpeakerTreeItemHolder * root = dynamic_cast <SpeakerTreeItemHolder *> (tree.getRootItem());
    
    // loop over speakers and add leaves to tree
    for( int i = 0; i < speakers.size(); i++ ){
        root->addSpkItem( speakers[i] );
    }
}

void SpeakerTree::addSpkItem()
{
    SpeakerTreeItemHolder * root = dynamic_cast <SpeakerTreeItemHolder *> (tree.getRootItem());
    Speaker spk = { 0, Eigen::Vector3f (0,0,1) };
    root->addSpkItem( spk );
}

void SpeakerTree::clear()
{
    SpeakerTreeItemHolder * root = dynamic_cast <SpeakerTreeItemHolder *> (tree.getRootItem());
    root->clearSubItems();
}

