/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez / xtingray                                          *
 *                                                                         *
 *   KTooN's versions:                                                     * 
 *                                                                         *
 *   2006:                                                                 *
 *    David Cuadrado                                                       *
 *    Jorge Cuadrado                                                       *
 *   2003:                                                                 *
 *    Fernado Roldan                                                       *
 *    Simena Dinas                                                         *
 *                                                                         *
 *   Copyright (C) 2010 Gustav Gonzalez - http://www.maefloresta.com       *
 *   License:                                                              *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "tuplibrarywidget.h"
#include "tglobal.h"
#include "tapplication.h"
#include "toptionaldialog.h"
#include "tconfig.h"
#include "tuplibrary.h"
#include "tupproject.h"
#include "tupsymboleditor.h"
#include "tuprequestbuilder.h"
#include "tosd.h"
#include "talgorithm.h"
// #include "taudioplayer.h"
#include "tdebug.h"

#include <QApplication>
#include <QGroupBox>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDesktopWidget>
#include <QBuffer>
#include <QGraphicsSvgItem>
#include <QSvgRenderer>
#include <QSvgGenerator>
#include <QComboBox>
#include <QTreeWidgetItemIterator>
#include <QProcess>
#include <QFileSystemWatcher>
#include <QChar>
#include <QPainter>

#include <cstdlib>
#include <ctime>

#define RETURN_IF_NOT_LIBRARY if (!k->library) return;

/**
 * This class defines the Library widget.
 * Here is where events around the Library widget are handled.
 * @author David Cuadrado
*/

struct TupLibraryWidget::Private
{
    Private() : library(0)
    {
       currentFrame.frame = 0;
       currentFrame.layer = 0;
       currentFrame.scene = 0;
    }

    TupLibrary *library;
    TupProject *project;
    TupItemPreview *display;
    TupItemManager *libraryTree;
    int childCount;
    QDir libraryDir;
    QComboBox *itemType;
    int currentPlayerId;
    QString oldId;
    bool renaming;
    bool mkdir;
    bool isNetworked;
    QTreeWidgetItem *lastItemEdited;
    QTreeWidgetItem *currentItemDisplayed;
    QFileSystemWatcher *watcher;

    struct Frame
    {
       int scene;
       int layer;
       int frame;
    } currentFrame;
};

TupLibraryWidget::TupLibraryWidget(QWidget *parent) : TupModuleWidgetBase(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    k->childCount = 0;
    k->renaming = false;
    k->mkdir = false;

    setWindowIcon(QPixmap(THEME_DIR + "icons/library.png"));
    setWindowTitle(tr("Library"));

    k->libraryDir = QDir(CONFIG_DIR + "libraries");
    k->display = new TupItemPreview(this);
    k->libraryTree = new TupItemManager(this);

    connect(k->libraryTree, SIGNAL(itemSelected(QTreeWidgetItem *)), this,
                                   SLOT(previewItem(QTreeWidgetItem *)));

    connect(k->libraryTree, SIGNAL(itemRemoved()), this,
                                   SLOT(removeCurrentGraphic()));

    connect(k->libraryTree, SIGNAL(itemCloned(QTreeWidgetItem*)), this,
                                   SLOT(cloneObject(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(itemExported(QTreeWidgetItem*)), this,
                                   SLOT(exportObject(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(itemRenamed(QTreeWidgetItem*)), this, 
                                   SLOT(renameObject(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this,
                                   SLOT(refreshItem(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(itemMoved(QString, QString)), this,
                                   SLOT(updateLibrary(QString, QString)));

    connect(k->libraryTree, SIGNAL(itemRequired()), this,
                                   SLOT(insertObjectInWorkspace()));

    connect(k->libraryTree, SIGNAL(itemCreated(QTreeWidgetItem*)), this,
                                   SLOT(activeRefresh(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(inkscapeEditCall(QTreeWidgetItem*)), this,
                                   SLOT(openInkscapeToEdit(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(gimpEditCall(QTreeWidgetItem*)), this,
                                   SLOT(openGimpToEdit(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(kritaEditCall(QTreeWidgetItem*)), this,
                                   SLOT(openKritaToEdit(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(myPaintEditCall(QTreeWidgetItem*)), this,
                                   SLOT(openMyPaintToEdit(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(newRasterCall()), this,
                                   SLOT(createRasterObject()));

    connect(k->libraryTree, SIGNAL(newVectorCall()), this,
                                   SLOT(createVectorObject()));

    QGroupBox *buttons = new QGroupBox(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttons);
    buttonLayout->setMargin(0);
    buttonLayout->setSpacing(0);

    QHBoxLayout *comboLayout = new QHBoxLayout;
    comboLayout->setMargin(0);
    comboLayout->setSpacing(0);

    k->itemType = new QComboBox();
    k->itemType->setIconSize(QSize(15, 15));
    k->itemType->setMaximumWidth(120);
    
    k->itemType->addItem(QIcon(THEME_DIR + "icons/bitmap.png"), tr("Image"));
    k->itemType->addItem(QIcon(THEME_DIR + "icons/svg.png"), tr("Svg File"));
    k->itemType->addItem(QIcon(THEME_DIR + "icons/bitmap_array.png"), tr("Image Array"));
    k->itemType->addItem(QIcon(THEME_DIR + "icons/svg_array.png"), tr("Svg Array"));
    comboLayout->addWidget(k->itemType);

    connect(k->itemType, SIGNAL(currentIndexChanged(int)), this, SLOT(importGraphicObject()));

    TImageButton *addGC = new TImageButton(QPixmap(THEME_DIR + "icons/plus_sign.png"), 22, buttons);
    addGC->setToolTip(tr("Add an object to library"));
    connect(addGC, SIGNAL(clicked()), this, SLOT(importGraphicObject()));
    comboLayout->addWidget(addGC);

    buttonLayout->addLayout(comboLayout);

    TImageButton *addFolderGC = new TImageButton(QPixmap(THEME_DIR + "icons/create_folder.png"), 22, buttons);
    connect(addFolderGC, SIGNAL(clicked()), this, SLOT(addFolder()));
    addFolderGC->setToolTip(tr("Adds a folder to the object's list"));
    buttonLayout->addWidget(addFolderGC);
    // SQA: Temporary code
    // addFolderGC->setEnabled(false);

    TImageButton *gctoDrawingArea = new TImageButton(QPixmap(THEME_DIR + "icons/library_to_ws.png"), 22, buttons);
    connect(gctoDrawingArea, SIGNAL(clicked()), this, SLOT(insertObjectInWorkspace()));
    gctoDrawingArea->setToolTip(tr("Inserts the selected object into the drawing area"));
    buttonLayout->addWidget(gctoDrawingArea);

    buttons->setLayout(buttonLayout);

    addChild(k->display);
    addChild(buttons);
    addChild(k->libraryTree);

    k->watcher = new QFileSystemWatcher(this);
    connect(k->watcher, SIGNAL(fileChanged(QString)), this, SLOT(updateItemFromSaveAction()));
}


TupLibraryWidget::~TupLibraryWidget()
{
    #ifdef K_DEBUG
           TEND;
    #endif
    delete k;
}

void TupLibraryWidget::resetGUI()
{
    k->library->reset();
    k->display->reset();
    k->libraryTree->cleanUI();
}

void TupLibraryWidget::setLibrary(TupLibrary *library)
{
    k->library = library;
    k->project = library->project(); 
}

void TupLibraryWidget::setNetworking(bool isNetworked)
{
    k->isNetworked = isNetworked;
}

void TupLibraryWidget::addFolder()
{
    k->libraryTree->createFolder();
    k->mkdir = true;
}

void TupLibraryWidget::activeRefresh(QTreeWidgetItem *item)
{
    k->mkdir = true;
    refreshItem(item);
}

void TupLibraryWidget::previewItem(QTreeWidgetItem *item)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    RETURN_IF_NOT_LIBRARY;

    if (item) {

        k->currentItemDisplayed = item;

        if (item->text(2).length() == 0) {
            QGraphicsTextItem *msg = new QGraphicsTextItem(tr("Directory"));
            k->display->render(static_cast<QGraphicsItem *>(msg));
            return;
        }

        TupLibraryObject *object = k->library->findObject(item->text(1) + "." + item->text(2).toLower());

        if (!object) {
            #ifdef K_DEBUG
                   tError() << "TupLibraryWidget::previewItem() - Fatal Error: Cannot find the object: " << item->text(1) + "." + item->text(2).toLower();
            #endif
            QGraphicsTextItem *msg = new QGraphicsTextItem(tr("No preview available"));
            k->display->render(static_cast<QGraphicsItem *>(msg));

            return;
        }

        switch (object->type()) {
                case TupLibraryObject::Svg:
                   {
                     QGraphicsSvgItem *svg = new QGraphicsSvgItem(object->dataPath()); 
                     k->display->render(static_cast<QGraphicsItem *>(svg)); 
                   }
                   break;
                case TupLibraryObject::Image:
                case TupLibraryObject::Item:
                   {
                     if (object->data().canConvert<QGraphicsItem *>()) {

                         k->display->render(qvariant_cast<QGraphicsItem *>(object->data()));

                         /* SQA: Just a test
                         TupSymbolEditor *editor = new TupSymbolEditor;
                         editor->setSymbol(object);
                         emit postPage(editor);
                         */    
                     } 
                   }
                   break;
                case TupLibraryObject::Sound:
                   {
                     /*
                     TAudioPlayer::instance()->setCurrentPlayer(k->currentPlayerId);
                     TAudioPlayer::instance()->stop();

                     k->currentPlayerId = TAudioPlayer::instance()->load(object->dataPath());
                     TAudioPlayer::instance()->play(0);
                     */
                   }
                   break;
                default:
                   {
                     #ifdef K_DEBUG
                            tDebug("library") << "Unknown symbol id: " << object->type();
                     #endif
                   }
                   break;
        }
    } else {
        QGraphicsTextItem *msg = new QGraphicsTextItem(tr("No preview available"));
        k->display->render(static_cast<QGraphicsItem *>(msg));
    }
}

void TupLibraryWidget::insertObjectInWorkspace()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (!k->libraryTree->currentItem()) { 
        #ifdef K_DEBUG
               tFatal() << "TupLibraryWidget::insertObjectInWorkspace() - There's no current selection!";
        #endif
        return;
    } else if (k->libraryTree->currentItem()->text(2).length() == 0) {
               #ifdef K_DEBUG
                      tFatal() << "TupLibraryWidget::insertObjectInWorkspace() - It's a directory!";
               #endif
               return;
    }

    QString objectKey = k->libraryTree->currentItem()->text(1) + "." + k->libraryTree->currentItem()->text(2).toLower();

    TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::InsertSymbolIntoFrame, objectKey,
                                TupLibraryObject::Type(k->libraryTree->currentItem()->data(1, 3216).toInt()), k->project->spaceContext(), 
                                0, QString(), k->currentFrame.scene, k->currentFrame.layer, k->currentFrame.frame);

    emit requestTriggered(&request);
}

void TupLibraryWidget::removeCurrentGraphic()
{
    if (!k->libraryTree->currentItem()) 
        return;

    QString extension = k->libraryTree->currentItem()->text(2);

    QString objectKey = k->libraryTree->currentItem()->text(1);
    TupLibraryObject::Type type = TupLibraryObject::Folder;

    // If it's NOT a directory
    if (extension.length() > 0) {
        objectKey = k->libraryTree->currentItem()->text(3);
        if (extension.compare("JPG")==0 || extension.compare("PNG")==0 || extension.compare("GIF")==0)
            type = TupLibraryObject::Image;
        if (extension.compare("SVG")==0)
            type = TupLibraryObject::Svg;
        if (extension.compare("OBJ")==0)
            type = TupLibraryObject::Item;
    } 

    TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::RemoveSymbolFromFrame, 
                                                   objectKey, type, k->project->spaceContext(), 0, QString(),
                                                   k->currentFrame.scene, k->currentFrame.layer, k->currentFrame.frame);
    emit requestTriggered(&request);
}

void TupLibraryWidget::cloneObject(QTreeWidgetItem* item)
{
    if (item) {
        QString id = item->text(3);
        TupLibraryObject *object = k->library->findObject(id);

        if (object) {
            QString smallId = object->smallId();
            QString extension = object->extension();
            TupLibraryObject::Type type = object->type();
            QString path = object->dataPath();
            int limit = path.lastIndexOf(QDir::separator());
            QString newPath = path.left(limit + 1); 

            QString symbolName = "";

            if (itemNameEndsWithDigit(smallId)) {
                int index = getItemNameIndex(smallId);
                symbolName = nameForClonedItem(smallId, extension, index, newPath);
                newPath += symbolName;
            } else {
                symbolName = nameForClonedItem(smallId, extension, newPath);
                newPath += symbolName;
            }

            QString baseName = symbolName.section('.', 0, 0);
            baseName = verifyNameAvailability(baseName, extension, true);
            symbolName = baseName + "." + extension.toLower();

            bool isOk = QFile::copy(path, newPath);

            if (!isOk) {
                #ifdef K_DEBUG
                       tError() << "TupLibraryWidget::cloneObject() - Fatal Error: Object file couldn't be cloned!";
                #endif
                return;
            }

            TupLibraryObject *newObject = new TupLibraryObject();
            newObject->setSymbolName(symbolName);
            newObject->setType(type);
            newObject->setDataPath(newPath);
            isOk = newObject->loadData(newPath);

            if (!isOk) {
                #ifdef K_DEBUG
                       tError() << "TupLibraryWidget::cloneObject() - Fatal Error: Object file couldn't be loaded!";
                #endif
                return;
            } 

            k->library->addObject(newObject);

            QTreeWidgetItem *item = new QTreeWidgetItem(k->libraryTree);
            item->setText(1, baseName);
            item->setText(2, extension);
            item->setText(3, symbolName);
            item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

            switch (object->type()) {
                    case TupLibraryObject::Item:
                         {
                             item->setIcon(0, QIcon(THEME_DIR + "icons/drawing_object.png"));
                             k->libraryTree->setCurrentItem(item);
                             previewItem(item);
                         }
                         break;
                    case TupLibraryObject::Image:
                         {
                             item->setIcon(0, QIcon(THEME_DIR + "icons/bitmap.png"));
                             k->libraryTree->setCurrentItem(item);
                             previewItem(item);
                         }
                         break;
                    case TupLibraryObject::Svg:
                         {
                             item->setIcon(0, QIcon(THEME_DIR + "icons/svg.png"));
                             k->libraryTree->setCurrentItem(item);
                             previewItem(item);
                         }
                         break;
                    case TupLibraryObject::Sound:
                         {
                             item->setIcon(0, QIcon(THEME_DIR + "icons/sound_object.png"));
                         }
                         break;
                    default:
                         {
                         }
                         break;
            }

        } else {
            #ifdef K_DEBUG
                   tError() << "TupLibraryWidget::cloneObject() - Fatal Error: Object doesn't exist! [ " << id << " ]";
            #endif
            return;
        }
    }
}

void TupLibraryWidget::exportObject(QTreeWidgetItem *item)
{
    QString id = item->text(3);
    TupLibraryObject *object = k->library->findObject(id);
    if (object) {
        QString path = object->dataPath();
        if (path.length() > 0) {
            QString fileExtension = object->extension();
            QString filter = tr("Images") + " ";

            if (fileExtension.compare("PNG") == 0)
                filter += "(*.png)"; 
            if ((fileExtension.compare("JPG") == 0) || (fileExtension.compare("JPEG") == 0))
                filter += "(*.jpg *.jpeg)";
            if (fileExtension.compare("GIF") == 0)
                filter += "(*.gif)";
            if (fileExtension.compare("XPM") == 0)
                filter += "(*.xpm)";
            if (fileExtension.compare("SVG") == 0)
                filter += "(*.svg)";

            QString target = QFileDialog::getSaveFileName(this, tr("Export object..."), QDir::homePath(), filter);
            if (target.isEmpty())
                return;

            if (QFile::exists(target)) {
                if (!QFile::remove(target)) {
                    #ifdef K_DEBUG
                           tError() << "TupLibraryWidget::exportObject() - Fatal Error: destination path already exists! [ " << id << " ]";
                    #endif
                    return;
                }
            }

            if (!QFile::copy(path, target)) {
                #ifdef K_DEBUG
                       tError() << "TupLibraryWidget::exportObject() - Error: Object file couldn't be exported! [ " << id << " ]";
                #endif
                return;
            } else {
                TOsd::self()->display(tr("Info"), tr("Item exported successfully!"), TOsd::Info);
            }
        } else {
            #ifdef K_DEBUG
                   tError() << "TupLibraryWidget::exportObject() - Error: Object path is null! [ " << id << " ]";
            #endif
            return;
        }
    } else {
        #ifdef K_DEBUG
               tError() << "TupLibraryWidget::exportObject() - Error: Object doesn't exist! [ " << id << " ]";
        #endif
        return;
    }
}

void TupLibraryWidget::renameObject(QTreeWidgetItem *item)
{
    if (item) {
        k->renaming = true;
        k->oldId = item->text(1);
        k->libraryTree->editItem(item, 1);
    }
}

void TupLibraryWidget::createRasterObject()
{
    QString name = "object00";
    QString extension = "PNG";
    name = verifyNameAvailability(name, extension, true);

    QSize size = k->project->dimension();
    int w = QString::number(size.width()).length();
    int h = QString::number(size.height()).length();

    int width = 1;
    int height = 1; 
    for(int i=0; i<w; i++)
        width *= 10;
    for(int i=0; i<h; i++)
        height *= 10;

    size = QSize(width, height);

    TupNewItemDialog dialog(name, TupNewItemDialog::Raster, size);
    if (dialog.exec() == QDialog::Accepted) {
        QString name = dialog.itemName();
        QSize size = dialog.itemSize();
        QColor background = dialog.background();
        QString extension = dialog.itemExtension();
        TupNewItemDialog::ThirdParty editor = dialog.software();

        QString imagesDir = k->project->dataDir() + "/images/";
        if (!QFile::exists(imagesDir)) {
            QDir dir;
            if (!dir.mkpath(imagesDir)) {
                #ifdef K_DEBUG
                       tError() << "TupLibraryWidget::createRasterObject() - Fatal Error: Couldn't create directory " << imagesDir;
                #endif
                TOsd::self()->display(tr("Error"), tr("Couldn't create images directory!"), TOsd::Error);
                return;
            }
        }
             
        QString path = imagesDir + name + "." + extension.toLower();
        QString symbolName = name; 
        if (QFile::exists(path)) {
            symbolName = nameForClonedItem(name, extension, imagesDir);
            path = imagesDir + symbolName + "." + extension.toLower();
        }

        symbolName += "." + extension.toLower();

        QImage::Format format = QImage::Format_RGB32;
        if (extension.compare("PNG")==0)
            format = QImage::Format_ARGB32;

        QImage *image = new QImage(size, format); 
        image->fill(background);

        bool isOk = image->save(path);

        if (isOk) {
            TupLibraryObject *newObject = new TupLibraryObject();
            newObject->setSymbolName(symbolName);
            newObject->setType(TupLibraryObject::Image);
            newObject->setDataPath(path);
            isOk = newObject->loadData(path);

            if (isOk) {
                k->library->addObject(newObject);

                QTreeWidgetItem *item = new QTreeWidgetItem(k->libraryTree);
                item->setText(1, name);
                item->setText(2, extension);
                item->setText(3, symbolName);
                item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
                item->setIcon(0, QIcon(THEME_DIR + "icons/bitmap.png"));
                k->libraryTree->setCurrentItem(item);

                previewItem(item);

                k->lastItemEdited = item;
                executeSoftware(editor, path);
            } else {
                #ifdef K_DEBUG
                       tError() << "TupLibraryWidget::createRasterObject() - Fatal Error: Object file couldn't be loaded from -> " << path;
                #endif
                return;
            }
        } else {
            #ifdef K_DEBUG
                   tError() << "TupLibraryWidget::createRasterObject() - Fatal Error: Object file couldn't be saved at -> " << path;
            #endif
            return;
        }
    }
}

void TupLibraryWidget::createVectorObject()
{
    QString name = "object00";
    QString extension = "SVG";
    name = verifyNameAvailability(name, extension, true);

    QSize size = k->project->dimension();
    int w = QString::number(size.width()).length();
    int h = QString::number(size.height()).length();

    int width = 1;
    int height = 1;
    for(int i=0; i<w; i++) 
        width *= 10;
    for(int i=0; i<h; i++) 
        height *= 10;

    size = QSize(width, height);

    TupNewItemDialog dialog(name, TupNewItemDialog::Vector, size);
    if (dialog.exec() == QDialog::Accepted) {
        QString name = dialog.itemName();
        QSize size = dialog.itemSize();
        QString extension = dialog.itemExtension();
        TupNewItemDialog::ThirdParty editor = dialog.software();

        QString vectorDir = k->project->dataDir() + "/svg/";
        if (!QFile::exists(vectorDir)) {
            QDir dir;
            if (!dir.mkpath(vectorDir)) {
                #ifdef K_DEBUG
                       tError() << "TupLibraryWidget::createVectorObject() - Fatal Error: Couldn't create directory " << vectorDir;
                #endif
                TOsd::self()->display(tr("Error"), tr("Couldn't create vector directory!"), TOsd::Error);
                return;
            }
        }

        QString path = vectorDir + name + "." + extension.toLower();
        QString symbolName = name;
        if (QFile::exists(path)) {
            symbolName = nameForClonedItem(name, extension, vectorDir);
            path = vectorDir + symbolName + "." + extension.toLower();
        }

        symbolName += "." + extension.toLower();

        QSvgGenerator generator;
        generator.setFileName(path);
        generator.setSize(size);
        generator.setViewBox(QRect(0, 0, size.width(), size.height()));
        generator.setTitle(name);
        generator.setDescription(tr("Tupi library item"));
        QPainter painter;
        painter.begin(&generator);
        bool isOk = painter.end();

        if (isOk) {
            TupLibraryObject *newObject = new TupLibraryObject();
            newObject->setSymbolName(symbolName);
            newObject->setType(TupLibraryObject::Svg);
            newObject->setDataPath(path);
            isOk = newObject->loadData(path);

            if (isOk) {
                k->library->addObject(newObject);
                QTreeWidgetItem *item = new QTreeWidgetItem(k->libraryTree);
                item->setText(1, name);
                item->setText(2, extension);
                item->setText(3, symbolName);
                item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
                item->setIcon(0, QIcon(THEME_DIR + "icons/svg.png"));

                k->libraryTree->setCurrentItem(item);
                previewItem(item);

                k->lastItemEdited = item;
                executeSoftware(editor, path);
            } else {
                #ifdef K_DEBUG
                       tError() << "TupLibraryWidget::createVectorObject() - Fatal Error: Object file couldn't be loaded from -> " << path;
                #endif
                return;
            }
        } else {
                #ifdef K_DEBUG
                       tError() << "TupLibraryWidget::createVectorObject() - Fatal Error: Object file couldn't be saved at -> " << path;
                #endif
                return;
        }

    }
}

void TupLibraryWidget::importBitmap()
{
    QString image = QFileDialog::getOpenFileName (this, tr("Import an image..."), QDir::homePath(),  
                                                  tr("Images") + " (*.png *.xpm *.jpg *.jpeg *.gif)");
    if (image.isEmpty()) 
        return;

    QFile f(image);
    QFileInfo fileInfo(f);

    QString symName = fileInfo.fileName().toLower();

    if (f.open(QIODevice::ReadOnly)) {
        QByteArray data = f.readAll();
        f.close();

        QPixmap *pixmap = new QPixmap(image);
        int picWidth = pixmap->width();
        int picHeight = pixmap->height();
        int projectWidth = k->project->dimension().width();
        int projectHeight = k->project->dimension().height();

        #ifdef K_DEBUG
               tFatal() << "TupLibraryWidget::importBitmap() - Image filename: " << symName << " | Raw Size: " << data.size();
               tFatal() << "TupLibraryWidget::importBitmap() - Image Size: " << "[" << picWidth << ", " << picHeight << "]" << " | Project Size: " << "[" << projectWidth << ", " << projectHeight << "]";
        #endif

        if (picWidth > projectWidth || picHeight > projectHeight) {
            QDesktopWidget desktop;
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Information"));
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setText(tr("Image is bigger than workspace."));
            msgBox.setInformativeText(tr("Do you want to resize it?"));
            msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.show();
            msgBox.move((int) (desktop.screenGeometry().width() - msgBox.width())/2,
                        (int) (desktop.screenGeometry().height() - msgBox.height())/2);

            int answer = msgBox.exec();

            if (answer == QMessageBox::Yes) {
                pixmap = new QPixmap();
                QString extension = fileInfo.suffix().toUpper();
                // QByteArray ba = extension.toAscii();
                QByteArray ba = extension.toLatin1();
                const char* ext = ba.data();
                if (pixmap->loadFromData(data, ext)) {
                    QPixmap newpix;
                    if (picWidth > projectWidth)
                        newpix = QPixmap(pixmap->scaledToWidth(projectWidth, Qt::SmoothTransformation));
                    else
                        newpix = QPixmap(pixmap->scaledToHeight(projectHeight, Qt::SmoothTransformation));
                    QBuffer buffer(&data);
                    buffer.open(QIODevice::WriteOnly);
                    newpix.save(&buffer, ext);
                }
            } 
        }

        int i = 0;
        QString tag = symName;
        TupLibraryObject *object = k->library->findObject(tag);
        while (object) {
               i++;
               int index = symName.lastIndexOf(".");
               QString name = symName.mid(0, index);
               QString extension = symName.mid(index, symName.length() - index);
               tag = name + "-" + QString::number(i) + extension;
               object = k->library->findObject(tag);
        }

        TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, tag,
                                                                          TupLibraryObject::Image, k->project->spaceContext(), data, QString(), 
                                                                          k->currentFrame.scene, k->currentFrame.layer, k->currentFrame.frame);
        emit requestTriggered(&request);

        data.clear();
    } else {
        TOsd::self()->display(tr("Error"), tr("Cannot open file: %1").arg(image), TOsd::Error);
    }
}

void TupLibraryWidget::importSvg()
{
    QString svgPath = QFileDialog::getOpenFileName (this, tr("Import a SVG file..."), QDir::homePath(),
                                                    tr("Vector") + " (*.svg)");
    if (svgPath.isEmpty())
        return;

    QFile f(svgPath);
    QFileInfo fileInfo(f);

    QString symName = fileInfo.fileName().toLower();

    if (f.open(QIODevice::ReadOnly)) {
        QByteArray data = f.readAll();
        f.close();

        // SQA: This block is just for debugging!   
        tFatal() << "TupLibraryWidget::importSvg() - Inserting SVG into project: " << k->project->projectName();
        int projectWidth = k->project->dimension().width();
        int projectHeight = k->project->dimension().height();
        tFatal() << "TupLibraryWidget::importSvg() - Project Size: " << "[" << projectWidth << ", " << projectHeight << "]";

        int i = 0;
        QString tag = symName;
        TupLibraryObject *object = k->library->findObject(tag);
        while (object) {
               i++;
               int index = symName.lastIndexOf(".");
               QString name = symName.mid(0, index);
               QString extension = symName.mid(index, symName.length() - index);
               tag = name + "-" + QString::number(i) + extension;
               object = k->library->findObject(tag);
        }

        TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, tag,
                                                       TupLibraryObject::Svg, k->project->spaceContext(), data, QString(), 
                                                       k->currentFrame.scene, k->currentFrame.layer, k->currentFrame.frame);
        emit requestTriggered(&request);
    } else {
        TOsd::self()->display(tr("Error"), tr("Cannot open file: %1").arg(svgPath), TOsd::Error);
    }
}

void TupLibraryWidget::importBitmapArray()
{
    QString dir = getenv("HOME");
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose the images directory..."), dir,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if (path.isEmpty())
        return;

    QDir source(path); 
    QFileInfoList photograms = source.entryInfoList(QDir::Files, QDir::Name);
    int size = photograms.size();

    // Ensuring to get only graphic files here. Check extensions! (PNG, JPG, GIF, XPM) 

    int imagesCounter = 0; 
    for (int i = 0; i < size; ++i) {
         if (photograms.at(i).isFile()) {
             QString extension = photograms.at(i).suffix().toUpper();
             if (extension.compare("JPG")==0 || extension.compare("PNG")==0 || extension.compare("GIF")==0 || 
                 extension.compare("XPM")==0)
                 imagesCounter++;
         }
    }

    if (imagesCounter > 0) {

        QString text = tr("Image files found: %1.").arg(imagesCounter);
        bool resize = false;

        QPixmap *pixmap = new QPixmap(photograms.at(0).absoluteFilePath());
        int picWidth = pixmap->width();
        int picHeight = pixmap->height(); 
        int projectWidth = k->project->dimension().width();
        int projectHeight = k->project->dimension().height();

        if (picWidth > projectWidth || picHeight > projectHeight) {
            text = text + "\n" + tr("Files are too big, so they will be resized.") + "\n"
                   + tr("Note: This task can take a while.");
            resize = true;
        }

        QDesktopWidget desktop;
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Information"));  
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText(text);
        msgBox.setInformativeText(tr("Do you want to continue?"));
        msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.show();
        msgBox.move((int) (desktop.screenGeometry().width() - msgBox.width())/2, 
                    (int) (desktop.screenGeometry().height() - msgBox.height())/2);

        int answer = msgBox.exec();

        if (answer == QMessageBox::Ok) {

            QString directory = source.dirName();
            k->libraryTree->createFolder(directory);

            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

            QFont font = this->font();
            font.setPointSize(8);

            QProgressDialog progressDialog(this, Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint 
                                                 | Qt::Dialog);
            progressDialog.setFont(font);
            progressDialog.setLabelText(tr("Loading images..."));
            progressDialog.setCancelButton(0);
            progressDialog.setRange(1, size);
            progressDialog.show();
            int index = 1;

            progressDialog.move((int) (desktop.screenGeometry().width() - progressDialog.width())/2 , 
                                (int) (desktop.screenGeometry().height() - progressDialog.height())/2);

            TupLibraryFolder *folder = new TupLibraryFolder(directory, k->project);
            k->library->addFolder(folder);

            for (int i = 0; i < size; ++i) {
                 if (photograms.at(i).isFile()) {
                     QString extension = photograms.at(i).suffix().toUpper();
                     if (extension.compare("JPG")==0 || extension.compare("PNG")==0 || extension.compare("GIF")==0 ||
                         extension.compare("XPM")==0) {
                         QString path = photograms.at(i).absoluteFilePath(); 
                         QString symName = photograms.at(i).fileName().toLower();
                         QFile f(path);
                         QFileInfo fileInfo(f);

                         if (f.open(QIODevice::ReadOnly)) {

                             QByteArray data = f.readAll();
                             f.close();

                             if (resize) {
                                 pixmap = new QPixmap();
                                 QString extension = fileInfo.suffix().toUpper();
                                 // QByteArray ba = extension.toAscii();
                                 QByteArray ba = extension.toLatin1(); 
                                 const char* ext = ba.data();
                                 if (pixmap->loadFromData(data, ext)) {
                                     int width = projectWidth;
                                     QPixmap newpix(pixmap->scaledToWidth(width, Qt::SmoothTransformation));
                                     QBuffer buffer(&data);
                                     buffer.open(QIODevice::WriteOnly);
                                     newpix.save(&buffer, ext);
                                 }
                             }
                           
                             TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, symName,
                                                                          TupLibraryObject::Image, k->project->spaceContext(), data, directory);
                             emit requestTriggered(&request);

                             if (i < photograms.size()-1 && imagesCounter > 1) {

                                 TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->currentFrame.scene, k->currentFrame.layer, 
                                                                              k->currentFrame.frame + 1, TupProjectRequest::Add, tr("Frame %1").arg(k->currentFrame.frame + 2));
                                 emit requestTriggered(&request);

                                 request = TupRequestBuilder::createFrameRequest(k->currentFrame.scene, k->currentFrame.layer, k->currentFrame.frame + 1, 
                                                             TupProjectRequest::Select);
                                 emit requestTriggered(&request);
                             }

                             progressDialog.setLabelText(tr("Loading image #%1").arg(index));
                             progressDialog.setValue(index);
                             index++;
                         } else {
                             QMessageBox::critical(this, tr("ERROR!"), tr("ERROR: Can't open file %1. Please, check file permissions and try again.").arg(symName), QMessageBox::Ok);
                             QApplication::restoreOverrideCursor();
                             return;
                         }
                     }
                 }
             }

             QApplication::restoreOverrideCursor();

        }
    } else {
        TOsd::self()->display(tr("Error"), tr("No image files were found.<br/>Please, try another directory"), TOsd::Error);
    }
}

void TupLibraryWidget::importSvgArray() 
{
    QDesktopWidget desktop;
    QString dir = getenv("HOME");
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose the SVG files directory..."), dir,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if (path.isEmpty())
        return;

    QDir source(path); 
    QFileInfoList photograms = source.entryInfoList(QDir::Files, QDir::Name);
    int size = photograms.size();

    // Ensuring to get only SVG files here. Check extension! (SVG)
    int svgCounter = 0;
    for (int i = 0; i < size; ++i) {
         if (photograms.at(i).isFile()) {
             QString extension = photograms.at(i).suffix().toUpper();
             if (extension.compare("SVG")==0)
                 svgCounter++;
         }
    }

    if (svgCounter > 0) {

        QString testFile = photograms.at(0).absoluteFilePath();
        QFile file(testFile);
        file.close();

        QString text = tr("%1 SVG files will be loaded.").arg(svgCounter);

        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Information"));  
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText(text);
        msgBox.setInformativeText(tr("Do you want to continue?"));
        msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.show();
        msgBox.move((int) (desktop.screenGeometry().width() - msgBox.width())/2, 
                    (int) (desktop.screenGeometry().height() - msgBox.height())/2);

        int answer = msgBox.exec();

        if (answer == QMessageBox::Ok) {

            QString directory = source.dirName();
            k->libraryTree->createFolder(directory);

            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

            QFont font = this->font();
            font.setPointSize(8);

            QProgressDialog progressDialog(this, Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Dialog);
            progressDialog.setFont(font);
            progressDialog.setLabelText(tr("Loading SVG files..."));
            progressDialog.setCancelButton(0);
            progressDialog.setRange(1, size);
            progressDialog.show();
            int index = 1;

            progressDialog.move((int) (desktop.screenGeometry().width() - progressDialog.width())/2 , 
                                (int) (desktop.screenGeometry().height() - progressDialog.height())/2);

            TupLibraryFolder *folder = new TupLibraryFolder(directory, k->project);
            k->library->addFolder(folder);

            for (int i = 0; i < size; ++i) {
                 if (photograms.at(i).isFile()) {
                     QString extension = photograms.at(i).suffix().toUpper();
                     if (extension.compare("SVG")==0) {
                         QString path = photograms.at(i).absoluteFilePath(); 
                         QString symName = photograms.at(i).fileName().toLower();
                         QFile f(path);

                         if (f.open(QIODevice::ReadOnly)) {

                             QByteArray data = f.readAll();
                             f.close();

                             TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, symName,
                                                                                       TupLibraryObject::Svg, k->project->spaceContext(), data, directory);
                             emit requestTriggered(&request);

                             if (i < photograms.size()-1 && svgCounter > 1) {

                                 TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->currentFrame.scene, k->currentFrame.layer, 
                                                                              k->currentFrame.frame + 1, TupProjectRequest::Add, tr("Frame %1").arg(k->currentFrame.frame + 2));
                                 emit requestTriggered(&request);

                                 request = TupRequestBuilder::createFrameRequest(k->currentFrame.scene, k->currentFrame.layer, k->currentFrame.frame + 1, 
                                                             TupProjectRequest::Select);
                                 emit requestTriggered(&request);
                             }

                             progressDialog.setLabelText(tr("Loading SVG file #%1").arg(index));
                             progressDialog.setValue(index);
                             index++;

                         } else {
                             QMessageBox::critical(this, tr("ERROR!"), tr("ERROR: Can't open file %1. Please, check file permissions and try again.").arg(symName), QMessageBox::Ok);
                             QApplication::restoreOverrideCursor();
                             return;
                         }
                     }
                 }
             }

             QApplication::restoreOverrideCursor();

        }
    } else {
        TOsd::self()->display(tr("Error"), tr("No SVG files were found.<br/>Please, try another directory"), TOsd::Error);
    }
}

void TupLibraryWidget::importSound()
{
    QString sound = QFileDialog::getOpenFileName(this, tr("Import audio file..."), QDir::homePath(),
                                                 tr("Sound file") + " (*.ogg *.wav *.mp3)");

    if (sound.isEmpty()) 
        return;

    QFile f(sound);
    QFileInfo fileInfo(f);
    QString symName = fileInfo.baseName();

    if (f.open(QIODevice::ReadOnly)) {
        QByteArray data = f.readAll();
        f.close();

        TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, symName,
                                                     TupLibraryObject::Sound, k->project->spaceContext(), data);
        emit requestTriggered(&request);
    } else {
        TOsd::self()->display(tr("Error"), tr("Cannot open file: %1").arg(sound), TOsd::Error);
    }
}

void TupLibraryWidget::sceneResponse(TupSceneResponse *response)
{
    switch (response->action()) {
            case TupProjectRequest::Select:
            {
                 k->currentFrame.frame = 0;
                 k->currentFrame.layer = 0;
                 k->currentFrame.scene = response->sceneIndex();
            }
            break;
    }
}

void TupLibraryWidget::libraryResponse(TupLibraryResponse *response)
{
    RETURN_IF_NOT_LIBRARY;

    switch (response->action()) {
            case TupProjectRequest::Add:
              {
                 if (response->symbolType() == TupLibraryObject::Folder) {
                     k->libraryTree->createFolder(response->arg().toString());
                     return;
                 }

                 QString folderName = response->parent(); 
                 QString id = response->arg().toString();

                 int index = id.lastIndexOf(".");
                 QString name = id.mid(0, index);
                 QString extension = id.mid(index + 1, id.length() - index).toUpper();
                 TupLibraryObject *obj = k->library->findObject(id);

                 if (index < 0)
                     extension = "OBJ"; 

                 QTreeWidgetItem *item;
                 if (folderName.length() > 0 && folderName.compare("library")!=0)
                     item = new QTreeWidgetItem(k->libraryTree->getFolder(folderName));
                 else
                     item = new QTreeWidgetItem(k->libraryTree);

                 item->setText(1, name);
                 item->setText(2, extension);
                 item->setText(3, id);
                 item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

                 if (obj) {
                     switch (obj->type()) {
                            case TupLibraryObject::Item:
                               {
                                 item->setIcon(0, QIcon(THEME_DIR + "icons/drawing_object.png"));
                                 k->libraryTree->setCurrentItem(item);
                                 previewItem(item);
                               }
                            break;
                            case TupLibraryObject::Image:
                               {
                                 item->setIcon(0, QIcon(THEME_DIR + "icons/bitmap.png"));
                                 k->libraryTree->setCurrentItem(item);
                                 previewItem(item);
                                 if (!k->isNetworked && k->project->spaceContext() != TupProject::NONE && !k->library->loadingProject())
                                     insertObjectInWorkspace();
                               }
                            break;
                            case TupLibraryObject::Svg:
                               {
                                 item->setIcon(0, QIcon(THEME_DIR + "icons/svg.png"));
                                 k->libraryTree->setCurrentItem(item);
                                 previewItem(item);
                                 if (!k->isNetworked && k->project->spaceContext() != TupProject::NONE && !k->library->loadingProject())
                                     insertObjectInWorkspace();
                               }
                            break;
                            case TupLibraryObject::Sound:
                               {
                                 item->setIcon(0, QIcon(THEME_DIR + "icons/sound_object.png"));
                               }
                            break;
                            default:
                               {
                               }
                            break;
                     }

                 } else {
                     #ifdef K_DEBUG
                            tFatal() << "TupLibraryWidget::libraryResponse() - No object found: " << id;
                     #endif
                 }
              }
            break;
            case TupProjectRequest::InsertSymbolIntoFrame:
              {
                 #ifdef K_DEBUG
                        tFatal() << "*** TupLibraryWidget::libraryResponse() -> InsertSymbolIntoFrame : No action taken";
                 #endif
              }
            break;
            case TupProjectRequest::RemoveSymbolFromFrame:
              {
                 QString id = response->arg().toString();

                 QTreeWidgetItemIterator it(k->libraryTree);
                 while ((*it)) {
                        // If target is not a folder 
                        if ((*it)->text(2).length() > 0) {
                            if (id == (*it)->text(3)) {
                                delete (*it);
                                // k->library->removeObject(id, true);
                                break;
                            } 
                        } else {
                            // If target is a folder
                            if (id == (*it)->text(1)) {
                                delete (*it);
                                k->library->removeFolder(id);
                                break;
                            } 
                        }
                        ++it;
                 }

                 previewItem(k->libraryTree->currentItem());
              }
            break;
            case TupProjectRequest::Remove:
              {
                 #ifdef K_DEBUG
                        tFatal() << "*** TupLibraryWidget::libraryResponse() -> Remove : No action taken";
                 #endif
              }
            break;
            default:
              {
                 #ifdef K_DEBUG
                        tFatal() << "*** TupLibraryWidget::libraryResponse() - Unknown/Unhandled project action: " << response->action();
                 #endif
              }
            break;
    }
}

void TupLibraryWidget::frameResponse(TupFrameResponse *response)
{
    if (response->action() == TupProjectRequest::Select) {
        k->currentFrame.frame = response->frameIndex();
        k->currentFrame.layer = response->layerIndex();
        k->currentFrame.scene = response->sceneIndex();
    }
}

void TupLibraryWidget::importGraphicObject()
{
    QString option = k->itemType->currentText();

    if (option.compare(tr("Image")) == 0) {
        importBitmap();
        return;
    }

    if (option.compare(tr("Image Array")) == 0) {
        importBitmapArray();
        return;
    }

    if (option.compare(tr("Svg File")) == 0) {
        importSvg();
        return;
    }

    if (option.compare(tr("Svg Array")) == 0) {
        importSvgArray();
        return;
    }
}

void TupLibraryWidget::refreshItem(QTreeWidgetItem *item)
{
    if (k->mkdir) {
        k->mkdir = false;

        QString base = item->text(1);
        if (base.length() == 0)
            return;

        QString tag = base;
        int i = 0;
        while (k->library->folderExists(tag)) {
               int index = tag.lastIndexOf("-");
               if (index < 0) {
                   tag = base + "-1";
               } else {
                   QString name = base.mid(0, index);
                   i++;
                   tag = name + "-" + QString::number(i);
               }
        }

        item->setText(1, tag);
        TupLibraryFolder *folder = new TupLibraryFolder(tag, k->project);
        k->library->addFolder(folder);

        QGraphicsTextItem *msg = new QGraphicsTextItem(tr("Directory"));
        k->display->render(static_cast<QGraphicsItem *>(msg));

        return;
    }

    if (k->renaming) {
        // Renaming directory
        if (k->libraryTree->isFolder(item)) {
            QString base = item->text(1);
            if (k->oldId.length() == 0 || base.length() == 0)
                return;

            if (k->oldId.compare(base) == 0)
                return;

            int i = 0;
            QString tag = base;
            while (k->library->folderExists(tag)) {
                   int index = tag.lastIndexOf("-");
                   if (index < 0) {
                       tag = base + "-1";
                   } else {
                       QString name = base.mid(0, index);
                       i++;
                       tag = name + "-" + QString::number(i);
                   }
            }

            if (!k->library->folderExists(tag)) {
                // rename directory here!
                if (k->library->folderExists(k->oldId)) {
                    k->library->renameFolder(k->oldId, tag);
                    item->setText(1, tag);
                    k->library->renameFolder(k->oldId, tag);
                }
            } 
        } else {
            // Renaming item
            if (k->oldId.length() == 0)
                return;

            QString newId = item->text(1);
            QString extension = item->text(2);

            if (k->oldId.compare(newId) != 0) {
                newId = verifyNameAvailability(newId, extension, false);
                QString oldId = k->oldId + "." + extension.toLower();
                item->setText(1, newId);

                newId = newId + "." + extension.toLower();
                item->setText(3, newId);

                QTreeWidgetItem *parent = item->parent();
                if (parent) 
                    k->library->renameObject(parent->text(1), oldId, newId);
                else
                    k->library->renameObject("", oldId, newId);

                TupLibraryObject::Type type = TupLibraryObject::Image;
                if (extension.compare("SVG")==0)
                    type = TupLibraryObject::Svg;
                if (extension.compare("OBJ")==0)
                    type = TupLibraryObject::Item;

                k->project->updateSymbolId(type, oldId, newId);
            }
        }

        k->renaming = false;
    }
}

void TupLibraryWidget::updateLibrary(QString node, QString target) 
{
    if (target.length() > 0)
        k->library->moveObject(node, target);
    else
        k->library->moveObjectToRoot(node);
}

void TupLibraryWidget::openInkscapeToEdit(QTreeWidgetItem *item)
{
    callExternalEditor(item, TupNewItemDialog::Inkscape);
}

void TupLibraryWidget::openGimpToEdit(QTreeWidgetItem *item)
{
    callExternalEditor(item, TupNewItemDialog::Gimp);
}

void TupLibraryWidget::openKritaToEdit(QTreeWidgetItem *item)
{
    callExternalEditor(item, TupNewItemDialog::Krita);
}

void TupLibraryWidget::openMyPaintToEdit(QTreeWidgetItem *item)
{
    callExternalEditor(item, TupNewItemDialog::MyPaint);
}

void TupLibraryWidget::callExternalEditor(QTreeWidgetItem *item, TupNewItemDialog::ThirdParty software)
{
    if (item) {
        k->lastItemEdited = item;
        QString id = item->text(1) + "." + item->text(2).toLower();
        TupLibraryObject *object = k->library->findObject(id);

        if (object) {
            QString path = object->dataPath();
            executeSoftware(software, path);
        } else {
            #ifdef K_DEBUG
                   tError() << "TupLibraryWidget::callExternalEditor() - Fatal Error: No object related to the current library item -" << id << "- was found!";
            #endif
        }
    } else {
        #ifdef K_DEBUG
               tError() << "TupLibraryWidget::callExternalEditor() - Error: Current library item is invalid!";
        #endif
    }
}

void TupLibraryWidget::executeSoftware(TupNewItemDialog::ThirdParty software, QString &path)
{
    if (path.length() > 0 && QFile::exists(path)) {
        QString program = "";
        switch(software) {
               case TupNewItemDialog::Inkscape:
                    program = "/usr/bin/inkscape";
               break;
               case TupNewItemDialog::Gimp:
                    program = "/usr/bin/gimp";
               break;
               case TupNewItemDialog::Krita:
                    program = "/usr/bin/krita";
               break;
               case TupNewItemDialog::MyPaint:
                    program = "/usr/bin/mypaint";
               break;
        }

        QStringList arguments;
        arguments << path;

        QProcess *editor = new QProcess(this);
        editor->start(program, arguments);

        // SQA: Check the path list and if it doesn't exist yet, then add it to 
        k->watcher->addPath(path);
    } else {
        #ifdef K_DEBUG
               tError() << "TupLibraryWidget::executeSoftware() - Fatal Error: Item path either doesn't exist or is empty";
        #endif
    }
}

void TupLibraryWidget::updateItemFromSaveAction()
{
    LibraryObjects collection = k->library->objects();
    QMapIterator<QString, TupLibraryObject *> i(collection);
    while (i.hasNext()) {
           i.next();
           TupLibraryObject *object = i.value();
           if (object) {
               updateItem(object->smallId(), object->extension().toLower(), object);
           } else {
               #ifdef K_DEBUG
                      tError() << "TupLibraryWidget::updateItemFromSaveAction() - Fatal Error: The library item modified was not found!";
               #endif
           }
    }

    TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->currentFrame.scene, k->currentFrame.layer, k->currentFrame.frame,
                                                                      TupProjectRequest::Select);
    emit requestTriggered(&request);
}

void TupLibraryWidget::updateItem(const QString &name, const QString &extension, TupLibraryObject *object)
{
    QString onEdition = name + "." + extension;
    QString onDisplay = k->currentItemDisplayed->text(1) + "." + k->currentItemDisplayed->text(2).toLower();

    TupLibraryObject::Type type = TupLibraryObject::Image;
    if (extension.compare("svg") == 0)
        type = TupLibraryObject::Svg;

    bool isOk = k->library->reloadObject(onEdition);
    if (isOk) 
        k->project->reloadLibraryItem(type, onEdition, object);

    if (onDisplay.compare(onEdition) == 0)
        previewItem(k->lastItemEdited);
}

bool TupLibraryWidget::itemNameEndsWithDigit(QString &name)
{
    QByteArray array = name.toLocal8Bit();

    QChar letter(array.at(array.size() - 1));
    if (letter.isDigit())
        return true;

    return false;
}

int TupLibraryWidget::getItemNameIndex(QString &name) const
{
    QByteArray array = name.toLocal8Bit();
    int index = 0;
    for (int i = array.size()-1; i >= 0; i--) {
         QChar letter(array.at(i));
         if (!letter.isDigit()) {
             index = i + 1;
             break;
         }
    }

    return index;
}

QString TupLibraryWidget::nameForClonedItem(QString &name, QString &extension, int index, QString &path) const
{
    QString symbolName = "";

    QString base = name.left(index); 
    int counter = name.right(index).toInt();

    while (true) {
           counter++;
           QString number = QString::number(counter);
           if (counter < 10)
               number = "0" + number; 
           symbolName = base + number + "." + extension.toLower();
           QString tester = path + symbolName;
           if (!QFile::exists(tester))
               break;
    }

    return symbolName;
}

QString TupLibraryWidget::nameForClonedItem(QString &smallId, QString &extension, QString &path) const
{
    QString symbolName = "";
    int index = 0;

    while(true) {
          QString number = QString::number(index);
          if (index < 10)
              number = "0" + number;

          QString base = smallId + number;
          symbolName = base + "." + extension.toLower();
          QString tester = path + symbolName;

          if (!QFile::exists(tester))
              break;

          index++;
    }

    return symbolName;
}

QString TupLibraryWidget::verifyNameAvailability(QString &name, QString &extension, bool isCloningAction) {

    int limit = 1;
    if (isCloningAction)
        limit = 0; 

    QList<QTreeWidgetItem *> list = k->libraryTree->findItems(name, Qt::MatchExactly, 1);
    if (list.size() > limit) {
        int total = 0;
        for (int i=0; i<list.size(); i++) {
             QTreeWidgetItem *node = list.at(i);
             if (node->text(2).compare(extension) == 0)
                 total++;
        }

        if (total > limit) {
            bool ok = false;
            if (itemNameEndsWithDigit(name)) {
                int index = getItemNameIndex(name);
                QString base = name.left(index);
                int counter = name.right(name.length() - index).toInt(&ok);
                if (ok) {
                    while (true) {
                           counter++;
                           QString number = QString::number(counter);
                           if (counter < 10)
                               number = "0" + number;
                           name = base + number;
                           QList<QTreeWidgetItem *> others = k->libraryTree->findItems(name, Qt::MatchExactly, 1);
                           if (others.size() == 0)
                               break;
                    }
                } else {
                    name = TAlgorithm::randomString(8);
                    #ifdef K_DEBUG
                           tWarning() << "TupLibraryWidget::verifyNameAvailability() - Warning: error while processing item name!";
                    #endif
                }
            } else {
                int index = name.lastIndexOf("-");
                if (index < 0) {
                    name += "-1";
                } else {
                    QString first = name.mid(0, index);
                    QString last = name.mid(index+1, name.length() - index);
                    int newIndex = last.toInt(&ok);
                    if (ok) {
                        newIndex++;
                        name = first + "-" + QString::number(newIndex);
                    } else {
                        name = TAlgorithm::randomString(8);
                        #ifdef K_DEBUG
                               tWarning() << "TupLibraryWidget::verifyNameAvailability() - Warning: error while processing item name!";
                        #endif
                    }
                }
            }
        }
    }

    return name;
}
