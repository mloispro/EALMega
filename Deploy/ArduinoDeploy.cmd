
echo Upload File: %1.hex
echo Platform: %2
echo COM: %3

IF /i "%2%"=="ATmega328P" goto ATmega328PChip
IF /i "%2%"=="ATmega2560" goto ATmega2560Chip

echo **This Script Doesnt Support Platform: %2 **
goto commonexit

:ATmega328PChip
echo Uploading to Uno...
start "" /wait /b "C:\Program Files (x86)\Arduino\hardware\tools\avr\bin\avrdude" -C"C:\Program Files (x86)\Arduino\hardware\tools\avr/etc/avrdude.conf" -v -p%2 -cwiring -P%3 -b115200 -D -Uflash:w:"%1.hex":i
goto commonexit

:ATmega2560Chip
echo Uploading to Mega...
start "" /wait /b "C:\Program Files (x86)\Arduino\hardware\tools\avr\bin\avrdude" -C"C:\Program Files (x86)\Arduino\hardware\tools\avr/etc/avrdude.conf" -v -p%2 -cwiring -P%3 -b115200 -D -Uflash:w:"%1.hex":i
goto commonexit

:commonexit
rem pause