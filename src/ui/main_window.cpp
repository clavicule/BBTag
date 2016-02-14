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
#include <QScrollArea>
#include <QImageReader>
#include <QInputDialog>
#include <QComboBox>


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
    QHBoxLayout* image_tag_layout = new QHBoxLayout();

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

    image_tag_layout->addWidget( import_button );
    image_tag_layout->addLayout( tag_tree_layout );
    image_tag_widget->setLayout( image_tag_layout );


    // --------
    // widget #3
    // right-most widget in splitter is image viewer
    QWidget* tag_viewer_widget = new QWidget( splitter );
    label_selector_ = new QComboBox( tag_viewer_widget );

    QPushButton* tag_button = new QPushButton( QIcon( ":/pixmaps/tag.png" ), "", tag_viewer_widget );
    tag_button->setCheckable( true );
    tag_button->setFixedSize( 32, 32 );
    tag_button->setIconSize( QSize( 32, 32 ) );
    tag_button->setShortcut( QKeySequence( Qt::Key_T ) );
    tag_button->setToolTip( "Use t-key as shortcut" );

    QHBoxLayout* viewer_buttons_layout = new QHBoxLayout();
    viewer_buttons_layout->addWidget( label_selector_ );
    viewer_buttons_layout->addWidget( tag_button );
    viewer_buttons_layout->setStretchFactor( label_selector_, 2 );

    tag_viewer_ = new TagViewer( tag_viewer_widget );
    tag_scroll_view_ = new TagScrollView( tag_viewer_widget );
    tag_scroll_view_->setWidget( tag_viewer_ );

    QVBoxLayout* tag_viewer_layout = new QVBoxLayout();
    tag_viewer_layout->addLayout( viewer_buttons_layout );
    tag_viewer_layout->addWidget( tag_scroll_view_ );
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

    QList<int> sizes;
    sizes << 300 << 300 << 400;
    splitter->setSizes( sizes );

    setCentralWidget( splitter );
    resize( 1000, 600 );

    // setup connections
    connect( import_button, SIGNAL( clicked() ), this, SLOT( import_images() ) );
    connect( add_button, SIGNAL( clicked() ), this, SLOT( add_label() ) );
    connect( remove_button, SIGNAL( clicked() ), this, SLOT( remove_images() ) );

    connect( tag_view_->selectionModel(), SIGNAL( selectionChanged(QItemSelection,QItemSelection) ), this, SLOT( update_viewer() ) );
    connect( label_selector_, SIGNAL( activated(int) ), this, SLOT( set_viewer_tag_options() ) );
    connect( tag_button, SIGNAL( toggled(bool) ), tag_viewer_, SLOT( set_tagging_status(bool) ) );
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
}

void MainWindow::update_viewer()
{
    QPixmap pix;
    QList<QRect> bbox;
    QColor color;
    QString label;

    QItemSelectionModel* selection_model = tag_view_->selectionModel();
    if( selection_model ) {
        QModelIndexList selection = selection_model->selectedRows();

        if( selection.count() == 1 ) {
            const QModelIndex& index = selection.first();
            TagItem::Elements elt = tag_model_->get_elements( index );

            pix.load( elt._fullpath );
            bbox = elt._bbox;
            color = elt._color;
            label = elt._label;

        }
    }

    // ok to send a null pixmap
    // the viewer will recognize that
    // and display a message instead
    tag_viewer_->set_color( color );
    tag_viewer_->set_label( label );
    tag_viewer_->set_tags( bbox );
    tag_viewer_->set_image( pix );
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
