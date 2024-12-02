# IRC-Client-Server-Assignment

An IRC Socket Programming Assignment of Networks &amp; Data Communications Module

## Disclaimer

The share of this ASSIGNMENT CONTENT is only a result of BEING REQUESTED.

I am not personally responsible for anything.

**FOR REFERENCE ONLY!**

Copy (or other illegal use) **AT YOUR OWN RISK**

**irc-ui folder (mentioned later) NOT PROVIDED due to its size.**

## Introduction

There are FOUR parts in this project:

1. client.cpp

Like a HexChat client IN CONSOLE (It can either act as the SuperBot or a normal client like a HexChat client to be used by a human being).

client.cpp should be compiled and run on a Windows VM (IP address fc00:1337::19).

2. IRC (folder) and irc-ui (folder) (Note only for GitHub repository: irc-ui not uploaded due to its size)

Like a HexChat client IN GUI (It can either act as the SuperBot or a normal client like a HexChat client to be used by a human being).

The IRC folder contains the GUI project source code which should be compiled and run on a Windows VM WITH PROPER LIBRARIES AND ENVIRONMENTS INSTALLED, 

the irc-ui folder contains the executable of GUI project (IRC.exe) which should be compiled and run on a Windows VM.

**IMPORTANT:**

The IRC folder contains all the source codes of this GUI application.

It's based on Qt library. General guide on how to compile and run it are given below.

The irc-ui folder is the EXACT RESULT of the compilation of the IRC project.

(So you don't need to compile the project in IRC folder)

The irc-ui folder has an "IRC.exe" executable inside it and you can just double click to run.
	
3. server.cpp

Like a miniircd server.

server.cpp should be compiled and run on a Linux VM.

4. funfacts.txt

Stores all the fun facts used by both clients (client.exe, IRC.exe).

funfacts.txt should be placed in the same place as client.cpp AND IRC.exe (inside irc-ui folder).

*NOTE: Please make sure that both VMs can ping each other successfully.

## CONTENT

1 Compile

2 Run & Operations

3 HexChat-related information

4 Features

## 1 Compile

[For server.cpp:]

On Linux VM: Open the terminal and enter the following command.

g++ -std=c++1z -pthread -o server server.cpp

[For client.cpp:]

On Windows VM: Open the terminal (NOT Powershell) and enter the following command.

g++ -std=c++11 -o client.exe client.cpp -lws2_32

**NOTE: DO NOT USE POWERSHELL**

[For IRC project (GUI version):]

(BE ADVISED THAT you DO NOT have to do this. As said before, the irc-ui folder already has the executable you may need.)

On Windows VM: 

a) Install any version of Qt6 library.

b) Select "Qt HTTP Server", "Qt Network", "Qt WebSocket" for additional libraries.

c) After proper installation, import the CMakeList file of the IRC project.

d) Compile & Use it using Qt Editor.

## 2 Run & Operations

a) Run the server first. On Linux VM, input to the terminal:

./server

Server should prompt that it is now listening on port 6667. If an error is prompted, check that if the port is occupied by other processes.

b) Then run the client(s). 

**Step 1: Create a SuperBot first.**

[For client.exe:]

Input to the terminal (NOT Powershell) on your Windows VM:

client.exe --name SuperBot --channel #<YOUR_CHANNEL>

OR: client.exe --channel #<YOUR_CHANNEL>

e.g. client.exe --name SuperBot --channel #testChannel

OR:  client.exe --channel #testChannel

*NOTE: Input your own one for <YOUR_CHANNEL>.

*NOTE: Do not forget the "#" before channel name.

[For IRC.exe:]

Once you open it, you will notice that the default name "SuperBot" is already in the nickname box.

This means you don't have to make any change to make the create a SuperBot.

**IMPORTANT**

Please ALWAYS make sure that a SuperBot is created in the channel because MOST of the features in this project REQUIRE one in the channel.

If not, you will be warned when using these features.

**Step 2: After creating a SuperBot, you may also want to create some normal users.**

[For client.exe:]

Input to the terminal (NOT Powershell) on your Windows VM:

client.exe --name <YOUR_NICKNAME> --channel #<YOUR_CHANNEL>

e.g. client.exe --name Tom --channel #testChannel

NOTE: Input your own ones for <YOUR_NICKNAME> and <#YOUR_CHANNEL>.

NOTE: Do not forget the "#" before channel name.

The client(s) should prompt that the connection is successful.

[For IRC.exe:]

Change the default name "SuperBot" to any name you wish, e.g. Tom.

## 3 HexChat-related information

You may also wish to connect a HexChat client to the server. After you successfully connect to the server, please first type the following command in the textbox:

/nick <YOUR_NICKNAME>

e.g. /nick Tom

After that, type in the following to join a channel:

/join #<YOUR_CHANNEL>

e.g. /join #testChannel

*NOTE: Do not forget the "#" before channel name.

4 ## Features

**IMPORTANT**

The format of commands should be same on all clients (client.exe, IRC.exe and HexChat).

On client.exe and HexChat please press Enter after inputting.

On IRC.exe, press the Send button after inputting.

[Public Message]

Text directly in channel to send a public message:

<YOUR_MESSAGE>

e.g. Hello everyone!

[Private Message]

To send a private message to specific user, input:

/msg <TARGET_NICKNAME> <YOUR_PRIVATE_MESSAGE>

e.g. /msg Jerry Hello Jerry! I am now talking to you privately so no one else will see this.

[Slap]

a) To slap a specific user, input:

!slap <TARGET_NICKNAME>

e.g. !slap Jerry

b) To randomly slap a user, input:

!slap

[Greeting]

To receive a greeting message from the SuperBot, input:

!hello

[Time]

To check out what time it is, input:

!time

[Kick]

To kick someone out of the channel, input:

!kick <TARGET_NICKNAME>

e.g. !kick Jerry

[Details of a specific user]

To check out the information about a specific user, input:

!whois <TARGET_NICKNAME>

e.g. !whois Jerry

[Topic]

a) To set the topic of the current channel, input:

!topic #<YOUR_CHANNEL> <TOPIC_NAME>

e.g. !topic #testChannel Fun

*NOTE: Do not forget the "#" before channel name.

b) To view the current topic, input:

!topic #<YOUR_CHANNEL>

e.g. !topic #testChannel

*NOTE: Do not forget the "#" before channel name.

[Mode Setting]

a) To set a user as an operator (admin), input:

!mode #<YOUR_CHANNEL> +o <YOUR_NICKNAME>

e.g. !mode #testChannel +o Tom

*NOTE: Do not forget the "#" before channel name.

b) To set a channel to moderated mode (only operators can speak), input:

!mode #<YOUR_CHANNEL> +m

e.g. !mode #testChannel +m

*NOTE: Do not forget the "#" before channel name.

c) To set a channel to normal mode (everyone can speak), input:

!mode #<YOUR_CHANNEL> -m

e.g. !mode #testChannel -m

*NOTE: Do not forget the "#" before channel name.

**END**

Last Updated Oct 4 2024
