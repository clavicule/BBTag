#include <ui/main_window.h>
#include <ui/tag_viewer.h>
#include <ui/tag_scroll_view.h>
#include <core/tag_model.h>
#include <core/tag_io.h>

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
#include <QLabel>
#include <QMenuBar>
#include <QColorDialog>
#include <QMessageBox>
#include <QCheckBox>
#include <QTextBrowser>


MainWindow::MainWindow(
        QWidget *parent
    ) : QMainWindow( parent )
{
    setWindowIcon( QIcon( ":/icon/bbtag.png" ) );

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
    tag_tree_button_layout->addWidget( new QLabel( "<b>Label</b>", image_tag_widget ) );
    tag_tree_button_layout->addWidget( add_button );
    tag_tree_button_layout->addWidget( remove_button );
    tag_tree_button_layout->addStretch();

    tag_model_ = new TagModel( this );
    tag_view_ = new QTreeView( image_tag_widget );
    tag_view_->setEditTriggers( QAbstractItemView::NoEditTriggers );
    tag_view_->setSelectionMode( QAbstractItemView::ExtendedSelection );
    tag_model_->attach( tag_view_ );
    tag_view_->setContextMenuPolicy( Qt::CustomContextMenu );

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

    tag_button_ = new QPushButton( QIcon( ":/pixmaps/tag.png" ), "", tag_viewer_widget );
    tag_button_->setCheckable( true );
    tag_button_->setFixedSize( 32, 32 );
    tag_button_->setIconSize( QSize( 32, 32 ) );
    tag_button_->setShortcut( QKeySequence( Qt::Key_T ) );
    tag_button_->setToolTip( "Use t-key as shortcut" );

    untag_button_ = new QPushButton( QIcon( ":/pixmaps/untag.png" ), "", tag_viewer_widget );
    untag_button_->setCheckable( true );
    untag_button_->setFixedSize( 32, 32 );
    untag_button_->setIconSize( QSize( 32, 32 ) );
    untag_button_->setShortcut( QKeySequence( Qt::Key_D ) );
    untag_button_->setToolTip( "Use d-key as shortcut" );


    QHBoxLayout* tag_buttons_layout = new QHBoxLayout();
    tag_buttons_layout->addWidget( new QLabel( "<b>Tag</b>", tag_viewer_widget ) );
    tag_buttons_layout->addWidget( label_selector_ );
    tag_buttons_layout->addWidget( tag_button_ );
    tag_buttons_layout->addSpacing( 20 );
    tag_buttons_layout->addWidget( untag_button_ );
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
    viewer_buttons_layout->addSpacing( 20 );
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

    // menubar
    QMenu* file_menu = menuBar()->addMenu( tr("&File" ) );
    QMenu* help_menu = menuBar()->addMenu( tr( "&Help" ) );

    QAction* open_xml_action = new QAction( tr( "&Open XML" ), this );
    QAction* open_and_merge_xml_action = new QAction( tr( "Open XML and Merge" ), this );

    QAction* save_xml_action = new QAction( tr( "&Save As XML" ), this );
    QAction* save_selection_xml_action = new QAction( tr( "Save Selection As XML" ), this );

    QAction* save_images_action = new QAction( tr( "Save As Cropped Images" ), this );
    QAction* save_selection_images_action = new QAction( tr( "Save Selection As Cropped Images" ), this );

    QAction* quit_action = new QAction( tr( "&Quit" ), this );

    QAction* help_action = new QAction( QIcon( ":/pixmaps/help.png" ), tr( "&Help" ), this );
    QAction* credits_action = new QAction( QIcon( ":/pixmaps/credits.png" ), tr( "Credits" ), this );

    open_xml_action->setShortcuts( QKeySequence::Open );
    open_and_merge_xml_action->setShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_O ) );
    save_xml_action->setShortcuts( QKeySequence::Save );
    save_selection_xml_action->setShortcuts( QKeySequence::SaveAs );
    quit_action->setShortcuts( QKeySequence::Quit );
    help_action->setShortcuts( QKeySequence::HelpContents );

    file_menu->addSection( QIcon( ":/pixmaps/open.png" ), "Open" );
    file_menu->addAction( open_xml_action );
    file_menu->addAction( open_and_merge_xml_action );
    file_menu->addSection( QIcon( ":/pixmaps/save.png" ), "Save" );
    file_menu->addAction( save_xml_action );
    file_menu->addAction( save_selection_xml_action );
    file_menu->addSeparator();
    file_menu->addAction( save_images_action );
    file_menu->addAction( save_selection_images_action );
    file_menu->addSeparator();
    file_menu->addAction( quit_action );

    help_menu->addAction( help_action );
    help_menu->addAction( credits_action );

    // context menu
    QAction* change_color_action = new QAction( QIcon( ":/pixmaps/change_color.png" ), "Change color", this );
    QAction* change_name_action = new QAction( QIcon( ":/pixmaps/change_label.png" ), "Change name", this );

    context_menu_ = new QMenu( "Edit", this );
    context_menu_->addAction( change_color_action );
    context_menu_->addAction( change_name_action );


    resize( QGuiApplication::primaryScreen()->availableSize() * 0.8 );

    // setup connections
    connect( import_button, SIGNAL( clicked() ), this, SLOT( import_images() ) );
    connect( add_button, SIGNAL( clicked() ), this, SLOT( add_label() ) );
    connect( remove_button, SIGNAL( clicked() ), this, SLOT( remove_images() ) );

    connect( tag_view_, SIGNAL( customContextMenuRequested(QPoint) ), this, SLOT( show_context_menu(QPoint) ) );
    connect( tag_view_->selectionModel(), SIGNAL( selectionChanged(QItemSelection,QItemSelection) ), this, SLOT( update_viewer() ) );
    connect( label_selector_, SIGNAL( currentIndexChanged(int) ), this, SLOT( set_viewer_tag_options() ) );
    connect( tag_button_, SIGNAL( toggled(bool) ), this, SLOT( enable_tag(bool) ) );
    connect( untag_button_, SIGNAL( toggled(bool) ), this, SLOT( enable_untag(bool) ) );
    connect( tag_viewer_, SIGNAL( tagged(QRect) ), this, SLOT( tag_image(QRect) ) );
    connect( tag_viewer_, SIGNAL( untagged(QString,QRect) ), this, SLOT( untag_image(QString, QRect) ) );

    connect( zoom_in_button, SIGNAL( clicked() ), tag_scroll_view_, SLOT( zoom_in() ) );
    connect( zoom_out_button, SIGNAL( clicked() ), tag_scroll_view_, SLOT( zoom_out() ) );
    connect( fit_to_view_button, SIGNAL( clicked() ), tag_scroll_view_, SLOT( fit_to_view() ) );

    connect( open_xml_action, SIGNAL( triggered() ), this, SLOT( open_xml() ) );
    connect( open_and_merge_xml_action, SIGNAL( triggered() ), this, SLOT( open_xml_and_merge() ) );
    connect( save_xml_action, SIGNAL( triggered() ), this, SLOT( save_as_xml() ) );
    connect( save_selection_xml_action, SIGNAL( triggered() ), this, SLOT( save_selection_as_xml() ) );
    connect( save_images_action, SIGNAL( triggered() ), this, SLOT( save_as_images() ) );
    connect( save_selection_images_action, SIGNAL( triggered() ), this, SLOT( save_selection_as_images() ) );
    connect( quit_action, SIGNAL( triggered() ), this, SLOT( close() ) );
    connect( help_action, SIGNAL( triggered() ), this, SLOT( show_help() ) );
    connect( credits_action, SIGNAL( triggered() ), this, SLOT( show_credits() ) );
    connect( change_color_action, SIGNAL( triggered() ), this, SLOT( change_selected_label_color() ) );
    connect( change_name_action, SIGNAL( triggered() ), this, SLOT( change_selected_label_name() ) );

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
    label_selector_->setCurrentText( new_label_name );
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
            // not an image --> keep going
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

void MainWindow::show_context_menu( const QPoint& pos )
{
    // ensure a label is selected
    selected_for_context_ = QModelIndex();
    QModelIndex index = tag_view_->indexAt( pos );

    if( index.isValid() &&
        index.parent() == tag_view_->rootIndex() &&
        tag_model_->get_label( index ) != TagModel::UNTAGGED &&
        tag_model_->get_label( index ) != TagModel::ALL
    ) {
        selected_for_context_ = index;
        context_menu_->exec( tag_view_->mapToGlobal( pos ) );
    }
}

void MainWindow::change_selected_label_color()
{
    if( !selected_for_context_.isValid() ) {
        return;
    }

    QColor selected_color = QColorDialog::getColor( tag_model_->get_color( selected_for_context_ ), this );
    if( !selected_color.isValid() ) {
        selected_for_context_ = QModelIndex();
        return;
    }

    tag_model_->set_color( selected_for_context_, selected_color );
    tag_view_->update( selected_for_context_ );
    update_tag_selector();
    update_viewer();
    selected_for_context_ = QModelIndex();
}

void MainWindow::change_selected_label_name()
{
    if( !selected_for_context_.isValid() ) {
        return;
    }

    QString selected_name = QInputDialog::getText( this, "Edit label name", "Change label name", QLineEdit::Normal, tag_model_->get_label( selected_for_context_ ) );
    if( selected_name.isEmpty() ) {
        selected_for_context_ = QModelIndex();
        return;
    }

    tag_model_->set_label( selected_for_context_, selected_name );
    tag_view_->update( selected_for_context_ );
    update_tag_selector();
    update_viewer();
    selected_for_context_ = QModelIndex();
}

void MainWindow::pop_up_file_dialog(
        QString& filename,
        QString& relative_dir,
        QFileDialog::AcceptMode mode
    )
{
    QDialog file_dialog( this );
    QLabel* file_label = new QLabel( &file_dialog );
    QLabel* dir_label = new QLabel( &file_dialog );
    dir_label->setEnabled( false );

    QPushButton* popup_file = new QPushButton( QIcon( ":/pixmaps/open.png" ), QString(), &file_dialog );
    QPushButton* popup_dir = new QPushButton( QIcon( ":/pixmaps/open.png" ), QString(), &file_dialog );
    popup_dir->setEnabled( false );

    QFileDialog* xml_dialog = new QFileDialog( &file_dialog );
    QFileDialog* dir_dialog = new QFileDialog( &file_dialog );

    xml_dialog->setFilter( QDir::Files );
    xml_dialog->setAcceptMode( mode );
    xml_dialog->selectNameFilter( "XML Files (*.xml)" );
    xml_dialog->setWindowTitle( "Select XML file" );
    xml_dialog->setDirectory( QDir::current() );

    dir_dialog->setOptions( QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    dir_dialog->setFileMode( QFileDialog::DirectoryOnly );
    dir_dialog->setWindowTitle( "Select path relative to directory" );
    dir_dialog->setDirectory( QDir::current() );

    QCheckBox* enable_dir = new QCheckBox( "Use image path relative to directory: ", &file_dialog );
    enable_dir->setChecked( false );

    QLabel* dir_info_label = new QLabel( "Relative path from: ", &file_dialog );
    dir_info_label->setEnabled( false );

    QHBoxLayout* file_layout = new QHBoxLayout();
    file_layout->addWidget( new QLabel( "Choose XML file: ", &file_dialog ) );
    file_layout->addWidget( file_label );
    file_layout->addWidget( popup_file );
    file_layout->setStretchFactor( file_label, 2 );

    QHBoxLayout* dir_layout = new QHBoxLayout();
    dir_layout->addWidget( dir_info_label );
    dir_layout->addWidget( dir_label );
    dir_layout->addWidget( popup_dir );
    dir_layout->setStretchFactor( dir_label, 2 );

    QPushButton* ok_button = new QPushButton( "OK", &file_dialog );
    QPushButton* cancel_button = new QPushButton( "Cancel", &file_dialog );

    QHBoxLayout* button_layout = new QHBoxLayout();
    button_layout->addStretch();
    button_layout->addWidget( ok_button );
    button_layout->addWidget( cancel_button );

    QVBoxLayout* main_layout = new QVBoxLayout();
    main_layout->addLayout( file_layout );
    main_layout->addWidget( enable_dir );
    main_layout->addLayout( dir_layout );
    main_layout->addStretch();
    main_layout->addLayout( button_layout );

    file_dialog.setLayout( main_layout );

    connect( enable_dir, SIGNAL( toggled(bool) ), popup_dir, SLOT( setEnabled(bool) ) );
    connect( enable_dir, SIGNAL( toggled(bool) ), dir_label, SLOT( setEnabled(bool) ) );
    connect( enable_dir, SIGNAL( toggled(bool) ), dir_info_label, SLOT( setEnabled(bool) ) );
    connect( popup_file, SIGNAL( clicked() ), xml_dialog, SLOT( exec() ) );
    connect( popup_dir, SIGNAL( clicked() ), dir_dialog, SLOT( exec() ) );
    connect( xml_dialog, SIGNAL( fileSelected(QString) ), file_label, SLOT( setText(QString) ) );
    connect( dir_dialog, SIGNAL( fileSelected(QString) ), dir_label, SLOT( setText(QString) ) );
    connect( ok_button, SIGNAL( clicked() ), &file_dialog, SLOT( accept() ) );
    connect( cancel_button, SIGNAL( clicked() ), &file_dialog, SLOT( reject() ) );

    file_dialog.exec();

    if( file_dialog.result() == QDialog::Rejected ) {
        return;
    }

    QString xml_file = file_label->text();
    if( xml_file.isEmpty() ) {
        return;
    }

    relative_dir = enable_dir->isChecked()? dir_label->text() : QString();
    filename = xml_file;
}

void MainWindow::open_xml_and_merge()
{
    QString filename;
    QString relative_dir;
    pop_up_file_dialog( filename, relative_dir, QFileDialog::AcceptOpen );
    if( filename.isEmpty() ) {
        return;
    }

    load_xml( filename, relative_dir, true );
}

void MainWindow::open_xml()
{
    QString filename;
    QString relative_dir;
    pop_up_file_dialog( filename, relative_dir, QFileDialog::AcceptOpen );
    if( filename.isEmpty() ) {
        return;
    }

    load_xml( filename, relative_dir, false );
}

void MainWindow::save_as_xml()
{
    QString filename;
    QString relative_dir;
    pop_up_file_dialog( filename, relative_dir, QFileDialog::AcceptSave );
    if( filename.isEmpty() ) {
        return;
    }

    save_xml( filename, relative_dir, QModelIndexList() );
}

void MainWindow::save_selection_as_xml()
{
    QItemSelectionModel* selection_model = tag_view_->selectionModel();
    if( !selection_model ) {
        QMessageBox::critical( this, "Error", "No valid selection" );
        return;
    }
    QModelIndexList selection = selection_model->selectedRows();

    QString filename;
    QString relative_dir;
    pop_up_file_dialog( filename, relative_dir, QFileDialog::AcceptSave );
    if( filename.isEmpty() ) {
        return;
    }

    save_xml( filename, relative_dir, selection );
}

void MainWindow::load_xml(
        const QString& filename,
        const QString& relative_dir,
        bool merge
    )
{
    QFile file( filename );
    if( !file.open( QFile::ReadOnly | QFile::Text ) ) {
        QMessageBox::critical( this, "Error", "Failed to read file " + file.errorString() );
        return;
    }

    QHash< QString, QList<TagItem::Elements> > elts;
    if( !TagIO::read_xml( &file, relative_dir, elts ) ) {
        QMessageBox::critical( this, "Error", "Failed to recognize file format/elements" );

    } else {
        tag_model_->init_from_elements( elts, merge );
        update_tag_selector();
        update_viewer();
    }

    file.close();
}

void MainWindow::save_xml(
        const QString& filename,
        const QString& relative_dir,
        const QModelIndexList& selection
    )
{
    QFile file( filename );
    if( !file.open( QFile::WriteOnly | QFile::Text ) ) {
        QMessageBox::critical( this, "Error", "Failed to write file " + file.errorString() );
        return;
    }

    TagIO::write_xml( &file, relative_dir, tag_model_->get_all_tags(), tag_model_->get_all_elements( selection ) );
    file.close();
}

void MainWindow::save_as_images()
{
    QString dir = QFileDialog::getExistingDirectory( this, "Select directory where to save images", QDir::currentPath() );
    if( dir.isEmpty() ) {
        return;
    }

    TagIO::write_images( QDir( dir ), tag_model_->get_all_elements( QModelIndexList() ) );
}

void MainWindow::save_selection_as_images()
{
    QItemSelectionModel* selection_model = tag_view_->selectionModel();
    if( !selection_model ) {
        QMessageBox::critical( this, "Error", "No valid selection" );
        return;
    }
    QModelIndexList selection = selection_model->selectedRows();

    QString dir = QFileDialog::getExistingDirectory( this, "Select directory where to save images", QDir::currentPath() );
    if( dir.isEmpty() ) {
        return;
    }

    TagIO::write_images( QDir( dir ), tag_model_->get_all_elements( selection ) );
}

void MainWindow::show_help()
{
    pop_up_html_dialog( QUrl( "qrc:/html/Help.html" ) );
}

void MainWindow::show_credits()
{
    pop_up_html_dialog( QUrl( "qrc:/html/Credits.html" ) );
}

void MainWindow::pop_up_html_dialog(
        const QUrl& url
    )
{
    QDialog html_dialog( this );

    QTextBrowser* text_help = new QTextBrowser( &html_dialog );
    text_help->setOpenExternalLinks( true );
    text_help->setSource( url );

    QPushButton* ok_button = new QPushButton( "OK", &html_dialog );

    QHBoxLayout* button_layout = new QHBoxLayout();
    button_layout->addStretch();
    button_layout->addWidget( ok_button );

    QVBoxLayout* main_layout = new QVBoxLayout();
    main_layout->addWidget( text_help );
    main_layout->addLayout( button_layout );
    main_layout->setStretchFactor( text_help, 2 );
    html_dialog.setLayout( main_layout );

    connect( ok_button, SIGNAL( clicked() ), &html_dialog, SLOT( accept() ) );
    html_dialog.resize( size() * 2 / 3 );
    html_dialog.exec();
}

void MainWindow::update_viewer()
{
    QPixmap pix;
    QList<TagViewer::TagDisplayElement> display_elements;

    QItemSelectionModel* selection_model = tag_view_->selectionModel();
    if( !selection_model ) {
        return;
    }

    QModelIndexList selection = selection_model->selectedRows();

    // iterate through the selection:
    // if multiple selection, check that all selected item reference to the same image (fullpath)
    // otherwise null data is sent to the viewer
    // in case of a label name being selected, consider the same image belonging to that label is selected

    // ensure and get the single image referenced in the selection
    QString fullpath_ref = get_image_from_index_list( selection );
    if( !fullpath_ref.isEmpty() ) {

        QList<TagItem::Elements> item_elts = tag_model_->get_elements( selection );

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
    QHash<QString, QColor> tags = tag_model_->get_all_tags();

    for( QHash<QString, QColor>::iterator tag_itr = tags.begin(); tag_itr != tags.end(); ++tag_itr ) {
        int idx = label_selector_->count();
        label_selector_->insertItem( idx, tag_itr.key() );
        label_selector_->setItemData( idx, QVariant( tag_itr.value() ), Qt::DecorationRole );
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

// using exclusive button group doesn't allow
// to have both buttons unchecked
// hence these 2 slots
void MainWindow::enable_tag(
        bool activate
    )
{
    if( activate ) {
        tag_viewer_->set_untagging_status( false );
        untag_button_->setChecked( false );
    }
    tag_viewer_->set_tagging_status( activate );
}

// using exclusive button group doesn't allow
// to have both buttons unchecked
// hence these 2 slots
void MainWindow::enable_untag(
        bool activate
    )
{
    if( activate ) {
        tag_viewer_->set_tagging_status( false );
        tag_button_->setChecked( false );
    }
    tag_viewer_->set_untagging_status( activate );
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

    QModelIndexList selection = selection_model->selectedRows();

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

    QModelIndexList selection = selection_model->selectedRows();

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
