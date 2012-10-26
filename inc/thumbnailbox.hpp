#ifndef THUMBNAILBOX_HPP
#define THUMBNAILBOX_HPP

#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QLabel>
#include <QTimer>
#include <QFileInfo>
#include <QDir>
#include <QMap>
#include <QPixmap>
#include <QMouseEvent>
#include <QMenu>

class Thumb;

class ThumbnailBox : public QFrame
{
    Q_OBJECT

public:

    ThumbnailBox(QWidget *parent);

signals:

    void
    resized();

    void
    selectionChanged();

    void
    itemSelected(int index);

    void
    itemSelected(const QString &path);

    void
    updated();

    void
    clicked(int index);

    void
    clicked(int index, const QPoint &pos);

    void
    rightClicked(int index);

    void
    rightClicked(int index, const QPoint &pos);

    void
    contextMenuRequested(const QPoint &pos);

    void
    menuItemSelected(QAction *action, const QString &item);

    void
    middleClicked(int index);

    void
    middleClicked(int index, const QPoint &pos);

private:

    bool
    updating_thumbnails;

    int
    _index;

    QFileInfoList
    _list;

    double
    _size;

    QStringList
    _filter;

    bool
    _showdirs;

    bool
    _isclickable;

    QString
    _path;

    int
    _pixmaxwh;

    QMap<QString, QPixmap>
    _pixcache;

    QPixmap
    (*_pixsource)(QString);

    QList<QAction*>
    _actions;

    QWidget
    *thumbcontainer;

    QHBoxLayout
    *thumbcontainerlayout;

    QWidget
    *thumbarea;

    QScrollBar
    *scrollbar;

    QTimer
    *updatetimer;

private slots:

    void
    resizeEvent(QResizeEvent *event);

    void
    showMenu(int index, const QPoint &pos);

public:

    QPixmap
    getPixmap(QString file);

    void
    clearCache();

    int
    availableWidth();

    int
    availableHeight();

    QFileInfoList&
    items();

    QStringList
    list();

    int
    count();

    double
    thumbSize();

    int
    thumbWidth();

    bool
    isValidIndex(int index);

    int
    index();

    bool
    isSelected();

    QFileInfo
    item(int index = -1);

    QString
    itemPath(int index = -1);

    bool
    isFirst();

    bool
    isLast();

    QString
    path();

    bool
    directoriesVisible();

    bool
    itemsClickable();

    void
    addMenuItem(QAction *action);

    void
    removeMenuItem(QAction *action = 0);

    bool
    isMenuEnabled();

public slots:

    void
    updateThumbnails();

    void
    updateThumbnails(int unused);

    void
    scheduleUpdateThumbnails();

    void
    setThumbSize(double percent);

    void
    setThumbSize(int percent);

    void
    setThumbWidth(int width);

    void
    select(int index);

    void
    unselect();

    void
    selectPrevious();

    void
    selectNext();

    void
    setNameFilter(QStringList filter);

    void
    reload();

    bool
    setList(const QStringList &paths, int selected = -1);

    bool
    setList(const QStringList &paths, const QString &selected);

    bool
    navigateTo(const QString &path, const QString &selected = "");

    bool
    navigate2(QString path, QString selected = ""); //for cool people only

    void
    setDirectoriesVisible(bool enable);

    void
    setItemsClickable(bool enable);

    //If thumbnails are larger than 200px (with or height),
    //this setting should be set to a higher value.
    //Has no effect if an external pixmap source is set.
    void
    setPixMaxWH(int wh);

    //MOC says:
    //error: no matching function for call to
    //‘ThumbnailBox::setPixmapSource(QPixmap&)’
    //note: no known conversion for argument 1 from ‘QPixmap’ to
    //‘QPixmap (*)(QString)’
    #if !defined(Q_MOC_RUN)
    void
    setPixmapSource(QPixmap(*pixsource)(QString));
    #endif

    void
    setPixmapSource();

};

class Thumb : public QFrame
{
    Q_OBJECT

public:
    
    Thumb(QWidget *parent = 0);

    void
    setIndex(int index);

protected:

    void
    mousePressEvent(QMouseEvent *event);

private:

    int
    index;

signals: //QPoint values are global

    void
    clicked(int index);

    void
    clicked(int index, const QPoint &pos);

    void
    rightClicked(int index);

    void
    rightClicked(int index, const QPoint &pos);

    void
    contextMenuRequested(const QPoint &pos);

    void
    middleClicked(int index);

    void
    middleClicked(int index, const QPoint &pos);

};

#endif
