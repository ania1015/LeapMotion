import cv2 as cv
import numpy as np
import random
######################################################## ZMIENNE ###################################################
NOZYCE = 1
PAPIER = 2
KAMIEN = 3

REMIS = -1
LOSE = 0
WIN = 100
######################################################## FUNKCJE ###################################################
def rysujLinie(image,x0,y0,xk,yk):
    color=(255,255,255)
    thickness=2
    startPoint=(x0,y0)
    endPoint=(xk,yk)
    image=cv.line(image, startPoint, endPoint,color, thickness )
def rysujNozyce(image):
    rysujLinie(image,250,250,500,200) #ramie

    rysujLinie(image,500,200,570,175) #wskazujacy 
    rysujLinie(image,570,175,620,177)

    rysujLinie(image,500,200,590,190) #srodkowy
    rysujLinie(image,590,190,650,192)

    rysujLinie(image,500,200,520,250) #kciuk
    rysujLinie(image,520,250,540,230)

    rysujLinie(image,500,200,520,170) #maly
    rysujLinie(image,520,170,530,174)
    rysujLinie(image,530,174,528,176)

    rysujLinie(image,500,200,540,175) #serdeczny
    rysujLinie(image,540,175,570,185)
    rysujLinie(image,570,185,568,187)


def rysujPapier(image):
    rysujLinie(image,250,250,500,200)

    rysujLinie(image,500,200,570,240) #kciuk
    rysujLinie(image,570,240,620,225)

    rysujLinie(image,500,200,590,190) #wskazujacy
    rysujLinie(image,590,190,650,192)

    rysujLinie(image,500,200,600,180) #srodkowy
    rysujLinie(image,600,180,660,185)

    rysujLinie(image,500,200,580,170) # serdeczny
    rysujLinie(image,580,170,650,174)

    rysujLinie(image,500,200,570,160) #maly
    rysujLinie(image,570,160,630,165)


def rysujKamien(image):
    rysujLinie(image,250,250,500,200) #ramie

    rysujLinie(image,500,200,525,225) #kciuk
    rysujLinie(image,525,225,535,215)

    rysujLinie(image,500,200,570,190) #wskazujacy
    rysujLinie(image,570,190,560,215)
    rysujLinie(image,560,215,550,220)
    rysujLinie(image,550,220,545,217)


    rysujLinie(image,500,200,580,180) #srodkowy
    rysujLinie(image,580,180,575,210)
    rysujLinie(image,575,210,560,205)

    rysujLinie(image,500,200,520,170) #maly
    rysujLinie(image,520,170,530,174)
    rysujLinie(image,530,174,528,176)

    rysujLinie(image,500,200,540,175) #serdeczny
    rysujLinie(image,540,175,560,185)
    rysujLinie(image,560,185,568,187)


def Podpis(wynik, image):
    
    match wynik:
        case 0: # przegrana
            image=cv.putText(image,"YOU LOSE :(",(250,400),cv.FONT_HERSHEY_PLAIN, 5, (0,0,255),2) #uzywa przestrzeni BGR
        case 100: # wygrana
            image=cv.putText(image,"YOU WIN :)",(250,400),cv.FONT_HERSHEY_PLAIN, 5, (0,255,0),2)
        case -1: # remis 
            image=cv.putText(image,"REMIS, LETS GO AGAIN",(250,400),cv.FONT_HERSHEY_PLAIN, 3, (0,255,0),2)

######################################################## KOD ###################################################
x='a'
print("kliknij x aby zakonczyc")
while (True):
    x=input() 

    if(x == 'x'):
        break;
    
    image_path=r'klatka.png'
    image=cv.imread(image_path)

    #losowanie enum rand 0 do 3 
    losuj=random.randint(1,3) #start, stop, step
    print("komputer: ", losuj)

    match losuj:
        case 1: rysujNozyce(image)
        case 2: rysujPapier(image)
        case 3: rysujKamien(image)

    #tymczasowe roziwazanie zamiast czujnika
    przeciwnik=random.randint(1,3) 
    print("ja: ", przeciwnik)

    if(losuj==przeciwnik):
        wynik = REMIS
    else:
        if((losuj==NOZYCE and przeciwnik==PAPIER) or (losuj==KAMIEN and przeciwnik==NOZYCE) or (losuj==PAPIER and przeciwnik==KAMIEN)):
            wynik = LOSE
        if((przeciwnik==NOZYCE and losuj==PAPIER) or (przeciwnik==KAMIEN and losuj==NOZYCE) or (przeciwnik==PAPIER and losuj==KAMIEN)):
            wynik = WIN


    Podpis(wynik,image)



    cv.imshow('klatka', image)

    cv.waitKey(0) 
    cv.destroyAllWindows()

    
