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
    project_c(const std::string projectTitle);
    ~project_c(void);

    // The project's display name.
    std::string name(void) const;

    // The filename (and relative path) of the project's WAV file. The path is
    // relative to the executable's working directory.
    std::string wav_filename(void) const;

private:
    // The project's display name.
    const std::string _name;
};

#endif
