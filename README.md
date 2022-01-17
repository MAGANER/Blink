# Blink
decentralised, encrypted, free messanger.

# Basic Technical Info
1. C++ standart is 17<br>
2. Originally compiled with MSVC19<br>
3. Tested well on windows 7,10. (it didn't worked on windows 8)<br>
4. Compiled for x86 architecture 

### Dependencies
1. ```SFML``` for network and base for GUI
2. ```TGUI``` for graphical interface
3. ```Sqleet``` for sqlite3 data base encryption(included)
4. ```Nlohmann``` for json processing(included)
5. ```Crypto++``` for AES encryption

### How does it work? Behind the Scene
First of all it uses TCP protocol for data transfering.<br>
Every user plays 2 roles at the same time - server and client.<br>
You should create room, then create link to give ability to another person<br>
to communicate through the Blink room. It can be saved on harddrive or it can<br>
be sent via e-mail.<br> To make it able, you should have your port forwarded(default is 8189).

So it has 2 interface modes: graphical and console.<br>

# TODO-list
It is not finished yet. I think it will be not finished at all. :smiling_face_with_tear:<br> 
- [x] check how correct is network connection in console mode.
- [ ] write Cmake file.
