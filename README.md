# Arduino Web Board

Arduino Web Board (AWB) is simple project which can be used to learn basic Arduino functionalities. 
Inside project there are following modules

* SH21
* TSL
* RELAY


Functionalities inside code for Arduino are:

* Get current and automatic sending state of Arduino modules via HTTP request
* Controling the lamp via HTTP request
* Handle toggling lamp on lux treshold with relay


# WARNING

This project contain relay module which can be connected to high voltage source. To **run** the code there is **no** need to connect source with relay.
Relay has led indicator of switch status. 

Do not connect relay while source is **ON**. **
Bad cable connection and using low-quality product can potentionaly potentially harm you, puting your life at risk and damaging household objects.


## Getting Started


These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

Full system has 3 parts:

* [Web Portal](Web) where we are gonna see all Arduino data
* [MegaHearth](MegaHeart) .NET Core 3 project for receiving Arduino data 
* [Arduino source code](ArduinoPowerBoard) for our hardware

## Authors

* **Armin Comic** - *Initial work* - [MedoDome](https://github.com/MedoDome)

## License

This project is licensed under the MIT License.

## Acknowledgments

* [Web template SB Admin](https://github.com/BlackrockDigital/startbootstrap-sb-admin)
* [Arduino forum](https://forum.arduino.cc/)
