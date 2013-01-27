#include "thumbnailbox.hpp"

ThumbnailBox::ThumbnailBox(QWidget *parent)
            : QFrame(parent),
              updating_thumbnails(false),
              _index(-1),
              _size(.3),
              _showdirs(false),
              _isclickable(true),
              _pixmaxwh(200),
              _pixsource(0),
              thumbarea(0)
{
    QHBoxLayout *hbox;

    hbox = new QHBoxLayout;

    thumbcontainer = new QWidget;
    thumbcontainerlayout = new QHBoxLayout;
    thumbcontainerlayout->setContentsMargins(QMargins());
    thumbcontainerlayout->setSpacing(2);
    //This is where the thumbarea will be...
    thumbcontainer->setLayout(thumbcontainerlayout);
    hbox->addWidget(thumbcontainer);

    scrollbar = new QScrollBar(Qt::Vertical);
    scrollbar->setTracking(true);
    scrollbar->setMinimum(0);
    scrollbar->setMaximum(0);
    connect(scrollbar,
            SIGNAL(valueChanged(int)),
            SLOT(updateThumbnails(int)));

    hbox->addWidget(scrollbar);

    setLayout(hbox);

    connect(this, SIGNAL(resized()), SLOT(updateThumbnails()));

    connect(this, SIGNAL(itemSelected(int)), SLOT(ensureItemVisible(int)));

    connect(this,
            SIGNAL(rightClicked(int, const QPoint&)),
            SLOT(showMenu(int, const QPoint&)));

    updatetimer = new QTimer(this);
    updatetimer->setInterval(100);
    updatetimer->stop();
    connect(updatetimer, SIGNAL(timeout()), SLOT(updateThumbnails()));
}

void
ThumbnailBox::resizeEvent(QResizeEvent *event)
{
    if (event->oldSize().isValid())
    emit resized();
}

void
ThumbnailBox::wheelEvent(QWheelEvent *event)
{
    int degrees = event->delta() / 8;
    int steps = degrees / 15;

    if (event->orientation() != Qt::Vertical) return;
    if (!scrollbar->isEnabled()) return;

    scrollbar->setValue(scrollbar->value() - steps);

    event->accept();
}

void
ThumbnailBox::showMenu(int index, const QPoint &pos)
{
    if (!isMenuEnabled()) return;

    QString item = itemPath(index);
    if (item.isEmpty()) return;

    QString title = QFileInfo(item).fileName();

    QMenu menu;
    QAction *action = new QAction(title, &menu);
    QFont font = action->font();
    font.setBold(true);
    action->setFont(font);
    action->setEnabled(false);
    menu.addAction(action);
    foreach (QAction *action, _actions)
    {
        menu.addAction(action);
    }

    emit menuItemSelected(menu.exec(pos), item);
}

QPixmap
ThumbnailBox::getPixmap(QString file)
{
    int maxwh = _pixmaxwh; //Thumbs shouldn't be larger than this

    //Search cache

    if (_pixsource) return _pixsource(file);

    if (_pixcache.contains(file)) return _pixcache[file];

    //Load file

    if (!QFile::exists(file)) return QPixmap();

    QImage img(QImage(file).scaled(maxwh, maxwh, Qt::KeepAspectRatio));
    QPixmap pix(QPixmap::fromImage(img));

    if (!pix.isNull()) _pixcache[file] = pix;

    return pix;
}

void
ThumbnailBox::clearCache()
{
    _pixcache.clear();
}

int
ThumbnailBox::availableWidth()
{
    return thumbcontainer->width();
}

int
ThumbnailBox::availableHeight()
{
    return thumbcontainer->height();
}

int
ThumbnailBox::columnCount()
{
    int cols;
    int padding = 5;
    int availablewidth = availableWidth();
    int thumbsize = thumbWidth();
    int thumbwidth = thumbsize;
    cols = availablewidth / (thumbwidth + padding); //2.9 -> 2
    return cols;
}

int
ThumbnailBox::rowCount()
{
    int rows;
    int padding = 5;
    int availableheight = availableHeight();
    int thumbsize = thumbWidth();
    int thumbheight = thumbsize;
    rows = availableheight / (thumbheight + padding); //2.9 -> 2
    return rows;
}

int
ThumbnailBox::topRow()
{
    int scrollpos = scrollbar->value();
    return scrollpos;
}

int
ThumbnailBox::bottomRow()
{
    return topRow() + (rowCount() - 1);
}

QFileInfoList&
ThumbnailBox::items()
{
    QFileInfoList &list(_list);
    for (int i = list.size(); --i >= 0 && !directoriesVisible();)
    {
        if (list[i].isDir()) list.removeAt(i);
    }
    return list;
}

QStringList
ThumbnailBox::list()
{
    QStringList paths;
    foreach (QFileInfo inf, items()) paths << inf.absoluteFilePath();
    return paths;
}

int
ThumbnailBox::count()
{
    return items().size();
}

double
ThumbnailBox::thumbSize()
{
    return _size;
}

int
ThumbnailBox::thumbWidth()
{
    int availablewidth = availableWidth();
    int width = availablewidth;
    width *= thumbSize();
    if (width < 30) width = 30; //Should be a const

    return width;
}

bool
ThumbnailBox::isValidIndex(int index)
{
    return (index >= 0 && index < count());
}

int
ThumbnailBox::index()
{
    if (_index < 0) _index = -1;
    if (_index >= list().size()) _index = -1;
    return _index;
}

bool
ThumbnailBox::isSelected()
{
    return (index() != -1);
}

QFileInfo
ThumbnailBox::item(int index)
{
    QFileInfo inf;
    if (index == -1) index = this->index();
    if (isValidIndex(index)) inf = items().at(index);
    return inf;
}

QString
ThumbnailBox::itemPath(int index)
{
    return item(index).absoluteFilePath();
}

bool
ThumbnailBox::isFirst()
{
    return (index() == 0);
}

bool
ThumbnailBox::isLast()
{
    return (index() == count() - 1);
}

QString
ThumbnailBox::path()
{
    return _path;
}

bool
ThumbnailBox::directoriesVisible()
{
    return _showdirs;
}

bool
ThumbnailBox::itemsClickable()
{
    return _isclickable;
}

void
ThumbnailBox::addMenuItem(QAction *action)
{
    if (!action) return;
    if (!_actions.contains(action))
    {
        _actions << action;
    }
}

void
ThumbnailBox::removeMenuItem(QAction *action)
{
    if (action) _actions.removeAll(action);
    else _actions.clear();
}

bool
ThumbnailBox::isMenuEnabled()
{
    return _actions.size();
}

void
ThumbnailBox::scrollToRow(int row)
{
    scrollbar->setValue(row);
}

void
ThumbnailBox::updateThumbnails()
{
    int count = this->count(); //Total amount of files
    //int availablewidth = availableWidth();
    //int availableheight = availableHeight();
    int thumbwidth;
    int thumbheight;
    int padding = 5;
    int cols;
    int rows;
    int totalrows; //Row count (incl. hidden)
    int totalhiddenrows;
    int maxthumbs; //Maximum visible thumbs
    int visiblethumbs; //Visible thumbs needed
    int scrollpos;
    int hiddenrows; //Rows hidden ABOVE viewport
    int hiddenthumbs; //Thumbs hidden ABOVE viewport

    if (updating_thumbnails) return;
    updating_thumbnails = true;

    int thumbsize = thumbWidth();
    thumbwidth = thumbsize;
    if (thumbwidth < 30) thumbwidth = 30;
    thumbheight = thumbwidth;

    cols = columnCount(); //2.9 -> 2
    rows = rowCount(); //2.9 -> 2
    if (!cols) cols = 1;
    if (!rows) rows = 1;
    maxthumbs = cols * rows;
    visiblethumbs = count;
    if (visiblethumbs > maxthumbs) visiblethumbs = maxthumbs;
    totalrows = count / cols; if (count % cols) totalrows++; //2.1 -> 3
    totalhiddenrows = totalrows - rows;
    if (totalhiddenrows < 0) totalhiddenrows = 0;

    //Important:
    //Everytime an update is triggered,
    //the thumbnail area widget is recreated
    //and filled with the amount of visible thumbs.
    //This is important for large directories (> 1000 items),
    //because creating 1000 thumbs is inefficient/stupid/sigsegv.

    if (thumbarea) delete thumbarea;
    thumbarea = new QWidget();
    thumbcontainerlayout->insertWidget(0, thumbarea);
    thumbarea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    if (totalhiddenrows >= 0) scrollbar->setMaximum(totalhiddenrows);
    scrollbar->setPageStep(rows);

    scrollpos = scrollbar->value();
    hiddenrows = scrollpos;
    hiddenthumbs = hiddenrows * cols;

    QVBoxLayout *vbox_rows = new QVBoxLayout;
    thumbarea->setLayout(vbox_rows);

    for (int i = 0; i < rows; i++)
    {
        QHBoxLayout *hbox_row = new QHBoxLayout;
        hbox_row->setSpacing(padding);
        for (int j = 0; j < cols; j++)
        {
            int relindex = i * cols + j;
            int absindex = relindex + hiddenthumbs;
            if (absindex >= count) break;
            int index = this->index();

            QString title = item(absindex).fileName();
            Thumb *thumb = new Thumb;
            thumb->setEnabled(itemsClickable());
            connect(thumb,
                    SIGNAL(clicked(int)),
                    SLOT(select(int)));
            thumb->setIndex(absindex);
            thumb->setFixedSize(QSize(thumbwidth, thumbheight));
            thumb->setFrameStyle(QFrame::Panel | QFrame::Raised);
            if (absindex == index) thumb->setFrameShadow(QFrame::Sunken);
            thumb->setLineWidth(2);
            thumb->setToolTip(title);
            hbox_row->addWidget(thumb);
            QVBoxLayout *vbox = new QVBoxLayout;
            QLabel *lbl = new QLabel();
            lbl->setPixmap(getPixmap(itemPath(absindex)));
            lbl->setScaledContents(true);
            vbox->addWidget(lbl);
            lbl = new QLabel(title);
            lbl->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
            vbox->addWidget(lbl);
            thumb->setLayout(vbox);
            connect(thumb,
                    SIGNAL(clicked(int)),
                    SIGNAL(clicked(int)));
            connect(thumb,
                    SIGNAL(clicked(int, const QPoint&)),
                    SIGNAL(clicked(int, const QPoint&)));
            connect(thumb,
                    SIGNAL(rightClicked(int)),
                    SIGNAL(rightClicked(int)));
            connect(thumb,
                    SIGNAL(rightClicked(int, const QPoint&)),
                    SIGNAL(rightClicked(int, const QPoint&)));
            connect(thumb,
                    SIGNAL(contextMenuRequested(const QPoint&)),
                    SIGNAL(contextMenuRequested(const QPoint&)));
            connect(thumb,
                    SIGNAL(middleClicked(int)),
                    SIGNAL(middleClicked(int)));
            connect(thumb,
                    SIGNAL(middleClicked(int, const QPoint&)),
                    SIGNAL(middleClicked(int, const QPoint&)));
        }
        hbox_row->addStretch(1);
        vbox_rows->addLayout(hbox_row);
    }
    vbox_rows->addStretch(1);

    emit updated();

    updating_thumbnails = false;
}

void
ThumbnailBox::updateThumbnails(int unused)
{
    updateThumbnails();
}

void
ThumbnailBox::scheduleUpdateThumbnails()
{
    updatetimer->setSingleShot(true);
    updatetimer->start();
}

void
ThumbnailBox::setThumbSize(double percent)
{
    if (percent < 0) percent = 0;
    if (percent > 1) percent = 1;
    _size = percent;

    updateThumbnails();

    setMinimumHeight(thumbWidth() * 1.5);
}

void
ThumbnailBox::setThumbSize(int percent)
{
    setThumbSize((double)percent / 100);
}

void
ThumbnailBox::setThumbWidth(int width)
{
    double percent = 0;
    int availwidth = availableWidth();
    if (availwidth) percent = width / availwidth;
    setThumbSize(percent);
}

void
ThumbnailBox::select(int index)
{
    if (!isEnabled()) return;

    if (!isValidIndex(index)) index = -1;
    if (index == this->index()) return;
    _index = index;
    //updateThumbnails(); //Segfault for no apparent reason
    scheduleUpdateThumbnails();

    emit selectionChanged();
    if (index != -1)
    {
        emit itemSelected(index);
        emit itemSelected(itemPath(index));
    }
}

void
ThumbnailBox::unselect()
{
    select(-1);
}

void
ThumbnailBox::selectPrevious()
{
    if (!isFirst()) select(index() - 1);
}

void
ThumbnailBox::selectNext()
{
    if (!isLast()) select(index() + 1);
}

void
ThumbnailBox::ensureItemVisible(int index)
{
    //Scroll to item, if out of viewport

    if (index < 0 || index >= count()) return;
    int col_count = columnCount();
    int row_item = (double)index / (double)col_count;
    int row_top = topRow();
    int row_bottom = bottomRow();

    if (row_item < row_top) //Item above viewport
    {
        //Make item row 2nd from top?
        scrollToRow(row_item);
    }
    else if (row_item > row_bottom) //Item below viewport
    {
        //Make item row 2nd from bottom?
        scrollToRow(row_item);
    }
}

void
ThumbnailBox::setNameFilter(QStringList filter)
{
    _filter = filter;

    reload();
}

void
ThumbnailBox::reload()
{
    QString path = this->path();
    _path.clear();
    navigateTo(path);
}

bool
ThumbnailBox::setList(const QStringList &paths, int selected)
{
    clearCache();

    _path.clear();
    _index = -1;
    _index = selected;

    QFileInfoList &list = _list;
    list.clear();

    foreach (QString path, paths)
    {
        QFileInfo inf(path);
        if (!inf.isFile() &&
            !inf.isDir()) continue; //Invalid entry
        list << inf;
    }

    updateThumbnails();

    emit selectionChanged();

    return true;
}

bool
ThumbnailBox::setList(const QStringList &paths, const QString &selected)
{
    return setList(paths, paths.indexOf(selected));
}

bool
ThumbnailBox::navigateTo(const QString &path, const QString &selected)
{
    QDir dir(path);
    if (!dir.exists()) return false;
    if (path == this->path()) return true;
    _path = path;

    clearCache();

    _index = -1;

    QFileInfoList &list = _list;
    QDir::Filters filter = QDir::AllEntries | QDir::Hidden | QDir::System;
    QDir::SortFlags flags = QDir::Name | QDir::DirsFirst;

    if (_filter.size()) list = dir.entryInfoList(_filter, filter, flags);
    else list = dir.entryInfoList(filter, flags);

    if (!selected.isEmpty()) _index = this->list().indexOf(selected);

    updateThumbnails();

    emit selectionChanged();

    return true;
}

bool
ThumbnailBox::navigate2(QString path, QString selected)
{
    return navigateTo(path);
}

void
ThumbnailBox::setDirectoriesVisible(bool enable)
{
    _showdirs = enable;

    updateThumbnails();
}

void
ThumbnailBox::setItemsClickable(bool enable)
{
    _isclickable = enable;

    updateThumbnails();
}

void
ThumbnailBox::setPixMaxWH(int wh)
{
    if (wh < 0) wh = 0; //Even 0 is nonsense
    _pixmaxwh = wh;
}

void
ThumbnailBox::setPixmapSource(QPixmap(*pixsource)(QString))
{
    _pixsource = pixsource;
}

void
ThumbnailBox::setPixmapSource()
{
    _pixsource = 0;
}



//---



Thumb::Thumb(QWidget *parent)
     : QFrame(parent),
       index(0)
{
}

void
Thumb::setIndex(int index)
{
    this->index = index;
}

void
Thumb::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked(index);
        emit clicked(index, event->globalPos());
    }
    else if (event->button() == Qt::RightButton)
    {
        emit rightClicked(index);
        emit rightClicked(index, event->globalPos());
        emit contextMenuRequested(event->globalPos());
    }
    else if (event->button() == Qt::MidButton)
    {
        emit middleClicked(index);
        emit middleClicked(index, event->globalPos());
    }
}

