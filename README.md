# gb

a game boy emulator written in C

![image](https://user-images.githubusercontent.com/98568521/158297935-62e3e7ce-a20b-4a83-8e01-c0b7f97cd5fb.png)
![image](https://user-images.githubusercontent.com/98568521/158298033-345a2860-b02b-428f-82e4-e06b58b8851f.png)
![image](https://user-images.githubusercontent.com/98568521/158298126-305de85b-5682-46b4-8096-7b5802c60708.png)
![image](https://user-images.githubusercontent.com/98568521/158298285-0a612776-4af3-45ec-846e-d7cac3dd2bb7.png)
![image](https://user-images.githubusercontent.com/98568521/158298377-87134bd4-d270-4a2c-aaa1-df2bddba6bd2.png)
![image](https://user-images.githubusercontent.com/98568521/158298488-5bf50304-6d23-45a2-9596-ed9f6cfbf553.png)

## usage

`gameboy.exe [ROM file]`

## keys

- arrow keys for dpad
- Z for B button
- X for A button
- enter for start button
- left/right shift for select button

## features

- minimal UI (will upgrade eventually)
- support for no mapper, MBC1

## how to use

i'm not sure i'll figure it out eventually, needs SDL2 .lib's and .dll's

## blargg test results

i don't care for accurate memory timing, nor m/t-cycle accuracy

![image](https://user-images.githubusercontent.com/98568521/158297562-dd7dd26b-d539-451c-a4b4-e1e36c23dffd.png)
![image](https://user-images.githubusercontent.com/98568521/158297592-fedfc698-b852-4ef7-aa46-d38e0e47da28.png)

## dmg-acid2 results

i use a scanline renderer, no fifo (yet)

![image](https://user-images.githubusercontent.com/98568521/158298701-7b83f6b6-c786-4773-b1aa-3847993c68bc.png)

## todo

- add support for MBC2, MBC3, MBC5 mappers
- add audio
- refactor
- add to UI
- let the end user build the project

## thanks

thanks to the emudev discord for helping me make this
