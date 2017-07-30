#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Utils.h"

// forward declaration
class SpeakerTree;
class SpeakerTreeItem;
class SpeakerTreeItemHolder;

class SpeakerTreeComponent :
public Component,
public ComboBox::Listener,
public Button::Listener
{
    
private:
    
    Label coord1;
    Label coord2;
    Label coord3;
    Label id;
    ComboBox convention;
    TextButton rmSpk;
    int itemId;
    SpeakerTreeItem & owner;
    
public:
    
    SpeakerTreeComponent( int _id, SpeakerTreeItem & _owner );
    void resized() override;
    void paint (Graphics& g) override;
    
    // get coords from labels
    Eigen::Vector3f getCoords();
    
    // set coords to labels
    void setCoords( const Eigen::Vector3f & coords );
    void getConfiguration( Speaker & spk );
    void comboBoxChanged (ComboBox *comboBoxThatHasChanged) override;
    void buttonClicked( Button* button ) override;
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpeakerTreeComponent)
    
};


class SpeakerTreeItem: public TreeViewItem
{
    
public:
    
    SpeakerTreeItemHolder & owner;
    SpeakerTreeComponent * component;
    int id;
    
    SpeakerTreeItem( int _id, SpeakerTreeItemHolder & _owner );
    SpeakerTreeComponent* createItemComponent() override;
    bool mightContainSubItems() override;
    void removeSpkItem();
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpeakerTreeItem)
    
};


class SpeakerTreeItemHolder: public TreeViewItem
{
    
public:
    
    SpeakerTreeItemHolder();
    void paintItem(Graphics& g, int width, int height) override;
    bool mightContainSubItems() override;
    void removeSpkItem( int itemId );
    void addSpkItem();
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpeakerTreeItemHolder)
};


class SpeakerTree: public Component
{
    
public:
    
    TreeView tree;
    
    SpeakerTree();
    ~SpeakerTree();
    void resized() override;
    void getConfiguration( std::vector<Speaker> & speakers );
    void addSpkItem();
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpeakerTree)
};
