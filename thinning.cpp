#include "thinning.h"

#define C(x,y) x,y
#define N(x,y) x,y+1
#define NE(x,y) x+1,y+1
#define NO(x,y) x-1,y+1
#define S(x,y) x,y-1
#define SE(x,y) x+1,y-1
#define SO(x,y) x-1,y-1
#define L(x,y) x+1,y
#define O(x,y) x-1,y

Thinning::Thinning()
{
}

void Thinning::Binarizacao(QImage *img)
{
    for (int y = 1; y < img->height(); y++) {
        for (int x = 1; x < img->width(); x++) {
            int valor = qGray(img->pixel(x,y)) > 128? 255: 0;
            img->setPixel(x,y, qRgb(valor, valor, valor));
        }
    }
}


void Thinning::Stentiford(QImage *img){

    bool ImagemAlterada = true;

    // define uma estrutura para a máscara
    struct mascara {
    int x;       // coordenada x
    int y;       // coordenada y
    bool verificaCor[9];   // vizinhos ao pixel
    bool apaga;  // flag para marcar se o pixel vai ser deletado
    };

    // copia a imagem original
    QImage *img1 = img;


 // condição de parada: até que a imagem não for alterada (não ter mais pixels removidos)
 while (ImagemAlterada) {

  // declaração de variáveis
     int x, y, n;
     unsigned long int qtPixels = 0;
     uint index=0;

    //percorre imagem e conta pixels pretos
    for (y = 1; y < img1->height(); y++) {
        for (x = 1; x < img1->width(); x++) {
            if( verificaPixel(img1, x, y) )
            qtPixels++;
        }
    }

    // qDebug () << "Pixels: " << qtPixels;

    //  aloca memória para a quantidade de pixels existente
    mascara *mask = new mascara[qtPixels];

    // percorre imagem para definir vizinhos
    for (y = 1; y < img1->height(); y++) {
        for (x = 1; x < img1->width(); x++) {

            // se for um pixel preto
            if( img1->pixel(x,y)== (qRgb(0,0,0)) ) {

                mask[index].x = x;
                mask[index].y = y;
                mask[index].apaga = false;

                // verifica se os pixels vizinhos são pretos

                if( verificaPixel(img1, L(x,y)) )
                mask[index].verificaCor[1] = true;
                else
                mask[index].verificaCor[1] = false;

                if( verificaPixel(img1, SE(x,y)) )
                mask[index].verificaCor[2] = true;
                else
                mask[index].verificaCor[2] = false;

                if( verificaPixel(img1, S(x,y)) )
                mask[index].verificaCor[3] = true;
                else
                mask[index].verificaCor[3] = false;

                if( verificaPixel(img1, SO(x,y)) )
                mask[index].verificaCor[4] = true;
                else
                mask[index].verificaCor[4] = false;

                if( verificaPixel(img1, O(x,y)) )
                mask[index].verificaCor[5] = true;
                else
                mask[index].verificaCor[5] = false;

                if( verificaPixel(img1, NO(x,y)) )
                mask[index].verificaCor[6] = true;
                else
                mask[index].verificaCor[6] = false;

                if( verificaPixel(img1, N(x,y)) )
                mask[index].verificaCor[7] = true;
                else
                mask[index].verificaCor[7] = false;

                if( verificaPixel(img1, NE(x,y)) )
                mask[index].verificaCor[8] = true;
                else
                mask[index].verificaCor[8] = false;

                index++;
                }

        }

    }


    // faz operações para cada uma das máscaras
    for(int M = 1; M <= 4; M++)    {

        for(index = 0; index < qtPixels; index++)  {

        int NumPixelsCorVizinhos = 0, NumConect = 0;
        bool terminal;
        bool status = false;


        // Percorrer a imagem até encontrar um pixel que se encaixe nas máscaras
        switch( M )  {

        case 1:
        // Máscara 1: verifica se pixel superior é branco e se pixel inferior é preto
        if( (!mask[index].apaga) && (mask[index].verificaCor[3] == true) && (mask[index].verificaCor[7] == false))
        status = true;
        break;

        case 2:
        // Máscara2: verifica se pixel esquerdo é branco e se pixel direito é preto
        if( (!mask[index].apaga) && (mask[index].verificaCor[5] == true) && (mask[index].verificaCor[1] == false))
        status = true;
        break;

        case 3:
        // Máscara3: verifica se pixel superior é preto e se pixel inferior é branco
        if( (!mask[index].apaga) && (mask[index].verificaCor[3] == false) && (mask[index].verificaCor[7] == true))
        status = true;
        break;

        case 4:
        // Máscara4: verifica se pixel esquerdo é preto e se pixel direito é branco
        if( (!mask[index].apaga) && (mask[index].verificaCor[5] == false) && (mask[index].verificaCor[1] == true) )
        status = true;
        break;
        }


        if( status ) {

        // verifica se pixel é ponto terminal
            for(n = 1; n <= 8 ; n++) {

                if( mask[index].verificaCor[n] == 1 )
                NumPixelsCorVizinhos++;
            }

            if( NumPixelsCorVizinhos == 1 )
            terminal = true;
            else
            terminal = false;


        // verifica número de conectividade
         for(int temp1 = 1; temp1 <= 8; temp1++) {
            int temp2;

                if(temp1 == 8)
                temp2 = 1;
                else
                temp2 = temp1+1;

                if( (mask[index].verificaCor[temp1] == 0) && (mask[index].verificaCor[temp2] == 1) )
                NumConect++;

            }

             /* se o pixel não é terminal e seu número de
            conectividade é 1, marca este pixel para ser apagado*/
         if( (!terminal) && (NumConect == 1) )

            mask[index].apaga = true;

           }

       }

    }


    ImagemAlterada = false;

            // apaga todos os pixels marcados
            for(index = 0; index < qtPixels; index++) {
                if( mask[index].apaga ) {
                img1->setPixel(QPoint( mask[index].x, mask[index].y), qRgb(255,255,255) );
                ImagemAlterada = true;
                }

               img = img1;
               delete (&mask);

            }


    }

}


bool Thinning::verificaPixel(QImage *img, int x, int y) {

    QImage *img1 = img;

    if ( img1->pixel(x,y) == (qRgb(0,0,0)) ) return true;
    else return false;


}


bool Thinning::vizinhanca(QImage *img, int x,int y) {



// verificaCorerifica se o número de vizinhos pretos está na faixa entre 2 e 6
int vizinhos=0;

vizinhos = ( verificaPixel(img, NO(x,y)) + verificaPixel(img, N(x,y)) + verificaPixel(img,NE(x,y)) +
           verificaPixel(img, O(x,y)) + verificaPixel(img, L(x,y)) + verificaPixel(img, SO(x,y)) +
           verificaPixel(img, S(x,y)) + verificaPixel(img, SE(x,y)) );

if (vizinhos < 2 || vizinhos > 6) return false;

// verificaCorerifica conectividade é igual 1...
int conectividade = 0;

if(verificaPixel (img, N(x,y))  == false && verificaPixel (img, NE(x,y)) == true)
        conectividade++;

if(verificaPixel(img, NE(x,y)) == false && verificaPixel(img, L(x,y)) == true)
    conectividade++;

if(verificaPixel(img, L(x,y)) == false && verificaPixel(img, SE(x,y)) == true)
    conectividade++;

if(verificaPixel(img, SE(x,y)) == false && verificaPixel(img, S(x,y)) == true)
    conectividade++;

if(verificaPixel(img, S(x,y)) == false && verificaPixel(img, SO(x,y)) == true)
    conectividade++;

if(verificaPixel(img, SO(x,y)) == false && verificaPixel(img, O(x,y)) == true)
    conectividade++;

if(verificaPixel(img, O(x,y)) == false && verificaPixel(img, NO(x,y)) == true)
    conectividade++;

if(verificaPixel(img, NO(x,y)) == false && verificaPixel(img, N(x,y)) == true)
    conectividade++;

if(conectividade == 1)
    return true;
else
    return false;

}


void Thinning::Holt(QImage *img) {

    int x,y;
    bool continua = true;
    QImage *img1 = img;
    QPoint * pixel;
    QList <QPoint *> *pontos_removidos = new QList<QPoint *>(); // cria a lista
    int contador;

    while(continua)    {

    continua = false;

    for (y = 1; y < img1->height(); y++) {
        for (x = 1; x < img1->width(); x++) {


        // verificaPixel somente para pixels pretos (otimização)
         if(verificaPixel(img1, x, y)) {

        // verificaPixelerifica fórmula de sobrevivencia do pixel
         if( ( !vizinhanca(img1,C(x,y)) || ( verificaPixel(img1,L(x,y)) && verificaPixel(img1,S(x,y) )
           && ( verificaPixel(img1,N(x,y)) || verificaPixel(img1,O(x,y)) ) ) ) == false) {
            // Então o pixel deve ser removido...
            continua = true;
            pontos_removidos->append(new QPoint(x , y));
            }

         }
     }
   }


    for (contador=0; contador < pontos_removidos->size(); contador++) {

      pixel = pontos_removidos->at(contador);
      img1->setPixel(QPoint( pixel->x() ,pixel->y()), qRgb(255,255,255) );
    }


     pontos_removidos->clear();



    for (y = 1; y < img1->height(); y++) {
        for (x = 1; x < img1->width(); x++) {


        // Verifica somente para pixels pretos
        // (somente para optimizar a velocidade)
            if(verificaPixel(img1, C(x,y)) == true)  {
            // Verifica formula de sobrevivencia do
            // pixel...
                if( ( !vizinhanca(img1, C(x,y)) || ( verificaPixel(img1, O(x,y))
                       && verificaPixel(img1,N(x,y)) && ( verificaPixel(img1, S(x,y))
                       || verificaPixel(img1, L(x,y)) ) ) ) == false) {
            // O pixel deve ser removido...
            continua = true;

             pontos_removidos->append(new QPoint(x , y));
                }
            }
        }
    }



    for (contador=0; contador < pontos_removidos->size(); contador++) {
        pixel = pontos_removidos->at(contador);
        img1->setPixel(QPoint( pixel->x() ,pixel->y()), qRgb(255,255,255) );
      }

       pontos_removidos->clear();

  }

 img = img1;

}


void Thinning::ZhanSuen(QImage *img) {

    bool continua = true;
    int x, y, contador;
    QPoint *pixel;
    QImage img1 = *img;

    //CArray <PixelPoint,PixelPoint> RemPoints;
    QList <QPoint *> *pontos_removidos = new QList<QPoint *>();
    //CArray<CArray<BYTE, BYTE> *,CArray<BYTE, BYTE> *> Iteraction;


    while (continua) {
    continua = false;

    // ------------   Primeira Iteração   ------------

    for (y = 1; y < img->height(); y++) {

        for (x = 1; x < img->width(); x++) {

     if(verificaPixel(img, x , y)) {
     // 1º Passo: Pixel Preto e Conectividade igual a 1.
     // 2º Passo: Existem ao menos dois pixels vizinhos pretos, e não mais do que seis;
        if (vizinhanca(img, C(x,y))) {
        // 3º Passo: Ao menos um dos pixels Norte, Leste ou Oeste é branco;
            if ( (!verificaPixel(img, N(x,y))) || (!verificaPixel(img, L(x,y)))|| (!verificaPixel(img, O(x,y))) ) {
        // 4º Passo: Ao menos um dos pixels Norte, Oeste ou Sul é branco;
                if ( (!verificaPixel(img, N(x,y))) || (!verificaPixel(img, O(x,y)))|| (!verificaPixel(img, S(x,y))) ) {
                    continua = true;
                    pontos_removidos->append(new QPoint(x , y));
               }
           }
       }
     }

     }
   }

     // Remover os pontos

    for (contador=0; contador < pontos_removidos->size(); contador++) {
           pixel = pontos_removidos->at(contador);
           img->setPixel(QPoint( pixel->x() ,pixel->y()), qRgb(255,255,255) );
         }

       pontos_removidos->clear();




    // ------------   Segunda Iteração   ------------

       for (y = 1; y < img->height(); y++) {

           for (x = 1; x < img->width(); x++) {

        if(verificaPixel(img, x , y)) {
          // 1º Passo: Pixel Preto e Conectividade igual a 1.
          // 2º Passo: Existem ao menos dois pixels vizinhos pretos, e não mais do que seis;
             if (vizinhanca(img, C(x,y))) {
             // 3º Passo: Ao menos um dos pixels Norte, Leste ou Sul é branco;
                 if ( (!verificaPixel(img, N(x,y))) || (!verificaPixel(img, L(x,y)))|| (!verificaPixel(img, S(x,y))) ) {
             // 4º Passo: Ao menos um dos pixels Oeste, Leste ou Sul é branco;
                     if ( (!verificaPixel(img, O(x,y))) || (!verificaPixel(img, L(x,y)))|| (!verificaPixel(img, S(x,y))) ) {
                        continua = true;
                        pontos_removidos->append(new QPoint(x , y));
                    }
                }
            }
          }

        }
      }


    // Remover os pontos



    for (contador=0; contador < pontos_removidos->size(); contador++) {
            pixel = pontos_removidos->at(contador);
            img->setPixel(QPoint( pixel->x() ,pixel->y()), qRgb(255,255,255) );
          }
           pontos_removidos->clear();

        }

    img1 = *img;
}
