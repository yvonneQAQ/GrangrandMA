#include <JuceHeader.h>
#include <juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h>

namespace
{
class GrangrandMAStandaloneApp final : public juce::JUCEApplication
{
public:
    GrangrandMAStandaloneApp()
    {
        juce::PropertiesFile::Options options;
        options.applicationName = JucePlugin_Name;
        options.filenameSuffix = ".settings";
        options.osxLibrarySubFolder = "Application Support";
        appProperties.setStorageParameters(options);
    }

    const juce::String getApplicationName() override { return JucePlugin_Name; }
    const juce::String getApplicationVersion() override { return JucePlugin_VersionString; }
    bool moreThanOneInstanceAllowed() override { return true; }
    void anotherInstanceStarted(const juce::String&) override {}

    void initialise(const juce::String&) override
    {
        auto holder = std::make_unique<juce::StandalonePluginHolder>(
            appProperties.getUserSettings(), false);

        // JUCE deliberately mutes standalone input by default to avoid acoustic
        // feedback. This app is specifically a live microphone processor, so
        // opt in to input immediately. Headphones are strongly recommended.
        holder->getMuteInputValue().setValue(false);

        mainWindow = std::make_unique<juce::StandaloneFilterWindow>(
            getApplicationName(),
            juce::LookAndFeel::getDefaultLookAndFeel().findColour(
                juce::ResizableWindow::backgroundColourId),
            std::move(holder));

        mainWindow->setVisible(true);
    }

    void shutdown() override
    {
        if (mainWindow != nullptr)
            mainWindow->getPluginHolder()->savePluginState();

        mainWindow = nullptr;
        appProperties.saveIfNeeded();
    }

    void systemRequestedQuit() override { quit(); }

private:
    juce::ApplicationProperties appProperties;
    std::unique_ptr<juce::StandaloneFilterWindow> mainWindow;
};
}

JUCE_CREATE_APPLICATION_DEFINE(GrangrandMAStandaloneApp)
