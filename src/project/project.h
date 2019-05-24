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
    project_c(const QString &projectDirectory);
    ~project_c(void);

    // The project's display name.
    const QString name;

    // Filenames of the project's resource files. The filenames include a path,
    // which is relative to the executable's working directory.
    struct filenames_s
    {
        // Name of the file containing the audio to be transcribed in this project.
        const QString wavFile;

        // Name of the file containing the user's transcription (so far) of the
        // project's audio. Note that this file only gets updated when the user
        // explicitly saves their transcription to disk, not when the text has
        // been typed into the GUI's text editor but has not yet been exported
        // to disk.
        const QString transcriptionFile;
    } filenames;

    // True if this is a valid project; false otherwise. A project might get
    // flagged as invalid if, for instance, some of its resource files couldn't
    // be found when the project was initialized.
    const bool isValid;

private:
};

#endif
