/*
 * test_proj.c
 *
 * Created: 4/14/2020 6:21:50 PM
 * Author : kerickson
 */

//#include <avr/io.h>




#include "avr.h"

#include "lcd.h"

#define A 57
#define As 54
#define B 51
#define C 48
#define Cs 45
#define D 43
#define Ds 40
#define E 38
#define F 36
#define Fs 34
#define G 32
#define Gs 30
#define a 28
#define as 27
#define b 25
#define c 24
#define cs 23
#define d 21
#define ds 20
#define e 19
#define f 18
#define fs 17
#define g 16
#define gs 15
#define restn 0
#define aa 14
#define aas 13

#define W 50000
#define H 25000
#define Q 12500
#define I 6250 //eight

struct dt{
   int year;
   int month;
   int day;
   int hour;
   int minute;
   int second;
   //int subsecond;
};

struct Note{
   int freq;
   int dur;
};

void modavr_wait(unsigned short nsec){
   TCCR0 = 2;
   while (nsec--) {
      TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 8) * 0.00002);
      SET_BIT(TIFR, TOV0);
      while (!GET_BIT(TIFR, TOV0));
   }
   TCCR0 = 0;
}

void play_note(int freq, int duration){
   // int k = #cycles?
   int i;
   int k = duration/(freq*2);
   for (i = 0; i<k; ++i){
      SET_BIT(PORTB, 3);
      modavr_wait(freq);
      CLR_BIT(PORTB, 3);
      modavr_wait(freq);
   }
}

void play_rest(){
   myavr_wait(400);
}

void play_song(const struct Note SONG[], int N){
   int i = 0;
   int k = 0;
   //for (i=0; i<N; ++i)
   while(i < N){
      k = get_key();
      if(4==k){
         break;
      }
      if(!SONG[i].freq){
         play_rest();
         ++i;
         continue;
      }
      else{
         play_note(SONG[i].freq, SONG[i].dur);
         ++i;
      }
   }
}


/*void wait(){
	volatile int i;
	for(i = 0; i<10000; ++i){
 //pass
	}
 }*/

void
myavr_wait(unsigned short msec)
{
   TCCR0 = 3;
   while (msec--) {
      TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
      SET_BIT(TIFR, TOV0);
      while (!GET_BIT(TIFR, TOV0));
   }
   TCCR0 = 0;
}


int is_pressed(int r, int cc){
   //set all pins to N/C
   DDRC = 0;
   PORTC = 0;
   //int check = 0;
   /*for(check = 0; check<r; ++check){
    SET_BIT(PORTB, 0);
    myavr_wait(500);
    CLR_BIT(PORTB, 0);
    myavr_wait(500);
    }
    myavr_wait(2000);*/

   int row = 7 -r;
   SET_BIT(DDRC, row);
   CLR_BIT(PORTC, row);

   CLR_BIT(DDRC, cc);
   SET_BIT(PORTC, cc);
   avr_wait(1);
   if (GET_BIT(PINC, cc)){
      //if NOT 0, return NOT PRESSED
      return 0;
   }
   return 1;



   //set r pin to strong 0 , set c pin to weak 1
   //read PIN @c
}


int get_key(void){
   int r, co;
   /*SET_BIT(PORTB, 0);
    myavr_wait(500);
    CLR_BIT(PORTB, 0);
    myavr_wait(500);*/
   //r = 3; r>-1; --r
   for (r = 3; r>-1; --r){
      for(co = 0; co<4; ++co){
         if(is_pressed(r,co)){
            return (co*4)+(3-r)+1;
         }
      }
   }
   return 0;
}


/*void
 myavr_wait(unsigned short msec)
 {
	TCCR0 = 3;
	while (msec--) {
 TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
 SET_BIT(TIFR, TOV0);
 while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
 }*/


int checkmonth(int m, int y){
   int leaptable[] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   int mtable[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   if((y%4)!=0){
      return mtable[m];
   }
   else if ((y %4)==0 &&  (y%100)!=0){
      return leaptable[m];
   }
   else if((y%100)==0 && (y%400)==0){
      return leaptable[m];
   }
   return mtable[m];
}


void increment_days(struct dt *dt){
   int maxdays = checkmonth(dt->month, dt->year);
   if (maxdays==dt->day){
      ++dt->month;
      dt->day = 1;
      if (dt->month > 12){
         dt->month = 1;
         ++dt->year;
      }
   }
   else{
      ++dt->day;
   }
}


void increment_dt(struct dt *dt){
   ++dt->second;
   if(60==dt->second){
      dt->second = 0;
      ++dt->minute;
      if (60==dt->minute){
         dt->minute = 0;
         ++dt->hour;
         if (24==dt->hour){
            dt->hour = 0;
            increment_days(dt);
         }
      }
   }
}

void display_dt(struct dt *dt){
   char buf[17];
   char bufb[17];
   sprintf(buf, "%02d/%02d/%04d", dt->month, dt->day, dt->year);
   sprintf(bufb, "%02d:%02d:%02d", dt->hour, dt->minute, dt->second);
   lcd_clr();
   lcd_pos(0,3);
   lcd_puts(buf);
   lcd_pos(1,0);
   lcd_puts(bufb);
}



void changeday(struct dt *dt){
   char daybuf[17];
   sprintf(daybuf, "**/**/****");
   lcd_pos(0,3);
   lcd_puts(daybuf);
   lcd_pos(0,3);
   int i = 0;
   for (i = 3; i<=10; i=i+3){
      lcd_pos(0,i);

      if(3==i){
         int gk = 0;
         while (gk!=16){
            myavr_wait(400);
            gk = get_key();
            if(!gk){
               continue;
            }
            else if(1==gk){
               ++dt->month;
               if(dt->month > 12){
                  dt->month = 1;
               }
               char mbuf[3];
               sprintf(mbuf, "%02d", dt->month);
               lcd_puts(mbuf);
               lcd_pos(0,i);
               continue;
            }
            else if(2==gk){
               --dt->month;
               if (dt->month < 1){
                  dt->month = 12;
               }
               char ambuf[3];
               sprintf(ambuf, "%02d", dt->month);
               lcd_puts(ambuf);
               lcd_pos(0,i);
               continue;
            }
         }
         continue;
      }
      else if(6==i){
         int hk = 0;
         while (hk!=16){
            myavr_wait(400);
            hk = get_key();
            if(!hk){
               continue;
            }

            else if(1==hk){
               ++dt->day;
               int daymax = checkmonth(dt->month, dt->year);
               if (dt->day > daymax){
                  dt->day = 1;
               }
               char dbuf[3];
               sprintf(dbuf, "%02d", dt->day);
               lcd_puts(dbuf);
               lcd_pos(0, i);
               continue;
            }

            else if(2==hk){
               --dt->day;
               int daymin = checkmonth(dt->month, dt->year);
               if (dt->day < 1){
                  dt->day = daymin;
               }
               char adbuf[3];
               sprintf(adbuf, "%02d", dt->day);
               lcd_puts(adbuf);
               lcd_pos(0, i);
               continue;
            }
         }
         //loop back for loop
         continue;
      }
      else{
         //deal with year here...
         int yk = 0;
         while (yk!=16){
            myavr_wait(400);
            yk = get_key();
            if(!yk){
               continue;
            }
            else if(1==yk){
               ++dt->year;
               char ybuf[5];
               sprintf(ybuf,"%04d",dt->year);
               lcd_puts(ybuf);
               lcd_pos(0,i);
            }
            else if(2==yk){
               --dt->year;
               char aybuf[5];
               sprintf(aybuf,"%04d",dt->year);
               lcd_puts(aybuf);
               lcd_pos(0,i);
            }
         }
         continue;
      }

   }
   return;
}

void changesec(struct dt *dt){
   char daybuf[17];
   sprintf(daybuf, "**:**:**");
   lcd_pos(1,0);
   lcd_puts(daybuf);
   lcd_pos(1,0);
   int i = 0;
   for (i = 0; i<=7; i=i+3){
      lcd_pos(1,i);

      if(0==i){
         //deal with HOUR...
         int gk = 0;
         while (gk!=16){
            myavr_wait(400);
            gk = get_key();
            if(!gk){
               continue;
            }
            else if(1==gk){
               ++dt->hour;
               if(dt->hour > 23){
                  dt->hour = 0;
               }
               char mbuf[3];
               sprintf(mbuf, "%02d", dt->hour);
               lcd_puts(mbuf);
               lcd_pos(1,i);
               continue;
            }
            else if(2==gk){
               --dt->hour;
               if (dt->hour < 0){
                  dt->hour = 23;
               }
               char ambuf[3];
               sprintf(ambuf, "%02d", dt->hour);
               lcd_puts(ambuf);
               lcd_pos(1,i);
               continue;
            }
         }
         continue;
      }
      else if(3==i){
         //deal with MINS here...
         int hk = 0;
         while (hk!=16){
            myavr_wait(400);
            hk = get_key();
            if(!hk){
               continue;
            }

            else if(1==hk){
               ++dt->minute;

               if (dt->minute > 59){
                  dt->minute = 0;
               }
               char dbuf[3];
               sprintf(dbuf, "%02d", dt->minute);
               lcd_puts(dbuf);
               lcd_pos(1, i);
               continue;
            }

            else if(2==hk){
               --dt->minute;

               if (dt->minute < 0){
                  dt->minute = 59;
               }
               char adbuf[3];
               sprintf(adbuf, "%02d", dt->minute);
               lcd_puts(adbuf);
               lcd_pos(1, i);
               continue;
            }
         }
         //loop back for loop
         continue;
      }
      else{
         //deal with SECS here...
         int yk = 0;
         while (yk!=16){
            myavr_wait(400);
            yk = get_key();
            if(!yk){
               continue;
            }
            else if(1==yk){
               ++dt->second;
               if(dt->second > 59){
                  dt->second = 0;
               }
               char ybuf[5];
               sprintf(ybuf,"%02d",dt->second);
               lcd_puts(ybuf);
               lcd_pos(1,i);
            }
            else if(2==yk){
               --dt->second;
               if(dt->second < 0){
                  dt->second = 59;
               }
               char aybuf[5];
               sprintf(aybuf,"%02d",dt->second);
               lcd_puts(aybuf);
               lcd_pos(1,i);
            }
         }
         continue;
      }

   }
   return;
}


void setclock(struct dt *dt){
   changeday(dt);
   changesec(dt);
   //ok = get_key();

}

/*
 int main(void){
	struct dt mydt = {2008, 2, 28, 23, 59, 56};
	lcd_init();
	int k;
	while(1){
 //myavr_wait(1000);
 myavr_wait(100);
 k = get_key();
 if(13==k){
 setclock(&mydt);
 }
 myavr_wait(1000);
 increment_dt(&mydt);
 display_dt(&mydt);
	}
 }*/
//const struct Note SONGA[] = {{E,H}, {G,Q}, {D,H}, {C,I}, {D,I}, {E,H}, {G,Q}, {D,H}, {E,H}, {G,Q}, {d,H}, {c,Q}, {G,H}, {F,I}, {E,I}, {D,H}, {E,H}, {G,Q}, {D,H}, {C,I}, {D,I}, {E,H}, {G,Q}, {D,H}, {E,H}, {G,Q}, {d,H}, {c,Q}, {G,W}};
//const struct Note ALTSONG[] = {{a,W}, {c,H}, {G,W}, {F,Q}, {G,Q}, {a,W}, {c,H}, {G,W},{G,Q},{a,W}, {c,H}, {g,W},{f,H},{c,W}, {as,Q}, {a,Q}, {G,W},{G,Q}, {restn, W}, {a,W}, {c,H}, {G,W}, {F,Q}, {G,Q}, {a,W}, {c,H}, {G,W},{G,Q}, {a,W}, {a,I}, {g,H}, {f,Q}, {c,W}, {c,W}, {c,H}, {restn, W}, {c,H}, {as, I}, {a,I}, {as, I}, {a,I}, {F,H}, {as, H}, {a,I}, {G,I},  {a,I}, {G,I}, {D,H}, {c,H}, {as,I}, {a,I}, {as,I}, {a,I}, {F,Q}, {as,Q}, {f,W}, {f, W}, {f,H}};
const struct Note ALTSONG[] = {{a,H}, {c,Q}, {G,H}, {F,I}, {G,I}, {a,H}, {c,Q}, {G,H},{G,I},{a,H}, {c,Q}, {g,H},{f,Q},{c,H}, {as,I}, {a,I}, {G,H},{G,I}, {restn, W}, {a,H}, {c,Q}, {G,H}, {F,I}, {G,I}, {a,H}, {c,Q}, {G,H},{G,I}, {a,H}, {a,I}, {g,H}, {f,Q}, {c,W}, {c,W}, {c,H}, {restn, W}, {c,H}, {as, I}, {a,I}, {as, I}, {a,I}, {F,H}, {as, H}, {a,I}, {G,I},  {a,I}, {G,I}, {D,H}, {c,H}, {as,I}, {a,I}, {as,I}, {a,I}, {F,Q}, {as,Q}, {f,W}, {f, W}, {f,H}};
const int N = sizeof(ALTSONG)/sizeof(ALTSONG[0]);
const struct Note SONGA[] = {{c,H}, {Fs,H}, {G,H}, {G,W}, {c,I}, {fs,I}, {fs,I}, {Fs,I}, {restn, W}, {fs,I},{fs,I}, {c,I}, {fs,I}, {fs,I}, {Fs,I}, {fs,I}, {Fs,I},{fs,I}, {Fs,I}, {c,I}, {fs,I}, {fs,I}, {Fs,I}, {restn, W}, {fs,I}, {fs,I}, {c,H}, {e,Q}, {fs,I}, {aa,I}, {g,H}, {e,H}, {c,I}, {a,I}, {Fs,I}, {Fs,I}, {Fs, Q}, {G,Q}, {restn, H}, {C,I}, {Fs, I}, {Fs, I},{Fs, I}, {G,Q},{as,Q}};
const int M = sizeof(SONGA)/sizeof(SONGA[0]);
const struct Note SONGB[] = {{G,Q}, {G,Q}, {A,Q}, {G,Q}, {c,H}, {d,H}, {G,Q}, {G,Q}, {A,Q}, {G,Q}, {d,H}, {c,H}, {G,Q}, {G,Q}, {g,Q}, {e,Q}, {c,Q}, {b,Q}, {a,H}};
const int O = sizeof(SONGB)/sizeof(SONGB[0]);


int main(void){
   SET_BIT(DDRB, 3);
   lcd_init();
   int k = 0;
   while (1){
      myavr_wait(100);
      k = get_key();
      if(!k){
         lcd_clr();
         continue;
      }
      else if(3==k){
         char bufsong[17];
         sprintf(bufsong, "Zelda's Lullaby");
         lcd_clr();
         lcd_pos(1,0);
         lcd_puts(bufsong);
         play_song(ALTSONG, N);
      }
      else if(2==k){
         char bufsonga[17];
         sprintf(bufsonga, "The Simpsons");
         lcd_clr();
         lcd_pos(1,0);
         lcd_puts(bufsonga);
         play_song(SONGA, M);
      }
      else if(1==k){
         char bufbi[17];
         sprintf(bufbi, "Happy Birthday");
         lcd_clr();
         lcd_pos(1,0);
         lcd_puts(bufbi);
         play_song(SONGB, O);
      }
      else{
         continue;
         
      }
   }
   /*char bufsong[17];
    sprintf(bufsong, "Zelda's Lullaby");
    lcd_clr();
    lcd_pos(1,0);
    lcd_puts(bufsong);*/
   
   //play_song(ALTSONG, N);
   
   /*while(1){
    SET_BIT(PORTB, 3);
    myavr_wait(1);
    CLR_BIT(PORTB, 3);
    myavr_wait(1);
    }*/
}
