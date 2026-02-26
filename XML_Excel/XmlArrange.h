#pragma once
#include <fstream>
#include <string>
#include <vector>

// xmlファイルを整理するクラス
class XmlArrange
{
private:
    // コマンド種類
    enum class XML_COMMAND_TYPE
    {
        NONE = -1,    // 無効
        PROJECT = 0,   // プロジェクト名
        CLASS_FUNCTION,// クラス&関数名
        EXPLANATION,   // 関数説明文
        VARIABLE,      // 変数説明文
        RETURN,        // 戻り値説明文
    };

    // 関数データ構造体
    struct XmlFunctionData
    {
        std::string functionName; // 関数名
        std::vector<std::string> functionExplanation; // 関数説明文
        std::vector<std::string> variableExplanations; // 変数説明文リスト
        std::vector<std::string> returnExplanation; // 戻り値説明文
    };

    // クラスデータ構造体
    struct XmlClassData
    {
        std::string className; // クラス名

        std::vector<XmlFunctionData> xmlFunctionDatas; // 関数データ構造体
    };

    // XMLデータ構造体
    struct XmlData
    {
        std::string projectName; // プロジェクト名

        std::vector<XmlClassData> xmlClassDatas; // クラスデータ構造体

        // クラスデータ数取得
        size_t classDataEnd() { return xmlClassDatas.size() - 1; }
        // 関数データ数取得
        size_t functionDataEnd() { return xmlClassDatas[classDataEnd()].xmlFunctionDatas.size() - 1; }
        // 関数説明文数取得
        size_t functionExplanationEnd() { return xmlClassDatas[classDataEnd()].xmlFunctionDatas[functionDataEnd()].functionExplanation.size() - 1; }
        // 変数説明文数取得
        size_t variableExplanationEnd() { return xmlClassDatas[classDataEnd()].xmlFunctionDatas[functionDataEnd()].variableExplanations.size() - 1; }
    };

    // XMLデータ
    XmlData mstXmlData;

public:
    XmlArrange();
    ~XmlArrange();

    // xmlファイルを読み込み用に整理する
    void Arrange();
private:
    // 指定の拡張子のファイルを取得
    std::string FileRxtension_Search(std::string folderPath, std::string fileRxtension);

    // 空白削除
    void TrimWhiteSpace(std::string* str);
    std::string TrimWhiteSpace(std::string str);

    // 文字列から説明を抽出
    std::vector<std::string> ExtractExplanation_String(std::string line, std::string startString, std::string endString);

    // ファイルから説明を抽出
    std::vector<std::string> ExtractExplanation_File(std::ifstream& xmlFile, std::string endString);

    // 文字列とファイルから説明を抽出
    std::vector<std::string> ExtractExplanation_StringAndFile(std::string line, std::ifstream& xmlFile, std::string startString, std::string endString);

    // 文字列をコマンドに変換
    XML_COMMAND_TYPE StringToCommand(std::string commandString);

    // クラス名と関数名を登録
    void RegisterClassFunction(std::string line);

    // 関数説明文を登録
    void RegisterFunctionExplanation(std::ifstream& xmlFile, std::string line);

    // 変数説明文を登録
    void RegisterVariableExplanation(std::ifstream& xmlFile, std::string line);

    // 戻り値説明文を登録
    void RegisterReturnExplanation(std::ifstream& xmlFile, std::string line);

    // XMLをデータ化する
    void XmlToData(std::string fileName);

    // データをExcel用のXmlファイルに変換する
    void DataToExcelXmlFile(std::string fileName);

    // Xmlファイルのあるディレクトリを取得する
    std::string GetXmlFileDir();
};