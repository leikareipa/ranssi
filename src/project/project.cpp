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
#include <QDir>
#include "project/project.h"
#include "wav/conversion.h"
#include "common.h"
#include "csv.h"

project_c::project_c(const QString &projectDirectory) :
    name(projectDirectory.split("/").last()),
    filenames({base_file_path(name, projectDirectory) + ".wav" /*WAV file*/,
               base_file_path(name, projectDirectory) + ".txt" /*transcription file*/,
               base_file_path(name, projectDirectory) + ".speakers" /*speaker names*/,}),
    isValid(bool(!projectDirectory.isEmpty() && QFileInfo(filenames.wavFile).exists()))
{
    return;
}

project_c::~project_c(void)
{
    return;
}

// Loads from file the list of speakers associated with this project.
QStringList project_c::speaker_names(void) const
{
    // The speakers file is optional for a project; if the file doesn't exist,
    // we just don't return any names.
    if (!QFileInfo(this->filenames.speakersFile).exists()) return {};

    QStringList speakerNames;

    QList<QStringList> speakerRows = csv_parse_c(this->filenames.speakersFile).rows();
    for (const auto &row: speakerRows)
    {
        speakerNames << row;
    }

    return speakerNames;
}

// The base path of the resource files of a project by the given name. For instance,
// if the project is called "Test", its base file path might be "Test/Test", and
// e.g. its WAV file could be found in "Test/Test.wav". If a non-empty directory
// name is given, this name is used as the path for the base name; such that if the
// given directory is "/home/user/alternative_test", the base path returned will be
// "/home/user/alternative_test/Test".
QString project_c::base_file_path(const QString &projectName, const QString &projectDirectory)
{
    QString path = (projectDirectory.isEmpty()? projectName : projectDirectory);

    if (!path.endsWith("/") ||
        !path.endsWith(QDir::separator()))
    {
        path += "/";
    }

    return (path + projectName);
}

// Creates a new project using the given file as the source of the project's
// audio. The file is assumed to be of a type from which audio can be extracted,
// e.g. a sound file, a video file, etc. (The extraction will likely be done via
// FFmpeg, so anything from which FFmpeg can output a WAV file.) Returns a null
// QString if the creation fails; otherwise a QString pointing to the new
// project's base directory.
//
// Note that the project will be created in a sub-directory of the current
// working directory.
QString project_c::create_project(const QString &sourceFilePath)
{
    if (!QFileInfo(sourceFilePath).exists())
    {
        qCritical() << "Failed to open the source file" << sourceFilePath << "for creating a new project with.";

        return QString();
    }

    const QString projectName = QFileInfo(sourceFilePath).baseName();

    if (QDir(projectName).exists())
    {
        qCritical() << "A project by the name" << projectName << "already exists in the current working directory.";

        return QString();
    }

    if (!QDir().mkdir(projectName))
    {
        qCritical() << "Failed to create a directory for project" << projectName;

        return QString();
    }

    if (!wav_conversion_c::convert_to_wav_file(sourceFilePath, (project_c::base_file_path(projectName) + ".wav")))
    {
        qCritical() << "Failed to convert" << sourceFilePath << "into a ranssi-compatible WAV file.";

        // At this point, we assume that the function would already have returned
        // if a directory by the given name already existed (i.e. was in use for
        // something else); and that such a directory has otherwise been created
        // specifically for this project; and since we failed to complete the
        // creation of the project, we should clean up by removing its directory.
        QDir(projectName).removeRecursively();

        return QString();
    }

    return QDir(projectName).absolutePath();
}
