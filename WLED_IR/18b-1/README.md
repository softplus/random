# 13 button remote control

![](preview.jpg)

## Layout

Col-1 | Col-2 | Col-3
--- | --- | ---
^ | + | PWR
v | - | SRC
1 | 2 | 3
4 | 5 | 6
7 | 8 | 9
Back | 0 | Mute

## Codes

Col-1 | Col-2 | Col-3
--- | --- | ---
^: 0xFF48B7 | +: 0xFF40BF | PWR: 0xFF28D7
v: 0xFF58A7 | \-: 0xFF6897 | SRC: 0xFFB847
1: 0xFFA05F | 2: 0xFFF00F | 3: 0xFF9867
4: 0xFF38C7 | 5: 0xFFB04F | 6: 0xFF18E7
7: 0xFF7887 | 8: 0xFFD827 | 9: 0xFFF807
Back: 0xFF609F | 0: 0xFF30CF | Mute: 0xFFC837

## WLED 

Col-1 | Col-2 | Col-3
--- | --- | ---
^: Faster | +: Brighter | PWR: On
v: Slower | \-: Darker | SRC: Next effect
1: Preset 1 | 2: Preset 2 | 3: Preset 3
4: Preset 4 | 5: Preset 5 | 6: Preset 6
7: Preset 7 | 8: Preset 8 | 9: Preset 9
Back: Next preset | 0: Static | Mute: Off

File: [ir.json](ir.json)

Download: [ir.json](https://raw.githubusercontent.com/softplus/random/master/WLED_IR/18b-1/ir.json)
