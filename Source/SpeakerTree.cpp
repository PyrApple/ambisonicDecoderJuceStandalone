
#include "SpeakerTree.h"

SpeakerTreeComponent::SpeakerTreeComponent( int _id, SpeakerTreeItem & _owner ): owner(_owner)
{
    itemId = _id;
    
    coord1.setText("0.0", dontSendNotification);
    coord1.setEditable(true);
    coord1.setColour(Label::textColourId, colourMain);
    addAndMakeVisible(coord1);
    
    coord2.setText("0.0", dontSendNotification);
    coord2.setEditable(true);
    coord2.setColour(Label::textColourId, colourMain);
    addAndMakeVisible(coord2);
    
    coord3.setText("1.0", dontSendNotification);
    coord3.setEditable(true);
    coord3.setColour(Label::textColourId, colourMain);
    addAndMakeVisible(coord3);
    
    id.setText(String(_id + 1), dontSendNotification);
    id.setEditable(true);
    id.setColour(Label::textColourId, colourMain);
    addAndMakeVisible(id);
    
    convention.setEditableText (true);
    convention.setJustificationType (Justification::centred);
    convention.addItem("aed", 1);
    convention.addItem("xyz", 2);
    convention.setSelectedId( 1, dontSendNotification );
    convention.setColour(ComboBox::outlineColourId, colourBkg);
    convention.setColour(ComboBox::backgroundColourId, colourBkg);
    convention.addListener( this );
    addAndMakeVisible( convention );
    
    rmSpk.setButtonText( "-" );
    rmSpk.addListener( this );
    rmSpk.setColour( TextButton::buttonColourId, colourMain );
    rmSpk.setColour( TextButton::textColourOffId, colourBkg );
    addAndMakeVisible( rmSpk );
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

// get coords from labels
Eigen::Vector3f SpeakerTreeComponent::getCoords()
{
    Eigen::Vector3f coords;
    coords[0] = coord1.getText().getFloatValue();
    coords[1] = coord2.getText().getFloatValue();
    coords[2] = coord3.getText().getFloatValue();
    return coords;
}

// set coords to labels
void SpeakerTreeComponent::setCoords( const Eigen::Vector3f & coords )
{
    coord1.setText( String( rmNearZero(coords[0], 10e-8) ), dontSendNotification );
    coord2.setText( String( coords[1] ), dontSendNotification );
    coord3.setText( String( coords[2] ), dontSendNotification );
}

void SpeakerTreeComponent::getConfiguration( Speaker & spk )
{
    // get coords
    Eigen::Vector3f coords = getCoords();
    
    // convert xyz to aed
    if( convention.getText() == "xyz" ){
        spk.aed = cartesianToSpherical(coords);
    }
    else{
        spk.aed = deg2radVect( coords );
    }
}

void SpeakerTreeComponent::comboBoxChanged (ComboBox *comboBoxThatHasChanged)
{
    if( comboBoxThatHasChanged == &convention ){
        
        // get coords
        Eigen::Vector3f coords = getCoords();
        
        // change coord system
        if( comboBoxThatHasChanged->getText() == "xyz" ){
            coords = sphericalToCartesian( deg2radVect( coords ));
            setCoords( coords );
        }
        else{
            coords = cartesianToSpherical( coords );
            setCoords( rad2degVect( coords ) );
        }
    }
}

void SpeakerTreeComponent::buttonClicked( Button* button )
{
    owner.removeSpkItem();
}


SpeakerTreeItem::SpeakerTreeItem( int _id, SpeakerTreeItemHolder & _owner ): owner(_owner)
{
    id = _id;
}

SpeakerTreeComponent* SpeakerTreeItem::createItemComponent()
{
    component = new SpeakerTreeComponent( id, *this );
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

void SpeakerTreeItemHolder::addSpkItem()
{
    addSubItem( new SpeakerTreeItem( getNumSubItems(), *this ) );
}


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
        SpeakerTreeComponent * component = dynamic_cast <SpeakerTreeComponent *> (item->component);
        component->getConfiguration( speakers[i] );
    }
}

void SpeakerTree::addSpkItem()
{
    SpeakerTreeItemHolder * root = dynamic_cast <SpeakerTreeItemHolder *> (tree.getRootItem());
    root->addSpkItem();
}
