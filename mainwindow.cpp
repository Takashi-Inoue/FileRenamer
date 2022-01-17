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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "application.h"
#include "pathsanalyzer.h"
#include "path/pathheaderview.h"
#include "path/pathmodel.h"
#include "stringbuilder/onfile/builderchainonfile.h"
#include "widgets/counterlabel.h"
#include "widgets/dialogdroppeddir.h"
#include "widgets/elidelabel.h"

#include <QAbstractButton>
#include <QDropEvent>
#include <QMessageBox>
#include <QMimeData>
#include <QScopeGuard>
#include <QSettings>
#include <QDebug>

namespace {
constexpr char settingsGroupName[] = "Main";
constexpr char settingsKeyWindowGeometry[] = "WindowGeometry";
constexpr char settingsKeyWindowState[] = "WindowState";

void loadMainGeometry(MainWindow *window, Ui::MainWindow *ui)
{
    QSharedPointer<QSettings> qSettings = Application::mainQSettings();
    qSettings->beginGroup(settingsGroupName);

    auto geometryArray = qSettings->value(settingsKeyWindowGeometry, QVariant{}).toByteArray();

    if (!geometryArray.isEmpty())
        window->restoreGeometry(geometryArray);

    auto stateArray = qSettings->value(settingsKeyWindowState, QVariant{}).toByteArray();

    if (!stateArray.isEmpty()) {
        window->restoreState(stateArray);
    } else {
        ui->dockWidgetLogs->setVisible(false);
        ui->splitter->setSizes({400, 300});
    }

    qSettings->endGroup();
}

void saveMainGeometry(MainWindow *window)
{
    QSharedPointer<QSettings> qSettings = Application::mainQSettings();

    qSettings->beginGroup(settingsGroupName);
    qSettings->setValue(settingsKeyWindowGeometry, window->saveGeometry());
    qSettings->setValue(settingsKeyWindowState, window->saveState());
    qSettings->endGroup();
}

} // anonymous

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent},
      ui{new Ui::MainWindow},
      m_pathModel{new PathModel{this}}
{
    ui->setupUi(this);

    QWidget *hSpacer = new QWidget{this};

    hSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->toolBar->insertWidget(ui->actionClearItems, hSpacer);

    loadMainGeometry(this, ui);
    initStatusBar();

    ui->actionDarkMode->setChecked(Application::isDarkMode());

    ui->tableView->setHorizontalHeader(new PathHeaderView(ui->tableView));
    ui->tableView->setModel(m_pathModel);

    setState(State::initial);

    QStringList paths = QApplication::arguments();

    paths.pop_front();

    registerPaths(paths);

    connect(ui->frameBuilderList, &FrameBuilderList::settingsChanged,
            m_pathModel, &PathModel::startCreateNewNames);

    connect(ui->frameBuilderList, SIGNAL(changeStarted()), this, SLOT(adaptorToChangeState()));
    connect(ui->frameBuilderList, SIGNAL(builderCleared()), this, SLOT(adaptorToChangeState()));

    connect(m_pathModel, &PathModel::itemCleared,    this, &MainWindow::adaptorToChangeState);
    connect(m_pathModel, &PathModel::readyToRename,  this, &MainWindow::adaptorToChangeState);
    connect(m_pathModel, &PathModel::renameStarted,  this, &MainWindow::adaptorToChangeState);
    connect(m_pathModel, &PathModel::renameStopped,  this, &MainWindow::adaptorToChangeState);
    connect(m_pathModel, &PathModel::renameFinished, this, &MainWindow::adaptorToChangeState);
    connect(m_pathModel, &PathModel::undoStarted,    this, &MainWindow::adaptorToChangeState);

    connect(m_pathModel, &PathModel::internalDataChanged, this, &MainWindow::startCreatingNewNames);
    connect(m_pathModel, &PathModel::sortingBroken,       this, &MainWindow::onSortingBroken);

    connect(ui->actionRename,     &QAction::triggered, m_pathModel, &PathModel::startRename);
    connect(ui->actionStop,       &QAction::triggered, m_pathModel, &PathModel::stopRename);
    connect(ui->actionUndo,       &QAction::triggered, m_pathModel, &PathModel::undoRename);
    connect(ui->actionClearItems, &QAction::triggered, m_pathModel, &PathModel::clear);

    connect(ui->actionRename, &QAction::triggered,
            ui->frameBuilderList, &FrameBuilderList::saveLastUsedSettings);

    startCreatingNewNames();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->setAccepted(ui->actionExit->isEnabled());

    if (!event->isAccepted())
        return;

    saveMainGeometry(this);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(QStringLiteral("text/uri-list")))
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QStringList paths;

    for (const QUrl &url : event->mimeData()->urls())
        paths << url.toLocalFile();

    registerPaths(paths);
}

void MainWindow::startCreatingNewNames()
{
    m_pathModel->startCreateNewNames(ui->frameBuilderList->builderChain());
}

void MainWindow::onSortingBroken()
{
    QHeaderView *header = ui->tableView->horizontalHeader();

    header->setSortIndicatorShown(false);
}

void MainWindow::onActionDarkModeTriggered(bool checked)
{
    checked ? Application::applyDarkPalette()
            : Application::applyDefaultPalette();
}

void MainWindow::adaptorToChangeState()
{
    static const int builderCleared = ui->frameBuilderList->metaObject()->indexOfSignal("builderCleared()");
    static const int changeStarted  = ui->frameBuilderList->metaObject()->indexOfSignal("changeStarted()");
    static const int itemCleared    = m_pathModel->metaObject()->indexOfSignal("itemCleared()");
    static const int readyToRename  = m_pathModel->metaObject()->indexOfSignal("readyToRename()");
    static const int renameStarted  = m_pathModel->metaObject()->indexOfSignal("renameStarted()");
    static const int renameStopped  = m_pathModel->metaObject()->indexOfSignal("renameStopped()");
    static const int renameFinished = m_pathModel->metaObject()->indexOfSignal("renameFinished()");
    static const int undoStarted    = m_pathModel->metaObject()->indexOfSignal("undoStarted()");

    static const QHash<int, QString> hashIndexToSignalName = { // for debug msg
        {builderCleared, QStringLiteral("builderCleared()")},
        {changeStarted,  QStringLiteral("changeStarted()")},
        {itemCleared,    QStringLiteral("itemCleared()")},
        {readyToRename,  QStringLiteral("readyToRename()")},
        {renameStarted,  QStringLiteral("renameStarted()")},
        {renameStopped,  QStringLiteral("renameStopped()")},
        {renameFinished, QStringLiteral("renameFinished()")},
        {undoStarted,    QStringLiteral("undoStarted()")},
    };

    qDebug() << "change state from Signal:" << hashIndexToSignalName[senderSignalIndex()];

    static const QHash<int, State> hashSignalToState = {
        {builderCleared, State::initial},
        {changeStarted,  State::initial},
        {itemCleared,    State::initial},
        {readyToRename,  State::ready},
        {renameStarted,  State::renaming},
        {renameStopped,  State::stopped},
        {renameFinished, State::finished},
        {undoStarted,    State::renaming},
    };

    auto itr = hashSignalToState.find(senderSignalIndex());

    if (itr == hashSignalToState.end())
        return;

    setState(itr.value());
}

void MainWindow::setState(MainWindow::State state)
{
    static const QHash<State, QList<bool>> hashForUI = {
        {State::initial,  {false, false, false, true,  true,  true}},
        {State::ready,    {true,  false, false, true,  true,  true}},
        {State::renaming, {false, true,  false, false, false, false}},
        {State::stopped,  {true,  false, true,  true,  false, true}},
        {State::finished, {false, false, true,  true,  false, true}},
    };

    enum Actions {rename, stop, undo, exit, changeSettigs, clearItems};

    ui->actionRename->setEnabled(hashForUI[state][rename]);
    ui->actionStop->setEnabled(hashForUI[state][stop]);
    ui->actionUndo->setEnabled(hashForUI[state][undo]);
    ui->actionExit->setEnabled(hashForUI[state][exit]);
    ui->actionClearItems->setEnabled(hashForUI[state][clearItems]);

    const bool isEnableToChangeSettings = hashForUI[state][changeSettigs];

    setAcceptDrops(isEnableToChangeSettings);

    ui->frameBuilderList->setEnabled(isEnableToChangeSettings);
    ui->tableView->setEnableToChangeItems(isEnableToChangeSettings);

    auto header = qobject_cast<PathHeaderView *>(ui->tableView->horizontalHeader());

    if (header != nullptr) {
        header->setEnableToChangeItems(isEnableToChangeSettings);

        if (state == State::ready)
            ui->tableView->resizeColumnToContents(int(PathModel::HSection::NewName));
    }
}

void MainWindow::registerPaths(const QStringList &paths)
{
    qInfo() << tr("Start registering paths.");

    auto sorting = qScopeGuard([this]() {
        QHeaderView *header = ui->tableView->horizontalHeader();

        if (header->isSortIndicatorShown())
            m_pathModel->sort(header->sortIndicatorSection(), header->sortIndicatorOrder());
    });

    PathsAnalyzer analyzer;

    analyzer.analyze(paths);

    if (!analyzer.isAllDir()) {
        qInfo() << tr("Register dropped paths.");
        m_pathModel->addPaths(analyzer.dirs(), analyzer.files());

        return;
    }

    int msgBoxResult = execConfirmRenameDirDlg(paths);

    if (msgBoxResult == QMessageBox::Cancel)
        return;

    if (msgBoxResult == QMessageBox::Yes) {
        qInfo() << tr("Register dropped directories.");
        m_pathModel->addPaths(analyzer.dirs(), {});

        return;
    }

    DialogDroppedDir dlg(analyzer.dirs(), this);

    if (dlg.exec() == QDialog::Rejected)
        return;

    m_pathModel->addPaths(dlg.dirsToRename(), dlg.filesToRename());
}

int MainWindow::execConfirmRenameDirDlg(const QStringList &dirPaths)
{
    const QString titles[] = {
        tr("Confirm Rename Dir")
      , tr("Confirm Rename Dirs")
    };

    const QString msgs[] = {
        tr("Directory has been sent.\n\nWould you like to rename this dir?")
      , tr("Directories have been sent.\n\nWould you like to rename these dirs?")
    };

    qsizetype indexMsg = (dirPaths.size() > 1);

    QMessageBox msgBox(QMessageBox::Question, titles[indexMsg], msgs[indexMsg],
                       QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this);

    msgBox.button(QMessageBox::No)->setText(tr("No, search files"));
    msgBox.button(QMessageBox::No)->setIcon(QIcon(":/icons/search"));
    msgBox.button(QMessageBox::Yes)->setIcon(QIcon(":/icons/folder"));
    msgBox.setDetailedText(dirPaths.join('\n'));

    QList<QAbstractButton *> buttons = msgBox.buttons();

    auto itr = std::find_if(buttons.begin(), buttons.end(), [&](QAbstractButton *button) {
        return msgBox.buttonRole(button) == QMessageBox::ActionRole;
    });

    if (itr != buttons.end())
        (*itr)->click();

    return msgBox.exec();
}

/*-------- initialize UI --------*/
void MainWindow::initStatusBar()
{
    auto statusItemCount = new CounterLabel(QStringLiteral("item"), QStringLiteral("items"), this);
    auto statusSelectedCount = new CounterLabel(tr("selected"), this);
    auto statusState = new ElideLabel(this);
    auto statusMain = new ElideLabel(this);

    statusMain->setElideMode(Qt::ElideMiddle);

    QFontMetrics fontMetrics(statusSelectedCount->font());

    int labelWidth = int(fontMetrics.boundingRect("888888 selected").width() * 1.1);

    statusItemCount->setFixedWidth(labelWidth);
    statusItemCount->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    statusSelectedCount->setFixedWidth(labelWidth);
    statusSelectedCount->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    ui->statusBar->layout()->setContentsMargins(0, 0, 0, 0);
    ui->statusBar->addPermanentWidget(statusItemCount, 1);
    ui->statusBar->addPermanentWidget(createVLine());
    ui->statusBar->addPermanentWidget(statusSelectedCount, 1);
    ui->statusBar->addPermanentWidget(createVLine());
    ui->statusBar->addPermanentWidget(statusState, 1);
    ui->statusBar->addPermanentWidget(createVLine());
    ui->statusBar->addPermanentWidget(statusMain, 8);

    labelWidth = int(fontMetrics.boundingRect("Same new name").width() * 1.1) + statusState->height() + 2;
    statusState->setFixedWidth(labelWidth);
    statusState->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    connect(m_pathModel, &PathModel::itemCountChanged, statusItemCount, &CounterLabel::setCount);

    connect(ui->tableView, &PathTableView::selectedCountChanged, statusSelectedCount, &CounterLabel::setCount);
    connect(ui->tableView, SIGNAL(statusTextChanged(QIcon,QString))
            , statusMain, SLOT(setTextWithElide(QIcon,QString)));
    connect(ui->tableView, SIGNAL(stateTextChanged(QIcon,QString))
            , statusState, SLOT(setTextWithElide(QIcon,QString)));
}

QFrame *MainWindow::createVLine()
{
    auto vLine = new QFrame(this);

    vLine->setFrameStyle(int(QFrame::Sunken) | int(QFrame::VLine));
    vLine->setLineWidth(1);

    return vLine;
}
