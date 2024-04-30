# NEST
A TAS enhanced emulator for the NES

# How to Run:
1) Clone this repository
2) Run "make build" in the console (make sure you have g++ installed)
3) Run a command to play a game, play a TAS, or create a TAS (see below for details)

# Instructions
To compile the project (make sure to do this first): make build  
To play a game: make play game=[game filename, without extension]  
	ex. make play game=donkey  
To play back a TAS: make play-tas game=[game] tas=[tas]  
	ex. make play-tas game=donkey tas=donkeytas  
To create a TAS: make create-tas game=[game] tas=[tas]  
	ex. make create-tas game=donkey tas=donkeytas  

Note: The make command may vary (you might need to use “mingw32-make” instead of “make). In addition, our project only runs on Windows, since we rely on a .dll file for SDL.  

TAS examples: We have provided two example TAS files in the tas folder, one for Super Mario Bros. and one for Donkey Kong.  

# Controls:  
WASD/Arrow Keys: Move  
Space/X: A  
Left Shift/Z: B  
Enter: Start  
Right Shift: Select  
Delete: Reset game  
Escape: Close emulator  
