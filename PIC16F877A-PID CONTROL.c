#include <16F877A.h> 
#device  ADC=10
#use delay(clock=20000000) 
#fuses NOWDT NOPROTECT NODEBUG NOBROWNOUT HS 
#use fast_io(c) 
#use fast_io(d) 
#use fast_io(b)
#include <stdlib.h> 
#define  LCD_ENABLE_PIN PIN_D0
#define  LCD_RS_PIN     PIN_D1
#define  LCD_RW_PIN     PIN_D2
#define  LCD_DATA4      PIN_D4
#define  LCD_DATA5      PIN_D5   
#define  LCD_DATA6      PIN_D6
#define  LCD_DATA7      PIN_D7
#include <lcd.c> 
#use rs232 (baud=9600, xmit=pin_C6, rcv=pin_C7, parity=N, stop=1)

float feedback; 
int16 setpoint; 
float voltaj; 
float a,b,c,rkt,ekt,qkt,pkt,ykt,ukt;
float MAX,MIN,pkt_l,ekt_l;
int control;
char str3[5];

#INT_TIMER0
void timer0_kesme () 
{ 
   set_timer0(6);
   
}

   void main ()
{
MIN=0.0;
MAX=400.0;
pkt_l=0.0;
ekt_l=0.0;
/*  PID Katsay�lar� */
a=10.9;
b=0.37;
c=43.5;
setpoint=110;
 
  setup_timer_2(T2_DIV_BY_16,255,1);   // PWM period = (PR2+1) x 4 x Tosc x TMR2 prescaler///prescale=16, PR2=195, postscale=16  100HZ ���N
  set_pwm1_duty(0);                   
  set_tris_c(0x00); 
  set_tris_b(0xFF);
  setup_CCP1(CCP_PWM);                  
  setup_adc(ADC_CLOCK_INTERNAL);       
  setup_adc_ports(all_analog);        
  lcd_init();                         
  setup_timer_0(RTCC_DIV_2); 
  set_timer0(6);// T_overflow = 2*(256-6)*0.2us = 100us
  enable_interrupts(INT_RTCC);        
  enable_interrupts(GLOBAL);          
  printf("\r**************** Merhaba *********************"); // Yeni sat�r, RS232 �zerinden belirtilen metni g�nder ve sat�r atla.
  printf("\n\rPWM Sinyali Gonderiliyor...\n\n");              // Sat�r atla, yeni sat�r, RS232 �zerinden belirtilen metni g�nder
  
 while(true) 
  {
 
     set_adc_channel(0);
     delay_ms(20);
     feedback=read_adc();
     delay_ms(20);
     voltaj=(float)feedback*250/1024;// adc,
     printf(lcd_putc"\fSicaklik%f",voltaj);
     printf(lcd_putc"\nSP%lu",setpoint);
     
ykt=voltaj;
rkt=setpoint;
ekt=rkt-ykt;//Hata hesaplan�yor
pkt=b*ekt+pkt_l;
qkt=c*(ekt-ekt_l);
ukt=pkt+a*ekt+qkt;// PID Hesaplan�yor

if(ukt > MAX)
{
pkt=pkt_l;
ukt =MAX;
}
else if(ukt < MIN)
{
pkt=pkt_l;
ukt=MIN;
}
 
ukt=ukt-3.0;
ukt=ukt/4.0;
control=(int16)ukt;//PWM Sinyali int D�n��t�r�l�yor.Float tipinde olamaz.
set_pwm1_duty(control);//PWM Sinyali G�nderiliyor
pkt_l =pkt;
ekt_l=ekt;
delay_ms(200);
 /* Setpoint'i artt�rma azaltma butonlar�*/
 if(input(pin_b5)==0)  setpoint artt�r
 {
    delay_ms(10);
    setpoint=setpoint+1;
    while(input(pin_b5)==0);
    
    delay_ms(10);
    
 }
  if(input(pin_b4)==0)                 // setpoint azalt
  
 {
    delay_ms(10);
    setpoint=setpoint-1;
    while(input(pin_b4)==0);
    delay_ms(10);
    
 }
 itoa(control,5,str3);//Seri Haberle�me Terminaline g�nderilen sinyal ASCII tipinden ��kar�ld�.
 puts("\n\r");
 putc(str3[0]);
 putc(str3[1]);/*putc((int8)control);//control = control>8;//putc((int8)control); Bu t�rde yaz�labilir.*/
    }
   
} 

