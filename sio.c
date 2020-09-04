/* Azbil SDC15 communication test program    */

#include <stdio.h>
#include <termios.h>
#include <sys/signal.h>
#include <unistd.h>
#include <fcntl.h>

#define _POSIX_SOURCE 1 /* POSIX comliant source (POSIX準拠のソース)*/

#define BAUDRATE B19200

int fd;
struct termios oldtio; /* current serial port settings
			  (現在のシリアルポートの設定を待避)*/
unsigned char *receive_buffer;

/*----- Open serial port method -----*/
int open_serial_port( char *modem_dev )
{

  struct termios newtio;

  /* Open modem device for reading and writing and not as controlling tty */
  /* 読み書きの為にモデムデバイスをオープンする。ノイズによってCTRL-Cが
     たまたま発生しても接続が切れないようにtty制御はしない */
  fd = open( modem_dev, O_RDWR | O_NOCTTY );

  if( fd < 0 ) {
    perror( modem_dev );
    exit(-1);
  }

  tcgetattr( fd, &oldtio );

  /* Clear the struct for new port settings */
  /* 制御文字の初期化を行う */
  newtio.c_iflag = 0;
  newtio.c_oflag = 0;
  newtio.c_cflag = 0;
  newtio.c_lflag = 0;
  newtio.c_line = 0;
  bzero( newtio.c_cc, sizeof(newtio.c_cc) );

  /* Settings for new port
      BAUDRATE : Set bps rate. You could also use cfsetispeed and cfsetospeed.
      ボーレートの設定。cfsetispeed,cfsetospeedも使用できる
      CRTSCTS  : Output hardware flow control ( only used if the cable has all
                  necessary lines. See sect. 7 of Serial-HOWTO )
             出力のハードウェアフロー制御(必要な結線が全てされているケースのみ)
      CS8      : 8n1 ( 8bit,no parity,1 stopbit)
      CLOCAL   : local connection,no modem control
      CREAD    : enable receiving characters
                    受信文字を有効にする
  */
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD ;

  /* next 4 lines are imported */
  /*  newtio.c_cflag |= CS8; */   /* 送受信に８ビット               */
  newtio.c_cflag &= ~CSTOPB;      /* 送受信にストップビットを１     */
  newtio.c_cflag |= PARENB;       /* 送受信にパリティチェックを行う */
  newtio.c_cflag &= ~PARODD;      /* 送受信に偶数パリティを使用する */


  
  /* IGNPAR  : ignore bytes with parity errors */
  /* (パリティエラーのデータは無視する )*/
  /* ICRNL  : */
  /* (CRをNLに対応させる。これを行わないと他のコンピュータでCRを入力しても
     入力が終わりにならない)*/
  newtio.c_iflag = IGNPAR;

  newtio.c_oflag = 0;     /* Raw output (Rawモードでの出力)*/
  newtio.c_lflag = 0;     /* Set input mode (non-canonical,no echo,....) */
  /* ICANN :カノニカル入力を有効にする
                                 全てのエコーを無効にし、プログラムに対し
                                 シグナルは送らせない */
  newtio.c_cc[VTIME] = 0; /* inter-character timer 2 sec
			     (キャラクタタイマは使わない) */
  newtio.c_cc[VMIN] = 1;  /* read single char received
			     (1文字来るまで読み込みをブロックする */

  /* Now clean the modem line and activate the settings for the port */
  /* モデムラインをクリアする */
  tcflush( fd, TCIFLUSH );
  /* 新しい設定を適用する */
  tcsetattr( fd, TCSANOW, &newtio );

  return 0;
}

/*----- Close serial port method -----*/
void close_serial_port( void )
{
  /* Restore the old port settings */
  tcsetattr( fd, TCSANOW, &oldtio);
  close( fd );
}


/*----- Output a character to current serial port -----*/
int put_serial_char( unsigned char c )
{
  if ( write(fd,&c,1) != 1 )
    return -1;
  return 0;
}


/*----- Output a string to current serial port -----*/
int put_serial_string( char *s )
{
  if ( write(fd,s,strlen(s) )  != 1 )
    return -1;
  return 0;
}


/*----- Get a character from current serial port -----*/
int get_serial_char()
{
  unsigned char c;
  int res;
  res = read( fd,(char *)&c, 1 );
  return c;
}


/*----- Azbil CPL communication Check Sum -----*/
void CPLSum( unsigned char *str, int len, unsigned char *buf )
{
	int	   			num=0;
	unsigned char	*ch;

	for( ch=&str[0] ; ch<&str[len] ; ch++ )
	  num += *ch;
	num = (-(num&0x000000FF)&0x000000FF);
	sprintf( (char*)buf, "%02X", num );
}
