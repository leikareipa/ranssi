/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 * Manages projects.
 *
 * In ranssi, a project consists of audio and its corresponding, user-produced
 * transcription.
 *
 */

#include "project/project.h"

project_c::project_c(const std::string title, const std::string wavFile) :
    title(title),
    wav(wav_c(wavFile))
{
    return;
}

project_c::~project_c()
{
    return;
}
