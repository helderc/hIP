/***************************************************************************

Copyright (c) 2014, Helder C. R. de Oliveira
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* Neither the name of the {organization} nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***************************************************************************/

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
