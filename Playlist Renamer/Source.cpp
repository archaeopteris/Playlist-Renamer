//////////////////////////////////////////////////////////////////////////
//	Name: Playlist Renamer												//
//	Version: 1.5														//
//	Author: Hayk Voskanyan												//
//																		//
//	Description:	This program reads the music playlist file 			//
//					and renames the audio file names according			//
//					to order in which they appear in the playlist.		//
//					Currently only wpl and m3u formats are supported.	//
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
#include <tchar.h>
#include <stdio.h>
#include <exception>
#include <errno.h>

// The Function tries to open requested file and returns true if succeded.
// Used for file existence check.
errno_t checkFileExistence(const std::string& name);

// Checks the error code given by fopen_s and prints apropriate message.
void printError(errno_t e);

int main()
{
	std::string line;
	std::cout << "		//////////////////////////////////" << std::endl;
	std::cout << "		//	Playlist Renamer	//" << std::endl;
	std::cout << "		//////////////////////////////////" << std::endl << std::endl;

	// String container for playlist file name;
	std::string playlistName = "";

	bool wplChecker = false;
	bool m3uChecker = false;
	int j = 0;
	
	// Try to find playlist files in current directory
	try
	{
		errno_t m3uErrorCode= checkFileExistence("playlist.m3u");
		errno_t wplErrorCode = checkFileExistence("playlist.wpl");

		if ((m3uErrorCode == 0) && (wplErrorCode != 0))
		{
			m3uChecker = true;
		}
		else if ((m3uErrorCode != 0) && (wplErrorCode == 0))
		{
			wplChecker = true;
		}
		else if ((m3uErrorCode == 0) && (wplErrorCode == 0))
		{
			m3uChecker = true;
			wplChecker = true;
		}
		else
		{
			throw m3uErrorCode;
		}
	}
	catch (errno_t e)
	{
		printError(e);
		std::cout << e << std::endl;
		std::cin.get();
		return EXIT_FAILURE;
	}
	


	// Checks if both supported formats exist, or only one of them.
	// If only one of files exists, program automatically selects it for processing,
	// otherwise it asks you, which file do you want.
	if (wplChecker && m3uChecker)
	{
		std::cout << "  I found both *.wpl and *.m3u files.\n";
		std::cout << "  Which one do you want to use?\n";
		std::cout << "  Press 1 for playlist.wpl file,\n";
		std::cout << "  Press 2 for playlist.m3u file." << std::endl;
		std::cin >> j;

		if (j == 1)
		{
			playlistName = "playlist.wpl";
			m3uChecker = false;
		}
		else if (j == 2)
		{
			playlistName = "playlist.m3u";
			wplChecker = false;
		}
		else
		{
			std::cout << "  Worng number.";
			return EXIT_FAILURE;
		}
	}
	else if (m3uChecker && !wplChecker)
	{
		playlistName = "playlist.m3u";
	}
	else if (wplChecker && !m3uChecker)
	{
		playlistName = "playlist.wpl";
	}
	else // This should never happen.
	{
		std::cout << "  Unknown Error...";
		std::cin.get();
		return EXIT_FAILURE;
	}

	std::cout << playlistName << " selected!" << std::endl;

	int i = 0;		// Keeps the sequence number.
	int k = 0;		// Counts number of renamed files.

	
	if (m3uChecker) { std::cout << "	This program reads the playlist.m3u music playlist file\n"; }

	if (wplChecker) { std::cout << "	This program reads the playlist.wpl music playlist file\n"; }
		
	std::cout << "	and renames the audio files according to order\n";
	std::cout << "	in which they appear in the playlist." << std::endl << std::endl;

	std::cout << "	NOTE. This executable program should be placed" << std::endl;
	std::cout << "	in the same folder with audio files," << std::endl;
	std::cout << "	as well as playlist file." << std::endl << std::endl << std::endl;

	std::cout << "	Press any key to begin the renaming process..." << std::endl;

	std::cin.get();


	if (m3uChecker)					// If playlist.m3u was selected
	{
		// Create an instance for ifstream.
		std::ifstream playlist("playlist.m3u");

		// Run on the file reading line by line.
		while (std::getline(playlist, line))
		{
			std::getline(playlist, line);

			// Find the begining of audio file name.
			size_t nameBeginning = line.find(",");

			// Get pure Audio name.
			std::string audioName = line.substr(nameBeginning + 1) + ".mp3";

			std::cout << audioName << std::endl;



			std::string sequenceNumb;

			// Convert the sequence number to the std::string.
			std::ostringstream convert;   // stream used for the conversion
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
			std::string renamedAudio;
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
				std::cout << i << "  Renaming ";
				std::cout << std::setfill(' ') << std::setw(40);
				std::cout << audioName;
				std::cout << std::setw(0);
				std::cout << "  Ok" << std::endl;

				k++;
			}
			else
			{
				perror("  Error renaming file ");
				std::cout << audioName << std::endl;
			}

			i++;
		}
	}///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else if (wplChecker)			// If playlist.wpl was selected
	{
		// Create an instance for ifstream.
		std::ifstream playlist("playlist.wpl");

		// Run on the file, reading line by line.
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
					std::string tempString = line.substr(line.length() - slashLoc);

					// Find the end of audio file name.
					size_t endLoc = tempString.find("\"");

					// Get pure Audio name.
					std::string audioName = tempString.substr(0, endLoc);

					std::string sequenceNumb;

					// Convert the sequence number to the string.
					std::ostringstream convert;   // stream used for the conversion
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
					std::string renamedAudio;
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
						std::cout << i << "  Renaming ";
						std::cout << std::setfill(' ') << std::setw(40);
						std::cout << audioName;
						std::cout << std::setw(0);
						std::cout << "  Ok" << std::endl;

						k++;
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
	}
	else
	{
		std::cout << "  Unknown error. Both wplChecker and m3uChecker are false..." << std::endl;
		std::cin.get();

		return EXIT_FAILURE;
	}

	std::cout << std::endl << std::endl << "Files found " << k << std::endl;

	// Prevent the console screen from closing at the end.
	std::cin.get();

	return EXIT_SUCCESS;
}



// The Function tries to open requested file and returns 0 if succeded and error code otherwise.
// Used for file existence check.
errno_t checkFileExistence(const std::string& name)
{
	errno_t tmpFile, err;			// Error Container.
	FILE *playlistFile;		// Playlist File Container.

	tmpFile = fopen_s(&playlistFile, name.c_str(), "r");
	err = tmpFile;

	// If file has been opened, close it.
	if (err == 0)
	{
		fclose(playlistFile);
	}
	return err;
}

// Checks the error code given by fopen_s and prints apropriate message.
void printError(errno_t errorCode)
{
	switch (errorCode)
	{
	case 1:
		std::cout << "Operation not permitted" << std::endl;
		break;
	case 2:
		std::cout << "No such file or directory" << std::endl;
		break;
	case 5:
		std::cout << "I/O error" << std::endl;
		break;
	case 6:
		std::cout << "No such device or address" << std::endl;
		break;
	case 7:
		std::cout << "Argument list too long" << std::endl;
		break;
	case 11:
		std::cout << "No more processes or not enough memory or maximum nesting level reached" << std::endl;
		break;
	case 12:
		std::cout << "Not enough memory" << std::endl;
		break;
	case 13:
		std::cout << "Permission denied" << std::endl;
		break;
	case 14:
		std::cout << "Bad address" << std::endl;
		break;
	case 16:
		std::cout << "Device or resource busy" << std::endl;
		break;
	case 17:
		std::cout << "File exists" << std::endl;
		break;
	case 19:
		std::cout << "No such device" << std::endl;
		break;
	case 20:
		std::cout << "Not a directory" << std::endl;
		break;
	case 21:
		std::cout << "Is a directory" << std::endl;
		break;
	case 22:
		std::cout << "Invalid argument" << std::endl;
		break;
	case 23:
		std::cout << "Too many files open in system" << std::endl;
		break;
	case 24:
		std::cout << "Too many open files" << std::endl;
		break;
	case 27:
		std::cout << "File too large" << std::endl;
		break;
	case 28:
		std::cout << "No space left on device" << std::endl;
		break;
	case 29:
		std::cout << "Invalid seek" << std::endl;
		break;
	case 30:
		std::cout << "Read-only file system" << std::endl;
		break;
	case 31:
		std::cout << "Too many links" << std::endl;
		break;
	case 32:
		std::cout << "Broken pipe" << std::endl;
		break;
	case 33:
		std::cout << "Math argument" << std::endl;
		break;
	case 34:
		std::cout << "Result too large" << std::endl;
		break;
	case 38:
		std::cout << "Filename too long" << std::endl;
		break;
	case 39:
		std::cout << "No locks available" << std::endl;
		break;
	case 41:
		std::cout << "irectory not empty" << std::endl;
		break;
	case 42:
		std::cout << "Illegal byte sequence" << std::endl;
		break;
	case 80:
		std::cout << "String was truncated" << std::endl;
		break;
	default:
		std::cout << "Unknown Error N" << errorCode << "..." << std::endl;
	}
}
