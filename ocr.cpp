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

#include "ocr.h"

OCR::OCR(QObject *parent) :
    QObject(parent)
{
    subImgLinhas = new QList<QImage>();
    subImgCaracteres = new QList< QList<QImage> >();
    listaCaracteres = new QList< QVector<double>* >();

    metodoExtracao = NENHUM;

    // lista das caracteristicas coletadas. Eh usado para mostrar na interface os resultados
    caracteristicas = NULL;
}



void OCR::analisePerfilProjecao(const QImage &img)
{
    /**
     *
     * 1 - Calculando o histograma das linhas
     *
     **/

    // Calculando o histograma das Linhas
    linhas = perfilProjecao(img, 0);

    // Fatiando a imagem, extraindo subimagens que sao as linhas
    subImgLinhas = subImagens(img, *linhas, 0);

    //qDebug() << "Total de linhas detectadas: " << subImgLinhas->length();


    /**
      *
      * 2 - Separando os caracteres de cada linha
      *
      **/

    // Calculando o perfil de cada linha
    for (int i = 0; i < subImgLinhas->size(); ++i) {
        listaCaracteres->append(perfilProjecao(subImgLinhas->at(i), 1));
    }


    // O tamanho de listaCaracteres corresponde a quantidade de linhas
    //qDebug() << "Perfil de cada linha: " << listaCaracteres->size();

    // varrendo a lista de histogramas para fatiar as imagens.
    for (int i = 0; i < listaCaracteres->size(); ++i) {
        // O histograma que estah em listaCaracteres.at(i) pertence a imagem subImgLinhas.at(i)
        subImgCaracteres->append(*subImagens(subImgLinhas->at(i), *listaCaracteres->at(i), 1));
    }


    /**
      * 3 - Normalizando em uma Malha 70x50 (Tamanho especificado no artigo)
      *
      **/
    normalizar(*subImgCaracteres, 50, 70);

    int x = 0;
    foreach (QList<QImage> l, *subImgCaracteres) {
        foreach (QImage img, l) {

            // Controla se o arquivo ja existe, para nao perder nenhum imagem
            QString id, f_name;
            QFileInfo f_info;

            do {
                id = QString::number(++x);
                //f_name = "C:/alfabeto/" + id + " img.png";
                f_name = "/home/helder/alfabeto_no_norm/" + id + " img.png";
                f_info = QFileInfo(f_name);

            } while (f_info.exists());

            bool salvo = img.save(f_name, "PNG");

            // Cria lista de caracteres
            listaOCRCaractere.append(new OCRCaractere(img, id));

            if (salvo) qDebug() << "Salvo";
            else qDebug() << "NAO Salvo";

            // TODO: Atualizar o histograma dos caracteres: listaCaracteres.indexOf(l);
        }
    }
}




/**
 * @brief Fraciona uma imagem "img" baseando se em seu histograma "v".
 *
 * @param img imagem a ser fracionada.
 * @param v Histograma da imagem.
 * @param horientacao Aqui deve se indicar se o perfil sera horizontal | (0 - Extrai linhas) ou vertical _ (1 - Extrai caracteres).
 *
 * @return Lista das subimagens de img.
 */
QList<QImage> *OCR::subImagens(const QImage &img, const QVector<double> &v, bool orientacao)
{
    QList<QImage> * listaImg = new QList<QImage>();

    int inicio = 0;
    int fim = 0;
    int d;

    // varre o histograma para separar as linhas
    for (int i = 0; i < v.size(); ++i) {
        d = v.at(i);

        // verifica inicio e fim de cada linha
        if (d != 0) {
            if (inicio == 0) inicio = i;
            else fim = i;
        }
        else
        // quando ja possui inicio e fim, extrai aquela faixa da imagem original
        if (inicio != 0 && fim != 0) {
            // self.img.copy(QRect(1, linInicio, self.img.width(), linFim - linInicio)))

            // Verifica se esta extraindo linhas ou caracteres
            QImage sub;
            // CARACTERES
            if (orientacao)
                sub = img.copy(inicio, 0, abs(fim - inicio), img.height());
            // LINHAS
            else
                sub = img.copy(0, inicio, img.width(), abs(fim - inicio));

            listaImg->append(sub);

            //qDebug() << "Tamanho: " << fim - inicio << "   " << sub.width() << "x" << sub.height();
            //bool salvo = sub.save("C://Teste//" + QString::number(orientacao) + " img" + QString::number(++offset) + ".png", "PNG");

            //if (salvo) qDebug() << "Salvo";
            //else qDebug() << "NAO Salvo";

            inicio = fim = 0;
        }
    }

    return listaImg;
}




/**
 * @brief OCR::subCaracteresImagens
 * @param img
 * @param v
 * @param h
 * @return
 */
QImage OCR::subCaracteresImagens(const QImage &img,
                                 const QVector<double> &histVert,
                                 const QVector<double> &histHor)
{
    int XInicio = 0, XFim = 0;
    int YInicio = 0, YFim = 0;
    int valor;

    // varre o histograma VERTICAL em busca dos limites da LINHA
    for (int i = 0; i < histVert.size(); ++i) {
        valor = histVert.at(i);

        // verifica inicio e fim da LINHA
        if (valor != 0) {
            if (XInicio == 0)
                XInicio = i;
            else
                XFim = i;
        }
    }


    // varre o histograma HORIZONTAL em busca dos limites da COLUNA
    for (int i = 0; i < histHor.size(); ++i) {
        valor = histHor.at(i);

        // verifica inicio e fim da LINHA
        if (valor != 0) {
            if (YInicio == 0)
                YInicio = i;
            else
                YFim = i;
        }
    }

    QImage subImg = img.copy(QRect(QPoint(XInicio, YInicio), QPoint(XFim, YFim)));

    //bool salvo = subImg.save("/home/helder/" + QString::number(XInicio +XFim) + " img" + QString::number(YFim) + ".png", "PNG");

    //if (salvo) qDebug() << "Salvo";
    //else qDebug() << "NAO Salvo";

    return subImg;
}




/**
 * @brief Calcula o perfil de projecao (histograma) horizontal | ou vertical _.
 *
 * @param img Imagem a ser analisada.
 * @param horientacao Aqui deve se indicar se o perfil sera horizontal (0) ou vertical (1).
 *
 * @return QVector<double> Histograma resultante da analise.
 */
QVector<double> * OCR::perfilProjecao(const QImage &img, bool orientacao)
{
    QVector<double> *perfil = new QVector<double>();
    int h = img.height();
    int w = img.width();

    // perfil vertical _
    if (orientacao) {
        // varredura da imagem
        for (int x = 0; x < w; x++) {
            double contagem = 0;
            for (int y = 0; y < h; y++) {
                contagem += qGray(img.pixel(x, y)) > 128 ? 0: 1;
            }

            perfil->append(contagem);
        }

        return perfil;
    }

    // perfil horizontal |
    //qDebug() << "V: " << perfil->size();

    // varredura da imagem
    for (int y = 0; y < h; y++) {
        double contagem = 0;
        for (int x = 0; x < w; x++) {
            contagem += qGray(img.pixel(x, y)) > 128 ? 0: 1;
        }

        perfil->append(contagem);
    }

    return perfil;
}




/**
 * @brief Normaliza os caracteres da lista. Ao fim, todos caracteres estarao
 *          cortados, onde sera removido as partes em branco e irao ter
 *          o tamanho largura: w e altura: h.
 *
 * @param img listaCaracteres Lista dos caracteres a ser processada.
 * @param w Largura da nova imagem do caractere.
 * @param h Altura da nova imagem do caractere.
 *
 */
void OCR::normalizar(QList<QList<QImage> > &listaImgCaracteres, int w, int h)
{
    //Thinning *thin = new Thinning();

    // OBS: O uso de iteradores eh necessario para pegar os ponteiros
    // Iterando sobre a Lista das Linhas
    QList<QList<QImage> >::iterator l;
    for (l = listaImgCaracteres.begin(); l != listaImgCaracteres.end(); ++l) {

        // Iterando sobre a Linha
        QList<QImage>::iterator l_img;
        for (l_img = (*l).begin(); l_img != (*l).end(); ++l_img) {

            // Perfil de projecao HORIZONTAL |
            QVector<double> *histogramaH = perfilProjecao((*l_img), 0);

            // Perfil de projecao VERTICAL _
            QVector<double> *histogramaV = perfilProjecao((*l_img), 1);

            // Extracao das areas em Branco da Imagem
            QImage imgTemp = subCaracteresImagens((*l_img), *histogramaV, *histogramaH);

            // Redimensiona a Imagem
            imgTemp = imgTemp.scaled(w, h);

            // Esqueletiza o caractere
            //thin->ZhanSuen(&imgTemp);

            // Altera o antigo
            (*l_img) = imgTemp;

            // TODO: Atualizar o histograma dos caracteres: listaCaracteres.indexOf(l);
            // listaCaracteres->at(i)->swap();
        }
    }
}




void OCR::extrairCaracteristicas(extratorCaracteristicas metodo, int k)
{
    // FIXME: Quando o outro metodo (de mesmo nome) estiver funcionando, remover esse!


    QList<QList<QImage> >::iterator l;
    for (l = subImgCaracteres->begin(); l != subImgCaracteres->end(); ++l) {

        // Iterando sobre a Linha
        QList<QImage>::iterator l_img;
        for (l_img = (*l).begin(); l_img != (*l).end(); ++l_img) {

            switch (metodo) {
            case FREEMAN: {
                //qDebug() << "Extracao de Caracteristicas: FREEMAN CHAIN CODE";
                QString code = caracteristicasFreemanChainCode((*l_img));
                qDebug() << code;
            }
                break;

            case MATRIZ_COOCORRENCIA: {
                // qDebug() << "Extracao de Caracteristicas: MATRIZ de COOCORRENCIA";
                QHash<QString, int> matriz = caracteristicasMatrizCoOcorrencia((*l_img), OCR::DIREITA);
                parseResultado("Matriz de CoOcorrencia", matriz);
                //qDebug() << "M: " << matriz;
            }
                break;

            case ENTROPIA: {
                QHash<QString, int> matriz = caracteristicasMatrizCoOcorrencia((*l_img), OCR::DIREITA);
                float entropia = caracteristicasEntropia(matriz);
                parseResultado("Entropia", matriz, entropia);
                //qDebug() << "M: " << matriz << "  Entropia: " << entropia;
            }
                break;

            case UNIFORMIDADE: {
                QHash<QString, int> matriz = caracteristicasMatrizCoOcorrencia((*l_img), OCR::DIREITA);
                float uniformidade = caracteristicasUniformidade(matriz);
                parseResultado("Uniformidade", matriz, uniformidade);
                //qDebug() << "M: " << matriz << "  Uniformidade: " << uniformidade;
            }
                break;

            case HOMOGENIEDADE: {
                QHash<QString, int> matriz = caracteristicasMatrizCoOcorrencia((*l_img), OCR::DIREITA);
                float homogeniedade = caracteristicasHomogeniedade(matriz);
                parseResultado("Homogeniedade", matriz, homogeniedade);
                //qDebug() << "M: " << matriz << "  Homogeniedade: " << homogeniedade;
            }
                break;

            case CONTRASTE: {
                QHash<QString, int> matriz = caracteristicasMatrizCoOcorrencia((*l_img), OCR::DIREITA);
                float contraste = caracteristicasContraste(matriz);
                parseResultado("Contraste", matriz, contraste);
                //qDebug() << "M: " << matriz << "  Contraste: " << contraste;
            }
                break;

            case CORRELACAO: {
                QHash<QString, int> matriz = caracteristicasMatrizCoOcorrencia((*l_img), OCR::DIREITA);
                float correlacao = caracteristicasCorrelacao(matriz);
                parseResultado("Correlacao", matriz, correlacao);
                //qDebug() << "M: " << matriz << "  Correlacao: " << correlacao;
            }
                break;

            case HISTOGRAMAS: {
                //caracteristicasHistogramas((*l_img));
            }
                break;

            case DIST_BORDA_CARACTERE: {
                int valor = caracteristicasDistBordaCaractere((*l_img), OCR::BC_DIR_ESQ_DES_DIAGONAL);
                parseResultado("Dist. Borda-Caractere", valor);
                //qDebug() << "Borda-Caractere: " << valor;
            }
                break;

            case NENHUM:
                qDebug() << "Extracao de Caracteristicas: NENHUM";
            }
        }
    }
}

void OCR::extrairCaracteristicas(bool teste,
                                 OCR::extratorCaracteristicas metodo,
                                 OCR::matrizOp matrizOperador,
                                 OCR::distBC bordaCaractereDir)
{
    //QList<OCRCaractere*>::iterator it;
    //for (it = listaOCRCaractere.begin(); it != listaOCRCaractere.end(); ++it) {

    QString cabecalho("ID; ");
    QString linha = "";

    foreach (OCRCaractere *c, listaOCRCaractere) {

        QImage caractere_img = c->imagem();
        linha.append(c->id() + "; ");

        switch (metodo) {

        case FREEMAN: {
            qDebug() << "BUG: Extracao de Caracteristicas: FREEMAN CHAIN CODE";
            //QString code = caracteristicasFreemanChainCode((*l_img));
            //qDebug() << code;
        }
            break;


        case MATRIZ_COOCORRENCIA: {
            // qDebug() << "Extracao de Caracteristicas: MATRIZ de COOCORRENCIA";
            QHash<QString, int> matriz = caracteristicasMatrizCoOcorrencia(caractere_img,
                                                                           matrizOperador);
            c->setMatrizCoOcorrencia(matriz);
            cabecalho.append("Matriz CoOcorrencia; ");
            linha.append(QString::number(matriz.value("00")) + "; " +
                         QString::number(matriz.value("01")) + "; " +
                         QString::number(matriz.value("10")) + "; " +
                         QString::number(matriz.value("11")) + "; ");

            float entropia = caracteristicasEntropia(matriz);
            c->setEntropia(entropia);
            cabecalho.append("Entropia; ");
            linha.append(QString::number(c->entropia()) + "; ");

            float uniformidade = caracteristicasUniformidade(matriz);
            c->setUniformidade(uniformidade);
            cabecalho.append("Uniformidade; ");
            linha.append(QString::number(c->uniformidade()) + "; ");

            float homogeniedade = caracteristicasHomogeniedade(matriz);
            c->setHomogeniedade(homogeniedade);
            cabecalho.append("Homogeniedade; ");
            linha.append(QString::number(c->homogeniedade()) + "; ");

            float contraste = caracteristicasContraste(matriz);
            c->setContraste(contraste);
            cabecalho.append("Contraste; ");
            linha.append(QString::number(c->contraste()) + "; ");

            float correlacao = caracteristicasCorrelacao(matriz);
            c->setCorrelacao(correlacao);
            cabecalho.append("Correlacao;");
            linha.append(QString::number(c->correlacao()) + "; ");
        }
            break;


        case HISTOGRAMAS: {
            caracteristicasHistogramas(*c);
        }
            break;

        case DIST_BORDA_CARACTERE: {
            int valor = caracteristicasDistBordaCaractere(caractere_img,
                                                          bordaCaractereDir);

            c->setDistBordaCaractere(valor);
            cabecalho.append("Dist. Borda-Caractere");
            linha.append(QString::number(c->distBordaCaractere()) + ";");

            //parseResultado("Dist. Borda-Caractere", valor);
            qDebug() << "Borda-Caractere: " << valor;
        }
                break;

        case NENHUM:
        case ENTROPIA: case UNIFORMIDADE: case HOMOGENIEDADE:
        case CONTRASTE: case CORRELACAO:
                qDebug() << "Extracao de Caracteristicas: NENHUM";
            }


        if (listaOCRCaractere.indexOf(c) == 0)
            emit texto(cabecalho);

        emit texto(linha);
        linha.clear();
    }


    //emit texto(QString("ID; Matriz CoOcorrencia; Entropia; Uniformidade; Homogeniedade; " \
    //                   "Contraste; Correlacao; Dist. Borda-Caractere"));

    /*foreach (OCRCaractere *c, listaOCRCaractere) {
        QHash<QString, int> h = c->matrizCo();
        QString s = c->id() + ";" +
                QString::number(h.value("00")) + "; " +
                QString::number(h.value("01")) + "; " +
                QString::number(h.value("10")) + "; " +
                QString::number(h.value("11")) + "; " +
                QString::number(c->entropia()) + "; " +
                QString::number(c->uniformidade()) + "; " +
                QString::number(c->homogeniedade()) + "; " +
                QString::number(c->contraste()) + "; " +
                QString::number(c->correlacao()) + "; " +
                QString::number(c->distBordaCaractere()) + ";";
        emit texto(s);
    }*/

    // Coloca a lista no devido lugar, se for para TREINAMENTO ou TESTE
    if (teste)
        listaOCRCaractereTeste = listaOCRCaractere;
    else
        listaOCRCaractereTreinamento = listaOCRCaractere;

    // Apaga a lista principal
    listaOCRCaractere.clear();
}




void OCR::caracteristicasHistogramas(OCRCaractere &c)
{
    QImage caractere_img = c.imagem();

    QVector<double> *perfHor = perfilProjecao(caractere_img, 0);
    c.setHistograma(perfHor, 1);

    QVector<double> *perfVer = perfilProjecao(caractere_img, 1);
    c.setHistograma(perfVer, 0);

    //qDebug() << "Perfil Horizontal: " << *perfHor;
    //qDebug() << "Perfil Vertical: " << *perfVer;
}



/**
 * @brief caracteristicasFreemanChainCode Chain-code de Freenab
 *
 *
 *          CODIGO DE FREEMAN
 *
 *              3  2  1                  (x-1,y-1)  (x,y-1) (x+1,y-1)
 *               \ | /                            \    |   /
 *            4 -- P -- 0        ---->   (x-1,y)  -- (x,y) -- (x+1,y)
 *               / | \                            /    |   \
 *              5  6  7                   (x-1,y+1) (x,y+1) (x+1,y+1)
 *
 * @param caractere Imagem a ser extraida o codigo
 *
 * @return Codigo extraido
 */
QString OCR::caracteristicasFreemanChainCode(const QImage &caractere)
{
    // buscando o elemento mais a ESQUERDA no TOPO
    QPoint pxInicial;
    int cinza = 0;

    // buscando por um pixel preto (0)
    for (int x = 0; x < caractere.width(); x++) {
        for (int y = 0; y < caractere.height(); y++) {
            cinza = qGray(caractere.pixel(x, y)) > 128 ? 0:1;

            if (cinza == 0) {
                pxInicial.setX(x);
                pxInicial.setY(y);
                break;
            }
        }
    }

    qDebug() << pxInicial;

    QPoint pxAnterior, pxAtual;
    QString code = "";
    QHash<QString, QPoint> vizValido;

    pxAnterior = pxAtual = pxInicial;

    do {
        // Busca pelo proximo vizinho que seja preto dentro da vizinhança 8x8.
        vizValido = freemanVizValidos(pxAtual, pxAnterior, caractere);

        // Pegando o Vizinho escolhido
        QHashIterator<QString, QPoint> it(vizValido);
        it.next();

        //QString k = it.key();
        QPoint pt = it.value();

        // Adcionando o codigo para a lista
        code.append(it.key());

        pxAnterior = pxAtual;
        pxAtual = pt;

        vizValido.clear();

        //qDebug() << pxAtual;

    // Soh para quando o ponto de partida eh o mesmo de chegada
    } while (pxInicial != pxAtual);

    //qDebug() << "Code: " << code;

    return code;
}


/**
 * @brief OCR::freemanVizValidos Atualmente esta buscando o proximo pixel de borda
 *          valido de acordo com o algoritmo de tracados de bordas de Moore.
 *
 * FIXME: Eh necessario fazer o metodo caracteristicasFreemanChainCode
 *          entenda o resultado retornado por esse
 *
 * @param p
 * @param p_ant
 * @param img
 *
 * @return
 */
QHash<QString, QPoint> OCR::freemanVizValidos(const QPoint &p,
                                              const QPoint p_ant,
                                              const QImage &img)
{
    QHash<QString, QPoint> viz;

    int x = p.x();
    int y = p.y();

    int w = img.width();
    int h = img.height();

    // Vizinhanca 8
    viz.insert("0", QPoint(x+1, y));
    viz.insert("1", QPoint(x+1, y-1));
    viz.insert("2", QPoint(x, y-1));
    viz.insert("3", QPoint(x-1, y-1));
    viz.insert("4", QPoint(x-1, y));
    viz.insert("5", QPoint(x-1, y+1));
    viz.insert("6", QPoint(x, y+1));
    viz.insert("7", QPoint(x+1, y+1));

    // Removendo pontos fora da imagem
    for (int i = 7; i <= 0; i--) {
        QPoint pt = viz.value(QString::number(i));

        // Coordenadas fora da imagem
        if ((pt.x() < 0) || (pt.x() >= w))
            viz.remove(QString::number(i));
        // Coordenadas fora da imagem
        else if ((pt.y() < 0) || (pt.y() >= h))
            viz.remove(QString::number(i));
        // Ponto atual eh i gual ao anterior
        else if (pt == p_ant)
            viz.remove(QString::number(i));
    }



    // descobrindo a posicao (codigo) do anterior
    int code_ant = viz.key(p_ant).toInt();

    // o total de vizinhos possiveis menos os que estao fora da imagem
    int total = viz.count();

    QHash<QString, QPoint> viz_v;

    // buscando o proximo pixel preto
    // para percorrer no sentido horario e pegar o codigo correto,
    // tenho que decrementar o code_ant
    while (total > 0) {
        // garantindo que seja um indice valido
        if (code_ant <= 0)
            code_ant = 7;

        // A Key de code_ant pode conter um valor invalido de key,
        // ja que varias keys foram removidas anteiormente
        if (viz.contains(QString::number(code_ant))) {
            // pega o proximo vizinho da vizinhaca 8
            QPoint np = viz.value(QString::number(code_ant));
            int cor = qGray(img.pixel(np)) > 128 ? 0:1;

            if (cor == 0) {
                // achou ponto preto
                viz_v.insert(QString::number(code_ant), np);
                return viz_v;
            }

            total--;
        }

        code_ant--;
    }

    // se encontrou o proximo pixel corretamente, nunca ira sair por aqui
    return viz_v;
}




/**
 * @brief caracteristicasMatrizCoOcorrencia Extrator de caracteristicas que usa
 *          Matriz de CoOcorrencia com 6 operadores: Direita, ABaixo, Direita e Acima,
 *          Direita e Abaixo, Direita e Abaixo (Diagonal), Direita e Acima (Diagonal)
 *
 * @param caractere Imagem do caractere a serem extraidas as caracteristicas
 * @param vizinhanca Posicao do proximo pixel a ser avaliado. Valor padrao eh 1.
 * @param operador Indica qual a vizinhaca a ser verificada
 *
 * @return Retorna um dicionario do tipo QHash<QString, int>.
 */
QHash<QString, int> OCR::caracteristicasMatrizCoOcorrencia(const QImage &caractere,
                                                           matrizOp operador,
                                                           int vizinhanca)
{
    QHash<QString, int> matriz = QHash<QString, int>();

    matriz.insert("00", 0);
    matriz.insert("01", 0);
    matriz.insert("10", 0);
    matriz.insert("11", 0);

    QPoint prox;
    int valor, valor_prox;
    QString v;

    for (int x = 0; x < caractere.width(); x++) {
        for (int y = 0; y < caractere.height(); y++) {

            // valor do pixel atual
            valor = qGray(caractere.pixel(x, y)) > 128 ? 1:0;

            // escolhendo o vizinho de acordo com o operador.
            // e verificando se a posicao eh valida.
            switch (operador) {
            case DIREITA: {
                // qDebug() << "Matriz CoOcorrencia: DIREITA;";

                prox.setX(x + vizinhanca);
                prox.setY(y);

                if (prox.x() >= caractere.width()) continue;

                valor_prox = qGray(caractere.pixel(prox)) > 128 ? 1:0;

                v = QString::number(valor) + QString::number(valor_prox);
                matriz[v] = matriz.value(v) + 1;
            }
                break;

            case ABAIXO: {
                //qDebug() << "Matriz CoOcorrencia: ABAIXO;";

                prox.setX(x);
                prox.setY(y + vizinhanca);

                if (prox.y() >= caractere.height()) continue;

                valor_prox = qGray(caractere.pixel(prox)) > 128 ? 1:0;

                v = QString::number(valor) + QString::number(valor_prox);
                matriz[v] = matriz.value(v) + 1;
            }
                break;

            case DIREITA_ACIMA: {
                //qDebug() << "Matriz CoOcorrencia: DIREITA e ACIMA;";

                // A DIREITA
                QPoint prox_dir(x + vizinhanca, y);
                // ACIMA
                QPoint prox_acima(x, y - vizinhanca);

                if (prox_dir.x() >= caractere.width() || prox_acima.y() < 0)
                    continue;

                int valor_prox_dir = qGray(caractere.pixel(prox_dir)) > 128 ? 1:0;
                int valor_prox_acima = qGray(caractere.pixel(prox_acima)) > 128 ? 1:0;

                // Salva o valor na Matriz (Dicionario)
                if (valor_prox_dir == 0 && valor_prox_acima == 0)
                    v = QString::number(valor) + "0";
                else if (valor_prox_dir == 1 && valor_prox_acima == 1)
                    v = QString::number(valor) + "1";
                else
                    break;

                matriz[v] = matriz.value(v) + 1;
            }
                break;

            case DIREITA_ABAIXO: {
                //qDebug() << "Matriz CoOcorrencia: DIREITA e ABAIXO;";

                // A DIREITA
                QPoint prox_dir(x + vizinhanca, y);
                // ABAIXO
                QPoint prox_abaixo(x, y + vizinhanca);

                if (prox_dir.x() >= caractere.width() || prox_abaixo.y() >= caractere.height())
                    continue;

                int valor_prox_dir = qGray(caractere.pixel(prox_dir)) > 128 ? 1:0;
                int valor_prox_abaixo = qGray(caractere.pixel(prox_abaixo)) > 128 ? 1:0;

                // Salva o valor na Matriz (Dicionario)
                if (valor_prox_dir == 0 && valor_prox_abaixo == 0)
                    v = QString::number(valor) + "0";
                else if (valor_prox_dir == 1 && valor_prox_abaixo == 1)
                    v = QString::number(valor) + "1";
                else
                    break;

                matriz[v] = matriz.value(v) + 1;
            }
                break;

            case DIREITA_ABAIXO_DIAGONAL: {
                //qDebug() << "Matriz CoOcorrencia: DIREITA e DIAGONAL ABAIXO;";

                // A DIREITA
                QPoint prox_dir(x + vizinhanca, y);
                // DIAGONAL ABAIXO
                QPoint prox_dia_abaixo(x + vizinhanca, y + vizinhanca);

                if (prox_dir.x() >= caractere.width() || prox_dia_abaixo.y() >= caractere.height())
                    continue;

                int valor_prox_dir = qGray(caractere.pixel(prox_dir)) > 128 ? 1:0;
                int valor_prox_dia_abaixo = qGray(caractere.pixel(prox_dia_abaixo)) > 128 ? 1:0;

                // Salva o valor na Matriz (Dicionario)
                if (valor_prox_dir == 0 && valor_prox_dia_abaixo == 0)
                    v = QString::number(valor) + "0";
                else if (valor_prox_dir == 1 && valor_prox_dia_abaixo == 1)
                    v = QString::number(valor) + "1";
                else
                    break;

                matriz[v] = matriz.value(v) + 1;
            }
                break;

            case DIREITA_ACIMA_DIAGONAL: {
                //qDebug() << "Matriz CoOcorrencia: DIREITA e DIAGONAL ACIMA;";

                // A DIREITA
                QPoint prox_dir(x + vizinhanca, y);
                // DIAGONAL ACIMA
                QPoint prox_dia_acima(x + vizinhanca, y - vizinhanca);

                if (prox_dir.x() >= caractere.width() || prox_dia_acima.y() < 0)
                    continue;

                int valor_prox_dir = qGray(caractere.pixel(prox_dir)) > 128 ? 1:0;
                int valor_prox_dia_acima = qGray(caractere.pixel(prox_dia_acima)) > 128 ? 1:0;

                // Salva o valor na Matriz (Dicionario)
                if (valor_prox_dir == 0 && valor_prox_dia_acima == 0)
                    v = QString::number(valor) + "0";
                else if (valor_prox_dir == 1 && valor_prox_dia_acima == 1)
                    v = QString::number(valor) + "1";
                else
                    break;

                matriz[v] = matriz.value(v) + 1;
            }
                break;
            }
        }
    }

    qDebug() << matriz;

    return matriz;
}




/**
 * @brief OCR::caracteristicasDistBordaCaractere
 * @param caractere
 * @param operador
 * @return
 */
int OCR::caracteristicasDistBordaCaractere(const QImage &caractere, OCR::distBC operador)
{
    int valor, soma = 0;

    switch (operador) {

    // Esquerda -> Direita
    case BC_DIREITA: {
        for (int y = 0; y < caractere.height(); y++) {
            for (int x = 0; x < caractere.width(); x++) {
                valor = qGray(caractere.pixel(x, y)) > 128 ? 1:0;
                if (valor == 0) break;
                soma++;
            }
        }
    }
        break;


    // Direita -> Esquerda
    case BC_ESQUERDA: {
        for (int y = 0; y < caractere.height(); y++) {
            for (int x = caractere.width()-1; x <= 0; x--) {
                valor = qGray(caractere.pixel(x, y)) > 128 ? 1:0;
                if (valor == 0) break;
                soma++;
            }
        }
    }
        break;


    // Bottom -> Up
    case BC_ACIMA: {
        for (int x = 0; x < caractere.width(); x++) {
            for (int y = caractere.height()-1; y <= 0; y--) {
                valor = qGray(caractere.pixel(x, y)) > 128 ? 1:0;
                if (valor == 0) break;
                soma++;
            }
        }
    }
        break;


    // Up -> Down
    case BC_ABAIXO: {
        for (int x = 0; x < caractere.width(); x++) {
            for (int y = 0; y < caractere.height(); y++) {
                valor = qGray(caractere.pixel(x, y)) > 128 ? 1:0;
                if (valor == 0) break;
                soma++;
            }
        }
    }
        break;


    // Bottom-Left -> Top-Right
    case BC_ESQ_DIR_SUB_DIAGONAL: {
        int h = caractere.height();
        int w = caractere.width();
        int max = h + w - 2;

        for (int e = 0; e <= max; e++) {
            for (int y = 0; y < w; y++) {
                for (int x = 0; x < h; x++) {
                    if (x + y - e == 0) {
                        valor = qGray(caractere.pixel(x, y)) > 128 ? 1:0;
                        if (valor == 0) break;
                        soma++;
                    }
                }
            }
        }
    }
        break;


    // Diagonal: Top-Left -> Bottom-Right
    case BC_ESQ_DIR_DES_DIAGONAL: {
        int h = caractere.height();
        int w = caractere.width();
        int max = h + w - 2;

        for (int e = max; e >= 0; e--) {
            for (int y = 0; y < w; y++) {
                for (int x = 0; x  < h; x++) {
                    if (x + y - e == 0) {
                        valor = qGray(caractere.pixel(x, y)) > 128 ? 1:0;
                        if (valor == 0) break;
                        soma++;
                    }
                }
            }
        }
    }
        break;


    // Bottom-Right -> Top-Left
    case BC_DIR_ESQ_SUB_DIAGONAL: {
        int h = caractere.height();
        int w = caractere.width();
        int max = h + w - 2;

        for (int e = max; e >= 0; e--) {
            for (int x = 0; x  < w; x++) {
                for (int y = 0; y < h; y++) {
                    if (x + y - e == 0)  {
                        valor = qGray(caractere.pixel(x, y)) > 128 ? 1:0;
                        if (valor == 0) break;
                        soma++;
                    }
                }
            }
        }
    }
        break;


    // Diagonal: Top-Right -> Bottom-Left
    case BC_DIR_ESQ_DES_DIAGONAL: {
        int h = caractere.height();
        int w = caractere.width();
        int max = h + w - 2;

        for (int e = 0; e <= max; e++) {
            for (int x = 0; x < w; x++) {
                for (int y = 0; y < h; y++) {
                    if (x + y - e == 0) {
                        valor = qGray(caractere.pixel(x, y)) > 128 ? 1:0;
                        if (valor == 0) break;
                        soma++;
                    }
                }
            }
        }
    }
        break;

    }

    return soma;
}




/**
 * @brief Calcula a probabilidade de ocorrencia de um elemento cuja chave eh KEY,
 *          dentro da matriz de coocorrencia
 *
 * @param matriz
 * @param key
 * @return
 */
float OCR::probabilidadeElemMatrizCoOcorrencia(const QHash<QString, int> &matriz, const QString &e_key)
{
    float soma_valores = 0.0;

    // soma todos os valores da matriz
    foreach (QString key, matriz.keys()) {
        soma_valores += matriz.value(key);
    }

    // calcula a probabilidade (p) do elemento (g): p(i,j) = g(i,j) / soma_valores
    float g = matriz.value(e_key);
    float p = g/soma_valores;

    return p;
}




/**
 * @brief Calcula a Entropia de uma Matriz de Coocorrencia.
 *
 *      GONZALEZ: A Entropia mede a aleatoridade dos elementos da Matriz. A
 *              entropia eh 0 quando todos os elementos sao 0 e eh maxima quando
 *              todos os elementos sao iguais. O valor maximo eh: 2*log2(K).
 *
 * @param matriz
 * @return
 */
float OCR::caracteristicasEntropia(const QHash<QString, int> &matriz)
{
    QHash<QString, float> matriz_prob;

    // Insere as probabilidades no dicionario
    foreach (QString key, matriz.keys()) {
        float prob = probabilidadeElemMatrizCoOcorrencia(matriz, key);
        matriz_prob.insert(key, prob);
    }

    // calculando a ENTROPIA
    float entropia = 0;
    foreach (QString key, matriz_prob.keys()) {
        float elemento_prob = matriz_prob.value(key);
        float e_log = elemento_prob <= 0 ? 0:log2(elemento_prob);
        entropia +=  elemento_prob * e_log;
    }

    return -entropia;
}




/**
 * @brief Calcula a UNIFORMIDADE da Matriz de CoOcorrencia.
 *
 *      GONZALEZ: Uniformidade eh uma medida de uniformidade no intervalo [0,1].
 *                  A uniformidade eh 1 para uma imagem constante.
 *
 * @param matriz
 *
 * @return
 */
float OCR::caracteristicasUniformidade(const QHash<QString, int> &matriz)
{
    QHash<QString, float> matriz_prob;

    // Insere as probabilidades no dicionario
    foreach (QString key, matriz.keys()) {
        float prob = probabilidadeElemMatrizCoOcorrencia(matriz, key);
        matriz_prob.insert(key, prob);
    }

    // calculando a UNIFORMIDADE ou ENERGIA
    float uniformidade = 0;
    foreach (QString key, matriz_prob.keys()) {
        float elemento_prob = matriz_prob.value(key);
        uniformidade +=  elemento_prob * elemento_prob;
    }

    return uniformidade;
}




/**
 * @brief Calcula a Homogeniedade da Matriz de CoOcorrencia.
 *
 *      GONZALEZ: Homogeniedade mede a proximidade espacial da distribuicao
 *              de elemetros das Matriz na diagonal. O intervalo de valores
 *              eh [0,1], com o maximo a ser atingido quando Matriz eh diagonal.
 *
 * @param matriz
 *
 * @return
 */
float OCR::caracteristicasHomogeniedade(const QHash<QString, int> &matriz)
{
    QHash<QString, float> matriz_prob;

    // Insere as probabilidades no dicionario
    foreach (QString key, matriz.keys()) {
        float prob = probabilidadeElemMatrizCoOcorrencia(matriz, key);
        matriz_prob.insert(key, prob);
    }

    // calculando a HOMOGENIEDADE
    float homogeniedade = 0;
    foreach (QString key, matriz_prob.keys()) {
        float elemento_prob = matriz_prob.value(key);
        int i = key.at(0).digitValue();
        int j = key.at(1).digitValue();
        homogeniedade +=  elemento_prob / (1 + abs(i - j));
    }

    return homogeniedade;
}




/**
 * @brief Calcula o Constraste baseado na Matriz de CoOcorrencia.
 *
 * @param matriz
 *
 * @return
 */
float OCR::caracteristicasContraste(const QHash<QString, int> &matriz)
{
    QHash<QString, float> matriz_prob;

    // Insere as probabilidades no dicionario
    foreach (QString key, matriz.keys()) {
        float prob = probabilidadeElemMatrizCoOcorrencia(matriz, key);
        matriz_prob.insert(key, prob);
    }

    // calculando Contraste
    float contraste = 0;
    foreach (QString key, matriz_prob.keys()) {
        float elemento_prob = matriz_prob.value(key);
        // i - j
        int i_sub_j = key.at(0).digitValue() - key.at(1).digitValue();
        contraste +=  i_sub_j * i_sub_j * elemento_prob;
    }

    return contraste;
}




/**
 * @brief OCR::caracteristicasCorrelacao
 *
 *      OBS.: Implementacao de acordo com o livro do Gonzalez. Tabela 11.3, pag 854.
 *      OBS2.: Onde "sig" eh 0 o resultado eh "nan".
 *
 * @param matriz
 *
 * @return
 */
float OCR::caracteristicasCorrelacao(const QHash<QString, int> &matriz)
{
    QHash<QString, float> matriz_prob;

    // Insere as probabilidades no dicionario
    foreach (QString key, matriz.keys()) {
        float prob = probabilidadeElemMatrizCoOcorrencia(matriz, key);
        matriz_prob.insert(key, prob);
    }

    // mr = sum_i^K ( i * sum_j^K(Pij) )
    // mc = sum_j^K ( j * sum_i^K(Pij) )
    float mr, mc;
    mr = mc = 0.0;

    foreach (QString key, matriz.keys()) {
        int i = key.at(0).digitValue();
        int j = key.at(1).digitValue();

        mr += i * matriz_prob.value(key);
        mc += j * matriz_prob.value(key);
    }

    float sig_r, sig_c;
    sig_r = sig_c = 0.0;

    foreach (QString key, matriz.keys()) {
        int i = key.at(0).digitValue();
        int j = key.at(1).digitValue();

        sig_r += (i - mr) * (i - mr) * matriz_prob.value(key);
        sig_c += (j - mc) * (j - mc) * matriz_prob.value(key);
    }

    sig_r = sqrt(sig_r);
    sig_c = sqrt(sig_c);

    float correlacao = 0.0;
    foreach (QString key, matriz.keys()) {
        int i = key.at(0).digitValue();
        int j = key.at(1).digitValue();

        correlacao += ((i - mr)*(j - mc)*matriz_prob.value(key)) / (sig_r * sig_c);
    }

    return correlacao;
}

void OCR::quantidadeCaracteres()
{
    qDebug() << "listaOCRCaractere.count: " << listaOCRCaractere.count();
}




void OCR::parseResultado(QString titulo, float resultado)
{
    emit inserirTexto(titulo, QString::number(resultado));
}



void OCR::parseResultado(QString titulo, QHash<QString, int> resultado)
{
    foreach (QString key, resultado.keys()) {
        emit inserirTexto(titulo, QString("%1; %2")
                                                    .arg(key)
                                                    .arg(QString::number(resultado.value(key))));
    }
}



void OCR::parseResultado(QString titulo, QHash<QString, int> resultado, float valor)
{
    foreach (QString key, resultado.keys()) {
        emit inserirTexto(titulo, QString("%1; %2; %3")
                                                    .arg(key)
                                                    .arg(QString::number(resultado.value(key)))
                                                    .arg(QString::number(valor)));
    }
}



void OCR::parseResultado(QString titulo, QVector<double> resultado)
{
}

void OCR::parseResultado(QString titulo, QString resultado)
{
    emit inserirTexto(resultado, titulo);
}

void OCR::classificarCaracteres()
{
    foreach (OCRCaractere *c, listaOCRCaractereTeste) {
        int menor_dist = 100;
        OCRCaractere *c_proximo = c;

        foreach (OCRCaractere *c_treina, listaOCRCaractereTreinamento) {
            int dist = abs(c->entropia() - c_treina->entropia());

            if (dist < menor_dist) {
                menor_dist = dist;
                c_proximo = c_treina;
            }
        }

        QString txt = QString(c->id() + "-" + c_proximo->id() + " : dist: " + QString::number(menor_dist));

        qDebug() << txt;
        emit texto(txt);
    }
}

void OCR::limparTudo()
{
    //if (linhas->count()) linhas->clear();
    if (listaCaracteres) listaCaracteres->clear();
    if (subImgLinhas) subImgLinhas->clear();
    if (subImgCaracteres) subImgCaracteres->clear();
    //if (cFreemanChainCode) cFreemanChainCode->clear();
    //if (caracteristicas) caracteristicas->clear();

    if (listaOCRCaractere.count() > 0) listaOCRCaractere.clear();
    if (listaOCRCaractereTreinamento.count() > 0) listaOCRCaractereTreinamento.clear();
    if (listaOCRCaractereTeste.count() > 0) listaOCRCaractereTeste.clear();
}

