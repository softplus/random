# 13 button remote control

![](preview.jpg)

[On Aliexpress](https://s.click.aliexpress.com/e/_9gupoe)

## Layout

Col-1 | Col-2 | Col-3
--- | --- | ---
ON | Timer | OFF
1 | \-- | 2
3 | \-- | 4
5 | \-- | 6
7 | \-- | 8
DIM | \-- | BRI

## Codes

Col-1 | Col-2 | Col-3
--- | --- | ---
ON: 0xFFA25D | Timer: 0xFF629D | OFF: 0xFFE21D
1: 0xFF22DD | \-- | 2: 0xFFC23D
3: 0xFFE01F | \-- | 4: 0xFF906F
5: 0xFF6897 | \-- | 6: 0xFFB04F
7: 0xFF30CF | \-- | 8: 0xFF7A85
DIM: 0xFF10EF | \-- | BRI: 0xFF5AA5

## WLED 

Col-1 | Col-2 | Col-3
--- | --- | ---
ON: on | Timer: Next effect | OFF: off
1: Preset 1 | \-- | 2: Preset 2
3: Preset 3 | \-- | 4: Preset 4
5: Preset 5 | \-- | 6: Preset 6
7: Preset 7 | \-- | 8: Preset 8
DIM: Slower | \-- | BRI: Faster

File: [ir.json](ir.json)

Download: [ir.json](https://raw.githubusercontent.com/softplus/random/master/WLED_IR/13b-1/ir.json)
