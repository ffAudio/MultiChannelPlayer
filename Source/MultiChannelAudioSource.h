/*
  ==============================================================================

    MultiChannelAudioSource.h
    Created: 8 Jan 2019 10:29:40am
    Author:  daniel -at- foleysfinest.com
    Github:  https://github.com/ffAudio

  ==============================================================================
*/

#pragma once

class MultiChannelAudioSource   : public PositionableAudioSource
{
public:
    MultiChannelAudioSource() = default;

    void setSource (AudioFormatReaderSource* sourceToUse)
    {
        if (sourceToUse == nullptr)
        {
            source.reset();
            return;
        }

        if (auto* reader = sourceToUse->getAudioFormatReader())
        {
            const auto numChannels = reader->getChannelLayout().size();
            buffer.setSize (numChannels, buffer.getNumSamples());
        }
        else
        {
            // Don't give me an invalid AudioFormatReaderSource!
            jassertfalse;
        }

        if (sampleRate > 0)
            sourceToUse->prepareToPlay (buffer.getNumSamples(), sampleRate);

        source.reset (sourceToUse);
    }

    // from AudioSource
    void prepareToPlay (int samplesPerBlockExpected, double sampleRateToUse) override
    {
        sampleRate = sampleRateToUse;

        if (source)
        {
            if (auto* reader = source->getAudioFormatReader())
                buffer.setSize(reader->getChannelLayout().size(), samplesPerBlockExpected);
            else
            {
                // Don't give me an invalid AudioFormatReaderSource!
                jassertfalse;
            }

            source->prepareToPlay (samplesPerBlockExpected, sampleRate);
        }
        else
        {
            buffer.setSize (2, samplesPerBlockExpected);
        }
    }

    void releaseResources() override
    {
        if (source)
            source->releaseResources();
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        if (source.get() == nullptr)
        {
            bufferToFill.clearActiveBufferRegion();
            return;
        }

        const auto numSamples = bufferToFill.numSamples;

        AudioSourceChannelInfo originalToFill { &buffer, 0, numSamples };
        source->getNextAudioBlock (originalToFill);

        // Do the downmix here, or even better, aggregate your matrix, so you can select different methods on the fly
        const auto numInChannels = buffer.getNumChannels();
        const auto numOutChannels = bufferToFill.buffer->getNumChannels();

        const auto inChannelFormat = (source->getAudioFormatReader()) ? source->getAudioFormatReader()->getChannelLayout() :
                                                                        AudioChannelSet::canonicalChannelSet (numInChannels);

        bufferToFill.clearActiveBufferRegion();

        for (int c = 0; c < numInChannels; ++c)
        {
            switch (inChannelFormat.getTypeOfChannel (c)) {
                case AudioChannelSet::left:
                    bufferToFill.buffer->addFrom (0, bufferToFill.startSample, buffer, c, 0, numSamples, 0.7);
                    break;
                case AudioChannelSet::right:
                    bufferToFill.buffer->addFrom (1, bufferToFill.startSample, buffer, c, 0, numSamples, 0.7);
                    break;
                case AudioChannelSet::centre:
                    bufferToFill.buffer->addFrom (0, bufferToFill.startSample, buffer, c, 0, numSamples, 0.7);
                    bufferToFill.buffer->addFrom (1, bufferToFill.startSample, buffer, c, 0, numSamples, 0.7);
                    break;
                case AudioChannelSet::LFE:
                case AudioChannelSet::LFE2:
                    bufferToFill.buffer->addFrom (0, bufferToFill.startSample, buffer, c, 0, numSamples, 0.7);
                    bufferToFill.buffer->addFrom (1, bufferToFill.startSample, buffer, c, 0, numSamples, 0.7);
                    break;
                default:
                    // assume surround for all others
                    bufferToFill.buffer->addFrom (0, bufferToFill.startSample, buffer, c, 0, numSamples, 0.4);
                    bufferToFill.buffer->addFrom (1, bufferToFill.startSample, buffer, c, 0, numSamples, -0.4);
                    break;
            }
        }
    }

    // from PositionableAudioSource
    void setNextReadPosition (int64 newPosition) override
    {
        if (source)
            source->setNextReadPosition (newPosition);
    }

    int64 getNextReadPosition() const override
    {
        if (source)
            return source->getNextReadPosition();

        return 0;
    }

    int64 getTotalLength() const override
    {
        if (source)
            return source->getTotalLength();

        return 0;
    }

    bool isLooping() const override
    {
        return false;
    }

private:
    double sampleRate = 0;

    std::unique_ptr<AudioFormatReaderSource> source;

    AudioBuffer<float> buffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiChannelAudioSource)
};
