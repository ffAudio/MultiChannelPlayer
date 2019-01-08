/*
  ==============================================================================

    The MainComponent

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    formatManager.registerBasicFormats();

    player.setSource (&transport);
    transport.setSource (&audioSource);

    deviceManager.initialiseWithDefaultDevices (0, 2);

    if (auto* device = deviceManager.getCurrentAudioDevice())
    {
        player.prepareToPlay (device->getCurrentSampleRate(), device->getDefaultBufferSize());
    }

    deviceManager.addAudioCallback (&player);

    addAndMakeVisible (load);
    addAndMakeVisible (stop);
    addAndMakeVisible (play);
    addAndMakeVisible (settings);
    addChildComponent (selector);

    stop.onClick = [this]{ transport.stop(); };
    play.onClick = [this]{ transport.start(); };
    load.onClick = [this]
    {
        FileBrowserComponent browser (FileBrowserComponent::canSelectFiles | FileBrowserComponent::openMode,
                                      File(),
                                      nullptr,
                                      nullptr);

        FileChooserDialogBox dialogBox ("Open some kind of file",
                                        "Please choose some kind of file that you want to open...",
                                        browser,
                                        false,
                                        Colours::lightgrey);
        if (dialogBox.show())
        {
            auto selected = browser.getSelectedFile (0);
            if (! selected.existsAsFile())
                return;

            auto* reader = formatManager.createReaderFor (selected);
            if (reader == nullptr)
            {
                AlertWindow::showMessageBox (AlertWindow::WarningIcon, "Cannot read File", "The selected file \"" + selected.getFullPathName() + "\" could not be read");
                return;
            }

            audioSource.setSource (new AudioFormatReaderSource (reader, true));
        }
    };

    settings.onClick = [this]
    {
        selector.setVisible (! selector.isVisible());
    };

    setSize (800, 600);
}

MainComponent::~MainComponent()
{
    deviceManager.removeAudioCallback (&player);
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced (5);
    auto buttonArea = area.removeFromTop (30);

    load.setBounds (buttonArea.removeFromLeft (100));
    stop.setBounds (buttonArea.removeFromLeft (100));
    play.setBounds (buttonArea.removeFromLeft (100));
    settings.setBounds (buttonArea.removeFromRight (100));

    selector.setBounds (area);
}
