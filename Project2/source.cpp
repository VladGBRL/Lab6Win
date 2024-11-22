#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream> 

#define NUM_PROCESSES 10

void CreatePipesAndProcesses() {
    HANDLE hPipes[NUM_PROCESSES][2];
    STARTUPINFO si[NUM_PROCESSES];
    PROCESS_INFORMATION pi[NUM_PROCESSES];
    TCHAR commandLine[256];

    for (int i = 0; i < NUM_PROCESSES; i++) {
        
        if (!CreatePipe(&hPipes[i][0], &hPipes[i][1], NULL, 0)) {
            std::cout << "Error creating pipe " << i + 1 << " (Error code: " << GetLastError() << ").\n";
            return;
        }

        SetHandleInformation(hPipes[i][0], HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
        SetHandleInformation(hPipes[i][1], HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);

        ZeroMemory(&si[i], sizeof(STARTUPINFO));
        si[i].cb = sizeof(STARTUPINFO);
        si[i].dwFlags |= STARTF_USESTDHANDLES;
        si[i].hStdInput = (i == 0) ? hPipes[i][0] : hPipes[i - 1][0];
        si[i].hStdOutput = hPipes[i][1];

        ZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));

        _stprintf_s(commandLine, _T("prime_checker.exe %d %d"), i * 1000, (i + 1) * 1000);

        std::cout << "Creating process " << i + 1 << " with command: " << commandLine << "\n";
        if (!CreateProcess(
            NULL,
            commandLine,
            NULL,
            NULL,
            TRUE,
            0,
            NULL,
            NULL,
            &si[i],
            &pi[i])) {
            std::cout << "Error creating process " << i + 1 << " (Error code: " << GetLastError() << ").\n";
            return;
        }

        CloseHandle(hPipes[i][1]);
    }

    DWORD bytesRead;
    char buffer[1024];
    std::string finalOutput = ""; 

    for (int i = 1; i < NUM_PROCESSES; i++) {
        std::cout << "Reading from pipe " << i + 1 << "...\n";
        if (!ReadFile(hPipes[i][0], buffer, sizeof(buffer), &bytesRead, NULL)) {
            std::cout << "Error reading from pipe " << i + 1 << " (Error code: " << GetLastError() << ").\n";
            return;
        }
        buffer[bytesRead] = '\0'; 
        finalOutput += buffer; 
        std::cout << "Output from process " << i + 1 << ": " << buffer << "\n";
    }

    for (int i = 0; i < NUM_PROCESSES; i++) {
        WaitForSingleObject(pi[i].hProcess, INFINITE);
        CloseHandle(pi[i].hProcess);
        CloseHandle(pi[i].hThread);
        CloseHandle(hPipes[i][0]);
    }

    std::cout << "\nFinal output after all processes completed:\n" << finalOutput << "\n";
}

int _tmain(int argc, TCHAR* argv[]) {
    CreatePipesAndProcesses();
    return 0;
}
