#ifndef PANECONTAINER_HPP
#define PANECONTAINER_HPP

#include <QWidget>
#include <QTabWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QLabel>

class MainWindow;

namespace openide {
namespace code {

class CodeEditor;

class PaneContainer : public QWidget
{
    Q_OBJECT

public:
    enum class Type {
        Leaf,   // Contains a QTabWidget
        Branch  // Contains a QSplitter with two child PaneContainers
    };

    explicit PaneContainer(MainWindow* parent, Type type = Type::Leaf);
    ~PaneContainer();

    // Type queries
    Type type() const { return m_type; }
    bool isLeaf() const { return m_type == Type::Leaf; }
    bool isBranch() const { return m_type == Type::Branch; }

    // Leaf operations
    QTabWidget* tabWidget() const { return m_tabWidget; }
    int tabCount() const;
    void addTab(QWidget* widget, const QString& label);
    void removeTab(int index);
    QWidget* widget(int index) const;
    QString tabText(int index) const;
    void setTabText(int index, const QString& text);
    int currentIndex() const;
    void setCurrentIndex(int index);
    QWidget* currentWidget() const;
    void setPaneNumber(int number);

    // Branch operations
    QSplitter* splitter() const { return m_splitter; }
    PaneContainer* leftChild() const { return m_leftChild; }
    PaneContainer* rightChild() const { return m_rightChild; }

    // Split operations (converts Leaf to Branch)
    void splitHorizontal();
    void splitVertical();

    // Tree traversal
    void getAllTabWidgets(QList<QTabWidget*>& widgets);
    void getAllLeafContainers(QList<PaneContainer*>& containers);
    PaneContainer* findContainerWithTabWidget(QTabWidget* tabWidget);

    // Check if this container or any child is empty and can be removed
    bool isEmpty() const;
    void simplify(); // Remove empty children and convert single-child branches to leaves

signals:
    void tabWidgetCreated(QTabWidget* tabWidget);
    void containerEmpty(PaneContainer* container);

private:
    void convertToLeaf();
    void convertToBranch(Qt::Orientation orientation);

    MainWindow* m_parent;
    Type m_type;
    QVBoxLayout* m_layout;

    // Leaf members
    QTabWidget* m_tabWidget;
    QLabel* m_paneLabel;

    // Branch members
    QSplitter* m_splitter;
    PaneContainer* m_leftChild;
    PaneContainer* m_rightChild;
};

} // namespace code
} // namespace openide

#endif // PANECONTAINER_HPP

