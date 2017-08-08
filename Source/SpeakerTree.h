#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Utils.h"

// forward declaration
class SpeakerTree;
class SpeakerTreeItem;
class SpeakerTreeItemHolder;

class SpeakerTreeComponent :
public Component,
public Button::Listener,
public Label::Listener,
public ComboBox::Listener
{
    
private:
    
    Label id, coord1, coord2, coord3;
    ComboBox convention;
    TextButton rmSpk;
    SpeakerTreeItem & owner;
    
public:
    
    SpeakerTreeComponent( SpeakerTreeItem & _owner );
    void resized() override;
    void paint (Graphics& g) override;
    
    void comboBoxChanged (ComboBox *comboBoxThatHasChanged) override;
    void labelTextChanged (Label *labelThatHasChanged) override;
    void buttonClicked( Button* button ) override;
    
    void setCoords( const Eigen::Vector3f & coords );
    Eigen::Vector3f getCoords();
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpeakerTreeComponent)
    
};


class SpeakerTreeItem :
public TreeViewItem
{
    
public:
    
    SpeakerTreeItemHolder & owner;
    SpeakerTreeComponent * component;
    Speaker speaker;
    String convention;
    
    SpeakerTreeItem( const Speaker & speaker, SpeakerTreeItemHolder & _owner );
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
    void addSpkItem( Speaker & speaker );
    
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
    void setConfiguration( std::vector<Speaker> & speakers );
    void addSpkItem();
    void clear();
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpeakerTree)
};
