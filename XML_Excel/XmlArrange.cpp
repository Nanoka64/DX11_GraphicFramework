

#include <windows.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "XmlArrange.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

// デバック用文字表示
void DebugDraw(std::string str)
{
    std::cout << str << std::endl;
}
void DebugDraw(int num)
{
    std::cout << num << std::endl;
}

XmlArrange::XmlArrange()
{
    mstXmlData = XmlData();
    mstXmlData.projectName = "";
    mstXmlData.xmlClassDatas.clear();
}
XmlArrange::~XmlArrange()
{
    mstXmlData.projectName = "";
    mstXmlData.xmlClassDatas.clear();
}

// xmlファイルを読み込み用に整理する
void XmlArrange::Arrange()
{
    std::string folderPath = "./"; // 取得したいフォルダパス    
    std::string projectName = "";// 取得したプロジェクトファイル名

    // プロジェクト名取得
    projectName = FileRxtension_Search(folderPath, ".vcxproj.filters");
    projectName = projectName.substr(0, projectName.size() - 16);

    int max = 10;
    for (int count = 0; count < max; count++)
    {
        if (FileRxtension_Search(folderPath, ".sln") != "")
        {
            break;
        }
        else if ((count + 1) >= max)
        {
            return;
        }
        folderPath = folderPath + "../";
    }

    std::string xmlFileDir = GetXmlFileDir();
    XmlToData(xmlFileDir + FileRxtension_Search(xmlFileDir, projectName + ".xml"));
    
    // XmlToData(folderPath + "x64/Debug/" + FileRxtension_Search(folderPath + "x64/Debug/", projectName + ".xml"));
    DataToExcelXmlFile(folderPath + "XML_Excel/_Excel.xml");
}

// 指定の拡張子のファイルを取得
std::string XmlArrange::FileRxtension_Search(std::string folderPath, std::string fileRxtension)
{
   try {
       // フォルダが存在するか確認
       if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
           std::cout << "指定されたフォルダは存在しません。" << std::endl;
           return "";
       }

       // ディレクトリを走査
       for (const auto& entry : fs::directory_iterator(folderPath)) {
           if (fs::is_regular_file(entry.status())) { // 通常ファイルのみ
               std::string forTemporaryStorage_File = entry.path().filename().string();
               if ((forTemporaryStorage_File.size() >= fileRxtension.size()) && forTemporaryStorage_File.substr(forTemporaryStorage_File.size() - fileRxtension.size(), fileRxtension.size()) == fileRxtension)
               {
                   return forTemporaryStorage_File;
               }
           }
       }
   }
   catch (const fs::filesystem_error& e) {
       std::cerr << "エラー: " << e.what() << std::endl;
   }

   return "";
}

// 空白削除
void XmlArrange::TrimWhiteSpace(std::string* str)
{
    // 先頭の空白削除
    while ((str->size() > 0) && (str->substr(0, 1) == " "))
    {
        *str = str->substr(1, str->size() - 1);
    }
}
// 空白削除
std::string XmlArrange::TrimWhiteSpace(std::string str)
{
    // 先頭の空白削除
    while ((str.size() > 0) && (str.substr(0, 1) == " "))
    {
        str = str.substr(1, str.size() - 1);
    }

    return str;
}

// 文字列から説明を抽出
std::vector<std::string> XmlArrange::ExtractExplanation_String(std::string line, std::string startString, std::string endString)
{
    // 宣言
    std::vector<std::string> explanations;
    explanations.clear();
    explanations.shrink_to_fit();

    // 文字列個所取得
    size_t startPos = line.find(startString);
    size_t endPos = line.find(endString);

    // 設定
    if (endPos == std::string::npos)
    {
        explanations.reserve(2);
        explanations.push_back(TrimWhiteSpace(line.substr(startPos + startString.size())));
        if (explanations[0] == "")
        {
            explanations.clear();
        }
        else
        {
            explanations.push_back(""); // ダミーで1つ確保
        }
    }
    else
    {
        explanations.push_back(TrimWhiteSpace(line.substr(startPos + startString.size(), endPos - (startPos + startString.size()))));
    }

    return explanations;
}

// ファイルから説明を抽出
std::vector<std::string> XmlArrange::ExtractExplanation_File(std::ifstream& xmlFile, std::string endString)
{
    std::vector<std::string> explanations;
    std::string line;
    while (std::getline(xmlFile, line))
    {
        // 不要なスペースを削除
        TrimWhiteSpace(&line);

        if ((line.size() >= endString.size()) && (line.substr(line.size() - endString.size(), endString.size()) == endString))
        {
            if (line.size() > endString.size())
            {
                explanations.push_back(TrimWhiteSpace(line.substr(0, line.size() - endString.size())));
            }
            break;
        }
        explanations.push_back(TrimWhiteSpace(line));
    }
    return explanations;
}

// 文字列とファイルから説明を抽出
std::vector<std::string> XmlArrange::ExtractExplanation_StringAndFile(std::string line, std::ifstream& xmlFile, std::string startString, std::string endString)
{
    std::vector<std::string> explanations;

    // 文字列から抽出
    explanations = ExtractExplanation_String(line, startString, endString);

    if (explanations.size() != 1)
    {
        if (explanations.size() == 2)
        {
            explanations.pop_back(); // ダミー削除
        }
        // ファイルから抽出
        std::vector<std::string> fileExplanations = ExtractExplanation_File(xmlFile, endString);
        explanations.insert(explanations.end(), fileExplanations.begin(), fileExplanations.end());
    }

    return explanations;
}

// 文字列をコマンドに変換
XmlArrange::XML_COMMAND_TYPE XmlArrange::StringToCommand(std::string commandString)
{
    if (commandString.substr(0, 10) == "<assembly>")
    {
        return XML_COMMAND_TYPE::PROJECT;
    }
    else if (commandString.substr(0, 13) == "<member name=")
    {
        return XML_COMMAND_TYPE::CLASS_FUNCTION;
    }
    else if (commandString.substr(0, 9) == "<summary>")
    {
        return XML_COMMAND_TYPE::EXPLANATION;
    }
    else if (commandString.substr(0, 12) == "<param name=")
    {
        return XML_COMMAND_TYPE::VARIABLE;
    }
    else if (commandString.substr(0, 9) == "<returns>")
    {
        return XML_COMMAND_TYPE::RETURN;
    }
    

    return XML_COMMAND_TYPE::NONE;
}

// クラス名と関数名を登録
void XmlArrange::RegisterClassFunction(std::string line)
{
    // クラス名範囲取得
    int classNameCount = 0;
    while (line.size() > classNameCount)
    {
        if (line.substr(classNameCount, 1) == ".")
        {
            break;
        }
        classNameCount++;
    }

    // TODO: classに属していない関数に反応しないようする
    // class無し
    if (line.size() <= classNameCount)
    {
        return;
    }

    // クラス名登録
    if (mstXmlData.xmlClassDatas.size() == 0)
    {
        XmlClassData xmlClassData = XmlClassData();
        xmlClassData.className = line.substr(16, classNameCount - 16);
        mstXmlData.xmlClassDatas.push_back(xmlClassData);
    }
    else if (line.substr(16, classNameCount - 16) != mstXmlData.xmlClassDatas[mstXmlData.classDataEnd()].className)
    {
        XmlClassData xmlClassData = XmlClassData();
        xmlClassData.className = line.substr(16, classNameCount - 16);
        mstXmlData.xmlClassDatas.push_back(xmlClassData);
    }

    // 関数名範囲取得
    classNameCount += 1;
    int functionNameCount = classNameCount;
    while (line.size() > functionNameCount)
    {
        if (line.substr(functionNameCount, 1) == "(")
        {
            break;
        }
        else if ((line.size() > (functionNameCount + 1)) && (line.substr(functionNameCount, 2) == "\">"))
        {
            break;
        }
        functionNameCount++;
    }

    // 関数名登録
    XmlFunctionData xmlFunctionData = XmlFunctionData();
    xmlFunctionData.functionName = line.substr(classNameCount, functionNameCount - classNameCount);
    mstXmlData.xmlClassDatas[mstXmlData.classDataEnd()].xmlFunctionDatas.push_back(xmlFunctionData);
}

// 関数説明文を登録
void XmlArrange::RegisterFunctionExplanation(std::ifstream& xmlFile, std::string line)
{
    // 関数説明文取得
    mstXmlData.xmlClassDatas[mstXmlData.classDataEnd()].xmlFunctionDatas[mstXmlData.functionDataEnd()].functionExplanation = ExtractExplanation_StringAndFile(line, xmlFile, "<summary>", "</summary>");
}

// 変数説明文を登録
void XmlArrange::RegisterVariableExplanation(std::ifstream& xmlFile, std::string line)
{
    // 変数説明開始位置取得
    int variableExplanationStart = 13;
    while (line.size() > (variableExplanationStart + 1))
    {
        if (line.substr(variableExplanationStart, 1) == ">")
        {
            variableExplanationStart++;
            break;
        }
        variableExplanationStart++;
    }

    // 変数説明文取得
    std::vector<std::string> variableExplanations = ExtractExplanation_StringAndFile(line, xmlFile, line.substr(0, variableExplanationStart), "</param>");
    
    for (int i = 0; i < variableExplanations.size(); i++)
    {
        if (variableExplanations[i].find("</member>") != std::string::npos)
        {
            return;
        }
        if (variableExplanations[i].find("</summary>") != std::string::npos)
        {
            return;
        }
        if (variableExplanations[i].find("</returns>") != std::string::npos)
        {
            return;
        }

        if (variableExplanations[i].find("<param name=") != std::string::npos)
        {
            // 変数説明開始位置取得
            variableExplanationStart = 13;
            while (variableExplanations[i].size() > (variableExplanationStart + 1))
            {
                if (variableExplanations[i].substr(variableExplanationStart, 1) == ">")
                {
                    variableExplanationStart++;
                    break;
                }
                variableExplanationStart++;
            }
            std::string variableExplanation = variableExplanations[i].substr(variableExplanationStart);
            variableExplanations.clear();
            variableExplanations.push_back(variableExplanation);
        }
    }

    mstXmlData.xmlClassDatas[mstXmlData.classDataEnd()].xmlFunctionDatas[mstXmlData.functionDataEnd()].variableExplanations.insert(mstXmlData.xmlClassDatas[mstXmlData.classDataEnd()].xmlFunctionDatas[mstXmlData.functionDataEnd()].variableExplanations.end(), variableExplanations.begin(), variableExplanations.end());
}

// 戻り値説明文を登録
void XmlArrange::RegisterReturnExplanation(std::ifstream& xmlFile, std::string line)
{
    // 戻り値説明終了位置取得
    mstXmlData.xmlClassDatas[mstXmlData.classDataEnd()].xmlFunctionDatas[mstXmlData.functionDataEnd()].returnExplanation = ExtractExplanation_StringAndFile(line, xmlFile, "<returns>", "</returns>");
}

// XMLをデータ化する
void XmlArrange::XmlToData(std::string fileName)
{
    // XML情報取得
    std::ifstream xmlFile;
    xmlFile.open(fileName);

    // ファイルオープン失敗
    if (!xmlFile.is_open())
    {
        std::cerr << "ファイルを開けませんでした: " << fileName << std::endl;
        return;
    }

    // XMLファイル読み込み
    std::string line;
    while (std::getline(xmlFile, line))
    {
        // 不要なスペースを削除
        TrimWhiteSpace(&line);

        // コマンド別処理
        switch(StringToCommand(line))
        {
        case XML_COMMAND_TYPE::PROJECT:// プロジェクト
            xmlFile >> line;
            mstXmlData.projectName = line.substr(1, line.size() - 2);
            break;

        case XML_COMMAND_TYPE::CLASS_FUNCTION:// クラス・関数
            RegisterClassFunction(line);
            break;

        case XML_COMMAND_TYPE::EXPLANATION:// 関数説明
            RegisterFunctionExplanation(xmlFile, line);
            break;

        case XML_COMMAND_TYPE::VARIABLE:// 変数説明
            RegisterVariableExplanation(xmlFile, line);
            break;

        case XML_COMMAND_TYPE::RETURN:// 返り値
            RegisterReturnExplanation(xmlFile, line);
            break;
        }
    }

    xmlFile.close();
}


// データをExcel用のXmlファイルに変換する
void XmlArrange::DataToExcelXmlFile(std::string fileName)
{
    // Excel用XMLファイル作成
    std::ofstream excelXmlFile;
    excelXmlFile.open(fileName);

    // ファイルオープン失敗
    if (!excelXmlFile.is_open())
    {
        std::cerr << "ファイルを開けませんでした: " << fileName << std::endl;
        return;
    }

    // XMLファイルに出力開始
    {
        // 開始タグ
        excelXmlFile << "<?xml version=\"1.0\"?>" << std::endl;
        excelXmlFile << "<doc>" << std::endl;

        // プロジェクト名
        excelXmlFile << "    <assembly>" << std::endl;
        excelXmlFile << ("    " + mstXmlData.projectName) << std::endl;
        excelXmlFile << "    </assembly>" << std::endl;

        // クラス枠
        for (int classNumber = 0; classNumber < mstXmlData.xmlClassDatas.size(); classNumber++)
        {
            // 関数枠
            for (int functionNumber = 0; functionNumber < mstXmlData.xmlClassDatas[classNumber].xmlFunctionDatas.size(); functionNumber++)
            {
                // クラス名
                excelXmlFile << "   <member className=\"" << mstXmlData.xmlClassDatas[classNumber].className << "\">" << std::endl;

                // 関数名
                excelXmlFile << "   <functionName>" << mstXmlData.xmlClassDatas[classNumber].xmlFunctionDatas[functionNumber].functionName << "</functionName>" << std::endl; 

                // 関数説明文
                excelXmlFile << "   <explanations>" << std::endl;
                for (int explanationNumber = 0; explanationNumber < mstXmlData.xmlClassDatas[classNumber].xmlFunctionDatas[functionNumber].functionExplanation.size(); explanationNumber++)
                {
                    excelXmlFile << mstXmlData.xmlClassDatas[classNumber].xmlFunctionDatas[functionNumber].functionExplanation[explanationNumber] << std::endl;
                }
                excelXmlFile << "   </explanations>" << std::endl;

                // 返り値
                if (mstXmlData.xmlClassDatas[classNumber].xmlFunctionDatas[functionNumber].returnExplanation.size() > 0)
                {
                    excelXmlFile << "   <returnExplanation>" << std::endl;
                    for (int returnExplanationNumber = 0; returnExplanationNumber < mstXmlData.xmlClassDatas[classNumber].xmlFunctionDatas[functionNumber].returnExplanation.size(); returnExplanationNumber++)
                    {
                        excelXmlFile << mstXmlData.xmlClassDatas[classNumber].xmlFunctionDatas[functionNumber].returnExplanation[returnExplanationNumber] << std::endl;
                    }
                    excelXmlFile << "   </returnExplanation>" << std::endl;
                }

                // 変数説明
                for (int variableNumber = 0; variableNumber < mstXmlData.xmlClassDatas[classNumber].xmlFunctionDatas[functionNumber].variableExplanations.size(); variableNumber++)
                {
                    excelXmlFile << "   <variable" << variableNumber << ">" << mstXmlData.xmlClassDatas[classNumber].xmlFunctionDatas[functionNumber].variableExplanations[variableNumber] << "</variable" << variableNumber << ">" << std::endl;
                }

                // クラス名閉じる
                excelXmlFile << "   </member>" << std::endl;
            }

        }

        // 終了タグ
        excelXmlFile << "</doc>" << std::endl;
    }

    excelXmlFile.close();
}


// Xmlファイルのあるディレクトリを取得する
std::string XmlArrange::GetXmlFileDir()
{
    char path[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, path, MAX_PATH);
    if (len == 0 || len == MAX_PATH) {
        throw std::runtime_error("Failed to get executable path.");
    }

    std::string fullPath(path);
    size_t pos = fullPath.find_last_of("\\/");
    if (pos != std::string::npos) {
        return fullPath.substr(0, pos + 1); // ディレクトリ部分のみ
    }
    return fullPath; // フォルダ区切りが見つからない場合
}

#undef _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING