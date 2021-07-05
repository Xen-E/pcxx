/*******************************************************************************************
*    Copyright (c) 2019 Xen <xen-dev@pm.me>                                                *
*    This file is part of the Playlist Converter++ project, AKA PC++ AKA PCxx.             *
*    Playlist Converter++ is free software; you can redistribute it and/or modify it under *
*    the terms of the GNU Lesser General Public License (LGPL) as published                *
*    by the Free Software Foundation; either version 3 of the License, or                  *
*    (at your option) any later version.                                                   *
*    Playlist Converter++ is distributed in the hope that it will be useful, but WITHOUT   *
*    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or                 *
*    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public                   *
*    License for more details.                                                             *
*    You should have received a copy of the GNU Lesser General Public License              *
*    along with this program. If not, see <https://www.gnu.org/licenses>.                  *
*******************************************************************************************/


#include "pcxx.h"
#include "ui_pcxx.h"

#include <misc.h>
#include <versions.h>
#include <aboutAnimator.h>

#include <QSettings>
#include <QDateTime>


bool _UNEXPORTED;
bool _CANCEL_EXPORTING;
bool _CANCEL_LOADING;

bool _ALREADY_ANIMATED;

QVector<File> Playlist;

PCxx::PCxx(QWidget *parent) : QMainWindow(parent /*, Qt::FramelessWindowHint*/), ui(new Ui::PCxx)
{
    ui->setupUi(this);

    this->setWindowTitle(QString("%1 (%2)").arg(Product::name, Architecture::humanReadable));
    this->setWindowIcon(QIcon(":/icons/pcxx.png"));

    QSettings settings;

    this->setGeometry(QRect(settings.value("Window/X", 400).toInt(),
                            settings.value("Window/Y", 200).toInt(),
                            settings.value("Window/Width", 750).toInt(),
                            settings.value("Window/Height", 450).toInt()));

    ui->settingsToolBox->setCurrentIndex(settings.value("Misc/SettingsSelectedPage", 0).toInt());
    ui->exportToolBox->setCurrentIndex(settings.value("Misc/ExportSelectedPage", 0).toInt());

    ui->PathPrefix->setChecked(settings.value("PathPrefix/Enable", false).toBool());
    ui->txtPathPrefixFind->setText(settings.value("PathPrefix/Find", "").toString());
    ui->txtPathPrefixReplace->setText(settings.value("PathPrefix/Replace", "").toString());
    ui->chkPathPrefixCaseSensitive->setChecked(settings.value("PathPrefix/CaseSensitive", false).toBool());

    ui->loadingCodec->setCurrentIndex(settings.value("Unicode/LoadingCodec", 31).toInt());
    ui->exportingCodec->setCurrentIndex(settings.value("Unicode/ExportingCodec", 31).toInt());

    ui->LoadingPerformanceSlider->setValue(settings.value("Misc/LoadingPerformance", 0).toInt());
    ui->ExportingPerformanceSlider->setValue(settings.value("Misc/ExportingPerformance", 0).toInt());

    ui->txtDirFilter->setText(settings.value("Misc/DirectoryFilter", "").toString());
    ui->chkDirFilterCaseSensitive->setChecked(settings.value("Misc/DirectoryFilterCaseSensitive", false).toBool());

    ui->chkColorMissingFiles->setChecked(settings.value("FilesList/ColorMissingFiles", false).toBool());
    ui->chkShowFilesListHeader->setChecked(settings.value("FilesList/ShowHeader", true).toBool());
    ui->chkResizeColumnsToFitContent->setChecked(settings.value("FilesList/ResizeColumnsToFitContent", true).toBool());
    ui->chkShowFilesListGrid->setChecked(settings.value("FilesList/ShowGrid", false).toBool());
    ui->chkAlternateFilesListColors->setChecked(settings.value("FilesList/AlternateColors", true).toBool());

    ui->chkLoadingShowMsgWhenDone->setChecked(settings.value("Notification/ShowMessageWhenLoadingDone", false).toBool());
    ui->chkExportShowMsgWhenDone->setChecked(settings.value("Notification/ShowMessageWhenExportingDone", true).toBool());
    ui->chkConfirmQuitUnexported->setChecked(settings.value("Notification/ConfirmQuitUnexported", true).toBool());
    ui->chkConfirmQuitLoading->setChecked(settings.value("Notification/ConfirmQuitLoading", true).toBool());
    ui->chkConfirmQuitExporting->setChecked(settings.value("Notification/ConfirmQuitExporting", true).toBool());
    ui->chkConfirmQuitID3Fixing->setChecked(settings.value("Notification/ConfirmQuitID3Fixer", true).toBool());

    ui->themeList->setCurrentIndex(settings.value("EyeCandy/ThemeMode", 1).toInt());
    ui->progressBarThemeList->setCurrentIndex(settings.value("EyeCandy/PBTheme", 4).toInt());
    ui->tabsStyle->setCurrentIndex(settings.value("EyeCandy/TabsStyle", 0).toInt());
    ui->tabsPosition->setCurrentIndex(settings.value("EyeCandy/TabsPosition", 0).toInt());
    ui->chkDisableAboutDescriptionAnimation->setChecked(settings.value("EyeCandy/DisableAboutDescriptionAnimation", false).toBool());

    ui->exportPlaylistFormat->setCurrentIndex(settings.value("Exporting/PlaylistFormat", 0).toInt());
    ui->exportSavePath->setText(settings.value("Exporting/SavePath", "").toString());
    ui->exportPlaylistTitle->setText(settings.value("Exporting/PlaylistTitle", QString("%1 V%2 Generated Playlist").arg(Product::name, Product::version)).toString());

    ui->chkExportExcludeFailedPathPrefix->setChecked(settings.value("Exporting/ExcludeFailedPathPrefix", false).toBool());
    ui->chkExportCheckFileExistence->setChecked(settings.value("Exporting/CheckFileExistence", false).toBool());
    ui->chkExportShowInDir->setChecked(settings.value("Exporting/ShowInFolderWhenDone", false).toBool());

    ui->chkExportExecuteCMD->setChecked(settings.value("Exporting/ExecuteCMD", false).toBool());
    ui->txtCMD->setText(settings.value("Exporting/Command", "notepad/gedit/kwrite/TextEdit/etc... {playlist}").toString());

    ui->LogsFontSize->setCurrentIndex(settings.value("Logs/FontSize", 1).toInt());


    //Metatype registers
    qRegisterMetaType<levelType>("levelType");
    qRegisterMetaType<Format>("Format");

    //Handles item editing, this signal is bloacked when adding new items.
    connect(ui->filesList, &QTableWidget::itemChanged, this, &PCxx::onFilesListItemChanged);

    //Sets item row to a fixed small height
    QHeaderView *verticalHeader = ui->filesList->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(20);


    //We're using QAction so this is important
    ui->filesList->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

    //Actions for files list
    delAction = new QAction(tr("Delete"), this);
    delAction->setShortcut(QKeySequence(tr("Del")));
    connect(delAction, &QAction::triggered, this, &PCxx::deleteFilesListItem);

    showInFolderAction = new QAction(tr("Show in Folder"), this);
    showInFolderAction->setShortcut(QKeySequence(tr("Ctrl+F")));
    connect(showInFolderAction, &QAction::triggered, this, &PCxx::showFilesListItemInFolder);

    selectAllAction = new QAction(tr("All"), this);
    selectAllAction->setShortcut(QKeySequence(tr("Ctrl+A")));
    connect(selectAllAction, &QAction::triggered, this, &PCxx::selectAllItems);

    highlightEmptyAction = new QAction(tr("Items with Empty Fields"), this);
    highlightEmptyAction->setShortcut(QKeySequence(tr("Ctrl+H")));
    connect(highlightEmptyAction, &QAction::triggered, this, &PCxx::highlightEmptyItems);

    clearSelectionAction = new QAction(tr("Deselect All"), this);
    clearSelectionAction->setShortcut(QKeySequence(tr("Ctrl+D")));
    connect(clearSelectionAction, &QAction::triggered, this, &PCxx::clearSelections);

    select = new QAction(tr("Select"), this);
    selectMenu = new QMenu;
    selectMenu->addAction(selectAllAction);
    selectMenu->addAction(clearSelectionAction);
    select->setMenu(selectMenu);

    highlight = new QAction(tr("Highlight"), this);
    highlightMenu = new QMenu;
    highlightMenu->addAction(highlightEmptyAction);
    highlight->setMenu(highlightMenu);

    ui->filesList->addAction(select);
    ui->filesList->addAction(highlight);

    QAction *sep1 = new QAction(this); sep1->setSeparator(true);
    ui->filesList->addAction(sep1);

    ui->filesList->addAction(delAction);
    ui->filesList->addAction(showInFolderAction);


    //Since alternaterowcolors option is acting weird, this will fix it
    ui->filesList->setAlternatingRowColors(settings.value("FilesList/AlternateColors", true).toBool());

    UpdateAboutPage();
}

void PCxx::UpdateAboutDescription(const QChar &letter)
{
    QString desc(ui->lblAboutDescription->toPlainText() + letter);
    ui->lblAboutDescription->setText(desc);
}

void PCxx::on_tabWidget_tabBarClicked(int index)
{
    if (index == 4 && //About tab
    !_ALREADY_ANIMATED &&
    !ui->chkDisableAboutDescriptionAnimation->isChecked()) {
        Animator *animator = new Animator;
        animator->description = ui->lblAboutDescription->toPlainText();
        animator->speed = 65; //ms
        connect(animator, &Animator::UpdateAboutDescription, this, &PCxx::UpdateAboutDescription);

        QThread *thread = new QThread;
        connect(thread, &QThread::started, animator, &Animator::Animate);

        animator->moveToThread(thread);

        ui->lblAboutDescription->clear();
        _ALREADY_ANIMATED = true;

        thread->start();
    }
}

void PCxx::UpdateAboutPage()
{
    ui->lblAboutProduct->setText(tr("%1 v%2").arg(Product::name, Product::version));
    ui->lblAboutCompiled->setText(tr("Compiled using %1 Version %2").arg(usedCompiler::name, usedCompiler::version));
    ui->lblAboutArchitecture->setText(tr("Built for %1 architecture.").arg(Architecture::humanReadable));
    ui->lblAboutAuthor->setText(QString("%1 <%2> / %3").arg(Developer::name, Developer::email, Developer::domain));
    ui->lblAboutQt->setText(tr("Qt framework, Version: %1 - Licensed under GPL.").arg(Libraries::qtVersion));
    ui->lblAboutTagLib->setText(tr("TagLib, Version: %1 - Licensed under LGPL.").arg(Libraries::taglibVersion));
}

void PCxx::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) event->acceptProposedAction();
    else QMessageBox::warning(this, tr("Not a file/folder"),
    tr("Only files/folders are allowed to be dropped here, Pick some media files and drag/drop them here."));
}

void PCxx::dropEvent(QDropEvent *event)
{
    const int droppedSize(event->mimeData()->urls().size());
    QStringList files; QString dirPath;

    if (droppedSize == 1) {
        const QUrl url(event->mimeData()->urls().at(0));
        QDir dir(url.toLocalFile());
        if (dir.exists()) {
            dirPath = dir.path();
            if (!ui->txtDirFilter->text().isEmpty()) {
                if (ui->txtDirFilter->text().contains(" ")) {
                    files = filterDir(ui->txtDirFilter->text(), dir.entryList(QDir::Files), ui->chkDirFilterCaseSensitive->isChecked());
                }
                else if (ui->txtDirFilter->text().length() > 0) {
                    files = filterDir(ui->txtDirFilter->text(), dir.entryList(QDir::Files), ui->chkDirFilterCaseSensitive->isChecked());
                }
            }
            else files = dir.entryList(QDir::Files);
        }
        else files.push_back(url.toLocalFile());
    }
    else if (droppedSize > 1) {
        foreach (const QUrl &url, event->mimeData()->urls()) {
            files.push_back(url.toLocalFile());
        }
    }

    FileReader *fileReader = new FileReader;
    fileReader->dir = dirPath;
    fileReader->files = files;
    fileReader->Performance = ui->LoadingPerformanceSlider->value();
    fileReader->ShowMsgWhenDone = ui->chkLoadingShowMsgWhenDone->isChecked();

    //Connecting signals and slots (QT 5+ way)
    connect(fileReader, &FileReader::log, this, &PCxx::UpdateLogs);
    connect(fileReader, &FileReader::ShowMessageBox, this, &PCxx::ShowMessageBox);
    connect(fileReader, &FileReader::setPBMax, this, &PCxx::UpdateLoadingPBMax);
    connect(fileReader, &FileReader::setPB, this, &PCxx::UpdateLoadingPB);
    connect(fileReader, &FileReader::disableUI, this, &PCxx::FilesAreLoading);
    connect(fileReader, &FileReader::UpdateStats, this, &PCxx::UpdateStats);
    connect(fileReader, &FileReader::UpdateFilesList, this, &PCxx::UpdateFilesList);

    QThread *thread = new QThread; //Thread must be created on heap or it will be destroyed with this scope
    connect(thread, &QThread::started, fileReader, &FileReader::Read); //void FileReader::Read is what starts the process

    fileReader->moveToThread(thread); //Moving file reader object to different thread so it won't block UI
    thread->start(); //Goodluck!
}

void PCxx::deleteFilesListItem()
{
    QList<QTableWidgetItem *> items = ui->filesList->selectedItems();

    if (!items.isEmpty()) {
        int colsNum = 6; int filePathPos = 0;

        for (int i = 0; i < (items.size() / colsNum); i++) {
            QTableWidgetItem *item = items.at(filePathPos);

            const int ItemIndexInPlaylist(GetItemIndexByFilePath(item->text()));

            if (ItemIndexInPlaylist > -1) {
                if (DeleteItemInPlaylist(Playlist[ItemIndexInPlaylist].ID)) {
                    UpdateLogs(info, tr("File \"%1\" deleted from the list.").arg(item->text()));
                }
                else { UpdateLogs(error, tr("Couldn't delete file \"%1\" from the list.").arg(item->text())); }
            }
            else { UpdateLogs(error, tr("Couldn't delete file \"%1\" from the list.").arg(item->text())); }

            filePathPos += 6;
        }

        UpdateStats();
        UpdateFilesList();
    }
    else {
        QMessageBox::warning(this, tr("No items selected"),
        tr("Select some items to delete first.%1"
        "You can use the mouse or you can hold the Ctrl/⌘ key and select/deselect items.").arg(NEWLINE));
    }

}

void PCxx::selectAllItems()
{
    ui->filesList->selectAll();
}

void PCxx::highlightEmptyItems()
{
    for (int r = 0; r < ui->filesList->rowCount(); r++) {
        for (int c = 1; c <= 3; c++) {
            QTableWidgetItem *item = ui->filesList->item(r, c);
            if (item->text().isEmpty()) {
                item->setBackground(QColor(232, 252, 3));
                item->setForeground(QColor(0, 0, 0));
            }
        }
    }
}

void PCxx::clearSelections()
{
    ui->filesList->clearSelection();
}

void PCxx::showFilesListItemInFolder()
{
    QList<QTableWidgetItem *> items = ui->filesList->selectedItems();

    if (!items.isEmpty()) {
        if (items.size() <= 6) {
            showInFolder(items[0]->text());
        }
        else {
            QMessageBox::warning(this, tr("Multiple items selected"),
            tr("Please select only ONE item."));
        }
    }
    else {
        QMessageBox::warning(this, tr("No item selected"),
        tr("In order to show a file in folder you need to select an item first."));
    }
}

PCxx::~PCxx()
{
    delete ui;
}

void PCxx::onFilesListItemChanged(QTableWidgetItem *item)
{
    switch (item->column()) {
    case 0: //File path
        if (!item->text().isEmpty()) {
            Playlist[item->row()].fullPath = item->text();
            if (ui->chkColorMissingFiles->isChecked()) {
                if (!QFile::exists(item->text())) {
                    QMessageBox::warning(this, tr("Missing File"),
                    tr("This file path that you entered doesn't exist!%1%1You can turn off this warning from settings in \"Files List\" section.").arg(NEWLINE));
                    item->setForeground(QColor("red"));
                }
            }
        }
        else {
            QMessageBox::warning(this, tr("File path is empty"), tr("File path can't be empty please provide at least one letter/character."));
            item->setText(Playlist[item->row()].fullPath);
        }
        break;
    case 1: //Title
        Playlist[item->row()].title = item->text();
        break;
    case 2: //Artist
        Playlist[item->row()].artist = item->text();
        break;
    case 3: //Album
        Playlist[item->row()].album = item->text();
        break;
    default:
        break;
    }

    UpdateLogs(info, tr("Item \"%1\" (%2) received an update: %3").arg(Playlist.at(item->row()).fullPath,
                                                                       QString::number(item->row()+1),
                                                                       item->text()));
}

void PCxx::on_btnFixID3_clicked()
{
    if (Playlist.size() > 0) {
        ui->filesList->clearContents();
        ui->filesList->setRowCount(0);

        ID3Fixer *fixer = new ID3Fixer;

        //Connecting signals and slots (QT 5+ way)
        connect(fixer, &ID3Fixer::log, this, &PCxx::UpdateLogs);
        connect(fixer, &ID3Fixer::ShowMessageBox, this, &PCxx::ShowMessageBox);
        connect(fixer, &ID3Fixer::setPBMax, this, &PCxx::UpdateLoadingPBMax);
        connect(fixer, &ID3Fixer::setPB, this, &PCxx::UpdateLoadingPB);
        connect(fixer, &ID3Fixer::disableUI, this, &PCxx::FilesAreLoading);
        connect(fixer, &ID3Fixer::UpdateStats, this, &PCxx::UpdateStats);
        connect(fixer, &ID3Fixer::UpdateFilesList, this, &PCxx::UpdateFilesList);

        QThread *thread = new QThread; //Thread must be created on heap or it will be destroyed with this scope
        connect(thread, &QThread::started, fixer, &ID3Fixer::Fix); //void ID3Fixer::Fix is what starts the process

        fixer->moveToThread(thread); //Moving parser object to different thread so it won't block UI
        thread->start(); //Goodluck!
    }
    else { QMessageBox::warning(this, tr("What?"), tr("There's nothing to fix here.")); }
}

void PCxx::on_btnAddPlaylist_clicked()
{
    const QString filter("All supported playlists (*.aimppl *.aimppl2 *.aimppl3 *.aimppl4 *.aimppl5 "
    "*.asx *.b4s *.fpl *.m3u *.m3u8 *.mpcpl *.pls *.wpl *.xspf *.json *.jspf *.txt *.xml);;"
    "MP3 URL Playlists (*.m3u *.m3u8);;"
    "museArc/SHOUTcast Playlists (*.pls);;"
    "XML Shareable Playlist Format (*.xspf);;"
    "JSON Shareable Playlist Format (*.json *.jspf);;"
    "foobar2000 Playlists (*.fpl);;"
    "Apple ITunes XML Playlists (*.xml);;"
    "Microsoft Windows Media Player Playlists (*.wpl);;"
    "Advanced Stream Redirector Playlists (*.asx);;"
    "Media Player Classic Playlists (*.mpcpl);;"
    "AIMP Playlists (*.aimppl *.aimppl2 *.aimppl2 *.aimppl4 *.aimppl5);;"
    "B4S (old Winamp) Playlists (*.b4s);;"
    "Text Document (*.txt)");

    const QString plfn = QFileDialog::getOpenFileName(this, tr("Choose a playlist to load..."), QString(), filter);

    if (plfn.isEmpty())
        return;

    Parser *parser = new Parser;
    parser->FileName = plfn;
    parser->Codec = ui->loadingCodec->itemText(ui->loadingCodec->currentIndex());

    //Most playlist files have no magic number
    //so i'm gonna use file extension to detect the type.
    if (plfn.contains(".aimppl"))
        parser->Type = Format::AIMPPL;
    else if (plfn.endsWith(".asx"))
        parser->Type = Format::ASX;
    else if (plfn.endsWith(".b4s"))
        parser->Type = Format::B4S;
    else if (plfn.endsWith(".fpl"))
        parser->Type = Format::FPL;
    else if (plfn.endsWith(".m3u") || plfn.endsWith(".m3u8"))
        parser->Type = Format::M3U;
    else if (plfn.endsWith(".mpcpl"))
        parser->Type = Format::MPCPL;
    else if (plfn.endsWith(".pls"))
        parser->Type = Format::PLS;
    else if (plfn.endsWith(".wpl"))
        parser->Type = Format::WPL;
    else if (plfn.endsWith(".xspf"))
        parser->Type = Format::XSPF;
    else if (plfn.endsWith(".xml"))
        parser->Type = Format::ITUNES;
    else if (plfn.endsWith(".json") || plfn.endsWith(".jspf"))
        parser->Type = Format::JSPF;
    else if (plfn.endsWith(".txt"))
        parser->Type = Format::TXT;

    parser->Performance = ui->LoadingPerformanceSlider->value();
    parser->ShowMsgWhenDone = ui->chkLoadingShowMsgWhenDone->isChecked();

    //Connecting signals and slots (QT 5+ way)
    connect(parser, &Parser::log, this, &PCxx::UpdateLogs);
    connect(parser, &Parser::ShowMessageBox, this, &PCxx::ShowMessageBox);
    connect(parser, &Parser::setPBMax, this, &PCxx::UpdateLoadingPBMax);
    connect(parser, &Parser::setPB, this, &PCxx::UpdateLoadingPB);
    connect(parser, &Parser::disableUI, this, &PCxx::FilesAreLoading);
    connect(parser, &Parser::UpdateStats, this, &PCxx::UpdateStats);
    connect(parser, &Parser::UpdateFilesList, this, &PCxx::UpdateFilesList);

    QThread *thread = new QThread; //Thread must be created on heap or it will be destroyed with this scope
    connect(thread, &QThread::started, parser, &Parser::Parse); //void Parser::Parse is what starts the process

    parser->moveToThread(thread); //Moving parser object to different thread so it won't block UI
    thread->start(); //Goodluck!
}

void PCxx::UpdateFilesList()
{
    //Reset before filling again
    ui->filesList->clearContents();
    ui->filesList->setRowCount(0);

    if (Playlist.size() > 0) {
        ui->filesList->blockSignals(true); //Blocks itemChanged signal

        ui->filesList->setColumnCount(6);
        ui->filesList->setRowCount(Playlist.size());

        for (auto &track : Playlist) {
            QTableWidgetItem *fullPathItem = new QTableWidgetItem(track.fullPath);
            QTableWidgetItem *titleItem    = new QTableWidgetItem(track.title);
            QTableWidgetItem *artistItem   = new QTableWidgetItem(track.artist);
            QTableWidgetItem *albumItem    = new QTableWidgetItem(track.album);

            QTableWidgetItem *lengthItem   = new QTableWidgetItem(track.length);
            lengthItem->setFlags(lengthItem->flags() ^ Qt::ItemIsEditable); //Read only

            QTableWidgetItem *fileSizeItem = new QTableWidgetItem(track.fileSize);
            fileSizeItem->setFlags(fileSizeItem->flags() ^ Qt::ItemIsEditable); //Read only

            const int index(&track-Playlist.begin());
            ui->filesList->setItem(index, 0, fullPathItem);
            ui->filesList->setItem(index, 1, titleItem);
            ui->filesList->setItem(index, 2, artistItem);
            ui->filesList->setItem(index, 3, albumItem);
            ui->filesList->setItem(index, 4, lengthItem);
            ui->filesList->setItem(index, 5, fileSizeItem);

            if (ui->chkColorMissingFiles->isChecked()) {
                if (!QFile::exists(track.fullPath)) {
                    fullPathItem->setText(tr("MISSING: %1").arg(track.fullPath));
                    fullPathItem->setForeground(QColor("red"));
                }
            }
        }

        if (ui->chkResizeColumnsToFitContent->isChecked()) {
            ui->filesList->resizeColumnToContents(0);
            ui->filesList->resizeColumnToContents(1);
            ui->filesList->resizeColumnToContents(2);
            ui->filesList->resizeColumnToContents(3);
            ui->filesList->resizeColumnToContents(4);
            ui->filesList->resizeColumnToContents(5);
        }

        ui->filesList->blockSignals(false);
    }
}

void PCxx::UpdateStats()
{
    uint64_t totalFileSize = 0;
    long totalLength       = 0;

    if (Playlist.size() > 0) {
        for (int f = 0; f < Playlist.size(); f++) {
            totalFileSize += Playlist.at(f).rawFileSize;
            totalLength   += Playlist.at(f).rawLength;
        }
        ui->lblStats->setText(tr("%1 Item(s) | %2 | %3").arg(QString::number(Playlist.size()),
                                                             humanizeBytes(totalFileSize),
                                                             formatLength(totalLength)));
        ui->btnClearFiles->setEnabled(true);
        ui->btnFixID3->setEnabled(true);
    }
    else {
        ui->btnClearFiles->setEnabled(false);
        ui->btnFixID3->setEnabled(false);
        ui->lblStats->setText(tr("No files added yet."));
    }
}

void PCxx::UpdateLoadingPBMax(const int value)
{
    ui->PB_LOADING->setMaximum(value);
}
void PCxx::UpdateLoadingPB(const int value)
{
    ui->PB_LOADING->setValue(value);
}
void PCxx::UpdateExportingPBMax(const int value)
{
    ui->PB_EXPORT->setMaximum(value);
}
void PCxx::UpdateExportingPB(const int value)
{
    ui->PB_EXPORT->setValue(value);
}

void PCxx::FilesAreLoading(const bool really)
{
    if (!really) {
        _CANCEL_LOADING = false;
        ui->PB_LOADING->setValue(0);
    }

    ui->btnStopLoading->setEnabled(really);

    ui->tabExport->setEnabled(!really);
    ui->tabSettings->setEnabled(!really);
    ui->btnAddPlaylist->setEnabled(!really);
    ui->btnAddDirectory->setEnabled(!really);
    ui->btnAddFiles->setEnabled(!really);
    ui->btnClearFiles->setEnabled(!really);
    ui->btnFixID3->setEnabled(!really);
    ui->tabExport->setEnabled(!really);
}
void PCxx::FilesAreExporting(const bool really)
{
    if (!really) {
        _CANCEL_EXPORTING = false;
        ui->PB_EXPORT->setValue(0);
    }

    ui->btnCancelExportation->setEnabled(really);

    ui->tabFiles->setEnabled(!really);
    ui->tabSettings->setEnabled(!really);
    ui->btnExport->setEnabled(!really);
    ui->ProductionToolBoxPage->setEnabled(!really);
    ui->ExportOptionsToolBoxPage->setEnabled(!really);
}

void PCxx::UpdateLogs(levelType type, const QString &log)
{
    QString color;

    switch (type) {
    case info:
        //If dark theme is selected then use white, black otherwise
        color = ui->themeList->currentIndex() == 1 ? "white" : "black";
        break;
    case warn:
        color = "orange";
        break;
    case error:
        color = "red";
        break;
    case done:
        color = "green";
        break;
    default:
        color = ui->themeList->currentIndex() == 1 ? "white" : "black";
        break;
    }

    ui->txtLogs->append(QString("<span style='color:%1;'>[ %2 ] => %3</span>%4").arg(
    color,
    QDateTime::currentDateTime().toString("hh:mm:ss"), //hours:minutes:seconds
    log,
    NEWLINE));
}

void PCxx::ShowMessageBox(levelType type, const QString &title, const QString &msg)
{
    switch (type) {
    case info:
        QMessageBox::information(this, title, msg);
        break;
    case warn:
        QMessageBox::warning(this, title, msg);
        break;
    case error:
        QMessageBox::critical(this, title, msg);
        break;
    case done:
        break;
    }
}

void PCxx::on_btnClearFiles_clicked()
{
    int ret = QMessageBox::warning(this, tr("Files reset"),
                                   tr("Are you sure you want to clear current files and start all over?"),
                                   QMessageBox::Yes | QMessageBox::Cancel);

    if (ret == QMessageBox::Yes) {
        Playlist.clear();
        ui->filesList->clearContents();
        ui->filesList->setRowCount(0);
        ui->btnClearFiles->setEnabled(false);
        UpdateStats();
        UpdateLogs(info, tr("Files has been cleared."));
    }
}

void PCxx::on_btnStopLoading_clicked()
{
    _CANCEL_LOADING = true;
}

void PCxx::closeEvent(QCloseEvent *event)
{
    if (ui->chkConfirmQuitLoading && (_PLAYLIST_BUSY_LOADING || _FILES_BUSY_LOADING)) {
        int res = QMessageBox::warning(this, tr("Ongoing loading process"),
                                   tr("There's currently a loading process going on...%1"
                                   "Do you want to cancel it and quit anyway?").arg(NEWLINE),
                                   QMessageBox::Yes | QMessageBox::Cancel);

        if (res == QMessageBox::Yes) { _CANCEL_LOADING = true; } else { event->ignore(); return; }
    }
    if (ui->chkConfirmQuitExporting->isChecked() &&_PLAYLIST_BUSY_EXPORTING) {
        int res = QMessageBox::warning(this, tr("Ongoing exporting process"),
                                   tr("There's currently a exporting process going on...%1"
                                   "Do you want to cancel it and quit anyway?").arg(NEWLINE),
                                   QMessageBox::Yes | QMessageBox::Cancel);

        if (res == QMessageBox::Yes) { _CANCEL_EXPORTING = true; } else { event->ignore(); return; }
    }
    if (ui->chkConfirmQuitID3Fixing->isChecked() && _FILES_ARE_GETTING_FIXED) {
        int res = QMessageBox::warning(this, tr("Ongoing ID3 fixing process"),
                                   tr("There's currently a ID3 fixing process going on...%1"
                                   "Do you want to cancel it and quit anyway?").arg(NEWLINE),
                                   QMessageBox::Yes | QMessageBox::Cancel);

        if (res == QMessageBox::Yes) { _CANCEL_LOADING = true; } else { event->ignore(); return; }
    }
    if (!_FILES_BUSY_LOADING &&
        !_PLAYLIST_BUSY_LOADING &&
        !_PLAYLIST_BUSY_EXPORTING &&
        ui->chkConfirmQuitUnexported->isChecked() &&
        _UNEXPORTED &&
        Playlist.size() > 0) {
        int res = QMessageBox::warning(this, tr("Unexported data"),
                                   tr("There's currently data/files that hasn't been exported yet%1"
                                   "Are you sure want to quit anyway?").arg(NEWLINE),
                                   QMessageBox::Yes | QMessageBox::Cancel);

        if (res == QMessageBox::Cancel) { event->ignore(); return; }
    }

    QSettings settings;

    settings.setValue("Window/X", this->geometry().x());
    settings.setValue("Window/Y", this->geometry().y());
    settings.setValue("Window/Width", this->geometry().width());
    settings.setValue("Window/Height", this->geometry().height());

    settings.setValue("Misc/SettingsSelectedPage", ui->settingsToolBox->currentIndex());
    settings.setValue("Misc/ExportSelectedPage", ui->exportToolBox->currentIndex());

    settings.setValue("PathPrefix/Enable", ui->PathPrefix->isChecked());
    settings.setValue("PathPrefix/Find", ui->txtPathPrefixFind->text());
    settings.setValue("PathPrefix/Replace", ui->txtPathPrefixReplace->text());
    settings.setValue("PathPrefix/CaseSensitive", ui->chkPathPrefixCaseSensitive->isChecked());

    settings.setValue("Unicode/LoadingCodec", ui->loadingCodec->currentIndex());
    settings.setValue("Unicode/ExportingCodec", ui->exportingCodec->currentIndex());

    settings.setValue("Misc/LoadingPerformance", ui->LoadingPerformanceSlider->value());
    settings.setValue("Misc/ExportingPerformance", ui->ExportingPerformanceSlider->value());

    settings.setValue("Misc/DirectoryFilter", ui->txtDirFilter->text());
    settings.setValue("Misc/DirectoryFilterCaseSensitive", ui->chkDirFilterCaseSensitive->isChecked());

    settings.setValue("Notification/ShowMessageWhenLoadingDone", ui->chkLoadingShowMsgWhenDone->isChecked());
    settings.setValue("Notification/ShowMessageWhenExportingDone", ui->chkExportShowMsgWhenDone->isChecked());
    settings.setValue("Notification/ConfirmQuitUnexported", ui->chkConfirmQuitUnexported->isChecked());
    settings.setValue("Notification/ConfirmQuitLoading", ui->chkConfirmQuitLoading->isChecked());
    settings.setValue("Notification/ConfirmQuitExporting", ui->chkConfirmQuitExporting->isChecked());
    settings.setValue("Notification/ConfirmQuitID3Fixer", ui->chkConfirmQuitID3Fixing->isChecked());

    settings.setValue("FilesList/ColorMissingFiles", ui->chkColorMissingFiles->isChecked());
    settings.setValue("FilesList/ShowHeader", ui->chkShowFilesListHeader->isChecked());
    settings.setValue("FilesList/ResizeColumnsToFitContent", ui->chkResizeColumnsToFitContent->isChecked());
    settings.setValue("FilesList/ShowGrid", ui->chkShowFilesListGrid->isChecked());
    settings.setValue("FilesList/AlternateColors", ui->chkAlternateFilesListColors->isChecked());

    settings.setValue("EyeCandy/ThemeMode", ui->themeList->currentIndex());
    settings.setValue("EyeCandy/PBTheme", ui->progressBarThemeList->currentIndex());
    settings.setValue("EyeCandy/TabsStyle", ui->tabsStyle->currentIndex());
    settings.setValue("EyeCandy/TabsPosition", ui->tabsPosition->currentIndex());
    settings.setValue("EyeCandy/DisableAboutDescriptionAnimation", ui->chkDisableAboutDescriptionAnimation->isChecked());


    settings.setValue("Exporting/PlaylistFormat", ui->exportPlaylistFormat->currentIndex());
    settings.setValue("Exporting/SavePath", ui->exportSavePath->text());
    settings.setValue("Exporting/PlaylistTitle", ui->exportPlaylistTitle->text());

    settings.setValue("Exporting/ExcludeFailedPathPrefix", ui->chkExportExcludeFailedPathPrefix->isChecked());
    settings.setValue("Exporting/CheckFileExistence", ui->chkExportCheckFileExistence->isChecked());
    settings.setValue("Exporting/ShowInFolderWhenDone", ui->chkExportShowInDir->isChecked());

    settings.setValue("Exporting/ExecuteCMD", ui->chkExportExecuteCMD->isChecked());
    settings.setValue("Exporting/Command", ui->txtCMD->text());

    settings.setValue("Logs/FontSize", ui->LogsFontSize->currentIndex());

}

void PCxx::on_btnClearLogs_clicked()
{
    ui->txtLogs->clear();
}

void PCxx::on_btnSaveLogs_clicked()
{
    if (ui->txtLogs->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Oops!", "There's nothing to save here!?");
        return;
    }

    const QString logsSD = QFileDialog::getSaveFileName(this, tr("Save logs..."), QString(), "Text Document (*.txt)");

    if (logsSD.isEmpty())
        return;

    QFile fs(!logsSD.endsWith(".txt") ? logsSD + ".txt" : logsSD);
    if (fs.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream ts(&fs);
        ts << ui->txtLogs->toPlainText();
        fs.close();
        UpdateLogs(done, tr("Logs has been written to \"%1\".").arg(logsSD));
    }
    else {
        QMessageBox::critical(this, tr("Failed"), tr("Failed to write logs into a file! Check logs for more info."));
        UpdateLogs(error, tr("Failed to open a new file to write logs, Here's why: %1").arg(fs.errorString()));
    }
}

void PCxx::on_LogsFontSize_currentIndexChanged(int index)
{
    QFont font(ui->txtLogs->font());
    QTextCursor cursor(ui->txtLogs->textCursor());

    switch (index) {
    case 0: //Small
        font.setPointSize(7);
        break;
    case 1: //Medium
        font.setPointSize(10);
        break;
    case 2: //Large
        font.setPointSize(13);
        break;
    case 3: //Very large
        font.setPointSize(16);
        break;
    }

    ui->txtLogs->setFont(font);
    ui->txtLogs->setTextCursor(cursor);
}

void PCxx::on_chkExportExecuteCMD_toggled(bool checked)
{
    ui->txtCMD->setEnabled(checked);
}

void PCxx::on_btnExport_clicked()
{
    if (Playlist.size() < 1) {
        QMessageBox::warning(this, tr("No files"),
        tr("Go to \"Files\" tab and add some files first :) You left it empty, There's nothing to export."));
        return;
    }
    if (ui->exportSavePath->text().isEmpty()) {
        QMessageBox::warning(this, tr("Empty field"),
        tr("You left \"Produce in\" text field empty, Please provide a save path for the new playlist."));
        return;
    }
    if (ui->exportPlaylistTitle->text().isEmpty()) {
        QMessageBox::warning(this, tr("Empty field"),
        tr("You left \"Playlist Title\" text field empty, Please provide a title for the new playlist."));
        return;
    }
    if (ui->PathPrefix->isChecked() && ui->txtPathPrefixFind->text().isEmpty()) {
        QMessageBox::warning(this, tr("Empty field"),
        tr("Path Prefix is enabled but you left \"Find\" text field empty."));
        return;
    }
    if (!ui->PathPrefix->isChecked() && ui->chkExportExcludeFailedPathPrefix->isChecked()) {
        QMessageBox::warning(this, tr("Path Prefix is disabled"),
        tr("Path Prefix is disabled but \"Path Prefix failure exclusion\" (Export's first option) is enabled."));
        return;
    }
    if (ui->chkExportExecuteCMD->isChecked() && ui->txtCMD->text().isEmpty()) {
        QMessageBox::warning(this, tr("Empty field"),
        tr("Command Execution is enabled but you left \"Command\" text field empty."));
        return;
    }

    Exporter *exporter = new Exporter;

    switch (ui->exportPlaylistFormat->currentIndex()) {
    case 0: //M3U
        exporter->format   = Format::M3U;
        exporter->SavePath = ui->exportSavePath->text() + ".m3u";
        break;
    case 1: //PLS
        exporter->format   = Format::PLS;
        exporter->SavePath = ui->exportSavePath->text() + ".pls";
        break;
    case 2: //XSPF
        exporter->format   = Format::XSPF;
        exporter->SavePath = ui->exportSavePath->text() + ".xspf";
        break;
    case 3: //JSPF
        exporter->format   = Format::JSPF;
        exporter->SavePath = ui->exportSavePath->text() + ".json";
        break;
    case 4: //WPL
        exporter->format   = Format::WPL;
        exporter->SavePath = ui->exportSavePath->text() + ".wpl";
        break;
    case 5: //ASX
        exporter->format   = Format::ASX;
        exporter->SavePath = ui->exportSavePath->text() + ".asx";
        break;
    case 6: //MPCPL
        exporter->format   = Format::MPCPL;
        exporter->SavePath = ui->exportSavePath->text() + ".mpcpl";
        break;
    case 7: //B4S
        exporter->format   = Format::B4S;
        exporter->SavePath = ui->exportSavePath->text() + ".b4s";
        break;
    case 8: //HTML
        exporter->format   = Format::HTML;
        exporter->SavePath = ui->exportSavePath->text() + ".html";
        break;
    case 9: //TXT
        exporter->format   = Format::TXT;
        exporter->SavePath = ui->exportSavePath->text() + ".txt";
        break;
    }

    exporter->PlaylistTitle = ui->exportPlaylistTitle->text();
    exporter->Performance   = ui->ExportingPerformanceSlider->value();
    exporter->Codec         = ui->exportingCodec->currentText();

    exporter->PathPrefix        = ui->PathPrefix->isChecked();
    exporter->PathPrefixFind    = ui->txtPathPrefixFind->text();
    exporter->PathPrefixReplace = ui->txtPathPrefixReplace->text();
    exporter->caseSensitive     = ui->chkPathPrefixCaseSensitive->isChecked();

    exporter->PathPrefixFailExclude = ui->chkExportExcludeFailedPathPrefix->isChecked();
    exporter->CheckFileExistence    = ui->chkExportCheckFileExistence->isChecked();
    exporter->ShowInFolderWhenDone  = ui->chkExportShowInDir->isChecked();
    exporter->ShowMsgWhenDone       = ui->chkExportShowMsgWhenDone->isChecked();
    exporter->ExecuteCMD            = ui->chkExportExecuteCMD->isChecked();
    exporter->CMD                   = ui->txtCMD->text();

    connect(exporter, &Exporter::log, this, &PCxx::UpdateLogs);
    connect(exporter, &Exporter::ShowMessageBox, this, &PCxx::ShowMessageBox);
    connect(exporter, &Exporter::setPBMax, this, &PCxx::UpdateLoadingPBMax);
    connect(exporter, &Exporter::setPB, this, &PCxx::UpdateLoadingPB);
    connect(exporter, &Exporter::disableUI, this, &PCxx::FilesAreExporting);

    QThread *thread = new QThread; //Thread must be created on heap or it will be destroyed with this scope
    connect(thread, SIGNAL(started()), exporter, SLOT(Export())); //void Exporter::Export() is what starts the process

    exporter->moveToThread(thread); //Moving exporter object to different thread so it won't block UI
    thread->start(); //Goodluck!
}

void PCxx::on_btnCancelExportation_clicked()
{
    _CANCEL_EXPORTING = true;
}

void PCxx::on_btnBrowseSavePath_clicked()
{
    const QString savePathSD = QFileDialog::getSaveFileName(this, tr("Where to save the new playlist..."), QString(), QString());

    if (savePathSD.isEmpty())
        return;

    ui->exportSavePath->setText(savePathSD);
}

void PCxx::on_chkShowFilesListHeader_toggled(bool checked)
{
    ui->filesList->horizontalHeader()->setHidden(!checked);
}

void PCxx::on_chkShowFilesListGrid_toggled(bool checked)
{
    ui->filesList->setShowGrid(checked);
}

void PCxx::on_chkAlternateFilesListColors_toggled(bool checked)
{
    ui->filesList->setAlternatingRowColors(checked);
}

void PCxx::on_tabsStyle_currentIndexChanged(int index)
{
    if (index == 0) ui->tabWidget->setTabShape(QTabWidget::Rounded);
    else if (index == 1) ui->tabWidget->setTabShape(QTabWidget::Triangular);
}

void PCxx::on_tabsPosition_currentIndexChanged(int index)
{
    switch (index) {
    case 0: //Top
        ui->tabWidget->setTabPosition(QTabWidget::North);
        break;
    case 1: //Left
        ui->tabWidget->setTabPosition(QTabWidget::West);
        break;
    case 2: //Bottom
        ui->tabWidget->setTabPosition(QTabWidget::South);
        break;
    case 3: //Right
        ui->tabWidget->setTabPosition(QTabWidget::East);
        break;
    default:
        ui->tabWidget->setTabPosition(QTabWidget::North);
        break;
    }
}

void PCxx::on_btnAddDirectory_clicked()
{
    const QString dirPath = QFileDialog::getExistingDirectory(this, tr("Select a directory to load..."), QString());

    if (dirPath.isEmpty())
        return;

    QDir dir(dirPath); QStringList files;

    if (!ui->txtDirFilter->text().isEmpty()) {
        if (ui->txtDirFilter->text().contains(" ")) {
            files = filterDir(ui->txtDirFilter->text(), dir.entryList(QDir::Files), ui->chkDirFilterCaseSensitive->isChecked());
        }
        else if (ui->txtDirFilter->text().length() > 0) {
            files = filterDir(ui->txtDirFilter->text(), dir.entryList(QDir::Files), ui->chkDirFilterCaseSensitive->isChecked());
        }
    }
    else files = dir.entryList(QDir::Files);

    FileReader *fileReader = new FileReader;

    fileReader->dir = dirPath;
    fileReader->files = files;
    fileReader->Performance = ui->LoadingPerformanceSlider->value();
    fileReader->ShowMsgWhenDone = ui->chkLoadingShowMsgWhenDone->isChecked();

    //Connecting signals and slots (QT 5+ way)
    connect(fileReader, &FileReader::log, this, &PCxx::UpdateLogs);
    connect(fileReader, &FileReader::ShowMessageBox, this, &PCxx::ShowMessageBox);
    connect(fileReader, &FileReader::setPBMax, this, &PCxx::UpdateLoadingPBMax);
    connect(fileReader, &FileReader::setPB, this, &PCxx::UpdateLoadingPB);
    connect(fileReader, &FileReader::disableUI, this, &PCxx::FilesAreLoading);
    connect(fileReader, &FileReader::UpdateStats, this, &PCxx::UpdateStats);
    connect(fileReader, &FileReader::UpdateFilesList, this, &PCxx::UpdateFilesList);

    QThread *thread = new QThread; //Thread must be created on heap or it will be destroyed with this scope
    connect(thread, &QThread::started, fileReader, &FileReader::Read); //void FileReader::Read is what starts the process

    fileReader->moveToThread(thread); //Moving file reader object to different thread so it won't block UI
    thread->start(); //Goodluck!
}

void PCxx::on_btnAddFiles_clicked()
{
    const QStringList files = QFileDialog::getOpenFileNames(this, tr("Select some media files..."), QString(), "All files (*.*)");

    if (files.isEmpty())
        return;

    FileReader *fileReader = new FileReader;
    fileReader->dir = QString();
    fileReader->files = files;
    fileReader->Performance = ui->LoadingPerformanceSlider->value();
    fileReader->ShowMsgWhenDone = ui->chkLoadingShowMsgWhenDone->isChecked();

    //Connecting signals and slots (QT 5+ way)
    connect(fileReader, &FileReader::log, this, &PCxx::UpdateLogs);
    connect(fileReader, &FileReader::ShowMessageBox, this, &PCxx::ShowMessageBox);
    connect(fileReader, &FileReader::setPBMax, this, &PCxx::UpdateLoadingPBMax);
    connect(fileReader, &FileReader::setPB, this, &PCxx::UpdateLoadingPB);
    connect(fileReader, &FileReader::disableUI, this, &PCxx::FilesAreLoading);
    connect(fileReader, &FileReader::UpdateStats, this, &PCxx::UpdateStats);
    connect(fileReader, &FileReader::UpdateFilesList, this, &PCxx::UpdateFilesList);

    QThread *thread = new QThread;
    connect(thread, &QThread::started, fileReader, &FileReader::Read); //void FileReader::Read is what starts the process

    fileReader->moveToThread(thread); //Moving file reader object to different thread so it won't block UI
    thread->start(); //Goodluck!
}
