/********************************************************************************
** Form generated from reading UI file 'serialsettings.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERIALSETTINGS_H
#define UI_SERIALSETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SerialSettings
{
public:
    QGridLayout *gridLayout_2;
    QDialogButtonBox *Ok_Cancel_box;
    QGroupBox *Parameters;
    QGridLayout *gridLayout_3;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QSpacerItem *horizontalSpacer_2;
    QVBoxLayout *verticalLayout_5;
    QComboBox *speedBox;
    QComboBox *dataBitBox;
    QComboBox *parityBox;
    QComboBox *stopBitBox;
    QLabel *picture;
    QGroupBox *Serial;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout;
    QComboBox *SerialSelect;
    QPushButton *UpdateAvaiblePorts;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QSpacerItem *horizontalSpacer_3;
    QVBoxLayout *verticalLayout_2;
    QLabel *SerialDescription;
    QLabel *SerialManufacturer;
    QLabel *SerialSN;

    void setupUi(QDialog *SerialSettings)
    {
        if (SerialSettings->objectName().isEmpty())
            SerialSettings->setObjectName(QStringLiteral("SerialSettings"));
        SerialSettings->resize(299, 303);
        gridLayout_2 = new QGridLayout(SerialSettings);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        Ok_Cancel_box = new QDialogButtonBox(SerialSettings);
        Ok_Cancel_box->setObjectName(QStringLiteral("Ok_Cancel_box"));
        Ok_Cancel_box->setOrientation(Qt::Vertical);
        Ok_Cancel_box->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout_2->addWidget(Ok_Cancel_box, 0, 2, 1, 1);

        Parameters = new QGroupBox(SerialSettings);
        Parameters->setObjectName(QStringLiteral("Parameters"));
        gridLayout_3 = new QGridLayout(Parameters);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        label_4 = new QLabel(Parameters);
        label_4->setObjectName(QStringLiteral("label_4"));

        verticalLayout_4->addWidget(label_4);

        label_5 = new QLabel(Parameters);
        label_5->setObjectName(QStringLiteral("label_5"));

        verticalLayout_4->addWidget(label_5);

        label_6 = new QLabel(Parameters);
        label_6->setObjectName(QStringLiteral("label_6"));

        verticalLayout_4->addWidget(label_6);

        label_7 = new QLabel(Parameters);
        label_7->setObjectName(QStringLiteral("label_7"));

        verticalLayout_4->addWidget(label_7);


        horizontalLayout_3->addLayout(verticalLayout_4);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        speedBox = new QComboBox(Parameters);
        speedBox->setObjectName(QStringLiteral("speedBox"));

        verticalLayout_5->addWidget(speedBox);

        dataBitBox = new QComboBox(Parameters);
        dataBitBox->setObjectName(QStringLiteral("dataBitBox"));

        verticalLayout_5->addWidget(dataBitBox);

        parityBox = new QComboBox(Parameters);
        parityBox->setObjectName(QStringLiteral("parityBox"));

        verticalLayout_5->addWidget(parityBox);

        stopBitBox = new QComboBox(Parameters);
        stopBitBox->setObjectName(QStringLiteral("stopBitBox"));

        verticalLayout_5->addWidget(stopBitBox);


        horizontalLayout_3->addLayout(verticalLayout_5);


        gridLayout_3->addLayout(horizontalLayout_3, 0, 0, 1, 1);


        gridLayout_2->addWidget(Parameters, 2, 0, 1, 2);

        picture = new QLabel(SerialSettings);
        picture->setObjectName(QStringLiteral("picture"));
        picture->setMaximumSize(QSize(70, 70));
        picture->setPixmap(QPixmap(QString::fromUtf8(":/new/Resources/004-engine.png")));
        picture->setScaledContents(true);

        gridLayout_2->addWidget(picture, 2, 2, 1, 1);

        Serial = new QGroupBox(SerialSettings);
        Serial->setObjectName(QStringLiteral("Serial"));
        gridLayout = new QGridLayout(Serial);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        SerialSelect = new QComboBox(Serial);
        SerialSelect->setObjectName(QStringLiteral("SerialSelect"));

        horizontalLayout->addWidget(SerialSelect);

        UpdateAvaiblePorts = new QPushButton(Serial);
        UpdateAvaiblePorts->setObjectName(QStringLiteral("UpdateAvaiblePorts"));

        horizontalLayout->addWidget(UpdateAvaiblePorts);


        verticalLayout_3->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(Serial);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        label_2 = new QLabel(Serial);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout->addWidget(label_2);

        label_3 = new QLabel(Serial);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout->addWidget(label_3);


        horizontalLayout_2->addLayout(verticalLayout);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        SerialDescription = new QLabel(Serial);
        SerialDescription->setObjectName(QStringLiteral("SerialDescription"));

        verticalLayout_2->addWidget(SerialDescription);

        SerialManufacturer = new QLabel(Serial);
        SerialManufacturer->setObjectName(QStringLiteral("SerialManufacturer"));

        verticalLayout_2->addWidget(SerialManufacturer);

        SerialSN = new QLabel(Serial);
        SerialSN->setObjectName(QStringLiteral("SerialSN"));

        verticalLayout_2->addWidget(SerialSN);


        horizontalLayout_2->addLayout(verticalLayout_2);


        verticalLayout_3->addLayout(horizontalLayout_2);


        gridLayout->addLayout(verticalLayout_3, 0, 0, 1, 1);


        gridLayout_2->addWidget(Serial, 0, 0, 1, 2);


        retranslateUi(SerialSettings);

        QMetaObject::connectSlotsByName(SerialSettings);
    } // setupUi

    void retranslateUi(QDialog *SerialSettings)
    {
        SerialSettings->setWindowTitle(QApplication::translate("SerialSettings", "Dialog", nullptr));
        Parameters->setTitle(QApplication::translate("SerialSettings", "\320\237\320\260\321\200\320\260\320\274\320\265\321\202\321\200\321\213", nullptr));
        label_4->setText(QApplication::translate("SerialSettings", "\320\241\320\272\320\276\321\200\320\276\321\201\321\202\321\214", nullptr));
        label_5->setText(QApplication::translate("SerialSettings", "\320\221\320\270\321\202 \320\264\320\260\320\275\320\275\321\213\321\205", nullptr));
        label_6->setText(QApplication::translate("SerialSettings", "\320\247\320\265\321\202\320\275\320\276\321\201\321\202\321\214", nullptr));
        label_7->setText(QApplication::translate("SerialSettings", "\320\241\321\202\320\276\320\277\320\276\320\262\321\213\321\205 \320\261\320\270\321\202", nullptr));
        picture->setText(QString());
        Serial->setTitle(QApplication::translate("SerialSettings", "\320\241\320\265\321\200\320\270\320\271\320\275\321\213\320\271 \320\277\320\276\321\200\321\202", nullptr));
        UpdateAvaiblePorts->setText(QApplication::translate("SerialSettings", "\320\236\320\261\320\275\320\276\320\262\320\270\321\202\321\214", nullptr));
        label->setText(QApplication::translate("SerialSettings", "\320\236\320\277\320\270\321\201\320\260\320\275\320\270\320\265", nullptr));
        label_2->setText(QApplication::translate("SerialSettings", "\320\237\321\200\320\276\320\270\320\267\320\262\320\276\320\264\320\270\321\202\320\265\320\273\321\214", nullptr));
        label_3->setText(QApplication::translate("SerialSettings", "\320\241\320\265\321\200\320\270\320\271\320\275\321\213\320\271 \320\275\320\276\320\274\320\265\321\200", nullptr));
        SerialDescription->setText(QString());
        SerialManufacturer->setText(QString());
        SerialSN->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class SerialSettings: public Ui_SerialSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERIALSETTINGS_H
