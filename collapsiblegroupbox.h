#ifndef COLLAPSIBLEGROUPBOX_H
#define COLLAPSIBLEGROUPBOX_H

#include <QGroupBox>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QLayout>

class CollapsibleGroupBox : public QGroupBox {
    Q_OBJECT
    Q_PROPERTY(int contentHeight READ getContentHeight WRITE setContentHeight)

public:
    explicit CollapsibleGroupBox(QWidget *parent = nullptr,bool expanded = true);

public slots:
    void toggleCollapse();


private:
    int getContentHeight() const;
    void setContentHeight(int height);

    QWidget *contentWidget;
    QPropertyAnimation *animation;
    bool isCollapsed;
    int originalHeight;
};

#endif // COLLAPSIBLEGROUPBOX_H
