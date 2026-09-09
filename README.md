# Bone Fertilizer OS

Enterprise desktop application for managing bone fertilizer operations, built with **C++17**, **Qt6**, and **SQLite**.

## Features

- **Multi-Role System** — Admin, Supplier, and Customer roles, each with a dedicated portal and role-based permissions
- **Dashboard** — Real-time overview of suppliers, customers, products, orders, revenue, deliveries, and low-stock alerts
- **Supplier Management** — Full CRUD, reward points, rankings, and delivery tracking
- **Customer Management** — Profile management, order history, and loyalty points
- **Product & Inventory** — Stock management with low-stock alerts and category filtering
- **Order Tracking** — Create, track, and cancel orders with a status workflow (Pending → Processing → Delivered)
- **Delivery Approval** — Suppliers submit deliveries; admin approves or rejects with automatic stock updates
- **Reporting** — Sales, inventory, supplier, customer, and delivery reports
- **Article Management** — Create and read industry articles (REOSSA bone fertilizer content)
- **Dark / Light Themes** — Toggle between `dark.qss` and `light.qss` stylesheets
- **English & Arabic** — Full internationalization via Qt Linguist (`bonechain_en.ts`, `bonechain_ar.ts`)

## Tech Stack

| Layer | Technology |
|-------|-----------|
| Language | C++17 |
| GUI Framework | Qt6 (Widgets, Sql, Svg) |
| Database | SQLite (via `QSqlDatabase`) |
| Build System | CMake 3.16+ |
| Installer | NSIS (Nullsoft Scriptable Install System) |
| Toolchain | MinGW-w64 (GCC) |

## Project Structure

```
Bone_Chain_APP/
├── Core/                  # Business logic layer (header-only model classes)
│   ├── clsDatabase.h      # SQLite singleton (AppData auto-seed)
│   ├── clsUser.h          # User model
│   ├── clsSupplier.h      # Supplier model
│   ├── clsCustomer.h      # Customer model
│   ├── clsProduct.h       # Product model
│   ├── clsOrder.h         # Order model
│   ├── clsDelivery.h      # Delivery model
│   ├── clsArticle.h       # Article model
│   ├── clsInventory.h     # Stock management
│   ├── clsRewardSystem.h  # Supplier points & ranking
│   └── clsReport.h        # Dashboard statistics
├── GUI/                   # Qt6 Widgets UI layer
│   ├── main.cpp           # Entry point
│   ├── mainwindow.cpp/h   # 20-screen stacked widget
│   ├── ThemeManager.cpp/h # Theme switching & persistence
│   └── *dialog.cpp/h      # CRUD dialogs (User, Supplier, Customer, Product, Order, Delivery, Article)
├── Data/                  # Seed database & text files
│   ├── BoneChain.db       # SQLite database
│   └── *.txt              # Migration seed data
├── Themes/                # QSS stylesheets (dark.qss, light.qss)
├── Assets/                # app_icon.ico, REOSSA logo
├── i18n/                  # Qt Linguist translation files
├── CMakeLists.txt         # Build & package configuration
├── installer.nsi          # NSIS installer script
└── build_and_package.bat  # One-click build + package script
```

## Build Instructions

### Prerequisites

- [Qt 6.8+](https://www.qt.io/download) with MinGW (or MSVC) kit
- CMake 3.16+
- NSIS 3.x (installer packaging only)

### Build with CMake

```bash
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Build & Package (Windows one-click)

```bat
build_and_package.bat
```

### Build the NSIS installer manually

```bash
& "C:\Program Files (x86)\NSIS\makensis.exe" installer.nsi
```

## Installation

1. Download `BoneFertilizerApp-1.0.0-Setup.exe`
2. Run the installer (requires admin privileges)
3. Launch the app from the Start Menu or Desktop shortcut

On first run, a writable copy of the database is created in `%APPDATA%` and auto-seeded from the bundled `Data/` files.

## Test Accounts

| Username  | Password | Role     |
|-----------|----------|----------|
| `admin`   | `1234`   | Admin    |
| `supplier`| `1234`   | Supplier |
| `customer`| `1234`   | Customer |
| `sarah`   | `1234`   | Admin    |

## Role-Based Access

| Feature             | Admin | Supplier | Customer |
|---------------------|:-----:|:--------:|:--------:|
| Dashboard           | ✅ | ✅ | ✅ |
| User Management     | ✅ | — | — |
| Supplier Management | ✅ | — | — |
| Customer Management | ✅ | — | — |
| Product Browsing    | ✅ | — | ✅ |
| Order Management    | ✅ | — | ✅ |
| Inventory           | ✅ | — | — |
| Reports             | ✅ | — | — |
| Articles            | ✅ | — | — |
| Deliveries          | ✅ | ✅ | — |
| Rewards / Points    | — | ✅ | ✅ |
| Profile             | — | ✅ | ✅ |

## Screenshots

> Coming soon.

## License

This project is developed for **REOSSA**. All rights reserved.
