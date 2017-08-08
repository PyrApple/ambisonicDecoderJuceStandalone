#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Utils.h"

// forward declaration
class SpeakerTreeItem;
class SpeakerTreeItemHolder;

//////////////////////////////////////////////////////////////////////////////////////////////////

class SpeakerTreeComponent :
public Component,
public Label::Listener,
public ComboBox::Listener
{
    
private:
    
    std::unordered_map< Label*, std::string > labelMap;
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
    
    void setCoords( const Eigen::Vector3f & coords );
    Eigen::Vector3f getCoords();
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpeakerTreeComponent)
    
};

//////////////////////////////////////////////////////////////////////////////////////////////////

class SpeakerTreeItem :
public TreeViewItem,
public Button::Listener
{
    
public:
    
    SpeakerTreeItemHolder & owner;
    Speaker speaker;
    String convention;
    
    SpeakerTreeItem( const Speaker & speaker, SpeakerTreeItemHolder & _owner );
    
    SpeakerTreeComponent* createItemComponent() override;
    void buttonClicked( Button* button ) override;
    bool mightContainSubItems() override;
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpeakerTreeItem)
    
};

//////////////////////////////////////////////////////////////////////////////////////////////////

class SpeakerTreeItemHolder: public TreeViewItem
{
    
public:
    
    SpeakerTreeItemHolder();
    
    void paintItem(Graphics& g, int width, int height) override;
    bool mightContainSubItems() override;
    
    void addSpkItem( const Speaker & speaker, bool overwriteId );
    void removeSpkItem( int itemId );
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpeakerTreeItemHolder)
};

//////////////////////////////////////////////////////////////////////////////////////////////////

class SpeakerTree: public Component
{
    
public:
    
    TreeView tree;
    
    SpeakerTree();
    ~SpeakerTree();
    
    void resized() override;
    
    void getConfiguration( std::vector<Speaker> & speakers );
    void setConfiguration( const std::vector<Speaker> & speakers );
    void addSpkItem();
    void clear();
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpeakerTree)
};
