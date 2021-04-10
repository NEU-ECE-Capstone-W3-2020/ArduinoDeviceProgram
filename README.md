Important: the feather needs to be programmed separately from the Arduino!! The correct beacon code should be on there but you might have to unsolder the tx/rx pins on the feather in order to program it with the USB :/

# Pin Config

![](./atmega328p-pinout.gif)

## Programming the main board

| Arduino | Kanasthetic device | 
| --- | --- |
| Reset | Pin1 |
| RX | Pin2 |
| TX | Pin3 | 

Sometimes for me it only worked if I took out the TX port. And sometimes if I took out the RX port?

Also just select Arduino Uno in Tools>Board and the normal serial port.

## Our configuration

| Arduino | Emic |
| --- | --- |
| 10 | Sout |
| 11 | Sin |

| Arduino | Feather |
| --- | --- |
| 2 | RX |
| 3 | TX |
