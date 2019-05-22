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

#ifndef PROJECT_H
#define PROJECT_H

#include <QString>
#include <vector>
#include <map>
#include "wav/wav.h"

class project_c
{
public:
    project_c(const std::string projectName);
    ~project_c(void);

    // The project's display name.
    const std::string name;

    // The audio which this project is to transcribe.
    const wav_c wav;

private:
};

#endif
