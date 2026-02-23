#include "CollapsibleGroupBox.h"

CollapsibleGroupBox::CollapsibleGroupBox( QWidget *parent,bool expanded)
    :  isCollapsed(!expanded) {

    // Создаем кнопку для сворачивания
    setCheckable(true);
    setChecked(expanded); // Изначально развернута

    //Запоминаем исходную высоту
    originalHeight = sizeHint().height();
    setStyleSheet("QGroupBox::indicator { width: 0px; height: 0px; }");
    connect(this, &QGroupBox::toggled, this, &CollapsibleGroupBox::toggleCollapse);
}

void CollapsibleGroupBox::toggleCollapse() {
    if (!isCollapsed) {
        // Сворачиваем
        originalHeight = height();
        setFixedHeight(25); // Только заголовок
        isCollapsed = true;
    } else {
        // Разворачиваем
        setFixedHeight(originalHeight);
        setMaximumHeight(QWIDGETSIZE_MAX);
        isCollapsed = false;
    }
}

int CollapsibleGroupBox::getContentHeight() const {
    return height();
}

void CollapsibleGroupBox::setContentHeight(int height) {
    setFixedHeight(height);
}
