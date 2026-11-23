# Media Library

A small Qt 6 / QML app for browsing a mock media library.

## Architecture

The UI is QML; the data model and logic are C++. The code has four layers:

- **QML UI** (`qml/`) – only shows data and sends user actions.
- **Interface** (`src/interface/`) – the only C++ that QML can see.
  - From UI: QML sends every action with one call, `UiEvents.post(message, data)`.
  - To UI: C++ updates read-only properties (`LibraryData.toUiXxx`, `LanguageData.toUiXxx`) and QML binds to them.
- **Logic** (`src/logic/`) – `LibraryManager` (search, filter, select) and `LanguageManager` (switch language).
- **Model** (`src/model/`) – `MediaLibraryModel` (`QAbstractListModel`, all items) and `MediaFilterModel` (`QSortFilterProxyModel`, the filtered list shown in QML).

The full architecture diagram is in [architecture.svg](architecture.svg).

## Features

- List of media items with title and creator
- Search by title or creator (case-insensitive; several words must all match)
- Filter by type (Movies, Music, Books, Podcasts, Others)
- Click an item to see its details
- Message when nothing matches, and a result count (e.g. "2 of 24 items")
- Switch between English and Swedish at runtime

## Build

### Prerequisites

| Tool | Version | Notes |
|---|---|---|
| Qt | 6.5 or later, MSVC 64-bit | Modules: Qt Quick, Qt Quick Controls 2, Qt Linguist Tools |
| Visual Studio | 2022 or 2026 | Workload: "Desktop development with C++" |
| CMake | 3.21 or later (4.2 or later for Visual Studio 2026) | A copy ships with Visual Studio |

The MSVC compiler does not need any environment setup; CMake locates Visual Studio automatically.

### Configuration

`build.bat` reads three build variables. Their defaults match the author's machine,
so **set them to the paths on your machine** before building.

| Variable | Description | Default (author's machine) |
|---|---|---|
| `QT_DIR` | Qt installation for MSVC 64-bit (the folder that contains `bin` and `lib`) | `C:/QtSS/6.12.0/msvc2022_64` |
| `VS_GENERATOR` | CMake generator for your Visual Studio version: `Visual Studio 18 2026` or `Visual Studio 17 2022` | `Visual Studio 18 2026` |
| `CMAKE_EXE` | Path to `cmake.exe`. Keep `cmake` if CMake is on `PATH` | `cmake` |

Each variable can be set in one of two ways:

- **Edit `build.bat`** – change the default values at the top of the file.
- **Environment variable** – set it in the terminal before running `build.bat`.
  An environment variable takes priority over the default in the file and only applies to that terminal session.

Example (PowerShell, run in this folder; replace the placeholders with your own paths):

```powershell
$env:QT_DIR       = "<Qt install dir>/<version>/msvc2022_64"
$env:VS_GENERATOR = "Visual Studio 17 2022"
$env:CMAKE_EXE    = "<VS install dir>\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
.\build.bat
```

The same in cmd:

```bat
set "QT_DIR=<Qt install dir>/<version>/msvc2022_64"
set "VS_GENERATOR=Visual Studio 17 2022"
set "CMAKE_EXE=<VS install dir>\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
build.bat
```

Only set the variables that differ from the defaults.
After changing `QT_DIR` or `VS_GENERATOR`, delete the `build` folder so CMake configures again.

## Run

Double-click `build.bat`. It builds the Release version and starts the app.

The app is `build\Release\appMediaLibrary.exe`. The build copies the Qt DLLs next to it, so it can also be started directly.

## Future improvements

- **Unit tests** – all logic is in the C++ library `mediacore`, which does not need QML.
  Qt Test can cover the JSON loader, search and filter rules, and the values `LibraryManager` publishes.
- **Load data in the background** – for a large library, load and parse the data in a background thread
  (e.g. `QtConcurrent::run` + `QFutureWatcher`), then call `MediaLibraryModel::setItems()` on the GUI thread
  and show a "loading" state meanwhile.
- **UI** – cover images or type icons, highlight the matched words in search results,
  keyboard navigation, and simple animations.
