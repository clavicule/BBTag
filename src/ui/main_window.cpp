#include <ui/main_window.h>
#include <ui/tag_viewer.h>
#include <ui/tag_scroll_view.h>
#include <core/tag_model.h>

#include <QLayout>
#include <QWidget>
#include <QFileSystemModel>
#include <QTreeView>
#include <QSplitter>
#include <QPushButton>
#include <QButtonGroup>
#include <QScrollArea>
#include <QImageReader>
#include <QInputDialog>
#include <QComboBox>
#include <QGuiApplication>
#include <QScreen>


MainWindow::MainWindow(
        QWidget *parent
    ) : QMainWindow( parent )
{
    // splitter is the central widget of the main window
    // it is made of 3 widgets:
    // - directory parser
    // - image tag list
    // - image viewer
    QSplitter* splitter = new QSplitter( this );
    splitter->setOrientation( Qt::Horizontal );


    // --------
    // widget #1
    // get the path to the working directory
    QString root_path = QDir::rootPath();

    // set the tree view to follow a directory parser model
    dir_model_ = new QFileSystemModel( this );
    dir_model_->setRootPath( root_path );
    dir_model_->setNameFilters( valid_image_format() );

    // left-most widget in splitter is a directory parser
    dir_view_ = new QTreeView( splitter );
    dir_view_->setModel( dir_model_ );
    dir_view_->setSelectionBehavior( QAbstractItemView::SelectRows );
    dir_view_->setSelectionMode( QAbstractItemView::ExtendedSelection );

    // initialize directory tree view
    const QModelIndex root_index = dir_model_->index( root_path );
    if( root_index.isValid() ) {
        dir_view_->setRootIndex( root_index );
        dir_view_->setColumnWidth( 0, 200 );
    }


    // --------
    // widget #2
    // middle widget in splitter is image tag tree
    QWidget* image_tag_widget = new QWidget( splitter );

    // setup add/remove buttons to add/remove image files from the list of images to tag
    // Icons unmodified under CreativeCommon Attribution-NonCommercial 3.0 Unported license
    // http://creativecommons.org/licenses/by-nc/3.0/
    // From Snip Master --> http://www.snipicons.com/
    QPushButton* import_button = new QPushButton( QIcon( ":/pixmaps/import.png" ), "", image_tag_widget );
    import_button->setFixedSize( 32, 32 );
    import_button->setIconSize( QSize( 32, 32 ) );
    import_button->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_Right ) );
    import_button->setToolTip( "Use Ctrl+right key as shortcut" );

    QPushButton* add_button = new QPushButton( QIcon( ":/pixmaps/add.png" ), "", image_tag_widget );
    add_button->setFixedSize( 32, 32 );
    add_button->setIconSize( QSize( 32, 32 ) );
    add_button->setShortcut( QKeySequence( Qt::Key_Plus ) );
    add_button->setToolTip( "Use Plus key as shortcut" );

    QPushButton* remove_button = new QPushButton( QIcon( ":/pixmaps/remove.png" ), "", image_tag_widget );
    remove_button->setFixedSize( 32, 32 );
    remove_button->setIconSize( QSize( 32, 32 ) );
    remove_button->setShortcut( QKeySequence( QKeySequence::Delete ) );
    remove_button->setToolTip( "Use Del key as shortcut");

    QHBoxLayout* tag_tree_button_layout = new QHBoxLayout();
    tag_tree_button_layout->addWidget( add_button );
    tag_tree_button_layout->addWidget( remove_button );
    tag_tree_button_layout->addStretch();

    tag_model_ = new TagModel( this );
    tag_view_ = new QTreeView( image_tag_widget );
    tag_view_->setEditTriggers( QAbstractItemView::NoEditTriggers );
    tag_view_->setSelectionMode( QAbstractItemView::ExtendedSelection );
    tag_model_->attach( tag_view_ );

    QVBoxLayout* tag_tree_layout = new QVBoxLayout();
    tag_tree_layout->addLayout( tag_tree_button_layout );
    tag_tree_layout->addWidget( tag_view_ );

    QHBoxLayout* image_tag_layout = new QHBoxLayout();
    image_tag_layout->addWidget( import_button );
    image_tag_layout->addLayout( tag_tree_layout );
    image_tag_widget->setLayout( image_tag_layout );


    // --------
    // widget #3
    // right-most widget in splitter is image viewer
    QWidget* tag_viewer_widget = new QWidget( splitter );
    label_selector_ = new QComboBox( tag_viewer_widget );

    QButtonGroup* tag_tools = new QButtonGroup( this );
    tag_tools->setExclusive( true );

    tag_button_ = new QPushButton( QIcon( ":/pixmaps/tag.png" ), "", tag_viewer_widget );
    tag_button_->setCheckable( true );
    tag_button_->setFixedSize( 32, 32 );
    tag_button_->setIconSize( QSize( 32, 32 ) );
    tag_button_->setShortcut( QKeySequence( Qt::Key_T ) );
    tag_button_->setToolTip( "Use t-key as shortcut" );
    tag_tools->addButton( tag_button_ );

    QPushButton* untag_button = new QPushButton( QIcon( ":/pixmaps/untag.png" ), "", tag_viewer_widget );
    untag_button->setCheckable( true );
    untag_button->setFixedSize( 32, 32 );
    untag_button->setIconSize( QSize( 32, 32 ) );
    untag_button->setShortcut( QKeySequence( Qt::Key_D ) );
    untag_button->setToolTip( "Use d-key as shortcut" );
    tag_tools->addButton( untag_button );

    QHBoxLayout* tag_buttons_layout = new QHBoxLayout();
    tag_buttons_layout->addWidget( label_selector_ );
    tag_buttons_layout->addWidget( tag_button_ );
    tag_buttons_layout->addWidget( untag_button );
    tag_buttons_layout->setStretchFactor( label_selector_, 2 );

    tag_scroll_view_ = new TagScrollView( tag_viewer_widget );
    tag_viewer_ = new TagViewer( tag_scroll_view_ );
    tag_scroll_view_->setWidget( tag_viewer_ );

    QPushButton* zoom_in_button = new QPushButton( QIcon( ":/pixmaps/zoom_in.png" ), "", tag_viewer_widget );
    zoom_in_button->setFixedSize( 32, 32 );
    zoom_in_button->setIconSize( QSize( 32, 32 ) );
    zoom_in_button->setToolTip( "Use ctrl+mouse wheel up as shortcut" );

    QPushButton* zoom_out_button = new QPushButton( QIcon( ":/pixmaps/zoom_out.png" ), "", tag_viewer_widget );
    zoom_out_button->setFixedSize( 32, 32 );
    zoom_out_button->setIconSize( QSize( 32, 32 ) );
    zoom_out_button->setToolTip( "Use ctrl+mouse wheel down as shortcut" );

    QPushButton* fit_to_view_button = new QPushButton( QIcon( ":/pixmaps/fit_to_view.png" ), "", tag_viewer_widget );
    fit_to_view_button->setFixedSize( 32, 32 );
    fit_to_view_button->setIconSize( QSize( 32, 32 ) );
    fit_to_view_button->setShortcut( QKeySequence( Qt::Key_F ) );
    fit_to_view_button->setToolTip( "Use f-key as shortcut" );

    QVBoxLayout* viewer_buttons_layout = new QVBoxLayout();
    viewer_buttons_layout->addWidget( zoom_in_button );
    viewer_buttons_layout->addWidget( zoom_out_button );
    viewer_buttons_layout->addWidget( fit_to_view_button );
    viewer_buttons_layout->addStretch();

    QHBoxLayout* viewer_layout = new QHBoxLayout();
    viewer_layout->addWidget( tag_scroll_view_ );
    viewer_layout->addLayout( viewer_buttons_layout );

    QVBoxLayout* tag_viewer_layout = new QVBoxLayout();
    tag_viewer_layout->addLayout( tag_buttons_layout );
    tag_viewer_layout->addLayout( viewer_layout );
    tag_viewer_widget->setLayout( tag_viewer_layout );

    // --------
    // Main Widget
    // arrange widgets inside the splitter widget
    splitter->addWidget( dir_view_ );
    splitter->addWidget( image_tag_widget );
    splitter->addWidget( tag_viewer_widget );
    splitter->setStretchFactor( 0, 1 );
    splitter->setStretchFactor( 1, 1 );
    splitter->setStretchFactor( 2, 3 );
    splitter->setCollapsible( 1, false );

    QList<int> sizes;
    sizes << 300 << 300 << 400;
    splitter->setSizes( sizes );

    setCentralWidget( splitter );
    resize( QGuiApplication::primaryScreen()->availableSize() * 0.8 );

    // setup connections
    connect( import_button, SIGNAL( clicked() ), this, SLOT( import_images() ) );
    connect( add_button, SIGNAL( clicked() ), this, SLOT( add_label() ) );
    connect( remove_button, SIGNAL( clicked() ), this, SLOT( remove_images() ) );

    connect( tag_view_->selectionModel(), SIGNAL( selectionChanged(QItemSelection,QItemSelection) ), this, SLOT( update_viewer() ) );
    connect( label_selector_, SIGNAL( currentIndexChanged(int) ), this, SLOT( set_viewer_tag_options() ) );
    connect( tag_button_, SIGNAL( toggled(bool) ), tag_viewer_, SLOT( set_tagging_status(bool) ) );
    connect( untag_button, SIGNAL( toggled(bool) ), tag_viewer_, SLOT( set_untagging_status(bool) ) );
    connect( tag_viewer_, SIGNAL( tagged(QRect) ), this, SLOT( tag_image(QRect) ) );
    connect( tag_viewer_, SIGNAL( untagged(QString,QRect) ), this, SLOT( untag_image(QString, QRect) ) );

    connect( zoom_in_button, SIGNAL( clicked() ), tag_scroll_view_, SLOT( zoom_in() ) );
    connect( zoom_out_button, SIGNAL( clicked() ), tag_scroll_view_, SLOT( zoom_out() ) );
    connect( fit_to_view_button, SIGNAL( clicked() ), tag_scroll_view_, SLOT( fit_to_view() ) );

    update_tag_selector();
}

MainWindow::~MainWindow()
{
}

QStringList MainWindow::valid_image_format()
{
    QStringList image_filters;
    QList<QByteArray> supported_img_format = QImageReader::supportedImageFormats();
    for( QList<QByteArray>::iterator fmt_itr = supported_img_format.begin(); fmt_itr != supported_img_format.end(); ++fmt_itr ) {
        image_filters.append( "*." + QString::fromUtf8( *fmt_itr ) );
    }

    return image_filters;
}

void MainWindow::import_images()
{
    // get the current selected rows for column 0 (directories and files)
    QItemSelectionModel* selection_model = dir_view_->selectionModel();
    if( !selection_model ) {
        return;
    }

    QFileInfoList img_to_import;

    // list all the valid image file in the selection
    QModelIndexList selection = selection_model->selectedRows();
    for( QModelIndexList::iterator s_itr = selection.begin(); s_itr != selection.end(); ++s_itr ) {

        QFileInfo img_info = dir_model_->fileInfo( *s_itr );

        // check if selection is a directory or a file
        if( img_info.isDir() ) {
            // if a directory, add all the image files from it
            img_to_import.append( QDir( img_info.absoluteFilePath() ).entryInfoList( valid_image_format(), QDir::Files ) );
        } else {
            img_to_import.append( img_info );
        }
    }

    tag_model_->import_images( img_to_import );
}

void MainWindow::add_label()
{
    QString new_label_name = QInputDialog::getText( this, "New label", "Enter new tag label", QLineEdit::Normal, "my_label" );
    if( new_label_name.isEmpty() ) {
        return;
    }

    // generate random color
    // and avoid colors to be too close to each other
    int r = qrand() % 255;
    int g = qrand() % 255;
    int b = qrand() % 255;

    tag_model_->add_new_label( QColor::fromRgb( r, g, b ), new_label_name );
    update_tag_selector();
}

void MainWindow::remove_images()
{
    // get the current selected rows for column 0 (directories and files)
    QItemSelectionModel* selection_model = tag_view_->selectionModel();
    if( !selection_model ) {
        return;
    }

    tag_model_->remove_items( selection_model->selectedRows() );
    update_tag_selector();
    update_viewer();
}

QString MainWindow::get_image_from_index_list(
        const QModelIndexList& index_list
    ) const
{
    QString reference_fullpath;
    for( QModelIndexList::const_iterator idx_itr = index_list.begin(); idx_itr != index_list.end(); ++idx_itr ) {
        QString current_fullpath = tag_model_->get_fullpath( *idx_itr );

        if( current_fullpath.isEmpty() ) {
            // not an image --> keeo going
            continue;

        } else if( reference_fullpath.isEmpty() ) {
            // first time we encounter a valid image
            reference_fullpath = current_fullpath;

        } else if( reference_fullpath != current_fullpath ) {
            // 2 different images
            return QString::null;
        }

    }

    return reference_fullpath;
}

void MainWindow::update_viewer()
{
    QPixmap pix;
    QList<TagViewer::TagDisplayElement> display_elements;

    QItemSelectionModel* selection_model = tag_view_->selectionModel();
    if( !selection_model ) {
        return;
    }

    QModelIndexList selection = selection_model? selection_model->selectedRows() : QModelIndexList();

    // iterate through the selection:
    // if multiple selection, check that all selected item reference to the same image (fullpath)
    // otherwise null data is sent to the viewer
    // in case of a label name being selected, consider the same image belonging to that label is selected

    // ensure and get the single image referenced in the selection
    QString fullpath_ref = get_image_from_index_list( selection );
    if( !fullpath_ref.isEmpty() ) {

        for( QModelIndexList::iterator idx_itr = selection.begin(); idx_itr != selection.end(); ++idx_itr ) {
            QList<TagItem::Elements> item_elts = tag_model_->get_elements( *idx_itr );

            for( QList<TagItem::Elements>::iterator elt_itr = item_elts.begin(); elt_itr != item_elts.end(); ++elt_itr ) {
                const TagItem::Elements& tag_elt = *elt_itr;

                QString cur_fullpath = tag_elt._fullpath;
                TagViewer::TagDisplayElement tag;

                // if fullpath is empty, we are dealing with a label name
                // therefore we need to find the item within that label
                if( cur_fullpath.isEmpty() ) {
                    TagItem::Elements elt_from_label = tag_model_->get_element( fullpath_ref, tag_elt._label );
                    tag._color = elt_from_label._color;
                    tag._label = elt_from_label._label;
                    tag._bbox = elt_from_label._bbox;

                } else {
                    tag._color = tag_elt._color;
                    tag._label = tag_elt._label;
                    tag._bbox = tag_elt._bbox;
                }

                display_elements.append( tag );
            }
        }
    }

    // ok to send a null pixmap
    // the viewer will recognize that
    // and display a message instead
    pix.load( fullpath_ref );
    tag_viewer_->set_image( pix );
    tag_viewer_->set_overlay_elements( display_elements );
    tag_viewer_->update();
}

void MainWindow::update_tag_selector()
{
    label_selector_->clear();
    QList<TagItem::Elements> tags = tag_model_->get_all_tags();

    for( QList<TagItem::Elements>::iterator tag_itr = tags.begin(); tag_itr != tags.end(); ++tag_itr ) {
        int idx = label_selector_->count();
        const TagItem::Elements& tag = *tag_itr;
        label_selector_->insertItem( idx, tag._label );
        label_selector_->setItemData( idx, QVariant( tag._color ), Qt::DecorationRole );
    }

    set_viewer_tag_options();

    bool tagging_status_enabled = !label_selector_->currentText().isEmpty();
    tag_button_->setEnabled( tagging_status_enabled );

    if( !tagging_status_enabled ) {
        tag_button_->setChecked( false );
        tag_viewer_->set_tagging_status( false );
    }
}

void MainWindow::set_viewer_tag_options()
{
    QVariant label = label_selector_->currentData( Qt::DisplayRole );
    QVariant color = label_selector_->currentData( Qt::DecorationRole );

    if( !label.isValid() || !color.isValid() ) {
        return;
    }

    tag_viewer_->set_tag_options( label.toString(), color.value<QColor>() );
}

void MainWindow::tag_image(
        const QRect& bbox
    )
{
    // ensure and get the single image referenced in the selection
    QItemSelectionModel* selection_model = tag_view_->selectionModel();
    if( !selection_model ) {
        return;
    }

    QModelIndexList selection = selection_model? selection_model->selectedRows() : QModelIndexList();

    QString fullpath_ref = get_image_from_index_list( selection );
    if( fullpath_ref.isEmpty() ) {
        return;
    }

    // ensure a label is selected
    QString label = label_selector_->currentText();
    if( label.isEmpty() ) {
        return;
    }

    // block viewer update as selection
    // will change --> allows viewer
    // to keep the same pixmap and scale factor
    selection_model->blockSignals( true );
    QModelIndex index = tag_model_->add_tag_to_label( fullpath_ref, label, bbox );
    selection_model->blockSignals( false );

    // add to current selection
    // useful for instance when tagging from untagged or from another label
    // to avoid deselection of current image
    if( index.isValid() ) {
        QItemSelection current_selection = selection_model->selection();
        current_selection.merge( QItemSelection( index, index ), QItemSelectionModel::Select );
        selection_model->select( current_selection, QItemSelectionModel::Select );
        tag_view_->setExpanded( index.parent(), true );
    }

    update_viewer();
}

void MainWindow::untag_image(
        const QString& label,
        const QRect& bbox
    )
{
    if( label.isEmpty() ) {
        return;
    }

    // ensure and get the single image referenced in the selection
    QItemSelectionModel* selection_model = tag_view_->selectionModel();
    if( !selection_model ) {
        return;
    }

    QModelIndexList selection = selection_model? selection_model->selectedRows() : QModelIndexList();

    QString fullpath_ref = get_image_from_index_list( selection );
    if( fullpath_ref.isEmpty() ) {
        return;
    }

    // block viewer update as selection
    // will change --> allows viewer
    // to keep the same pixmap and scale factor
    selection_model->blockSignals( true );
    QModelIndex index = tag_model_->remove_tag_from_label( fullpath_ref, label, bbox );
    selection_model->blockSignals( false );

    // add to current selection
    // useful for instance when tagging from untagged or from another label
    // to avoid deselection of current image
    if( index.isValid() ) {
        QItemSelection current_selection = selection_model->selection();
        current_selection.merge( QItemSelection( index, index ), QItemSelectionModel::Select );
        selection_model->select( current_selection, QItemSelectionModel::Select );
        tag_view_->setExpanded( index.parent(), true );
    }

    update_viewer();
}
