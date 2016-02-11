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
#include <QTextCodec>


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
    QPushButton* add_button = new QPushButton( QIcon( ":/pixmaps/add.png" ), "", image_tag_widget );
    QPushButton* remove_button = new QPushButton( QIcon( ":/pixmaps/remove.png" ), "", image_tag_widget );
    add_button->setFixedSize( 32, 32 );
    remove_button->setFixedSize( 32, 32 );
    add_button->setIconSize( QSize( 32, 32 ) );
    remove_button->setIconSize( QSize( 32, 32 ) );

    QVBoxLayout* image_tag_button_layout = new QVBoxLayout();
    image_tag_button_layout->addWidget( add_button );
    image_tag_button_layout->addWidget( remove_button );

    tag_model_ = new TagModel( this );
    QTreeView* tag_tree_view = new QTreeView( image_tag_widget );
    tag_tree_view->setEditTriggers( QAbstractItemView::NoEditTriggers );
    tag_model_->attach( tag_tree_view );

    image_tag_layout->addLayout( image_tag_button_layout );
    image_tag_layout->addWidget( tag_tree_view );
    image_tag_widget->setLayout( image_tag_layout );


    // --------
    // widget #3
    // right-most widget in
    // on OSX: don't mind the debug messages when mouse hover scrollbar:
    // bug report as of Feb 6th 2016.
    // --> https://bugreports.qt.io/browse/QTBUG-49899
    tag_viewer_ = new TagViewer( splitter );
    tag_scroll_view_ = new TagScrollView( splitter );
    tag_scroll_view_->setWidget( tag_viewer_ );

    // --------
    // Main Widget
    // arrange widgets inside the splitter widget
    splitter->addWidget( dir_view_ );
    splitter->addWidget( image_tag_widget );
    splitter->addWidget( tag_scroll_view_ );
    splitter->setStretchFactor( 0, 1 );
    splitter->setStretchFactor( 1, 1 );
    splitter->setStretchFactor( 2, 3 );

    QList<int> sizes;
    sizes << 300 << 300 << 400;
    splitter->setSizes( sizes );

    setCentralWidget( splitter );
    resize( 1000, 600 );

    // setup connections
    connect( add_button, SIGNAL( clicked() ), this, SLOT( import_images() ) );
    connect( remove_button, SIGNAL( clicked() ), this, SLOT( remove_images() ) );
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

    QStringList img_to_import;

    // list all the valid image file in the selection
    QModelIndexList selection = selection_model->selectedRows();
    for( QModelIndexList::iterator s_itr = selection.begin(); s_itr != selection.end(); ++s_itr ) {

        QFileInfo img_info = dir_model_->fileInfo( *s_itr );

        // check if selection is a directory or a file
        if( img_info.isDir() ) {
            // if a directory, add all the image files from it
            QFileInfoList img_list = QDir( img_info.absoluteFilePath() ).entryInfoList( valid_image_format(), QDir::Files );
            for( QFileInfoList::iterator fi_itr = img_list.begin(); fi_itr != img_list.end(); ++fi_itr ) {
                img_to_import.append( (*fi_itr).absoluteFilePath() );
            }

        } else {
            img_to_import.append( img_info.absoluteFilePath() );
        }
    }

    tag_model_->import_images( img_to_import );
}

void MainWindow::remove_images()
{

}
