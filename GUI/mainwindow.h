#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QComboBox>
#include <QTabWidget>
#include <QProgressBar>
#include <QFont>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QIcon>
#ifdef HAS_QT_SVG
#include <QSvgRenderer>
#endif
#include <QApplication>
#include <string>
#include "../Core/clsUser.h"
#include "../Core/clsSupplier.h"
#include "../Core/clsCustomer.h"
#include "../Core/clsProduct.h"
#include "../Core/clsOrder.h"

using namespace std;

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Navigation & Auth
    void handleLogin();
    void logout();
    void navigateToScreen(int index);

    // Data Management Slots
    void refreshDashboard();
    void loadUsersData();
    void deleteSelectedUser();
    void loadSuppliersData();
    void deleteSelectedSupplier();
    void loadCustomersData();
    void deleteSelectedCustomer();
    void loadProductsData();
    void deleteSelectedProduct();
    void loadOrdersData();
    void cancelSelectedOrder();
    void loadInventoryData();
    void loadArticlesData();
    void deleteSelectedArticle();
    void refreshMyRewardsScreen();

private:
        void switchLanguage();
        void toggleTheme();

    // --- Core Layout Components ---
    QWidget *centralWidget;
    QHBoxLayout *mainLayout;

    QWidget *sidebarWidget;
    QVBoxLayout *sidebarLayout;
    QStackedWidget *stackedScreens;

    // --- Sidebar Buttons ---
    QPushButton *btnDashboard, *btnUsers, *btnSuppliers, *btnCustomers;
    QPushButton *btnProducts, *btnOrders, *btnInventory, *btnReports;
    QPushButton *btnArticles, *btnLogout;
    QPushButton *btnDeliveries;
    // --- Supplier Navigation Buttons ---
    QPushButton *btnSupDashboard;
    QPushButton *btnMyDeliveries;
    QPushButton *btnMyRewards;
    QPushButton *btnMyProfile;
    // --- Customer Navigation Buttons ---
    QPushButton *btnCustDashboard;
    QPushButton *btnCustProducts;
    QPushButton *btnCustOrders;
    QPushButton *btnCustPoints;
    QPushButton *btnCustProfile;


    enum ScreenIndex {
        Login = 0, Dashboard, Users, Suppliers, Customers,
        Products, Orders, Inventory, Reports, Articles, Deliveries,
        SupDashboard, MyDeliveries, MyRewards, MyProfile,
        CustDashboard, CustProducts, CustOrders, CustPoints, CustProfile
    };

    // --- Core Setup Functions ---
    void setupUi();
    void setupSidebar();
    QPixmap createLogo(int size);
    void updateLoginThemeButton(QPushButton *btn);
    void refreshAllStyles();
    QTableWidget* createStandardTable(QStringList headers); // UI Optimization Helper
    void applyRolePermissions(int role);
    // --- The 11 Screens ---
    void setupLoginScreen();
    void setupDashboardScreen();
    void setupUsersScreen();
    void setupSuppliersScreen();
    void setupCustomersScreen();
    void setupProductsScreen();
    void setupOrdersScreen();
    void setupInventoryScreen();
    void setupReportsScreen();
    void setupArticlesScreen();
    void loadAllReportsData();
    void setupDeliveriesScreen();
    void loadDeliveriesData();
    void approveSelectedDelivery();
    void rejectSelectedDelivery();
    // --- UI Elements ---
    QLineEdit *txtUsername, *txtPassword;
    QLabel *lblRevenue, *lblBones, *lblTotalUsers, *lblTotalOrders,*lblTotalDeliveries;

    // --- Supplier Portal ---
    void setupSupplierDashboardScreen();
    void setupMyDeliveriesScreen();
    void setupMyRewardsScreen();
    void setupMyProfileScreen();
    void refreshSupplierDashboard();
    void openNewDeliveryForm();
    void refreshMyDeliveriesTable();
    void refreshMyProfileScreen();

    // --- Customer Portal ---
    void setupCustDashboardScreen();
    void setupCustProductsScreen();
    void setupCustOrdersScreen();
    void setupCustPointsScreen();
    void setupCustProfileScreen();
    void loadCustDashboardData();
    void loadCustOrdersData();

    // Supplier Dashboard Metrics
    QLabel *lblSupDashTotalDeliveries;
    QLabel *lblSupDashTotalBones;
    QLabel *lblSupDashPoints;
    QLabel *lblSupDashRank;

    QLabel *lblRewardPoints;
    QLabel *lblRewardRank;
    QProgressBar *barNextRank; // This needs #include <QProgressBar> at the top of your file!
    QFrame *certFrames[3];

    // --- Profile UI Variables ---
    QLabel *lblProfileName;
    QLabel *lblProfileUsername;
    QLabel *lblProfilePhone;
    QLabel *lblProfileEmail;
    QLabel *lblProfileRole;


    QTableWidget *tableUsers;
    QTableWidget *tableSuppliers;
    QTableWidget *tableCustomers;
    QTableWidget *tableProducts;
    QTableWidget *tableOrders;
    QTableWidget *tableInventory;
    QTableWidget *tableArticles;
    QTableWidget *tableDeliveries;
    // --- Dashboard Elements ---
    QLabel *lblDashSuppliers, *lblDashCustomers, *lblDashProducts;
    QLabel *lblDashOrders, *lblDashBones, *lblDashRevenue,*lblDashDeliveries;

    QPushButton *btnQuickAddSupplier, *btnQuickAddProduct;
    QPushButton *btnQuickAddArticle, *btnQuickReports;

    // Article Control Buttons
    QPushButton *btnAdd;
    QPushButton *btnEdit;
    QPushButton *btnDel;

    QTableWidget *tableDashLowStock;

    // --- Add this near your other table pointers ---
    QComboBox *cmbInventoryFilter;


    // --- Reports Dashboard Elements ---
    QTabWidget *tabReports;
    QTableWidget *tableRepSales;
    QTableWidget *tableRepInventory;
    QTableWidget *tableRepSuppliers;
    QTableWidget *tableRepCustomers;
    QTableWidget *tableRepDeliveries;

    // 3. Data Tables(SUPPLIER)
    QTableWidget *tableSupRecentDeliveries;
    QTableWidget *tableSupMyDeliveries;

    clsUser _currentUser = clsUser::GetEmptyUserObject();
    string _currentSupplierID;
    string _currentCustomerID;
    int _currentRole = 0;

    // --- Customer Portal Members ---
    QLabel *lblCustTotalOrders, *lblCustCompletedOrders, *lblCustPoints;
    QTableWidget *tableCustRecentOrders;
    QTableWidget *tableCustProducts;
    QLineEdit *txtCustSearch;
    QComboBox *cmbCustCategoryFilter;
    QTableWidget *tableCustOrders;
    QLabel *lblCustPointsDisplay, *lblCustRewardLevel;
    QLabel *lblCustProfName, *lblCustProfPhone, *lblCustProfEmail, *lblCustProfAddr, *lblCustProfType;
    int _custOrderFilter = -1; // -1 = All, 1 = Pending, 3 = Processing, 4 = Delivered, 5 = Cancelled
};
