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

#include "pathtableviewmenu.h"

#include "pathmodel.h"

#include <QAbstractItemView>
#include <QFileInfo>
#include <QMetaEnum>
#include <QModelIndex>

namespace {
constexpr int sectionTextWidth = 300;
}

PathTableViewMenu::PathTableViewMenu(QAbstractItemView *view)
    : QMenu(view),
      m_view(view)
{
    Q_CHECK_PTR(view);

    createActions();
    createMenu();
}

void PathTableViewMenu::addActionsToWidget(QWidget *widget) const
{
    widget->addActions(m_actions.values());
}

void PathTableViewMenu::setEnabledActions(Action action, bool isEnable)
{
    m_actions[action]->setEnabled(isEnable);
}

void PathTableViewMenu::updateActions()
{
    auto pathModel = qobject_cast<PathModel*>(m_view->model());

    Q_CHECK_PTR(pathModel);

    using HSection = PathModel::HSection;
    const QModelIndex currentIndex = m_view->currentIndex();

    const auto currentSection = HSection(currentIndex.column());

    const QString copyNameText((currentSection == HSection::Path) ? QStringLiteral("Copy path")
                                                                  : QStringLiteral("Copy name"));

    m_actions[Action::CopyName]->setText(copyNameText);

    QFileInfo newPathInfo(pathModel->fullPath(currentIndex.row(), HSection::NewName));
    const bool isNewPathExists = newPathInfo.exists();

    m_actions[Action::OpenMulti]->setEnabled(isNewPathExists);

    bool isDir = true;

    if (currentSection == HSection::NewName) {
        isDir = isNewPathExists ? newPathInfo.isDir()
                                : pathModel->isDir(currentIndex.row());
        m_actions[Action::OpenPath]->setEnabled(isNewPathExists);
        m_actions[Action::DeletePath]->setEnabled(isNewPathExists);
    } else {
        m_actions[Action::OpenPath]->setEnabled(true);

        bool isCurrentSectionOriginalName = currentSection == HSection::OriginalName;

        m_actions[Action::DeletePath]->setEnabled(isCurrentSectionOriginalName);

        if (isCurrentSectionOriginalName)
            isDir = pathModel->isDir(currentIndex.row());
    }

    const QString typeText = isDir ? QStringLiteral("directory")
                                   : QStringLiteral("file");

    const QString openPathText = QStringLiteral("Open %1").arg(typeText);
    const QString deletePathText = QStringLiteral("Delete %1").arg(typeText);

    m_actions[Action::OpenPath]->setText(openPathText);
    m_actions[Action::DeletePath]->setText(deletePathText);

    const QString originalName = pathModel->originalName(currentIndex.row());
    const QString newName = pathModel->newName(currentIndex.row());

    m_actions[Action::OpenMulti]->setText(QStringLiteral("Open both files/dirs"));

    adjustSize();

    if (width() < sectionTextWidth)
        setMinimumWidth(sectionTextWidth + 20);

    const QString sectionNameText(pathModel->data(currentIndex).toString());
    const QString sectionBothText(QStringLiteral("%1 | %2").arg(originalName, newName));
    const QFont &sectionFont(m_sections[Section::FileName]->font());

    m_sections[Section::FileName]->setText(elide(sectionNameText, sectionFont, sectionTextWidth));
    m_sections[Section::MultiFiles]->setText(elide(sectionBothText, sectionFont, sectionTextWidth));
}

void PathTableViewMenu::createActions()
{
    const QHash<Action, QString> texts = {
        {Action::RemoveItem, QStringLiteral("Remove from list(&R)")}
      , {Action::CopyName,   QStringLiteral("Copy name(&C)")}
      , {Action::OpenPath,   QStringLiteral("Open file(&O)")}
      , {Action::DeletePath, QStringLiteral("Delete file(&D)")}
      , {Action::OpenMulti , QStringLiteral("Open both files(&B)")}
    };

    const QHash<Action, QIcon> icons = {
        {Action::RemoveItem, QIcon(QStringLiteral(":/x.svg"))}
      , {Action::CopyName,   QIcon(QStringLiteral(":/res/images/file.svg"))}
      , {Action::OpenPath,   QIcon(QStringLiteral(":/res/images/exec.svg"))}
      , {Action::DeletePath, QIcon(QStringLiteral(":/res/images/delete_file.svg"))}
      , {Action::OpenMulti , QIcon(QStringLiteral(":/res/images/exec_double.svg"))}
    };

    const QHash<Action, QKeySequence> shortcuts = {
        {Action::RemoveItem, QKeySequence::Delete}
      , {Action::CopyName,   QKeySequence::Copy}
      , {Action::OpenPath,   QKeySequence::Open}
      , {Action::DeletePath, QKeySequence()}
      , {Action::OpenMulti , QStringLiteral("Ctrl+Shift+O")}
    };

    const QHash<Action, QString> signalNames = {
        {Action::RemoveItem, QStringLiteral("requestRemoveItem()")}
      , {Action::CopyName,   QStringLiteral("requestCopyName()")}
      , {Action::OpenPath,   QStringLiteral("requestOpenPath()")}
      , {Action::DeletePath, QStringLiteral("requestDeletePath()")}
      , {Action::OpenMulti , QStringLiteral("requestOpenMulti()")}
    };

    QMetaEnum enumActions = QMetaEnum::fromType<Action>();

    for (int i = 0, count = enumActions.keyCount(); i < count; ++i) {
        Action actionType = Action(enumActions.value(i));
        QAction *action = new QAction(icons[actionType], texts[actionType], this);

        m_actions[actionType] = action;

        action->setShortcutContext(Qt::WidgetShortcut);
        action->setShortcut(shortcuts[actionType]);

        int signalIndex = PathTableViewMenu::metaObject()->indexOfSignal(qPrintable(signalNames[actionType]));
        int actionSignalIndex = action->metaObject()->indexOfSignal("triggered(bool)");

        Q_ASSERT(signalIndex != -1 && actionSignalIndex != -1);

        QMetaMethod actionSignal = action->metaObject()->method(actionSignalIndex);
        QMetaMethod signal = PathTableViewMenu::metaObject()->method(signalIndex);

        connect(action, actionSignal, this, signal);
    }
}

void PathTableViewMenu::createMenu()
{
    QAction *sectionList = addSection(QStringLiteral("List"));

    addAction(m_actions[Action::RemoveItem]);

    m_sections[Section::FileName] = addSection(QStringLiteral("filename"));

    addAction(m_actions[Action::CopyName]);
    addAction(m_actions[Action::OpenPath]);
    addAction(m_actions[Action::DeletePath]);

    m_sections[Section::MultiFiles] = addSection(QStringLiteral("both"));

    addAction(m_actions[Action::OpenMulti]);

    QFont sectionFont(sectionList->font());

    sectionFont.setBold(true);
    sectionFont.setItalic(true);
//    sectionFont.setPointSizeF(sectionFont.pointSizeF() + 0.5);
    sectionFont.setUnderline(true);

    sectionList->setFont(sectionFont);

    for (auto itr = m_sections.begin(), end = m_sections.end(); itr != end; ++itr)
        itr.value()->setFont(sectionFont);
}

QString PathTableViewMenu::elide(QStringView text, const QFont &font, int width) const
{
    return QFontMetrics(font).elidedText(text.toString(), Qt::ElideMiddle, width);
}
