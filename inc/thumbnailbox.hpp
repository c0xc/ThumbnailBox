#ifndef THUMBNAILBOX_HPP
#define THUMBNAILBOX_HPP

#include <QDebug>
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

    QMap<int, QColor>
    _colors;

    QMap<QString, int>
    _file_colors;

    QMap<QString, QPixmap>
    _pixcache;

    QPixmap
    (*_pixsource)(const QString&);

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

private slots:

    void
    resizeEvent(QResizeEvent *event);

    void
    wheelEvent(QWheelEvent *event);

    void
    showMenu(int index, const QPoint &pos);

public:

    QPixmap
    getPixmap(const QString &file);

    void
    clearCache();

    int
    availableWidth() const;

    int
    availableHeight() const;

    int
    columnCount() const;

    int
    rowCount() const;

    int
    topRow() const;

    int
    bottomRow() const;

    QFileInfoList
    items() const;

    QStringList
    list() const;

    int
    count() const;

    double
    thumbSize() const;

    int
    thumbWidth() const;

    bool
    isValidIndex(int index) const;

    int
    index() const;

    bool
    isSelected() const;

    QFileInfo
    item(int index = -1) const;

    QString
    itemPath(int index = -1) const;

    bool
    isFirst() const;

    bool
    isLast() const;

    QString
    path() const;

    bool
    directoriesVisible() const;

    bool
    itemsClickable() const;

    void
    addMenuItem(QAction *action);

    void
    removeMenuItem(QAction *action = 0);

    bool
    isMenuEnabled() const;

    void
    undefineColors();

    void
    defineColor(const QColor &color, int number = 1);

    void
    clearColors(int number = 0);

    void
    setFileColor(const QString &file, int color = 1);

    void
    setFileColors(const QStringList &files, int color = 1);

    QColor
    fileColor(const QString &file) const;

public slots:

    void
    scrollToRow(int row);

    void
    updateThumbnails();

    void
    updateThumbnails(int unused);

    void
    scheduleUpdateThumbnails(int timeout = 100);

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
    ensureItemVisible(int index);

    void
    setNameFilter(const QStringList &filter);

    bool
    setList(const QStringList &paths, int selected = -1);

    bool
    setList(const QStringList &paths, const QString &selected);

    void
    clear();

    void
    refresh();

    bool
    navigateTo(const QString &path, const QString &selected = "");

    bool
    navigate2(const QString &path, const QString &selected = "");

    void
    reload();

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
    setPixmapSource(QPixmap(*pixsource)(const QString&));
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
