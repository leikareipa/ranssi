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

#include <QDebug>
#include "project/project.h"
#include "common.h"

project_c::project_c(const std::string projectName) :
    name(projectName),
    wav(wav_c(name + "/" + name + ".wav"))
{
    return;
}

project_c::~project_c(void)
{
    return;
}
