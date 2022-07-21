/*****  pp_spi.c  -V2.0 **********************************************************************************
* @file     pp_spi.c
* @version  V2.0
* @date     08 May 2022
* @note     Produced by TOMMY 

�g�p�ϐ��E�|�C���^�E��`�l
�O���[�o����`(�t�@�C���S�̂Ŏg�p)
���[�J����`(�֐����Ŏg�p)

SPI�ʐM�C���^�[�t�F�[�X
1.�s����`�E�L�����֐�
�Q�ƃf�[�^�V�[�g�FVA10800/10820_PG_v19
- spi_a,spi_b�͂ǂ����PORTA�Œ�`
- FUNCSEL�Ŏg�������邱�ƁiPORTB�ɂ������ꍇ��FUNCSEL�����ӂ��K�v�j
* PORTA�ł�MOSIA/MISOA/CLKA / PORTB�ł�MOSIA/MISOA/CLKA
-> FUNCSEL1 = 30,29.31      / -> FUNCSEL2 = 8,7,9
* PORTA�ł�MOSIB/MISOB/CLKB / PORTB�ł�MOSIB/MISOB/CLKB
-> FUNCSEL2 = 19,18,20      / -> FUNCSEL1 = 4-18,3-17,5-19 (FUNCSEL2 = 14,13,15)

2.�ʐM���x�E�����ݒ�֐�
- frequency(int sp, uint8_t PRESCALE, uint8_t SCRDV, int ms,uint8_t size)
sp = select port
PRESCALE & SCRDV -> Clock Devide
ms = master/slave select
size = transmit data size (0x07 or 0x0F)

3.�`�b�v�Z���N�g�̒�`
- init_cs(int cs)
�s���A�T�C���\���w�肷��s���ԍ��������ɓ���
�w��s�����̐�����init_cs���L�ڂ���K�v������.

4.�������݁E�ǂݍ��݊֐�
- spi_write�֐���master��p�̏������݊֐�
�߂�l��VOR_SPI->BANK[sp].DATA;�@��ݒ肵�Ă����
�ϐ�/�z�� = spi_write(select port , data);
�Ƃ���΃f�[�^�擾��ϐ�/�z��Ɋi�[���\�ł���

slave��p��receive�֐���reply�֐�
��{�ݒ��spi_write�֐��Ɠ��l
receive�֐��́u���������R�}���h�w����������v�ƂȂ�悤��
�����ɋL�ڂ���֐��iMbed��SPI�ʐM�iSlave�j�̏������ɉ��킹�Ē�`�j

5.���荞�݊֐��@2022�N5��11��
- �{���ɕK�v�H
�uCS��LOW��Ԃ̂܂܂ŒʐM���o���Ȃ��Ȃ��Ă��܂��v
�uFIFO�ւ̃f�[�^�����[�h��1�񂵂����s����Ȃ��v
�Ƃ������ɑ΂���I/O�ɂ��\�t�g�E�F�ACS����ɂ��邱�Ƃ�
CS��C�ӂ�LOW��ԂɈێ��ł��C1Byte��A���ő��M���邽��Buffer�v�����ӂ�邱�Ƃ��Ȃ�
-> �������Ԍv������ł͕��A������K�v����H������I/O�s���ł̃`�b�v�Z���N�g����̈�
���g�Ŋ��荞�݊֐����Ē�`����K�v�����邱�Ƃɒ��ӁI�I
****************************************************************************************/

#include "pp_spi_v2.0.h"

volatile int SPI_WRITE_CNT = 0;
volatile int SPI_READ_CNT = 0;

/************************* spi function *************************/

//Debag1 SD�J�[�h��pp_spi�Ŏg�p���Ă����ݒ�֐��ł����{���Ă݂�
void spi(int sp,int mosi,int miso,int clk, int ms){
  VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= 
    ((1<<PERIPH_CLK_ENAB_PORTIO) | (1<<PERIPH_CLK_ENAB_IOCONFIG)
     | (1<<PERIPH_CLK_ENAB_PORTA) | (1<<PERIPH_CLK_ENAB_PORTB) | (1<<PERIPH_CLK_ENAB_IRQSEL));
  switch(sp){
    case 0://spi_a
      VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= (1<<PERIPH_CLK_ENAB_SPIA);
      if(ms == 1){
        VOR_IOCONFIG->PORTA[28] |= (FUNCSEL1 << IOCONFIG_PORTA_FUNSEL_Pos);
      }
      if(ms == 0){
        VOR_IOCONFIG->PORTA[28] |= (FUNCSEL0 << IOCONFIG_PORTA_FUNSEL_Pos);
      }
      VOR_IOCONFIG->PORTA[mosi] |= (FUNCSEL1 << IOCONFIG_PORTA_FUNSEL_Pos);        // Set pin function for SPIA_MOSI def29
      VOR_IOCONFIG->PORTA[miso] |= (FUNCSEL1 << IOCONFIG_PORTA_FUNSEL_Pos);        // Set pin function for SPIA_MISO def30
      VOR_IOCONFIG->PORTA[clk] |= (FUNCSEL1 << IOCONFIG_PORTA_FUNSEL_Pos);        // Set pin function for SPIA_SCLK def31
      break;
    case 1://spi_b
//      VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= (1<<PERIPH_CLK_ENAB_SPIB);
//      VOR_IOCONFIG->PORTB[mosi] |= (FUNCSEL1 << IOCONFIG_PORTB_FUNSEL_Pos);
//      VOR_IOCONFIG->PORTB[miso] |= (FUNCSEL1 << IOCONFIG_PORTB_FUNSEL_Pos);
//      VOR_IOCONFIG->PORTB[clk] |= (FUNCSEL1 << IOCONFIG_PORTB_FUNSEL_Pos);
      printf("SPIB setting\n");
      VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= (1<<PERIPH_CLK_ENAB_SPIB);
      VOR_IOCONFIG->PORTA[mosi] |= (FUNCSEL2 << IOCONFIG_PORTA_FUNSEL_Pos);
      VOR_IOCONFIG->PORTA[miso] |= (FUNCSEL2 << IOCONFIG_PORTA_FUNSEL_Pos);
      VOR_IOCONFIG->PORTA[clk] |= (FUNCSEL2 << IOCONFIG_PORTA_FUNSEL_Pos);
      break;
    case 2://spi_c
      VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= (1<<PERIPH_CLK_ENAB_SPIC);
      break;
  }
}

void init_cs(int cs){
  VOR_GPIO->BANK[0].DIR |= (1 << cs);
  VOR_GPIO->BANK[0].DATAMASK |= (1 << cs);
}

void cs(int pin_num,int cs){
  switch(cs){
    case 0:
      VOR_GPIO->BANK[0].DATAOUT &= ~(1 << pin_num);
      break;
    case 1:
      VOR_GPIO->BANK[0].DATAOUT |= (1 << pin_num);
      break;
  }
}

void frequency(int sp, uint8_t PRESCALE, uint8_t SCRDV, int ms,uint8_t size){
    switch(ms){
    case 0:
      VOR_SPI->BANK[sp].CLKPRESCALE = PRESCALE ;
      VOR_SPI->BANK[sp].CTRL0 = (SCRDV<<SPI_PERIPHERAL_CTRL0_SCRDV_Pos 
                                 | size << SPI_PERIPHERAL_CTRL0_SIZE_Pos); //size = 7(8bit)
      VOR_SPI->BANK[sp].CTRL0 &= ~(SPI_PERIPHERAL_CTRL0_SPO_Msk 
                                   | SPI_PERIPHERAL_CTRL0_SPH_Msk); // Polarity and phase = 0x0
      break;
    case 1:
      VOR_SPI->BANK[sp].CLKPRESCALE = PRESCALE ;
      VOR_SPI->BANK[sp].CTRL0 = size << SPI_PERIPHERAL_CTRL0_SIZE_Pos;
      VOR_SPI->BANK[sp].CTRL0 &= ~(SPI_PERIPHERAL_CTRL0_SPO_Msk
                                   | SPI_PERIPHERAL_CTRL0_SPH_Msk);
      break;
  }
}

// SS<<SPI_PERIPHERAL_CTRL1_SS_Pos �̓n�[�h�E�F�A�I�ɗp�ӂ��ꂽCS�s��
// �����ؗp�Ƃ��ă������c��
uint8_t spi_write(int sp,uint8_t data){// master transmit data
  int busy_loop_cnt = 0;
  int k = 1;
  VOR_SPI->BANK[sp].CTRL1 |= (SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk 
                              | SPI_PERIPHERAL_CTRL1_BLOCKMODE_Msk);
  SPI_WRITE_CNT = SPI_WRITE_CNT + 1;
  if(SPI_WRITE_CNT == 12*k){
    VOR_SPI->BANK[sp].FIFO_CLR = (SPI_PERIPHERAL_FIFO_CLR_TXFIFO_Msk 
                                  | SPI_PERIPHERAL_FIFO_CLR_RXFIFO_Msk);
    k = k + 1;
    if(k == 21){
      SPI_WRITE_CNT = 0;
    }
  }
  VOR_SPI->BANK[sp].DATA = data;
  
  VOR_SPI->BANK[sp].CTRL1 |= (SPI_PERIPHERAL_CTRL1_ENABLE_Msk);      // enable block
  VOR_SPI->BANK[sp].CTRL1 &=  ~(SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk);  // clr pause bit
//  while((VOR_SPI->BANK[0].STATUS & SPI_PERIPHERAL_STATUS_BUSY_Msk) !=0) {
  while((VOR_SPI->BANK[sp].STATUS & SPI_PERIPHERAL_STATUS_BUSY_Msk) !=0) {
    busy_loop_cnt ++ ;
  }
  return VOR_SPI->BANK[sp].DATA;
}

uint8_t spi_receive(int sp){//CMD Receive Confirming
  VOR_SPI->BANK[sp].CTRL1 = (1<<SPI_PERIPHERAL_CTRL1_MS_Pos 
                             | SPI_PERIPHERAL_CTRL1_BLOCKMODE_Msk);
//  VOR_SPI->BANK[sp].DATA = 0xFC;
  VOR_SPI->BANK[sp].CTRL1 |= (SPI_PERIPHERAL_CTRL1_ENABLE_Msk );
  return VOR_SPI->BANK[sp].DATA;
}

uint8_t spi_reply(int sp,uint8_t data){// reply data for request
  int j = 1;
  int busy_loop_cnt = 0;
  VOR_SPI->BANK[sp].CTRL1 = (1<<SPI_PERIPHERAL_CTRL1_MS_Pos 
                             | SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk | SPI_PERIPHERAL_CTRL1_BLOCKMODE_Msk);
  
  SPI_READ_CNT = SPI_READ_CNT + 1;
  if(SPI_READ_CNT == 12*j){
    VOR_SPI->BANK[sp].FIFO_CLR = (SPI_PERIPHERAL_FIFO_CLR_TXFIFO_Msk 
                                  | SPI_PERIPHERAL_FIFO_CLR_RXFIFO_Msk);
    j = j + 1;
    if(j == 21){
      SPI_READ_CNT = 0;
    }
  }
  
  VOR_SPI->BANK[sp].DATA = data;
  VOR_SPI->BANK[sp].CTRL1 |= (SPI_PERIPHERAL_CTRL1_ENABLE_Msk );
  while((VOR_SPI->BANK[sp].STATUS & SPI_PERIPHERAL_STATUS_BUSY_Msk) !=0) {
    busy_loop_cnt ++ ;
  }
  return VOR_SPI->BANK[sp].DATA;
}

