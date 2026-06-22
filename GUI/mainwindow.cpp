#include "mainwindow.h"
#include "ThemeManager.h"
#include "userdialog.h"
#include "supplierdialog.h"
#include "productdialog.h"
#include "articledialog.h"
#include "articlereaderdialog.h"
#include "customerdialog.h"
#include "orderstatusdialog.h"
#include "deliverydialog.h"
#include <QInputDialog> // NEW: For the Order Status popup
#include <QStringList>
#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QMessageBox>
#include <QDateTime>
#include <QProgressBar>
#include <QGroupBox>
#include <QFormLayout>
#include <QFrame>
#include <QApplication>
#include <QSettings>
#include <QProcess>
#include <QFile>
#include <QDir>
#ifdef HAS_QT_SVG
#include <QSvgRenderer>
#endif

// --- Backend Core Includes ---
#include "../Core/clsUser.h"
#include "../Core/clsSupplier.h"
#include "../Core/clsCustomer.h"
#include "../Core/clsProduct.h"
#include "../Core/clsOrder.h"
#include "../Core/clsArticle.h"
#include "../Core/clsInventory.h"
#include "../Core/clsReport.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi();
    refreshAllStyles();
    sidebarWidget->hide();
    stackedScreens->setCurrentIndex(ScreenIndex::Login);
    setWindowIcon(QIcon(createLogo(64)));
}

MainWindow::~MainWindow() {}

QPixmap MainWindow::createLogo(int size)
{
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);

    // Try loading SVG logo from Assets folder
#ifdef HAS_QT_SVG
    QString svgPath = QApplication::applicationDirPath() + "/../Assets/REOSSA_logo_enhanced.svg";
    if (!QFile::exists(svgPath))
        svgPath = QApplication::applicationDirPath() + "/Assets/REOSSA_logo_enhanced.svg";
    if (!QFile::exists(svgPath))
        svgPath = QDir::currentPath() + "/Assets/REOSSA_logo_enhanced.svg";

    if (QFile::exists(svgPath)) {
        QSvgRenderer renderer(svgPath);
        if (renderer.isValid()) {
            QRectF bounds = renderer.viewBoxF();
            QPixmap native(bounds.size().toSize());
            native.fill(Qt::transparent);
            QPainter p(&native);
            p.setRenderHint(QPainter::Antialiasing);
            p.setRenderHint(QPainter::SmoothPixmapTransform);
            renderer.render(&p);
            p.end();

            QImage img = native.toImage();
            int x1 = img.width(), y1 = img.height(), x2 = 0, y2 = 0;
            for (int y = 0; y < img.height(); ++y) {
                const QRgb *row = reinterpret_cast<const QRgb*>(img.constScanLine(y));
                for (int x = 0; x < img.width(); ++x) {
                    if (qAlpha(row[x]) > 40) {
                        if (x < x1) x1 = x;
                        if (x > x2) x2 = x;
                        if (y < y1) y1 = y;
                        if (y > y2) y2 = y;
                    }
                }
            }

            if (x1 < x2 && y1 < y2) {
                QPixmap cropped = QPixmap::fromImage(img.copy(x1, y1, x2 - x1 + 1, y2 - y1 + 1));
                pix = cropped.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            } else {
                pix = native.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
            return pix;
        }
    }
#endif

    // Fallback: QPainter-based logo
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    int half = size / 2;
    qreal r = half * 0.92;

    QRadialGradient bg(half, half, r, half * 0.6, half * 0.6);
    bg.setColorAt(0, QColor("#2ecc71"));
    bg.setColorAt(0.6, QColor("#27ae60"));
    bg.setColorAt(1, QColor("#1e8449"));
    p.setBrush(bg);
    p.setPen(QPen(QColor("#1abc9c"), qMax(size / 60.0, 1.5)));
    p.drawEllipse(QPointF(half, half), r, r);

    int boneY = half - half / 3;
    int bw = half * 0.8;
    int bh = qMax(half / 8.0, 2.0);
    int knob = qMax(half / 6.0, 2.0);

    QRectF shaftRect(half - bw / 2, boneY - bh / 2, bw, bh);
    p.setBrush(QColor("#ffffff"));
    p.setPen(QPen(QColor("#1e8449"), qMax(1.0, size / 100.0)));
    p.drawRoundedRect(shaftRect, bh / 3, bh / 3);
    p.drawEllipse(QPointF(half - bw / 2, boneY), knob, knob);
    p.drawEllipse(QPointF(half + bw / 2, boneY), knob, knob);

    int lx = half;
    int ly = boneY - half / 3;
    int leafW = half / 2;
    int leafH = half / 2.5;

    QPainterPath leaf;
    leaf.moveTo(lx, ly - leafH / 2);
    leaf.cubicTo(lx + leafW / 2, ly - leafH / 3, lx + leafW / 3, ly + leafH / 4, lx, ly + leafH / 6);
    leaf.cubicTo(lx - leafW / 3, ly + leafH / 4, lx - leafW / 2, ly - leafH / 3, lx, ly - leafH / 2);
    p.setBrush(QColor("#f1c40f"));
    p.setPen(Qt::NoPen);
    p.drawPath(leaf);

    p.setPen(QPen(QColor("#f39c12"), qMax(size / 40.0, 1.5)));
    p.drawLine(lx, ly + leafH / 6, lx, ly + leafH / 3);

    int fontSize = qMax(size / 5, 9);
    QFont f("Segoe UI", fontSize, QFont::Bold);
    p.setFont(f);
    QRectF textRect(half - r * 0.9, half + half / 6, r * 1.8, half / 2);
    p.setPen(QColor(0, 0, 0, 50));
    p.drawText(textRect.translated(1, 1), Qt::AlignCenter, "REOSSA");
    p.setPen(QColor("#ffffff"));
    p.drawText(textRect, Qt::AlignCenter, "REOSSA");

    p.end();
    return pix;
}




void MainWindow::setupUi()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setupSidebar();

    stackedScreens = new QStackedWidget(this);
    mainLayout->addWidget(sidebarWidget);
    mainLayout->addWidget(stackedScreens, 1);

    // Initialize all 10 screens
    setupLoginScreen();
    setupDashboardScreen();
    setupUsersScreen();
    setupSuppliersScreen();
    setupCustomersScreen();
    setupProductsScreen();
    setupOrdersScreen();
    setupInventoryScreen();
    setupReportsScreen();
    setupArticlesScreen();
    setupDeliveriesScreen();
    // NEW:
    setupSupplierDashboardScreen();
    setupMyDeliveriesScreen();
    setupMyRewardsScreen();
    setupMyProfileScreen();
    // Customer screens
    setupCustDashboardScreen();
    setupCustProductsScreen();
    setupCustOrdersScreen();
    setupCustPointsScreen();
    setupCustProfileScreen();
}

// --- UI Helper Method ---
// This saves hundreds of lines of code by standardizing table creation
QTableWidget* MainWindow::createStandardTable(QStringList headers)
{
    QTableWidget* table = new QTableWidget(0, headers.size(), this);
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    return table;
}

void MainWindow::setupSidebar()
{
    sidebarWidget = new QWidget(this);
    sidebarWidget->setFixedWidth(220);
    sidebarWidget->setStyleSheet("background-color: #2c3e50; color: white;");
    sidebarLayout = new QVBoxLayout(sidebarWidget);
    sidebarLayout->setContentsMargins(0, 10, 0, 0);

    // Logo header
    QLabel *logoLabel = new QLabel(this);
    logoLabel->setPixmap(createLogo(80));
    logoLabel->setFixedSize(80, 80);
    logoLabel->setAlignment(Qt::AlignCenter);
    sidebarLayout->addWidget(logoLabel, 0, Qt::AlignCenter);

    QLabel *brand = new QLabel("REOSSA", this);
    brand->setStyleSheet("font-size: 16px; font-weight: bold; color: #1abc9c; border: none; padding: 0; margin: 0;");
    brand->setAlignment(Qt::AlignCenter);
    sidebarLayout->addWidget(brand);

    // Decorative separator
    QFrame *sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("border: none; background-color: #34495e; max-height: 1px; margin: 5px 15px 5px 15px;");
    sidebarLayout->addWidget(sep);

    btnDashboard = new QPushButton(tr(" Dashboard"), this);
    btnUsers = new QPushButton(tr(" Users"), this);
    btnSuppliers = new QPushButton(tr(" Suppliers"), this);
    btnCustomers = new QPushButton(tr(" Customers"), this);
    btnProducts = new QPushButton(tr(" Products"), this);
    btnOrders = new QPushButton(tr(" Orders"), this);
    btnInventory = new QPushButton(tr(" Inventory Alerts"), this);
    btnReports = new QPushButton(tr(" Reports"), this);
    btnArticles = new QPushButton(tr(" Articles"), this);
    btnLogout = new QPushButton(tr(" Logout"), this);
    btnDeliveries = new QPushButton(tr(" Deliveries"), this);

    //Supplier Part :
    btnSupDashboard = new QPushButton(tr(" Dashboard"), this);
    btnMyDeliveries = new QPushButton(tr(" My Deliveries"), this);
    btnMyRewards    = new QPushButton(tr(" My Points & Badges"), this);
    btnMyProfile    = new QPushButton(tr(" My Profile"), this);

    //Customer Part :
    btnCustDashboard = new QPushButton(tr(" Dashboard"), this);
    btnCustProducts = new QPushButton(tr(" Products"), this);
    btnCustOrders   = new QPushButton(tr(" My Orders"), this);
    btnCustPoints   = new QPushButton(tr(" My Points"), this);
    btnCustProfile  = new QPushButton(tr(" My Profile"), this);

    QPushButton* buttons[] = {btnDashboard, btnUsers, btnSuppliers, btnCustomers,
                              btnProducts, btnOrders, btnInventory, btnReports, btnArticles,btnDeliveries,
                              btnSupDashboard,btnMyDeliveries,btnMyRewards,btnMyProfile,
                              btnCustDashboard,btnCustProducts,btnCustOrders,btnCustPoints,btnCustProfile};

   for (QPushButton* btn : buttons) {
        // We use a specific QSS rule just for the sidebar buttons here
        btn->setStyleSheet(R"(
            QPushButton {
                text-align: left;
                padding: 12px 15px;
                border: none;
                font-size: 15px;
                background-color: transparent;
                color: #bdc3c7;
                border-radius: 6px;
                margin: 2px 10px;
            }
            QPushButton:hover {
                background-color: #34495e;
                color: #1abc9c;
            }
        )");
        btn->setCursor(Qt::PointingHandCursor);
        sidebarLayout->addWidget(btn);
    }

    sidebarLayout->addStretch();

    // Custom style just for the Logout button on the sidebar
    btnLogout->setStyleSheet(R"(
        QPushButton {
            background-color: #c0392b;
            color: white;
            padding: 12px;
            font-weight: bold;
            border-radius: 6px;
            margin: 10px;
        }
        QPushButton:hover { background-color: #a53125; }
    )");
    sidebarLayout->addWidget(btnLogout);

    // Language switch button
    QPushButton *btnLang = new QPushButton(tr("  [ EN / AR ]"), this);
    btnLang->setStyleSheet(R"(
        QPushButton {
            background-color: rgba(255,255,255,0.07);
            color: #cbd5e1;
            padding: 10px 16px;
            font-size: 13px;
            font-weight: 600;
            border-radius: 20px;
            margin: 4px 12px;
            border: 1px solid rgba(255,255,255,0.06);
        }
        QPushButton:hover {
            background-color: rgba(255,255,255,0.14);
            color: #60a5fa;
            border: 1px solid rgba(96,165,250,0.3);
        }
    )");
    btnLang->setCursor(Qt::PointingHandCursor);
    sidebarLayout->addWidget(btnLang);
    connect(btnLang, &QPushButton::clicked, this, &MainWindow::switchLanguage);

    // Theme toggle button
    QPushButton *btnTheme = new QPushButton(tr(" Theme"), this);
    btnTheme->setObjectName("sidebarThemeBtn");
    btnTheme->setStyleSheet(R"(
        QPushButton {
            background-color: rgba(255,255,255,0.07);
            color: #cbd5e1;
            padding: 10px 16px;
            font-size: 13px;
            font-weight: 600;
            border-radius: 20px;
            margin: 4px 12px;
            border: 1px solid rgba(255,255,255,0.06);
        }
        QPushButton:hover {
            background-color: rgba(255,255,255,0.14);
            color: #fbbf24;
            border: 1px solid rgba(251,191,36,0.3);
        }
    )");
    QString themeIcon = ThemeManager::instance().currentTheme() == "dark"
        ? QString::fromUtf8("\xE2\x98\xBE ")   // ☾
        : QString::fromUtf8("\xE2\x98\x80 ");   // ☀
    btnTheme->setText(themeIcon + tr(" Theme"));
    btnTheme->setCursor(Qt::PointingHandCursor);
    sidebarLayout->addWidget(btnTheme);
    connect(btnTheme, &QPushButton::clicked, this, &MainWindow::toggleTheme);

    // Routing
    connect(btnDashboard, &QPushButton::clicked, [this](){ navigateToScreen(ScreenIndex::Dashboard); });
    connect(btnUsers, &QPushButton::clicked, [this](){ navigateToScreen(ScreenIndex::Users); });
    connect(btnSuppliers, &QPushButton::clicked, [this](){ navigateToScreen(ScreenIndex::Suppliers); });
    connect(btnCustomers, &QPushButton::clicked, [this](){ navigateToScreen(ScreenIndex::Customers); });
    connect(btnProducts, &QPushButton::clicked, [this](){ navigateToScreen(ScreenIndex::Products); });
    connect(btnOrders, &QPushButton::clicked, [this](){ navigateToScreen(ScreenIndex::Orders); });
    connect(btnInventory, &QPushButton::clicked, [this](){ navigateToScreen(ScreenIndex::Inventory); });
    connect(btnReports, &QPushButton::clicked, [this](){ navigateToScreen(ScreenIndex::Reports); });
    connect(btnArticles, &QPushButton::clicked, [this](){ navigateToScreen(ScreenIndex::Articles); });
    connect(btnLogout, &QPushButton::clicked, this, &MainWindow::logout);
    connect(btnDeliveries, &QPushButton::clicked, [this]() {
        navigateToScreen(ScreenIndex::Deliveries);
        loadDeliveriesData();
    });

    // Connect the Supplier buttons to their screens
    connect(btnSupDashboard, &QPushButton::clicked, [this]() { navigateToScreen(ScreenIndex::SupDashboard); });
    connect(btnMyDeliveries, &QPushButton::clicked, [this]() { navigateToScreen(ScreenIndex::MyDeliveries); });
    connect(btnMyRewards, &QPushButton::clicked, [this]() { navigateToScreen(ScreenIndex::MyRewards); });
    connect(btnMyProfile, &QPushButton::clicked, [this]() { navigateToScreen(ScreenIndex::MyProfile); });

    // Connect the Customer buttons to their screens
    connect(btnCustDashboard, &QPushButton::clicked, [this]() { navigateToScreen(ScreenIndex::CustDashboard); });
    connect(btnCustProducts, &QPushButton::clicked, [this]() { navigateToScreen(ScreenIndex::CustProducts); });
    connect(btnCustOrders, &QPushButton::clicked, [this]() { navigateToScreen(ScreenIndex::CustOrders); });
    connect(btnCustPoints, &QPushButton::clicked, [this]() { navigateToScreen(ScreenIndex::CustPoints); });
    connect(btnCustProfile, &QPushButton::clicked, [this]() { navigateToScreen(ScreenIndex::CustProfile); });
}

void MainWindow::navigateToScreen(int index)
{
    // Role gate: prevent accessing screens outside the user's role
    if (_currentRole == clsUser::enRole::Supplier && index < ScreenIndex::SupDashboard) {
        // Suppliers may access Articles (read-only)
        if (index != ScreenIndex::Articles) return;
    }
    if (_currentRole == clsUser::enRole::Customer) {
        // Customers may access their screens and Articles
        if (index != ScreenIndex::Articles && (index < ScreenIndex::CustDashboard || index > ScreenIndex::CustProfile))
            return;
    }

    stackedScreens->setCurrentIndex(index);

    // Auto-load data when switching tabs
    switch(index) {
        case ScreenIndex::Dashboard: refreshDashboard(); break;
        case ScreenIndex::Users: loadUsersData(); break;
        case ScreenIndex::Suppliers: loadSuppliersData(); break;
        case ScreenIndex::Customers: loadCustomersData(); break;
        case ScreenIndex::Products: loadProductsData(); break;
        case ScreenIndex::Orders: loadOrdersData(); break;
        case ScreenIndex::Inventory: loadInventoryData(); break;
        case ScreenIndex::Reports: loadAllReportsData(); break;
        case ScreenIndex::Articles: loadArticlesData(); break;
        case ScreenIndex::Deliveries: loadDeliveriesData(); break;
        case ScreenIndex::SupDashboard: refreshSupplierDashboard(); break;
        case ScreenIndex::MyDeliveries: refreshMyDeliveriesTable(); break;
        case ScreenIndex::MyRewards: refreshMyRewardsScreen(); break;
        case ScreenIndex::MyProfile: refreshMyProfileScreen(); break;
        case ScreenIndex::CustDashboard: break;
        case ScreenIndex::CustProducts: break;
        case ScreenIndex::CustOrders: break;
        case ScreenIndex::CustPoints: loadCustDashboardData(); break;
        case ScreenIndex::CustProfile: loadCustDashboardData(); break;
    }
}

void MainWindow::applyRolePermissions(int role)
{
    // 1. Hide ALL buttons by default
    btnDashboard->setVisible(false);
    btnUsers->setVisible(false);
    btnSuppliers->setVisible(false);
    btnCustomers->setVisible(false);
    btnProducts->setVisible(false);
    btnOrders->setVisible(false);
    btnInventory->setVisible(false);
    btnReports->setVisible(false);
    btnDeliveries->setVisible(false);

    btnSupDashboard->setVisible(false);
    btnMyDeliveries->setVisible(false);
    btnMyRewards->setVisible(false);
    btnMyProfile->setVisible(false);

    btnCustDashboard->setVisible(false);
    btnCustProducts->setVisible(false);
    btnCustOrders->setVisible(false);
    btnCustPoints->setVisible(false);
    btnCustProfile->setVisible(false);

    btnArticles->setVisible(false);

    btnAdd->setVisible(false);
    btnEdit->setVisible(false);
    btnDel->setVisible(false);

    // 2. Show buttons based on role
    if (role == clsUser::enRole::Admin)
    {
        btnDashboard->setVisible(true);
        btnUsers->setVisible(true);
        btnSuppliers->setVisible(true);
        btnCustomers->setVisible(true);
        btnProducts->setVisible(true);
        btnOrders->setVisible(true);
        btnInventory->setVisible(true);
        btnReports->setVisible(true);
        btnDeliveries->setVisible(true);
        btnArticles->setVisible(true);
        btnAdd->setVisible(true);
        btnEdit->setVisible(true);
        btnDel->setVisible(true);

        _currentRole = clsUser::enRole::Admin;
        stackedScreens->setCurrentIndex(ScreenIndex::Dashboard);
        refreshDashboard();
    }
    else if (role == clsUser::enRole::Supplier)
    {
        btnSupDashboard->setVisible(true);
        btnMyDeliveries->setVisible(true);
        btnMyRewards->setVisible(true);
        btnMyProfile->setVisible(true);
        btnArticles->setVisible(true);

        _currentRole = clsUser::enRole::Supplier;
        stackedScreens->setCurrentIndex(ScreenIndex::SupDashboard);

        refreshSupplierDashboard();
        refreshMyDeliveriesTable();
        refreshMyRewardsScreen();
        refreshMyProfileScreen();
    }
    else if (role == clsUser::enRole::Customer)
    {
        btnCustDashboard->setVisible(true);
        btnCustProducts->setVisible(true);
        btnCustOrders->setVisible(true);
        btnCustPoints->setVisible(true);
        btnCustProfile->setVisible(true);
        btnArticles->setVisible(true);

        _currentRole = clsUser::enRole::Customer;
        stackedScreens->setCurrentIndex(ScreenIndex::CustDashboard);
        loadCustDashboardData();
        loadArticlesData();
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(tr("Welcome"));
        msgBox.setText(tr("Welcome to the Bone Fertilizer System.\nYou can browse our articles and knowledge base."));
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
    }
}

// ==========================================
// 1. LOGIN SCREEN
// ==========================================

void MainWindow::setupLoginScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);
    l->setAlignment(Qt::AlignCenter);
    l->setSpacing(15);

    // Logo
    QLabel *logoLabel = new QLabel(this);
    logoLabel->setPixmap(createLogo(180));
    logoLabel->setFixedSize(180, 180);
    logoLabel->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel(tr("Secure Login"), this);
    title->setStyleSheet("font-size: 18px; font-weight: 600; color: #5d6d7e; margin-top: 0; letter-spacing: 1px;");

    txtUsername = new QLineEdit(this);
    txtUsername->setPlaceholderText(tr("Username"));
    txtUsername->setFixedSize(300, 40);

    txtPassword = new QLineEdit(this);
    txtPassword->setPlaceholderText(tr("Password"));
    txtPassword->setEchoMode(QLineEdit::Password);
    txtPassword->setFixedSize(300, 40);

    QPushButton *btnLogin = new QPushButton(tr("Secure Login"), this);
    btnLogin->setFixedSize(300, 45);
    btnLogin->setStyleSheet("background-color: #2980b9; color: white; font-weight: bold; font-size: 16px;");

    // Language toggle
    QPushButton *btnLang = new QPushButton(tr("[ EN / AR ]"), this);
    btnLang->setFixedSize(300, 40);
    btnLang->setStyleSheet(
        "QPushButton {"
        "  background-color: transparent;"
        "  color: #64748b;"
        "  font-size: 13px;"
        "  font-weight: 600;"
        "  border: 1.5px solid #cbd5e1;"
        "  border-radius: 20px;"
        "  padding: 8px;"
        "}"
        "QPushButton:hover {"
        "  border: 1.5px solid #2563eb;"
        "  color: #2563eb;"
        "  background-color: rgba(37,99,235,0.06);"
        "}"
    );
    btnLang->setCursor(Qt::PointingHandCursor);

    l->addWidget(logoLabel, 0, Qt::AlignCenter);
    l->addWidget(title, 0, Qt::AlignCenter);
    l->addWidget(txtUsername, 0, Qt::AlignCenter);
    l->addWidget(txtPassword, 0, Qt::AlignCenter);
    l->addWidget(btnLogin, 0, Qt::AlignCenter);
    l->addSpacing(10);
    l->addWidget(btnLang, 0, Qt::AlignCenter);
    l->addSpacing(6);

    // Theme toggle on login screen
    QPushButton *btnLoginTheme = new QPushButton(this);
    btnLoginTheme->setObjectName("loginThemeBtn");
    btnLoginTheme->setFixedSize(300, 40);
    btnLoginTheme->setCursor(Qt::PointingHandCursor);
    updateLoginThemeButton(btnLoginTheme);
    l->addWidget(btnLoginTheme, 0, Qt::AlignCenter);

    connect(btnLogin, &QPushButton::clicked, this, &MainWindow::handleLogin);
    connect(btnLang, &QPushButton::clicked, this, &MainWindow::switchLanguage);
    connect(btnLoginTheme, &QPushButton::clicked, this, [this, btnLoginTheme]() {
        ThemeManager &tm = ThemeManager::instance();
        tm.loadTheme(tm.currentTheme() == "light" ? "dark" : "light");
        updateLoginThemeButton(btnLoginTheme);
    });
    stackedScreens->insertWidget(ScreenIndex::Login, w);
}

void MainWindow::handleLogin()
{
    string user = txtUsername->text().toStdString();
    string pass = txtPassword->text().toStdString();

    clsUser CurrentUser = clsUser::FindByUsernameAndPassword(user, pass);

    if (!CurrentUser.IsEmpty() && CurrentUser.IsActive()) {
        txtPassword->clear();

        _currentUser = CurrentUser;
        _currentSupplierID.clear();
        _currentCustomerID.clear();

        if (CurrentUser.Role() == clsUser::enRole::Supplier) {
            _currentSupplierID = CurrentUser.SupplierID();
        }
        if (CurrentUser.Role() == clsUser::enRole::Customer) {
            _currentCustomerID = CurrentUser.CustomerID();
        }

        sidebarWidget->show();
        applyRolePermissions(CurrentUser.Role());

    } else {
        QMessageBox::critical(this, tr("Access Denied"), tr("Invalid Username/Password or Inactive Account."));
    }
}

void MainWindow::logout()
{
    sidebarWidget->hide();
    txtPassword->clear();
    _currentSupplierID.clear();
    _currentCustomerID.clear();
    _currentRole = 0;
    stackedScreens->setCurrentIndex(ScreenIndex::Login);
}

void MainWindow::switchLanguage()
{
    QSettings settings;
    QString current = settings.value("language", "en").toString();
    settings.setValue("language", current == "en" ? "ar" : "en");
    settings.sync();
    QProcess::startDetached(QApplication::applicationFilePath());
    QApplication::quit();
}

void MainWindow::toggleTheme()
{
    ThemeManager &tm = ThemeManager::instance();
    QString next = tm.currentTheme() == "light" ? "dark" : "light";
    tm.loadTheme(next);

    refreshAllStyles();

    // Update sidebar nav buttons
    for (QPushButton *btn : {btnDashboard, btnUsers, btnSuppliers, btnCustomers,
                             btnProducts, btnOrders, btnInventory, btnReports,
                             btnArticles, btnDeliveries,
                             btnSupDashboard, btnMyDeliveries, btnMyRewards,
                             btnMyProfile,
                             btnCustDashboard, btnCustProducts, btnCustOrders,
                             btnCustPoints, btnCustProfile})
    {
        QString bg = tm.currentTheme() == "dark" ? "#1E293B" : "transparent";
        QString txt = tm.currentTheme() == "dark" ? "#CBD5E1" : "#bdc3c7";
        QString hov = tm.currentTheme() == "dark" ? "#60A5FA" : "#1abc9c";
        btn->setStyleSheet(QString(R"(
            QPushButton {
                text-align: left;
                padding: 12px 15px;
                border: none;
                font-size: 15px;
                background-color: %1;
                color: %2;
                border-radius: 6px;
                margin: 2px 10px;
            }
            QPushButton:hover {
                background-color: #34495e;
                color: %3;
            }
        )").arg(bg, txt, hov));
    }

    // Update sidebar theme button label
    QPushButton *sbBtn = sidebarWidget->findChild<QPushButton*>("sidebarThemeBtn");
    if (sbBtn) {
        bool dark = tm.currentTheme() == "dark";
        QString icon = dark
            ? QString::fromUtf8("\xE2\x98\xBE ")
            : QString::fromUtf8("\xE2\x98\x80 ");
        sbBtn->setText(icon + tr(dark ? "Dark Mode" : "Light Mode"));
    }

    // Update login theme button if visible
    QPushButton *loginBtn = findChild<QPushButton*>("loginThemeBtn");
    if (loginBtn)
        updateLoginThemeButton(loginBtn);
}

void MainWindow::updateLoginThemeButton(QPushButton *btn)
{
    ThemeManager &tm = ThemeManager::instance();
    bool dark = tm.currentTheme() == "dark";
    QString icon = dark ? QString::fromUtf8("\xE2\x98\xBE ")    // ☾
                        : QString::fromUtf8("\xE2\x98\x80 ");   // ☀
    btn->setText(icon + tr(dark ? "Switch to Light" : "Switch to Dark"));
    QString fg = dark ? "#e2e8f0" : "#64748b";
    QString border = dark ? "#475569" : "#cbd5e1";
    QString accent = dark ? "#fbbf24" : "#2563eb";
    btn->setStyleSheet(
        QString("QPushButton {"
                "  background-color: transparent;"
                "  color: %1;"
                "  font-size: 13px;"
                "  font-weight: 600;"
                "  border: 1.5px solid %2;"
                "  border-radius: 20px;"
                "  padding: 8px;"
                "}"
                "QPushButton:hover {"
                "  border: 1.5px solid %3;"
                "  color: %3;"
                "  background-color: rgba(37,99,235,0.06);"
                "}")
        .arg(fg, border, accent)
    );
}

void MainWindow::refreshAllStyles()
{
    ThemeManager &tm = ThemeManager::instance();
    bool dark = tm.currentTheme() == "dark";

    // Color palette based on current theme
    QString cardBg   = dark ? "#1E293B" : "white";
    QString cardBdr  = dark ? "#334155" : "#dfe6e9";
    QString panelBg  = dark ? "#1E293B" : "white";
    QString titleClr = dark ? "#F8FAFC" : "#2c3e50";
    QString bodyClr  = dark ? "#CBD5E1" : "#7f8c8d";
    QString valueClr = dark ? "#F8FAFC" : "#2c3e50";
    QString frameBg  = dark ? "#1E293B" : "#f8f9fa";
    QString frameBdr = dark ? "#334155" : "#dfe6e9";

    // ========== DASHBOARD ==========
    if (lblDashSuppliers) {
        QString card = QString("background-color: %1; padding: 15px; border-radius: 8px; "
                               "font-size: 16px; font-weight: bold; border: 1px solid %2; color: %3;")
                       .arg(cardBg, cardBdr, valueClr);
        QString cardOrg = card + "color: #d35400;";
        QString cardGrn = card + "color: #27ae60;";
        QString cardBlu = card + "color: #2980b9;";

        lblDashSuppliers->setStyleSheet(card);
        lblDashCustomers->setStyleSheet(card);
        lblDashProducts->setStyleSheet(card);
        lblDashOrders->setStyleSheet(card);
        lblDashBones->setStyleSheet(cardOrg);
        lblDashRevenue->setStyleSheet(cardGrn);
        lblDashDeliveries->setStyleSheet(cardBlu);
    }

    if (btnQuickAddSupplier) {
        QString act = QString("background-color: %1; color: white; padding: 10px; "
                              "border-radius: 6px; font-weight: bold;")
                      .arg(dark ? "#2563EB" : "#34495e");
        btnQuickAddSupplier->setStyleSheet(act);
        btnQuickAddProduct->setStyleSheet(act);
        btnQuickAddArticle->setStyleSheet(act);
        btnQuickReports->setStyleSheet(act);
    }

    // ========== SUPPLIER DASHBOARD ==========
    if (lblSupDashTotalDeliveries) {
        QString card = QString("background-color: %1; padding: 20px; border-radius: 8px; "
                               "font-size: 16px; font-weight: bold; border: 1px solid %2;")
                       .arg(cardBg, cardBdr);
        lblSupDashTotalDeliveries->setStyleSheet(card + "color: #2980b9;");
        lblSupDashTotalBones->setStyleSheet(card + "color: #d35400;");
        lblSupDashPoints->setStyleSheet(card + "color: #27ae60;");
        lblSupDashRank->setStyleSheet(card + "color: #8e44ad;");
    }

    // ========== CUSTOMER DASHBOARD ==========
    QFrame *custCard = findChild<QFrame*>("custDashCard");
    if (custCard) {
        custCard->setStyleSheet(
            QString("QFrame { background-color: %1; border-radius: 8px; border: 1px solid %2; }")
            .arg(panelBg, cardBdr)
        );
    }

    // ========== MY REWARDS (cards) ==========
    QFrame *statsCard = findChild<QFrame*>("rewardsStatsCard");
    if (statsCard) {
        statsCard->setStyleSheet(
            QString("QFrame { background-color: %1; border-radius: 8px; border: 1px solid %2; }")
            .arg(cardBg, cardBdr)
        );
    }
    QFrame *certsCard = findChild<QFrame*>("rewardsCertsCard");
    if (certsCard) {
        certsCard->setStyleSheet(
            QString("QFrame { background-color: %1; border-radius: 8px; border: 1px solid %2; }")
            .arg(cardBg, cardBdr)
        );
    }
    // ========== MY PROFILE / CUSTOMER PROFILE ==========
    QFrame *profileCard = findChild<QFrame*>("profileCard");
    if (profileCard) {
        profileCard->setStyleSheet(
            QString("QFrame { background-color: %1; border-radius: 8px; border: 1px solid %2; }")
            .arg(cardBg, cardBdr)
        );
    }
    QFrame *custProfileCard = findChild<QFrame*>("customerProfileCard");
    if (custProfileCard) {
        custProfileCard->setStyleSheet(
            QString("QFrame { background-color: %1; border-radius: 8px; border: 1px solid %2; }")
            .arg(cardBg, cardBdr)
        );
    }

    // ========== PROFILE LABELS ==========
    if (lblProfileName) {
        lblProfileName->setStyleSheet(QString("color: %1; font-weight: bold; border: none; font-size: 16px;").arg(valueClr));
        lblProfileUsername->setStyleSheet(QString("color: %1; font-weight: bold; border: none; font-size: 16px;").arg(valueClr));
        lblProfilePhone->setStyleSheet(QString("color: %1; font-weight: bold; border: none; font-size: 16px;").arg(valueClr));
        lblProfileEmail->setStyleSheet(QString("color: %1; font-weight: bold; border: none; font-size: 16px;").arg(valueClr));
    }
    if (lblCustProfName) {
        lblCustProfName->setStyleSheet(QString("color: %1; border: none;").arg(valueClr));
        lblCustProfPhone->setStyleSheet(QString("color: %1; border: none;").arg(valueClr));
        lblCustProfEmail->setStyleSheet(QString("color: %1; border: none;").arg(valueClr));
        lblCustProfAddr->setStyleSheet(QString("color: %1; border: none;").arg(valueClr));
        lblCustProfType->setStyleSheet(QString("color: %1; border: none;").arg(valueClr));
    }

    // ========== CUSTOMER POINTS ==========
    QFrame *pointsCard = findChild<QFrame*>("pointsCard");
    if (pointsCard) {
        pointsCard->setStyleSheet(
            QString("QFrame { background-color: %1; border-radius: 8px; border: 1px solid %2; }")
            .arg(cardBg, cardBdr)
        );
    }
}

// ==========================================
// 2. DASHBOARD SCREEN
// ==========================================
void MainWindow::setupDashboardScreen()
{
    QWidget *w = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(w); // Split left and right

    // ==========================================
    // LEFT PANEL: Metrics & Quick Actions
    // ==========================================
    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);

    QLabel *title = new QLabel(tr("Admin Control Center"), this);
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");

    // --- Metrics Grid ---
    QGridLayout *metricsGrid = new QGridLayout();
    lblDashSuppliers = new QLabel(tr("Suppliers: 0"));
    lblDashCustomers = new QLabel(tr("Customers: 0"));
    lblDashProducts  = new QLabel(tr("Products: 0"));
    lblDashOrders    = new QLabel(tr("Orders: 0"));
    lblDashBones     = new QLabel(tr("Collected Bones: 0 kg"));
    lblDashRevenue   = new QLabel(tr("Total Revenue: $0.00"));
    lblDashDeliveries = new QLabel(tr("Total Deliveries: 0"));

    // Style the metrics to look like cards
    QString cardStyle = "background-color: white; padding: 15px; border-radius: 8px; font-size: 16px; font-weight: bold; border: 1px solid #dfe6e9;";
    lblDashSuppliers->setStyleSheet(cardStyle);
    lblDashCustomers->setStyleSheet(cardStyle);
    lblDashProducts->setStyleSheet(cardStyle);
    lblDashOrders->setStyleSheet(cardStyle);
    lblDashBones->setStyleSheet(cardStyle + "color: #d35400;"); // Orange accent
    lblDashRevenue->setStyleSheet(cardStyle + "color: #27ae60;"); // Green accent
    lblDashDeliveries->setStyleSheet(cardStyle + "color: #2980b9;");

    metricsGrid->addWidget(lblDashSuppliers, 0, 0);
    metricsGrid->addWidget(lblDashCustomers, 0, 1);
    metricsGrid->addWidget(lblDashProducts, 1, 0);
    metricsGrid->addWidget(lblDashOrders, 1, 1);
    metricsGrid->addWidget(lblDashBones, 2, 0);
    metricsGrid->addWidget(lblDashRevenue, 2, 1);
    metricsGrid->addWidget(lblDashDeliveries, 3, 0);

    // --- Quick Actions ---
    QLabel *lblActions = new QLabel(tr("Quick Actions"), this);
    lblActions->setStyleSheet("font-size: 18px; font-weight: bold; margin-top: 20px;");

    QHBoxLayout *actionsLayout = new QHBoxLayout();
    btnQuickAddSupplier = new QPushButton(tr("+ Add Supplier"));
    btnQuickAddProduct  = new QPushButton(tr("+ Add Product"));
    btnQuickAddArticle  = new QPushButton(tr("+ Create Article"));
    btnQuickReports     = new QPushButton(tr("View Reports"));

    QString actionStyle = "background-color: #34495e; color: white; padding: 10px; border-radius: 6px; font-weight: bold;";
    btnQuickAddSupplier->setStyleSheet(actionStyle);
    btnQuickAddProduct->setStyleSheet(actionStyle);
    btnQuickAddArticle->setStyleSheet(actionStyle);
    btnQuickReports->setStyleSheet(actionStyle);

    connect(btnQuickAddSupplier, &QPushButton::clicked, [this]() {
    SupplierDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
        refreshDashboard(); // Instantly update the metrics if saved!
    }
    });

    connect(btnQuickAddProduct, &QPushButton::clicked, [this]() {
    ProductDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
        refreshDashboard(); // Instantly update metrics and Low Stock Alerts table!
    }
});

connect(btnQuickAddArticle, &QPushButton::clicked, [this]() {
    ArticleDialog dialog(this);
    dialog.exec();
    // Articles don't directly update the dashboard metrics currently,
    // but we exec() it here so the quick action works!
});

    actionsLayout->addWidget(btnQuickAddSupplier);
    actionsLayout->addWidget(btnQuickAddProduct);
    actionsLayout->addWidget(btnQuickAddArticle);
    actionsLayout->addWidget(btnQuickReports);

    leftLayout->addWidget(title);
    leftLayout->addLayout(metricsGrid);
    leftLayout->addWidget(lblActions);
    leftLayout->addLayout(actionsLayout);
    leftLayout->addStretch();

    // ==========================================
    // RIGHT PANEL: Low Stock Alerts
    // ==========================================
    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightPanel->setFixedWidth(400); // Keep it neatly to the side

    QLabel *alertTitle = new QLabel(tr("⚠️ Low Stock Alerts"), this);
    alertTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #c0392b; margin-bottom: 10px;");

    tableDashLowStock = createStandardTable({tr("Product"), tr("Stock"), tr("Min")});
    tableDashLowStock->verticalHeader()->setVisible(false);

    rightLayout->addWidget(alertTitle);
    rightLayout->addWidget(tableDashLowStock);

    // Combine Panels
    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(rightPanel);

    // Connect Quick Action Buttons to their respective screens
    connect(btnQuickReports, &QPushButton::clicked, [this]() { navigateToScreen(ScreenIndex::Reports); });
    // Note: The "Add" buttons will be connected to the Dialog pop-ups once we build them!

    stackedScreens->insertWidget(ScreenIndex::Dashboard, w);
}

void MainWindow::refreshDashboard()
{
    // 1. Update the Metrics Grid using clsReport
    lblDashSuppliers->setText(tr("Suppliers: %1").arg(clsReport::TotalSuppliers()));
    lblDashCustomers->setText(tr("Customers: %1").arg(clsReport::TotalCustomers()));
    lblDashProducts->setText(tr("Products: %1").arg(clsReport::TotalProducts()));
    lblDashOrders->setText(tr("Orders: %1").arg(clsReport::TotalOrders()));

    lblDashBones->setText(tr("Collected Bones: %1 kg").arg(QString::number(clsReport::TotalCollectedBones(), 'f', 1)));
    lblDashRevenue->setText(tr("Total Revenue: $%1").arg(QString::number(clsReport::TotalRevenue(), 'f', 2)));
    lblDashDeliveries->setText(tr("Total Deliveries: %1").arg(clsDelivery::GetDeliveriesList().size()));

    // 2. Populate the Low Stock Alert Table
    tableDashLowStock->setRowCount(0);
    vector<clsProduct> vLowStock = clsInventory::GetLowStockAlerts();

    for (size_t i = 0; i < vLowStock.size(); i++)
    {
        tableDashLowStock->insertRow(i);
        tableDashLowStock->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vLowStock[i].Name())));

        QTableWidgetItem *stockItem = new QTableWidgetItem(QString::number(vLowStock[i].StockQuantity()));
        stockItem->setForeground(QBrush(Qt::red)); // Visually flag the low number
        stockItem->setFont(QFont("Arial", 10, QFont::Bold));

        tableDashLowStock->setItem(i, 1, stockItem);
        tableDashLowStock->setItem(i, 2, new QTableWidgetItem(QString::number(vLowStock[i].MinimumStock())));
    }
}

// ==========================================
// 3. USERS SCREEN
// ==========================================
void MainWindow::setupUsersScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);

    QLabel *title = new QLabel(tr("Users Management"), this);
    title->setStyleSheet("font-size: 20px; font-weight: bold;");

    tableUsers = createStandardTable({tr("ID"), tr("Username"), tr("Name"), tr("Role"), tr("Status")});

    // Setup Action Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnAdd = new QPushButton(tr("Add New User"), this);
    QPushButton *btnEdit = new QPushButton(tr("Edit User"), this);
    QPushButton *btnDel = new QPushButton(tr("Delete User"), this);
    btnDel->setProperty("role", "danger");

    // Styling the buttons
    btnAdd->setStyleSheet("background-color: #27ae60; color: white; padding: 8px; width: 130px;");
    btnEdit->setStyleSheet("background-color: #f39c12; color: white; padding: 8px; width: 130px;"); // Orange for Edit
    btnDel->setStyleSheet("background-color: #e74c3c; color: white; padding: 8px; width: 130px;");

    btnLayout->addStretch();
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnEdit); // Add to layout
    btnLayout->addWidget(btnDel);

    l->addWidget(title);
    l->addWidget(tableUsers);
    l->addLayout(btnLayout);

    // Connect Delete
    connect(btnDel, &QPushButton::clicked, this, &MainWindow::deleteSelectedUser);

    // Connect Add
    connect(btnAdd, &QPushButton::clicked, [this]() {
        UserDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
            loadUsersData();
        }
    });

    // NEW: Connect Edit
    connect(btnEdit, &QPushButton::clicked, [this]() {
        int row = tableUsers->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, tr("Select User"), tr("Please select a user to edit from the table first."));
            return;
        }

        // Grab the User ID from the selected row
        QString id = tableUsers->item(row, 0)->text();

        UserDialog dialog(this);
        dialog.loadUserForEdit(id.toStdString()); // Tell the dialog to load this user!

        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
            loadUsersData(); // Instantly refresh the table with the updated data
        }
    });

    stackedScreens->insertWidget(ScreenIndex::Users, w);
}

void MainWindow::loadUsersData()
{
    tableUsers->setRowCount(0);
    vector<clsUser> vList = clsUser::GetUsersList();
    for (size_t i = 0; i < vList.size(); i++) {
        tableUsers->insertRow(i);
        tableUsers->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vList[i].UserID())));
        tableUsers->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(vList[i].Username())));
        tableUsers->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(vList[i].FullName())));
        tableUsers->setItem(i, 3, new QTableWidgetItem(QString::number(vList[i].Role())));
        tableUsers->setItem(i, 4, new QTableWidgetItem(vList[i].IsActive() ? tr("Active") : tr("Inactive")));
    }
}

void MainWindow::deleteSelectedUser()
{
    int row = tableUsers->currentRow();
    if (row < 0) return;
    QString id = tableUsers->item(row, 0)->text();
    if (QMessageBox::question(this, tr("Confirm"), tr("Delete User %1?").arg(id)) == QMessageBox::Yes) {
        clsUser user = clsUser::Find(id.toStdString());
        if (user.IsEmpty()) return;
        // Also delete linked supplier/customer to avoid orphan records
        if (user.Role() == clsUser::enRole::Supplier && !user.SupplierID().empty()) {
            clsSupplier::Find(user.SupplierID()).Delete();
        }
        if (user.Delete()) loadUsersData();
    }
}

// ==========================================
// 4. SUPPLIERS SCREEN
// ==========================================
void MainWindow::setupSuppliersScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);

    QLabel *title = new QLabel(tr("Suppliers Management"), this);
    title->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 10px;");

    tableSuppliers = createStandardTable({tr("ID"), tr("Name"), tr("Phone"), tr("Bone Type"), tr("Points")});

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnAdd = new QPushButton(tr("Add New Supplier"), this);
    QPushButton *btnEdit = new QPushButton(tr("Edit Supplier"), this);
    QPushButton *btnDel = new QPushButton(tr("Delete Supplier"), this);
    btnDel->setProperty("role", "danger");

    btnAdd->setStyleSheet("background-color: #27ae60; color: white; padding: 8px; width: 130px;");
    btnEdit->setStyleSheet("background-color: #f39c12; color: white; padding: 8px; width: 130px;");
    btnDel->setStyleSheet("background-color: #e74c3c; color: white; padding: 8px; width: 130px;");

    btnLayout->addStretch();
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnEdit);
    btnLayout->addWidget(btnDel);

    l->addWidget(title);
    l->addWidget(tableSuppliers);
    l->addLayout(btnLayout);

    // --- Connections ---
    connect(btnDel, &QPushButton::clicked, this, &MainWindow::deleteSelectedSupplier);

    connect(btnAdd, &QPushButton::clicked, [this]() {
        SupplierDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
            loadSuppliersData();
        }
    });

    connect(btnEdit, &QPushButton::clicked, [this]() {
        int row = tableSuppliers->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, tr("Selection Required"), tr("Please select a supplier to edit."));
            return;
        }
        QString id = tableSuppliers->item(row, 0)->text();
        SupplierDialog dialog(this);
        dialog.loadSupplierForEdit(id.toStdString());
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
            loadSuppliersData();
        }
    });

    stackedScreens->insertWidget(ScreenIndex::Suppliers, w);
}

void MainWindow::loadSuppliersData()
{
    tableSuppliers->setRowCount(0);
    vector<clsSupplier> vList = clsSupplier::GetSuppliersList();
    for (size_t i = 0; i < vList.size(); i++) {
        tableSuppliers->insertRow(i);
        tableSuppliers->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vList[i].SupplierID())));
        tableSuppliers->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(vList[i].FirstName())));
        tableSuppliers->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(vList[i].PhoneNumber())));
        tableSuppliers->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(vList[i].BoneType())));
        tableSuppliers->setItem(i, 4, new QTableWidgetItem(QString::number(vList[i].Points())));
    }
}

void MainWindow::deleteSelectedSupplier()
{
    int row = tableSuppliers->currentRow();
    if (row < 0) return;
    QString id = tableSuppliers->item(row, 0)->text();
    if (QMessageBox::question(this, tr("Confirm"), tr("Delete Supplier %1?").arg(id)) == QMessageBox::Yes) {
        // Also delete linked user to avoid orphans
        vector<clsUser> users = clsUser::GetUsersList();
        for (clsUser& u : users) {
            if (u.SupplierID() == id.toStdString()) {
                u.Delete();
                break;
            }
        }
        if (clsSupplier::Find(id.toStdString()).Delete()) loadSuppliersData();
    }
}

// ==========================================
// 5. CUSTOMERS SCREEN
// ==========================================
void MainWindow::setupCustomersScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);

    QLabel *title = new QLabel(tr("Customers Management"), this);
    title->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 10px;");

    tableCustomers = createStandardTable({tr("ID"), tr("Name"), tr("Phone"), tr("Address"), tr("Type")});

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnAdd = new QPushButton(tr("Add Customer"), this);
    QPushButton *btnEdit = new QPushButton(tr("Edit Customer"), this);
    QPushButton *btnDel = new QPushButton(tr("Delete Customer"), this);
    btnDel->setProperty("role", "danger");

    btnAdd->setStyleSheet("background-color: #27ae60; color: white; padding: 8px; width: 130px;");
    btnEdit->setStyleSheet("background-color: #f39c12; color: white; padding: 8px; width: 130px;");
    btnDel->setStyleSheet("background-color: #e74c3c; color: white; padding: 8px; width: 130px;");

    btnLayout->addStretch();
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnEdit);
    btnLayout->addWidget(btnDel);

    l->addWidget(title);
    l->addWidget(tableCustomers);
    l->addLayout(btnLayout);

    connect(btnDel, &QPushButton::clicked, this, &MainWindow::deleteSelectedCustomer);

    connect(btnAdd, &QPushButton::clicked, [this]() {
        CustomerDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) loadCustomersData();
    });

    connect(btnEdit, &QPushButton::clicked, [this]() {
        int row = tableCustomers->currentRow();
        if (row < 0) return;
        QString id = tableCustomers->item(row, 0)->text();
        CustomerDialog dialog(this);
        dialog.loadCustomerForEdit(id.toStdString());
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) loadCustomersData();
    });

    stackedScreens->insertWidget(ScreenIndex::Customers, w);
}

void MainWindow::loadCustomersData()
{
    tableCustomers->setRowCount(0);
    vector<clsCustomer> vList = clsCustomer::GetCustomersList();
    for (size_t i = 0; i < vList.size(); i++) {
        tableCustomers->insertRow(i);
        tableCustomers->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vList[i].CustomerID())));
        tableCustomers->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(vList[i].FirstName())));
        tableCustomers->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(vList[i].PhoneNumber())));
        tableCustomers->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(vList[i].Address())));
        tableCustomers->setItem(i, 4, new QTableWidgetItem(QString::number(vList[i].CustomerType())));
    }
}

void MainWindow::deleteSelectedCustomer()
{
    int row = tableCustomers->currentRow();
    if (row < 0) return;
    QString id = tableCustomers->item(row, 0)->text();
    if (QMessageBox::question(this, tr("Confirm"), tr("Delete Customer %1?").arg(id)) == QMessageBox::Yes) {
        // Also delete linked user to avoid orphans
        vector<clsUser> users = clsUser::GetUsersList();
        for (clsUser& u : users) {
            if (u.CustomerID() == id.toStdString()) {
                u.Delete();
                break;
            }
        }
        if (clsCustomer::Find(id.toStdString()).Delete()) loadCustomersData();
    }
}

// ==========================================
// 6. PRODUCTS SCREEN
// ==========================================
void MainWindow::setupProductsScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);

    QLabel *title = new QLabel(tr("Products Catalog"), this);
    title->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 10px;");

    tableProducts = createStandardTable({tr("ID"), tr("Name"), tr("Category"), tr("Price"), tr("Stock")});

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnAdd = new QPushButton(tr("Add New Product"), this);
    QPushButton *btnEdit = new QPushButton(tr("Edit Product"), this);
    QPushButton *btnDel = new QPushButton(tr("Delete Product"), this);
    btnDel->setProperty("role", "danger");

    btnAdd->setStyleSheet("background-color: #27ae60; color: white; padding: 8px; width: 130px;");
    btnEdit->setStyleSheet("background-color: #f39c12; color: white; padding: 8px; width: 130px;");
    btnDel->setStyleSheet("background-color: #e74c3c; color: white; padding: 8px; width: 130px;");

    btnLayout->addStretch();
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnEdit);
    btnLayout->addWidget(btnDel);

    l->addWidget(title);
    l->addWidget(tableProducts);
    l->addLayout(btnLayout);

    // --- Connections ---
    connect(btnDel, &QPushButton::clicked, this, &MainWindow::deleteSelectedProduct);

    connect(btnAdd, &QPushButton::clicked, [this]() {
        ProductDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
            loadProductsData();
        }
    });

    connect(btnEdit, &QPushButton::clicked, [this]() {
        int row = tableProducts->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, tr("Selection Required"), tr("Please select a product to edit."));
            return;
        }
        QString id = tableProducts->item(row, 0)->text();
        ProductDialog dialog(this);
        dialog.loadProductForEdit(id.toStdString());
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
            loadProductsData();
        }
    });

    stackedScreens->insertWidget(ScreenIndex::Products, w);
}

void MainWindow::loadProductsData()
{
    tableProducts->setRowCount(0);
    vector<clsProduct> vList = clsProduct::GetProductsList();
    for (size_t i = 0; i < vList.size(); i++) {
        tableProducts->insertRow(i);
        tableProducts->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vList[i].ProductID())));
        tableProducts->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(vList[i].Name())));
        tableProducts->setItem(i, 2, new QTableWidgetItem(QString::number(vList[i].Category())));
        tableProducts->setItem(i, 3, new QTableWidgetItem("$" + QString::number(vList[i].Price(), 'f', 2)));
        tableProducts->setItem(i, 4, new QTableWidgetItem(QString::number(vList[i].StockQuantity())));
    }
}

void MainWindow::deleteSelectedProduct()
{
    int row = tableProducts->currentRow();
    if (row < 0) return;
    if (clsProduct::Find(tableProducts->item(row, 0)->text().toStdString()).Delete()) loadProductsData();
}

// ==========================================
// 7. ORDERS SCREEN
// ==========================================
void MainWindow::setupOrdersScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);

    QLabel *title = new QLabel(tr("Order Processing"), this);
    title->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 10px;");

    tableOrders = createStandardTable({tr("Order ID"), tr("Customer ID"), tr("Product ID"), tr("Total Price"), tr("Status Code")});

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnUpdateStatus = new QPushButton(tr("Update Status"), this);
    QPushButton *btnCancel = new QPushButton(tr("Cancel Order"), this);
    btnCancel->setProperty("role", "danger");

    btnUpdateStatus->setStyleSheet("background-color: #2980b9; color: white; padding: 8px; width: 130px;");
    btnCancel->setStyleSheet("background-color: #e74c3c; color: white; padding: 8px; width: 130px;");

    btnLayout->addStretch();
    btnLayout->addWidget(btnUpdateStatus);
    btnLayout->addWidget(btnCancel);

    l->addWidget(title);
    l->addWidget(tableOrders);
    l->addLayout(btnLayout);

    connect(btnCancel, &QPushButton::clicked, this, &MainWindow::cancelSelectedOrder);

  // --- NEW: Custom Order Status Update Logic ---
connect(btnUpdateStatus, &QPushButton::clicked, [this]() {
    int row = tableOrders->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, tr("Select Order"), tr("Please select an order to update."));
        return;
    }

    QString orderID = tableOrders->item(row, 0)->text();
    clsOrder order = clsOrder::Find(orderID.toStdString());

    if (!order.IsEmpty()) {
        // Open our custom colored dialog
        OrderStatusDialog dialog(order.Status(), this);

        if (dialog.exec() == QDialog::Accepted) {
            int newStatus = dialog.getSelectedStatus();
            int oldStatus = order.Status();

            // Validate state transitions
            bool valid = true;
            if (oldStatus == clsOrder::enStatus::Pending)
                valid = (newStatus == clsOrder::enStatus::Confirmed ||
                         newStatus == clsOrder::enStatus::Processing ||
                         newStatus == clsOrder::enStatus::Cancelled);
            else if (oldStatus == clsOrder::enStatus::Confirmed)
                valid = (newStatus == clsOrder::enStatus::Processing ||
                         newStatus == clsOrder::enStatus::Cancelled);
            else if (oldStatus == clsOrder::enStatus::Processing)
                valid = (newStatus == clsOrder::enStatus::Delivered);
            else if (oldStatus == clsOrder::enStatus::Cancelled)
                valid = false;
            else if (oldStatus == clsOrder::enStatus::Delivered)
                valid = (newStatus == clsOrder::enStatus::Delivered && !order.PointsAwarded());

            if (!valid) {
                QMessageBox::warning(this, tr("Invalid Transition"),
                    tr("The selected status change is not allowed.\n"
                    "Valid flow: Pending \xe2\x86\x92 Confirmed \xe2\x86\x92 Processing \xe2\x86\x92 Delivered"));
                return;
            }

            order.SetStatus((clsOrder::enStatus)newStatus);

            // Credit 100 customer points when order is Delivered (once only)
            if (newStatus == clsOrder::enStatus::Delivered && !order.PointsAwarded()) {
                clsCustomer cust = clsCustomer::Find(order.CustomerID());
                if (!cust.IsEmpty()) {
                    cust.SetPoints(cust.Points() + 100);
                    cust.Save();
                    order.SetPointsAwarded(true);
                }
            }

            order.Save();

            loadOrdersData();
            refreshDashboard();
        }
    }
});
    stackedScreens->insertWidget(ScreenIndex::Orders, w);
}

void MainWindow::loadOrdersData()
{
    tableOrders->setRowCount(0);
    vector<clsOrder> vList = clsOrder::GetOrdersList();

    for (size_t i = 0; i < vList.size(); i++) {
        tableOrders->insertRow(i);
        tableOrders->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vList[i].OrderID())));
        tableOrders->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(vList[i].CustomerID())));
        tableOrders->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(vList[i].ProductID())));
        tableOrders->setItem(i, 3, new QTableWidgetItem("$" + QString::number(vList[i].TotalPrice(), 'f', 2)));

        // --- NEW: Color Coded Status Logic ---
        QTableWidgetItem *statusItem = new QTableWidgetItem();
        statusItem->setFont(QFont("Arial", 10, QFont::Bold)); // Make it bold

        int status = vList[i].Status();

        if (status == clsOrder::enStatus::Pending) {
            statusItem->setText(tr("Pending"));
            statusItem->setForeground(QBrush(QColor("#f39c12")));
        }
        else if (status == clsOrder::enStatus::Confirmed) {
            statusItem->setText(tr("Confirmed"));
            statusItem->setForeground(QBrush(QColor("#3498db")));
        }
        else if (status == clsOrder::enStatus::Processing) {
            statusItem->setText(tr("Processing"));
            statusItem->setForeground(QBrush(QColor("#9b59b6")));
        }
        else if (status == clsOrder::enStatus::Delivered) {
            statusItem->setText(tr("Delivered"));
            statusItem->setForeground(QBrush(QColor("#27ae60")));
        }
        else if (status == clsOrder::enStatus::Cancelled) {
            statusItem->setText(tr("Cancelled"));
            statusItem->setForeground(QBrush(QColor("#e74c3c")));
        }

        // Insert the colored item into column 4
        tableOrders->setItem(i, 4, statusItem);
    }
}

void MainWindow::cancelSelectedOrder()
{
    int row = tableOrders->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, tr("Select Order"), tr("Please select an order to cancel."));
        return;
    }

    string orderID = tableOrders->item(row, 0)->text().toStdString();
    string productID = tableOrders->item(row, 2)->text().toStdString();

    clsOrder order = clsOrder::Find(orderID);
    if (order.IsEmpty()) return;

    if (!order.Cancel()) {
        QMessageBox::warning(this, tr("Cannot Cancel"),
            tr("Only Pending or Confirmed orders may be cancelled."));
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Confirm Cancellation"),
                                  tr("Are you sure you want to cancel this order? The items will be returned to inventory."),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
        order.SetStatus(clsOrder::enStatus::Pending);
        order.Save();
        loadOrdersData();
        return;
    }

    // Restore Inventory
    clsProduct product = clsProduct::Find(productID);
    if (!product.IsEmpty()) {
        product.SetStockQuantity(product.StockQuantity() + order.Quantity());
        product.Save();
    }

    QMessageBox::information(this, tr("Order Cancelled"), tr("The order has been cancelled and the items have been returned to inventory."));
    loadOrdersData();
    refreshDashboard();
}

// ==========================================
// 8. INVENTORY ALERTS SCREEN
// ==========================================
void MainWindow::setupInventoryScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);

    // Header Layout with Title and Filter
    QHBoxLayout *headerLayout = new QHBoxLayout();

    QLabel *title = new QLabel(tr("Inventory Control"), this);
    title->setStyleSheet("font-weight: bold; font-size: 20px;");

    QLabel *lblFilter = new QLabel(tr("Filter View:"), this);
    lblFilter->setStyleSheet("font-weight: bold;");

    cmbInventoryFilter = new QComboBox(this);
    cmbInventoryFilter->addItem(tr("All Current Stock"));
    cmbInventoryFilter->addItem(tr("Low Stock Alerts"));
    cmbInventoryFilter->addItem(tr("Out Of Stock"));
    cmbInventoryFilter->setStyleSheet("padding: 5px; border-radius: 4px; border: 1px solid #bdc3c7; width: 150px;color: #2980b9; font-weight: bold;");

    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(lblFilter);
    headerLayout->addWidget(cmbInventoryFilter);

    // Add the new "Status" column to the table
    tableInventory = createStandardTable({tr("Product ID"), tr("Name"), tr("Quantity"), tr("Minimum"), tr("Status")});

    l->addLayout(headerLayout);
    l->addWidget(tableInventory);

    // Re-load the data whenever the Admin changes the filter dropdown!
    connect(cmbInventoryFilter, &QComboBox::currentIndexChanged, [this]() {
        loadInventoryData();
    });

    stackedScreens->insertWidget(ScreenIndex::Inventory, w);
}

void MainWindow::loadInventoryData()
{
    tableInventory->setRowCount(0);

    // Get all products to evaluate
    vector<clsProduct> vProducts = clsProduct::GetProductsList();
    int currentFilter = cmbInventoryFilter->currentIndex();
    // 0 = All Stock, 1 = Low Stock, 2 = Out Of Stock

    int row = 0; // Keep track of the row index for insertion

    for (size_t i = 0; i < vProducts.size(); i++)
    {
        int stock = vProducts[i].StockQuantity();
        int minStock = vProducts[i].MinimumStock();

        // Determine the Status
        bool isOutOfStock = (stock == 0);
        bool isLowStock = (stock > 0 && stock <= minStock);

        // Apply the Admin's Filter
        if (currentFilter == 1 && !isLowStock && !isOutOfStock) continue; // Skip healthy stock if viewing Low
        if (currentFilter == 2 && !isOutOfStock) continue; // Skip everything with >0 stock if viewing Out

        // If it passes the filter, add it to the table!
        tableInventory->insertRow(row);
        tableInventory->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(vProducts[i].ProductID())));
        tableInventory->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(vProducts[i].Name())));
        tableInventory->setItem(row, 2, new QTableWidgetItem(QString::number(stock)));
        tableInventory->setItem(row, 3, new QTableWidgetItem(QString::number(minStock)));

        // Generate the visual Status Badge
        QTableWidgetItem *statusItem = new QTableWidgetItem();
        statusItem->setFont(QFont("Arial", 10, QFont::Bold));

        if (isOutOfStock) {
            statusItem->setText(tr("Out of Stock"));
            statusItem->setForeground(QBrush(QColor("#c0392b")));
        }
        else if (isLowStock) {
            statusItem->setText(tr("Low Stock"));
            statusItem->setForeground(QBrush(QColor("#e67e22")));
        }
        else {
            statusItem->setText(tr("Healthy"));
            statusItem->setForeground(QBrush(QColor("#27ae60")));
        }

        tableInventory->setItem(row, 4, statusItem);
        row++; // Increment our UI row index
    }
}
// ==========================================
// 9. REPORTS SCREEN
// ==========================================
void MainWindow::setupReportsScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(w);

    // Header Layout with Title and Refresh Button
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *title = new QLabel(tr("System Reports & Analytics"), this);
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50;");

    QPushButton *btnRefreshReports = new QPushButton(tr(" Refresh Data"), this);
    btnRefreshReports->setStyleSheet("background-color: #34495e; color: white; padding: 8px 15px; border-radius: 4px; font-weight: bold;");

    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(btnRefreshReports);

    // Initialize the Tab Widget
    tabReports = new QTabWidget(this);

    // --- Tab 1: Sales & Orders Report ---
    QWidget *tabSales = new QWidget();
    QVBoxLayout *lSales = new QVBoxLayout(tabSales);
    tableRepSales = createStandardTable({tr("Order ID"), tr("Customer"), tr("Product"), tr("Total Revenue"), tr("Status")});
    lSales->addWidget(tableRepSales);
    tabReports->addTab(tabSales, tr("Sales Reports"));

    // --- Tab 2: Inventory Valuation ---
    QWidget *tabInv = new QWidget();
    QVBoxLayout *lInv = new QVBoxLayout(tabInv);
    tableRepInventory = createStandardTable({tr("Product"), tr("Category"), tr("Stock Level"), tr("Min Alert"), tr("Status")});
    lInv->addWidget(tableRepInventory);
    tabReports->addTab(tabInv, tr("Inventory Reports"));

    // --- Tab 3: Supplier Performance ---
    QWidget *tabSup = new QWidget();
    QVBoxLayout *lSup = new QVBoxLayout(tabSup);
    tableRepSuppliers = createStandardTable({tr("Supplier Name"), tr("Type"), tr("Avg Weekly (kg)"), tr("Points Issued")});
    lSup->addWidget(tableRepSuppliers);
    tabReports->addTab(tabSup, tr("Supplier Reports"));

    // --- Tab 4: Customer Insights ---
    QWidget *tabCust = new QWidget();
    QVBoxLayout *lCust = new QVBoxLayout(tabCust);
    tableRepCustomers = createStandardTable({tr("Customer Name"), tr("Type"), tr("Contact"), tr("Points Balance")});
    lCust->addWidget(tableRepCustomers);
    tabReports->addTab(tabCust, tr("Customer Reports"));

    // --- Tab 5: Deliveries Report (NEW) ---
    QWidget *tabDel = new QWidget();
    QVBoxLayout *lDel = new QVBoxLayout(tabDel);
    tableRepDeliveries = createStandardTable({tr("Delivery ID"), tr("Supplier"), tr("Date"), tr("Bone Type"), tr("Quantity (kg)"), tr("Status")});
    lDel->addWidget(tableRepDeliveries);
    tabReports->addTab(tabDel, tr("Delivery Reports"));

    // Assemble the main layout
    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(tabReports);

    // Connect the refresh button to a generic load function
    connect(btnRefreshReports, &QPushButton::clicked, this, &MainWindow::loadAllReportsData);

    stackedScreens->insertWidget(ScreenIndex::Reports, w);
}

void MainWindow::loadAllReportsData()
{
    // 1. Load Sales Report
    tableRepSales->setRowCount(0);
    vector<clsOrder> vOrders = clsOrder::GetOrdersList();
    for (size_t i = 0; i < vOrders.size(); i++) {
        tableRepSales->insertRow(i);
        tableRepSales->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vOrders[i].OrderID())));
        tableRepSales->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(vOrders[i].CustomerID())));
        tableRepSales->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(vOrders[i].ProductID())));
        tableRepSales->setItem(i, 3, new QTableWidgetItem("$" + QString::number(vOrders[i].TotalPrice(), 'f', 2)));

        // --- COLOR CODED STATUS ---
        QTableWidgetItem *statusItem = new QTableWidgetItem();
        statusItem->setFont(QFont("Arial", 10, QFont::Bold));

        if (vOrders[i].Status() == clsOrder::enStatus::Delivered) {
            statusItem->setText(tr("Delivered"));
            statusItem->setForeground(QBrush(QColor("#27ae60")));
        } else if (vOrders[i].Status() == clsOrder::enStatus::Cancelled) {
            statusItem->setText(tr("Cancelled"));
            statusItem->setForeground(QBrush(QColor("#e74c3c")));
        } else {
            statusItem->setText(tr("In Progress"));
            statusItem->setForeground(QBrush(QColor("#f39c12")));
        }
        tableRepSales->setItem(i, 4, statusItem);
    }

    // 2. Load Inventory Report
    tableRepInventory->setRowCount(0);
    vector<clsProduct> vProducts = clsProduct::GetProductsList();
    for (size_t i = 0; i < vProducts.size(); i++) {
        tableRepInventory->insertRow(i);
        tableRepInventory->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vProducts[i].Name())));
        tableRepInventory->setItem(i, 1, new QTableWidgetItem(QString::number(vProducts[i].Category())));
        tableRepInventory->setItem(i, 2, new QTableWidgetItem(QString::number(vProducts[i].StockQuantity())));
        tableRepInventory->setItem(i, 3, new QTableWidgetItem(QString::number(vProducts[i].MinimumStock())));

        // --- COLOR CODED STATUS ---
        QTableWidgetItem *stockStatusItem = new QTableWidgetItem();
        stockStatusItem->setFont(QFont("Arial", 10, QFont::Bold));

        int stock = vProducts[i].StockQuantity();
        int minStock = vProducts[i].MinimumStock();

        if (stock == 0) {
            stockStatusItem->setText(tr("Out of Stock"));
            stockStatusItem->setForeground(QBrush(QColor("#c0392b")));
        } else if (stock <= minStock) {
            stockStatusItem->setText(tr("Needs Restock"));
            stockStatusItem->setForeground(QBrush(QColor("#e67e22")));
        } else {
            stockStatusItem->setText(tr("Healthy"));
            stockStatusItem->setForeground(QBrush(QColor("#27ae60")));
        }
        tableRepInventory->setItem(i, 4, stockStatusItem);
    }

    // 3. Load Supplier Report
    tableRepSuppliers->setRowCount(0);
    vector<clsSupplier> vSuppliers = clsSupplier::GetSuppliersList();
    for (size_t i = 0; i < vSuppliers.size(); i++) {
        tableRepSuppliers->insertRow(i);
        tableRepSuppliers->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vSuppliers[i].FirstName())));
        tableRepSuppliers->setItem(i, 1, new QTableWidgetItem(QString::number(vSuppliers[i].SupplierType())));
        tableRepSuppliers->setItem(i, 2, new QTableWidgetItem(QString::number(vSuppliers[i].WeeklyQuantity())));
        tableRepSuppliers->setItem(i, 3, new QTableWidgetItem(QString::number(vSuppliers[i].Points())));
    }

    // 4. Load Customer Report
    tableRepCustomers->setRowCount(0);
    vector<clsCustomer> vCustomers = clsCustomer::GetCustomersList();
    for (size_t i = 0; i < vCustomers.size(); i++) {
        tableRepCustomers->insertRow(i);
        tableRepCustomers->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vCustomers[i].FirstName())));
        tableRepCustomers->setItem(i, 1, new QTableWidgetItem(QString::number(vCustomers[i].CustomerType())));
        tableRepCustomers->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(vCustomers[i].PhoneNumber())));
        tableRepCustomers->setItem(i, 3, new QTableWidgetItem("0"));
    }

    // 5. Load Deliveries Report
    tableRepDeliveries->setRowCount(0);
    vector<clsDelivery> vReportDeliveries = clsDelivery::GetDeliveriesList();
    for (size_t i = 0; i < vReportDeliveries.size(); i++) {
        tableRepDeliveries->insertRow(i);
        tableRepDeliveries->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vReportDeliveries[i].DeliveryID())));
        // Show supplier name
        clsSupplier s = clsSupplier::Find(vReportDeliveries[i].SupplierID());
        QString supName = s.IsEmpty()
            ? QString::fromStdString(vReportDeliveries[i].SupplierID())
            : QString::fromStdString(s.FirstName() + " (" + s.SupplierID() + ")");
        tableRepDeliveries->setItem(i, 1, new QTableWidgetItem(supName));
        tableRepDeliveries->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(vReportDeliveries[i].Date())));
        tableRepDeliveries->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(vReportDeliveries[i].ProductID())));
        tableRepDeliveries->setItem(i, 4, new QTableWidgetItem(QString::number(vReportDeliveries[i].Quantity())));

        // --- COLOR CODED STATUS ---
        QTableWidgetItem *statusItem = new QTableWidgetItem();
        statusItem->setFont(QFont("Arial", 10, QFont::Bold));

        if (vReportDeliveries[i].Status() == clsDelivery::enStatus::Approved) {
            statusItem->setText(tr("Approved"));
            statusItem->setForeground(QBrush(QColor("#27ae60")));
        } else if (vReportDeliveries[i].Status() == clsDelivery::enStatus::Rejected) {
            statusItem->setText(tr("Rejected"));
            statusItem->setForeground(QBrush(QColor("#e74c3c")));
        } else {
            statusItem->setText(tr("Pending"));
            statusItem->setForeground(QBrush(QColor("#f39c12")));
        }

        tableRepDeliveries->setItem(i, 5, statusItem);
    }
}
// ==========================================
// 10. ARTICLES SCREEN
// ==========================================
void MainWindow::setupArticlesScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);

    QLabel *title = new QLabel(tr("Articles & Publications"), this);
    title->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 10px;");

    tableArticles = createStandardTable({tr("ID"), tr("Title"), tr("Category"), tr("Publish Date")});

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnAdd = new QPushButton(tr("Create Article"), this);
    btnEdit = new QPushButton(tr("Edit Article"), this);
    btnDel = new QPushButton(tr("Delete Article"), this);
    btnDel->setProperty("role", "danger");

    btnAdd->setStyleSheet("background-color: #27ae60; color: white; padding: 8px; width: 130px;");
    btnEdit->setStyleSheet("background-color: #f39c12; color: white; padding: 8px; width: 130px;");
    btnDel->setStyleSheet("background-color: #e74c3c; color: white; padding: 8px; width: 130px;");

    btnLayout->addStretch();
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnEdit);
    btnLayout->addWidget(btnDel);

    l->addWidget(title);
    l->addWidget(tableArticles);
    l->addLayout(btnLayout);

    // --- Connections ---
    connect(btnDel, &QPushButton::clicked, this, &MainWindow::deleteSelectedArticle);

    connect(btnAdd, &QPushButton::clicked, [this]() {
        if (_currentUser.Role() != clsUser::enRole::Admin) return;
        ArticleDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
            loadArticlesData();
        }
    });

    connect(btnEdit, &QPushButton::clicked, [this]() {
        if (_currentUser.Role() != clsUser::enRole::Admin) return;
        int row = tableArticles->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, tr("Selection Required"), tr("Please select an article to edit."));
            return;
        }
        QString id = tableArticles->item(row, 0)->text();
        ArticleDialog dialog(this);
        dialog.loadArticleForEdit(id.toStdString());
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
            loadArticlesData();
        }
    });

    // --- Connections ---

// NEW: Double-Click to Read!
connect(tableArticles, &QTableWidget::cellDoubleClicked, [this](int row, int column) {
    QString id = tableArticles->item(row, 0)->text();
    ArticleReaderDialog reader(id.toStdString(), this);
    reader.exec(); // Pop open the reader window
});
    stackedScreens->insertWidget(ScreenIndex::Articles, w);
}

void MainWindow::loadArticlesData()
{
    tableArticles->setRowCount(0);
    vector<clsArticle> vList = clsArticle::GetArticlesList();
    for (size_t i = 0; i < vList.size(); i++) {
        tableArticles->insertRow(i);
        tableArticles->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vList[i].ArticleID())));
        tableArticles->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(vList[i].Title())));
        tableArticles->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(vList[i].Category())));
        tableArticles->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(vList[i].PublishDate())));
    }
}

void MainWindow::deleteSelectedArticle()
{
    if (_currentUser.Role() != clsUser::enRole::Admin) return;
    int row = tableArticles->currentRow();
    if (row < 0) return;
    QString title = tableArticles->item(row, 1)->text();
    auto reply = QMessageBox::question(this, tr("Confirm Delete"),
        tr("Delete article \"%1\"?\nThis cannot be undone.").arg(title),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (reply != QMessageBox::Yes) return;
    if (clsArticle::Find(tableArticles->item(row, 0)->text().toStdString()).Delete()) loadArticlesData();
}

// ==========================================
// 11. DELIVERIES SCREEN
// ==========================================

void MainWindow::setupDeliveriesScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);

    QLabel *title = new QLabel(tr("Deliveries Management"), this);
    title->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 10px;");

    tableDeliveries = createStandardTable({tr("Delivery ID"), tr("Supplier"), tr("Date"), tr("Bone Type"), tr("Quantity (kg)"), tr("Status")});

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnAdd = new QPushButton(tr("Log New Delivery"), this);
    QPushButton *btnApprove = new QPushButton(tr("Approve Delivery"), this);
    QPushButton *btnReject = new QPushButton(tr("Reject"), this);

    btnAdd->setStyleSheet("background-color: #34495e; color: white; padding: 8px; width: 140px;");
    btnApprove->setStyleSheet("background-color: #27ae60; color: white; padding: 8px; width: 140px; font-weight: bold;");
    btnReject->setStyleSheet("background-color: #e74c3c; color: white; padding: 8px; width: 140px;");

    btnLayout->addStretch();
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnApprove);
    btnLayout->addWidget(btnReject);

    l->addWidget(title);
    l->addWidget(tableDeliveries);
    l->addLayout(btnLayout);

    // --- Connections ---
    connect(btnAdd, &QPushButton::clicked, [this]() {
        DeliveryDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
            loadDeliveriesData();
            QMessageBox::information(this, tr("Success"), tr("Delivery submitted! It is now Pending approval."));
        }
    });

    connect(btnApprove, &QPushButton::clicked, this, &MainWindow::approveSelectedDelivery);
    connect(btnReject, &QPushButton::clicked, this, &MainWindow::rejectSelectedDelivery);

    stackedScreens->insertWidget(ScreenIndex::Deliveries, w);
}

void MainWindow::loadDeliveriesData()
{
    tableDeliveries->setRowCount(0);

    vector<clsDelivery> vDeliveries = clsDelivery::GetDeliveriesList();

    for (size_t i = 0; i < vDeliveries.size(); i++) {
        tableDeliveries->insertRow(i);
        tableDeliveries->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vDeliveries[i].DeliveryID())));

        // Show supplier name; store raw ID in UserRole for approval
        clsSupplier s = clsSupplier::Find(vDeliveries[i].SupplierID());
        QString rawID = QString::fromStdString(vDeliveries[i].SupplierID());
        QString supplierDisplay = s.IsEmpty()
            ? rawID
            : QString::fromStdString(s.FirstName() + " (" + s.SupplierID() + ")");
        QTableWidgetItem *supItem = new QTableWidgetItem(supplierDisplay);
        supItem->setData(Qt::UserRole, rawID);
        tableDeliveries->setItem(i, 1, supItem);

        tableDeliveries->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(vDeliveries[i].Date())));
        tableDeliveries->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(vDeliveries[i].ProductID())));
        tableDeliveries->setItem(i, 4, new QTableWidgetItem(QString::number(vDeliveries[i].Quantity())));

        // Color Code the Status
        QTableWidgetItem *statusItem = new QTableWidgetItem();
        statusItem->setFont(QFont("Arial", 10, QFont::Bold));

        if (vDeliveries[i].Status() == clsDelivery::enStatus::Approved) {
            statusItem->setText(tr("Approved"));
            statusItem->setForeground(QBrush(QColor("#27ae60")));
        } else if (vDeliveries[i].Status() == clsDelivery::enStatus::Rejected) {
            statusItem->setText(tr("Rejected"));
            statusItem->setForeground(QBrush(QColor("#e74c3c")));
        } else {
            statusItem->setText(tr("Pending"));
            statusItem->setForeground(QBrush(QColor("#f39c12")));
        }

        tableDeliveries->setItem(i, 5, statusItem);
    }
}

void MainWindow::approveSelectedDelivery()
{
    int row = tableDeliveries->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, tr("Select Delivery"), tr("Please select a pending delivery to approve."));
        return;
    }

    // 1. Check if it is already approved
    QString currentStatus = tableDeliveries->item(row, 5)->text();
    if (currentStatus == tr("Approved")) {
        QMessageBox::information(this, tr("Already Approved"), tr("This delivery has already been processed."));
        return;
    }

    // 2. Read the data from the table (SupplierID from UserRole to get raw ID)
    string deliveryID = tableDeliveries->item(row, 0)->text().toStdString();
    QTableWidgetItem *supItem = tableDeliveries->item(row, 1);
    string supplierID = supItem->data(Qt::UserRole).toString().toStdString();
    if (supplierID.empty())
        supplierID = supItem->text().toStdString();
    double quantity   = tableDeliveries->item(row, 4)->text().toDouble();

    // ==========================================
    // Core Logic 1: Find the Supplier & Record Delivery
    //    RecordDelivery() awards consistent points
    //    and updates the supplier's weekly quantity.
    // ==========================================
    clsSupplier supplier = clsSupplier::Find(supplierID);
    if (supplier.IsEmpty()) {
        QMessageBox::warning(this, tr("Supplier Not Found"),
            tr("The supplier for this delivery no longer exists in the system."));
        return;
    }
    supplier.RecordDelivery(quantity);

    // ==========================================
    // Core Logic 2: Update Delivery Status
    // ==========================================
    clsDelivery delivery = clsDelivery::Find(deliveryID);
    if (!delivery.IsEmpty()) {
        delivery.SetStatus(clsDelivery::enStatus::Approved);
        delivery.Save();
    }

    // ==========================================
    // Core Logic 3: Update Inventory Stock
    //    Link delivery (bone type) to a product
    // ==========================================
    {
        string boneType = tableDeliveries->item(row, 3)->text().toStdString();
        vector<clsProduct> products = clsProduct::GetProductsList();
        if (!products.empty()) {
            bool found = false;
            string boneLower = boneType;
            transform(boneLower.begin(), boneLower.end(), boneLower.begin(), ::tolower);
            for (clsProduct& p : products) {
                string pName = p.Name();
                transform(pName.begin(), pName.end(), pName.begin(), ::tolower);
                // Match if product name contains the bone type (e.g. "Cow Bones" in product name)
                if (pName.find(boneLower) != string::npos ||
                    boneLower.find(pName.substr(0, 3)) != string::npos) {
                    p.SetStockQuantity(p.StockQuantity() + (int)quantity);
                    p.Save();
                    found = true;
                    break;
                }
            }
            if (!found) {
                // Fallback: increase first product's stock
                products[0].SetStockQuantity(products[0].StockQuantity() + (int)quantity);
                products[0].Save();
            }
        }
    }

    // 3. Update the UI directly
    QTableWidgetItem *statusItem = tableDeliveries->item(row, 5);
    statusItem->setText("Approved");
    statusItem->setForeground(QBrush(QColor("#27ae60")));

    QMessageBox::information(this, tr("Delivery Approved"),
        tr("Successfully approved! %1 kg recorded and points awarded to supplier.").arg(quantity));

    refreshDashboard();
}

void MainWindow::rejectSelectedDelivery()
{
    int row = tableDeliveries->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, tr("Select Delivery"), tr("Please select a pending delivery to reject."));
        return;
    }

    QString currentStatus = tableDeliveries->item(row, 5)->text();
    if (currentStatus == tr("Approved")) {
        QMessageBox::warning(this, tr("Already Approved"), tr("Cannot reject an already approved delivery."));
        return;
    }
    if (currentStatus == tr("Rejected")) {
        QMessageBox::information(this, tr("Already Rejected"), tr("This delivery has already been rejected."));
        return;
    }

    string deliveryID = tableDeliveries->item(row, 0)->text().toStdString();

    clsDelivery delivery = clsDelivery::Find(deliveryID);
    if (!delivery.IsEmpty()) {
        delivery.SetStatus(clsDelivery::enStatus::Rejected);
        delivery.Save();
    }

    QTableWidgetItem *statusItem = tableDeliveries->item(row, 5);
    statusItem->setText(tr("Rejected"));
    statusItem->setForeground(QBrush(QColor("#e74c3c")));

    QMessageBox::information(this, tr("Delivery Rejected"), tr("The delivery has been rejected."));
}

// ==========================================
// SUPPLIER PORTAL: DASHBOARD
// ==========================================

void MainWindow::setupSupplierDashboardScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(w);

    QLabel *title = new QLabel(tr("Supplier Dashboard"), this);
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    mainLayout->addWidget(title);

    // --- 1. Metrics Cards (Grid) ---
    QGridLayout *metricsGrid = new QGridLayout();
    lblSupDashTotalDeliveries = new QLabel(tr("Total Deliveries: 0"));
    lblSupDashTotalBones      = new QLabel(tr("Total Bones Delivered: 0 kg"));
    lblSupDashPoints          = new QLabel(tr("Current Points: 0"));
    lblSupDashRank            = new QLabel(tr("Current Rank: Bronze"));

    QString cardStyle = "background-color: white; padding: 20px; border-radius: 8px; font-size: 16px; font-weight: bold; border: 1px solid #dfe6e9;";
    lblSupDashTotalDeliveries->setStyleSheet(cardStyle + "color: #2980b9;"); // Blue
    lblSupDashTotalBones->setStyleSheet(cardStyle + "color: #d35400;");      // Orange
    lblSupDashPoints->setStyleSheet(cardStyle + "color: #27ae60;");          // Green
    lblSupDashRank->setStyleSheet(cardStyle + "color: #8e44ad;");            // Purple

    metricsGrid->addWidget(lblSupDashTotalDeliveries, 0, 0);
    metricsGrid->addWidget(lblSupDashTotalBones, 0, 1);
    metricsGrid->addWidget(lblSupDashPoints, 1, 0);
    metricsGrid->addWidget(lblSupDashRank, 1, 1);

    mainLayout->addLayout(metricsGrid);

    // --- 2. Recent Deliveries Table ---
    QLabel *subtitle = new QLabel(tr("Recent Deliveries"), this);
    subtitle->setStyleSheet("font-size: 18px; font-weight: bold; margin-top: 20px; margin-bottom: 5px;");
    mainLayout->addWidget(subtitle);

    tableSupRecentDeliveries = createStandardTable({tr("Delivery ID"), tr("Date"), tr("Bone Type"), tr("Quantity (kg)"), tr("Status")});
    mainLayout->addWidget(tableSupRecentDeliveries);

    // Add it to the system
    stackedScreens->insertWidget(ScreenIndex::SupDashboard, w);
}

void MainWindow::refreshSupplierDashboard()
{
    vector<clsDelivery> vAllDeliveries = clsDelivery::GetDeliveriesList();
    vector<clsDelivery> vMyDeliveries;

    double totalBones = 0;

    for (clsDelivery& d : vAllDeliveries) {
        if (d.SupplierID() == _currentSupplierID) {
            vMyDeliveries.push_back(d);
            totalBones += d.Quantity();
        }
    }

    lblSupDashTotalDeliveries->setText(tr("Total Deliveries: %1").arg(vMyDeliveries.size()));
    lblSupDashTotalBones->setText(tr("Total Bones Delivered: %1 kg").arg(QString::number(totalBones)));

    clsSupplier currentSupplier = clsSupplier::Find(_currentSupplierID);
    int points = 0;
    if (!currentSupplier.IsEmpty()) {
        points = currentSupplier.Points();
    }

    lblSupDashPoints->setText(tr("Current Points: %1").arg(points));

    QString rank = tr("Bronze");
    QString rankColor = "#cd7f32";

    if (points >= 10000) {
        rank = tr("Platinum");
        rankColor = "#34495e";
    } else if (points >= 5000) {
        rank = tr("Gold");
        rankColor = "#f1c40f";
    } else if (points >= 2000) {
        rank = tr("Silver");
        rankColor = "#95a5a6";
    }

    lblSupDashRank->setText(tr("Current Rank: %1").arg(rank));

    // تلوين بطاقة الرتبة ديناميكياً
    QString cardStyle = "background-color: white; padding: 20px; border-radius: 8px; font-size: 16px; font-weight: bold; border: 1px solid #dfe6e9;";
    lblSupDashRank->setStyleSheet(cardStyle + "color: " + rankColor + ";");

    // ==========================================
    // 5. ملء جدول "آخر التوصيلات" (Recent Deliveries)
    // ==========================================
    tableSupRecentDeliveries->setRowCount(0);

    // نبدأ من نهاية المصفوفة لكي نعرض أحدث 5 توصيلات فقط
    int rowCounter = 0;
    for (int i = vMyDeliveries.size() - 1; i >= 0 && rowCounter < 5; i--) {
        tableSupRecentDeliveries->insertRow(rowCounter);
        tableSupRecentDeliveries->setItem(rowCounter, 0, new QTableWidgetItem(QString::fromStdString(vMyDeliveries[i].DeliveryID())));
        tableSupRecentDeliveries->setItem(rowCounter, 1, new QTableWidgetItem(QString::fromStdString(vMyDeliveries[i].Date())));
        tableSupRecentDeliveries->setItem(rowCounter, 2, new QTableWidgetItem(QString::fromStdString(vMyDeliveries[i].ProductID())));
        tableSupRecentDeliveries->setItem(rowCounter, 3, new QTableWidgetItem(QString::number(vMyDeliveries[i].Quantity())));

        // تلوين حالة التوصيلة
        QTableWidgetItem *statusItem = new QTableWidgetItem();
        statusItem->setFont(QFont("Arial", 10, QFont::Bold));

        if (vMyDeliveries[i].Status() == clsDelivery::enStatus::Approved) {
            statusItem->setText(tr("Approved"));
            statusItem->setForeground(QBrush(QColor("#27ae60"))); // أخضر
        } else if (vMyDeliveries[i].Status() == clsDelivery::enStatus::Rejected) {
            statusItem->setText(tr("Rejected"));
            statusItem->setForeground(QBrush(QColor("#e74c3c"))); // أحمر
        } else {
            statusItem->setText(tr("Pending"));
            statusItem->setForeground(QBrush(QColor("#f39c12"))); // برتقالي
        }

        tableSupRecentDeliveries->setItem(rowCounter, 4, statusItem);
        rowCounter++;
    }
}

void MainWindow::refreshMyDeliveriesTable()
{
    vector<clsDelivery> vAllDeliveries = clsDelivery::GetDeliveriesList();

    tableSupMyDeliveries->setRowCount(0);

    int rowCounter = 0;
    for (clsDelivery &d : vAllDeliveries) {
        if (d.SupplierID() == _currentSupplierID) {
            tableSupMyDeliveries->insertRow(rowCounter);
            tableSupMyDeliveries->setItem(rowCounter, 0, new QTableWidgetItem(QString::fromStdString(d.DeliveryID())));
            tableSupMyDeliveries->setItem(rowCounter, 1, new QTableWidgetItem(QString::fromStdString(d.Date())));
            tableSupMyDeliveries->setItem(rowCounter, 2, new QTableWidgetItem(QString::fromStdString(d.ProductID())));
            tableSupMyDeliveries->setItem(rowCounter, 3, new QTableWidgetItem(QString::number(d.Quantity())));

            // Color code the status
            QTableWidgetItem *statusItem = new QTableWidgetItem();
            statusItem->setFont(QFont("Arial", 10, QFont::Bold));

            if (d.Status() == clsDelivery::enStatus::Approved) {
                statusItem->setText(tr("Approved"));
                statusItem->setForeground(QBrush(QColor("#27ae60")));
            } else if (d.Status() == clsDelivery::enStatus::Rejected) {
                statusItem->setText(tr("Rejected"));
                statusItem->setForeground(QBrush(QColor("#e74c3c")));
            } else {
                statusItem->setText(tr("Pending"));
                statusItem->setForeground(QBrush(QColor("#f39c12")));
            }

            tableSupMyDeliveries->setItem(rowCounter, 4, statusItem);
            rowCounter++;
        }
    }
}
// ==========================================
// SUPPLIER PORTAL: MY DELIVERIES
// ==========================================

void MainWindow::setupMyDeliveriesScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(w);

    QLabel *title = new QLabel(tr("My Deliveries"), this);
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");

    // --- Action Buttons ---
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnNewDelivery = new QPushButton(tr("Log New Delivery"), this);
    btnNewDelivery->setStyleSheet("background-color: #3498db; color: white; padding: 10px; font-weight: bold; border-radius: 4px;");

    btnLayout->addWidget(btnNewDelivery);
    btnLayout->addStretch(); // Pushes button to the left

    connect(btnNewDelivery, &QPushButton::clicked, this, &MainWindow::openNewDeliveryForm);

    // --- Deliveries Table ---
    tableSupMyDeliveries = createStandardTable({tr("Delivery ID"), tr("Date"), tr("Bone Type"), tr("Quantity (kg)"), tr("Status")});

    mainLayout->addWidget(title);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(tableSupMyDeliveries);

    // Insert into the system so the button can find it!
    stackedScreens->insertWidget(ScreenIndex::MyDeliveries, w);
}

// ==========================================
// SUPPLIER PORTAL: REWARDS & PROFILE (Placeholders)
// ==========================================


void MainWindow::setupMyRewardsScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(w);
    mainLayout->setContentsMargins(20, 20, 20, 20); // Give the whole screen some breathing room
    mainLayout->setSpacing(20);

    QLabel *title = new QLabel(tr("My Points & Badges"), this);
    title->setStyleSheet("font-size: 26px; font-weight: bold; color: #2c3e50;");
    mainLayout->addWidget(title);

    // ==========================================
    // TOP CARD: STATUS & PROGRESS
    // ==========================================
    QFrame *cardStats = new QFrame(this);
    cardStats->setObjectName("rewardsStatsCard");
    cardStats->setStyleSheet("QFrame { background-color: white; border-radius: 8px; border: 1px solid #dfe6e9; }");
    QVBoxLayout *statsLayout = new QVBoxLayout(cardStats);
    statsLayout->setContentsMargins(20, 20, 20, 20);
    statsLayout->setSpacing(15);

    QLabel *lblStatsTitle = new QLabel(tr("Current Status"), cardStats);
    lblStatsTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #34495e; border: none;");

    lblRewardPoints = new QLabel(tr("Total Points: 0"), cardStats);
    lblRewardRank = new QLabel(tr("Current Rank: Bronze"), cardStats);
    lblRewardPoints->setStyleSheet("font-size: 22px; font-weight: bold; color: #27ae60; border: none;");
    lblRewardRank->setStyleSheet("font-size: 18px; font-weight: bold; color: #8e44ad; border: none;");

    // Upgraded Progress Bar (White text, thicker bar, lighter background)
    barNextRank = new QProgressBar(cardStats);
    barNextRank->setFixedHeight(30);
    barNextRank->setStyleSheet(
        "QProgressBar {"
        "   border: 1px solid #bdc3c7;"
        "   border-radius: 6px;"
        "   text-align: center;"
        "   color: #f39c12;"        // FIX: White text so it's readable!
        "   font-weight: bold;"
        "   font-size: 14px;"
        "   background-color: #ecf0f1;" // Lighter grey background
        "}"
        "QProgressBar::chunk {"
        "   background-color: #3498db;" // Blue progress fill
        "   border-radius: 5px;"
        "}"
    );

    statsLayout->addWidget(lblStatsTitle);
    statsLayout->addWidget(lblRewardPoints);
    statsLayout->addWidget(lblRewardRank);

    QLabel *lblProgressText = new QLabel(tr("Progress to next rank:"), cardStats);
    lblProgressText->setStyleSheet("font-size: 14px; color: #7f8c8d; border: none; margin-top: 10px;");
    statsLayout->addWidget(lblProgressText);
    statsLayout->addWidget(barNextRank);

    mainLayout->addWidget(cardStats);

    // ==========================================
    // BOTTOM CARD: CERTIFICATES
    // ==========================================
    QFrame *cardCerts = new QFrame(this);
    cardCerts->setObjectName("rewardsCertsCard");
    cardCerts->setStyleSheet("QFrame { background-color: white; border-radius: 8px; border: 1px solid #dfe6e9; }");
    QVBoxLayout *certsMainLayout = new QVBoxLayout(cardCerts);
    certsMainLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *lblCertsTitle = new QLabel(tr("Certifications & Awards"), cardCerts);
    lblCertsTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #34495e; border: none; margin-bottom: 10px;");
    certsMainLayout->addWidget(lblCertsTitle);

    QHBoxLayout *certsLayout = new QHBoxLayout();
    certsLayout->setSpacing(20);

    auto createCert = [&](const QString& icon, const QString& title, const QString& req) -> QFrame* {
        QFrame *frame = new QFrame(cardCerts);
        frame->setMinimumSize(200, 180);
        frame->setStyleSheet(
            "QFrame { background-color: #f8f9fa; border: 2px dashed #bdc3c7; border-radius: 12px; }"
        );

        QVBoxLayout *lay = new QVBoxLayout(frame);
        lay->setContentsMargins(15, 20, 15, 15);
        lay->setSpacing(8);

        // Icon area
        QLabel *ico = new QLabel(frame);
        ico->setAlignment(Qt::AlignCenter);
        ico->setStyleSheet("font-size: 36px; border: none; color: #b0b0b0;");

        // Title
        QLabel *lbl = new QLabel(frame);
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setStyleSheet("color: #b0b0b0; font-size: 13px; font-weight: 600; border: none;");

        // Requirement
        QLabel *reqLbl = new QLabel(req, frame);
        reqLbl->setAlignment(Qt::AlignCenter);
        reqLbl->setStyleSheet("color: #ccd1d9; font-size: 11px; border: none;");

        ico->setText(icon);
        lbl->setText(title);
        lay->addStretch();
        lay->addWidget(ico);
        lay->addWidget(lbl);
        lay->addWidget(reqLbl);
        lay->addStretch();

        // Store pointers for later update
        frame->setProperty("icoPtr", QVariant::fromValue(reinterpret_cast<quintptr>(ico)));
        frame->setProperty("lblPtr", QVariant::fromValue(reinterpret_cast<quintptr>(lbl)));
        frame->setProperty("reqPtr", QVariant::fromValue(reinterpret_cast<quintptr>(reqLbl)));
        return frame;
    };

    certFrames[0] = createCert("\xF0\x9F\x94\x92", tr("Eco Friendly Supplier"), tr("Unlock at 1,000 pts"));
    certFrames[1] = createCert("\xF0\x9F\x94\x92", tr("Sustainability Champion"), tr("Unlock at 5,000 pts"));
    certFrames[2] = createCert("\xF0\x9F\x94\x92", tr("Golden Partner"), tr("Unlock at 10,000 pts"));

    certsLayout->addWidget(certFrames[0]);
    certsLayout->addWidget(certFrames[1]);
    certsLayout->addWidget(certFrames[2]);

    certsMainLayout->addLayout(certsLayout);
    mainLayout->addWidget(cardCerts);

    mainLayout->addStretch();
    stackedScreens->insertWidget(ScreenIndex::MyRewards, w);
}

void MainWindow::refreshMyRewardsScreen()
{
    clsSupplier currentSupplier = clsSupplier::Find(_currentSupplierID);
    int points = 0;
    if (!currentSupplier.IsEmpty()) {
        points = currentSupplier.Points();
    }
    lblRewardPoints->setText(tr("Total Points: %1").arg(points));

    // 2. Calculate Rank & Update Progress Bar
    QString rank = tr("Bronze");
    int nextTier = 2000;

    if (points >= 10000) {
        rank = tr("Platinum");
        barNextRank->setRange(0, 1);
        barNextRank->setValue(1);
        barNextRank->setFormat(tr("Max Rank Reached!"));
    } else if (points >= 5000) {
        rank = tr("Gold"); nextTier = 10000;
    } else if (points >= 2000) {
        rank = tr("Silver"); nextTier = 5000;
    }

    lblRewardRank->setText(tr("Current Rank: %1").arg(rank));

    if (points < 10000) {
        barNextRank->setRange(0, nextTier);
        barNextRank->setValue(points);
        barNextRank->setFormat(tr("%1 / %2 pts").arg(points).arg(nextTier));
    }

    // 3. Reset all certificates to locked, then unlock conditionally
    struct CertInfo {
        int pts;
        const char* iconLocked;
        const char* iconUnlocked;
        const char* title;
        const char* req;
        const char* ribbonBg;
        const char* borderClr;
        const char* textClr;
        const char* badgeClr;
    };
    CertInfo certs[3] = {
        {1000, "\xF0\x9F\x94\x92", "\xF0\x9F\x9C\x91", QT_TRANSLATE_NOOP("MainWindow", "Eco Friendly Supplier"), QT_TRANSLATE_NOOP("MainWindow", "1,000 pts required"),
         "#27ae60", "#27ae60", "#1e8449", "#e8f8f0"},
        {5000, "\xF0\x9F\x94\x92", "\xF0\x9F\x8C\x8D", QT_TRANSLATE_NOOP("MainWindow", "Sustainability Champion"), QT_TRANSLATE_NOOP("MainWindow", "5,000 pts required"),
         "#2980b9", "#2980b9", "#1a5276", "#eaf2f8"},
        {10000, "\xF0\x9F\x94\x92", "\xE2\xAD\x90", QT_TRANSLATE_NOOP("MainWindow", "Golden Partner"), QT_TRANSLATE_NOOP("MainWindow", "10,000 pts required"),
         "#f39c12", "#f1c40f", "#7d6608", "#fff8e1"},
    };

    for (int i = 0; i < 3; i++) {
        QFrame *f = certFrames[i];
        QLabel *ico = reinterpret_cast<QLabel*>(f->property("icoPtr").value<quintptr>());
        QLabel *lbl = reinterpret_cast<QLabel*>(f->property("lblPtr").value<quintptr>());
        QLabel *reqLbl = reinterpret_cast<QLabel*>(f->property("reqPtr").value<quintptr>());

        if (points >= certs[i].pts) {
            // Ribbon band at top via gradient top border, clean card below
            f->setStyleSheet(QString(
                "QFrame {"
                "  background-color: white;"
                "  border: 2px solid %2;"
                "  border-top: 14px solid %2;"
                "  border-radius: 8px;"
                "}"
            ).arg(certs[i].borderClr));
            if (ico) {
                ico->setText(certs[i].iconUnlocked);
                ico->setStyleSheet(QString(
                    "font-size: 40px; border: none; background: %1;"
                    "  border-radius: 20px; padding: 4px; color: %2;"
                ).arg(certs[i].badgeClr, certs[i].textClr));
            }
            if (lbl) {
                lbl->setText(tr(certs[i].title));
                lbl->setStyleSheet(QString(
                    "color: %1; font-size: 14px; font-weight: 700; border: none; margin-top: 4px;"
                ).arg(certs[i].textClr));
            }
            if (reqLbl) {
                reqLbl->setText(tr("✓ %1 pts earned").arg(certs[i].pts));
                reqLbl->setStyleSheet(QString(
                    "color: %1; font-size: 11px; font-weight: 600; border: none;"
                ).arg(certs[i].borderClr));
            }
        } else {
            f->setStyleSheet(
                "QFrame {"
                "  background-color: #f8f9fa;"
                "  border: 2px dashed #ccd1d9;"
                "  border-radius: 8px;"
                "}"
            );
            if (ico) {
                ico->setText(certs[i].iconLocked);
                ico->setStyleSheet("font-size: 34px; border: none; color: #6b7280;");
            }
            if (lbl) {
                lbl->setText(tr(certs[i].title));
                lbl->setStyleSheet("color: #6b7280; font-size: 13px; font-weight: 600; border: none;");
            }
            if (reqLbl) {
                reqLbl->setText(tr(certs[i].req));
                reqLbl->setStyleSheet("color: #94a3b8; font-size: 11px; border: none;");
            }
        }
    }
}

void MainWindow::setupMyProfileScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(w);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    QLabel *title = new QLabel(tr("My Profile"), this);
    title->setStyleSheet("font-size: 26px; font-weight: bold; color: #2c3e50;");
    mainLayout->addWidget(title);

    // ==========================================
    // PROFILE CARD
    // ==========================================
    QFrame *card = new QFrame(this);
    card->setObjectName("profileCard");
    card->setStyleSheet("QFrame { background-color: white; border-radius: 8px; border: 1px solid #dfe6e9; }");

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(30, 30, 30, 30); // Good padding inside the card

    QLabel *cardHeader = new QLabel(tr("Account Information"), card);
    cardHeader->setStyleSheet("font-size: 18px; font-weight: bold; color: #34495e; border: none; margin-bottom: 15px;");
    cardLayout->addWidget(cardHeader);

  // Using QFormLayout to perfectly align labels and data
    QFormLayout *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignLeft);
    form->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    form->setSpacing(20);

    QString labelStyle = "font-weight: bold; color: #7f8c8d; border: none; font-size: 15px;";
    QString valueStyle = "color: #2c3e50; font-weight: bold; border: none; font-size: 16px;";

    // 1. Initialize the global labels with empty strings
    lblProfileName = new QLabel("", card);
    lblProfileUsername = new QLabel("", card);
    lblProfilePhone = new QLabel("", card);
    lblProfileEmail = new QLabel("", card);
    lblProfileRole = new QLabel("", card);

    // Apply the styles
    lblProfileName->setStyleSheet(valueStyle);
    lblProfileUsername->setStyleSheet(valueStyle);
    lblProfilePhone->setStyleSheet(valueStyle);
    lblProfileEmail->setStyleSheet(valueStyle);
    lblProfileRole->setStyleSheet(valueStyle);

    // 2. Create the static "Key" labels
    auto createKey = [&](const QString& text) {
        QLabel *lbl = new QLabel(text, card);
        lbl->setStyleSheet(labelStyle);
        return lbl;
    };

    // 3. Add them to the form layout
    form->addRow(createKey(tr("Full Name:")), lblProfileName);
    form->addRow(createKey(tr("Username:")), lblProfileUsername);
    form->addRow(createKey(tr("Email:")), lblProfileEmail);
    form->addRow(createKey(tr("Phone Number:")), lblProfilePhone);
    form->addRow(createKey(tr("Account Role:")), lblProfileRole);

    cardLayout->addLayout(form);

    // Divider Line
    QFrame *line = new QFrame(card);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("border: none; background-color: #ecf0f1; max-height: 1px; margin-top: 15px; margin-bottom: 15px;");
    cardLayout->addWidget(line);

    // Edit Button (Read-only for now)
    QPushButton *btnEdit = new QPushButton(tr("Request Profile Update"), card);
    btnEdit->setFixedWidth(200);
    btnEdit->setStyleSheet("background-color: #ecf0f1; color: #7f8c8d; padding: 10px; font-weight: bold; border-radius: 4px;");
    cardLayout->addWidget(btnEdit);

    mainLayout->addWidget(card);
    mainLayout->addStretch(); // Pushes the card to the top so it doesn't stretch weirdly

    stackedScreens->insertWidget(ScreenIndex::MyProfile, w);
}

void MainWindow::refreshMyProfileScreen()
{
    if (!_currentUser.IsEmpty())
    {
        lblProfileName->setText(QString::fromStdString(_currentUser.FullName()));
        lblProfileUsername->setText(QString::fromStdString(_currentUser.Username()));
        lblProfileEmail->setText(QString::fromStdString(_currentUser.Email()));
        lblProfilePhone->setText(QString::fromStdString(_currentUser.PhoneNumber()));

        if (_currentUser.Role() == clsUser::enRole::Supplier) {
            lblProfileRole->setText(tr("Certified Supplier"));
            lblProfileRole->setStyleSheet("color: #27ae60; font-weight: bold; border: none; font-size: 16px;");
        } else if (_currentUser.Role() == clsUser::enRole::Admin) {
            lblProfileRole->setText(tr("System Administrator"));
            lblProfileRole->setStyleSheet("color: #e74c3c; font-weight: bold; border: none; font-size: 16px;");
        }
    }
}

// ==========================================
// CUSTOMER PORTAL
// ==========================================

void MainWindow::setupCustDashboardScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);
    l->setContentsMargins(20, 20, 20, 20);
    l->setSpacing(20);

    QLabel *title = new QLabel(tr("Customer Dashboard"), this);
    title->setStyleSheet("font-size: 26px; font-weight: bold; color: #2c3e50;");

    // Metrics row
    QHBoxLayout *metrics = new QHBoxLayout();
    metrics->setSpacing(15);

    auto createMetric = [&](const QString& label, QLabel*& value, const QString& color) {
        QFrame *card = new QFrame(w);
        card->setStyleSheet("QFrame { background-color: white; border-radius: 8px; border: 1px solid #dfe6e9; }");
        QVBoxLayout *cl = new QVBoxLayout(card);
        cl->setContentsMargins(20, 15, 20, 15);
        cl->setAlignment(Qt::AlignCenter);
        value = new QLabel("0", card);
        value->setStyleSheet(QString("font-size: 28px; font-weight: bold; color: %1; border: none;").arg(color));
        value->setAlignment(Qt::AlignCenter);
        QLabel *lbl = new QLabel(label, card);
        lbl->setStyleSheet("font-size: 13px; color: #7f8c8d; border: none;");
        lbl->setAlignment(Qt::AlignCenter);
        cl->addWidget(value);
        cl->addWidget(lbl);
        card->setMinimumSize(180, 100);
        return card;
    };

    metrics->addWidget(createMetric(tr("Total Orders"), lblCustTotalOrders, "#2980b9"));
    metrics->addWidget(createMetric(tr("Completed"), lblCustCompletedOrders, "#27ae60"));
    metrics->addWidget(createMetric(tr("My Points"), lblCustPoints, "#f39c12"));
    metrics->addStretch();

    // Recent Orders
    QHBoxLayout *recentHeader = new QHBoxLayout();
    QLabel *subTitle = new QLabel(tr("Recent Orders"), this);
    subTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #34495e; margin-top: 10px;");

    QPushButton *btnRefreshDash = new QPushButton(tr("\xE2\x9F\xB3  Refresh"), this);
    btnRefreshDash->setStyleSheet("background-color: #3498db; color: white; padding: 5px 12px; border-radius: 4px; font-weight: bold; font-size: 12px;");
    connect(btnRefreshDash, &QPushButton::clicked, this, &MainWindow::loadCustDashboardData);

    recentHeader->addWidget(subTitle);
    recentHeader->addStretch();
    recentHeader->addWidget(btnRefreshDash);

    tableCustRecentOrders = createStandardTable({tr("Order ID"), tr("Product"), tr("Qty"), tr("Total"), tr("Date"), tr("Status")});

    QVBoxLayout *cl = static_cast<QVBoxLayout*>(l);
    cl->addWidget(title);
    cl->addLayout(metrics);
    cl->addLayout(recentHeader);
    cl->addWidget(tableCustRecentOrders);

    stackedScreens->insertWidget(ScreenIndex::CustDashboard, w);
}

void MainWindow::setupCustProductsScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);
    l->setContentsMargins(20, 20, 20, 20);
    l->setSpacing(15);

    QLabel *title = new QLabel(tr("Product Catalog"), this);
    title->setStyleSheet("font-size: 26px; font-weight: bold; color: #2c3e50;");

    // Search & Filter bar
    QHBoxLayout *filterBar = new QHBoxLayout();
    txtCustSearch = new QLineEdit(this);
    txtCustSearch->setPlaceholderText(tr("Search products..."));
    txtCustSearch->setFixedWidth(250);
    txtCustSearch->setStyleSheet("padding: 6px; border: 1px solid #bdc3c7; border-radius: 4px;");

    cmbCustCategoryFilter = new QComboBox(this);
    cmbCustCategoryFilter->addItem(tr("All Categories"), -1);
    cmbCustCategoryFilter->addItem(tr("Raw Bone Fertilizer"), clsProduct::enCategory::RawBoneFertilizer);
    cmbCustCategoryFilter->addItem(tr("Powder Fertilizer"), clsProduct::enCategory::PowderFertilizer);
    cmbCustCategoryFilter->addItem(tr("Organic Fertilizer"), clsProduct::enCategory::OrganicFertilizer);
    cmbCustCategoryFilter->addItem(tr("Feed Supplement"), clsProduct::enCategory::FeedSupplement);
    cmbCustCategoryFilter->setStyleSheet(
    "QComboBox { padding: 6px; border: 1px solid #bdc3c7; border-radius: 4px; }"
);
    QPushButton *btnSearch = new QPushButton(tr("Search"), this);
    btnSearch->setStyleSheet("background-color: #2980b9; color: white; padding: 6px 15px; border-radius: 4px; font-weight: bold;");

    QPushButton *btnCreateOrder = new QPushButton(tr("+ Create Order"), this);
    btnCreateOrder->setStyleSheet("background-color: #27ae60; color: white; padding: 6px 15px; border-radius: 4px; font-weight: bold;");

    filterBar->addWidget(txtCustSearch);
    filterBar->addWidget(btnSearch);
    filterBar->addWidget(cmbCustCategoryFilter);
    filterBar->addStretch();
    filterBar->addWidget(btnCreateOrder);

    tableCustProducts = createStandardTable({tr("Product ID"), tr("Name"), tr("Category"), tr("Price ($)"), tr("Stock"), tr("Expiry")});

    // Filter logic
    auto reloadProducts = [this]() {
        tableCustProducts->setRowCount(0);
        vector<clsProduct> all = clsProduct::GetProductsList();
        QString query = txtCustSearch->text().toLower();
        int catFilter = cmbCustCategoryFilter->currentData().toInt();

        for (size_t i = 0; i < all.size(); i++) {
            if (!query.isEmpty() && !QString::fromStdString(all[i].Name()).toLower().contains(query))
                continue;
            if (catFilter != -1 && all[i].Category() != catFilter)
                continue;

            int r = tableCustProducts->rowCount();
            tableCustProducts->insertRow(r);
            tableCustProducts->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(all[i].ProductID())));
            tableCustProducts->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(all[i].Name())));
            QString cat;
            switch (all[i].Category()) {
                case 1: cat = tr("Raw Bone Fertilizer"); break;
                case 2: cat = tr("Powder Fertilizer"); break;
                case 3: cat = tr("Organic Fertilizer"); break;
                case 4: cat = tr("Feed Supplement"); break;
            }
            tableCustProducts->setItem(r, 2, new QTableWidgetItem(cat));
            tableCustProducts->setItem(r, 3, new QTableWidgetItem(QString::number(all[i].Price(), 'f', 2)));
            tableCustProducts->setItem(r, 4, new QTableWidgetItem(QString::number(all[i].StockQuantity())));
            tableCustProducts->setItem(r, 5, new QTableWidgetItem(QString::fromStdString(all[i].ExpiryDate())));
        }
    };

    connect(btnSearch, &QPushButton::clicked, this, reloadProducts);
    connect(cmbCustCategoryFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, reloadProducts);
    connect(txtCustSearch, &QLineEdit::returnPressed, this, reloadProducts);

    // Create Order dialog
    connect(btnCreateOrder, &QPushButton::clicked, [this]() {
        if (_currentCustomerID.empty()) {
            QMessageBox::warning(this, tr("Error"), tr("No customer profile linked to your account."));
            return;
        }
        QDialog dlg(this);
        dlg.setWindowTitle(tr("Create Order"));
        dlg.setFixedSize(350, 280);
        dlg.setStyleSheet("background-color: white; color: #2c3e50; font-size: 14px;");

        QVBoxLayout *dl = new QVBoxLayout(&dlg);

        QComboBox *cmbProduct = new QComboBox(&dlg);
        cmbProduct->setStyleSheet("padding: 5px; border: 1px solid #bdc3c7; border-radius: 4px;");
        vector<clsProduct> prods = clsProduct::GetProductsList();
        for (auto& p : prods) {
            if (p.StockQuantity() > 0)
                cmbProduct->addItem(QString::fromStdString(p.Name() + " ($" + to_string(p.Price()) + ")"),
                    QString::fromStdString(p.ProductID()));
        }

        QSpinBox *spinQty = new QSpinBox(&dlg);
        spinQty->setRange(1, 999);
        spinQty->setPrefix(tr("Qty: "));

        QLabel *lblTotal = new QLabel(tr("Total: $0.00"), &dlg);
        lblTotal->setStyleSheet("font-size: 18px; font-weight: bold; color: #27ae60; border: none;");

        connect(cmbProduct, QOverload<int>::of(&QComboBox::currentIndexChanged), [&]() {
            if (cmbProduct->currentIndex() < 0) return;
            string pid = cmbProduct->currentData().toString().toStdString();
            clsProduct p = clsProduct::Find(pid);
            lblTotal->setText(tr("Total: $%1").arg(p.Price() * spinQty->value(), 0, 'f', 2));
        });
        connect(spinQty, QOverload<int>::of(&QSpinBox::valueChanged), [&]() {
            if (cmbProduct->currentIndex() < 0) return;
            string pid = cmbProduct->currentData().toString().toStdString();
            clsProduct p = clsProduct::Find(pid);
            lblTotal->setText(tr("Total: $%1").arg(p.Price() * spinQty->value(), 0, 'f', 2));
        });

        QPushButton *btnSubmit = new QPushButton(tr("Submit Order"), &dlg);
        btnSubmit->setStyleSheet("background-color: #27ae60; color: white; padding: 8px; border-radius: 4px; font-weight: bold;");

        connect(btnSubmit, &QPushButton::clicked, [&]() {
            if (cmbProduct->currentIndex() < 0) return;
            string pid = cmbProduct->currentData().toString().toStdString();
            clsProduct prod = clsProduct::Find(pid);
            if (prod.IsEmpty()) return;

            int qty = spinQty->value();
            if (qty > prod.StockQuantity()) {
                QMessageBox::warning(&dlg, tr("Insufficient Stock"),
                    tr("Only %1 units available. Requested: %2.").arg(prod.StockQuantity()).arg(qty));
                return;
            }

            // Generate new order ID
            int maxNum = 0;
            for (auto& o : clsOrder::GetOrdersList()) {
                string oid = o.OrderID();
                if (oid.size() > 4 && oid.substr(0, 4) == "ORD-") {
                    try { int n = stoi(oid.substr(4)); if (n > maxNum) maxNum = n; }
                    catch (...) {}
                }
            }
            clsOrder order = clsOrder::GetAddNewOrderObject("ORD-" + to_string(maxNum + 1));
            order.SetCustomerID(_currentCustomerID);
            order.SetProductID(pid);
            order.SetQuantity(qty);
            order.SetTotalPrice(prod.Price() * qty);
            order.SetOrderDate(QDateTime::currentDateTime().toString("yyyy-MM-dd").toStdString());
            order.SetStatus(clsOrder::enStatus::Pending);
            order.Save();

            // Decrease stock
            prod.SetStockQuantity(prod.StockQuantity() - qty);
            prod.Save();

            QMessageBox::information(&dlg, tr("Success"), tr("Order placed successfully!"));
            dlg.accept();
        });

        dl->addWidget(new QLabel(tr("Select Product:"), &dlg));
        dl->addWidget(cmbProduct);
        dl->addWidget(spinQty);
        dl->addWidget(lblTotal);
        dl->addStretch();
        dl->addWidget(btnSubmit);

        dlg.exec();
    });

    l->addWidget(title);
    l->addLayout(filterBar);
    l->addWidget(tableCustProducts);

    stackedScreens->insertWidget(ScreenIndex::CustProducts, w);
}

void MainWindow::setupCustOrdersScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);
    l->setContentsMargins(20, 20, 20, 20);
    l->setSpacing(15);

    QLabel *title = new QLabel(tr("My Orders"), this);
    title->setStyleSheet("font-size: 26px; font-weight: bold; color: #2c3e50;");

    // Status filter buttons
    QHBoxLayout *filters = new QHBoxLayout();
    QPushButton *btnAll = new QPushButton(tr("All"), this);
    QPushButton *btnPending = new QPushButton(tr("Pending"), this);
    QPushButton *btnProcessing = new QPushButton(tr("Processing"), this);
    QPushButton *btnDelivered = new QPushButton(tr("Delivered"), this);
    QPushButton *btnCancelled = new QPushButton(tr("Cancelled"), this);

    QString activeBtn = "background-color: #2980b9; color: white; padding: 6px 12px; border-radius: 4px; font-weight: bold;";
    QString inactiveBtn = "background-color: #ecf0f1; color: #7f8c8d; padding: 6px 12px; border-radius: 4px;";
    btnAll->setStyleSheet(activeBtn);
    btnPending->setStyleSheet(inactiveBtn);
    btnProcessing->setStyleSheet(inactiveBtn);
    btnDelivered->setStyleSheet(inactiveBtn);
    btnCancelled->setStyleSheet(inactiveBtn);

    tableCustOrders = createStandardTable({tr("Order ID"), tr("Product"), tr("Qty"), tr("Total"), tr("Date"), tr("Status")});

    auto setFilter = [=](QPushButton* active, int statusFilter) {
        for (auto b : {btnAll, btnPending, btnProcessing, btnDelivered, btnCancelled})
            b->setStyleSheet(inactiveBtn);
        active->setStyleSheet(activeBtn);
        _custOrderFilter = statusFilter;
        loadCustOrdersData();
    };

    connect(btnAll, &QPushButton::clicked, [=]() { setFilter(btnAll, -1); });
    connect(btnPending, &QPushButton::clicked, [=]() { setFilter(btnPending, 1); });
    connect(btnProcessing, &QPushButton::clicked, [=]() { setFilter(btnProcessing, 3); });
    connect(btnDelivered, &QPushButton::clicked, [=]() { setFilter(btnDelivered, 4); });
    connect(btnCancelled, &QPushButton::clicked, [=]() { setFilter(btnCancelled, 5); });

    filters->addWidget(btnAll);
    filters->addWidget(btnPending);
    filters->addWidget(btnProcessing);
    filters->addWidget(btnDelivered);
    filters->addWidget(btnCancelled);
    filters->addStretch();

    QPushButton *btnRefreshOrders = new QPushButton(tr("\xE2\x9F\xB3  Refresh"), this);
    btnRefreshOrders->setStyleSheet("background-color: #3498db; color: white; padding: 5px 12px; border-radius: 4px; font-weight: bold; font-size: 12px;");
    connect(btnRefreshOrders, &QPushButton::clicked, [=]() {
        loadCustOrdersData();
    });
    filters->addWidget(btnRefreshOrders);

    l->addWidget(title);
    l->addLayout(filters);
    l->addWidget(tableCustOrders);

    stackedScreens->insertWidget(ScreenIndex::CustOrders, w);
}

void MainWindow::setupCustPointsScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);
    l->setContentsMargins(20, 20, 20, 20);
    l->setSpacing(20);

    QLabel *title = new QLabel(tr("My Points & Rewards"), this);
    title->setStyleSheet("font-size: 26px; font-weight: bold; color: #2c3e50;");

    QFrame *card = new QFrame(this);
    card->setObjectName("pointsCard");
    card->setStyleSheet("QFrame { background-color: white; border-radius: 8px; border: 1px solid #dfe6e9; }");
    QVBoxLayout *cl = new QVBoxLayout(card);
    cl->setContentsMargins(30, 30, 30, 30);
    cl->setSpacing(15);

    lblCustPointsDisplay = new QLabel(tr("Current Points: 0"), this);
    lblCustPointsDisplay->setStyleSheet("font-size: 24px; font-weight: bold; color: #f39c12; border: none;");

    lblCustRewardLevel = new QLabel(tr("Reward Level: Bronze"), this);
    lblCustRewardLevel->setStyleSheet("font-size: 18px; font-weight: bold; color: #8e44ad; border: none;");

    QLabel *info = new QLabel(tr("Earn points by placing orders. 100 points per order completed."), this);
    info->setStyleSheet("font-size: 13px; color: #95a5a6; border: none;");

    cl->addWidget(lblCustPointsDisplay);
    cl->addWidget(lblCustRewardLevel);
    cl->addWidget(info);

    l->addWidget(title);
    l->addWidget(card);
    l->addStretch();

    stackedScreens->insertWidget(ScreenIndex::CustPoints, w);
}

void MainWindow::setupCustProfileScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);
    l->setContentsMargins(20, 20, 20, 20);
    l->setSpacing(20);

    QLabel *title = new QLabel(tr("My Profile"), this);
    title->setStyleSheet("font-size: 26px; font-weight: bold; color: #2c3e50;");

    QFrame *card = new QFrame(this);
    card->setObjectName("customerProfileCard");
    card->setStyleSheet("QFrame { background-color: white; border-radius: 8px; border: 1px solid #dfe6e9; }");
    QVBoxLayout *cl = new QVBoxLayout(card);
    cl->setContentsMargins(30, 30, 30, 30);
    cl->setSpacing(12);

    auto createField = [&](const QString& label) -> QLabel* {
        QHBoxLayout *row = new QHBoxLayout();
        QLabel *k = new QLabel(label, card);
        k->setStyleSheet("font-weight: bold; color: #34495e; border: none; min-width: 120px;");
        QLabel *v = new QLabel("-", card);
        v->setStyleSheet("color: #2c3e50; border: none;");
        row->addWidget(k);
        row->addWidget(v);
        row->addStretch();
        cl->addLayout(row);
        return v;
    };

    lblCustProfName = createField(tr("Name:"));
    lblCustProfPhone = createField(tr("Phone:"));
    lblCustProfEmail = createField(tr("Email:"));
    lblCustProfAddr = createField(tr("Address:"));
    lblCustProfType = createField(tr("Type:"));

    l->addWidget(title);
    l->addWidget(card);
    l->addStretch();

    stackedScreens->insertWidget(ScreenIndex::CustProfile, w);
}

void MainWindow::loadCustDashboardData()
{
    if (_currentCustomerID.empty()) return;

    clsCustomer cust = clsCustomer::Find(_currentCustomerID);
    int totalOrders = 0, completedOrders = 0;

    for (const auto& o : clsOrder::GetOrdersList()) {
        if (o.CustomerID() == _currentCustomerID) {
            totalOrders++;
            if (o.Status() == clsOrder::enStatus::Delivered) completedOrders++;
        }
    }

    if (lblCustTotalOrders) lblCustTotalOrders->setText(QString::number(totalOrders));
    if (lblCustCompletedOrders) lblCustCompletedOrders->setText(QString::number(completedOrders));
    if (lblCustPoints) lblCustPoints->setText(QString::number(cust.Points()));

    // Recent Orders (last 5)
    if (tableCustRecentOrders) {
        tableCustRecentOrders->setRowCount(0);
        vector<clsOrder> all = clsOrder::GetOrdersList();
        int count = 0;
        for (int i = (int)all.size() - 1; i >= 0 && count < 5; i--) {
            if (all[i].CustomerID() == _currentCustomerID) {
                int r = tableCustRecentOrders->rowCount();
                tableCustRecentOrders->insertRow(r);
                tableCustRecentOrders->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(all[i].OrderID())));
                clsProduct p = clsProduct::Find(all[i].ProductID());
                tableCustRecentOrders->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(p.IsEmpty() ? all[i].ProductID() : p.Name())));
                tableCustRecentOrders->setItem(r, 2, new QTableWidgetItem(QString::number(all[i].Quantity())));
                tableCustRecentOrders->setItem(r, 3, new QTableWidgetItem(QString::number(all[i].TotalPrice(), 'f', 2)));
                tableCustRecentOrders->setItem(r, 4, new QTableWidgetItem(QString::fromStdString(all[i].OrderDate())));
                QString status;
                switch (all[i].Status()) {
                    case 1: status = tr("Pending"); break;
                    case 2: status = tr("Confirmed"); break;
                    case 3: status = tr("Processing"); break;
                    case 4: status = tr("Delivered"); break;
                    case 5: status = tr("Cancelled"); break;
                }
                tableCustRecentOrders->setItem(r, 5, new QTableWidgetItem(status));
                count++;
            }
        }
    }

    // Points screen
    if (lblCustPointsDisplay) {
        lblCustPointsDisplay->setText(tr("Current Points: %1").arg(cust.Points()));
        QString rank = tr("Bronze");
        if (cust.Points() >= 5000) rank = tr("Platinum");
        else if (cust.Points() >= 2000) rank = tr("Gold");
        else if (cust.Points() >= 500) rank = tr("Silver");
        lblCustRewardLevel->setText(tr("Reward Level: %1").arg(rank));
    }

    // Profile screen
    if (lblCustProfName) {
        lblCustProfName->setText(QString::fromStdString(cust.FirstName()));
        lblCustProfPhone->setText(QString::fromStdString(cust.PhoneNumber()));
        lblCustProfEmail->setText(QString::fromStdString(_currentUser.Email()));
        lblCustProfAddr->setText(QString::fromStdString(cust.Address()));
        QString type;
        switch (cust.CustomerType()) {
            case 1: type = tr("Farmer"); break;
            case 2: type = tr("Breeder"); break;
            case 3: type = tr("Company"); break;
        }
        lblCustProfType->setText(type);
    }
}

void MainWindow::loadCustOrdersData()
{
    if (tableCustOrders == nullptr) return;
    tableCustOrders->setRowCount(0);

    for (const auto& o : clsOrder::GetOrdersList()) {
        if (o.CustomerID() != _currentCustomerID) continue;
        if (_custOrderFilter != -1 && o.Status() != _custOrderFilter) continue;
        int r = tableCustOrders->rowCount();
        tableCustOrders->insertRow(r);
        tableCustOrders->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(o.OrderID())));
        clsProduct p = clsProduct::Find(o.ProductID());
        tableCustOrders->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(p.IsEmpty() ? o.ProductID() : p.Name())));
        tableCustOrders->setItem(r, 2, new QTableWidgetItem(QString::number(o.Quantity())));
        tableCustOrders->setItem(r, 3, new QTableWidgetItem(QString::number(o.TotalPrice(), 'f', 2)));
        tableCustOrders->setItem(r, 4, new QTableWidgetItem(QString::fromStdString(o.OrderDate())));
        QString status;
        switch (o.Status()) {
            case 1: status = tr("Pending"); break;
            case 2: status = tr("Confirmed"); break;
            case 3: status = tr("Processing"); break;
            case 4: status = tr("Delivered"); break;
            case 5: status = tr("Cancelled"); break;
        }
        tableCustOrders->setItem(r, 5, new QTableWidgetItem(status));
    }
}

void MainWindow::openNewDeliveryForm()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Log New Delivery"));
    dialog.setMinimumWidth(300);
    dialog.setStyleSheet(
        "QLabel { color: #1c4a78; font-weight: bold; margin-top: 5px; }"
        "QComboBox { padding: 4px; border: 1px solid #bdc3c7; border-radius: 4px; }"
        "QSpinBox { padding: 4px; border: 1px solid #bdc3c7; border-radius: 4px; }"
        "QSpinBox::up-button, QSpinBox::down-button {"
        "  width: 20px; border: 1px solid #bdc3c7;"
        "  background-color: #f0f0f0; border-radius: 2px;"
        "}"
    );
    // ----------------------------------------------------
    QVBoxLayout layout(&dialog);

    // Form Inputs
    QComboBox *cmbBoneType = new QComboBox(&dialog);
    cmbBoneType->addItems({tr("Cow Bones"), tr("Sheep Bones"), tr("Chicken Bones"), tr("Mixed")});

    QSpinBox *spinQuantity = new QSpinBox(&dialog);
    spinQuantity->setRange(1, 10000); // 1kg to 10,000kg
    spinQuantity->setSuffix(tr(" kg"));

    QPushButton *btnSubmit = new QPushButton(tr("Submit Delivery"), &dialog);
    btnSubmit->setStyleSheet("background-color: #27ae60; color: white; font-weight: bold; padding: 8px;");

    layout.addWidget(new QLabel(tr("Bone Type:")));
    layout.addWidget(cmbBoneType);
    layout.addWidget(new QLabel(tr("Quantity:")));
    layout.addWidget(spinQuantity);
    layout.addWidget(btnSubmit);

    // What happens when they click submit
   // What happens when they click submit
    connect(btnSubmit, &QPushButton::clicked, [&dialog, cmbBoneType, spinQuantity, this]() {

        // 1. Generate Data
        string deliveryID = "DEL-" + to_string(QDateTime::currentMSecsSinceEpoch());
        string date = QDateTime::currentDateTime().toString("yyyy-MM-dd").toStdString();

        // 2. Create the object and set the data
        clsDelivery newDelivery = clsDelivery::GetAddNewDeliveryObject(deliveryID);
        newDelivery.SetSupplierID(_currentSupplierID);
        newDelivery.SetProductID(cmbBoneType->currentText().toStdString());
        newDelivery.SetQuantity(spinQuantity->value());
        newDelivery.SetDate(date);
        newDelivery.SetStatus(clsDelivery::enStatus::Pending);

        // 3. Save it to Data/Deliveries.txt
        newDelivery.Save();

        // 4. Notify user and close popup
        QMessageBox::information(&dialog, tr("Success"), tr("Delivery logged! It is now pending Admin approval."));
        dialog.accept();

        // 5. Refresh the UI tables
        refreshSupplierDashboard();
        refreshMyDeliveriesTable(); // We will create this next!
    });

    dialog.exec(); // Show the popup
}
