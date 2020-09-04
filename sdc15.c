/**********************************************************/
/*   Azbil SDC15 CPL communication test program           */
/*                              T.Nishimori   2016/04/29  */
/* members:                                               */
/*    sdc15.c                                             */
/*    sio.c                                               */
/* 依存情報:                                              */
/*    libc.so.6 => /lib/libc.so.6                         */
/*    /lib/ld-linux.so.2 => /lib/ld-linux.so.2            */
/**********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sio.c"

#define STX 0x02
#define ETX 0x03
#define	BUF 1024

int main( int argc, char **argv ) {

  /* シリアルポート通信関連の変数 */
  char rs_rbuffer[BUF];
  char rs_sbuffer[BUF];
  int c;
  int i = 0;
  int CheckSum = 1;

  static int STX_RECIEVED;
  char *serial_dev;

  unsigned char	theHdr[BUF];          /* imported */
  unsigned long	SzToSnd;             /* imported */
  unsigned char	theMsg[BUF];  /* imported */

  char command[BUF];  /* imported */
  char recieve[BUF];  /* imported */


  /* 引数のチェック&シリアルデバイス名の取得 */
  if( ( argc == 3 ) || ( argc == 4 ) ) {
    serial_dev = argv[1];
  } else {
    fprintf( stderr, "usage: sdc15 [serial_dev] [address(1digit)] [set value]\n" );
    exit( 1 );
  }

  /* シリアル通信用バッファの初期化 */
  memset( rs_rbuffer, '\0', BUF );
  memset( rs_sbuffer, '\0', BUF );

  memset( theHdr, '\0', BUF );     /* added */
  memset( theMsg, '\0', BUF );     /* added */

  memset( command, '\0', BUF );     /* added */
  memset( recieve, '\0', BUF );     /* added */

  /* 送信する電文の作成*/
  if ( argc == 3 )
    //    strcpy( command, "RS,14593W,1" );   /* imported */
    strcpy( command, "RS,14356W,1" );   /* imported */
  else if ( argc == 4 ) {
    strcpy( command, "WS,14593W," );   /* imported */
    strcat( command, argv[3] );
  }

  sprintf( (char*)theHdr, "\x02\x30" );     /* imported */

  if( (argv[2][0] >= 0x30) && (argv[2][0] <= 0x39) && (argv[2][1] == '\0') ) {
    theHdr[2] = argv[2][0];
  } else {
    fprintf( stderr, "usage: sdc15 [serial_dev] [address(1digit)] [set value]\n" );
    exit( 1 );
  }
  theHdr[3] = 0x30;
  theHdr[4] = 0x30;
  theHdr[5] = 0x58;
  
  sprintf( (char*)theMsg, "%s%s\x03", theHdr, command );    /* imported */

  if( CheckSum ){                                          /* imported */
    SzToSnd = strlen( (char*)theMsg );                     /* imported */
    CPLSum( theMsg, SzToSnd, &theMsg[SzToSnd] );           /* imported */
  }
  strcat( (char*)theMsg, "\r\n" );                         /* imported */
  SzToSnd = strlen( (char*)theMsg );                       /* imported */

  /*  sprintf(rs_sbuffer,"%choge%c",STX,ETX); */       /* subtract */

  /* シリアルポートの初期化 */
  open_serial_port(serial_dev);

  /* シリアルポートへデータを送信 */
  put_serial_string( theMsg );

  /* シリアルポートからのデータ受信待ちループ */
  while ( 1 ) {
    c = 0;
    /* シリアルポートから1バイトのデータを受信                   */
    /* データがある時だけ読み込み可能で,他の場合はブロックされる */
    c = get_serial_char();
    /* STX以降(ETXまで)をデータとして認識 */
    if ( c == STX ) {
      STX_RECIEVED = 1;
    }
    /* アスキー文字のみをバッファに格納 */
    if( c >= 0x30 ) {
      rs_rbuffer[i] = c;
      i++;
    }
    /* STX受信後にETXを受け取ったらそこまでを通信文と判断して処理 */
    if( c == ETX && STX_RECIEVED ) {
      /* 受信した電文を画面出力 */
      printf("[Recv(%d)]:%s\n", strlen(rs_rbuffer), rs_rbuffer);
      /* 受信バッファのクリア */
      memset( rs_rbuffer, '\0', BUF );
      /* 受信文字列カウンタの初期化 */
      i = 0;
      STX_RECIEVED = 0;
      break;
    }
    /* バッファから溢れそうになったら終了 */
    if( i > BUF - 1 )
      break;
  }

  /* シリアルポートを閉じる */
  close_serial_port();

  return 0;
}


