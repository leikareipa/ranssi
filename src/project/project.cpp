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
    filenames({base_file_name() + ".wav" /*WAV file*/,
               base_file_name() + ".txt" /*transcription file*/,})
{
    return;
}

project_c::~project_c(void)
{
    return;
}

// The names of a project's files are expected to derive from the name of the
// project and to be stored in a directory named likewise; such that e.g. a
// project named "Test" will have filenames beginning with "Test/Test", where
// e.g. the WAV file would be "Test/Test.wav". The path is relative to the
// executable's working directory.
std::string project_c::base_file_name(void) const
{
    return (this->name + "/" + this->name);
}
