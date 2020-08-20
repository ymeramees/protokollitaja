# Protokollitaja
ISSF Shooting sport ranking and results display software. Main program and a server from a set of programs.

Unfortunately, main part of the code is quite ugly, as this project was started in 2007 when I had just started learning Qt and programming in general. Quality of the code has (hopefully) improved over the years.

As it was initially developed for national competitions, the program is in Estonian language. Adding translation framework and translation to English is planned sometime in future.

### Main Features
* Most ISSF shooting sport diciplines included
* Autocomplete when inserting competitors' names, based on a database
* Results display on separate screen with automatic ranking
* Serial connection with Disag target scoring machines RM III Universal and RM IV
* Connection with electronic targets through SiusData for live capturing of results
* Results upload to a server from where these will be shown on a [webpage](https://webzone.ee/protokollitaja) 
* Results export to .xls, .csv and *.txt files
* Results import
* Sorting of competitors by different properties (name, date of birth, target no, etc.)
* Moving/copying competitors between diciplines
* Connection with [QFinaal](https://github.com/ymeramees/qfinaal)
* Connection with [Protolehelugeja](https://github.com/ymeramees/protolehelugeja)
* Simulation of results from electronic targets with [Prototester](https://github.com/ymeramees/prototester)
* Check for updates on startup

## Built With

* [Qt](http://www.qt-project.org)
* Depends on [QFinaal](https://github.com/ymeramees/qfinaal) for building. Make sure to clone QFinaal next to Protokollitaja folder.

### Installing/Deploying
On Windows, make sure to copy needed dll's from your Qt installation into respective folders (platforms, printsupport) into the same directory where executable is. This is also needed when building/running from Qt Creator.
On Linux, this is not needed.

In addition, Data folder is needed. Program will search for autocomplete databases from there. If this folder does not exist it will be created, but autocomplete will not work.

## License

This project is licensed under the AGPLv3 License - see the [LICENSE.md](LICENSE.md) file for details
