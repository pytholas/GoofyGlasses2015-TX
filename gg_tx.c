/* gg_tx.c
 * goofy glasses tx toolkit
 * compile: gcc -O2 ht8.c -lftdi -lncurses -o ht8
 * this program transmits dmx-style rgb packets
 * 
 * broadcasts from PC/XBEE S1 to Mrf24j40m/Atmega328
 *
 * Benjamin Jeffery
 * University of Idaho
 * jeff7981@vandals.uidaho.edu
 * 10/14/2015
 *
 * Copyright 2015, Benjamin Jeffery
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public Licese as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <ftdi.h>
#include <stdio.h>
#include <curses.h>

#define DOT 100000
#define DASH 300000
#define DORK 150000
#define DROOL 1000000
#define DAB 500000
#define SLP 40000

static size_t getEncodedBufferSize(size_t sourceSize);
long millisec();
uint8_t sorc[96] = { };
uint8_t dest[96] = { };


int main()
{
  struct ftdi_context *ftdi;
  struct ftdi_device_list *devlist, *curdev;
  char letter;
  char metter;
  char myterm;
  bool news;
  int i = 0;
  int j = 0;
  int k = 0;
  int ret = 0;
  int res;
  int xbee_stat;
  int nbytes;
  int pm;
  int f;
  size_t write_index;
  long strt, stp;
  uint8_t temp[3] = { };
  uint8_t dPack[96] = { };
  uint8_t whitePack[96] = {255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215, 255,255,215};
  uint8_t whit2Pack[96] = {255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255};

  uint8_t redPack[96] = {255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0};
  uint8_t greenPack[96] = {0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0};
  uint8_t bluePack[96] = {0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255};
  uint8_t yellowPack[96] = {255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0};
  uint8_t cyanPack[96] = {0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255};
  uint8_t magentaPack[96] = {255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255,255,0,255};
  uint8_t indegoPack[96] = {75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130,75,0,130};
  uint8_t coralPack[96] = {255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80,255,127,80};
  uint8_t dodgerPack[96] = {30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255,30,144,255};
  uint8_t goldPack[96] = {255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0,255,190,0};
  uint8_t ui1Pack[96] = {255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190};
  uint8_t ui2Pack[96] = {255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0, 255,255,190, 255,190,0};

  uint8_t rain1Pack[96] = {255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 0,0,0, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 0,0,0, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 0,0,0, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 0,0,0, 0,206,209, 186,85,211};
  uint8_t rain2Pack[96] = {186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 75,0,130, 255,127,80, 0,0,0, 255,190,0, 0,0,0, 141,239,79, 0,0,0, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 0,0,0, 255,127,80, 30,144,255, 255,190,0, 0,0,0, 141,239,79, 131,194,209, 0,206,209};
  uint8_t rain3Pack[96] = {0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 0,0,0, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 0,0,0, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 0,0,0, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 0,0,0};
  uint8_t rain4Pack[96] = {0,0,0, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 0,0,0, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 0,0,0, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79};
  uint8_t rain5Pack[96] = {141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 0,0,0, 255,127,80, 30,0,55, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 75,0,0, 255,127,80, 0,0,0, 255,190,0, 220,20,60};
  uint8_t rain6Pack[96] = {220,20,60, 141,239,79, 0,0,0, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 0,0,0, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,0,0, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 0,0,0, 255,127,80, 30,144,255, 255,190,0};
  uint8_t rain7Pack[96] = {255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 75,0,130, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 75,0,130, 255,127,80, 30,144,255};
  uint8_t rain8Pack[96] = {30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 75,0,130, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 75,0,130, 255,127,80};
  uint8_t rain9Pack[96] = {255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 75,0,130, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 75,0,130};
  uint8_t rain10Pack[96] = {75,0,130, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 75,0,130, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255};
  uint8_t rain11Pack[96] = {255,0,255, 75,0,130, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 75,0,130, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255};
  uint8_t rain12Pack[96] = {0,255,255, 255,0,255, 75,0,130, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 75,0,130, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0};
  uint8_t rain13Pack[96] = {255,255,0, 0,255,255, 255,0,255, 75,0,130, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 75,0,130, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255};
  uint8_t rain14Pack[96] = {0,0,255, 255,255,0, 0,255,255, 255,0,255, 75,0,130, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 75,0,130, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0};
  uint8_t rain15Pack[96] = {0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 75,0,130, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 75,0,130, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0};
  uint8_t rain16Pack[96] = {255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 75,0,130, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255, 255,0,0, 0,255,0, 0,0,255, 255,255,0, 0,255,255, 255,0,255, 75,0,130, 255,127,80, 30,144,255, 255,190,0, 220,20,60, 141,239,79, 31,194,109, 0,206,209, 186,85,211, 255,255,255};

  uint8_t twnk1Pack[96] = {55,0,0, 0,0,0, 0,155,0, 0,0,0, 0,0,55, 0,0,0, 155,0,0, 0,0,0, 0,55,0, 0,0,0, 0,0,55, 0,0,0, 55,0,0, 0,0,0, 0,55,0, 0,0,0, 0,0,155, 0,0,0, 55,0,0, 0,0,0, 0,55,0, 0,0,0, 0,0,55, 0,0,0, 155,115,0, 0,0,0, 0,55,0, 0,0,0, 0,0,55, 0,0,0, 55,0,0, 0,0,0};
 uint8_t twnk2Pack[96] = {0,0,0, 155,0,0, 0,0,0, 0,55,0, 0,0,0, 0,0,55, 0,0,0, 55,0,0, 0,0,0, 0,155,0, 0,0,0, 0,0,55, 0,0,0, 55,0,0, 0,0,0, 0,55,0, 0,0,0, 0,0,155, 0,0,0, 55,0,0, 0,0,0, 0,55,0, 0,0,0, 0,0,55, 0,0,0, 155,0,0, 0,0,0, 0,55,0, 0,0,0, 0,0,55, 0,0,0, 155,115,0};
 uint8_t twnk3Pack[96] = {0,0,55, 0,0,0, 55,0,0, 0,0,0, 0,55,0, 0,0,0, 0,0,155, 0,0,0, 55,0,0, 0,0,0, 0,55,0, 0,0,0, 0,0,55, 0,0,0, 155,0,0, 0,0,0, 0,55,0, 0,0,0, 0,0,55, 0,0,0, 55,0,0, 0,0,0, 0,155,0, 0,0,0, 0,0,55, 0,0,0, 55,0,0, 0,0,0, 0,55,0, 0,0,0, 0,0,155, 0,0,0};
 uint8_t twnk4Pack[96] = {155,0,0, 0,0,55, 0,155,0, 255,190,0, 0,0,155, 0,55,0, 155,0,0, 0,0,55, 0,155,0, 55,0,0, 0,0,155, 0,55,0, 155,0,0, 0,0,55, 0,155,0, 55,0,0, 0,0,155, 0,55,0, 155,0,0, 0,0,55, 0,155,0, 55,0,0, 0,0,155, 0,55,0, 155,0,0, 0,0,55, 255,115,0, 55,0,0, 0,0,155, 0,55,0, 155,0,0, 0,0,55};
  uint8_t twnk5Pack[96] = {0,155,0, 255,0,0, 0,0,155, 0,255,0, 155,0,0, 0,0,255, 0,155,0, 255,190,0, 0,0,155, 0,255,0, 155,0,0, 0,0,255, 0,155,0, 255,0,0, 0,0,155, 0,255,0, 155,0,0, 0,0,255, 0,155,0, 255,190,0, 0,0,155, 0,255,0, 155,0,0, 0,0,255, 0,155,0, 255,0,0, 0,0,155, 0,255,0, 155,0,0, 0,0,255, 0,155,0, 255,0,0};
  uint8_t twnk6Pack[96] = {0,0,255, 0,155,0, 255,190,0, 0,0,155, 0,255,0, 155,0,0, 0,0,255, 0,155,0, 255,0,0, 0,0,155, 0,255,0, 155,0,0, 0,0,255, 0,155,0, 255,0,0, 0,0,155, 0,255,0, 155,0,0, 0,0,255, 0,155,0, 255,0,0, 0,0,155, 0,255,0, 155,0,0, 0,0,255, 0,155,0, 255,0,0, 0,0,155, 0,255,0, 155,0,0, 0,0,255, 0,155,0};
  // gold is 255,190,0
  uint8_t twnk7Pack[96] = {155,0,0, 0,0,255, 0,155,0, 255,190,0, 0,0,155, 0,255,0, 155,0,0, 0,0,255, 0,155,0, 255,190,0, 0,0,155, 0,255,0, 155,0,0, 0,0,255, 0,155,0, 255,190,0, 0,0,155, 0,255,0, 155,0,0, 0,0,255, 0,155,0, 255,190,0, 0,0,155, 0,255,0, 155,0,0, 0,0,255, 0,155,0, 255,190,0, 0,0,155, 0,255,0, 155,0,0, 0,0,255};
  uint8_t twnk8Pack[96] = {0,255,0, 255,190,0, 0,0,155, 0,155,0, 255,190,0, 0,0,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,0,0, 0,0,255, 0,255,0, 255,190,0, 0,0,155, 0,255,0, 155,0,0, 0,0,255, 0,155,0, 255,190,0, 0,0,155, 0,255,0, 155,0,0};
  uint8_t twnk9Pack[96] = {255,255,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,0,0, 0,0,255, 0,255,0, 255,190,0, 0,0,155, 0,255,0, 155,0,0, 0,0,255, 0,155,0, 255,190,0, 0,0,155, 0,255,0};
  uint8_t twnk10Pack[96] = {255,190,0, 255,255,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,0,0, 0,0,255, 0,255,0, 255,190,0, 0,0,155, 0,255,0, 155,0,0, 0,0,255, 0,155,0, 255,190,0, 0,0,155};
  uint8_t twnk11Pack[96] = {0,155,0, 255,190,0, 255,255,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,190,0, 0,0,255, 0,255,0, 255,0,0, 0,0,255, 0,255,0, 255,190,0, 0,0,155, 0,255,0, 155,0,0, 0,0,255, 0,155,0, 255,190,0};
  // half as bright
  uint8_t twnk12Pack[96] = {65,60,0, 0,0,0, 150,135,0, 0,0,0, 60,60,55, 0,0,0, 155,140,0, 0,0,0, 70,55,0, 0,0,0, 70,70,55, 0,0,0, 55,40,0, 0,0,0, 70,55,0, 0,0,0, 170,170,155, 0,0,0, 155,140,0, 0,0,0, 70,55,0, 0,0,0, 60,60,55, 0,0,0, 155,115,0, 0,0,0, 70,55,0, 0,0,0, 70,70,55, 0,0,0, 55,50,0, 0,0,0};
 uint8_t twnk13Pack[96] = {0,0,0, 155,140,0, 0,0,0, 60,55,0, 0,0,0, 70,70,55, 0,0,0, 55,50,0, 0,0,0, 170,155,0, 0,0,0, 60,60,55, 0,0,0, 55,50,0, 0,0,0, 60,55,0, 0,0,0, 170,170,155, 0,0,0, 55,50,0, 0,0,0, 60,55,0, 0,0,0, 60,60,55, 0,0,0, 155,140,0, 0,0,0, 60,55,0, 0,0,0, 60,60,55, 0,0,0, 155,115,0};
 uint8_t twnk14Pack[96] = {60,60,55, 0,0,0, 55,50,0, 0,0,0, 60,55,0, 0,0,0, 160,160,155, 0,0,0, 55,50,0, 0,0,0, 60,55,0, 0,0,0, 60,60,55, 0,0,0, 155,130,0, 0,0,0, 60,55,0, 0,0,0, 60,60,55, 0,0,0, 55,50,0, 0,0,0, 160,155,0, 0,0,0, 60,60,55, 0,0,0, 55,50,0, 0,0,0, 60,55,0, 0,0,0, 160,160,155, 0,0,0};
 uint8_t twnk15Pack[96] = {155,150,0, 60,60,55, 160,155,0, 255,190,0, 160,160,155, 60,55,0, 155,150,0, 60,60,55, 160,155,0, 55,50,0, 160,160,155, 60,55,0, 155,150,0, 60,60,55, 160,155,0, 55,50,0, 160,160,155, 60,55,0, 155,150,0, 60,60,55, 160,155,0, 55,50,0, 160,160,155, 60,55,0, 155,150,0, 60,60,55, 255,190,0, 55,50,0, 160,160,155, 60,55,0, 155,150,0, 60,60,55};
 uint8_t glowPack[96] = {155,150,0, 60,60,0, 160,155,0, 155,90,0, 160,160,0, 60,55,0, 155,150,0, 60,60,0, 160,155,0, 55,50,0, 160,160,0, 60,55,0, 155,150,0, 60,60,0, 160,155,0, 55,50,0, 160,160,0, 60,55,0, 155,150,0, 60,60,0, 160,155,0, 55,50,0, 160,160,0, 60,55,0, 155,150,0, 60,60,0, 255,190,0, 55,50,0, 160,160,0, 60,55,0, 155,150,0, 60,60,0};
  uint8_t rnPack[96] = {255,0,0, 255,0,0, 255,0,0, 0,0,0, 0,255,0, 0,255,0, 0,255,0, 0,0,0, 0,0,255, 0,0,255, 0,0,255, 0,0,0, 255,255,0, 255,255,0, 255,255,0, 0,0,0, 0,255,255, 0,255,255, 0,255,255, 0,0,0, 255,0,255, 255,0,255, 255,0,255, 0,0,0, 255,255,215, 255,255,215, 255,255,215, 0,0,0, 255,215,0, 255,215,0, 255,215,0, 0,0,0};
  uint8_t igPack[96] = {255,255,215, 255,255,215, 255,255,215, 0,0,0, 255,215,0, 255,215,0, 255,215,0, 0,0,0, 255,255,215, 255,255,215, 255,255,215, 0,0,0, 255,215,0, 255,215,0, 255,215,0, 0,0,0, 255,255,215, 255,255,215, 255,255,215, 0,0,0, 255,215,0, 255,215,0, 255,215,0, 0,0,0, 255,255,215, 255,255,215, 255,255,215, 0,0,0, 255,215,0, 255,215,0, 255,215,0, 0,0,0};
 uint8_t itPack[96] = {255,255,215, 255,255,215, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 255,215,0, 255,215,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 255,215,0, 255,215,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 255,215,0, 255,215,0, 0,0,0, 0,0,0};

  size_t n = sizeof(whitePack);
  size_t l = sizeof(dest);
  size_t m = getEncodedBufferSize(l);

  printf("Hello, welcome to Ben's Halftime toolkit!\n");

  // XBEE init 
  if ((ftdi = ftdi_new()) == 0)
    {
      fprintf(stderr, "ftdi_new failed\n");
      return 1;
    } else {
    fprintf(stderr, "ftdi_new success\n");
  }
  if ((res = ftdi_usb_find_all(ftdi, &devlist, 0x0403, 0x6001)) <0) {
    fprintf(stderr, "no ftdi devices found\n");
    ftdi_list_free(&devlist);
    ftdi_free(ftdi);
    return 1;
  } else {
    fprintf(stderr, "%d ftdi devices found.\n", res);
  }

  // open ftdi context
  if ((ret = ftdi_usb_open_dev(ftdi, devlist[0].dev)) < 0)
    {
      fprintf(stderr, "unable to open ftdi: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
      ftdi_free(ftdi);
      return ret;            
    }
  else {
    fprintf(stderr, "ftdi_open successful\n");
  }

  ret = ftdi_set_baudrate(ftdi, 57600);
  if (ret < 0) {
    fprintf(stderr, "unable to set baud rate: %d (%s).\n", ret, ftdi_get_error_string(ftdi));
  } else {
    printf("baudrate set.\n");
  }

  f = ftdi_set_line_property(ftdi, 8, STOP_BIT_1, NONE);
  if(f < 0) {
    fprintf(stderr, "unable to set line parameters: %d (%s).\n", ret, ftdi_get_error_string(ftdi));
  } else {
    printf("line parameters set.\n");
  }

  ftdi_list_free(&devlist);
  printf("broadcasting.\n");

  //open curses session
  initscr();
  raw();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  noecho();
  attron(A_BOLD);
  printw("a=twinkle routine; b=blue flash; c=coral flash; d=dark;  e=green flash; \n");
  printw("f=twink8 flash; g=gold flash; h=twink9; i=idaho; j=twink10; k=cyan flash \n");
  printw("l= twink11; m=magenta; n=gold on; o=white on; p=rainbow med \n");
  printw("q=rainbow cycle; r=red flash; s=rainbow short;  \n");
  printw("v=vandal morse; w=white flash; x=low gold on\n");
  printw("y=yellow flash; z=slow twinkle;  \n");
  printw(" [=marqee one way; ]=marqee the other\n  ");
  printw("  \n");
 printw("comma key <,> to stop the loops\n");
  printw("TYPING IN CAPS FLASHES LETTERS IN MORSE CODE\n");
  printw("dot key <.> to quit\n");

  do {
    letter = getch();
    switch(letter) {
    case 'A':    // A morse
      printw("%c", letter);
      refresh();
      // a DOT DASH
      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'b':  // blue flash
      nbytes = ftdi_write_data(ftdi, bluePack, m);
      usleep(SLP);
      break;

    case 'B':   //blue morse
      printw("%c", letter);
      refresh();
      // b DASH DOT DOT DOT
      nbytes = ftdi_write_data(ftdi, bluePack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, bluePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, bluePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, bluePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'c':  // coral flash
      nbytes = ftdi_write_data(ftdi, coralPack, m);
      usleep(SLP);
      break;

    case 'C':    // coral morse
      printw("%c", letter);
      refresh();
      // DASH DOT DASH DOT
      nbytes = ftdi_write_data(ftdi, coralPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      
      nbytes = ftdi_write_data(ftdi, coralPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      
      nbytes = ftdi_write_data(ftdi, coralPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      
      nbytes = ftdi_write_data(ftdi, coralPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'd':  // dark
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(SLP);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(SLP);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(SLP);
      break;

    case 'D':    //dodger blue morse
      printw("%c", letter);
      refresh();
      // d DASH DOT DOT
      nbytes = ftdi_write_data(ftdi, dodgerPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, dodgerPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, dodgerPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'e':  // green flash
      nbytes = ftdi_write_data(ftdi, greenPack, m);
      usleep(SLP);
      break;

    case 'E':  // green morse
      // e dot
      printw("%c", letter);
      refresh();
      nbytes = ftdi_write_data(ftdi, greenPack, m);
      usleep(DOT);
      break;

    case 'f':   // twnk8 flash
      nbytes = ftdi_write_data(ftdi, twnk8Pack, m);
      usleep(SLP);
      break;

    case 'F':    // morse
      printw("%c", letter);
      refresh();
      // f DOT DOT DASH DOT
      nbytes = ftdi_write_data(ftdi, redPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, redPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, redPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, redPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'g':  // gold flash
      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(SLP);
      break;

    case 'G':    // morse
      printw("%c", letter);
      refresh();
      // g DASH DASH DOT
      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'h':   // twnk9 flash
      nbytes = ftdi_write_data(ftdi, twnk9Pack, m);
      usleep(SLP);
      break;

    case 'H':   //twnk9 morse
      printw("%c", letter);
      refresh();
      // h DOT DOT DOT DOT
      nbytes = ftdi_write_data(ftdi, twnk9Pack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, twnk9Pack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, twnk9Pack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, twnk9Pack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

      // i below

    case 'I':  // morse
      printw("%c", letter);
      refresh();
      // i DOT DOT
      nbytes = ftdi_write_data(ftdi, indegoPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, indegoPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'j':   // tnk10 flash
      nbytes = ftdi_write_data(ftdi, twnk10Pack, m);
      usleep(SLP);
      break;

    case 'J':   // morse
      printw("%c", letter);
      refresh();
      // j DOT DASH DASH DASH
      nbytes = ftdi_write_data(ftdi, twnk10Pack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, twnk10Pack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, twnk10Pack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, twnk10Pack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'k':  // yellow flash
      nbytes = ftdi_write_data(ftdi, yellowPack, m);
      usleep(SLP);
      break;

    case 'K':   // morse
      printw("%c", letter);
      refresh();
      // k DASH DOT DASH
      nbytes = ftdi_write_data(ftdi, yellowPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, yellowPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, yellowPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'l':   // tnk11 flash
      nbytes = ftdi_write_data(ftdi, twnk11Pack, m);
      usleep(SLP);
      break;

    case 'L':   // morse
      printw("%c", letter);
      refresh();
      // l DOT DASH DOT DOT
      nbytes = ftdi_write_data(ftdi, twnk11Pack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, twnk11Pack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, twnk11Pack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, twnk11Pack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'm':  // magenta flash
      nbytes = ftdi_write_data(ftdi, magentaPack, m);
      usleep(SLP);
      break;

    case 'M':   // morse
      printw("%c", letter);
      refresh();
      // m DASH DASH
      nbytes = ftdi_write_data(ftdi, magentaPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, magentaPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'n':  // gold hold
      do {
	myterm = getch();
	nbytes = ftdi_write_data(ftdi, goldPack, m);
	usleep(DORK);
      } while (myterm != ','); 
      break;

    case 'N':   // morse
      printw("%c", letter);
      refresh();
      // n DASH DOT
      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'o':  // white hold
      do {
	myterm = getch();
	nbytes = ftdi_write_data(ftdi, whitePack, m);
	usleep(DOT);
      } while (myterm != ',');  
      break;

    case 'O':   //dodger morse
      printw("%c", letter);
      refresh();
      // o DASH DASH DASH
      nbytes = ftdi_write_data(ftdi, dodgerPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, dodgerPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, dodgerPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DASH);
      break;

    case 'p':  // rainbow med
      nbytes = ftdi_write_data(ftdi, rain5Pack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, rain6Pack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, rain7Pack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, rain8Pack, m);
      usleep(DASH);
      break;

    case 'P':  // morse
      printw("%c", letter);
      refresh();
      // DOT DASH DASH DOT
      nbytes = ftdi_write_data(ftdi, coralPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, coralPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, coralPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, coralPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

      // 'q' coded below
    case 'Q':  // morse
      printw("%c", letter);
      refresh();
      // q DASH DASH DOT DASH
      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'r':  // red flash
      nbytes = ftdi_write_data(ftdi, redPack, m);
      usleep(SLP);
      break;

    case 'R':     // red morse
      printw("%c", letter);
      refresh();
      // r DOT DASH DOT
      nbytes = ftdi_write_data(ftdi, redPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, redPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, redPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 's':  // rainbow short
      nbytes = ftdi_write_data(ftdi, rain1Pack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, rain2Pack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, rain3Pack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, rain4Pack, m);
      usleep(DASH);
      break;

    case 'S':  // morse
      printw("%c", letter);
      refresh();
      // s DOT DOT DOT
      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'T':  // morse
      printw("%c", letter);
      refresh();
      // t DASH
      nbytes = ftdi_write_data(ftdi, greenPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'U':  // morse
      printw("%c", letter);
      refresh();
      // u DOT DOT DASH
      nbytes = ftdi_write_data(ftdi, bluePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, bluePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, bluePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DASH);
      break;

    case 'v':   // vandals morse
      // v DOT DOT DOT DASH
      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      usleep(DASH);

      // a DOT DASH
      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      usleep(DASH);

      // n DASH DOT
      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      usleep(DASH);

      // d DASH DOT DOT
      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      usleep(DASH);

      // a DOT DASH
      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      usleep(DASH);

      // l DOT DASH DOT DOT
      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      usleep(DASH);

      // s DOT DOT DOT
      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      usleep(DASH);
      break;

    case 'V':  // morse
      printw("%c", letter);
      refresh();
      // v DOT DOT DOT DASH
      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'w':  // white flash
      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(SLP);
      break;

    case 'W':  // morse
      printw("%c", letter);
      refresh();
      // w DOT DASH DASH
      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'x':  // low gold hold
      do {
	myterm = getch();
	nbytes = ftdi_write_data(ftdi, glowPack, m);
	usleep(DROOL);
      } while (myterm != ','); 
      break;

    case 'X':  // morse
      printw("%c", letter);
      refresh();
      // x DASH DOT DOT DASH
      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'y':  // yellow flash
      nbytes = ftdi_write_data(ftdi, yellowPack, m);
      usleep(SLP);
      break;

    case 'Y':  // morse
      printw("%c", letter);
      refresh();
      // y DASH DOT DASH DASH
      nbytes = ftdi_write_data(ftdi, yellowPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, yellowPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, yellowPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, yellowPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'z':
      do {
	metter = getch();
	nbytes = ftdi_write_data(ftdi, twnk12Pack, m);
	usleep(DASH);
	usleep(DASH);
	nbytes = ftdi_write_data(ftdi, twnk13Pack, m);
	usleep(DASH);
	usleep(DASH);
	nbytes = ftdi_write_data(ftdi, twnk14Pack, m);
	usleep(DASH);
	usleep(DASH);
	nbytes = ftdi_write_data(ftdi, twnk15Pack, m);
	usleep(DASH);
	usleep(DASH);
      } while (metter != ',');      
      break;

    case 'Z':  // morse
      printw("%c", letter);
      refresh();
      // z DASH DASH DOT DOT
      nbytes = ftdi_write_data(ftdi, indegoPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, indegoPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, indegoPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, indegoPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      break;

    case 'u':  // go morse
      printw("%c", letter);
      refresh();
      // g1 DASH DASH DOT
      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      
      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      
      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      usleep(DASH);

      // o1 DASH DASH DASH
      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      
      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      
      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DASH);
      usleep(DOT);

      //      g2 DASH DASH DOT
      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      
      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      
      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      usleep(DASH);

      // o2 DASH DASH DASH
      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      
      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      
      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DASH);
      usleep(DOT);

      // g3 DASH DASH DOT
      nbytes = ftdi_write_data(ftdi, ui1Pack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      
      nbytes = ftdi_write_data(ftdi, ui2Pack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      
      nbytes = ftdi_write_data(ftdi, ui1Pack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      usleep(DASH);

      //      o3 DASH DASH DASH
      nbytes = ftdi_write_data(ftdi, ui2Pack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      
      nbytes = ftdi_write_data(ftdi, ui1Pack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      
      nbytes = ftdi_write_data(ftdi, ui2Pack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DASH);
      break;

    case 'i':   // idaho morse
      printw("%c", letter);
      refresh();
     // i dot dot (white)
      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      
      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      usleep(DASH);
      // d dash dot dot (gold)
      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      usleep(DASH);
      // a dot dash (white)
      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      usleep(DASH);
      // h dot dot dot dot (gold)
      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);

      nbytes = ftdi_write_data(ftdi, goldPack, m);
      usleep(DOT);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      usleep(DASH);
      // o dash dash dash (white)
      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      
      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DOT);
      
      nbytes = ftdi_write_data(ftdi, whitePack, m);
      usleep(DASH);
      nbytes = ftdi_write_data(ftdi, dPack, m);
      usleep(DASH);
      usleep(DOT);
      break;

    case 'a':  //asterion
      printw("%c", letter);
      refresh();
      // twink a little 4 beats
      for(i=0; i<4; i++) {
	nbytes = ftdi_write_data(ftdi, twnk1Pack, m);
	usleep(DASH);
	
	nbytes = ftdi_write_data(ftdi, twnk2Pack, m);
	usleep(DASH);

	nbytes = ftdi_write_data(ftdi, twnk3Pack, m);
	usleep(DASH);

	nbytes = ftdi_write_data(ftdi, twnk1Pack, m);
	usleep(DASH);
      }

      // twink more 4 beats
      for(i=0; i<4; i++) {
	nbytes = ftdi_write_data(ftdi, twnk4Pack, m);
	usleep(DORK);
	
	nbytes = ftdi_write_data(ftdi, twnk5Pack, m);
	usleep(DORK);

	nbytes = ftdi_write_data(ftdi, twnk6Pack, m);
	usleep(DORK);

	nbytes = ftdi_write_data(ftdi, twnk7Pack, m);
	usleep(DORK);
      }
      // twink emphatically 4 beats
      for(i=0; i<8; i++) {
	nbytes = ftdi_write_data(ftdi, twnk8Pack, m);
	usleep(DOT);
	
	nbytes = ftdi_write_data(ftdi, twnk9Pack, m);
	usleep(DOT);

	nbytes = ftdi_write_data(ftdi, twnk10Pack, m);
	usleep(DOT);

	nbytes = ftdi_write_data(ftdi, twnk11Pack, m);
	usleep(DOT);
      }
      usleep(DASH);
      usleep(DASH);
      break;

    case 'q':   // rainbow cycle
      do {
	metter = getch();
	nbytes = ftdi_write_data(ftdi, rain1Pack, m);
	usleep(SLP);
	nbytes = ftdi_write_data(ftdi, dPack, m);
	usleep(SLP);
	nbytes = ftdi_write_data(ftdi, rain2Pack, m);
	usleep(SLP);
	nbytes = ftdi_write_data(ftdi, dPack, m);
	usleep(SLP);
	nbytes = ftdi_write_data(ftdi, rain3Pack, m);
	usleep(SLP);
	nbytes = ftdi_write_data(ftdi, dPack, m);
	usleep(SLP);
	nbytes = ftdi_write_data(ftdi, rain4Pack, m);
	usleep(SLP);
	nbytes = ftdi_write_data(ftdi, dPack, m);
	usleep(SLP);
      } while (metter != ',');      
      break;

    case '[':   // marqee left
      // load the broadcast array
      for (i=0; i<96; i++) {
	sorc[i] = igPack[i];
      }
      // loop
      do {
	myterm = getch();
	// store the rollover values
	for (i=0; i<3; i++) {
	  temp[i] = sorc[i];
	}
	for (i=0; i<96; i++) {
	  if(i<93)
	    sorc[i] = sorc[i+3];
	  else sorc[i] = temp[i%3];
	}
	nbytes = ftdi_write_data(ftdi, sorc, m);
        usleep(DAB);
      } while (myterm != ',');  
      break;

    case ']':   // marqee right
      for (i=0; i<96; i++) {
	sorc[i] = igPack[i];
      }

      do {
	myterm = getch();
	for (i=93; i<96; i++) {
	  temp[i-93] = sorc[i];
	}
	for (i=95; i>-1; i--) {
	  if(i>2)
	    sorc[i] = sorc[i-3];
	  else sorc[i] = temp[i];
	}
	nbytes = ftdi_write_data(ftdi, sorc, m);
        usleep(DAB);
      } while (myterm != ',');  
      break;

    default:
      usleep(DAB);
    }
    nbytes = ftdi_write_data(ftdi, dPack, m);
  } while (letter != '.');
  
  endwin();   // close curses session
  
  if ((ret = ftdi_usb_close(ftdi)) < 0)
    {
      fprintf(stderr, "unable to close ftdi1: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
      return 1;
    }

  ftdi_free(ftdi);
  
  printf("End of program.\n");
  
  return 0;
} // END main




static size_t getEncodedBufferSize(size_t sourceSize)
{
  size_t s;
  s = sourceSize + sourceSize / 254 + 1;
  //printf("buffer size is : %zd.\n", s);
  return s;
}



