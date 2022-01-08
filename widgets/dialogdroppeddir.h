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

#include <QDialog>

namespace Ui {
class DialogDroppedDir;
}

class DialogDroppedDir : public QDialog
{
    Q_OBJECT
public:
    using ParentChildrenPair = QPair<QString, QStringList>;

    DialogDroppedDir(const QList<ParentChildrenPair> &dirs, QWidget *parent = nullptr);
    ~DialogDroppedDir() override;

    QList<ParentChildrenPair> dirsToRename() const;
    QList<ParentChildrenPair> filesToRename() const;

private slots:
    void onPushButtonOkClicked();

private:
    QString fixFiltersString(QStringView filtersString) const;
    QString iniFilePath() const;
    void loadSettings();
    void saveSettings() const;

    Ui::DialogDroppedDir *ui;

    const QList<ParentChildrenPair> m_dirsToSearch;
    QList<ParentChildrenPair> m_dirs;
    QList<ParentChildrenPair> m_files;
};
