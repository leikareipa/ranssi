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

#include <vector>
#include "wav/wav.h"

class project_c
{
public:
    project_c(const std::string title, const std::string wavFile);
    ~project_c(void);

    const std::string title;
    const wav_c wav;

private:
};

#endif
