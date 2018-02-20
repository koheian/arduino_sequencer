# arduino_sequencer
## Overview
A music sequencer which works on Arduino Uno.
Comments in source files are written in Japanese.

Arduino Unoで動くシーケンサです。
ソースファイルのコメントは日本語で書かれています。

## Hardware
### With _mobileBB shield for Mozzi_
I used [mobileBB shield for Mozzi](https://github.com/BreadboardBand/mobileBBshield_for_Mozzi) created by The Breadboard Band to build the first prototype.
Though this is not essential, this makes it easier to make the circuit.

プロトタイプ作成に、The Breadboard Bandさんの[mobileBB shield for Mozzi](https://github.com/BreadboardBand/mobileBBshield_for_Mozzi)を使用させて頂きました。
なくても十分作成可能ですが、あれば回路を組むのがいくらか楽になります。

License of _mobileBB shield for Mozzi_ is below (quotation from [https://github.com/BreadboardBand/mobileBBshield_for_Mozzi/blob/master/LICENSE](https://github.com/BreadboardBand/mobileBBshield_for_Mozzi/blob/master/LICENSE)).

_mobileBB shield for Mozzi_ のライセンスは以下です（[https://github.com/BreadboardBand/mobileBBshield_for_Mozzi/blob/master/LICENSE](https://github.com/BreadboardBand/mobileBBshield_for_Mozzi/blob/master/LICENSE)からの引用）。  

>This work is licensed under a
>Attribution 3.0 Unported (CC BY 3.0)
>http://creativecommons.org/licenses/by/3.0/deed.en
>
>Copyright by The Breadboard Band 2013 - 2014

### Components
When you make without _mobileBB shield for Mozzi_, you need:
- tactile switch, 4 units
- potentiometer, 1 unit

_mobileBB shield for Mozzi_ を使用しない場合、必要なもの：
- タクタイルスイッチ × 4
- 可変抵抗 × 1

Below is optional whether you use _mobileBB shield for Mozzi_ or not:
- high power Red Green & Blue LED, 1 unit

_mobileBB shield for Mozzi_ に関係なく、あってもなくても良いもの：
- パワーフルカラーLED × 1

### How to Make The Circuit
- Connect digital input pin No. 2, 3, 4 and 5 to tactile switches respectively. Connent each of them to a 5 V power source pin.
- デジタルインプットピン2, 3, 4, 5それぞれにタクタイルスイッチを接続し、5 Vの電源ピンに接続します。
- Connect analog input pin No. 1 to a potentiometer and a 5 V power source pin.
- アナログインプットピン1を可変抵抗、5 V電源ピンに接続します。
- Connect digital output pin No. 6, 7 and 8 to LEDs respectively, which are not high power LEDs.
- デジタルアウトプットピン6, 7, 8をそれぞれLEDに接続します。ここでのLEDはパワーLEDではありません。
- Connect digital output pin No. 9, 10 and 12 to a high power LED to assign red, green and blue to each pin.
- デジタルアウトプットピン9, 10, 12をそれぞれパワーLEDに接続します。このとき、それぞれのピンにred, green, blueを割り当てるようにします。

## License
This work is under The MIT License (see [./LICENSE](./LICENSE) for detail).

MIT Licenseを使用しています（詳細は[./LICENSE](./LICENSE)を参照）。
