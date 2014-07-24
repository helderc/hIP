#include "aba.h"

Aba::Aba(QWidget *parent) :
    QWidget(parent)
{
    // Layout "mae" do widget
    QVBoxLayout *vertLayout = new QVBoxLayout(this);
    vertLayout->setObjectName(QStringLiteral("verticalLayout"));
    vertLayout->setContentsMargins(0,0,0,0);

    // Area de Texto: Criacao e configuracao
    textEdit = new QPlainTextEdit(this);

    QFont font("Monospace");
    font.setStyleHint(QFont::Monospace);
    font.setPointSize(12);
    textEdit->setFont(font);
    textEdit->setReadOnly(true);

    vertLayout->addWidget(textEdit);

    QHBoxLayout *horLayout = new QHBoxLayout();
    horLayout->setObjectName(QStringLiteral("horizontalLayout"));
    horLayout->setContentsMargins(0,0,0,5);

    btnSalvar = new QPushButton(QIcon(":/new/icones/icones/document-save.png"),
                                "Salvar .csv",
                                this);

    // Espacador par deixar o botao no canto direito
    QSpacerItem *horSpacer = new QSpacerItem(0, 0,
                                             QSizePolicy::Expanding,
                                             QSizePolicy::Minimum);
    horLayout->addItem(horSpacer);
    horLayout->addWidget(btnSalvar);

    vertLayout->addLayout(horLayout);

}

void Aba::inserirTexto(QString texto)
{
    this->textEdit->appendPlainText(texto);
}
