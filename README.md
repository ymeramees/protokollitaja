# Protokollitaja
ISSF Shooting sport ranking and results display software suite. Now monorepo for previously separate programs:
* [Protokollitaja](protokollitaja/README.md) - Main ranking and results display program and a server from a set of programs.
* [Protofinaal](protofinaal) - New implementation of [QFinaal](https://github.com/ymeramees/qfinaal). In a very early stage at the moment.
* [Protolehelugeja](protolehelugeja/README.md) - For reading results from paper target scoring machines. Disag RM-III and Disag RM-IV are currently supported.
* [ProtoRangeControl](protorangecontrol/README.md) - For controlling multiple (InBand) electronic targets in a range.

For more details about each program, see their respective readme files.

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
* Connection with [Protolehelugeja](protolehelugeja/README.md)
* Connection with [ProtoRangeControl](protorangecontrol/README.md)
* Simulation of results from electronic targets with [Prototester](https://github.com/ymeramees/prototester)
* Check for updates on startup

## Built With

* [Qt](http://www.qt-project.org)

### Installing/Deploying
On Windows, make sure to copy needed dll's from your Qt installation into respective folders (platforms, printsupport) into the same directory where executable is. This is also needed when building/running from Qt Creator.
On Linux, this is not needed.

In addition, Data folder is needed for Protokollitaja. Program will search for autocomplete databases from there. If this folder does not exist it will be created, but autocomplete will not work.

## License

This project is licensed under the AGPLv3 License - see the [LICENSE.md](LICENSE.md) file for details
