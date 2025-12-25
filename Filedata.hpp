#include <filesystem>
#include <string>
#include <fstream>
#include <commdlg.h>
#include <span>

namespace File {
    struct data {
        std::vector<std::string> file_list;
        bool FilesScanned = false;
        void getList(std::string* fileList) {
            std::span files{ file_list };
            int iFiles = 0;
            for (std::string data : files) {
                iFiles++;
                fileList[iFiles] = data;
            }
        }
        void OpenFileDialog(std::string& fileName) {
            // common dialog box structure, setting all fields to 0 is important
            OPENFILENAMEA ofn;
            char szFileName[MAX_PATH];

            ZeroMemory(&ofn, sizeof(ofn));
            szFileName[0] = 0;

            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL;
            ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0";
            ofn.lpstrFile = szFileName;
            ofn.nMaxFile = 520;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
            GetOpenFileNameA(&ofn);
            fileName = std::string_view((char*)ofn.lpstrFile);
        }
        void ScanFiles(const std::string_view& directory = "musics", const std::string_view& file_extension = ".wav") {
            static int64_t i64FileCount = 0;
            file_list.clear();
            try {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(
                    directory, std::filesystem::directory_options::skip_permission_denied)) {
                    if (entry.is_regular_file() && entry.path().extension() == file_extension) {
                        file_list.push_back(entry.path().string());
                        std::cout << __TIME__ << " Found " << i64FileCount << " File: " << file_list[i64FileCount] << std::endl;
                        i64FileCount++;
                    }
                }
                FilesScanned = true;
                i64FileCount = 0;
                // Получаем длительности для всех файлов

            }
            catch (const std::filesystem::filesystem_error& e) {
                std::cerr << "Filesystem Error: " << e.what() << std::endl;
                FilesScanned = false;
            }
        }
    };
    auto data_ptr = std::make_unique<data>();
};