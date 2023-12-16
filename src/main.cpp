#include "platform.hpp"
#include <imgui.h>
#include <stdio.h>

struct WavData {
    s32 fileSize;
    s32 type;
    s32 channels;
    s32 sampleRate;
    s32 bitsPerSample;
    s32 dataSize;
};

void printWavInfo(const char* filePath) {
    WavData data {};

    FILE* file = fopen(filePath, "rb");

    // Check to make sure this is a RIFF file
    {
        char buffer[4] {};
        size_t result = fread(buffer, sizeof(char), 4, file);
        if (result == EOF || !strcmp(buffer, "RIFF")) {
            printf("Not a valid WAV file! No RIFF id.");
            return;
        }
    }

    fread(&data.fileSize, 4, 1, file);

    // Check to make sure this is a WAVE file.
    {
        char buffer[4] {};
        size_t result = fread(buffer, sizeof(char), 4, file);
        if (result == EOF || !strcmp(buffer, "WAVE")) {
            printf("Not a valid WAV file! No WAVE id.");
            return;
        }
    }

    while (!feof(file)) {
        char buffer[5] {};
        size_t size {};
        fread(buffer, sizeof(char), 4, file);
        buffer[4] = '\0';
        size_t result = fread(&size, 4, 1, file);

        if (result <= 0) {
            break;
        }

        if (strcmp(buffer, "fmt ") == 0) {
            // handle fmt section
            fread(&data.type, 2, 1, file);
            fread(&data.channels, 2, 1, file);
            fread(&data.sampleRate, 4, 1, file);
            fseek(file, 4, SEEK_CUR); // unused
            fseek(file, 2, SEEK_CUR); // unused
            fread(&data.bitsPerSample, 2, 1, file);
        }
        else if (strcmp(buffer, "data") == 0) {
            // handle data section
            data.dataSize = size;
            fseek(file, size, SEEK_CUR); // for now, skip the data.
        }
        else {
            // unsupported section - ignore it
            fseek(file, size, SEEK_CUR);
        }
    }

    fclose(file);

    printf("fileSize: %i\n", data.fileSize);
    printf("type: %i\n", data.type);
    printf("channels: %i\n", data.channels);
    printf("sampleRate: %i\n", data.sampleRate);
    printf("bitsPerSample: %i\n", data.bitsPerSample);
    printf("dataSize: %i\n", data.dataSize);

    s32 top = (data.dataSize - 44) * 8;
    s32 bot = data.sampleRate * (data.channels * data.bitsPerSample);
    f32 seconds = static_cast<f32>(top) / bot;
    f32 minutes = seconds / 60.0f;
    printf("duration: %f minutes (%f seconds)\n", minutes, seconds);
}

int main() {
    Platform::init();

    printWavInfo("ambiencefun.wav");

    while (!Platform::wantsToQuit()) {
        Platform::handleEvents();
        Platform::Renderer::startFrame();

        ImGui::ShowDemoWindow();

        static char wavFilePath[500] {};
        ImGui::InputText("WAV File Path", wavFilePath, 500);
        if (ImGui::Button("Print WAV Info")) {
            printWavInfo(wavFilePath);
        }

        Platform::Renderer::endFrame();
    }

    Platform::free();
    return 0;
}