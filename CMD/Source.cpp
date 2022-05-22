#include <iostream>
#include<string>
#include<map>
#include<math.h>
#include<vector>
#include <Windows.h> // for SetConsoleTitle
#include <fstream>  // for files write and read
#include <sstream> // for spilt
#include<algorithm>  // for transform string
#include<stdlib.h>  // for clc function
#include <sys/stat.h>
using namespace std;

bool IsPathExist(const std::string& s)
{
    struct stat buffer;
    return (stat(s.c_str(), &buffer) == 0);
}
bool FileExists(string myfile)
{
    ifstream file(myfile.c_str());
    if (file)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

class virtual_disk {

public:
    virtual_disk() {
        if (!FileExists("CMD.txt")) {
            ofstream Filetxt("CMD.txt");
            // supper_block "0" 1024 byte
            char x = '0';
            for (int i = 0; i < 1024; i++)
            {
                Filetxt << x;
            }
            // Meta_data "*" 4*1024 byte
            x = '*';
            for (int i = 0; i < 4 * 1024; i++)
            {
                Filetxt << x;
            }
            // data_file "#" 1019*1024 byte
            x = '#';
            for (int i = 0; i < 1019 * 1024; i++)
            {
                Filetxt << x;
            }
            Filetxt.close();
        }
        else {

        }
    }

    void write_block(vector<char>v, int index) {
        //index--;
        index *= 1024;
        fstream Filetxt("CMD.txt");
        Filetxt.seekg(index, ios::beg);
        for (int i = 0; i < min((int)v.size(), 1024); i++) {
            Filetxt << v[i];
        }
        Filetxt.close();
    }
    vector<char> get_block(int index) {
        int st = (index * 1024);
        int en = st + 1024;
        vector<char>val = ReadFormTo(st, en);
        return val;
    }
    vector<char> ReadFormTo(int start, int end) {
        vector<char>val;
        string block;
        ifstream Filetxt("CMD.txt");
        Filetxt >> block;
        for (int i = start; i < end; i++) {
            val.push_back(block[i]);
        }
        Filetxt.close();
        return val;
    }
};

// FAT_TABLE

class FAT_table
{
public:
    vector<int>fat;
    //constractor for initializations
    FAT_table()
    {
        if (!FileExists("CMD.txt")) {
            for (int i = 0; i < 1024; i++) {
                if (i < 5) {
                    fat.push_back(-1);
                }
                else {
                    fat.push_back(0);
                }
            }
        }
        else {
            fat = get_FAT_table();
        }
    }
    void write_FAT_table()
    {
        fstream Filetxt("CMD.txt");
        Filetxt.seekg(1024, ios::beg);
        vector<char> val = intTobyte(fat);
        for (int i = 0; i < val.size(); i++) {
            if (i < val.size()) {
                Filetxt << val[i];
            }
            else {
                Filetxt << '0';
            }
        }
        Filetxt.close();
    }

    vector<int> get_FAT_table()
    {
        vector<char> val = ReadFormTo(1024, 4 * 1024);
        vector<int> reFat = bytesToInt(val);
        return reFat;
    }

    void print_FAT_table()
    {
        vector<int>v = get_FAT_table();
        for (int a : v) {
            cout << a << " ";
        }
    }

    int get_avalible_block() {
        vector<int> v = get_FAT_table();
        int index = 0;
        for (int a : v) {
            if (a == 0) {
                return index;
            }
            index++;
        }
        return -1;
    }

    int get_avalible_blocks() {
        vector<int> v = get_FAT_table();
        int index = 0;
        for (int a : v) {
            if (a == 0) {
                index++;
            }
        }
        return index;
    }

    int get_next_index(int index) {
        vector<int>v = get_FAT_table();
        return v[index];
    }

    void set_next_index(int index, int value) {
        fat[index] = value;
        write_FAT_table();
    }

    vector<char> ReadFormTo(int start, int end) {
        vector<char>val;
        string block;
        ifstream Filetxt("CMD.txt");
        Filetxt >> block;
        for (int i = start; i < end; i++) {
            val.push_back(block[i]);
        }
        Filetxt.close();
        return val;
    }

    vector<int> bytesToInt(vector<char>vas) {
        int num = 0;
        vector<int>val;
        for (int i = 0; i < vas.size(); i++) {
            if ((i + 1) % 4 == 0)
            {
                num = (num * 10) + (vas[i] - '0');
                if (num == 1111) {
                    val.push_back(-1);
                }
                else {
                    val.push_back(num);
                }
                num = 0;
            }
            else
            {
                num = (num * 10) + (vas[i] - '0');
            }
        }
        return val;
    }

    vector<char> intTobyte(vector<int>v) {
        vector<char>val;
        for (int i = 0; i < v.size(); i++) {
            int n = v[i];
            if (n == -1) {
                val.push_back(1 + '0');
                val.push_back(1 + '0');
                val.push_back(1 + '0');
                val.push_back(1 + '0');
            }
            else {
                char v1 = n % 10;
                n /= 10;
                char v2 = n % 10;
                n /= 10;
                char v3 = n % 10;
                n /= 10;
                char v4 = n % 10;
                val.push_back(v4 + '0');
                val.push_back(v3 + '0');
                val.push_back(v2 + '0');
                val.push_back(v1 + '0');
            }
        }
        return val;
    }
};
FAT_table fa_tab;
virtual_disk vir_disk;
class Directory_Entry {
public:
    char FileName[11];
    char FileEmpty[12];
    char FileAttribute;
    int  FileSize;
    int  Firstcluster;
    vector<char> get_bytes(Directory_Entry d) {
        vector<char>v;
        for (int i = 0; i < 11; i++) {
            v.push_back(d.FileName[i]);
        }
        for (int i = 0; i < 12; i++) {
            v.push_back(d.FileEmpty[i]);
        }
        v.push_back(d.FileAttribute);
        int n = d.FileSize;
        char v1 = n % 10;
        n /= 10;
        char v2 = n % 10;
        n /= 10;
        char v3 = n % 10;
        n /= 10;
        char v4 = n % 10;
        v.push_back(v4 + '0');
        v.push_back(v3 + '0');
        v.push_back(v2 + '0');
        v.push_back(v1 + '0');
        n = d.Firstcluster;
        v1 = n % 10;
        n /= 10;
        v2 = n % 10;
        n /= 10;
        v3 = n % 10;
        n /= 10;
        v4 = n % 10;
        v.push_back(v4 + '0');
        v.push_back(v3 + '0');
        v.push_back(v2 + '0');
        v.push_back(v1 + '0');
        return v;
    }

    Directory_Entry get_Directory_Entry(vector<char>v) {
        Directory_Entry d;
        int num1 = 0, num2 = 0;
        for (int i = 0; i < v.size(); i++) {
            if (i < 11) {
                d.FileName[i] = v[i];
            }
            else if (i < 23) {
                d.FileEmpty[i - 11] = v[i];
            }
            else if (i == 23) {
                d.FileAttribute = v[i];
            }
            else if (i < 28) {
                num1 = (num1 * 10) + (v[i] - '0');
            }
            else {
                num2 = (num2 * 10) + (v[i] - '0');
            }
        }
        d.FileSize = num1;
        d.Firstcluster = num2;
        return d;
    }
};

class Directory : public Directory_Entry
{
public:
    vector<Directory_Entry>list;
    Directory* Parent;
    void Write_directory() {
        if (list.size() != 0) {
            vector<char>dir_table_bytes;
            for (int i = 0; i < list.size(); i++) {
                vector<char>dir_ele = list[i].get_bytes(list[i]);
                for (int j = 0; j < 32; j++) {
                    dir_table_bytes.push_back(dir_ele[j]);
                }
            }
            int number_of_record_blockes = ceil((double)dir_table_bytes.size() / 1024.0);
            //cout << number_of_record_blockes << " " << list.size() << " " << dir_table_bytes.size()<< endl;
            int number_fill_blocks = dir_table_bytes.size() / 1024;
            int rem_bytes = dir_table_bytes.size() % 1024;
            if (fa_tab.get_avalible_blocks() >= number_of_record_blockes) {
                for (int i = 0; i < number_fill_blocks; i++) {
                    vector<char>valuetowrite;
                    for (int j = i * 1024; j < (i + 1) * 1024; j++) {
                        valuetowrite.push_back(dir_table_bytes[j]);
                    }
                    int pos = fa_tab.get_avalible_block();
                    if (pos != -1) {
                        vir_disk.write_block(valuetowrite, pos);
                        fa_tab.set_next_index(pos, -1);
                    }
                }
                vector<char>valuetowrite;
                for (int i = number_fill_blocks * 1024; i < (number_fill_blocks * 1024) + rem_bytes; i++) {
                    valuetowrite.push_back(dir_table_bytes[i]);
                }
                int pos = fa_tab.get_avalible_block();
                //cout << pos << endl;
                vir_disk.write_block(valuetowrite, pos);
                fa_tab.set_next_index(pos, -1);
            }
            else {
                cout << "Net enough blockes\n";
            }
        }
        else {
            cout << "list is empty\n";
        }
    }

    vector<Directory_Entry> read_directory() {
        vector<Directory_Entry> valdir;
        Directory_Entry d;
        vector<char> vas = ReadFormTo(5120);
        for (int i = 0; i < vas.size() / 32; i++) {
            vector<char>ele;
            for (int j = i * 32; j < 32 * (i + 1); j++) {
                ele.push_back(vas[j]);
            }
            valdir.push_back(d.get_Directory_Entry(ele));
            ele.clear();
        }
        return valdir;
    }

    int search_directory(string filename) {
        vector<char> vas(11);
        for (int i = 0; i < 11; i++) {
            if (filename.size() > i) {
                vas[i] = filename[i];
            }
            else {
                vas[i] = '0';
            }
        }
        int findval = -1;
        vector<Directory_Entry> valdir1 = read_directory();
        for (int i = 0; i < valdir1.size(); i++) {
            int ok = 1;
            for (int j = 0; j < 11; j++) {
                if (valdir1[i].FileName[j] != vas[j]) {
                    ok = 0;
                    break;
                }
            }
            if (ok) {
                findval = i;
                break;
            }
        }
        return findval;
    }

    void updata_directory(Directory_Entry d) {
        string s = "";
        for (int i = 0; i < 11; i++) {
            s += d.FileName[i];
        }
        if (search_directory(s) != -1) {
            string block;
            fstream Filetxt("CMD.txt");
            Filetxt >> block;
            int index = 0;
            for (int i = 5120; i < block.size(); i++) {
                if (block[i] == '#') {
                    i += 31;
                }
                else {
                    int ok = 1;
                    int ind = i;
                    for (int j = 0; j < 11 && ind < block.size(); j++) {
                        if (d.FileName[j] != block[ind]) {
                            ok = 0;
                            break;
                        }
                        ind++;
                    }
                    if (ok) {
                        index = i;
                    }
                }
                if (index) {
                    break;
                }
            }
            if (index != 0) {
                Filetxt.seekg(index, ios::beg);
                vector<char> dbytes = d.get_bytes(d);
                for (int i = 0; i < 32; i++) {
                    Filetxt << dbytes[i];
                }
            }
        }
        else {
            cout << "not found\n";
        }
    }
    void delete_directory(string filename) {
        vector<char> vas(11);
        for (int i = 0; i < 11; i++) {
            if (filename.size() > i) {
                vas[i] = filename[i];
            }
            else {
                vas[i] = '0';
            }
        }
        if (search_directory(filename) != -1) {
            string block;
            fstream Filetxt("CMD.txt");
            Filetxt >> block;
            int index = 0;
            for (int i = 5120; i < block.size(); i++) {
                if (block[i] == '#') {
                    i += 31;
                }
                else {
                    int ok = 1;
                    int ind = i;
                    for (int j = 0; j < 11 && ind < block.size(); j++) {
                        if (vas[j] != block[ind]) {
                            ok = 0;
                            break;
                        }
                        ind++;
                    }
                    if (ok) {
                        index = i;
                    }
                }
                if (index) {
                    break;
                }
            }
            if (index != 0) {
                Filetxt.seekg(index, ios::beg);
                for (int i = 0; i < 32; i++) {
                    Filetxt << '#';
                }
            }
        }
        else {
            cout << "not found\n";
        }
    }

    vector<char> ReadFormTo(int start) {
        vector<char>val;
        string block;
        ifstream Filetxt("CMD.txt");
        Filetxt >> block;
        for (int i = start; i < block.size(); i++) {
            if (block[i] == '#') {
                i += 31;
            }
            else {
                val.push_back(block[i]);
            }
        }
        Filetxt.close();
        return val;
    }
};
// get path of file
string Current_Location()  //return the current location.
{
    return __FILE__;
}

// clc function clear the console
void cls()
{
    system("CLS");
}
// help function
map<string, string>mp;
map<int, string>mpcommend;
void help()
{
    mp["cls"] = "            Clears the screen."; mpcommend[0] = "cls";
    mp["help"] = "            Provides Help information for Windows commands."; mpcommend[1] = "help";
    mp["quie"] = "            Quits the CMD.EXE program(command interpreter)."; mpcommend[2] = "quie";
    mp["import"] = "            Import text file(s) from your computer​"; mpcommend[3] = "import";
    mp["export"] = "            Export text file(s) to your computer"; mpcommend[4] = "export";
    mp["type"] = "            Displays the contents of a text file."; mpcommend[5] = "type";
    mp["rename"] = "            Renames a file."; mpcommend[6] = "rename";
    mp["rd"] = "            Removes a directory."; mpcommend[7] = "rd";
    mp["md"] = "            Creates a directory."; mpcommend[8] = "md";
    mp["del"] = "            Deletes one or more files."; mpcommend[9] = "cls";
    mp["dir"] = "            List the contents of directory."; mpcommend[10] = "del";
    mp["copy"] = "            Copies one or more files to another location​."; mpcommend[11] = "copy";
    mp["cd"] = "            Change the current default directory to . If the argument is not present, report the current directory. If the directory does not exist an appropriate error should be reported.";
    mpcommend[12] = "cd";
}
//dir command List the contents of directory.​


void xport(string a, string b)   //export text file(s) to your computer​.
{
    char ch, sourceFile[30], targetFile[30];
    FILE* fs, * ft;
    for (int i = 0; i < a.size(); i++) {
        sourceFile[i] = a[i];
    }
    fs = fopen(sourceFile, "r");
    if (fs == NULL)
    {
        cout << "\nError Occurred!\n";
        return;
    }
    for (int i = 0; i < b.size(); i++) {
        targetFile[i] = b[i];
    }
    ft = fopen(targetFile, "w");
    if (ft == NULL)
    {
        cout << "\nError Occurred!\n";
        return;
    }
    ch = fgetc(fs);
    while (ch != EOF)
    {
        fputc(ch, ft);
        ch = fgetc(fs);
    }
    fclose(fs);
    fclose(ft);
}
// spilt string
vector<string>comSpilt;
void spilt(string s)
{
    stringstream ss(s);
    string word;
    while (ss >> word) {
        comSpilt.push_back(word);
    }
    transform(comSpilt[0].begin(), comSpilt[0].end(), comSpilt[0].begin(), ::tolower);//make all character lowercase

}
int main()
{
    Directory dire;
    SetConsoleTitleA("Command Prompt");
    string location = Current_Location(); // for location the fill
    while (true)
    {
        cout << location << "> ";
        string Command; // for the enter Command to do
        getline(cin, Command); // split
        spilt(Command);
        if (comSpilt.size() == 0)
        {
            continue;
        }
        //done
        else if (comSpilt[0] == "help")
        {
            help();
            if (comSpilt.size() == 1) {
                for (int i = 0; i < mp.size(); i++) {
                    cout << "\t" << mpcommend[i] << mp[mpcommend[i]] << endl;
                }
            }
            else if (comSpilt.size() == 2) {
                if (mp[comSpilt[1]] == "") {
                    cout << comSpilt[1] << " This command is not supported by the help utility" << endl;

                }
                else {
                    cout << "\t" << comSpilt[1] << mp[comSpilt[1]] << endl;
                }
            }
            else {
                cout << "HELP [command]\n\tcommand - displays help information on that command.\n";
            }

            comSpilt.clear();
        }
        //done
        else if (comSpilt[0] == "cls")
        {
            cls();
            comSpilt.clear();
        }
        //done
        else if (comSpilt[0] == "cd")
        {
            if (comSpilt.size() == 1) {
                cout << location << endl;
            }
            else if (comSpilt.size() == 2) {
                if (dire.search_directory(comSpilt[1]) != -1) {
                    location += "\\" + comSpilt[1];
                }
                else {
                    cout << "NOT found\n";
                }
            }
            else {
                cout << "The syntax of the command is incorrect.\n";
            }

            comSpilt.clear();
        }
        //done
        else if (comSpilt[0] == "dir")
        {
            cout << "\n";
            cout << "   Directory of " << location << endl;
            cout << "\n";
            int cntfile = 0, cntfolder = 0, cursizefile = 0;
            vector<Directory_Entry> listele = dire.read_directory();
            for (int i = 0; i < listele.size(); i++) {
                if (listele[i].FileAttribute == '1') {
                    cout << "  <DIR>\t\t";
                    for (int j = 0; j < 11; j++) {
                        if (listele[i].FileName[j] != '0') {
                            cout << listele[i].FileName[j];
                        }
                    }
                    cntfolder++;
                }
                else {
                    cout << "  \t\t" << listele[i].FileSize << " ";
                    for (int j = 0; j < 11; j++) {
                        if (listele[i].FileName[j] != '0') {
                            cout << listele[i].FileName[j];
                        }
                    }
                    cursizefile += listele[i].FileSize;
                    cntfile++;
                }
                cout << "\n";
            }
            cout << "\t" << cntfile << " File(s) \t\t" << cursizefile << " bytes" << endl;
            cout << "\t" << cntfolder << " Dir(s) \t\t" << 1024 * 1024 - cursizefile << " free bytes" << endl;
            comSpilt.clear();
        }
        //done
        else if (comSpilt[0] == "quit")
        {
            break;
            comSpilt.clear();
        }
        //done
        else if (comSpilt[0] == "copy")
        {
            if (comSpilt.size() == 3) {
                string a = comSpilt[1];
                string b = comSpilt[2];
                if (a != b && IsPathExist(a)) {
                    xport(a, b);
                }
                else {
                    cout << "Error path\ncopy F:\KHES\asd.txt D:\as.txt";
                }
            }
            else {
                cout << "The syntax of the command is incorrect.\n";
            }
            comSpilt.clear();
        }
        else if (comSpilt[0] == "del")
        {
            map<string, int>mp;
            vector<Directory_Entry>vec = dire.read_directory();
            for (int i = 0; i < vec.size(); i++) {
                string s = "";
                for (int j = 0; j < 11; j++) {
                    if (vec[i].FileName[j] == '0') {
                        break;
                    }
                    else {
                        s += vec[i].FileName[j];
                    }
                }
                if (vec[i].FileAttribute == '1') {
                    mp[s] = 1;
                }
                else {
                    mp[s] = 0;
                }
            }
            if (comSpilt.size() == 1) {
                cout << "The syntax of the command is incorrect.\n";
            }
            else {
                for (int i = 1; i < comSpilt.size(); i++) {
                    string nameoffolder = comSpilt[i];
                    if (dire.search_directory(nameoffolder) != -1 && mp[nameoffolder] == 0) {
                        dire.delete_directory(nameoffolder);
                    }
                    else {
                        cout << "The system cannot find the file specified or it is folder.\n";
                    }
                }
            }
            comSpilt.clear();
        }
        else if (comSpilt[0] == "md")
        {
            if (comSpilt.size() == 1) {
                cout << "The syntax of the command is incorrect.\n";
            }
            else {
                for (int i = 1; i < comSpilt.size(); i++) {
                    string nameoffolder = comSpilt[i];
                    if (dire.search_directory(nameoffolder) == -1) {
                        Directory_Entry elem;
                        for (int i = 0; i < 11; i++) {
                            if (nameoffolder.size() > i) {
                                elem.FileName[i] = nameoffolder[i];
                            }
                            else {
                                elem.FileName[i] = '0';
                            }
                        }
                        for (int i = 0; i < 12; i++) {
                            elem.FileEmpty[i] = '0';
                        }
                        elem.FileAttribute = '1';
                        elem.FileSize = 0;
                        elem.Firstcluster = 0;
                        dire.list.push_back(elem);
                        dire.Write_directory();
                        dire.list.clear();
                    }
                    else {
                        cout << "A subdirectory or file " << nameoffolder << " already exists.\n";
                    }
                }
            }
            comSpilt.clear();
        }
        //done
        else if (comSpilt[0] == "rd")
        {
            map<string, int>mp;
            vector<Directory_Entry>vec = dire.read_directory();
            for (int i = 0; i < vec.size(); i++) {
                string s = "";
                for (int j = 0; j < 11; j++) {
                    if (vec[i].FileName[j] == '0') {
                        break;
                    }
                    else {
                        s += vec[i].FileName[j];
                    }
                }
                if (vec[i].FileAttribute == '1') {
                    mp[s] = 1;
                }
                else {
                    mp[s] = 0;
                }
            }
            if (comSpilt.size() == 1) {
                cout << "The syntax of the command is incorrect.\n";
            }
            else {
                for (int i = 1; i < comSpilt.size(); i++) {
                    string nameoffolder = comSpilt[i];
                    if (dire.search_directory(nameoffolder) != -1 && mp[nameoffolder] == 1) {
                        dire.delete_directory(nameoffolder);
                    }
                    else {
                        cout << "The system cannot find the file specified.\n";
                    }
                }
            }
            comSpilt.clear();
        }
        //done
        else if (comSpilt[0] == "rename")
        {

            if (comSpilt.size() == 3) {
                string old = "", Newa = "", alls;
                for (int i = 0; i < 11; i++) {
                    if (comSpilt[1].size() > i) {
                        old += comSpilt[1][i];
                    }
                    else {
                        old += '0';
                    }
                }
                for (int i = 0; i < 11; i++) {
                    if (comSpilt[2].size() > i) {
                        Newa += comSpilt[2][i];
                    }
                    else {
                        Newa += '0';
                    }
                }
                if (dire.search_directory(comSpilt[1]) != -1) {
                    fstream file("CMD.txt");
                    file >> alls;
                    for (int i = 5120; i < alls.size(); i++) {
                        if (alls[i] == '#') {
                            i += 1023;
                        }
                        else {
                            int ok = 1, j, inds = i;
                            for (j = 0; j < 11; j++, inds++) {
                                if (alls[inds] != old[j]) {
                                    ok = 0;
                                    break;
                                }
                            }
                            if (ok) {
                                for (j = 0; j < 11; j++, i++) {
                                    alls[i] = Newa[j];
                                }
                                break;
                            }
                            else {
                                i += 1023;
                            }
                        }
                    }
                    file.close();
                    ofstream filef("CMD.txt");
                    filef << alls;
                    filef.close();
                }
                else {
                    cout << "NOt found\n";
                }
            }
            comSpilt.clear();
        }
        else if (comSpilt[0] == "type")
        {
            if (comSpilt.size() == 1) {
                cout << "The syntax of the command is incorrect.\n";
            }
            else {
                for (int i = 1; i < comSpilt.size(); i++) {
                    if (dire.search_directory(comSpilt[i]) != -1) {
                        if (comSpilt.size() > 2) {
                            cout << comSpilt[i] << endl << endl;
                        }
                        fstream filea(comSpilt[i]);
                        string vs;
                        while (getline(filea, vs)) {
                            cout << vs << endl;
                        }
                        filea.close();
                    }
                    else {
                        cout << "Not find file\n";
                    }
                }
            }
            comSpilt.clear();
        }
        //done
        else if (comSpilt[0] == "import")
        {
            if (comSpilt.size() == 2) {
                if (IsPathExist(comSpilt[1])) {
                    string str = comSpilt[1], namestr = "";
                    for (int i = str.size() - 1; i >= 0; i--) {
                        if (str[i] == '\\') {
                            break;
                        }
                        else {
                            namestr += str[i];
                        }

                    }
                    reverse(namestr.begin(), namestr.end());
                    if (dire.search_directory(namestr) == -1) {
                        Directory_Entry elem;
                        for (int i = 0; i < 11; i++) {
                            if (namestr.size() > i) {
                                elem.FileName[i] = namestr[i];
                            }
                            else {
                                elem.FileName[i] = '0';
                            }
                        }
                        for (int i = 0; i < 12; i++) {
                            elem.FileEmpty[i] = '0';
                        }
                        elem.FileAttribute = '0';
                        fstream Filetxt(str);
                        string szi;
                        Filetxt >> szi;
                        elem.FileSize = szi.size();
                        Filetxt.close();
                        elem.Firstcluster = 12;
                        dire.list.push_back(elem);
                        dire.Write_directory();
                        dire.list.clear();
                    }
                    else {
                        cout << "A subdirectory or file already exists.\n";
                    }
                }
                else {
                    printf("Not Found Path\n");
                }
            }
            else {
                cout << "The syntax of the command is incorrect.\n";
            }

            comSpilt.clear();
        }
        //done
        else if (comSpilt[0] == "export")
        {
            if (comSpilt.size() == 3) {
                if (IsPathExist(comSpilt[1])) {
                    if (dire.search_directory(comSpilt[2]) != -1) {
                        string a = comSpilt[1];
                        if (a[a.size() - 1] == '\\') {
                            a += comSpilt[2];
                        }
                        else {
                            a += "\\" + comSpilt[2];
                        }

                        string b = "F:\\KHES\\c++\\aeer\\" + comSpilt[2];
                        cout << a << "  " << b << endl;
                        xport(b, a);
                    }
                    else {
                        cout << "A subdirectory or file NOt Exists.\n";
                    }
                }
                else {
                    printf("Not Found Path\n");
                }
            }
            else {
                cout << "The syntax of the command is incorrect.\n";
            }
            comSpilt.clear();
        }
        else
        {
            cout << "'" << comSpilt[0] << "' is not recognized as an internal or external command,operable program or batch file.\n";
            comSpilt.clear();
        }
    }
}
