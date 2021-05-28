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


#ifndef PCXX_H
#define PCXX_H

#include <QMainWindow>
#include <QDragEnterEvent>
#include <QCloseEvent>

#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QMimeData>

#include <QElapsedTimer>
#include <QMessageBox>
#include <QtDebug>

#include <QTableWidget>
#include <QTableWidgetItem>

#include <QThread>

#include <QAction>
#include <QMenu>


#include <fileReader.h>
#include <parser.h>
#include <exporter.h>
#include <id3Fixer.h>
#include <types.h>


QT_BEGIN_NAMESPACE
namespace Ui { class PCxx; }
QT_END_NAMESPACE


class PCxx : public QMainWindow
{
    Q_OBJECT

public:
    PCxx(QWidget *parent = nullptr);
    ~PCxx();

public slots:
    void UpdateLoadingPBMax(const int value);
    void UpdateLoadingPB(const int value);

    void UpdateExportingPBMax(const int value);
    void UpdateExportingPB(const int value);

    void UpdateLogs(levelType type, const QString &log);

    void FilesAreLoading(const bool really);
    void FilesAreExporting(const bool really);

    void UpdateStats();
    void UpdateFilesList();

    void ShowMessageBox(levelType type, const QString &title, const QString &msg);

    void UpdateAboutDescription(const QChar &letter);


private slots:
    void onFilesListItemChanged(QTableWidgetItem *item);

    void on_btnAddPlaylist_clicked();

    void on_btnClearFiles_clicked();

    void on_btnStopLoading_clicked();

    void on_btnClearLogs_clicked();

    void on_btnSaveLogs_clicked();

    void on_LogsFontSize_currentIndexChanged(int index);

    void on_chkExportExecuteCMD_toggled(bool checked);

    void on_btnExport_clicked();

    void on_btnCancelExportation_clicked();

    void on_btnBrowseSavePath_clicked();

    void on_chkShowFilesListHeader_toggled(bool checked);

    void on_chkShowFilesListGrid_toggled(bool checked);

    void on_chkAlternateFilesListColors_toggled(bool checked);

    void on_btnAddDirectory_clicked();

    void on_btnAddFiles_clicked();

    void on_btnFixID3_clicked();

    void on_tabWidget_tabBarClicked(int index);

    void on_tabsStyle_currentIndexChanged(int index);

    void on_tabsPosition_currentIndexChanged(int index);

private:
    Ui::PCxx *ui;

    QAction *select;
    QAction *highlight;
    QMenu *selectMenu;
    QMenu *highlightMenu;
    QAction *selectAllAction;
    QAction *highlightEmptyAction;
    QAction *clearSelectionAction;
    QAction *delAction;
    QAction *showInFolderAction;

    void deleteFilesListItem();
    void showFilesListItemInFolder();
    void selectAllItems();
    void highlightEmptyItems();
    void clearSelections();

    void UpdateAboutPage();

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    void closeEvent(QCloseEvent *event);
};

extern bool _FILES_BUSY_LOADING;
extern bool _PLAYLIST_BUSY_LOADING;
extern bool _PLAYLIST_BUSY_EXPORTING;
extern bool _FILES_ARE_GETTING_FIXED;

#endif // PCXX_H
