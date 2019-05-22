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

project_c::project_c(const std::string projectTitle) :
    _name(projectTitle)
{
    return;
}

project_c::~project_c(void)
{
    return;
}

std::string project_c::name(void) const
{
    return this->_name;
}

std::string project_c::wav_filename(void) const
{
    return (this->_name + "/" + this->_name + ".wav");
}
