#pragma once
namespace Blink
{
	enum class RoomNetworkMode
	{
		OneToOne,
		OneToOnes,
		Decentralysed
	};
	static const char* get_room_help()
	{
		return "Blink provides several ways for  room behaviour:\n\
				1) OneToOne is you(room maintainer) and person who connects\n\
				    to your room. Simply, there is no one else who can connect.\n\n\
			    2) OneToOnes is you(room maintainer) and different people who communictes\n\
					with each other, including you.\n\n\
			    3) Decentralysed mode has no maintainers or owners of any kind!\n\n\
					So once you start this room you can leave  it, but everyone,who\n\
					has access to it, can connect at any time he/she wants.";
	}
};