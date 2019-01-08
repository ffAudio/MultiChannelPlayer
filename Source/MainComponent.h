/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "MultiChannelAudioSource.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    AudioDeviceManager      deviceManager;
    AudioFormatManager      formatManager;

    MultiChannelAudioSource audioSource;
    AudioTransportSource    transport;
    AudioSourcePlayer       player;

    TextButton load     { "Load" };
    TextButton stop     { "Stop" };
    TextButton play     { "Play" };
    TextButton settings { "Settings" };

    AudioDeviceSelectorComponent selector
    {
        deviceManager, 0, 0, 2, 8,
        false, false, true, false
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
