ThumbnailBox
============

This is a simple Qt widget that shows thumbnails.



Example
-------

    QStringList image_files = ...;
    ThumbnailBox thumbnailbox = new ThumbnailBox(this);
    thumbnailbox->setNameFilter(QStringList() << "*.jpg" << "*.bmp");
    thumbnailbox->setList(image_files);
    connect(thumbnailbox,
            SIGNAL(itemSelected(const QString&)),
            SLOT(doSomethingUseful(const QString&)));
    //...



Notes
-----

This module was written for Qt 4.8.

This thing really doesn't do much. It's just a box with thumbnails.



License
-------

Please see the file called LICENSE.




