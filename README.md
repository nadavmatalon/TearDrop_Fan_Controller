#TearDrop Fan Controller

##Table of Contents

* [Screenshots](#screenshots)
* [About the Project](#about-the-project)
* [About the Fan Controller](#about-the-fan-controller)
* [License](#license)


##Screenshots

<table>
	<tr>
		<td align="center" width=25% >
			<a href="https://raw.githubusercontent.com/nadavmatalon/TearDrop_Fan_Controller/master/images/TDFC_Arduino.jpg">
				<img src="images/TDFC_Arduino.jpg" height="105px" />
			</a>
		</td>
		<td align="center" width=25% >
			<a href="https://raw.githubusercontent.com/nadavmatalon/TearDrop_Fan_Controller/master/images/TDFC_PCB.jpg">
				<img src="images/TDFC_PCB.jpg" height="105px" />
			</a>
		</td>
		<td align="center" width=25% >
			<a href="https://raw.githubusercontent.com/nadavmatalon/TearDrop_Fan_Controller/master/images/TearDrop_1.jpg">
				<img src="images/TearDrop_1.jpg" height="105px" />
			</a>
		</td>
		<td align="center" width=25% >
			<a href="https://raw.githubusercontent.com/nadavmatalon/TearDrop_Fan_Controller/master/images/TearDrop_2.jpg">
				<img src="images/TearDrop_2.jpg" height="105px" />
			</a>
		</td>
	</tr>
</table>


##About the Project
 
__TearDrop__ is a custom, open-frame, water-cooled scratch build I’ve been 
working on as part of the SnowCrash initiative.

The __SnowCrash__ initiative aims at designing and building end-to-end customized 
personal computers, addressing unique functional requirements – from developing 
the software control applications, to hardware configuration and case modding. 
As part of the initiative's research phase, the TearDrop customized computer 
prototype has been developed.

The underlying motivation for this specifc project was to research, design, 
prototype and finally create everything that's needed for a fully-customized 
desktop computer.

In this context, I’ve learned to program micro-controllers, design, prototype 
and build electronic circuit boards, make CNC cutting templates, interface with 
various kinds of hardware, write front-end applications, and lots more.

This project also helped me to become proficient in multiple software tools, including: 
[Google Sketchup](http://www.sketchup.com/), 
[Arduino IDE](http://www.arduino.cc/), [Processing 2.0](http://processing.org/), 
[CadSoft Eagle](http://www.cadsoftusa.com/download-eagle/freeware/), 
and [Inkscape](http://www.inkscape.org/en/).
 
The __project-log__ of this build (which I haven't had time to update recently, but 
does include loads of cool pictures :-), can be found 
at [bit-tech](http://forums.bit-tech.net/showthread.php?t=234218).
 

##About the Fan Controller

This repo contains the custom Fan Controller I’ve designed and built 
to control __TearDrop__’s cooling fans.

This repo contains the following files:

* __arduino__

    The source code for the 2 x ATmega328P that run the Fan Controller 

    The files can can be directly accessed by clicking these links:

    * [Fan Controller: IC1](arduino/TearDrop_Fan_Controller_IC1/TearDrop_Fan_Controller_IC1.ino)

    * [Fan Controller: IC2](arduino/TearDrop_Fan_Controller_IC2/TearDrop_Fan_Controller_IC2.ino)

	The source code requires three supporting open-source [Arduino](http://www.arduino.cc/) libraries.

	For convenience, these libraries are inlcuded here in their own sub-folder.

	Credit goes to:

	[PinChangeInt](http://playground.arduino.cc/Main/PinChangeInt)

	[OneWire](https://github.com/ntruchsess/arduino-OneWire/tree/master)

	[WSWire](https://github.com/steamfire/WSWireLib)


* __eagle__

    This folder contains the complete hardware schematics & PCB design of the Fan Controller.

    __Please do not click on the files in this sub-folder as they are in foramts that 
    are incompatible with Github's hosting server__.

    However, the files can be downloaded and run as usual with [CadSoft](http://www.cadsoftusa.com/?language=en)'s 
    free version of [Eagle](http://www.cadsoftusa.com/download-eagle/freeware/).


* __parts__

	A speadsheet with all information about the parts used for building the Fan Controller.

	__The file in this sub-folder is also in a foramt that's incompatible with Github's hosting server__.

    However, it can be downloaded and run as usual with any spreadsheet editing software.

 
##  License

<p>Released under the <a href="http://www.opensource.org/licenses/MIT">MIT license</a>.</p>

