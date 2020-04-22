# Arduino Web Board

Arduino Power Board is project code for Arduino


Functionalities inside code for Arduino are:

* Get current and automatic sending state of Arduino modules via HTTP request
* Controling the lamp via HTTP request
* Handle toggling lamp on lux treshold with relay

All code is made from example source codes of libraries and modified to AWB needs.

# WARNING

This project contain relay module which can be connected to high voltage source. To **run** the code there is **no** need to connect source with relay.
Relay has led indicator of switch status. 

Do not connect relay while source is **ON**. **
Bad cable connection and using low-quality product can potentionaly potentially harm you, puting your life at risk and damaging household objects.


## Getting Started


These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

If you don't have modules, disable them in Arduino source code and values for that module will be set to **-2**;

To have everything working you need following:

* Arduino IDE or anyother you prefer
* Arduino
* SH21
* TSL
* RELAY
* Board


### Installing

Open your ArduinoIDE and upload the code

### Settings

Inside [ArduinoPowerBoard.ino](ArduinoPowerBoard.ino) from 0 to 60line, you can find various variables used in this project with all comments.


## Built With

* [ArduinoIDE](https://www.arduino.cc/en/main/software) - The web framework used
* [.NET Core3.1](https://dotnet.microsoft.com/download/dotnet-core/3.1) - Dependency Management

## Contributing

Feel free to send PRs.

## Versioning

#TBD

## Authors

* **Armin Comic** - *Initial work* - [MedoDome](https://github.com/MedoDome)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* [Web template SB Admin](https://github.com/BlackrockDigital/startbootstrap-sb-admin)
* [Arduino forum](https://forum.arduino.cc/)
