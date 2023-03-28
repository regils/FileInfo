#include <iostream>
#include <filesystem>
#include <fstream>
#include <Windows.h>


using namespace std;


// function which gets .exe and .dll files from selected directory and other directories inside
void get_file_paths(vector<filesystem::path>& paths, const string& current_path){
	for (const auto& file : filesystem::directory_iterator(current_path)){
		// if file path is a directory - get file paths in this directory
		if (filesystem::is_directory(file)){
			get_file_paths(paths, file.path().string());
		}
		// if file path is a file - enter file path into our vector
		else if ((file.path().extension() == ".exe" || file.path().extension() == ".dll")){
			paths.push_back(file.path());
		}
	}
}


// function which gets version of file (directory should end on "...\\filename")
string GetFileVersion(const string& filePath){
	// getting file description
    DWORD handle = 0;
    DWORD fileVersionInfoSize = GetFileVersionInfoSizeA(filePath.c_str(), &handle);

	// allocating memory for file information
    LPVOID fileVersionInfo = malloc(fileVersionInfoSize);

	// getting file version information
    GetFileVersionInfoA(filePath.c_str(), handle, fileVersionInfoSize, fileVersionInfo);
    UINT len = 0;
    VS_FIXEDFILEINFO* fileInfo = nullptr;
    VerQueryValueA(fileVersionInfo, "\\", (LPVOID*)&fileInfo, &len);

	// forming file version as string
    string version = to_string(HIWORD(fileInfo->dwFileVersionMS)) + "."
        + to_string(LOWORD(fileInfo->dwFileVersionMS)) + "."
        + to_string(HIWORD(fileInfo->dwFileVersionLS)) + "."
        + to_string(LOWORD(fileInfo->dwFileVersionLS));

	// freeing memory for file information
    free(fileVersionInfo);

    return version;
}


int main(){
	// initializing variables
	string folderPath;
	string txtPath;
	string txtName;
	ofstream list;

	cout << "Enter folder path: ";
	cin >> folderPath;
	cout << "Enter path to text document: ";
	cin >> txtPath;
	cout << "Enter text document name: ";
	cin >> txtName;
	const string txtPath_Full = txtPath + "\\" + txtName + ".txt";

	// creating vector class variable for file paths
	vector<filesystem::path> paths;

	// getting file paths
	get_file_paths(paths, folderPath);

	// creating and opening text document for editing
	list.open(txtPath_Full);

	// adding a check if file is opened and entering files' information
	if (list.is_open()) {
		for (int i = 0; i < paths.size(); i++) {
			list << paths[i].filename().string() << "\t" 
				<< filesystem::last_write_time(paths[i]) << "\t" 
				<< GetFileVersion(paths[i].string()) << endl;
		}
		cout << "Text document was successfuly created! You can find it at: " << txtPath_Full << endl;
	}
	else
		cout << "Error!";

	// closing text document for editing
	list.close();

	return 0;
}