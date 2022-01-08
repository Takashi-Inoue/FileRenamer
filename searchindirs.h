/*
 * Copyright 2021 Takashi Inoue
 *
 * This file is part of FileRenamer.
 *
 * FileRenamer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FileRenamer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FileRenamer.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <QStringList>

class QDir;

class SearchInDirs
{
public:
    using ParentChildrenPair = QPair<QString, QStringList>;

    struct Settings {
        bool isSearchDirs;
        bool isSearchFiles;
        int hierarchy;
        QStringList filters;
    };

    SearchInDirs(const Settings &settings);

    QList<ParentChildrenPair> dirs() const;
    QList<ParentChildrenPair> files() const;

    void exec(QList<ParentChildrenPair> targetDirs);

private:
    void searchForDirs(const QDir &parentDir, QList<ParentChildrenPair> &targetDirs);
    void searchForFiles(const QDir &parentDir);
    void searchOneLayer(QList<ParentChildrenPair> &targetDirs, const QStringList &nameFilters);
    QString addSeparator(QStringView dirPath);

    const Settings m_settings;

    QList<ParentChildrenPair> m_dirs;
    QList<ParentChildrenPair> m_files;
};
