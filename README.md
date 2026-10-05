# EepyAlarm

An alarm for your computer that helps you sleep earlier.

For people who always seem to be doing something on their computer late into the night, and want to fix their sleep schedule
(like me, this is so totally me like I'm writing this readme at 12AM right now)

Thank you to Stardance for enabling this project!

<img src=assets\V1_GIF.gif />

[LATEST RELEASE](https://github.com/Alex-SJuice/EepyAlarm/releases/tag/v1.0.0)

***!EepyAlarm is only available for WINDOWS at the moment!***

## How to use it

go to the latest release, download the program, and run it.

On initial setup, you input,
- the time you would like to sleep at
- the current time you sleep at
- by how much you want to go towards that goal

for example,
- I sleep at 1AM right now
- I would like to sleep at 11PM
- I would like to improve by 1 minute every day

It will create a configuration file at C:/Users/current_user/EepyAlarm and store everything there.

Once set, the alarm will stay running in the background and automatically trigger a little earlier every day until you reach your target!

To reset the settings, just run the executable with any kind of input argument. Anything will do.

## Features

1.0:
- runs in the background
- rings at a specific time, starting at the current sleep time, progressing up to the target sleep time
- user input based initialization
- text based alarm

2.0 (potential):
- installer
- automatic start on system boot
- GUI for setup
- nicer looking alarm

## How it works / Technical details

A surprising amount went into the first version of the program. Here's just a few of the highlights:
- using while loops to gracefully handle bad inputs during initialization
	- omg it took so long to figure out that you neet to fflush(stdout) or else user input doesn't work.
- recreating printf so that it prints to a new console window, while also supporting format strings i.e. "I am %d years old"
	- this involved the ... (variable number of function arguments) and stdarg.h
- keeping track of when to ring by storing the last time it rang in the config file

AI was not used to generate any code.

## Credits
Man, can I say *The C Programming Language* is an amazing book?
It has like everything you need to write in C, except examples for some functions.
I only occasionally needed to google certain things, the rest was all in the book.

Shout outs to Kernighan and Ritchie for making C

W3Schools and Geeks for Geeks for some IO examples

AI (Gemini & Deepseek) for research into the windows API and debugging.