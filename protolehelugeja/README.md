# Protolehelugeja
Software for reading shooting sport results from target scoring machines. Part of a set of programs for [Protokollitaja](https://github.com/ymeramees/protokollitaja) and meant to be used together with it.

Unfortunately, main part of the code is quite ugly, as this project was started in 2007 when I had just started learning Qt and programming in general. Quality of the code has (hopefully) improved over the years.

As it was initially developed for national competitions, the program is in Estonian language. Adding translation framework and translation to English is planned sometime in future.

### Main Features
* TCP connection with [Protokollitaja](https://github.com/ymeramees/protokollitaja) to retreive competitors' names and send back results
* Find Protokollitaja automatically on same network with broadcasting packet
* Serial connection with Disag target scoring machines RM III Universal and RM IV
* 50m rifle, air rifle and air pistol targets are supported with up to 2 shots per target (5 would not give reliable results from scoring machines)
* Results and shot locations display on screen
* Target numbers or names can be used to search competitors from Protokollitaja
* Send "save" command to Protokollitaja

## Built With

* [Qt](http://www.qt-project.org)

### Installing/Deploying
On Windows, make sure to copy needed dll's from your Qt installation into respective folders (platforms, printsupport) into the same directory where executable is. This is also needed when building/running from Qt Creator.
On Linux, this is not needed.

## License

This project is licensed under the AGPLv3 License - see the [LICENSE.md](LICENSE.md) file for details
