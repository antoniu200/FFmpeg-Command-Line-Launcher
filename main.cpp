#include <iostream>
#include <windows.h>
#include <vector>
#include <fstream>
#include <string>
#include <limits>

using namespace std;

void leave_or_start_over() {
	cout << "Please type in the action you want to do next:\n";
	cout << "Leave (L)\n";
	cout << "Start over (S)\n";
}

int main() {
	string ffmpeg_loc;
	string output_loc;
	ifstream dir_in("Locations.txt");
	ifstream command_in("command.txt");


	/// Where the command is created and saved, if not given. Should be moved in a function.
	string command;
	getline(command_in, command);
	command_in.close();
	if (command == "") {
		cout << "Please type in the command you want to use with ffmpeg.exe.\nReplace the path of the audio file with \"<a>\" and the path of the image\nfile with \"<i>\".\n";
		getline(cin, command);

		ofstream command_out("command.txt");
		command_out << command;
		system("cls");
	}


	/// Where the FFmpeg and output directory location is saved, if not given. Should be moved in a function.
	getline(dir_in, ffmpeg_loc);
	getline(dir_in, output_loc);
	output_loc.resize(output_loc.size() - 1);
	dir_in.close();
	if (ffmpeg_loc == "") {
		cout << "Please copy the location of your ffmpeg.exe or drag & drop it here.\n";
		getline(cin, ffmpeg_loc);

		ofstream fout("Locations.txt");
		fout << ffmpeg_loc << "\n";
		fout.close();
	}
	/* if (output_loc == "") {
		cout << "Please type in the output directory or drag & drop it here.\n";
		getline(cin, output_loc);

		ofstream fout("Locations.txt");
		fout << output_loc;
		fout.close();
	}*/ /// Needs testing!


	/// Main program, part 1.
	cout << "Hello!\nTo begin using this program, press Enter.\n";
	getchar();
	vector<string> image_paths;
	vector<string> audio_paths;
	string curr_path; /// String used to read paths and to be emplaced in above vectors

	/// Where image and audio locations are set, if needed. Should be moved in a function.
	for (;;) {
		image_paths.clear();
		image_paths.shrink_to_fit();
		audio_paths.clear();
		audio_paths.shrink_to_fit();
		system("cls");

		cout << "Please drag & drop your images here in the order you want them encoded.\nOnce you are done, press Enter.\n";
		while (getline(cin, curr_path) && curr_path != "") {
			image_paths.emplace_back(curr_path);
		}
		system("cls");
		if (!image_paths.size()) {
			cout << "You did not enter any image paths.\n";
			leave_or_start_over();

			char x;
			if (x == 'L' || x == 'l')
				return 0;
			else
				continue;
		}

		cout << "Please drag & drop your audio files here in the order you want them encoded.\nOnce you are done, press Enter.\n";

		while (getline(cin, curr_path) && curr_path != "") {
			audio_paths.emplace_back(curr_path);
		}
		system("cls");
		if (!audio_paths.size()) {
			cout << "You did not enter any audio file paths.\n";
			leave_or_start_over();

			char x;
			x = (char)getchar();
			if (x == 'L' || x == 'l')
				return 0;
			else
				continue;
		}

		if (audio_paths.size() != image_paths.size()) {
			cout << "The number of images given is different than the number of audio files given.\nPlease type in the action you want to do next:\n1. Start over\n2. Remove the trailing ";
			bool audios = 0;
			if (audio_paths.size() > image_paths.size())
				cout << "audio files", audios = 1;
			else
				cout << "images";
			cout << "\n";
			if (audios)
				cout << "3. Use the same image for every audio file\n";

			char next_action;
			cin >> next_action;
			if (next_action == '1') {
				system("cls");
				continue;
			} else if (next_action == '2') {
				if (audio_paths.size() > image_paths.size())
					while (audio_paths.size() > image_paths.size())
						audio_paths.pop_back();
				else
					while (image_paths.size() > audio_paths.size())
						image_paths.pop_back();
			} else {
				image_paths.resize(audio_paths.size());
				for (size_t i = 1; i < image_paths.size(); i++)
					image_paths[i] = image_paths[0];
			}
		}
		break;
	}

    /// Command runner. Should be moved in a function.
	for (unsigned i = 0; i < audio_paths.size(); i++) {
		size_t r_limit;
		for (size_t j = audio_paths[i].size() - 1; j < numeric_limits<size_t>::max(); j--)
			if (audio_paths[i][j] == '.') {
				r_limit = j;
				break;
			}
		size_t l_limit;
		for (size_t j = r_limit; j < numeric_limits<size_t>::max(); j--)
			if (audio_paths[i][j] == '\\') {
				l_limit = j;
				break;
			}

		string audio_name;
		for (size_t j = l_limit + 1; j < r_limit; j++)
			audio_name += audio_paths[i][j];

		cout << "Running command " << i + 1 << " out of " << audio_paths.size() << "...\n";

		/// Command patcher:
		string current_command = "";
		string file_type = "";
		size_t file_type_pos = 0;
		for (size_t it = 0; it < command.size(); it++) {
			if (command[it] == '<' && command[it + 1] == 'i')
				current_command += image_paths[i], it += 2;
			else if (command[it] == '<' && command[it + 1] == 'a')
				current_command += audio_paths[i], it += 2;
			else {
				if (command[it] != '.')
					current_command += command[it];
				else {
					file_type_pos = it;
					break;
				}
			}
		}
		if (file_type_pos)
			for (; file_type_pos < command.size(); file_type_pos++)
				file_type += command[file_type_pos];

		/// Executing command:
		string operation;
		operation += '"' + ffmpeg_loc + ' ' + current_command + ' ' + output_loc + audio_name + file_type + '"' + '"';
		system(operation.c_str());
	}

	/// Main program, part 2.
	cout << "Done!\nPress any key to exit.\n";
	getchar();
}
