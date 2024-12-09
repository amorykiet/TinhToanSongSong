#include <iostream>
#include <fstream>
#include <vector>
#include <chrono> // Thư viện đo thời gian
#include <omp.h> // Thư viện OpenMP
using namespace std;
using namespace chrono;

void encodeMatrixToFile(const vector<vector<int>>& matrix, const string& filename) {
    ofstream outFile(filename);

    if (!outFile) {
        cout << "Cant open file to write" << endl;
        return;
    }

    // Mã hóa và ghi từng phần tử của mảng
    for (const auto& row : matrix) {
        for (int element : row) {
            char encodedChar;
            switch (element % 4) {
            case 0: encodedChar = 'S'; break;
            case 1: encodedChar = 'A'; break;
            case 2: encodedChar = 'M'; break;
            case 3: encodedChar = 'I'; break;
            }
            outFile << encodedChar << " ";
        }
        outFile << endl;
    }

    outFile.close();
}

void encodeMatrixToFileWithParallel(const vector<vector<int>>& matrix, const string& filename, int num_of_threads = 4) {
    ofstream outFile(filename);

    if (!outFile) {
        cout << "Cant open file to write" << endl;
        return;
    }

    int n = matrix.size();
    vector<string> encodedMatrix(n); // Lưu kết quả mã hóa tạm thời

    // Song song hóa việc mã hóa từng hàng của mảng
    #pragma omp parallel num_threads(num_of_threads)
    {
        #pragma omp for
        for (int i = 0; i < n; i++) {
            string row;
            for (int j = 0; j < n; j++) {
                char encodedChar;
                switch (matrix[i][j] % 4) {
                case 0: encodedChar = 'S'; break;
                case 1: encodedChar = 'A'; break;
                case 2: encodedChar = 'M'; break;
                case 3: encodedChar = 'I'; break;
                }
                row += encodedChar;
                row += " ";
            }
            encodedMatrix[i] = row;
        }
    
    }


    // Ghi mảng mã hóa vào tệp
    for (const auto& row : encodedMatrix) {
        outFile << row << endl;
    }

    outFile.close();
}

vector<vector<int>> readMatrixFromFile(const string& filename) {
    ifstream inFile(filename);
    if (!inFile) {
        cout << "Cant open file to read" << endl;
        exit(1);
    }

    int n;
    inFile >> n; // Đọc kích thước mảng

    vector<vector<int>> matrix(n, vector<int>(n));

    // Đọc các phần tử của mảng
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            inFile >> matrix[i][j];
        }
    }

    inFile.close();
    return matrix;
}

int main() {
    // Tên tệp đầu vào và đầu ra
    const string inputFilename = "input.txt";
    const string outputFilename = "output.txt";
    const string outputFilenameOmp = "output_omp.txt";


    // Đọc mảng từ tệp input.txt
    vector<vector<int>> matrix = readMatrixFromFile(inputFilename);


    // Tuần tự
    auto start = high_resolution_clock::now();

    encodeMatrixToFile(matrix, outputFilename);

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    cout << "Execute time (microseconds): " << duration.count() << endl;


    // Song song


    start = high_resolution_clock::now();

    // Điền số lượng luồng vào đây -------------------------->
    encodeMatrixToFileWithParallel(matrix, outputFilenameOmp, 10);

    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start);

    cout << "Execute time in parallel (microseconds): " << duration.count() << endl;



    return 0;
}
