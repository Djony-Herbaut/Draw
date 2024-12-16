# IDE Draw++

## Description
**IDE Draw++** is a simple text-based Integrated Development Environment (IDE) built using **GTK+ 3**. It allows users to:

- Create, open, and save text files.
- Display logs and console output.
- Visualize graphical content in a dedicated drawing area.

This project demonstrates basic usage of GTK for creating graphical user interfaces and integrates features like a text editor, file handling, and a drawing area.

---

## Features
- **Text Editor**: Edit and manage text files with basic functionalities.
- **Console/Logs**: View logs, errors, and outputs.
- **Drawing Area**: Dedicated space for graphical content or visual outputs.
- **Menu Bar**: Easy access to file operations such as "New", "Open", and "Save".

---

## Prerequisites
To compile and run the project, you need the following libraries and tools:

- **GTK+ 3** (development files)
- **gcc** (GNU Compiler Collection)
- **pkg-config** (to manage GTK dependencies)

---

## Installation on Linux

1. Install the required libraries and tools:
   ```bash
   sudo apt update
   sudo apt install libgtk-3-dev gcc pkg-config
   ```

2. Clone the project repository:
   ```bash
   git clone <repository-url>
   cd <project-folder>
   ```

3. Compile the project:
   ```bash
    gcc -o ide ide.c `pkg-config --cflags --libs gtk+-3.0`
   ```

4. Run the executable:
   ```bash
   ./ide
   ```

---

## Usage
- **New File**: Clear the editor to start a new file.
- **Open File**: Load a text file into the editor.
- **Save File**: Save the content of the editor to a file.
- **Console/Logs**: Monitor logs and messages.
- **Drawing Area**: Visualize graphical content.

---
