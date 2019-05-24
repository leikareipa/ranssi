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

#include <QFileInfo>
#include <QDebug>
#include "project/project.h"
#include "common.h"

project_c::project_c(const QString &projectDirectory) :
    name(projectDirectory.split("/").last()),
    filenames({projectDirectory + "/" + name + ".wav" /*WAV file*/,
               projectDirectory + "/" + name + ".txt" /*transcription file*/,}),
    isValid(bool(!projectDirectory.isEmpty() &&
                 QFileInfo(filenames.wavFile).exists() &&
                 QFileInfo(filenames.transcriptionFile).exists()))
{
    return;
}

project_c::~project_c(void)
{
    return;
}
