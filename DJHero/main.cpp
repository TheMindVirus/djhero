#include <Windows.h>
#include <Xinput.h>
#include <mmsystem.h>
#include <stdio.h>
#pragma comment(lib, "XInput.lib")
#pragma comment(lib, "WinMM.lib")

#define MIDIMSG(CHANNEL, CTRL, VALUE) (0xB0 + CHANNEL + (CTRL << 8) + (VALUE << 16))
#define SCALEUP(VALUE) ((255 * VALUE) / 127)
#define SCALEDN(VALUE) ((127 * VALUE) / 255)
#define THUMBUP(VALUE) ((VALUE * 512) - 32768)
#define THUMBDN(VALUE) ((VALUE + 32768) / 512)
#define DECKFWD(VALUE) (VALUE - 64)
#define DECKBCK(VALUE) (VALUE + 64)
#define BUTTONF(VALUE, BTNVAL) ((((int)VALUE) > 0) ? BTNVAL : 0)

struct GLOBAL
{
	bool running;
	XINPUT_CAPABILITIES ControllerCaps[4];
	XINPUT_STATE Controller[4];
	XINPUT_STATE ControllerPrevious[4];
	MIDIOUTCAPS MIDIcaps;
	HMIDIOUT MIDIdev;
	int numDevs;
	int szCaps;
	GLOBAL()
	{
		running = false;
		numDevs = midiOutGetNumDevs();
		szCaps = sizeof(MIDIOUTCAPS);
	}
};
static struct GLOBAL App;

BOOL WINAPI ExitHandler(DWORD signal)
{
	App.running = false;
	return TRUE;
}

int main()
{
	SetConsoleCtrlHandler(ExitHandler, true);
	printf("__//%s\\\\__\n", "DJHero");

	for(int i = 0; i < App.numDevs; ++i)
	{
		midiOutGetDevCaps(i, &App.MIDIcaps, App.szCaps);
		if (0 == strcmp(App.MIDIcaps.szPname, "DJHero"))
		{
			if (MMSYSERR_NOERROR == midiOutOpen(&App.MIDIdev, i, NULL, NULL, CALLBACK_NULL))
			{
				printf("%s\n", "loopMIDI Device opened");
				App.running = true;
				break;
			}
		}
	}
	if (!App.running)
	{
		printf("Error: %s\n", "Could not find loopMIDI device named DJHero");
	}
	
	while(App.running)
	{
		for (int i = 0; i < 4; ++i)
		{
			App.ControllerPrevious[i] = App.Controller[i];
			XInputGetState(i, &App.Controller[i]);
			XInputGetCapabilities(i, XINPUT_FLAG_GAMEPAD, &App.ControllerCaps[i]);

			//Analog Controls
			if (App.Controller[i].Gamepad.bLeftTrigger != App.ControllerPrevious[i].Gamepad.bLeftTrigger)
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 0, SCALEDN(App.Controller[i].Gamepad.bLeftTrigger)));
			if (App.Controller[i].Gamepad.bRightTrigger != App.ControllerPrevious[i].Gamepad.bRightTrigger)
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 1, SCALEDN(App.Controller[i].Gamepad.bRightTrigger)));
			if (App.ControllerCaps[i].SubType == 23)
			{
				if (App.Controller[i].Gamepad.sThumbLX != App.ControllerPrevious[i].Gamepad.sThumbLX)
					midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 2, DECKBCK(App.Controller[i].Gamepad.sThumbLX)));
				if (App.Controller[i].Gamepad.sThumbLY != App.ControllerPrevious[i].Gamepad.sThumbLY)
					midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 3, DECKBCK(App.Controller[i].Gamepad.sThumbLY)));
			}
			else
			{
				if (App.Controller[i].Gamepad.sThumbLX != App.ControllerPrevious[i].Gamepad.sThumbLX)
					midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 2, THUMBDN(App.Controller[i].Gamepad.sThumbLX)));
				if (App.Controller[i].Gamepad.sThumbLY != App.ControllerPrevious[i].Gamepad.sThumbLY)
					midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 3, THUMBDN(App.Controller[i].Gamepad.sThumbLY)));
			}
			if (App.Controller[i].Gamepad.sThumbRX != App.ControllerPrevious[i].Gamepad.sThumbRX)
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 4, THUMBDN(App.Controller[i].Gamepad.sThumbRX)));
			if (App.Controller[i].Gamepad.sThumbRY != App.ControllerPrevious[i].Gamepad.sThumbRY)
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 5, THUMBDN(App.Controller[i].Gamepad.sThumbRY)));
			
			//Digital Controls
			if ((App.Controller[i].Gamepad.wButtons & 0x0001) != (App.ControllerPrevious[i].Gamepad.wButtons & 0x0001))
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 6, BUTTONF(App.Controller[i].Gamepad.wButtons & 0x0001, 127)));
			if ((App.Controller[i].Gamepad.wButtons & 0x0002) != (App.ControllerPrevious[i].Gamepad.wButtons & 0x0002))
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 7, BUTTONF(App.Controller[i].Gamepad.wButtons & 0x0002, 127)));
			if ((App.Controller[i].Gamepad.wButtons & 0x0004) != (App.ControllerPrevious[i].Gamepad.wButtons & 0x0004))
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 8, BUTTONF(App.Controller[i].Gamepad.wButtons & 0x0004, 127)));
			if ((App.Controller[i].Gamepad.wButtons & 0x0008) != (App.ControllerPrevious[i].Gamepad.wButtons & 0x0008))
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 9, BUTTONF(App.Controller[i].Gamepad.wButtons & 0x0008, 127)));
			if ((App.Controller[i].Gamepad.wButtons & 0x0010) != (App.ControllerPrevious[i].Gamepad.wButtons & 0x0010))
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 10, BUTTONF(App.Controller[i].Gamepad.wButtons & 0x0010, 127)));
			if ((App.Controller[i].Gamepad.wButtons & 0x0020) != (App.ControllerPrevious[i].Gamepad.wButtons & 0x0020))
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 11, BUTTONF(App.Controller[i].Gamepad.wButtons & 0x0020, 127)));
			if ((App.Controller[i].Gamepad.wButtons & 0x0040) != (App.ControllerPrevious[i].Gamepad.wButtons & 0x0040))
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 12, BUTTONF(App.Controller[i].Gamepad.wButtons & 0x0040, 127)));
			if ((App.Controller[i].Gamepad.wButtons & 0x0080) != (App.ControllerPrevious[i].Gamepad.wButtons & 0x0080))
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 13, BUTTONF(App.Controller[i].Gamepad.wButtons & 0x0080, 127)));
			if ((App.Controller[i].Gamepad.wButtons & 0x0100) != (App.ControllerPrevious[i].Gamepad.wButtons & 0x0100))
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 14, BUTTONF(App.Controller[i].Gamepad.wButtons & 0x0100, 127)));
			if ((App.Controller[i].Gamepad.wButtons & 0x0200) != (App.ControllerPrevious[i].Gamepad.wButtons & 0x0200))
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 15, BUTTONF(App.Controller[i].Gamepad.wButtons & 0x0200, 127)));
			if ((App.Controller[i].Gamepad.wButtons & 0x0400) != (App.ControllerPrevious[i].Gamepad.wButtons & 0x0400))
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 16, BUTTONF(App.Controller[i].Gamepad.wButtons & 0x0400, 127)));
			if ((App.Controller[i].Gamepad.wButtons & 0x0800) != (App.ControllerPrevious[i].Gamepad.wButtons & 0x0800))
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 17, BUTTONF(App.Controller[i].Gamepad.wButtons & 0x0800, 127)));
			if ((App.Controller[i].Gamepad.wButtons & 0x1000) != (App.ControllerPrevious[i].Gamepad.wButtons & 0x1000))
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 18, BUTTONF(App.Controller[i].Gamepad.wButtons & 0x1000, 127)));
			if ((App.Controller[i].Gamepad.wButtons & 0x2000) != (App.ControllerPrevious[i].Gamepad.wButtons & 0x2000))
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 19, BUTTONF(App.Controller[i].Gamepad.wButtons & 0x2000, 127)));
			if ((App.Controller[i].Gamepad.wButtons & 0x4000) != (App.ControllerPrevious[i].Gamepad.wButtons & 0x4000))
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 20, BUTTONF(App.Controller[i].Gamepad.wButtons & 0x4000, 127)));
			if ((App.Controller[i].Gamepad.wButtons & 0x8000) != (App.ControllerPrevious[i].Gamepad.wButtons & 0x8000))
				midiOutShortMsg(App.MIDIdev, MIDIMSG(i, 21, BUTTONF(App.Controller[i].Gamepad.wButtons & 0x8000, 127)));
		}
	
#ifdef _DEBUG
		/*
		printf("%d\n", SCALEDN(App.Controller[0].Gamepad.bLeftTrigger));
		printf("0x%08x\n", MIDIMSG(0, 0, SCALEDN(App.Controller[0].Gamepad.bLeftTrigger)));
		printf("%d\n", App.Controller[0].Gamepad.sThumbLX);
		printf("%d\n", THUMBDN(App.Controller[0].Gamepad.sThumbLX));
		printf("%d\n", App.Controller[0].Gamepad.wButtons);
		printf("%d\n", BUTTONF(App.Controller[0].Gamepad.wButtons & 0x100l, 127));
		*/
		printf("%d\n", App.ControllerCaps[0].SubType);
		printf("%d\n", DECKBCK(App.Controller[0].Gamepad.sThumbLX));
		printf("%d\n", THUMBDN(App.Controller[0].Gamepad.sThumbRX));
		printf("%d\n", THUMBDN(App.Controller[0].Gamepad.sThumbRY));
		Sleep(200);
#else
		Sleep(0);
#endif
	}

	printf("%s\n", "Closing...");
	Sleep(1000);
	return 0;
}