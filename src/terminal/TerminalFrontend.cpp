#include "terminal/TerminalFrontend.hpp"
#include "MainWindow.hpp"

using namespace openide::terminal;

TerminalFrontend::TerminalFrontend(MainWindow* parent) : QAbstractScrollArea(parent->getCentralWidget()) {}
