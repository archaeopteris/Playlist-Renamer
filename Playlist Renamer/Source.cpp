//////////////////////////////////////////////////////////////////////////
//	Name: Playlist Renamer												//
//	Version: 1.0														//
//	Author: Hayk Voskanyan												//
//																		//
//	Description:	This program reads the *.wpl music playlist file	//
//					and renames the audio file names according to order //
//					in which they appear in the	playlist.				//
//																		//
//			Note:	The executable program should be placed in the		//
//					same folder with audio files as well as				//
//					playlist file.										//
//																		//
//////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>	// For reverse
#include <Windows.h>	// For MoveFileEx
#include <WinBase.h>
#include <iomanip>

using namespace std;

// Create an instance for ifstream.
ifstream playlist("playlist.wpl");

void main()
{
	string line;
	cout << "		//////////////////////////////////" << endl;
	cout << "		//	Playlist Renamer	//" << endl;
	cout << "		//////////////////////////////////" << endl << endl;

	cout << "	This program reads the *.wpl music playlist file\n";
	cout << "	and renames the audio file names according to order\n";
	cout << "	in which they appear in the playlist." << endl << endl;

	cout << "	NOTE. This executable program should be placed" << endl;
	cout << "	in the same folder with audio files," << endl;
	cout << "	as well as playlist file." << endl << endl << endl;

	cout << "	Press any key to begin to rename files..." << endl;

	cin.get();

	// Run on the file reading line by line.
	while (getline(playlist, line))
	{
		// Check for the beginning of sequence part in a playlist script.
		if (line == "        <seq>")
		{
			int i = 1;

			while (getline(playlist, line))
			{
				if (line == "        </seq>") { break; }	// Check for the end of the sequence.

				// Reverse the line to search for the beginning of audio file name.
				reverse(line.begin(), line.end());

				// Find the beginning of audio file name.
				size_t slashLoc = line.find("\\", 2);
				
				// Restore the original order of symbols in a line.
				reverse(line.begin(), line.end());

				// Temporary string that contains the audio file name plus some trash at the end of line.
				string tempString = line.substr(line.length() - slashLoc);

				// Find the end of audio file name.
				size_t endLoc = tempString.find("\"");

				// Get pure Audio name.
				string audioName = tempString.substr(0, endLoc);

				string sequenceNumb;

				// Convert the sequence number to the string.
				ostringstream convert;   // stream used for the conversion
				convert << i;
				if (i < 10)
				{
					sequenceNumb = "00" + convert.str();
				}
				else if (i >= 10 && i < 100)
				{
					sequenceNumb = "0" + convert.str();
				}
				else
				{
					sequenceNumb = convert.str();
				}

				// Construct the new name of the file adding a sequence number at the beginning.
				string renamedAudio;
				renamedAudio += sequenceNumb + ". " + audioName;

				// Convert string to wstring
				std::wstring stemp = std::wstring(audioName.begin(), audioName.end());
				std::wstring stemp2 = std::wstring(renamedAudio.begin(), renamedAudio.end());
				LPCWSTR sw = stemp.c_str();
				LPCWSTR sw2 = stemp2.c_str();

				// Rename the audio file according to sequence order
				int rc = MoveFileEx(sw, sw2, MOVEFILE_COPY_ALLOWED);

				// Print the status of renaming.
				if (rc)
				{
					cout << i << "  Renaming ";
					cout << setfill(' ') << setw(40);
					cout << audioName;
					cout << setw(0); 
					cout << "  Ok" << endl;
				}
				else
				{
					perror("Error renaming file ");
				}

				i++;

			}
			break;
		}
	}

	// Prevent the console screen from closing at the end.
	cin.get();
}
